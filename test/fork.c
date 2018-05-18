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
    syscall(SCHED_SETWEIGHT, 0, 20); // I am 20


