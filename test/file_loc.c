#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <asm-generic/errno.h>
#include <linux/gps.h>

int main(int argc, char** argv) {
	int ret;
	char *path = argv[1];
	printf("%s\n", path);

	struct gps_location file_loc = malloc(sizeof(struct gps_location));
	ret = syscall(381, path, file_loc);
	if(ret != 0) {
		return -1;
	}
}


