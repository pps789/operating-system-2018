#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<linux/rotation.h>


int main() {
	int d = 0;
	int rl, wl, ru, wu = 0;
	printf("start\n");
	for(d=0; d<10; d++) {
		rl = syscall(381, 30, 15);
		if(rl == 0)
			printf("read lock at degree: 30 + 15\n");
	}
	return 0;
}

