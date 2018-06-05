#include<stdio.h>
#include<stdlib.h>

int main() {
    FILE *fp = fopen("tmp.txt", "w");

    fprintf(fp, "HO!\n");

    return 0;
}
