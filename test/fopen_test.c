#include<stdio.h>
#include<stdlib.h>

int main() {
    FILE *fp = fopen("/home/owner/proj4/tmp.txt", "w");

    fprintf(fp, "HO!\n");

    return 0;
}
