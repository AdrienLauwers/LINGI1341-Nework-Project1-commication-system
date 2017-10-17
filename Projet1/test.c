#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "packet_interface.h"

static uint8_t pkt_tab[] = {0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};


//1
void new(void) {
	pkt_t* pkt = pkt_new();
	CU_ASSERT_PTR_NOT_NULL(pkt);
	pkt_del(pkt);
}
//2
void type(void) {
	pkt_t * pkt = pkt_new();

	pkt_status_code return_status = pkt_set_type(pkt, PTYPE_DATA);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), PTYPE_DATA);
	return_status = pkt_set_type(pkt, 4);
	CU_ASSERT_NOT_EQUAL(return_status, PKT_OK);

	pkt_del(pkt);
}
//3
void tr(void) {
	pkt_t * pkt = pkt_new();
	pkt_status_code return_status = pkt_set_type(pkt, 2);
	return_status = pkt_set_tr(pkt, 1);
	CU_ASSERT_EQUAL(return_status, E_TR);
	return_status = pkt_set_type(pkt, PTYPE_DATA);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_tr(pkt), 0);
	return_status = pkt_set_tr(pkt, 2);
	CU_ASSERT_EQUAL(return_status, E_TR);

	pkt_del(pkt);
}

//4
void window(void) {
	pkt_t * pkt = pkt_new();
	int random = rand()%32;
	pkt_status_code return_status = pkt_set_window(pkt, random);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_window(pkt), random);
	return_status = pkt_set_window(pkt, 32);
	CU_ASSERT_EQUAL(return_status, E_WINDOW);

	pkt_del(pkt);
}

//5
void seqnum(void) {
	pkt_t * pkt = pkt_new();
	int random = rand()%256;
	pkt_status_code return_status = pkt_set_seqnum(pkt, random);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_seqnum(pkt), random);

	pkt_del(pkt);
}

//6
void length(void) {
	pkt_t * pkt = pkt_new();

	int random = rand()%513;

	pkt_status_code return_status = pkt_set_length(pkt, random);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	return_status = pkt_set_tr(pkt, 0);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), random);
	return_status = pkt_set_type(pkt, PTYPE_DATA);
	return_status = pkt_set_tr(pkt, 1);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);
	return_status = pkt_set_length(pkt, 513);
	CU_ASSERT_EQUAL(return_status, E_LENGTH);

	pkt_del(pkt);
}

//7
void timestamp(void) {
	pkt_t * pkt = pkt_new();
	uint32_t random = rand()%4294967296;
	pkt_status_code return_status = pkt_set_timestamp(pkt, random);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_timestamp(pkt), random);

	pkt_del(pkt);
}


//10
void payload(void) {
	pkt_t * pkt = pkt_new();
	char * buf = "Test de 15 char";
	char buf2[100];
	memcpy(buf2, buf, 15);
	pkt_status_code return_status = pkt_set_payload(pkt, buf, 16);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_STRING_EQUAL(pkt_get_payload(pkt), buf);
	CU_ASSERT_NOT_EQUAL(pkt_get_length(pkt), 1);
	return_status = pkt_set_payload(pkt, buf2, 100);

	pkt_del(pkt);
}
//10
void encode(void) {
	pkt_t * pkt1 = pkt_new();
	pkt_t * pkt2 = pkt_new();
	char * buf = "Test de 15 char";
	char data[528];
	pkt_status_code return_status = pkt_set_payload(pkt1, buf, 16);
	size_t len = 528;
	return_status = pkt_encode(pkt1, data,  &len);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	return_status = pkt_decode(data, sizeof data, pkt2);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_STRING_EQUAL(buf, pkt_get_payload(pkt2));
	CU_ASSERT_EQUAL(pkt_get_tr(pkt1), pkt_get_tr(pkt2));
	CU_ASSERT_EQUAL(pkt_get_crc1(pkt1), pkt_get_crc1(pkt2));
	CU_ASSERT_EQUAL(pkt_get_crc2(pkt1), pkt_get_crc2(pkt2));
	CU_ASSERT_EQUAL(pkt_get_type(pkt1), pkt_get_type(pkt2));
	CU_ASSERT_EQUAL(pkt_get_window(pkt1), pkt_get_window(pkt2));
	CU_ASSERT_EQUAL(pkt_get_timestamp(pkt1), pkt_get_timestamp(pkt2));

	pkt_del(pkt1);
	pkt_del(pkt2);

}

int main(){
	printf("Salut : %u \n", pkt_tab[1]);
  CU_pSuite pSuite = NULL;
	/* initialisation de la suite*/
	if(CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* création de la suite */
	pSuite = CU_add_suite("Suite",NULL,NULL);
	if(NULL == pSuite){
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Ajout à la suite */
	if(NULL == CU_add_test(pSuite, "pkt_new", new) ||
			NULL == CU_add_test(pSuite, "pkt_get_set_type", type) ||
			NULL == CU_add_test(pSuite, "pkt_get_set_tr", tr)||
			NULL == CU_add_test(pSuite, "pkt_get_set_window", window)||
			NULL == CU_add_test(pSuite, "pkt_get_set_seqnum", seqnum)||
			NULL == CU_add_test(pSuite, "pkt_get_set_length", length)||
			NULL == CU_add_test(pSuite, "pkt_get_set_timestamp", timestamp)||
			NULL == CU_add_test(pSuite, "pkt_get_set_payload", payload)||
			NULL == CU_add_test(pSuite, "pkt_get_set_encode", encode)
			)
	   {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Lancement des tests */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
