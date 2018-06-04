#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <asm-generic/errno.h>

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

int main(int argc, char** argv) {
	int ret;
	struct gps_location gps_loc;

    if (argc != 2) {
        printf("Usage: ./file_loc (file path)\n");
        return 1;
    }
	
	ret = syscall(381, argv[1], &gps_loc);
	//error case
	if(ret != 0) {
        perror("");
		return -1;
	}

	printf("path: %s\n", argv[1]);
	printf("https://www.google.co.kr/maps/place/");
	printf("%d.%06d,", gps_loc.lat_integer, gps_loc.lat_fractional);
	printf("%d.%06d\n", gps_loc.lng_integer, gps_loc.lng_fractional);
	return 0;
}


