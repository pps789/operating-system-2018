#include<stdio.h>
#include<stdlib.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<linux/rotation.h>

int main() {

	int degree = 90;
	int range = 30;
	int i;
	for(i=0; i<10; i++) {
		if(i <= 2)
			syscall(381, degree, range);
		else if(i == 3)
			syscall(382, degree, range);
		else if(i <= 6)
			syscall(381, degree, range);
		else if(i <=8)
			syscall(382, degree, range);
		else
			syscall(381, degree, rnage);
	}
	syscall(380, degree);
}
		
