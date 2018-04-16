#include<stdio.h>
#include<stdlib.h>
#include<linux/rotation.h>
#include<unistd.h>

int main() {
	int unlock = 1;
	int d = 0;
	for(d=0; d<360; d= d+60) {
		unlock = syscall(385, d, 40);
		printf("write unlocked at %d+40\n", d);
	}
	return 0;
}

