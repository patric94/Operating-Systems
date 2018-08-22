#
# In order to execute this "Makefile" just type "make"
#	A. Delis (ad@di.uoa.gr)
#

OBJS 	= main.o hashTable.o record.o skipList.o
SOURCE	= main.c hashTable.c record.c skipList.c
HEADER  = hashTable.h record.h skipList.h
OUT  	= stucs
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

Record.o: record.c
	$(CC) $(FLAGS) record.c

hashTable.o: hashTable.c
	$(CC) $(FLAGS) hashTable.c

skipList.o: skipList.c
	$(CC) $(FLAGS) skipList.c
# clean house
clean:
	rm -f $(OBJS) $(OUT)
