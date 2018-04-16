#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<linux/rotation.h>

int main() {
	int lock, unlock = 1;
	//lock read read write read
	lock = syscall(381, 30, 15);
	if(lock == 0) printf("read lock\n");
	lock = 1;
	lock = syscall(381, 30, 15);
	if(lock == 0) printf("read lock\n");
	lock = 1;
	lock = syscall (382, 30, 20);
	if(lock == 0) printf("write lock\n");
	lock = 1;
	lock = syscall (381, 50, 10);
	if(lock == 0) printf("read lock\n");
	
	syscall(380, 30);
	unlock = syscall(383, 30, 15);
	unlock = syscall(383, 30, 15);
	unlock = syscall(385, 30, 20);
	
}
