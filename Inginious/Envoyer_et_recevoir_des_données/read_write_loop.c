
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
#include <sys/time.h>

#define STDIN 0
#define STDOUT 1
/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(const int sfd){
  struct timeval tv;
  fd_set readfds;
  tv.tv_sec = 6;
  tv.tv_usec = 0;

  char buf1[1024];
  char buf2[1024];

  int end = 0;
  int err;
  while(end == 0){

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);
    FD_SET(sfd, &readfds);

    select(sfd+1, &readfds, NULL, NULL, &tv);

    if(FD_ISSET(STDIN, &readfds)){
      err = read(STDIN, (void *)buf1, 1024);
      if(err > 0){
        send(sfd, (void *)buf1, err, 0);
      }
    }
    if(FD_ISSET(sfd, &readfds)){
      err = read(sfd, (void *)buf2, 1024);
      
      if(err > 0){
        write(STDOUT, (void *)buf2, err);
      }
    }
    end = feof(stdin);
  }
}
