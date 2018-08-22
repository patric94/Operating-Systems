OBJS 	= restaurant.o assistive.o doorman.o waiter.o customer.o
SOURCE	= restaurant.c assistive.c doorman.c waiter.c customer.c
HEADER  = assisive.h
OUT = restaurant doorman waiter customer
CC	= gcc
FLAGS   = -g -c

all: $(OUT)

restaurant : restaurant.o assistive.o
	$(CC) restaurant.o assistive.o -lpthread -o restaurant

doorman : doorman.o assistive.o
	$(CC) doorman.o assistive.o -lpthread -o doorman

waiter : waiter.o assistive.o
	$(CC) waiter.o assistive.o -lpthread -o waiter

customer : customer.o assistive.o
	$(CC) customer.o assistive.o -lpthread -o customer

assistive.o : assistive.c
	$(CC) $(FLAGS) assistive.c

restaurant.o: restaurant.c
	$(CC) $(FLAGS) restaurant.c

doorman.o: doorman.c
	$(CC) $(FLAGS) doorman.c

waiter.o: waiter.c
	$(CC) $(FLAGS) waiter.c

customer.o: customer.c
	$(CC) $(FLAGS) customer.c
# clean house
clean:
	rm -f $(OBJS) $(OUT)
