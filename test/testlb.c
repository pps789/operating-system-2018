#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<errno.h>
#include<signal.h>
#include<time.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int main(int argc, char* argv[]) {
	int c, weight;
	struct timespec begin, end;


