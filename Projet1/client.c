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

	int sfd = create_socket(NULL, 0,&real_addr, port);
	int fd;
	if(file != NULL)
		fd = open((const char *)file, O_RDONLY);
	else
		fd = STDIN_FILENO;

	pkt_t* pkt_send = pkt_new();
	if(pkt_send == NULL){
     fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_send);
      return;
    }
	pkt_set_window(pkt_send,1);
	pkt_t* pkt_ack = pkt_new();
	if(pkt_ack == NULL){
     fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_ack);
		pkt_del(pkt_send);
      return;
    }
	int bufferEmpty = 0;
	int endFile = 0;
	int max_length = 0;
	struct timeval tv;
  	tv.tv_sec = 5;
  	tv.tv_usec = 0;

	char buffer_read[MAX_PAYLOAD_SIZE];
	char packet_encoded[1024];
	fd_set read_set;
	while(endFile == 0)
	{
		FD_ZERO(&read_set);
		FD_SET(fd, &read_set);
		FD_SET(sfd, &read_set);

		max_length = (fd > sfd) ? fd+1 : sfd+1;
		//On considère que la variable peut être modifiée après l'appel de la fonction, ont utilise donc une autre structure.
		struct timeval newtv = tv;
		select(max_length, &read_set,NULL, NULL, &newtv);

		if(FD_ISSET(fd, &read_set)) {
			int length = read(fd,(void *)buffer_read, MAX_PAYLOAD_SIZE);

			if(length == 0){
				//SI TAILLE 0 => Fin de fichier
				endFile =1;
			}
			else if(length < 0){
				//Si TAILLE -1 => Erreur
				perror("Error reading file");
				pkt_del(pkt_send);
				pkt_del(pkt_ack);
				return;
	   		}
			else if(length > 0){
				pkt_set_payload(pkt_send,(const char*)buffer_read,(size_t) length);
				if(pkt_encode(pkt_send,packet_encoded,(size_t *)&length)!= PKT_OK)
				{
					fprintf(stderr, "An occur failed while creating a data packet.");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}
				if(write(sfd, packet_encoded,length) < 0)
				{
					fprintf(stderr, "An occur failed while sending a packet.");
					pkt_del(pkt_send);
					pkt_del(pkt_ack);
      				return;
				}
			}
		}
		else if( FD_ISSET(sfd, &read_set)){ //on a reçut un aquittement ou un nack
			printf("ALOOOO\n");
			int length = read(sfd, (void *)packet_encoded, 1024);
			printf("%s\n", packet_encoded);
			if(length> 0 && pkt_decode((const char *)packet_encoded,(size_t )length,pkt_ack) == PKT_OK){
				fprintf(stdout,"BIEN RECU\n");
			}
		}
		//TEMPORAIREMENT POUR ENVOYER QU'UN PACKET
		//endFile = 1;
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
