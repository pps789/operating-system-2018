#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int main(int argc, char* argv[]) {
    int c, weight;
    struct timespec begin, end;
    long long i, tmp;
    long t;
    long long num = 2, last = 9000000000000000000;
  

    if (argc != 2) {
        printf("INPUT ERROR\n");
        return -1;
    }

    weight = atoi(argv[1]);

    // set weight and error check
    c = syscall(SCHED_SETWEIGHT, getpid(), weight);
    if (c < 0) {
        printf("WEIGHT SETTING FAILED.\n");
        return -1;
    }

    weight = syscall(SCHED_GETWEIGHT);
    printf("Process : %d, Weight : %d\n", getpid(), weight);

    // start
    clock_gettime(CLOCK_MONOTONIC, &begin);
     
    while (num <= last) {
        for(i=2; i<=num; i++) {
            if (num % i == 0) {
                tmp = i;
            }
        }   
        num++;
    }

    // end
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    
    t = end.tv_sec - begin.tv_sec;

    printf("Weight : %d, Time : %ldsec\n", weight, t);
    return 0;
}
