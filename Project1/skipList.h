#include "hashTable.h"

typedef struct snode * snodePtr;

typedef struct snode {
    int key;
    RecPtr value;
    int level;
    snodePtr* ForwardPtrs;
} snode;

typedef struct skipList {
    int size;
    int MaxLevel;
    int MaxValue;
    snodePtr header;        //header node
    snodePtr terNode;       //terminating node
} skipList;

static int rand_level(int );

snodePtr create_snode(snodePtr, int , int, RecPtr);

void delete_snode(snodePtr);

skipList* initialize(skipList *, int , int );

int insertSkip(skipList *, int, RecPtr, int);

RecPtr searchSkip(skipList *, int);

int deleteSkip(skipList *, int, int, RecPtr *, int);

void raSearch(skipList*, int, int);

void mostCourses(skipList*, float, int);

void bottomSearch(skipList*, int);

void purgeSkip(skipList*);

void printSkip(skipList *);

void printGPAS(skipList *);
