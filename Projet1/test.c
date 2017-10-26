#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <CUnit/Basic.h>
#include "packet_interface.h"
#include "server.h"
#include "client.h"

//static uint8_t pkt_tab[] = {0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};


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

void encode_with_error_timestamp(void){
	pkt_t * pkt1 = pkt_new();
	pkt_t * pkt2 = pkt_new();
	char * buf = "Test de 15 char";
	char data[528];
	pkt_status_code return_status = pkt_set_payload(pkt1, buf, 16);
	size_t len = 528;
	return_status = pkt_encode(pkt1, data,  &len);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	memset((void*)(data+4), 1345, 4);
	return_status = pkt_decode(data, sizeof data, pkt2);
	CU_ASSERT_EQUAL(return_status, E_CRC);
	pkt_del(pkt1);
	pkt_del(pkt2);
}

void encode_with_error_crc1(void){
	pkt_t * pkt1 = pkt_new();
	pkt_t * pkt2 = pkt_new();
	char * buf = "Test de 15 char";
	char data[528];
	pkt_status_code return_status = pkt_set_payload(pkt1, buf, 16);
	size_t len = 528;
	return_status = pkt_encode(pkt1, data,  &len);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	memset((void*)(data+8), 1345, 4);
	return_status = pkt_decode(data, sizeof data, pkt2);
	CU_ASSERT_EQUAL(return_status, E_CRC);
	pkt_del(pkt1);
	pkt_del(pkt2);
}


void encode_with_error_crc2(void){
	pkt_t * pkt1 = pkt_new();
	pkt_t * pkt2 = pkt_new();
	char * buf = "Test de 15 char";
	char data[528];
	pkt_status_code return_status = pkt_set_payload(pkt1, buf, 16);
	size_t len = 528;
	return_status = pkt_encode(pkt1, data,  &len);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	memset((void*)(data+12), 1345, 4);
	return_status = pkt_decode(data, sizeof data, pkt2);
	CU_ASSERT_EQUAL(return_status, E_CRC);
	pkt_del(pkt1);
	pkt_del(pkt2);
}


void encode_with_error_payload(void){
	pkt_t * pkt1 = pkt_new();
	pkt_t * pkt2 = pkt_new();
	char * buf = "Test de 15 char";
	char data[528];
	pkt_status_code return_status = pkt_set_payload(pkt1, buf, 16);
	size_t len = 528;
	return_status = pkt_encode(pkt1, data,  &len);
	CU_ASSERT_EQUAL(return_status, PKT_OK);
	memset((void*)(data+pkt_get_length(pkt1)), 1345, 4);
	return_status = pkt_decode(data, sizeof data, pkt2);
	CU_ASSERT_EQUAL(return_status, E_CRC);
	pkt_del(pkt1);
	pkt_del(pkt2);
}

void send_receive(void){
	int status;
	pid_t pid = fork();
	if(pid < 0)
		exit(EXIT_FAILURE);

	if(pid == 0){
		fflush(stdout);
		receive_data("::1", 12346, "result.txt");
	}
	else{
		fflush(stdout);
		send_data("::1", 12346, "test.txt");
		int fils = waitpid(pid, &status, 0);
		if(fils == -1)
			exit(EXIT_FAILURE);
	}
	fflush(stdout);
	FILE* fd1 = fopen("test.txt", "r");
	if(fd1 == NULL) fclose(fd1);
	FILE* fd2 = fopen("result.txt", "r");
	if(fd2 == NULL) fclose(fd2);
	int ch1 = getc(fd1);
	int ch2 = getc(fd2);
	while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
         ch1 = getc(fd1);
         ch2 = getc(fd2);
      }
  CU_ASSERT(ch1 == ch2);
	fclose(fd1);
  fclose(fd2);
}


int main(){
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
			NULL == CU_add_test(pSuite, "pkt_get_set_window", window)||
			NULL == CU_add_test(pSuite, "pkt_get_set_seqnum", seqnum)||
			NULL == CU_add_test(pSuite, "pkt_get_set_length", length)||
			NULL == CU_add_test(pSuite, "pkt_get_set_timestamp", timestamp)||
			NULL == CU_add_test(pSuite, "pkt_get_set_payload", payload)||
			NULL == CU_add_test(pSuite, "pkt_get_set_encode", encode)||
			NULL == CU_add_test(pSuite, "send_receive", send_receive)||
			NULL == CU_add_test(pSuite, "encode_with_error_timestamp", encode_with_error_timestamp) ||
			NULL == CU_add_test(pSuite, "encode_with_error_crc1", encode_with_error_crc1) ||
			NULL == CU_add_test(pSuite, "encode_with_error_crc2", encode_with_error_crc2) ||
			NULL == CU_add_test(pSuite, "encode_with_error_payload", encode_with_error_payload)
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
