#include "hashTable.h"

int hashFunc(int id, int size){
    return id % size;
}

void createHashTable(int size, RecPtr* table){
    int i;
    for ( i = 0; i < size; i++) {
        table[i]= NULL;
    }
}

void deleteHash(RecPtr* ht, int size, int pc, int id) {
    RecPtr temp,prev,succ;
    int hash = hashFunc(pc, size);
    temp = ht[hash];
    while (temp != NULL) {
        if(temp->next->studID == id){
            printRecord(temp->next);
            prev = temp;
            temp = temp->next;                  //Here we connect the previous and the next(succ) item in the hash
            succ = temp->next;                  //so after the deletion of the the reuqested record we wont mess up
            prev->next = succ;                  //with the hash table
            temp->next = NULL;
            deleteRecord(temp);
            break;
        }
        temp = temp->next;
    }
}

int insertHash(RecPtr in, RecPtr* ht, int size){
    RecPtr temp;
    int hash = hashFunc(in->postCode, size);
    temp = ht[hash];
    if(temp == NULL){
        ht[hash] = in;
        return 1;
    }
    else{
        while(temp != NULL){
            if(temp->next == NULL ){
                temp->next = in;
                return 1;
            }
            temp = temp->next;
        }
    }
}

RecPtr searchHash(int searchID, int searchPC, RecPtr* ht, int size){
    RecPtr temp;
    int hash = hashFunc(searchPC, size);
    temp = ht[hash];
    while (temp != NULL) {
        if (temp->studID == searchID) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

float getAvg(RecPtr* ht, int size, int pc){
    float tavg = 0;
    RecPtr temp;
    int hash = hashFunc(pc, size), count = 0;
    temp = ht[hash];
    while (temp != NULL) {
        if(temp->postCode == pc){
            count++;
            tavg += temp->gpa;
        }
        temp = temp->next;
    }
    if (count) {
        return tavg/count;
    } else {
        return 0;
    }
}

void coursesToTake(RecPtr* ht, int pc, char* dep, int size) {
    RecPtr temp;
    int hash = hashFunc(pc, size), count = 0, courses = 0;
    temp = ht[hash];
    while (temp != NULL) {
        if(temp->postCode == pc && memcmp(temp->deprt, dep, sizeof(temp->deprt)) == 0){
            count++;
            courses += temp->courses;
            printRecord(temp);
        }
        temp = temp->next;
    }
    if (!count) {
        fprintf(stderr,"not found\n");
    }
    else{
        fprintf(stdout, "%d\n", courses);
    }
}

float percentile(RecPtr* ht, int pc, int size, int studs) {
    RecPtr temp;
    int hash = hashFunc(pc, size), count = 0;
    temp = ht[hash];
    while (temp != NULL) {
        if(temp->postCode == pc){
            count++;
        }
        temp = temp->next;
    }
    return (float)(count)/(float)(studs);
}

void top_students(RecPtr* ht, int pc, int size, int amount) {
    RecPtr temp;
    RecPtr* sorting;
    int hash = hashFunc(pc, size), count = 0, pos, i;
    float min;
    temp = ht[hash];
    if ( (sorting = (RecPtr*) malloc(amount*sizeof(RecPtr))) == NULL ) {
        fprintf(stderr,"Memory Error for the sorting table!\n");
    }
    else {
        for (i = 0; i < amount; i++) {                      //We fill the array first with NULL value in case in the post code given, live
            sorting[i] = NULL;                             //less people than requested
        }
        while (temp != NULL && count < amount) {
            if (temp->postCode == pc) {
                sorting[count] = temp;                  //We then take the first k students found in the bucket
                count++;
            }
            temp = temp->next;
        }
        while (temp != NULL) {
            min = sorting[0]->gpa;
            pos = 0;
            if (temp->postCode == pc) {
                for (i = 1; i < amount; i++) {          //Then if the item we check from the bucket is in the post code we want
                    if (sorting[i]->gpa < min) {        //we try to find the student with the minimum gpa score
                        min = sorting[i]->gpa;
                        pos = i;
                    }
                }
                if (temp->gpa > min) {                  //Then if the score of the student from the bucket has a higher gpa score
                    sorting[pos] = temp;                //than the one with the minimum score from the table we swap them.
                }
            }
            temp = temp->next;
        }
        for (i = 0; i < amount; i++) {                  //Then the table has the students with the highest gpa scores
            printRecord(sorting[i]);
        }
        for ( i = 0; i < amount; i++) {
            sorting[i] = NULL;
        }
        free(sorting);
    }
}

void purgeHash(RecPtr* ht, int size){
    RecPtr iter,temp;
    int i;
    for (i=0; i<size; i++){
        iter = ht[i];
        while((temp = iter) != NULL){
            iter = iter->next;
            deleteRecord(temp);
        }
    }
    free(ht);
}

void printHash(RecPtr* ht, int size){
    int i;
    RecPtr temp;
    for (i=0; i<size; i++){
        temp = ht[i];
        while(temp != NULL){
            fprintf(stderr,"||%d|| %d %s %s %.2f %d %s %d\n", i, temp->studID, temp->lastname, temp->firstname, temp->gpa, temp->courses, temp->deprt, temp->postCode);
            temp = temp->next;
        }
    }
}
