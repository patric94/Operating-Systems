#include "assistive.h"

int main(int argc, char *argv[]) {
    int period, id, errDT, designated_table, count = 0;
    void *restaurant;
	char *filename;
    Restaurant *myRes;
    Table *tables;
    DtoC *dtc;
    Bar *myBar;
    Stand *stands;
	FILE *append_fp;



/*---------THERE IS A -f ARGUMENT ON COMMAND LINE !! DO NOT FORGET !!---------*/

    srand(getpid());

    if ( argc != 7) {
        fprintf(stdout, "D Error ! Wrong input of arguments\n");
        return 1;
    }

    for (int i = 1; i < argc; i+=2) {
        if ( strcmp(argv[i], "-d") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stdout, "D Error ! Amount of period must be an integer.\n");
                return 1;
            }
            period = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-s") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stdout, "D Error ! The id of shared memory must be an integer.\n");
                return 1;
            }
            id = atoi(argv[i+1]);
        }
		else if ( strcmp(argv[i], "-f") == 0 ) {
			filename = argv[i+1];
        }
        else {
            fprintf(stdout, "D Error ! Unacceptable in-line argument! %s\n", argv[i]);
            return 1;
        }
    }
    fprintf(stdout, "in Doorman with ID : %d\n", getpid());
    if ((restaurant = shmat(id,(void*) 0, 0)) == (void*)-1) {
        perror("Attachment Doorman");
        return 1;
    }
    myRes = (Restaurant *)restaurant;
    tables = (Table *)((void*)restaurant + sizeof(Restaurant));
    dtc = (DtoC *)((void*)restaurant + sizeof(Restaurant) + (myRes->numOfTables*sizeof(Table)));
    myBar = (Bar *)((void*)restaurant + sizeof(Restaurant) + (myRes->numOfTables*sizeof(Table)) + sizeof(DtoC));
    stands = (Stand *)(myBar + sizeof(Bar));

    if ( sem_init(&dtc->customer_queue, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 1.");
        return 1;
    }

    if ( sem_init(&dtc->customer_request, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 2.");
        return 1;
    }

    if ( sem_init(&dtc->doorman_answer, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 3.");
        return 1;
    }

    if ( sem_init(&dtc->customer_answer, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 4.");
        return 1;
    }

    if ( sem_init(&dtc->bar2queue, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 11.");
        return 1;
    }

    if ( sem_init(&dtc->bar4queue, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 12.");
        return 1;
    }

    if ( sem_init(&dtc->bar6queue, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 13.");
        return 1;
    }

    if ( sem_init(&dtc->bar8queue, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 14.");
        return 1;
    }

    if ( sem_init(&dtc->modifying_bar, 1, 1) != 0 ) {
        perror("Couldn't initialize Semaphore 15.");
        return 1;
    }

    if ( sem_init(&dtc->bar_answer, 1, 0) != 0 ) {
        perror("Couldn't initialize Semaphore 16.");
        return 1;
    }

    while((count < myRes->Customers) || (myBar->currSize > 0)){
        if (count == myRes->Customers) {
            //ypparxoun atoma sto bar
			sem_wait(&myRes->tables_free);
        }
        sem_wait(&dtc->modifying_bar);
        if (myBar->currSize != 0) {
			sem_wait(&myRes->append_fp);
			if ((append_fp = fopen(filename, "a")) == 0) {
                fprintf(append_fp,"Error ! Can't open append file! \n");
				fflush(append_fp);
                return 1;
            }
            fprintf(append_fp, "Checking for Customers in Bar if there is a table ready for them.\n");
			fflush(append_fp);
			fclose(append_fp);
			sem_post(&myRes->append_fp);
			print_bar(myBar, stands);
            // sleep(rand()%period+1);
            for (int i = 0; i < myBar->barSize; i++) {
                if (stands[i].CustomerID == 0) { //No one is sitting from this stand and beyond.
                    continue;
                }
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
					fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
					return 1;
				}
                fprintf(append_fp, "Cheking for table, for Customer : %d from Bar.\n", stands[i].CustomerID);
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
                if ( (designated_table = checking_tables(tables, myRes->numOfTables, stands[i].grpPeople)) != -1 ) {
					sem_wait(&myRes->append_fp);
					if ((append_fp = fopen(filename, "a")) == 0) {
		                fprintf(append_fp,"Error ! Can't open append file! \n");
						fflush(append_fp);
		                return 1;
		            }
                    fprintf(append_fp, "Found table for Customer : %d who was sitting at the bar.\n", stands[i].CustomerID);
					fflush(append_fp);
					fclose(append_fp);
					sem_post(&myRes->append_fp);
                    dtc->barAnswer = designated_table;
                    tables[designated_table].CustomerID = stands[i].CustomerID;
                    tables[designated_table].Occupied = 1;
                    if (stands[i].grpPeople <= 2) {
                        sem_post(&dtc->bar2queue);
                    }
                    else if (stands[i].grpPeople <= 4) {
                        sem_post(&dtc->bar4queue);
                    }
                    else if (stands[i].grpPeople <= 6) {
                        sem_post(&dtc->bar6queue);
                    }
                    else {
                        sem_post(&dtc->bar8queue);
                    }
					remove_from_bar(myBar, stands, stands[i].CustomerID);
                    sem_post(&dtc->bar_answer);
					sem_wait(&myRes->append_fp);
					if ((append_fp = fopen(filename, "a")) == 0) {
						fprintf(append_fp,"Error ! Can't open append file! \n");
						fflush(append_fp);
						return 1;
					}
					fprintf(append_fp, "BAR SIZE %d\n", myBar->currSize);
					fprintf(append_fp, "Place %d to the table %d\n", tables[designated_table].CustomerID, designated_table);
					fflush(append_fp);
					fclose(append_fp);
					sem_post(&myRes->append_fp);
                }
                else {
					sem_wait(&myRes->append_fp);
					if ((append_fp = fopen(filename, "a")) == 0) {
		                fprintf(append_fp,"Error ! Can't open append file! \n");
						fflush(append_fp);
		                return 1;
		            }
                    fprintf(append_fp, "No table found for Customer %d\n", stands[i].CustomerID);
					fflush(append_fp);
					fclose(append_fp);
					sem_post(&myRes->append_fp);
                }
            }
        }
        sem_post(&dtc->modifying_bar);

        if (count < myRes->Customers) {
            sem_post(&dtc->customer_queue);
            sem_wait(&dtc->customer_request);// waiting for a customer at entrace.
            //Searching for a table according to the group's number of people.
            // sleep(rand()%period+1);
            if ( (designated_table = checking_tables(tables, myRes->numOfTables, dtc->Entrance[1])) != -1 ) { //found table for the customer group.
				myRes->OnTable++;
                tables[designated_table].CustomerID = dtc->Entrance[0];
                tables[designated_table].Occupied = 1;
                dtc->doorAnswer = designated_table;
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
	                fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
	                return 1;
	            }
                fprintf(append_fp, "Placing Customer ID %d on table %d.\n", dtc->Entrance[0], designated_table);
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
            }
            else if ( spaceInBar(myBar, dtc) ) { //no table but still space in the bar for them to wait.
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
	                fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
	                return 1;
	            }
				fprintf(append_fp, "Found space in Bar\n");
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
                dtc->doorAnswer = -2;
                sem_post(&dtc->modifying_bar);
            }
            else { //No table and no space in bar.
                dtc->doorAnswer = -1;
            }
            sem_post(&dtc->doorman_answer); //Posted my answer.
            sem_wait(&dtc->customer_answer); //Waiting for customer's answer.
            if (dtc->custAnswer == 0) {
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
	                fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
	                return 1;
	            }
				fprintf(append_fp, "Customer sat on the table.\n");
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
            }
            else if (dtc->custAnswer == 2) {
                placeAtBar(dtc, myBar, stands);
                sem_wait(&dtc->modifying_bar);
				myRes->OnTable++;
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
	                fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
	                return 1;
	            }
				fprintf(append_fp, "TON EBALA!\n");
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
                print_bar(myBar, stands);
                sem_post(&dtc->modifying_bar);
            }
            else {
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
	                fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
	                return 1;
	            }
				fprintf(append_fp, "Customer left the restaurant.\n");
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
            }
            sem_wait(&myRes->modifying_tables);
            print_Tables(tables, myRes->numOfTables);
            sem_post(&myRes->modifying_tables);
            count++;
        }
    }


    fprintf(stdout, "Job's Done !!!\n");

    /*Customer and Doorman Communication Semaphores*/
    sem_destroy(&dtc->customer_queue);
    sem_destroy(&dtc->customer_request);
    sem_destroy(&dtc->doorman_answer);
    sem_destroy(&dtc->customer_answer);
    sem_destroy(&dtc->bar2queue);
    sem_destroy(&dtc->bar4queue);
    sem_destroy(&dtc->bar6queue);
    sem_destroy(&dtc->bar8queue);
    sem_destroy(&dtc->bar_answer);

    errDT = shmdt((void *) restaurant);
    if (errDT == -1) {
        perror ("Doorman Detachment.");
        exit(1);
    }
    exit(0);
}
