#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef struct{
	float	x;
	float	y;
} MyRecord;


int isNumber(int, char*);

void answering_query(char *, char *, char *, char *, int, int, int, char *, char *, char *, char *, int, FILE *);
