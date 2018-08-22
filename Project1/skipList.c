#include "skipList.h"

int rand_level(int MaxLevel) {
    int level = 1;
    while (rand() < RAND_MAX / 2 && level < MaxLevel)
        level++;
    return level;
}

snodePtr create_snode(snodePtr s, int lvl, int nkey, RecPtr val){ //Creating a skip list node
    int i;
    if ( (s = (snodePtr)malloc(sizeof(snode))) == NULL){
        fprintf(stderr,"Memory Error for a new node!\n");
        return NULL;
    }
    s->key = nkey;
    s->level = lvl;
    s->value = val;
    if ( (s->ForwardPtrs = (snodePtr*)malloc(sizeof(snodePtr)*lvl)) == NULL){
        fprintf(stderr,"Memory Error for a new node's array of forward pointers!\n");
        return NULL;
    }
    for ( i = 0; i < lvl ; i++) {
        s->ForwardPtrs[i] = NULL;
    }

    return s;
}

void delete_snode(snodePtr s) {
    int i;
    for ( i = 0; i < s->level; i++ ){
        s->ForwardPtrs[i] = NULL;
    }

    free( s->ForwardPtrs );
    free(s);
}

skipList* initialize(skipList* sl, int mxL , int mxV){
    int i;
    //Initializing the terminating Node
    snodePtr terNode;
    if ( (terNode = (snodePtr)malloc(sizeof(snode))) == NULL){
        fprintf(stderr,"Memory Error for the terminating node!\n");
        return NULL;
    }
    terNode->level = mxL;
    terNode->key = mxV;
    terNode->value = NULL;
    if ( (terNode->ForwardPtrs = (snodePtr*)malloc(sizeof(snodePtr)*mxL)) == NULL){
        fprintf(stderr,"Memory Error for the terminating node's array of forward pointers!\n");
        return NULL;
    }
    for ( i = 0; i < mxL ; i++) {
        terNode->ForwardPtrs[i] = NULL;
    }
    terNode->level = mxL;
    sl->terNode = terNode;

    //Initializing the header Node
    snodePtr header;
    if ( (header = (snodePtr)malloc(sizeof(snode))) == NULL){
        fprintf(stderr,"Memory Error for the header node!\n");
        return NULL;
    }
    if ( (header->ForwardPtrs = (snodePtr*)malloc(sizeof(snodePtr)*mxL)) == NULL){
        fprintf(stderr,"Memory Error for header node's array of forward pointers!\n");
        return NULL;
    }
    for ( i = 0; i < mxL ; i++) {
        header->ForwardPtrs[i] = terNode;
    }
    header->key = 0;
    header->level = mxL;
    header->value = NULL;
    sl->header = header;

    sl->size = 0;
    return sl;
}

int insertSkip(skipList* sl, int newKey, RecPtr newValue, int MaxLevel){
    snodePtr* update;
    snodePtr temp;
    int i, lvl;
    if ( (update = (snodePtr*)malloc(sizeof(snodePtr)*MaxLevel)) == NULL){
        fprintf(stderr,"Memory Error for the update array for insert function!\n");
        return 0;
    }

    temp = sl->header;
    for ( i = MaxLevel-1; i >= 0 ; i--) {
        while (temp->ForwardPtrs[i]->key < newKey) {    //If the newKey is higher than the levels key jump to next
            temp = temp->ForwardPtrs[i];                //node of the list with the same level
        }
        update[i] = temp;                               //Else assign the node which is until now the node in this level
    }                                                   //with the lowest key from the newKey into update table

    temp = temp->ForwardPtrs[0];
    if ( temp->key == newKey ) {
        deleteRecord(temp->value);//check it later
        temp->value = newValue;
        for ( i = 0; i < MaxLevel; i++) {
            update[i] = NULL;
        }
        free(update);
        return 1;
    } else {
        lvl = rand_level(MaxLevel);
        sl->size++;
        snodePtr s = NULL;
        temp = create_snode(s, lvl, newKey, newValue);
        for ( i = 0; i < lvl; i++) {
            temp->ForwardPtrs[i] = update[i]->ForwardPtrs[i];
            update[i]->ForwardPtrs[i] = temp;
        }
        for ( i = 0; i < MaxLevel; i++) {
            update[i] = NULL;
        }
        free(update);
        return 1;
    }

}

RecPtr searchSkip(skipList* sl, int searchKey){
    int mxlvl, i;
    snodePtr temp;
    temp = sl->header;
    mxlvl = temp->level;
    for (i = mxlvl-1; i >= 0; i--) {
        while (temp->ForwardPtrs[i]->key < searchKey) {   //If the searchKey is higher than the levels key jump to next
            temp = temp->ForwardPtrs[i];                  //node of the list with the same level
        }
    }
    temp = temp->ForwardPtrs[0];
    if (temp->key == searchKey) {
        return temp->value;
    } else {
        return NULL;
    }
}

int deleteSkip(skipList* sl, int deleteKey, int MaxLevel, RecPtr* ht, int hashSize){
    snodePtr* update;
    snodePtr temp;
    int i;
    if ( (update = (snodePtr*)malloc(sizeof(snodePtr)*MaxLevel)) == NULL){
        fprintf(stderr,"Memory Error for the update array for insert function!\n");
        return 0;
    }

    temp = sl->header;
    for ( i = MaxLevel-1; i >= 0 ; i--) {
        while (temp->ForwardPtrs[i]->key < deleteKey) {    //If the deleteKey is higher than the levels key jump to next
            temp = temp->ForwardPtrs[i];                //node of the list with the same level
        }
        update[i] = temp;                               //Else assign the node which is until now the node in this level
    }                                                   //with the lowest key from the deleteKey into update table

    temp = temp->ForwardPtrs[0];
    if (temp->key == deleteKey) {
        deleteHash(ht, hashSize, temp->value->postCode, temp->value->studID);
        for (i = 0; i < MaxLevel; i++) {
            if (update[i]->ForwardPtrs[i]!=temp) {
                break;
            }
            update[i]->ForwardPtrs[i] = temp->ForwardPtrs[i];
        }
        delete_snode(temp);
        sl->size--;
        for ( i = 0; i < MaxLevel; i++) {
            update[i] = NULL;
        }
        free(update);
        return 1;
    }
    else{
        for ( i = 0; i < MaxLevel; i++) {
            update[i] = NULL;
        }
        free(update);
        return 0;               //Key not found!
    }
}

void raSearch(skipList* sl, int lowerKey, int upperKey) {
    int mxlvl, i, count = 0;
    float tgpa = 0;
    snodePtr temp;
    temp = sl->header;
    mxlvl = temp->level;
    for (i = mxlvl-1; i >= 0; i--) {
        while (temp->ForwardPtrs[i]->key < lowerKey) {   //If the searchKey is higher than the levels key jump to next
            temp = temp->ForwardPtrs[i];                  //node of the list with the same level
        }
    }
    temp = temp->ForwardPtrs[0];                        //Since we found the either the lowerKey or a Student's ID that is
    while (temp->key <= upperKey) {                     //higher than that then we iterate the skip list like a normal
        count++;                                        //linked list until we found something bigger than the upperKey
        tgpa += temp->value->gpa;
        temp = temp->ForwardPtrs[0];
    }
    if (count) {
        fprintf(stdout,"%.2f\n", tgpa/count);
    } else {
        fprintf(stdout,"%d\n", count);
    }
}

void mostCourses(skipList* sl, float avg, int maxval) {
    int max = 0;
    snodePtr temp;
    RecPtr highstud;
    temp = sl->header->ForwardPtrs[0];
    while (temp->key != maxval) {                    //We iterate the skip list like a linked list in order
        if (temp->value->courses >= max ) {         //to find those with the highest courses needed to complete their studies.
            max = temp->value->courses;
            highstud = temp->value;
        }
        temp = temp->ForwardPtrs[0];
    }
    temp = sl->header->ForwardPtrs[0];
    while (temp->key != maxval) {
        if (temp->value->courses == highstud->courses) { //Then we iterate the skip list again to find those with the highest courses
            if (temp->value->gpa > avg) {               //and their gpa to be higher than the one given.
                printRecord(temp->value);
            }
        }
        temp = temp->ForwardPtrs[0];
    }
}

void bottomSearch(skipList* sl, int amount){
    int i, k, pos;
    float max;
    snodePtr temp;
    snodePtr* sorting;
    temp = sl->header->ForwardPtrs[0];
    if ( (sorting = (snodePtr*) malloc(amount*sizeof(snodePtr))) == NULL ) {
        fprintf(stderr,"Memory Error for the sorting table!\n");
    } else {
        for (k = 0; k < amount; k++) {                  //We fill the sorting table with the first k values that we get from the skiplist
            sorting[k] = temp;
            temp = temp->ForwardPtrs[0];
        }
        while (temp->key != sl->terNode->key) {
            max = sorting[0]->value->gpa;
            pos = 0;
            for (i = 1; i < amount; i++) {              //We then keep the posistion of the student with the highest gpa score
                if (sorting[i]->value->gpa > max) {
                    max = sorting[i]->value->gpa;
                    pos = i;
                }
            }
            if (temp->value->gpa < max) {               //Then if the new student we get from the skiplist has a lower gpa score
                sorting[pos] = temp;                    //than the student with the highest gpa score, we swap them
            }
            temp = temp->ForwardPtrs[0];
        }
        for (i = 0; i < amount; i++) {                  //Then we have a table with the k students with the lowest gpa scores from the university
            printRecord(sorting[i]->value);
        }
        for ( i = 0; i < amount; i++) {
            sorting[i] = NULL;
        }
        free(sorting);
    }
}

void purgeSkip(skipList* sl){
    snodePtr iter, temp;
    iter = sl->header;
    while ((temp = iter) != NULL) {
        iter = iter->ForwardPtrs[0];
        delete_snode(temp);
    }
    free(sl);
}

void printSkip(skipList* sl){
    int i;
    snodePtr s_iter;
    s_iter = sl->header;
    do {
        for ( i = 0; i < s_iter->level; i++) {
            if(s_iter->key){
                fprintf(stdout,"KEY : %d : %d ---> |%d|\n", s_iter->key, i, s_iter->ForwardPtrs[i]->key);
            }
        }
        s_iter = s_iter->ForwardPtrs[0];
    } while(s_iter->value != NULL);

}

void printGPAS(skipList* sl){
    int i;
    snodePtr s_iter;
    s_iter = sl->header->ForwardPtrs[0];
    while (s_iter->key != sl->terNode->key) {
        fprintf(stdout, "%d : %.2f\n",s_iter->key, s_iter->value->gpa);
        s_iter = s_iter->ForwardPtrs[0];
    }
}
