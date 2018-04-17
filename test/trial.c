#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/rotation.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    FILE *f;
    int lock;
    int unlock;
    int num;
    int i;
    int check;
    int _strlen;
    int id;
    // check input
    if (argc != 2) {
        printf("input error\n");
        return 1;
    }

    _strlen = strlen(argv[1]);
    // check input is digit
    for(i=0;i<_strlen;i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            printf("input is not digit\n");
            return 1;
        }
    }
    id = atoi(argv[1]);

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
            printf("file open error\n");
            return 1;
        }

        fscanf(f, "%d", &num);

        printf("trial-%d: %d = ", id, num);
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

        // file close and check error
        check = fclose(f);
        if (check != 0) {
            printf("file close error\n");
            return 1;
        }

        // unlock and check error
        unlock = syscall(383, 90, 90);
        if(unlock<0) {
            perror("read unlock error\n");
            return 1;
        }
    }
    return 0;
}
