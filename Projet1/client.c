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
	int fd = open((const char *)file, O_RDONLY);
	pkt_t* pkt_send = pkt_new();
	if(pkt_send == NULL){
      fprintf(stderr, "An occur failed with the creation of the packet.");
		pkt_del(pkt_send);
      return;
    }	
	pkt_set_window(pkt_send,1);
	
	int bufferEmpty = 0;
	int endFile = 0;
	int max_length = 0;
	struct timeval tv;
  	tv.tv_sec = 5;
  	tv.tv_usec = 0;
	
	char buffer_read[MAX_PAYLOAD_SIZE]; 
	
	fd_set read_set;
	while(endFile == 0 || bufferEmpty==1)
	{
		FD_ZERO(&read_set);
		FD_SET(fd, &read_set);
		
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
				return;
	   		}	
			else if(length > 0){
				/*if(isEmpty == 0) {
					indexl1 = index1;
					seql = seqnum;
					isEmpty = 1;
				}

				if(encode(env, &seqnum, buf1, length, & (buffer1[index1]))!=0){//, ntv) != 0) {	
					pkt_del(env);
					pkt_del(ack);
					return;
				}
				send(sfd, (void*)(buffer1[index1].buff), buffer1[index1].len,0);

				index1 = (index1 + 1)%window;*/
			}
		}
	}
	
	printf("%d %d",endFile,bufferEmpty);
}