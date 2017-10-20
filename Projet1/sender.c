#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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
#include "packet_interface.h"
#include "client.h"

int
main (int argc, char **argv)
{
 char *fichier = NULL; 
  char *hostname = NULL; 
  int port;
  int c; //Permet la gestion des arguments
  int index;
  opterr = 0;

  while ((c = getopt (argc, argv, "f:")) != -1)
    switch (c)
      {
      case 'f':
        fichier = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  index = optind;
  if(index+2 != argc){
    fprintf(stderr, "Arguments invalides\n");
    return 1;
  }
  //On récupère l'hote et le port du reviever fourni par l'utilisateur
  hostname = argv[index];
  port = atoi(argv[index+1]);
  //Lorsqu'on le port et l'adresse, on appelle send_data pour établir créer un socket,la connection, et envoyer les packets
  send_data(hostname, port, fichier);
  return EXIT_SUCCESS;
	
}
