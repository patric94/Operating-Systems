OBJS 	= shapes.o assistive.o circle.o semicircle.o ring.o square.o ellipse.o
SOURCE	= shapes.c assistive.c circle.c semicircle.c ring.c square.c ellipse.c
HEADER  = assisive.h
OUT = shapes circle semicircle ring square ellipse
CC	= gcc
FLAGS   = -g -c

all: $(OUT)

shapes : shapes.o assistive.o
	$(CC) shapes.o assistive.o -o shapes

circle : circle.o assistive.o
	$(CC) circle.o assistive.o -lm -o circle 

semicircle : semicircle.o assistive.o
	$(CC) semicircle.o assistive.o -lm -o semicircle

ring : ring.o assistive.o
	$(CC) ring.o assistive.o -lm -o ring

square : square.o assistive.o
	$(CC) square.o assistive.o -lm -o square   

ellipse : ellipse.o assistive.o
	$(CC) ellipse.o assistive.o -lm -o ellipse 

shapes.o: shapes.c
	$(CC) $(FLAGS) shapes.c

assistive.o : assistive.c
	$(CC) $(FLAGS) assistive.c

circle.o: circle.c
	$(CC) $(FLAGS) circle.c

semicircle.o: semicircle.c
	$(CC) $(FLAGS) semicircle.c

ring.o: ring.c
	$(CC) $(FLAGS) ring.c

square.o: square.c
	$(CC) $(FLAGS) square.c

ellipse.o: ellipse.c
	$(CC) $(FLAGS) ellipse.c
# clean house
clean:
	rm -f $(OBJS) $(OUT)
