#ifndef __CLIENT_H_
#define __CLIENT_H_


void send_data(char *hostname, int port, char* file);
void adapt_ack(int *small_seq,int seq, int *small_index,int window);
#endif
