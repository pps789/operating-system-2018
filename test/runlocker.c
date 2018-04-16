#include<stdio.h>
#include<stdlib.h>
#include<linux/rotation.h>
#include<unistd.h>

int main() {
	int unlock = 1;
	int d = 0;
	for(d=0; d<360; d= d+30) {
		unlock = syscall(383, d, 10);
		printf("read unlocked at %d+10\n", d);
	}
	return 0;
}

