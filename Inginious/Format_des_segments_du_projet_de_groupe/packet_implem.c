#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
/* Extra #includes */
/* Your code will be inserted here */

struct __attribute__((__packed__)) pkt {
	struct header {
		ptypes_t TYPE : 2;
		unsigned int TR : 1;
		unsigned int WINDOW : 5;
		uint8_t SEQNUM;
		uint16_t LENGTH;
	} header;
	uint32_t TIMESTAMP;
	uint32_t CRC1;
	char * PAYLOAD;
	uint32_t CRC2;
};

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
	pkt_t *new_pkt;
	new_pkt = (pkt_t*) malloc(sizeof(pkt_t));
	if(new_pkt == NULL){
		perror("Erreur lors du malloc du package");
		return NULL;
	}
	new_pkt->PAYLOAD = (char *) malloc(sizeof(char));
	if(new_pkt->PAYLOAD == NULL){
		perror("Erreur lors du malloc du paylod");
		return NULL;
	}
	return new_pkt;
}

void pkt_del(pkt_t *pkt)
{
	free(pkt->PAYLOAD);
	free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
	if(len == 0)
		return E_UNCONSISTENT;

	if(len < 4) //trop petit pour contenir le header
		return E_NOHEADER;


	/*****************************************
							ON CHECK LE HEADER
	******************************************/

	pkt_status_code verif_status;

	uint8_t hd = data[0];

	//Decodage du type / 2premiers bits du premier octet
	verif_status = pkt_set_type(pkt, hd >> 6); //On ne veut que les deux bits de poids lourd ici Ex : si uint8_t est 11001010 alors on obtiendra 00000011 pour le type
	if(verif_status != PKT_OK)
		return verif_status;

	//set du TR à 0 avant recalcul de CRC1 / 3eme bit du premier octet
	verif_status = pkt_set_tr(pkt, 0);
	if(verif_status != PKT_OK)
		return verif_status;

	//Décodage de la window / 5derniers bits du premier octect
	verif_status = pkt_set_window(pkt, by&31); //On veut les 5bits de poids faibles donc on fait AND 00011111 Ex : 10100110 & 00011111 = 00000110
	if(verif_status != PKT_OK)
		return verif_status;


	//Décodage Seqnum / deuxième octet
	verif_status = pkt_set_seqnum(pkt, data[1]);
	if(verif_status != PKT_OK)
		return verif_status;

	//Décodage de Length / Length est en network byte-order et il faut donc la convertir en host byte-order avec noths()
	uint16_t pkt_length = ntohs(*((uint16_t *)(data + 2))); // (data+2) = Les 2bytes après les 2premiers bytes
	verif_status = pkt_set_length(pkt, pkt_length);
	if(verif_status != PKT_OK)
		return verif_status;

	/*****************************************
	OK POUR LE HEADER, ON PASSE AU CRC/PAYLOAD
	******************************************/

	//TODO faire le CRC / PAYLOAD

	return verif_status;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
	/* Your code will be inserted here */
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
	return pkt->header.TYPE;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
	return pkt->header.TR;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
	return pkt->header.WINDOW;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
	return pkt->header.SEQNUM;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
	return ntohs(pkt->header.LENGTH);
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
	return pkt->TIMESTAMP;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
	return ntohl(pkt->CRC1);
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
	return ntohl(pkt->CRC2);
}

const char* pkt_get_payload(const pkt_t* pkt)
{
	size_t pkt_length = pkt_get_length(pkt);
	if(pkt_length == 0)
		return NULL;
	return pkt->PAYLOAD;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
	if(type == PTYPE_DATA || type == PTYPE_ACK || type == PTYPE_NACK){
		pkt->header.TYPE = type;
		return PKT_OK;
	}
	return E_TYPE;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
	if(pkt->header.TYPE == PTYPE_DATA){
		pkt->header.TR = tr;
		return PKT_OK;
	}
	return E_TR;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
	if(window > MAX_WINDOW_SIZE)
		return E_WINDOW;
	pkt->header.WINDOW = window;
	return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
	pkt->header.SEQNUM = seqnum;
	return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
	if(length > MAX_PAYLOAD_SIZE)
		return E_LENGTH;
	pkt->header.LENGTH = length;
	return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
	pkt->TIMESTAMP = timestamp;
	return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
	pkt->CRC1 = crc1;
	return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
	pkt->CRC2 = crc2;
	return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
							    const char *data,
								const uint16_t length)
{
	pkt_status_code return_status = pkt_set_length(pkt, length);

	if(return_status == PKT_OK){
		memcpy(pkt->PAYLOAD, data, length);
	}
	return status;
}
