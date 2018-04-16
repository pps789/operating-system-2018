#include<stdio.h>
#include<stdlib.h>
#include<linux/rotation.h>
#include<unistd.h>

int main() {
	int d = 0;
	int rl, wl, ru, wu = 1;
	printf("start\n");
	while(1) {

		rl = syscall(381, 30, 15);
		if(rl == 0)
			printf("read lock at degree: 30 + 15\n");
		rl = 1;
	}
	return 0;
}

