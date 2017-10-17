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

int send_ack(pkt_t *pkt_ack, int seqnum, int sfd, int ack){

  pkt_status_code return_status;

  return_status = pkt_set_seqnum(pkt_ack, seqnum);
  if(return_status != PKT_OK){
    perror("Creation de l'acknowledge : ");
    return -1;
  }

  if(ack == PTYPE_ACK)
    return_status = pkt_set_type(pkt_ack, PTYPE_ACK);
  else if(ack == PTYPE_NACK)
    return_status = pkt_set_type(pkt_ack, PTYPE_NACK);
  if(return_status != PKT_OK){
    perror("Creation de l'acknowledge : ");
    return -1;
  }


  return_status = pkt_set_payload(pkt_ack, NULL, 0);
  if(return_status != PKT_OK){
    perror("Creation de l'acknowledge : ");
    return -1;
  }

  char buf[12];
  size_t buf_len = 12;

  return_status = pkt_encode(pkt_ack, buf, &buf_len);
  if(return_status != PKT_OK){
    perror("Encodage de l'acknowledge");
    return -1;
  }
  send(sfd, buf, buf_len, 0);


  return 0;
}


void receive_data(char* hostname, int port, char* file){

	//On récupere la real address
	struct sockaddr_in6 real_addr;
	memset(&real_addr, 0, sizeof(real_addr));
	const char* ret_real = real_address(hostname, &real_addr);
	if (ret_real != NULL){
		fprintf(stderr, "Address '%s' is not recognized.", hostname);
		return;
	}
	//On crée le socket adéquat
	int sfd = create_socket(&real_addr, port, NULL, 0);

  	if(sfd < 0)
    	return;
  	//On attend une connection
  	int wait = wait_for_client(sfd);
  	if(wait < 0)
    	return;


	int fd;
  if(file != NULL)
    fd = open((const char *)file, O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR );
  else
    fd = STDOUT_FILENO;

	struct timeval tv;
  tv.tv_sec = 5;
	tv.tv_usec = 0;
	int endFile = 0;
	int max_length = 0;

	pkt_t* pkt_rcv = pkt_new();
	if(pkt_rcv == NULL){
     	fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_rcv);
      	return;
    }
	pkt_t* pkt_ack = pkt_new();
	if(pkt_ack == NULL){
    	fprintf(stderr, "An occur failed while creating a data packet.");
		pkt_del(pkt_ack);
		pkt_del(pkt_rcv);
      return;
    }
	char packet_encoded[1024];
	fd_set read_set;
	while(endFile == 0){


		FD_ZERO(&read_set);
		FD_SET(sfd, &read_set);

		max_length = (fd > sfd) ? fd+1 : sfd+1;
		struct timeval newtv = tv;
		select(max_length, &read_set,NULL, NULL, &newtv);

		if(FD_ISSET(sfd, &read_set )) { //on lit les donnees du client
            int length = read(sfd,(void *)packet_encoded, 1024);

			if(length == 0){
				endFile = 1;
				//Cas ou on recoit send(sfd, (const void *)EOF, 0,0);??
				//On envoie le dernier packet recu ?
			}
			else if(length < 0){
				//On envoie le dernier packet recu ?
				pkt_del(pkt_rcv);
				pkt_del(pkt_ack);
				return;
	   		}
			else if(length > 0){
				if(pkt_decode((const char*)packet_encoded,(size_t)length,pkt_rcv) == PKT_OK && pkt_get_type(pkt_rcv) == PTYPE_DATA)
				{

          				if(write(fd, (void *)pkt_get_payload(pkt_rcv), pkt_get_length(pkt_rcv)) < 0)
						{
							fprintf(stderr,"ERROR SENDING PACKET");
						}
					//int seqnum = pkt_get_seqnum(pkt_rcv);
					//CAS OU ON RECOIS SEULEMENT UN HEADER
					if(length != 4)
					{
            /*
						if (send_ack(pkt_ack, seqnum, sfd ,PTYPE_ACK) !=0){
							pkt_del(pkt_rcv);
							pkt_del(pkt_ack);

							//Pas sur du return..
							//return;
						}*/
					}
					else
					{

						//send_ack(pkt_ack, seqnum, sfd,PTYPE_NACK);
						//Pas de check d'erreur de message car peut etre seqnum corrompu ?
						//Du coup, méthode env_ack bonne ?

					}
				}
			}
		}
	}


	close(sfd);
	//close(fd) ??
	pkt_del(pkt_ack);
  	pkt_del(pkt_rcv);
}
