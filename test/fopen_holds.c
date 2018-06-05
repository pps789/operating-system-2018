#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("tmp.txt", "w");

    fprintf(fp, "KKK\n");

    while(1);
}
