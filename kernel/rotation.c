#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/spinlock.h>
#include<linux/spinlock_types.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include<uapi/asm-generic/errno-base.h>
#include<linux/list.h>
#include<linux/signal.h>

static struct list_head pend_list = LIST_HEAD_INIT(pend_list);
static struct list_head acq_list = LIST_HEAD_INIT(acq_list);
static int ref_counter[360] = {0, };

struct rot_lock_t {
	int degree;
	int range;
	pid_t pid;
};

DEFINE_SPINLOCK(rot_spinlock);

int sys_set_rotation(int dgree) {
}

int sys_rotlock_read(int degree, int range) {
}

int sys_rotlock_write(int degree, int range) {
}

int sys_rotunlock_read(int degree, int range) {
}

int sys_rotunlock_write(int degree, int range) {
}


