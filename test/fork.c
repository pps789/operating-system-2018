#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define SCHED_SETWEIGHT 380
#define SCHED_GETWEIGHT 381

int main() {
    int child, i;

    for(i=1;i<=20;i++) {
        syscall(SCHED_SETWEIGHT, 0, i); // I am 20
        printf("root is %d, weight is %d\n", getpid(), syscall(SCHED_GETWEIGHT, 0));
        child = fork();
        if (child == 0) {
            printf("child %d's weight is %d\n", getpid(), syscall(SCHED_GETWEIGHT, 0));
            while(1);
            return 0;
        }
        else if(child < 0) {
            printf("WTF?!\n");
            perror("");
        }
    }
    while(1);

    return 0;
}
