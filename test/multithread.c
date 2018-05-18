#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381
#define GETTID 224

int gettid(){return syscall(GETTID);}

void entry(int w) {
    int result;
    printf("my pid is %d and tid is %d, try %d\n", getpid(), gettid(), w);
    result = syscall(SCHED_SETWEIGHT, gettid(), w);
    if (result < 0) {
        fprintf(stderr, "failed %d: ", w);
        perror("");
    }
    printf("my pid is %d ans tid is %d, got %d\n", getpid(), gettid(),
            syscall(SCHED_GETWEIGHT, 0));
    while(1); // busy loop!
}

pthread_t tids[33];
int main() {
    int i, num = 1e9, tmp = 0;
    for(i=1;i<=20;i++) pthread_create(tids+i, NULL, entry, (void *)i);

    // do some busy job:
    for(i=2;i<=num;i++) if(num%i) tmp++;

    printf("root is now weight %d\n", syscall(SCHED_GETWEIGHT, 0));
}
