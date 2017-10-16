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

void receive_data(char* hostname, int port, char* file){
	//A DELETE
	printf("%s",file);
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
}


int send_ack(pkt_t *pkt_ack, int seqnum, int sfd, int ack){

  pkt_status_code return_status;

  return_status = pkt_set_seqnum(pkt_ack, seqnum);
  if(return_status != PKT_OK){
    perror("Creation de l'acknowledge : ");
    return -1;
  }
  if(ack == PTYPE_ACK)
    return_status = pkt_set_type(pkt_ack, PTYPE_ACK);
  else
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
