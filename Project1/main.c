#include <ctype.h>
#include <time.h>
#include "skipList.h"

#define MaxLevel 3
#define MaxValue 2000000

int isNumber(int, char*);

int main(int argc,char *argv[]){

    srand(time(NULL));

    FILE *fp;
    int i, hashSize;
    int flagB, flagL, flagC;   // Flags in order to check which operation characters have been put as input.
    flagB = flagL = flagC = 0;
    char buffer[200];
    if(argc < 3|| argc > 7){
        fprintf(stderr,"Error ! Wrong input of arguments ! Please re-run the program correctly!\n");
        return 1;
    }
    for (i = 1; i < argc; i+= 2) {   //A for loop in order to check the in-line arguments
        if (strcmp(argv[i], "-b") == 0) {
            if(!isNumber(strlen(argv[i+1]), argv[i+1])){ //Using a fucntion to check is a string contains any non-arithmetic characters
                fprintf(stderr,"Error ! Hash table size is not a number ! Please re-run the program correctly!\n");
                return 1;
            }
            flagB = 1;
            hashSize = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-l") == 0) {
            flagL = 1;
            if ((fp = fopen(argv[i+1], "r")) == 0) {
                fprintf(stderr,"Error ! Can't open operation file! \n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-c") == 0) {
            flagC = 1;
        }
        else {
            fprintf(stderr,"Error ! Not an acceptable parameter ! Please re-run the program correctly!\n");
            return 1;
        }
    }
    if (!flagB) {
        fprintf(stderr,"Error ! Parameter '-b' is mandatory! Please re-run the program correctly!\n");
        return 1;
    }
    skipList* skipLst;
    if( (skipLst = (skipList *) malloc(sizeof(skipList))) == NULL){
        fprintf(stderr,"Memory Error for the skip list!");
        return 1;
    }
    initialize(skipLst, MaxLevel, MaxValue);
    RecPtr* hashTable;
    if( (hashTable = (RecPtr*) malloc(hashSize*sizeof(RecPtr))) == NULL){
        fprintf(stderr,"Memory Error for the hash table!\n");
        return 1;
    }
    createHashTable(hashSize, hashTable);
    if(!flagL){
        fp = stdin;
    }
    while(fgets(buffer,sizeof(buffer),fp) != NULL){
        char* token;
        char* newline = strchr(buffer, '\n' ); //getting rid of newline character
        *newline = 0;
        char temp[200];
        strcpy(temp, buffer);                  //in order to print the command given
        if((token = strtok(buffer," ")) == NULL){
            fprintf(stderr,"Not an acceptable command for the program! Please re-run the program correctly!\n");
            return 1;
        }     //action
        if(strcmp(token, "i") == 0){ //1
            int id, crs, pc;
            float avg;
            char *fname, *lname, *dpt;
            token = strtok(NULL, " "); //studID
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this record.\n");
                continue;
            }
            id = atoi(token);
            if ((searchSkip(skipLst, id)) != NULL) {                //check for uniqueness
                fprintf(stderr,"Student ID must be unique! Please re-enter this record.\n");
                continue;
            }
            token = strtok(NULL, " "); //lastname
            lname = token;
            token = strtok(NULL, " "); //firstname
            fname = token;
            token = strtok(NULL, " "); //gpa
            if(!isNumber(strlen(token),token)){
                fprintf(stderr,"GPA must be a float value. Please re-enter this record.\n");
                continue;
            }
            avg = atof(token);
            token = strtok(NULL, " "); //courses
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Courses must be an integer value. Please re-enter this record.\n");
                continue;
            }
            crs = atoi(token);
            if( crs < 0 || crs > 52){
                fprintf(stderr,"Not an acceptable amount of remaining courses for this Student. Please re-enter this record.\n");
                continue;
            }
            token = strtok(NULL, " "); //deprt
            dpt = token;
            token = strtok(NULL, " "); //postCode
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Postal code must be an integer value. Please re-enter this record.\n");
                continue;
            }
            pc = atoi(token);
            if( pc < 10000 || pc > 99999){
                fprintf(stderr,"Postal code must be 5 characters long. Please re-enter this record.\n");
                continue;
            }
            RecPtr r = NULL;
            r = createRecord(r, id, lname, fname, avg, crs, dpt, pc);
            if (r != NULL) {
                insertSkip(skipLst , r->studID, r, MaxLevel);
                insertHash(r, hashTable, hashSize);
                printRecord(r);
            }
            else {
                fprintf(stderr,"Memory error! Please re-run the program correctly!\n");
                purgeHash(hashTable, hashSize);
                purgeSkip(skipLst);
                return 1;
            }
        }
        else if (strcmp(token, "q") == 0) {//2
            int id;
            RecPtr pRec;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this search request.\n");
                continue;
            }
            id = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            if ((pRec = searchSkip(skipLst, id)) != NULL) {
                printRecord(pRec);
            } else {
                fprintf(stderr,"not found\n");
            }
        }
        else if (strcmp(token, "m") == 0) {//3
            int id, ncrs;
            float navg;
            RecPtr pRec;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this modify request.\n");
                continue;
            }
            id = atoi(token);
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token),token)){
                fprintf(stderr,"GPA must be a float value. Please re-enter this modify request.\n");
                continue;
            }
            navg = atof(token);
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Courses must be an integer value. Please re-enter this record.\n");
                continue;
            }
            ncrs = atoi(token);
            if( ncrs < 0 || ncrs > 52){
                fprintf(stderr,"Not an acceptable amount of remaining courses for this Student. Please re-enter this modify request.\n");
                continue;
            }
            fprintf(stderr,"%s\n", temp);       //printing command given.
            if ((pRec = searchSkip(skipLst, id)) != NULL) {
                modifyRec(pRec, navg, ncrs);
                printRecord(pRec);
            } else {
                fprintf(stderr,"not found\n");
            }
        }
        else if(strcmp(token, "d") == 0){//4
            int id;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this delete request.\n");
                continue;
            }
            id = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            if(!(deleteSkip(skipLst, id, MaxLevel, hashTable, hashSize))){//Deletes from both SkipList and HashTable
                fprintf(stderr,"not found\n");
            }
        }
        else if (strcmp(token, "ra") == 0) {//5
            int idL, idU;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this raverage request.\n");
                continue;
            }
            idL = atoi(token);
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student ID must be an integer value. Please re-enter this raverage request.\n");
                continue;
            }
            idU = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            raSearch(skipLst, idL, idU);
        }
        else if (strcmp(token, "a") == 0) {//6
            int pc;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Postal code must be an integer value. Please re-enter this average-by-postcode request.\n");
                continue;
            }
            pc = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            fprintf(stdout,"%.2f\n", getAvg(hashTable, hashSize, pc));
        }
        else if (strcmp(token, "ta") == 0) {//7
            fprintf(stderr,"%s\n", temp);       //printing command given.
            int pc, amount;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student's amount must be an integer value. Please re-enter this taverage request.\n");
                continue;
            }
            amount = atoi(token);
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Postal code must be an integer value. Please re-enter this taverage request.\n");
                continue;
            }
            pc = atoi(token);
            top_students(hashTable, pc, hashSize, amount);
        }
        else if (strcmp(token, "b") == 0) {//8
            int amount;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Student's amount must be an integer value. Please re-enter this taverage request.\n");
                continue;
            }
            amount = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            bottomSearch(skipLst, amount);
        }
        else if (strcmp(token, "ct") == 0) {//9
            int pc;
            char* dpt;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Postal code must be an integer value. Please re-enter this courses-to-take request.\n");
                continue;
            }
            pc = atoi(token);
            token = strtok(NULL, " ");
            dpt = token;
            fprintf(stderr,"%s\n", temp);       //printing command given.
            coursesToTake(hashTable, pc, dpt, hashSize);
        }
        else if (strcmp(token, "f") == 0) {//10
            float avg;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token),token)){
                fprintf(stderr,"GPA must be a float value. Please re-enter this find request.\n");
                continue;
            }
            avg = atof(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            mostCourses(skipLst, avg, MaxValue);
        }
        else if (strcmp(token, "p") == 0) {//11
            int pc;
            token = strtok(NULL, " ");
            if(!isNumber(strlen(token), token)){
                fprintf(stderr,"Postal code must be an integer value. Please re-enter this percentile request.\n");
                continue;
            }
            pc = atoi(token);
            fprintf(stderr,"%s\n", temp);       //printing command given.
            fprintf(stdout,"%.2f\n", percentile(hashTable, pc, hashSize, skipLst->size));
        }
        else if (strcmp(token, "pe") == 0) {//12
            fprintf(stderr,"%s\n", temp);       //printing command given.
        }
        else if(strcmp(token, "e") == 0){//13
            fprintf(stderr,"%s\n", temp);       //printing command given.
            // printf("These are the Student Records :\n");
            // printf("Group By Post Code:\n");
            // printHash(hashTable, hashSize);
            // printf("Group By Student ID:\n");
            // printSkip(skipLst);
            // printf("%d\n", skipLst->size);
            // printGPAS(skipLst);
            purgeHash(hashTable, hashSize);
            purgeSkip(skipLst);
            break;
        }
        else{
            fprintf(stderr,"Not an acceptable command for the program! Please re-run the program correctly!\n");
            purgeHash(hashTable, hashSize);
            purgeSkip(skipLst);
            return 1;
        }
    }
    if(flagL){
        fclose(fp);
    }
    return 0;
}



int isNumber(int len, char * string){       //A fucntion to check if a given string is either an int or a float number.
    int k;
    char* s;
    s = string;
    for (k = 0 ;k < len; k++){
        if(!isdigit(*s) && *s != '.'){
            return 0;
        }
        s++;
    }
    return 1;
}
