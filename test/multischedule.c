#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int sched[] = {0, 1, 2, 3, 5};
char* policy[] = {"NORMAL", "RR", "FIFO", "BATCH", "IDLE"};

int job() {
    int P = 1e9+7;
    int i, sum=0;
    for(i=1;i<=P;i++) if(P%i==0) sum++;
    return sum;
}

int main() {
    // assume that default: SCHED_WRR
    int i;

    printf("current my scheduler is %d\n", sched_getscheduler(0));
    for(i=0;i<5;i++){
        int cur = sched[i], r;
        char *curp = policy[i];
        struct sched_param param;

        param.sched_priority = 0;
        if(cur==1 || cur==2) param.sched_priority = 1;
        if(fork()) continue;

        printf("pid %d try to set %d\n", getpid(), cur);

        r = sched_setscheduler(0, cur, &param);

        if (r < 0) {
            printf("setscheduler failed on pid %d: \n", getpid());
            perror("");
            return 0;
        }

        printf("pid %d ok %d!\n", getpid(), sched_getscheduler(0));

        r = syscall(SCHED_GETWEIGHT, 0);
        printf("getweight of %s: %d\n", curp, r);
        if (r < 0) {
            perror("");
        }
        job();

        r = syscall(SCHED_SETWEIGHT, 0, 20);
        printf("setweight of %s: %d\n", curp, r);
        if (r < 0) {
            perror("");
        }
        job();

        // now return to WRR.
        param.sched_priority = 0;
        r = sched_setscheduler(0, 6, &param);
        printf("back to the WRR, %s: %d\n", curp, r);
        if (r < 0) {
            perror("");
        }
        job();

        r = sched_getscheduler(0);
        printf("back to the %d, %s\n", r, curp);
        if (r < 0) {
            perror("");
        }
        job();

        r = syscall(SCHED_SETWEIGHT, 0, 20);
        printf("retry, setseight of %s: %d\n", curp, r);
        if (r < 0) {
            perror("");
        }
        return 0;
    }
    return 0;
}
