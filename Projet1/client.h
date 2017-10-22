#ifndef __CLIENT_H_
#define __CLIENT_H_
#include <sys/time.h>
/*
 * Envoie de données lues dans un fichier ou dans le flux stdin vers un destinataire
 * La fonction :
 * - Crée un socket en IPV6 , UDP et se connecte à l'adresse de destination
 * - Envoie des données par packet et réagit suivant s'il recoit un acquittement
 *   un non-acquittement
 *
 * @hostname: L'addresse à laquelle on se connecte
 * @port: Le port auquel on se connecte
 * @file: Un nom de fichier contenant les données ou NULL si non spécifié
 * @post: Toutes les données du contenues dans le fichier ou sur le stdin on été envoyée
 *
 * @return:
 */
void send_data(char *hostname, int port, char* file);

/*
 * Adapte le buffer en cas d'acquittement.
 * La fonction verifie que:
 *  -
 *
 * @small_seq: Pointeur vers le seqnum du plus petit element du buffer
 * @seq: Numéro de la séquence acquittée
 * @small_index: L'index du plus petit élément dans le buffer
 * @window: La taille de la fenêtre
 * @timestamp: Le temps auquel le packet a été envoyé par le sender,
 *             ce temps est récupéré sur l'acquittement.
 * @tv: Le timeout actuel
 * @nbre_tv: Le nombre de calcul de rtt moyen déja réalisé
 */
void adapt_ack(int *small_seq,int seq, int *small_index,int window, uint32_t timestamp, struct timeval tv, int  nbre_tv,int *buffer_size);

/*
 * Mets à jours le timeout
 * La fonction verifie que:
 * - Le CRC32 du header recu est le mÃªme que celui decode a la fin
 *   du header (en considerant le champ TR a 0)
 * - S'il est present, le CRC32 du payload recu est le meme que celui
 *   decode a la fin du payload
 * - Le type du paquet est valide
 * - La longueur du paquet et le champ TR sont valides et coherents
 *   avec le nombre d'octets recus.
 *
 * @tv1: Le temps auquel on a recu l'acquittement
 * @timestamp: Le temps auquel le packet a été envoyé
 * @tv: Temps Timeout courant
 * @nbre_tv: Nombre de calculé de adapt_rtt déja réalisé
 *
 * @post: tv contient le nouveau timeout recalculé
 */
void adapt_rtt(struct timeval tv1, uint32_t timestamp, struct timeval *tv, int nbre_tv);
#endif
