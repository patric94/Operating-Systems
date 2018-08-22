#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record * RecPtr;

typedef struct Record {
    int studID;
    char lastname[50];  //Safer than dynamic allocation. I accept at max 50 characters
    char firstname[50]; //Safer than dynamic allocation. I accept at max 50 characters
    float gpa;
    int courses;
    char deprt[3];      // I took the liberty of setting the max characters of the department info to 3 characters
    int postCode;
    RecPtr next;        // A pointer to another record of a student. Used for the chained hashing
} Record;

RecPtr createRecord (RecPtr, int, char*, char*, float, int, char*, int);

void deleteRecord (RecPtr);

void modifyRec(RecPtr, float, int);

void printRecord(RecPtr);
