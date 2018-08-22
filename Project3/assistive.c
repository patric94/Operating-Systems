#include "assistive.h"

int isNumber(int len, char * string){       //A fucntion to check if a given string is either an int or a float number.
    int k;
    char* s;
    s = string;
    for (k = 0 ;k < len; k++){
        if(!isdigit(*s)/* && *s != '.'*/){
            return 0;
        }
        s++;
    }
    return 1;
}

void print_Tables(Table * myTables, int numOfTables) {
    for (int i = 0; i < numOfTables; i++) {
        fprintf(stderr, "table %d : with %d, %d, %d, %d, %d, %d\n", i, myTables[i].Capacity, myTables[i].CustomerID, myTables[i].Occupied, myTables[i].WaiterID, myTables[i].WishToPay, myTables[i].WishToOrder);
    }
}
/*-------------------------Doorman Functions----------------------------------*/

int checking_tables(Table *myTables, int numOfTables, int group){
    for (int i = 0; i < numOfTables; i++) {
        if (group <= myTables[i].Capacity && !(myTables[i].Occupied)) {
            return i;
        }
    }
    return -1;
}

int spaceInBar(Bar *myBar, DtoC *dtc){
    sem_wait(&dtc->modifying_bar);
    fprintf(stderr, "Checking bar for customer %d.\n", dtc->Entrance[0]);
    if (myBar->currSize + dtc->Entrance[1] <= myBar->barSize) {
        return 1;
    }
    else {
        return 0;
    }
}

void placeAtBar(DtoC *dtc, Bar *myBar, Stand *stands){
    sem_wait(&dtc->modifying_bar);
    myBar->currSize += dtc->Entrance[1];
    fprintf(stderr, "Customer : %d will sit at the bar(%d).\n", dtc->Entrance[0], myBar->currSize);
    for (int i = 0; i < myBar->barSize; i++) {
        if (stands[i].CustomerID == 0) { //No one sitting on this stand.
            stands[i].CustomerID = dtc->Entrance[0];
            stands[i].grpPeople = dtc->Entrance[1];
            break;
        }
        else{ //Another customer is sitting in this stand.
            continue;
        }
    }
    sem_post(&dtc->modifying_bar);
}

void remove_from_bar(Bar *myBar, Stand *stands, int id){
    for (int i = 0; i < myBar->barSize; i++) {
        if (stands[i].CustomerID != 0) { //Someone sitting on this stand.
            if (stands[i].CustomerID == id) {
                int k = i;
                myBar->currSize -= stands[i].grpPeople;
                while ( k < myBar->barSize) {
                    if (k+1 != myBar->barSize) {
                        stands[k].CustomerID = stands[k+1].CustomerID;
                        stands[k].grpPeople = stands[k+1].grpPeople;
                        stands[k+1].CustomerID = 0;
                        stands[k+1].grpPeople = 0;
                        k++;
                    }
                    else break;
                }
            }
        }
        else break; //No one is sitting from this stand and beyond.
    }
}

void print_bar(Bar *myBar, Stand *stands){
    for (int i = 0; i < myBar->barSize; i++) {
        fprintf(stderr, "ID : %d kai ATOMA : %d\n", stands[i].CustomerID, stands[i].grpPeople);
    }
}

/*----------------------------------------------------------------------------*/
/*-------------------------Customer Functions---------------------------------*/

void sitting_at_table(Restaurant *myRes, Table *tables, int period, int groupsTable, char *filename){
	FILE *append_fp;
    sem_wait(&myRes->wants_order);
    myRes->Order = 1;
    tables[groupsTable].WishToOrder = 1;
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return;
	}
    fprintf(append_fp, "\t\tCalling a Waiter on table %d with Customer ID : %d.\n", groupsTable, getpid());
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&myRes->waiters_sleep);
    sem_wait(&myRes->take_order);
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return;
	}
    fprintf(append_fp, "\t\t%d : Our order was taken from Waiter : %d.\n", getpid(), tables[groupsTable].WaiterID);
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&myRes->wants_order);
    sleep(rand()%period +1);
    sem_wait(&myRes->wants_bill);
    myRes->Bill = 1;
    tables[groupsTable].WishToPay = 1;
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return;
	}
    fprintf(append_fp, "\t\t%d : We finished. We want the check from Waiter %d.\n", getpid(), tables[groupsTable].WaiterID);
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&myRes->waiters_sleep);
    sem_wait(&myRes->make_check);
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return;
	}
    fprintf(append_fp, "\t\t%d : We had a lovely night!\n", getpid());
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sem_post(&myRes->wants_bill);
    return;
}


void waiting_at_bar(Restaurant *myRes, Table *tables, int period, Bar *myBar, sem_t *myQueue, DtoC *dtc, Stand *stands, char *filename){
	FILE *append_fp;
    sem_wait(myQueue);
    sem_wait(&dtc->bar_answer); //enimerono doorman oti diabasa to pou kathomai.
    int mytable = dtc->barAnswer;
	// sem_wait(&myRes->tables_free);
	sem_wait(&myRes->append_fp);
	if ((append_fp = fopen(filename, "a")) == 0) {
		fprintf(append_fp,"Error ! Can't open append file! \n");
		fflush(append_fp);
		return;
	}
	fprintf(append_fp, "\t\tThank you mister Doorman for the table. ID : %d\n", getpid());
	fflush(append_fp);
	fclose(append_fp);
	sem_post(&myRes->append_fp);
    sitting_at_table(myRes, tables, period, mytable, filename);
}

/*----------------------------------------------------------------------------*/
/*-------------------------Waiter Functions-----------------------------------*/

void receving_payment(Restaurant *myRes, Table *tables, int period, int moneyAmount, char *filename) {
	FILE *append_fp;
    sem_wait(&myRes->modifying_tables);
    for (int i = 0; i < myRes->numOfTables; i++) {
        if (tables[i].Occupied) {
            if (tables[i].WishToPay) {
                if (tables[i].WaiterID == getpid()) {
                    for (int k = 0; k < myRes->wrongWaiter; k++) {
                        sem_post(&myRes->calling_colleague);
						sem_wait(&myRes->append_fp);
						if ((append_fp = fopen(filename, "a")) == 0) {
							fprintf(append_fp,"Error ! Can't open append file! \n");
							fflush(append_fp);
							return;
						}
                        fprintf(append_fp, "\tThank you calling_colleague %d \n", k);
						fflush(append_fp);
						fclose(append_fp);
						sem_post(&myRes->append_fp);
                    }
                    myRes->wrongWaiter = 0;
                    myRes->Bill = 0;
                    tables[i].WishToPay = 0;
					sem_wait(&myRes->append_fp);
					if ((append_fp = fopen(filename, "a")) == 0) {
						fprintf(append_fp,"Error ! Can't open append file! \n");
						fflush(append_fp);
						return;
					}
                    fprintf(append_fp, "\tWaiter %d need %d gold, %d silver and %d copper for table's %d order with Customer : %d.\n", getpid(), rand()%moneyAmount + 1, rand()%100+1, rand()%100+1, i, tables[i].CustomerID);
					fflush(append_fp);
					fclose(append_fp);
					sem_post(&myRes->append_fp);
                    sleep(rand()%period + 1);
                    tables[i].WaiterID = -1;
                    sem_post(&myRes->make_check);
                    tables[i].CustomerID = -1;
                    tables[i].Occupied = 0;
					myRes->Left++;
					sem_post(&myRes->tables_free);
                    sem_post(&myRes->modifying_tables);
                    return; //To make it realistic and go back and put the money in the deposit box.
                }
                else {
                    sem_post(&myRes->waiters_sleep);
					sem_wait(&myRes->append_fp);
					if ((append_fp = fopen(filename, "a")) == 0) {
						fprintf(append_fp,"Error ! Can't open append file! \n");
						fflush(append_fp);
						return;
					}
                    fprintf(append_fp, "\tGonna call the right one with %d.\n", tables[i].WaiterID);
					fflush(append_fp);
					fclose(append_fp);
					sem_post(&myRes->append_fp);
                    myRes->wrongWaiter++;
					sem_post(&myRes->modifying_tables);
                    sem_wait(&myRes->calling_colleague);
                    return;
                }
            }
        }
    }
}

void placing_order(Restaurant *myRes, Table *tables, int period, char *filename){
	FILE *append_fp;
    sem_wait(&myRes->modifying_tables);
    for (int i = 0; i < myRes->numOfTables; i++) {
        if (tables[i].Occupied) {
            if (tables[i].WishToOrder) {
                tables[i].WaiterID = getpid();
                tables[i].WishToOrder = 0;
                myRes->Order = 0;
                sem_post(&myRes->modifying_tables);
				sem_wait(&myRes->append_fp);
				if ((append_fp = fopen(filename, "a")) == 0) {
					fprintf(append_fp,"Error ! Can't open append file! \n");
					fflush(append_fp);
					return;
				}
                fprintf(append_fp, "\tWaiter %d taking order from table %d with Customer %d\n", getpid(), i, tables[i].CustomerID);
				fflush(append_fp);
				fclose(append_fp);
				sem_post(&myRes->append_fp);
                sleep(rand()%period + 1);
                sem_post(&myRes->take_order);
                return; //To make it realistic that he goes back to the kitchen to place the order.
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
