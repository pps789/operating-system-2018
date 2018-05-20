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

int trial_division(int);

int main(int argc, char* argv[]) {
    struct timespec begin, end;
    int prime, pnum, weight;
    int i, status = 0;
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

    clock_gettime(CLOCK_MONOTONIC, &begin);

    for(i=0; i<pnum; i++) {
        pids[i]=fork();
        if (pids[i] < 0) {
            printf("Error\n");
            return 1;
        }
        else if (pids[i] == 0) {
            // child
            trial_division(prime);
            return 0;
        }
    }

    while (wait(&status) > 0);
    clock_gettime(CLOCK_MONOTONIC, &end);
    dt = end.tv_sec - begin.tv_sec;
    du = end.tv_nsec - begin.tv_nsec;
    if (du < 0) {
        dt--;
        du += 1e9;
    }
    printf("Number of pc: %d, Time: %ld.%09ldsec\n\n", pnum, dt, du);

    return 0;

}

int trial_division(int prime) {
    int weight;
    long long i, tmp = 0;
    long t;

    weight = syscall(SCHED_GETWEIGHT, getpid());
    if (weight < 0) {
        perror("WEIGHT GETTING FAILED: ");
        return -1;
    }
//	printf("%d = ", prime);
	i = 2;
	while(prime > 1) {

        if (prime % i == 0) {
			prime /= i;
/*			if(prime == 1) {
				printf("%lld\n", i);
			}
			else
				printf("%lld * ", i);
*/			i = 2;
		}
		else
			i++;
    }

    return tmp;
}
