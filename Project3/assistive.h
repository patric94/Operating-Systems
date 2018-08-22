#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>



//Struct for the linked list of the bar.
typedef struct Stand {
    int grpPeople;
    int CustomerID;
}Stand;

typedef struct Bar {
    int barSize;
    int currSize;
}Bar;

//Doorman to Customer struct.
typedef struct DtoC {    //struct so we can handle the request between one
    int Entrance[2];        //customer from the entrace and the doorman`
    int doorAnswer;
    int custAnswer;
    int barAnswer;
    /*Customer and Doorman Communication Semaphores*/
    sem_t customer_queue;
    sem_t customer_request;
    sem_t doorman_answer;
    sem_t customer_answer;
    sem_t modifying_bar;
    sem_t bar2queue;
    sem_t bar4queue;
    sem_t bar6queue;
    sem_t bar8queue;
    sem_t bar_answer;
}DtoC;

typedef struct Table {      //struct containing info for each table.
    int Capacity;
    int Occupied;
    int WaiterID;
    int WishToPay;
    int WishToOrder;
    int CustomerID;
}Table;

typedef struct Restaurant { //struct for the restaurant's info
    int numOfTables;        //and the semaphores.
    int Order;
    int Bill;
    int Waiters;
    int wrongWaiter;
    int Customers;
	int OnTable;
	int Left;
    /*Customer and Waiter Communication Semaphores*/
    sem_t wants_bill;
    sem_t wants_order;
    sem_t waiters_sleep;
    sem_t take_order;
    sem_t make_check;
    sem_t calling_colleague;
    sem_t modifying_tables;
	sem_t tables_free;
	sem_t append_fp;
}Restaurant;

int isNumber(int len, char * string);

void print_Tables(Table * myTables, int numOfTables);

int checking_tables(Table *myTables, int numOfTables, int group);

int spaceInBar(Bar *myBar, DtoC *dtc);

void placeAtBar(DtoC *dtc, Bar *myBar, Stand *stands);

void remove_from_bar(Bar *myBar, Stand *stands, int id);

void print_bar(Bar *myBar, Stand *stands);

void sitting_at_table(Restaurant *myRes, Table *tables, int period, int groupsTable, char *filename);

void waiting_at_bar(Restaurant *myRes, Table *tables, int period, Bar *myBar, sem_t *myQueue, DtoC *dtc, Stand *stands, char *filename);

void receving_payment(Restaurant *myRes, Table *tables, int period, int moneyAmount, char *filename);

void placing_order(Restaurant *myRes, Table *tables, int period, char *filename);
