#include "assistive.h"

int main(int argc, char *argv[]) {
    int moneyAmount, period, id, errDT;
    void *restaurant;
	char *filename;
    Restaurant *myRes;
	FILE *append_fp;
    Table *tables;


    srand(getpid());

    if ( argc != 9) {
        fprintf(stderr, "W Error ! Wrong input of arguments\n");
        return 1;
    }

/*---------THERE IS A -f ARGUMENT ON COMMAND LINE !! DO NOT FORGET !!---------*/


    for (int i = 1; i < argc; i+=2) {
        if ( strcmp(argv[i], "-m") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "W Error ! Amount of money must be an integer.\n");
                return 1;
            }
            moneyAmount = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-d") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "W Error ! Amount of period must be an integer.\n");
                return 1;
            }
            period = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-s") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "W Error ! The id of shared memory must be an integer.\n");
                return 1;
            }
            id = atoi(argv[i+1]);
        }
		else if ( strcmp(argv[i], "-f") == 0 ) {
			filename = argv[i+1];
        }
        else {
            fprintf(stderr, "W Error ! Unacceptable in-line argument! %s\n", argv[i]);
            return 1;
        }
    }

    if ((restaurant = shmat(id,(void*) 0, 0)) == (void*)-1) {
        perror("Attachment Waiter");
        return 1;
    }
    myRes = (Restaurant *)restaurant;
    tables = (Table *)((void*)restaurant + sizeof(Restaurant));

    do {
		sem_wait(&myRes->append_fp);
		if ((append_fp = fopen(filename, "a")) == 0) {
			fprintf(append_fp,"Error ! Can't open append file! \n");
			fflush(append_fp);
			return 1;
		}
		fprintf(append_fp, "\tWaiter with ID : %d is in the kitchen.\n", getpid());
		fflush(append_fp);
		fclose(append_fp);
		sem_post(&myRes->append_fp);
        sem_wait(&myRes->waiters_sleep);
		sem_wait(&myRes->append_fp);
		if ((append_fp = fopen(filename, "a")) == 0) {
			fprintf(append_fp,"Error ! Can't open append file! \n");
			fflush(append_fp);
			return 1;
		}
        fprintf(append_fp, "\tWaiter : %d gonna start working.\n", getpid());
		fflush(append_fp);
		fclose(append_fp);
		sem_post(&myRes->append_fp);
        if (myRes->Bill) {
            receving_payment(myRes, tables, period, moneyAmount, filename);
        }
        else if (myRes->Order) {
            placing_order(myRes, tables, period, filename);
        }
    } while (myRes->Left < myRes->OnTable);

	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return 1;
	}
    fprintf(append_fp, "\tWaiter %d HAD A VERY PLEASANT DAY !\n", getpid());
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&myRes->waiters_sleep);


    errDT = shmdt((void *) restaurant);
    if (errDT == -1) {
        perror ("Waiter Detachment.");
        exit(1);
    }

    return 0;
}
