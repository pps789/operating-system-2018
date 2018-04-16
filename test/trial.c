#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/rotation.h>
#include <unistd.h>
int main(int argc, char* argv[]) {
	FILE *f;
	int lock;
	int unlock;
	int num;
	int i;
	int check;
	// check input
	if (argc != 2) {
    perror("input error");
		return 1;
	}

	while(1) { 

    // lock and check error
    lock = syscall(381, 90, 90);
		if(lock<0) {
      perror("read lock error");
			return 1;
		}
    // file open and check error
		f = fopen("integer", "r");
		if (f == NULL) {
      perror("file open error");
			return 1;
		}

		fscanf(f, "%d", &num);

    // file close and check error
		check = fclose(f);
		if (check != 0) {
      perror("file close error");
			return 1;
		}

		printf("trial-%s: %d = ", argv[1], num);
    if (num<2) {
      printf("%d\n", num);
		}
    else {
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
		}
	  printf("\n");

		// unlock and check error
	  unlock = syscall(383, 90, 90);
	  if(unlock<0) {
      perror("read unlock error\n");
		  return 1;
	  }
	}
	return 0;
}
