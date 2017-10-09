#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


/* Block the caller until a message is received on sfd,
 * and connect the socket to the source addresse of the received message.
 * @sfd: a file descriptor to a bound socket but not yet connected
 * @return: 0 in case of success, -1 otherwise
 * @POST: This call is idempotent, it does not 'consume' the data of the message,
 * and could be repeated several times blocking only at the first call.
 */
int wait_for_client(int sfd){
  struct sockaddr_in6 their_addr;
  socklen_t sin_size = sizeof(struct sockaddr_in6);
  memset(&their_addr, 0, sin_size);
  char buf[1024];
  int err;
  err = recvfrom(sfd, buf, 1024, 0, (struct sockaddr *)&their_addr, &sin_size);
  if(err == -1){
    perror("erreur lors de la reception du message : ");
    return -1;
  }
  err = connect(sfd, (struct sockaddr *)&their_addr, sin_size);
  if(err == -1){
    perror("erreur lors de la connextion : ");
  }
  return err;
}
