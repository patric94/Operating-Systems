#include "record.h"


int hashFunc(int , int );

void createHashTable(int , RecPtr* );

int insertHash(RecPtr , RecPtr* , int );

RecPtr searchHash(int, int, RecPtr*, int);

void deleteHash(RecPtr*, int, int, int);

float getAvg(RecPtr*, int, int);

void coursesToTake(RecPtr*, int, char*, int);

float percentile(RecPtr*, int, int, int);

void top_students(RecPtr*, int, int, int);

void purgeHash(RecPtr*, int );

void printHash(RecPtr* , int );
