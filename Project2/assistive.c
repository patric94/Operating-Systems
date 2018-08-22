#include "assistive.h"

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


void answering_query(char *x, char* y, char* r, char* forth, int workers, int points, int mod, char *shape, char *exe, char *dir, char *filename, int myID, FILE *shape_out_fp){
    int i, j, wrk;
    MyRecord rec;
    struct pollfd fdarray[workers];
    int temp = 0, rc; //Temporary variable to hold the number of workers who checked +1 record than teh result of the division numOfrecords / workers.
    char ** fpath;  //We need the mod to decide how many workers will check +1 record from the result of the division numOfrecords / workers
    if ((fpath = (char**) malloc(workers*sizeof(char*))) == NULL) {
        fprintf(stderr, "Error ! Not enough space for fifoname's array!\n");
        exit(1);
    }
    int check[workers];
    int wrks[workers];
    FILE *fds_fp[workers];
    for (i = 0; i < workers; i++) {
        char * fifoname;
        if ((fifoname = (char *) malloc(20*sizeof(char))) == NULL) {
            fprintf(stderr, "Error ! Not enough space for fifo's name\n");
            exit(0);
        }
        if (( fpath[i] = (char *) malloc(256*sizeof(char))) == NULL) {
            fprintf(stderr, "Error ! Not enough memory for current directory!\n");
            exit(0);
        }
        strcpy(fpath[i], dir);
        sprintf(fifoname, "%d_%d.fifo", myID, i);
        strcat(fpath[i], fifoname);
        if ((mkfifo(fpath[i], 0666) == -1) && (errno != EEXIST)){
            perror("mkfifo error!");
            exit(0);
        }
        check[i] = 0;
        if ((mod - i)>0) { //If there is still a non-zero value in mod then more workers will check +1 record than the result of the division.
            temp = i+1;
            if ((wrk = fork()) == -1) {
                perror("Fork Error on Worker.") ; exit(1);
            } else if (wrk == 0) {
                char pnts[7], bytes[10], *cpath;
                sprintf(pnts, "%d", points+1);
                sprintf(bytes, "%d", i*(points+1)*(int)sizeof(rec));
                if (( cpath = (char *) malloc(256*sizeof(char))) == NULL) {
                    fprintf(stderr, "Error ! Not enough memory for current directory!\n");
                    exit(0);
                }
                getcwd(cpath, 256);
                strcat(cpath, shape);
                if (forth == NULL) {
                    execlp(cpath, exe, "-i", filename, "-a", x, y, r, "-o", fpath[i], "-f", bytes, "-n", pnts, NULL);
                } else {
                    execlp(cpath, exe, "-i", filename, "-a", x, y, r, forth, "-o", fpath[i], "-f", bytes, "-n", pnts, NULL);
                }
            } else {
                fprintf(stderr, "Worker %d with id %d of handler %d is executing the requested shape.\n", i, wrk, myID);
                if ((fds_fp[i] = fopen(fpath[i], "r")) == 0){
                    perror("Error in opening fifo file");
                    exit(1);
                }
                wrks[i] = wrk;
            }
        } else { //If there is a zero value in mod or we assigned already mod workers to read +1 record than the result of the division
            if ((wrk = fork()) == -1) {
                perror("Fork Error on Worker.") ; exit(1);
            } else if(wrk == 0) { //The offset here begins from the amount of workers with +1 records checked plus those workers who ckecked
                char pnts[7], bytes[10], *cpath;  //normally numOfrecords / workers records after the previous amouf of workers. IF THAT EXISTS.
                sprintf(pnts, "%d", points);
                sprintf(bytes, "%d", (int)(temp*(points+1)*sizeof(rec))+(int)((i-temp)*points*sizeof(rec)));
                if (( cpath = (char *) malloc(256*sizeof(char))) == NULL) {
                    fprintf(stderr, "Error ! Not enough memory for current directory!\n");
                    exit(0);
                }
                getcwd(cpath, 256);
                strcat(cpath, shape);
                if (forth == NULL) {
                    execlp(cpath, exe, "-i", filename, "-a", x, y, r, "-o", fpath[i], "-f", bytes, "-n", pnts, NULL);
                } else {
                    execlp(cpath, exe, "-i", filename, "-a", x, y, r, forth, "-o", fpath[i], "-f", bytes, "-n", pnts, NULL);
                }
            } else {
                fprintf(stderr, "Worker %d with id %d of handler %d is executing the requested shape.\n", i, wrk, myID);
                if ((fds_fp[i] = fopen(fpath[i], "r")) == 0){
                    perror("Error in opening fifo file");
                    exit(1);
                }
                wrks[i] = wrk;
            }
        }
        free(fifoname);
    }
    char *outfile;
    if (( outfile = (char *) malloc(256*sizeof(char))) == NULL) {
        fprintf(stderr, "Error ! Not enough memory for current directory!\n");
        exit(1);
    }
    sprintf(outfile, "%s%d.out", dir, myID);
    if ((shape_out_fp = fopen(outfile, "wt")) == 0) {
        perror(".out file error !");
        exit(1);
    }
    int waiter;
    for (i = 0; i < workers; i++) {
        int status;
        waiter = wait(&status);
        int temp;
        for (j = 0; j < workers; j++) {
            if (waiter == wrks[j]) {
                temp = j;
                break;
            }
        }
        MyRecord temprec;
        while (fscanf(fds_fp[temp], "%f%f", &temprec.x, &temprec.y) != EOF) {
            fprintf(shape_out_fp, "%.2f\t%.2f\n",temprec.x, temprec.y);
        }
        fclose(fds_fp[temp]);
    }
    free(outfile);
    for (i = 0; i < workers; i++) {
        free(fpath[i]);
    }
    free(fpath);
    fclose(shape_out_fp);
    free(dir);
    exit(0);
}
