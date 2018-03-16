#include<stdio.h>
#include<unistd.h>

int main(){
    printf("Syscall: %d\n", syscall(380));
}
