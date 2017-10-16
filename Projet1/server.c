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
