#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <prinfo.h>
#define __NR_ptree 380
#define NR_MAX 300
/*
struct prinfo {
	long state;             
	pid_t pid;              
	pid_t parent_pid;       
	pid_t first_child_pid;  
	pid_t next_sibling_pid; 
	long uid;               
	char comm[64];          
};
*/
void print_ptree(struct prinfo *p, int nr) {
	int i=0;
	for (i=0; i<nr; i++){
		printf("%s,%d,%ld,%d,%d,%d,%d\n", 
			p[i].comm, p[i].pid, p[i].state,
			p[i].parent_pid, p[i].first_child_pid,
			p[i].next_sibling_pid, p[i].uid);
	}
}

int main() {
	int nr=NR_MAX;
	int nr_tot = 0;
	struct prinfo *buf = (struct prinfo*)malloc(sizeof(struct prinfo)*NR_MAX);
	nr_tot = syscall(__NR_ptree, buf, &nr);
	
	printf("syscall test start!\n");
	print_ptree(buf, nr);
	printf("syscall test end!\n");
	printf("number of total process: %d\n", nr_tot);
	printf("number of copied process: %d\n", nr);
	free(buf);
	return 0;
}
