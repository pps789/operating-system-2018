#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/rotation.h>
#include <unistd.h>
int main(int argc, char* argv[]) {
	FILE *f;
	int num;
	int lock;
	int unlock;
  int i; 
	int check;
	// check input
	if (argc != 2) {
    perror("input error");
		return 1;
	}
	// check input is digit
	for(i=0;i<strlen(argv[1]);i++) {
    if (argv[1][i] < '0' || argv[1][i] > '9') {
			perror("input is not digit");
			return 1;
		}
	}
	num = atoi(argv[1]);

	while(1) {
    // lock and check error
    lock = syscall(382, 90, 90);
    if(lock<0) {
      perror("write lock error");
      return 1;
		}
 
    // file open and check error
		f = fopen("integer", "w");
		if (f == NULL) {
      perror("file open error");
			return 1;
		}

		fprintf(f, "%d", num);
		printf("selector: %d\n", num);
		
		// file close and check error
		check = fclose(f);
		if (check != 0) {
      perror("file close error");
			return 1;
		}
	
		// unlock and check error
		unlock = syscall(385, 90, 90);
		if(unlock<0) {
      perror("write unlock error");
		  return 1;
		}

		num++;
	}
	return 0;
}

