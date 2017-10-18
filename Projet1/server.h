#ifndef __SERVER_H_
#define __SERVER_H_

void receive_data(char* hostname, int port, char* file);

int send_ack(pkt_t *pkt_ack, int seqnum, int sfd, int ack);
void add_buffer(int index, int seq_rcv, int seq_exp,char ** buffer_payload, size_t *buffer_len, pkt_t * pkt_rcv, int window);
#endif
