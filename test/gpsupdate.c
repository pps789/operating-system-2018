#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};

int main(int argc, char** argv) {
	struct gps_location* gps_loc = malloc(sizeof(struct gps_location));
	int ret = 0;
	int lat_int, lat_frac, lng_int, lng_frac = 0;
	char* tmp;
	int acc;
	int i;

	if(argc != 4) {
		printf("Please input adaquate input string\n");
		return -1;
	}
	
	//translate input string into integer form
	printf("lat\n");
	tmp = strtok(argv[1], ".");
	lat_int = atoi(tmp);
	tmp = strtok(NULL, ".");
	if(tmp != NULL)
		lat_frac = atoi(tmp);
	
	tmp = strtok(argv[2], ".");
	lng_int = atoi(tmp);
	tmp = strtok(NULL, ".");
	if(tmp != NULL)
		lng_frac = atoi(tmp);
	acc = atoi(argv[3]);

	//put integer into gps_loc
	gps_loc->lat_integer = lat_int;
	gps_loc->lat_fractional = lat_frac;
	gps_loc->lng_integer = lng_int;
	gps_loc->lng_fractional = lng_frac;
	gps_loc->accuracy = acc;
	printf("Lat: %d.%d\nLng: %d.%d\nAcc: %d\n", gps_loc->lat_integer, gps_loc->lat_fractional, gps_loc->lng_integer, gps_loc->lng_fractional, gps_loc->accuracy);
	
	//ret = syscall(380, gps_loc);
}
