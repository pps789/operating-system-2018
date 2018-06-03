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
	char *path = argv[1];
	struct gps_location* gps_loc = malloc(sizeof(struct gps_location));
	
	ret = syscall(381, path, gps_loc);
	//error case
	if(ret != 0) {
		return -1;
	}

	printf("path: %s\n", path);
	printf("https://www.google.co.kr/maps/place/");
	printf("%d.%06d°N+", gps_loc->lat_integer, gps_loc->lat_fractional);
	printf("%d.%06d°E\n", gps_loc->lng_integer, gps_loc->lng_fractional);
	return 0;
}


