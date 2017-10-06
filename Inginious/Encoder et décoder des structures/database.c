#include <stdlib.h>
#include <stdio.h>


#include "macros.h"
#include "record.h"

int main(int argc, const char **argv){
	struct record new_record;

	if (record_init(&new_record)) {
		ERROR("Couldn't initialize a record!");
		return EXIT_FAILURE;
	}
  record_set_type(&new_record, 1);
	char * test = (char *) malloc(10);
	test = "test";
	int ret_set = record_set_payload(&new_record, test, 20);
	int length1= new_record.header.LENGTH;
	record_set_uuid(&new_record, 1);
	char * test2 = (char *) malloc(10);
	int ret_get = record_get_payload(&new_record, test2, 8);
	int length2 = new_record.header.LENGTH;
	/* Some debug code be guarded in ifdef's
	 * Enable it compiling with `make debug`
	 */
#ifdef DEBUG
	LOG("Content of the record after initialization:");
	LOG("\tType= %d", record_get_type(&new_record));
	if (record_has_footer(&new_record))
		LOG("\tUUID= %d", record_get_uuid(&new_record));
	else
		LOG("\tNo footer");
	LOG("\tLength= %d", record_get_length(&new_record));
	LOG("\tPayload= %s , %d", new_record.PAYLOAD, ret_set);

	LOG("\tPayload= %s , %d", test2, ret_get);
	LOG("\tLength1= %d , Length2= %d", length1, length2);
#endif

	/* Do something with the record, e.g. write it in a file, load one from
	 * a file, ...
	 * You should test your implementation here.
	 * Think on using the program arguments
	 * as an easy way to script a lot of tests.
	 */
	 struct record second_record;

	 	if (record_init(&second_record)) {
	 		ERROR("Couldn't initialize a record!");
	 		return EXIT_FAILURE;
	 	}

	 FILE *fp;
	 fp=fopen("output2.dms","w+");
	 int count2W = record_write(&new_record, fp);
	 printf("%d \n", count2W);
	 int count2 = record_read(&second_record, fp);
	 printf("%d \n", count2);
	 fclose(fp);
	return EXIT_SUCCESS;
}
