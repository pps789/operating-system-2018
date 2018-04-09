#include <stdio.h>
#include <stdlib.h>
#include <linux/rotation.h>
#include <errono.h>
int main(int argc, char* argv[]) {
  FILE *f;
  int num = atoi(argv[1]);
  int lock;
  int unlock;
	while(1) {
    lock = syscall(382, 90, 90);
    if(lock<0) {
      perror("write lock error\n");
      return 0;
		}

		f = fopen("integer", "w");
	  fprintf(f, "%d", num);
		printf("selector: %d\n", num);
		fclose(f);
		
		unlock = syscall(385, 90, 90);
		if(unlock<0) {
      perror("write unlock error\n");
		  return 0;
		}

		num++;
  }
  return 0;
}

