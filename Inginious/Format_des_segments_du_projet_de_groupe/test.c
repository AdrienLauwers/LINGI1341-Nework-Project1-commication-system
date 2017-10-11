#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "packet_interface.h"

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
void type(void) {
	pkt_t * pkt = pkt_new();

	pkt_status_code return_status = pkt_set_type(pkt, PTYPE_DATA);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), PTYPE_DATA);
	return_status = pkt_set_type(pkt, 4);
	CU_ASSERT_NOT_EQUAL(return_status, PKT_OK);

	pkt_del(pkt);
}


int main(int argc, char* argv[]){
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
			NULL == CU_add_test(pSuite, "pkt_get_set_type", type)
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
