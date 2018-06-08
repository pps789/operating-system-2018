#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <asm-generic/errno-base.h>

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

int main(int argc, char** argv) {
	struct gps_location gps_loc;
	int ret = 0;
	int lat_int, lat_frac = 0, lng_int, lng_frac = 0;
	char* tmp;
	int acc;
	int i;

	if(argc != 4) {
		printf("Please input adaquate input string\n");
        printf("Usage: ./gpsupdate (lat) (lng) (accuracy)\n");
		return 1;
	}
	
	// translate input string into integer form
	tmp = strtok(argv[1], ".");
	lat_int = atoi(tmp);
	tmp = strtok(NULL, ".");
	if(tmp != NULL) {
		for(i=0; tmp[i] != '\0'; i++) {
			lat_frac = lat_frac*10 + (tmp[i] - '0');
		}
		for(i; i<6; i++) {
			lat_frac = lat_frac*10;
		}
		while(lat_frac >= 1000000) lat_frac /= 10;
	}
	tmp = strtok(argv[2], ".");
	lng_int = atoi(tmp);
	tmp = strtok(NULL, ".");
	if(tmp != NULL) {
		for(i=0; tmp[i] != '\0'; i++) {
			lng_frac = lng_frac*10 + (tmp[i] - '0');
		}
		for(i; i<6; i++) {
			lng_frac = lng_frac*10;
		}
		while(lng_frac >= 1000000) lng_frac /= 10;
	}
	acc = atoi(argv[3]);
	if(acc < 0) {
		printf("Accuracy should not be negative integer.\n");
		return -EINVAL;
	}

	// put integer into gps_loc
	gps_loc.lat_integer = lat_int;
	gps_loc.lat_fractional = lat_frac;
	gps_loc.lng_integer = lng_int;
	gps_loc.lng_fractional = lng_frac;
	gps_loc.accuracy = acc;
	printf("Lat: %d.%06d\nLng: %d.%06d\nAcc: %d\n", gps_loc.lat_integer, gps_loc.lat_fractional, gps_loc.lng_integer, gps_loc.lng_fractional, gps_loc.accuracy);
	
	ret = syscall(380, &gps_loc);
    if (ret < 0) perror("");
}
