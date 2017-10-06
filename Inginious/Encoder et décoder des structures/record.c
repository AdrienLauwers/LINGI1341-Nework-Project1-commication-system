#include "record.h"
#include <stdio.h>
#include <stdlib.h>

int record_init(struct record *r){
    r->header.TYPE=0;
    r->header.F=0;
    r->header.LENGTH=0;
    r->PAYLOAD = NULL;
    return 0;
}

void record_free(struct record *r){
    if (r->header.LENGTH !=0){
        free(r->PAYLOAD);
        r->header.LENGTH=0;
        r->PAYLOAD= NULL;
  }
}


int record_get_type(const struct record *r){
    return r->header.TYPE;
}

void record_set_type(struct record *r, int type){
    r->header.TYPE = type;
}

int record_get_length(const struct record *r){
    return r->header.LENGTH;
}

int record_set_payload(struct record *r, const char *buf, int n){
    if ( buf == NULL || n == 0 ) {
       record_free(r);
       return 0;
    }
    (r->PAYLOAD)= (char *) realloc((r->PAYLOAD), n * sizeof(char));
    if (r->PAYLOAD == NULL) return -1;

    memcpy(r->PAYLOAD, buf, n);
    r->header.LENGTH = n ;

    return 0;
}


int record_get_payload(const struct record *r, char *buf, int n){

    memcpy(buf, r->PAYLOAD, n);
    return (r->header.LENGTH < n) ? r->header.LENGTH : n;;


}

int record_has_footer(const struct record *r){
    return r->header.F;
}

void record_delete_footer(struct record *r){
  if (r->header.F !=0){
      r->header.F= 0;
      r->UUID=0;
  }

}

void record_set_uuid(struct record *r, unsigned int uuid){
   r->header.F= 1;
   r->UUID = uuid;

}

unsigned int record_get_uuid(const struct record *r){
    if (r->header.F == 0) return 0;
    return r->UUID;

}

int record_write(const struct record *r, FILE *f){
    uint16_t *x  =  (uint16_t *)&(r->header);
    uint16_t y= (*x);

    uint16_t *x1  = x+1;
    uint16_t y1= htons(*x1);

    int count=0;
    int temp=0;
    temp = fwrite( &y, sizeof(uint16_t), 1, f);
    if (temp!= 1 ) return -1;
    count+= temp * sizeof(uint16_t);
    temp = fwrite( &y1, sizeof(uint16_t), 1, f);
    if (temp!= 1 ) return -1;
    count+= temp * sizeof(uint16_t);

    if (r->header.LENGTH !=0 ){

        temp = fwrite( (r->PAYLOAD), sizeof(char), r->header.LENGTH, f);
        if (temp!= r->header.LENGTH ) return -1;
        count+= temp * sizeof(char);
    }

    if (r->header.F == 1) {
        temp = fwrite( &(r->UUID), sizeof(uint32_t), 1, f);
        if (temp!= 1) return -1; // 1 * 4bytes= 4 octe
     count+= temp * sizeof(uint32_t);
        }
   //fclose(f);
   //if (fclose(f)!=0) return -1; // error fclose
    return count;
}
int record_read(struct record *r, FILE *f){
    int count=0;
    int temp=0;

    uint16_t *x =  (uint16_t *)&(r->header);
    uint16_t y;

    uint16_t *x1  = (uint16_t *)((char *)(&(r->header))+2);
    uint16_t y1;

  //read header
    temp = fread( &y, sizeof(uint16_t), 1, f);
		printf("Test 1\n");
    if (temp== -1 ) return -1;
    count+= temp * sizeof(uint16_t);
     *x= y;

    temp = fread( &y1, sizeof(uint16_t), 1, f); // length
		printf("Test 2\n");
    if (temp==- 1 ) return -1;
    count+= temp * sizeof(uint16_t);
    *x1 = ntohs(y1);

   // read payload
    if ( r->header.LENGTH !=0 ){
        char * buf= (char *) malloc (sizeof(char) * r->header.LENGTH);
        temp = fread( buf, sizeof(char), r->header.LENGTH, f);
				printf("Test 3\n");
        if (temp!= r->header.LENGTH ) return -1;
        record_set_payload(r, buf, r->header.LENGTH); // check return
        count+= temp * sizeof(char);
    }

   //read footer
    if (r->header.F == 1) {
        temp = fread( &(r->UUID), sizeof(uint32_t), 1, f);
				printf("Test 4\n");
        if (temp!= 1) return -1; // 1 * 4bytes= 4 octe
        count+= temp * sizeof(uint32_t);
    }

  // if (fclose(f)!=0) return -1; // error fclose
    return count;
}
