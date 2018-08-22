#include "assistive.h"

#define SEGMENTPERM 0666

int main(int argc, char *argv[]) {
    int customersAmount, period, restaurantID, err, errDT, moneyAmount, numOfTables, barSize;
    char buffer[200], d_waiter[5], d_doorman[5], d_customer[5], *filename;
    FILE *read_fp;
    void *restaurant;
    Table *tables;
    Restaurant *myRes;
    Bar *myBar;

/*---------THERE IS A -b ARGUMENT ON COMMAND LINE !! DO NOT FORGET !!---------*/
/*---------THERE IS A -f ARGUMENT ON COMMAND LINE !! DO NOT FORGET !!---------*/


    if ( argc != 11) {
        fprintf(stderr, "Error ! Wrong input of arguments\n");
        return 1;
    }

    for (int i = 1; i < argc; i+=2) {
        if ( strcmp(argv[i], "-n") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "Error ! Amount of money must be an integer.\n");
                return 1;
            }
            customersAmount = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-l") == 0 ) {
            if ((read_fp = fopen(argv[i+1], "r")) == 0) {
                fprintf(stderr,"Error ! Can't open config file! \n");
                return 1;
            }
        }
        else if ( strcmp(argv[i], "-d") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "Error ! The amount of time must be an integer.\n");
                return 1;
            }
            period = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-b") == 0 ) {
            if (!isNumber(strlen(argv[i+1]), argv[i+1])) {
                fprintf(stderr, "Error ! Amount of barSize must be an integer.\n");
                return 1;
            }
            barSize = atoi(argv[i+1]);
        }
		else if ( strcmp(argv[i], "-f") == 0 ) {
			filename = argv[i+1];
			fprintf(stderr, "%s\n", filename);
        }
        else {
            fprintf(stderr, "Error ! Unacceptable in-line argument! %s\n", argv[i]);
            return 1;
        }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Getting the information needed from the configuration file.
    while (fgets(buffer, sizeof(buffer), read_fp) != NULL) {
        char* token;
        char* newline = strchr(buffer, '\n' ); //getting rid of newline character
        *newline = 0;
        token = strtok(buffer," ");
        if ( strcmp(token, "t") == 0 ) {//initializing the restaurant's tables.
            token = strtok(NULL, " ,");
            numOfTables = atoi(token);
            //Creating a shared memory segment for the Restaurant.
            restaurantID = shmget(IPC_PRIVATE, (sizeof(Restaurant) + (numOfTables*sizeof(Table)) + sizeof(DtoC) + sizeof(Bar) + (barSize * sizeof(Stand))), SEGMENTPERM);
            if (restaurantID == -1) {
                perror("Restaurant Creation");
                return 1;
            }
            else {
                fprintf(stderr, "Allocated %d\n", restaurantID);
            }
            //Attaching to the memory segment
            if ((restaurant = shmat(restaurantID,(void*) 0, 0)) == (void*)-1) {
                perror("Attachment Restaurant");
                return 1;
            }
            myRes = (Restaurant *)restaurant;
            myRes->numOfTables = numOfTables;
            myRes->Customers = customersAmount;
            myRes->Bill = 0;
            myRes->Order = 0;
            myRes->wrongWaiter = 0;
			myRes->OnTable = 0;
			myRes->Left = 0;
            tables = (Table *)((void *)restaurant + sizeof(Restaurant));
            myBar = (Bar *)((void*)restaurant + sizeof(Restaurant) + (myRes->numOfTables*sizeof(Table)) + sizeof(DtoC));
            myBar->barSize = barSize;
            myBar->currSize = 0;

            for (int i = 0; i < myRes->numOfTables; i++) {
                token = strtok(NULL, ", ");
                tables[i].Capacity = atoi(token);
                tables[i].CustomerID = -1;
                tables[i].Occupied = 0;
                tables[i].WaiterID = -1;
                tables[i].WishToPay = 0;
                tables[i].WishToOrder = 0;
            }
        }
        else if ( strcmp(token, "w") == 0) {//initializing the restaurant's waiters.
            token = strtok(NULL, ", ");
            myRes->Waiters = atoi(token);
            token = strtok(NULL, " ,");
            moneyAmount = atoi(token);
        }
        else if ( strcmp(token, "d") == 0 ) {//Initializing each program's period time.
            token = strtok(NULL, " ,");
            strncpy(d_customer, token, 5);
            token = strtok(NULL, " ,");
            strncpy(d_waiter, token, 5);
            token = strtok(NULL, " ");
            strncpy(d_doorman, token, 5);
        }
        else if ( strcmp(token, "e") == 0 ) { //End of configuration inputs. Comments exist in the config.txt
            break;
        }
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // print_Tables(tables, myRes->numOfTables);
    // fprintf(stderr, "Bar's capacity %d total Waiters %d\n", myRes->Bar, myRes->Waiters);
    //Initializing the semaphores.

    if ( sem_init(&myRes->waiters_sleep, 1, 0) != 0 ) {
		perror("Couldn't initialize Semaphore 5.");
		return 1;
	}

    if ( sem_init(&myRes->take_order, 1, 0) != 0 ) {
		perror("Couldn't initialize Semaphore 6.");
		return 1;
	}

    if ( sem_init(&myRes->make_check, 1, 0) != 0 ) {
		perror("Couldn't initialize Semaphore 7.");
		return 1;
	}

    if ( sem_init(&myRes->wants_bill, 1, 1) != 0 ) {
		perror("Couldn't initialize Semaphore 8.");
		return 1;
	}

    if ( sem_init(&myRes->calling_colleague, 1, 0) != 0 ) {
		perror("Couldn't initialize Semaphore 9.");
		return 1;
	}

    if ( sem_init(&myRes->wants_order, 1, 1) != 0 ) {
		perror("Couldn't initialize Semaphore 10.");
		return 1;
	}

    if ( sem_init(&myRes->modifying_tables, 1, 1) != 0 ) {
		perror("Couldn't initialize Semaphore 17.");
		return 1;
	}

	if ( sem_init(&myRes->tables_free, 1, numOfTables) != 0 ) {
		perror("Couldn't initialize Semaphore 18.");
		return 1;
	}

	if ( sem_init(&myRes->append_fp, 1, 1) != 0 ) {
		perror("Couldn't initialize Semaphore 19.");
		return 1;
	}

    //Starting the call of the other executables.
    int doorman_id, waiter_id[myRes->Waiters], customer_id[customersAmount];
    char segment_id[10], money[10];

    sprintf(segment_id, "%d", restaurantID);
    sprintf(money, "%d", moneyAmount);

    if ( (doorman_id = fork()) == -1 ) {
        perror("Fork Error on Doorman");
        return 1;
    }
    else if ( doorman_id == 0 ) {//Creating the doorman.
        execlp("./doorman", "./doorman", "-d", d_doorman, "-s", segment_id, "-f", filename, NULL);
    }

    for (int i = 0; i < myRes->Waiters; i++) {
        if ( (waiter_id[i] = fork()) == -1 ) {
            perror("Fork Error on Waiter");
            return 1;
        }
        else if ( waiter_id[i] == 0) {
            execlp("./waiter", "./waiter", "-d", d_waiter, "-m", money, "-s", segment_id, "-f", filename, NULL);
        }
    }


    char cust[2];
    for (int i = 0; i < customersAmount; i++) {
        if ( (customer_id[i] = fork()) == -1 ) {
            perror("Fork Error on Customer");
            return 1;
        }
        else if ( customer_id[i] == 0 ) {
            srand(getpid());
            sprintf(cust, "%d", rand()%8+1);
            execlp("./customer", "./customer", "-d", d_customer, "-n", cust, "-s", segment_id, "-f", filename, NULL);
        }
    }

    printf("WAITING CUSTOMERS\n");

    for (int i = 0; i <customersAmount; i++) {
        int status;
        waitpid(customer_id[i], &status, 0);
    }

    printf("CUSTOMERS DONE\n");



    for (int i = 0; i <myRes->Waiters; i++) {
        int status;
        waitpid(waiter_id[i], &status, 0);
    }

    printf("CLOSING THE RESTAURANT\n");

    /*Customer and Waiter Communication Semaphores*/
    sem_destroy(&myRes->waiters_sleep);
    sem_destroy(&myRes->take_order);
    sem_destroy(&myRes->make_check);
    sem_destroy(&myRes->wants_bill);
    sem_destroy(&myRes->calling_colleague);
    sem_destroy(&myRes->wants_order);
    sem_destroy(&myRes->modifying_tables);
	sem_destroy(&myRes->append_fp);

    //Detaching from the shared memory segment.
    errDT = shmdt((void *) restaurant);
    if (errDT == -1) {
        perror ("Restaurant Detachment.");
        return 1;
    }

    //Removing the shared memory segment.
    err = shmctl(restaurantID, IPC_RMID, 0);
	if (err == -1) {
        perror("Restaurant Removal.");
        return 1;
    }
	else {
        fprintf(stderr, "Removed Restaurant. %d\n",err);
    }

    fclose(read_fp);
    return 0;
}
