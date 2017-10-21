#ifndef __CLIENT_H_
#define __CLIENT_H_


void send_data(char *hostname, int port, char* file);
void adapt_ack(int *small_seq,int seq, int *small_index,int window, uint32_t timestamp, struct timeval tv, int  nbre_tv);
void adapt_rtt(struct timeval tv1, uint32_t timestamp, struct timeval *tv, int nbre_tv);
#endif
