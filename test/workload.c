#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <wait.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int trial_devision();

int main(int argc, char* argv[]) {
	struct timespec begin, end;
	int pnum;
	int i;
    pid_t *pids;
    long dt, du;

    if(argc != 4) {
        printf("Usage: ./workload (prime) (number of process) (weight)\n");
        return 1;
    }

    //start
    prime = atoi(argv[1]);
    pnum = atoi(argv[2]);
    weight = atoi(argv[3]);
    syscall(SCHED_SETWEIGHT, 0, weight);
    pids = (pid_t*)malloc(sizeof(pid_t)*pnum);
    
    for(i=0; i<pnum; i++) {
        pids[i]=fork();
        if (pids[i] < 0) printf("Error\n");
        else if (pids[i] == 0) {
            trial_devision();
            exit(0);
        }
        else {
            wait(NULL);
            clock_gettime(CLOCK_MONOTONIC, &end);
	        t = end.tv_sec - begin.tv_sec;
            if (i == pnum-1)
            printf("Number of pc: %d, Time: %ldsec\n\n", pnum, t);
        }	
	}
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
        perror("WEIGHT SETTING FAILED: ");
        return -1;
    }

    weight = syscall(SCHED_GETWEIGHT, getpid());
    if (weight < 0) {
        perror("WEIGHT GETTING FAILED: ");
        return -1;
    }

    for(i=2; i<=last; i++) {
        if (num % i == 0) {
            tmp++;
        }
    }   
        num++;

    //t = end.tv_sec - begin.tv_sec;
    //printf("Weight : %d, Time : %ldsec\n", weight, t);
    return 0;
}
