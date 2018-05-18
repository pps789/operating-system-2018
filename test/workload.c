#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int trial_devision();

int main(int argc, char* argv[]) {
	struct timespec begin, end;
	int pnum;
	int i;
	long t;
	if(argc != 2) {
		printf("INPUT ERROR\n");
		return -1;
	}

	//start
	clock_gettime(CLOCK_MONOTONIC, &begin);
	pnum = atoi(argv[2]);
	for(i=0; i< atoi(argv[2]); i++) {
		fork();
		trial_devision();
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	t = end.tv_sec - begin.tv_sec;
	printf("Number of pc: %d, Time: %ldsec\n", pnum, t);
	return 0;

}
int trial_devision() {
    int c, weight;
    struct timespec begin, end;
    long long i, tmp;
    long t;
    long long num = 2, last = 1000000007;
  

    weight = rand()%19 + 1;

    // set weight and error check
    c = syscall(SCHED_SETWEIGHT, getpid(), weight);
    if (c < 0) {
        printf("WEIGHT SETTING FAILED.\n");
        return -1;
    }

    weight = syscall(SCHED_GETWEIGHT, getpid());
    printf("Process : %d, Weight : %d\n", getpid(), weight);

    // start
    //clock_gettime(CLOCK_MONOTONIC, &begin);
     
    while (num <= last) {
        for(i=2; i<=num; i++) {
            if (num % i == 0) {
                tmp = i;
            }
        }   
        num++;
    }

    // end
    //clock_gettime(CLOCK_MONOTONIC, &end);
    
    
    t = end.tv_sec - begin.tv_sec;

    printf("Weight : %d, Time : %ldsec\n", weight, t);
    return 0;
}
