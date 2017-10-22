#include "socket.h"
#include "client.h"
#include "packet_interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* * sockaddr_in6 */
#include <netdb.h>
#include <math.h>

void adapt_ack(int *small_seq,int seq, int *small_index,int window, uint32_t timestamp, struct timeval tv, int  nbre_tv,int *buffer_size)
{

	 int i;
	printf("Dans le adapt_ack\n");
	printf("seq: %d\n",seq);
	printf("small_seq: %d\n",*small_seq);
	printf("%d",*buffer_size);
	 for(i=0; *small_seq != seq; i++){
		
	    *small_index = (*small_index+1)%window; //seq est le prochain element attendu
	   	*small_seq = (*small_seq+1)%256;
		 *buffer_size = (*buffer_size)- 1;
		if(*small_seq == seq){//adapte le rtt et le temps maximal d'attente car le receiver a reçut seql
		struct timeval tv1;
			gettimeofday(&tv1, NULL);
			adapt_rtt(tv1, timestamp, &tv, nbre_tv);
		 }
	// printf("small_seq: %d\n",*small_seq);
	 }
}

void adapt_rtt(struct timeval tv1, uint32_t timestamp, struct timeval *tv, int nbre_tv){
	double rtt = (100000 * (tv->tv_sec) + tv->tv_usec)/1.5;
	int dif = 100000 * tv1.tv_sec+ tv1.tv_usec - timestamp;
	if(nbre_tv == 10)
		rtt = rtt - rtt / 10 + dif;
	else{
		nbre_tv++;
		rtt = (rtt * (nbre_tv-1) + dif) / nbre_tv;
	}
	rtt = rtt * 1.5;
	tv->tv_sec = rtt / 100000;
	tv->tv_usec = (int) rtt % 100000;

}


void send_data(char *hostname, int port, char* file){
	struct sockaddr_in6 real_addr;
	memset(&real_addr, 0, sizeof(real_addr));
	const char* ret_real = real_address(hostname, &real_addr);
	if (ret_real != NULL){
    	fprintf(stderr, "Address '%s' is not recognized.\n", hostname);
   		 return;
	}
	//Création du socket(qui est un file directory) à partir de l'adresse IPV6 récupérée avec la méthode real_adress
	int sfd = create_socket(NULL, 0,&real_addr, port);
	int fd;
	//Ouvre le fichier si l'utilisateur en a fournit un en argument.
	if(file != NULL)
		fd = open((const char *)file, O_RDONLY);
	else
		fd = STDIN_FILENO;

	//Création d'une structure packet qui sera utilisée pour envoyer les informations
	pkt_t* pkt_send = pkt_new();
	if(pkt_send == NULL){
     fprintf(stderr, "pkt_send : An occur failed while creating a data packet.\n");
		pkt_del(pkt_send);
      return;
    }
	//Initialisation la window du packet à 1, la window va être s'adaptée par la suite avec la window du reciever
	pkt_set_window(pkt_send,1);
	//Création d'une structure packet qui contiendra les ACK et NACK
	pkt_t* pkt_ack = pkt_new();
	if(pkt_ack == NULL){
     fprintf(stderr, "pkt_ack : An occur failed while creating a data packet.\n");
		pkt_del(pkt_ack);
		pkt_del(pkt_send);
      return;
    }

	int endFile = 0; //on regarde si fin du fichier ou non
	int max_length = 0; //Taille maximal du file directory, utilisé pour l'appel du select
	struct timeval tv;
  	tv.tv_sec = 5; //On met 5 seconde d'intervalle
  	tv.tv_usec = 0;
	int buffer_size = 0;
	int actual_index = 0; //index actuel du buffer1
  	int small_index = 0; //index du plus petit element dans le buffer1
  	int small_seq = 0; // valeur du segment le plus petit dans le buffer
 	int buffer_empty = 0; //indique si le buffer est vide (0, 1 sinon)
  	int seq_exp = 0; //prochain numero de sequence a envoyer
  	char *buffer_packet[MAX_WINDOW_SIZE]; //Permet de stocker les payload recu
	int buffer_len[MAX_WINDOW_SIZE]; //Permet de stocker la taille des payload recu
	memset(buffer_len,-1,MAX_WINDOW_SIZE);
	int window = 1;
	int nbre_tv = 0;
	
	char buffer_read[MAX_PAYLOAD_SIZE] ; //Buffer utilisé pour stocker le payload
	char packet_encoded[1024]; //buffer utilisé pour lire les données encodées
	fd_set read_set;
	printf("AVANT LE WHILE\n");
	printf("actual_index : %d\n",actual_index);
	printf("small_index: %d\n",small_index);
	printf("small_seq: %d\n",small_seq);
	printf("seq_exp: %d\n",seq_exp);
	printf("buffer_empty: %d\n",buffer_empty);
	printf("buffer_size: %d\n",buffer_size);
	while(buffer_empty==1 || endFile == 0)
	{

		FD_ZERO(&read_set);
		FD_SET(sfd, &read_set);
		if(endFile == 0 && (buffer_empty == 0 || actual_index!=small_index) ) {
			FD_SET(fd, &read_set);//prepare le premier flux (fichier) inutile si on est deja arrives a la fin ou si le buffer est rempli
       	}


		//calcul de la taille max entre les deux file directory
		max_length = (fd > sfd) ? fd+1 : sfd+1;
		//On considère que la variable peut être modifiée après l'appel de la fonction,
		//on utilise donc une autre structure.
		struct timeval newtv = tv;

		//Permet de gerer plusieurs entrées et sorties à la fois
		select(max_length, &read_set,NULL, NULL, &newtv);

		if(FD_ISSET(fd, &read_set)) {

		
			//On lit dans le fichier, et on stocke les données dans le buffer read
			//La taille maximul correspond à la taille du payload
			int length = read(fd,(void *)buffer_read, MAX_PAYLOAD_SIZE);

			//SI TAILLE 0 => Fin de fichier
			if(length == 0){

				endFile =1;
			}
			//Si TAILLE -1 => Erreur
			else if(length < 0){
				perror("Error reading file");
				pkt_del(pkt_send);
				pkt_del(pkt_ack);
				return;
	   		}
			//Si taille > 1 => ok, en peut remplir de données le packet
			else if(length > 0){
				printf("AVANT LE buffer_empty\n");
				printf("actual_index : %d\n",actual_index);
				printf("small_index: %d\n",small_index);
				printf("small_seq: %d\n",small_seq);
				printf("seq_exp: %d\n",seq_exp);
				printf("buffer_empty: %d\n",buffer_empty);
				printf("buffer_size: %d\n",buffer_size);
				if(buffer_empty == 0){
					printf("PASSE DANS LE IS EMPTY\n");
					small_index = actual_index;
					small_seq = seq_exp;
					buffer_empty = 1;
				}
				printf("Après le buffer_empty\n");
				printf("actual_index : %d\n",actual_index);
				printf("small_index: %d\n",small_index);
				printf("small_seq: %d\n",small_seq);
				printf("seq_exp: %d\n",seq_exp);
				printf("buffer_empty: %d\n",buffer_empty);
				printf("buffer_size: %d\n",buffer_size);
				//On place, dans notre packet, les données lues dans le fichier
				pkt_set_payload(pkt_send,(const char*)buffer_read,(size_t) length);

				pkt_set_seqnum(pkt_send,seq_exp);
				seq_exp = (seq_exp + 1)%256;


				buffer_packet[actual_index] = malloc (pkt_get_length(pkt_send));
				buffer_len[actual_index] = 1024;
				buffer_size++;
				//On encode le packet pour l'envoyer après
				if(pkt_encode(pkt_send,buffer_packet[actual_index],(size_t *)&buffer_len[actual_index])!= PKT_OK)
				{
					//Si le message de retour est différent de PKT_OK => Il y a eu un probème
					fprintf(stderr, "pkt_encode ici : An occur failed while creating a data packet.\n");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}

				//Envoiela packet au reciever
				if(write(sfd,buffer_packet[actual_index],buffer_len[actual_index]) < 0)
				{
					fprintf(stderr, "write : An occur failed while sending a packet.\n");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}
				else
				{
					printf("[[[ SEGMENT NUM %d SENT]]]\n",pkt_get_seqnum(pkt_send));
				}
				actual_index = (actual_index + 1)%window;
				printf("A la fin du send packet\n");
				printf("actual_index : %d\n",actual_index);
				printf("small_index: %d\n",small_index);
				printf("small_seq: %d\n",small_seq);
				printf("seq_exp: %d\n",seq_exp);
				printf("buffer_empty: %d\n",buffer_empty);
				printf("buffer_size: %d\n",buffer_size);
			}
			//close(sfd);
			//fd = STDIN_FILENO;

		}
		//Cas ou on a reçu un ACK/NACK
		if( FD_ISSET(sfd, &read_set)){
			//Lecture du packet encodé recu et le place dans la variable packet_encoded
			int length = read(sfd, (void *)packet_encoded, 1024);
			//Decodage du  packet reçu dans pkt_ack
			if(length> 0 && pkt_decode((const char *)packet_encoded,(size_t )length,pkt_ack) != PKT_OK){
				fprintf(stdout,"[[[ ERROR RECIEVING ACK NUM %d ]]]\n",pkt_get_seqnum(pkt_ack));
			}
			else
			{
				int i = pkt_get_type(pkt_ack);
				if(i == 1)
				{
					fprintf(stdout,"[[[ SEGMENT NUM %d RECIEVED ]]]\n",pkt_get_seqnum(pkt_ack));
				}
				else if(i == 2)
				{
					fprintf(stdout,"[[[ ACK NUM %d RECIEVED ]]]\n",pkt_get_seqnum(pkt_ack));
				}
				else if(i == 3)
				{
					fprintf(stdout,"[[[ NACK %d RECIEVED ]]]\n",pkt_get_seqnum(pkt_ack));
				}
				else{
					printf("[[[ ERROR ON SEGNUM RECIEVED ]]]\n");
				}
				int window1 = pkt_get_window(pkt_ack);
				uint8_t seq = pkt_get_seqnum(pkt_ack);

				printf("Réception d'un ACK\n");
				printf("Réception d'un ACK\n");
				printf("actual_index : %d\n",actual_index);
				printf("small_index: %d\n",small_index);
				printf("small_seq: %d\n",small_seq);
				printf("seq_exp: %d\n",seq_exp);
				printf("buffer_empty: %d\n",buffer_empty);
				printf("buffer_size: %d\n",buffer_size);
				if(window1 > window && small_index == actual_index){ //le buffer etait rempli
					printf("Passe dans le if window\n");
					window = window1;
					actual_index ++;
				
				}
				printf("Après le if window\n");
				printf("actual_index : %d\n",actual_index);
				printf("small_index: %d\n",small_index);
				printf("small_seq: %d\n",small_seq);
				printf("seq_exp: %d\n",seq_exp);
				printf("buffer_empty: %d\n",buffer_empty);
				printf("buffer_size: %d\n",buffer_size);
				pkt_set_window(pkt_send, window); //changera pas si window invalide

				if(pkt_get_type(pkt_ack)==PTYPE_ACK){

			 		 adapt_ack(&small_seq, seq, &small_index,window, pkt_get_timestamp(pkt_ack), tv, nbre_tv,&buffer_size);
					printf("Après le adapt_ack\n");
					printf("actual_index : %d\n",actual_index);
					printf("small_index: %d\n",small_index);
					printf("small_seq: %d\n",small_seq);
					printf("seq_exp: %d\n",seq_exp);
					printf("buffer_empty: %d\n",buffer_empty);
					printf("buffer_size: %d\n",buffer_size);
					if(endFile == 1 && small_seq == (seq_exp - 1)) {
						buffer_empty = 0;
					}
					
				}
			}
		}
	}
	if(send(sfd, (const void *)EOF, 0,0) < 0)
	{
		fprintf(stderr, "send EOF : An occur failed while sending EOF.\n");
		pkt_del(pkt_send);
		pkt_del(pkt_ack);
		return;
	}
	close(sfd);
	close(fd);
	pkt_del(pkt_ack);
  	pkt_del(pkt_send);
}
