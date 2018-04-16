#include<stdio.h>
#include<stdlib.h>
#include<linux/rotation.h>
#include<unistd.h>

int main() {
	int lock = 1;
	int d = 0;
	for(d=0; d<360; d= d+30) {
		lock = syscall(381, d, 10);
		printf("read locked at %d+10\n", d);
	}
	return 0;
}

