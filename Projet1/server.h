#ifndef __SERVER_H_
#define __SERVER_H_

/*
 * Recois les données venant du sender et envoit un aqcquittement
 * un non-acquittement en fonction du paquet recu
 * La fonction :
 * - Recois des packets, les décodes et envoie un (non-)acquittement
 * - Ecris le contenu des packets soit dans un fichier soit sur le flux stdout
 *
 * @hostname: L'addresse à laquelle on se connecte
 * @port: Le port auquel on se connecte
 * @file: Un nom de fichier contenant les données ou NULL si non spécifié
 *
 * @post: Toutes les données recues sont écrites dans le fichier ou sur le stdout
 */
void receive_data(char* hostname, int port, char* file);

/*
 * Renvoit au sender un acquittement ou un non-acquittement en fonction du packet recu
 * La fonction :
 * - Encode un nouveau packet, l'envoie au sender
 *
 * @pkt_ack: Le paquet qui sera encodé et envoyé
 * @seqnum: Le seqnum du packet que l'on a recu
 * @sfd: Le file descriptor du socket utilisé
 * @ack: PTYPE_ACK si acquittement, PTYPE_NACK si non-acquittement
 * @time_data: Le timestamp du packet recu, à encoder dans le packet d'acquittement
 * @window: La taille de la window
 *
 * @return: Renvoit 0 en cas de succès, sinon -1
 */
int send_ack(pkt_t *pkt_ack, int seqnum, int sfd, int ack, uint32_t time_data, int window);

/*
 * Mets à jours le buffer
 * La fonction :
 * - 
 *
 * @index:
 * @seq_rcv: Le seqnum recu
 * @buffer_payload:
 * @buffer_len:
 * @pkt_rcv: Le packet recu
 * @window: La taille de la window
 *
 * @post: Renvoit 0 en cas de succès, sinon -1
 */

void add_buffer(int index, int seq_rcv, int seq_exp,char ** buffer_payload, int *buffer_len, pkt_t * pkt_rcv, int window);
#endif
