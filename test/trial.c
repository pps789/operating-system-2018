#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

int main(int argc, char* argv[]) {
    struct timespec begin, end;
    long long i, tmp;
    long t;
    long long num = 2, last = 9000000000000000000;
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
    return 0;
}
