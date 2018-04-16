#include <stdio.h>
#include <stdlib.h>
#include <linux/rotation.h>
#include <unistd.h>
int main(int argc, char* argv[]) {
  FILE *f;
	int lock;
	int unlock;
  int num;
  int i;
	while(1) {

    lock = syscall(381, 90, 90);
		if(lock<0) {
      perror("read lock error\n");
			return 0;
		}

		f = fopen("integer", "r");
    fscanf(f, "%d", &num);
		fclose(f);
		printf("trial-%s: %d = ", argv[1], num);
		
		for(i=2;i<=num;i++){
      if(num%i==0) {
				printf("%d", i);
				num /= i;
				if(num!=1) {
				  printf(" * ");
				}
				i--;
      }
		}
    printf("\n");
		unlock = syscall(383, 90, 90);
		if(unlock<0) {
      perror("read unlock error\n");
			return 0;
		}
	}

  return 0;
}
