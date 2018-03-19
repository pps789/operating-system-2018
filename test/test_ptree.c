#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#define __NR_ptree 380
#define NR_MAX 300
struct prinfo {
	long state;
	pid_t pid;
	pid_t parent_pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long uid;
	char comm[64];
};
void print_form(struct prinfo *p, int idx, int now_tap) {
	int i;
	for (i=0; i<now_tap; i++) printf("  ");
	printf("%s,%d,%ld,%d,%d,%d,%d\n", 
	p[idx].comm, p[idx].pid, p[idx].state,
	p[idx].parent_pid, p[idx].first_child_pid,
	p[idx].next_sibling_pid, p[idx].uid);
}
void print_ptree(struct prinfo *ptree, int nr) {
	int now = 0, stack_pnt = 0;
	int stack[nr];
	int i;
	for (i=0; i<nr; i++){
		
		print_form(ptree, i, now);
		
		if (ptree[i].next_sibling_pid != 0) {
			stack[stack_pnt] = now;	stack_pnt++;
		}

		if (ptree[i].first_child_pid != 0) {
			now++;
		}
		else { 
			stack_pnt--; now = stack[stack_pnt]; 
		}
	}
}

int main() {
	int nr=NR_MAX;
	struct prinfo *buf = (struct prinfo*)malloc(sizeof(struct prinfo)*NR_MAX);
	syscall(__NR_ptree, buf, &nr);
	printf("---------------syscall test start!----------------\n");
	print_ptree(buf, nr);
	printf("---------------syscall test end!----------------\n");
	free(buf);
	return 0;
}
