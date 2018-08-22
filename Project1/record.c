#include "record.h"

RecPtr createRecord (RecPtr r, int id, char* lname, char* fname, float avg, int crs, char* dpt, int pc){

    if( (r = (RecPtr) malloc( sizeof(Record)) )== NULL ){
        fprintf(stderr,"Memory Error for the new record!\n");
        return NULL;
    }
    r->next = NULL;
    memset(r->lastname, '\0', sizeof(r->lastname));
    memset(r->firstname, '\0', sizeof(r->firstname));
    memset(r->deprt, '\0', sizeof(r->deprt));

    r->studID = id;
    memcpy(r->lastname, lname, sizeof(r->lastname)-1);
    memcpy(r->firstname, fname, sizeof(r->firstname)-1);
    r->gpa = avg;
    r->courses = crs;
    memcpy(r->deprt, dpt, sizeof(dpt));
    r->postCode = pc;
    return r;
}

void deleteRecord(RecPtr r){
    r->next = NULL;
    free(r);
}

void modifyRec(RecPtr r, float navg, int ncrs) {
    r->courses = ncrs;
    r->gpa = navg;
}

void printRecord(RecPtr r) {
    if (r != NULL) {
        fprintf(stdout,"%d %s %s %.2f %d %s %d\n", r->studID, r->lastname, r->firstname, r->gpa, r->courses, r->deprt, r->postCode);
    }
}
