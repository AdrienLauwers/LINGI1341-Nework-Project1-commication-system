#include "socket.h"
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

void send_data(char *hostname, int port, char* file){
	struct sockaddr_in6 real_addr;
	memset(&real_addr, 0, sizeof(real_addr));
	const char* ret_real = real_address(hostname, &real_addr);
	if (ret_real != NULL){
    	fprintf(stderr, "Address '%s' is not recognized.", hostname);
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
     fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_send);
      return;
    }
	//Initialisation la window du packet à 1, la window va être s'adaptée par la suite avec la window du reciever
	pkt_set_window(pkt_send,1);
	//Création d'une structure packet qui contiendra les ACK et NACK
	pkt_t* pkt_ack = pkt_new(); 
	if(pkt_ack == NULL){
     fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_ack);
		pkt_del(pkt_send);
      return;
    }
	int bufferEmpty = 0;
	int endFile = 0; //on regarde si fin du fichier ou non
	int max_length = 0; //Taille maximal du file directory, utilisé pour l'appel du select
	struct timeval tv;
  	tv.tv_sec = 5; //On met 5 seconde d'intervalle
  	tv.tv_usec = 0;

	char buffer_read[MAX_PAYLOAD_SIZE]; //Buffer utilisé pour stocker le payload
	char packet_encoded[1024]; //buffer utilisé pour lire les données encodées
	fd_set read_set;
	while(endFile == 0)
	{
		FD_ZERO(&read_set);
		FD_SET(fd, &read_set);
		FD_SET(sfd, &read_set);
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
				
				//pkt_set_tr(pkt_send,0);
				//On place, dans notre packet, les données lues dans le fichier 
				pkt_set_payload(pkt_send,(const char*)buffer_read,(size_t) length);
				//On encode le packet pour l'envoyer après
				if(pkt_encode(pkt_send,packet_encoded,(size_t *)&length)!= PKT_OK)
				{
					//Si le message de retour est différent de PKT_OK => Il y a eu un probème
					fprintf(stderr, "An occur failed while creating a data packet.");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}
				//Envoiela packet au reciever
				if(write(sfd, packet_encoded,length) < 0)
				{
					fprintf(stderr, "An occur failed while sending a packet.");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}
				else
				{
					printf("[[[ SEGMENT NUM %d SENT]]]\n",pkt_get_seqnum(pkt_send));
				}
			}
		}
		//Cas ou on a reçu un ACK/NACK
		else if( FD_ISSET(sfd, &read_set)){ 
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

			}
		}
		
		endFile = feof(stdin);
	}

	/*
	send(sfd, (const void *)EOF, 0,0);
	close(sfd);
	//close(fd) ??
	pkt_del(pkt_ack);
  	pkt_del(pkt_send);	*/
	printf("%d %d\n",endFile,bufferEmpty);
}
