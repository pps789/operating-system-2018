#include<stdio.h>
#include<stdlib.h>
#include<linux/rotation.h>


int main() {
	int d = 0;
	int rl, wl, ru, wu = 0;
	rl = syscall(381, 30, 15);
	if(rl == 0)
		printf("read lock at degree: %d + %d\n", 30, 15);
	rl = syscall(381, 30, 15);
	if(rl == 0)
		printf("read lock at degree: %d + %d\n", 30, 15);
	wl = syscall(382, 30, 15);
	if(wl == 0)
		printf("write lock at degree: %d + %d\n", 30, 15);
	ru = syscall(383, 30);
	if(ru == 0)
		printf("unlocked readlock at %d\n", d*30);
	wu = syscall(385, 30);
	if(wu == 0)
		printf("unlocked writedlock at %d\n", d*30);
	wl,rl, ru, wu=0;

}

