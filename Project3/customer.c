#include "assistive.h"



int main(int argc, char *argv[]) {
    int people, period, id, errDT, groupsTable;
    void *restaurant;
	char *filename;
    Restaurant *myRes;
    Table *tables;
    DtoC *dtc;
    Bar *myBar;
    Stand *stands;
	FILE *append_fp;

    // ts.tv_sec = rand()%30+1; //random amount of seconds that customer  will wait if seated on the bar.

    srand(getpid());

/*---------THERE IS A -f ARGUMENT ON COMMAND LINE !! DO NOT FORGET !!---------*/


    if ( argc != 9) {
        fprintf(stderr, "C Error ! Wrong input of arguments\n");
        return 1;
    }

    for (int i = 1; i < argc; i+=2) {
        if ( strcmp(argv[i], "-n") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "C Error ! Amount of customers must be an integer.\n");
                return 1;
            }
            people = atoi(argv[i+1]);
            if( people < 1 || people > 8){
                fprintf(stderr,"C Error ! Wrong input of customers.\n");
                return 1;
            }
        }
        else if ( strcmp(argv[i], "-d") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "C Error ! Amount of period must be an integer.\n");
                return 1;
            }
            period = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-s") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "C Error ! The id of shared memory must be an integer.\n");
                return 1;
            }
            id = atoi(argv[i+1]);
        }
		else if ( strcmp(argv[i], "-f") == 0 ) {
			filename = argv[i+1];
        }
        else {
            fprintf(stderr, "C Error ! Unacceptable in-line argument! %s\n", argv[i]);
            return 1;
        }
    }


    fprintf(stdout, "\t\tin Customer with ID : %d\n", getpid());
    if ((restaurant = shmat(id,(void*) 0, 0)) == (void*)-1) {
        perror("\t\tAttachment Customer");
        return 1;
    }
    myRes = (Restaurant *)restaurant;
    tables =  (Table *)((void *)restaurant + sizeof(Restaurant));
    dtc = (DtoC *)((void*)restaurant + sizeof(Restaurant) + (myRes->numOfTables*sizeof(Table)));
    myBar = (Bar *)((void*)restaurant + sizeof(Restaurant) + (myRes->numOfTables*sizeof(Table)) + sizeof(DtoC));
    stands = (Stand *)(myBar + sizeof(Bar));

	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return 1;
	}
    fprintf(append_fp, "\t\tCustomer %d : came to the queue of the Restaurant \n", getpid());
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_wait(&dtc->customer_queue);
	dtc->Entrance[0] = getpid();
	dtc->Entrance[1] = people; //Informing the doorman about the number of the group
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return 1;
	}
    fprintf(append_fp, "\t\tCustomerID = %d is at Restaurant's Entrance\n", getpid());
    fprintf(append_fp, "\t\t%d : We are a group of %d people\n", dtc->Entrance[0], dtc->Entrance[1]);
    fprintf(append_fp, "\t\tRequesting a table from the Doorman\n");
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&dtc->customer_request);
    sem_wait(&dtc->doorman_answer); //Waiting for doormans answer.
    if (dtc->doorAnswer >= 0) {                                      /*A table was found.*/
        groupsTable = dtc->doorAnswer;
        dtc->custAnswer = 0;
        dtc->Entrance[0] = 0;
        dtc->Entrance[1] = 0;
        sem_post(&dtc->customer_answer);
		sem_wait(&myRes->tables_free);
        sitting_at_table(myRes, tables, period, groupsTable, filename);
    }
    else if (dtc->doorAnswer == -2) { //there is space at the bar. randomly choosing if we sit or not.
        dtc->custAnswer = rand()%2 + 1;
        if (dtc->custAnswer == 1) { //We do not want to sit at the bar.
			sem_wait(&myRes->append_fp);
			if ((append_fp = fopen(filename, "a")) == 0) {
				fprintf(append_fp,"Error ! Can't open append file! \n");
				fflush(append_fp);
				return 1;
			}
            fprintf(append_fp, "\t\tWe, Customer : %d, are not going to wait at the bar. Have a nice evening!\n", getpid());
			fflush(append_fp);
			fclose(append_fp);
			sem_post(&myRes->append_fp);
            dtc->Entrance[0] = 0;
            dtc->Entrance[1] = 0;
            sem_post(&dtc->customer_answer);
        }
        else{ //We will wait at the bar for some time until a spot if found.
			sem_wait(&myRes->append_fp);
			if ((append_fp = fopen(filename, "a")) == 0) {
				fprintf(append_fp,"Error ! Can't open append file! \n");
				fflush(append_fp);
				return 1;
			}
            fprintf(append_fp, "\t\tWe, Customer : %d, are going to wait at the bar. Let us know for any further detail.\n", getpid());
			fflush(append_fp);
			fclose(append_fp);
			sem_post(&myRes->append_fp);
            sem_post(&dtc->customer_answer);
            if (people <= 2) {
                waiting_at_bar(myRes, tables, period, myBar, &dtc->bar2queue, dtc, stands, filename);
            }
            else if (people <= 4) {
                waiting_at_bar(myRes, tables, period, myBar, &dtc->bar4queue, dtc, stands, filename);
            }
            else if (people <= 6) {
                waiting_at_bar(myRes, tables, period, myBar, &dtc->bar6queue, dtc, stands, filename);
            }
            else {
                waiting_at_bar(myRes, tables, period, myBar, &dtc->bar8queue, dtc, stands, filename);
            }
        }
    }
    else {                                                     /*No table was found in either bar or restaurant*/
		sem_wait(&myRes->append_fp);
		if ((append_fp = fopen(filename, "a")) == 0) {
			fprintf(append_fp,"Error ! Can't open append file! \n");
			fflush(append_fp);
			return 1;
		}
        fprintf(append_fp, "\t\tSadly no table for %d with %d people.\n", dtc->Entrance[0], dtc->Entrance[1]);
		fflush(append_fp);
		fclose(append_fp);
		sem_post(&myRes->append_fp);
        dtc->custAnswer = -1;
        dtc->Entrance[0] = 0;
        dtc->Entrance[1] = 0;
        sem_post(&dtc->customer_answer);
    }


    errDT = shmdt((void *) restaurant);
    if (errDT == -1) {
        perror ("\t\tCustomer Detachment.");
        exit(1);
    }

    exit(0);
}
