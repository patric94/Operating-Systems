#include "assistive.h"

int main(int argc, char *argv[]) {
    struct stat st = {0};
    int i, workers, BinSize, numOfrecords, wmod, points;
    int flagI, flagW, flagD; // Flags in order to check which operation characters have been put as input.
    int circle_hdl;
    int queries = 0;
    flagI = flagW = flagD = 0;
    char buffer[200], *dir; // String that keeps the current working directory and a buffer for the queries.
    char *filename;
    FILE *read_fp;
    MyRecord rec;



    if ( argc != 7 ) {
        fprintf(stderr, "Error ! Wrong input of arguments\n");
        return 1;
    }
    for (i = 1; i < argc; i+= 2) { //A 'for' loop in order to check the in-line arguments.
        if ( strcmp(argv[i], "-i") == 0 ) {
            flagI = 1;
            if ((read_fp = fopen(argv[i+1], "rb")) == 0) {
                fprintf(stderr,"Error ! Can't open input file! \n");
                fclose(read_fp);
                return 1;
            }
            filename = argv[i+1];
            fseek (read_fp , 0 , SEEK_END);
            BinSize = ftell (read_fp);
            rewind (read_fp);
            numOfrecords = (int) BinSize/sizeof(rec);
        }
        else if ( strcmp(argv[i], "-w") == 0 ) {
            if ( !isNumber(strlen(argv[i+1]), argv[i+1]) ) {
                fprintf(stderr, "Error ! Workers count must be an integer value !\n");
                return 1;
            }
            flagW = 1;
            workers = atoi(argv[i+1]);
        }                                               //In this argument I expect a name of the folder to be created
        else if ( strcmp(argv[i], "-d") == 0 ) {         //so i dont accept the absolute path.
            if (( dir = (char *) malloc(256*sizeof(char))) == NULL) {
                fprintf(stderr, "Error ! Not enough memory for current directory!\n");
                return 0;
            }//Allocating enough space for both currnet path and the temp directory and the names of fifo files.
            getcwd(dir, 256);
            strcat(dir, "/");
            strcat(dir, argv[i+1]);
            if ( stat(dir, &st) == -1 ) {
                if (mkdir(dir, 0700) != 0) {
                    perror("mkdir Error!");
                }
            }
            strcat(dir, "/");
            flagD = 1;
        }
        else {
            fprintf(stderr, "Error ! Unacceptable in-line argument!\n");
            return 1;
        }
    }
    if ( !flagD || !flagI || !flagW ) {
        fprintf(stderr, "Error ! Wrong input of arguments ! Missing a mandatory one !\n");
        return 1;
    }
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        int *hdls = NULL;
        char **colours = NULL;
        queries++;
        int i, failure = 0;
        char *token;
        char delim[200];
        strncpy(delim, buffer, 200);
        char* newline = strchr(buffer, '\n' ); //getting rid of newline character
        *newline = 0;
        wmod = numOfrecords % workers;
        points = numOfrecords / workers;
        token = strtok(buffer, " ,;");
        if (token == NULL) { //If just the enter is given as a query.
            continue;
        }
        if (strcmp(token, "exit") == 0){ // Program is going to exit.
            break;
        }
        int handlersAmount = 0;
        int circle_hdl ,semicircle_hdl, ring_hdl, square_hdl, ellipse_hdl;
        do {
            if (strcmp(token, "circle") == 0) { // Program chosen = Circle.
                handlersAmount++;
                char *colour, *x, *y, *r;
                token = strtok(NULL, " ,;");
                for (i = 0; i < 4; i++) {
                    if (i == 0) { //We check the value to be valid but we keep it as a String for later.
                        x = token;
                    } else if (i == 1) {
                        y = token;
                    } else if (i == 2) {
                        r = token;
                    } else {
                        colour = token;
                    }
                    token = strtok(NULL, " ,;");
                }
                FILE *circle_out_fp;
                if ((circle_hdl = fork()) == -1) {
                    perror("Fork Error on Handler");
                    return 1;
                } else if (circle_hdl == 0) { //Creating the circle's query.
                    answering_query(x, y, r, NULL, workers, points, wmod, "/circle", "./circle", dir, filename, getpid(), circle_out_fp);
                    free(hdls);
                    free(dir);
                    fclose(read_fp);
                    exit(1);
                } else {
                    fprintf(stderr, "I am the MASTER with id %d and CHILD's id %d who's executing a Circle Query. \n",getpid(), circle_hdl );
                    if ((hdls = (int *)realloc(hdls, handlersAmount*sizeof(int))) == NULL) {
                        free(hdls);
                        perror("Error (re)allocating memory");
                        exit(1);
                    }
                    hdls[handlersAmount-1] = circle_hdl;
                    if ((colours = (char **)realloc(colours, handlersAmount*sizeof(char*))) == NULL) {
                        free(colours);
                        perror("error on allocating colours table");
                        exit(1);
                    }
                    colours[handlersAmount-1] =  colour;
                }
            } else if (strcmp(token, "semicircle") == 0) {
                handlersAmount++;
                char *x, *y, *r, *pos, *colour;
                token = strtok(NULL, " ,;");
                for (i = 0; i < 5; i++) {
                    if (i == 0) { //We check the value to be valid but we keep it as a String for later.
                        x = token;
                    } else if (i == 1) {
                        y = token;
                    } else if (i == 2) {
                        r = token;
                    } else if (i == 3) {
                        pos = token;
                    } else {
                        colour = token;
                    }
                    token = strtok(NULL, " ,;");
                }
                FILE *semicircle_out_fp;
                if ((semicircle_hdl = fork()) == -1) {
                    perror("Fork Error on Handler");
                    return 1;
                } else if (semicircle_hdl == 0) { //Creating the semicircle's query.
                    answering_query(x, y, r, pos, workers, points, wmod, "/semicircle", "./semicircle", dir, filename, getpid(), semicircle_out_fp);
                    free(hdls);
                    free(dir);
                    fclose(read_fp);
                    exit(1);
                } else {
                    fprintf(stderr, "I am the MASTER with id %d and CHILD's id %d who's executing a Semicircle Query. \n",getpid(), semicircle_hdl );
                    if ((hdls = (int *)realloc(hdls, handlersAmount*sizeof(int))) == NULL) {
                        free(hdls);
                        free(dir);
                        fclose(read_fp);
                        perror("Error (re)allocating memory");
                        exit(1);
                    }
                    hdls[handlersAmount-1] = semicircle_hdl;
                    if ((colours = (char **)realloc(colours, handlersAmount*sizeof(char*))) == NULL) {
                        perror("error on allocating colours table");
                        exit(1);
                    }
                    colours[handlersAmount-1] =  colour;
                }
            } else if (strcmp(token, "ring") == 0) {
                handlersAmount++;
                char *x, *y, *r1, *r2, *colour;
                token = strtok(NULL, " ,;");
                for (i = 0; i < 5; i++) {
                    if (i == 0) { //We check the value to be valid but we keep it as a String for later.
                        x = token;
                    } else if (i == 1) {
                        y = token;
                    } else if (i == 2) {
                        r1 = token;
                    } else if (i == 3) {
                        r2 = token;
                    } else {
                        colour = token;
                    }
                    token = strtok(NULL, " ,;");
                }
                FILE *ring_out_fp;
                if ((ring_hdl = fork()) == -1) {
                    perror("Fork Error on Handler");
                    return 1;
                } else if (ring_hdl == 0) { //Creating the ring's query.
                    answering_query(x, y, r1, r2, workers, points, wmod, "/ring", "./ring", dir, filename, getpid(), ring_out_fp);
                    free(hdls);
                    free(dir);
                    fclose(read_fp);
                    exit(1);
                } else {
                    fprintf(stderr, "I am the MASTER with id %d and CHILD's id %d who's executing a Ring Query. \n",getpid(), ring_hdl );
                    if ((hdls = (int *)realloc(hdls, handlersAmount*sizeof(int))) == NULL) {
                        free(hdls);
                        perror("Error (re)allocating memory");
                        exit(1);
                    }
                    hdls[handlersAmount-1] = ring_hdl;
                    if ((colours = (char **)realloc(colours, handlersAmount*sizeof(char*))) == NULL) {
                        perror("error on allocating colours table");
                        exit(1);
                    }
                    colours[handlersAmount-1] =  colour;
                }
            } else if (strcmp(token, "square") == 0) {
                handlersAmount++;
                char *x, *y, *r, *colour;
                token = strtok(NULL, " ,;");
                for (i = 0; i < 4; i++) {
                    if (i == 0) { //We check the value to be valid but we keep it as a String for later.
                        x = token;
                    } else if (i == 1) {
                        y = token;
                    } else if (i == 2) {
                        r = token;
                    } else {
                        colour = token;
                    }
                    token = strtok(NULL, " ,;");
                }
                FILE *square_out_fp;
                if ((square_hdl = fork()) == -1) {
                    perror("Fork Error on Handler");
                    return 1;
                } else if (square_hdl == 0) { //Creating the square's query.
                    answering_query(x, y, r, NULL, workers, points, wmod, "/square", "./square", dir, filename, getpid(), square_out_fp);
                    free(hdls);
                    free(dir);
                    fclose(read_fp);
                    exit(1);
                } else {
                    fprintf(stderr, "I am the MASTER with id %d and CHILD's id %d who's executing a Square Query. \n",getpid(), square_hdl );
                    if ((hdls = (int *)realloc(hdls, handlersAmount*sizeof(int))) == NULL) {
                        free(hdls);
                        perror("Error (re)allocating memory");
                        exit(1);
                    }
                    hdls[handlersAmount-1] = square_hdl;
                    if ((colours = (char **)realloc(colours, handlersAmount*sizeof(char*))) == NULL) {
                        perror("error on allocating colours table");
                        exit(1);
                    }
                    colours[handlersAmount-1] =  colour;
                }
            } else if (strcmp(token, "ellipse") == 0) {
                handlersAmount++;
                char *h, *k, *a, *b, *colour;
                token = strtok(NULL, " ,;");
                for (i = 0; i < 5; i++) {
                    if (i == 0) { //We check the value to be valid but we keep it as a String for later.
                        h = token;
                    } else if (i == 1) {
                        k = token;
                    } else if (i == 2) {
                        a = token;
                    } else if (i == 3) {
                        b = token;
                    } else {
                        colour = token;
                    }
                    token = strtok(NULL, " ,;");
                }
                FILE *ellipse_out_fp;
                if ((ellipse_hdl = fork()) == -1) {
                    perror("Fork Error on Handler");
                    return 1;
                } else if (ellipse_hdl == 0) { //Creating the ellipse's query.
                    answering_query(h, k, a, b, workers, points, wmod, "/ellipse", "./ellipse", dir, filename, getpid(), ellipse_out_fp);
                    free(hdls);
                    free(dir);
                    fclose(read_fp);
                    exit(1);
                } else {
                    fprintf(stderr, "I am the MASTER with id %d and CHILD's id %d who's executing a Ellipse Query. \n",getpid(), ellipse_hdl );
                    if ((hdls = (int *)realloc(hdls, handlersAmount*sizeof(int))) == NULL) {
                        free(hdls);
                        perror("Error (re)allocating memory");
                        exit(1);
                    }
                    hdls[handlersAmount-1] = ellipse_hdl;
                    if ((colours = (char **)realloc(colours, handlersAmount*sizeof(char*))) == NULL) {
                        perror("error on allocating colours table");
                        exit(1);
                    }
                    colours[handlersAmount-1] =  colour;
                }
            } else {
                fprintf(stderr, "Error ! %s is not an acceptable command !\n", token);
                failure = 1;
                break;
            }
        } while(token != NULL && delim[buffer-token+strlen(token)] != ';'); // to check for more in-line queries
        if (failure) {
            fprintf(stderr, "Please re-enter a valid query\n");
            continue;
        }
        char *scriptdest;
        char *scriptname;
        FILE *script_fp;
        if ((scriptname = (char *) malloc(20*sizeof(char))) == NULL) {
            perror("Error on memory allocation");
            exit(1);
        }
        sprintf(scriptname, "%d_script.gnuplot", queries);
        if ((scriptdest = (char*) malloc(256*sizeof(char))) == NULL) {
            perror("Error on memory allocation");
            exit(1);
        }
        strcpy(scriptdest, dir);
        strcat(scriptdest, scriptname);
        if ((script_fp = fopen(scriptdest, "w")) == 0) {
            perror("Cannot open gnuplot script");
            exit(1);
        }
        fprintf(script_fp, "set terminal png\n");
        fprintf(script_fp, "set size ratio -1\n");
        fprintf(script_fp, "set output \"%d_image.png\"\n", queries); //the output pngs will be stored on the parent folder of TempDir
        for (i = 0; i < handlersAmount; i++) {
            if (i == 0) {
                fprintf(script_fp, "plot \"%s%d.out\" notitle with points pointsize 0.2 linecolor rgb \"%s\"", dir, hdls[i], colours[i]);
            } else {
                fprintf(script_fp, ", \\\n\"%s%d.out\" notitle with points pointsize 0.2 linecolor rgb \"%s\"", dir, hdls[i], colours[i]);
            }
        }
        fclose(script_fp);
        free(scriptname);
        for (i = 0; i < handlersAmount; i++) {  //Waiting for all handlers to finish.
            int status;
            waitpid(hdls[i], &status, WIFCONTINUED(status));
        }
        int gnu;
        fprintf(stderr, "Executing Gnuplot for query number : %d\n", queries);
        if ((gnu = fork()) == -1) {
            perror("Fork Error on Gnuplot");
            exit(1);
        } else if (gnu == 0) {
            execlp("gnuplot", "gnuplot", scriptdest, NULL);
        }
        free(hdls);
        free(scriptdest);
        free(colours);
        fflush(stderr);
    }
    int rm;
    if ((rm = fork()) == -1) { // Deleting folder and all of its contents.
        perror("Fork Error for removing directory");
        exit(1);
    } else if (rm == 0) {
        execlp("rm", "rm", "-r", dir, NULL);
    }
    free(dir);
    fclose(read_fp);
    return 0;
}
