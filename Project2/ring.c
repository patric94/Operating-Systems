#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "assistive.h"

int main(int argc, char *argv[]) {
    int i ,BinSize, numOfrecords, pointsToReadCount, bytesToSkip;
    int flagF, flagN, flagI, flagO, flagA, modulo;
    flagI = flagA = flagO = flagF = flagN = 0;
    float x, y, r1, r2, max, min;
    FILE *read_fp, *write_fp;
    MyRecord rec;

    if ( argc < 10 || argc > 14) {
        fprintf(stderr, "Error ! Wrong input of arguments\n");
        return 1;
    }
    i = 1;
    while ( i < argc ) {
        if ( strcmp(argv[i], "-i") == 0 ) {
            flagI = 1;
            if ((read_fp = fopen(argv[i+1], "rb")) == 0) {
                fprintf(stderr,"Error ! Can't open input file! @    \n");
                return 1;
            }
            i+= 2;
        }
        else if ( strcmp(argv[i], "-o") == 0 ) {
            flagO = 1;
            if ((write_fp = fopen(argv[i+1], "w")) == 0) {
                fprintf(stderr,"Error ! Can't open output file! \n");
                return 1;
            }
            i+= 2;
        }
        else if ( strcmp(argv[i], "-a") == 0 ) { //Checking for correct in-line arguments input after the flag -a.
            flagA = 1;
            if ( i+1 >= argc ) { fprintf(stderr, "Error ! Wrong input of arguments\n"); return 1;  }
            if ( !isNumber(strlen(argv[i+1]), argv[i+1]) ) {
                fprintf(stderr, "Not an acceptable input for the ring's centre.\n");
                return 1;
            }
            x = atof(argv[i+1]);
            if ( i+2 >= argc ) { fprintf(stderr, "Error ! Wrong input of arguments\n"); return 1;  }
            if ( !isNumber(strlen(argv[i+2]), argv[i+2]) ) {
                fprintf(stderr, "Not an acceptable input for the ring's centre.\n");
                return 1;
            }
            y = atof(argv[i+2]);
            if ( i+3 >= argc ) { fprintf(stderr, "Error ! Wrong input of arguments\n"); return 1;  }
            if ( !isNumber(strlen(argv[i+3]), argv[i+3]) ) {
                fprintf(stderr, "Not an acceptable input for the ring's big rad.\n");
                return 1;
            }
            r1 = atof(argv[i+3]);
            if ( i+4 >= argc ) { fprintf(stderr, "Error ! Wrong input of arguments\n"); return 1;  }
            if ( !isNumber(strlen(argv[i+4]), argv[i+4]) ) {
                fprintf(stderr, "Not an acceptable input for the ring's small rad.\n");
                return 1;
            }
            r2 = atof(argv[i+4]);
            i+= 5;
        }
        else if ( strcmp(argv[i], "-f") == 0 ) {
            flagF = 1;
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "Error ! Not an acceptable input for the bytesToSkip.\n");
                return 1;
            }
            bytesToSkip = atoi(argv[i+1]);
            i+= 2;
        }
        else if ( strcmp(argv[i], "-n") == 0 ) {
            flagN = 1;
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "Error ! Not an acceptable input for the pointsToReadCount.\n");
                return 1;
            }
            pointsToReadCount = atoi(argv[i+1]);
            i+= 2;
        }
        else {
            fprintf(stderr, "Error ! Unacceptable in-line argument!\n");
            return 1;
        }
    }
    if ( !flagI || !flagO || !flagA ) {
        fprintf(stderr, "Error ! Missing a mandatory in-line argument\n");
        return 1;
    }
    if ( r1 > r2) {
        max = r1;
        min = r2;
    }
    else {
        max = r2;
        min = r1;
    }

    fseek (read_fp , 0 , SEEK_END);
    BinSize = ftell (read_fp);
    rewind (read_fp);
    numOfrecords = (int) BinSize/sizeof(rec);

    if (flagF && bytesToSkip < BinSize) {   //We check if there is a -f flag on input and if the bytesToSkip requested are valid
        modulo = bytesToSkip % sizeof(rec); //We use the modulo to make the offset a valid input in case we have a non-zero modulo
        fseek(read_fp, bytesToSkip - modulo, SEEK_SET);   //If we get the invalid input we skip minus modulo bytes to make it a
        numOfrecords -= (bytesToSkip-modulo)/sizeof(rec); //valid input
    }
    else if (flagF){
        fprintf(stderr, "Error ! Wrong input for the offset parameter.\n");
        return 1;
    }

    if (flagN && pointsToReadCount <= numOfrecords && pointsToReadCount > 0) {    //We check if there is a -n flag on input and if the pointsToReadCount requested
        numOfrecords = pointsToReadCount;               //are valid. We check the -n flag after the -f flag so we can check if we are able to
    }                                                   //move so many times in the file
    else if (flagN){
        fprintf(stderr, "Error ! Wrong input for the pointsToReadCount OR Wrong combination of offset and pointsToReadCount.\n");
        return 1;
    }


    for (i = 0; i < numOfrecords; i++) {
        fread(&rec, sizeof(rec), 1, read_fp);
        if (powf(rec.x - x, 2.0)+powf(rec.y - y, 2.0) <= powf(max, 2.0) && powf(rec.x - x, 2.0)+powf(rec.y - y, 2.0) >= powf(min, 2.0)) {
            fprintf(write_fp, "%.2f\t%.2f\n",rec.x, rec.y );
            fflush(write_fp);
        }
    }

    fclose(read_fp);
    fclose(write_fp);
    return 0;
}
