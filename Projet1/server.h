#ifndef __SERVER_H_
#define __SERVER_H_

void receive_data(char* hostname, int port, char* file);

int send_ack(pkt_t *pkt_ack, int seqnum, int sfd, int ack);

#endif
