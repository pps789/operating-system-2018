#include<stdio.h>
#include<unistd.h>

int main() {
    int i;
    for(i=0;i<5;i++) fork();
    while(1); // busy loop!
    return 0;
}
