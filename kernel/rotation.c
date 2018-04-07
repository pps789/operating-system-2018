#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/spinlock.h>
#include<linux/spinlock_types.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include<uapi/asm-generic/errno-base.h>
#include<linux/list.h>
#include<linux/signal.h>


#define TYPE_READ 1
#define TYPE_WRITE 2

static struct list_head pend_list = LIST_HEAD_INIT(pend_list);
static struct list_head acq_list = LIST_HEAD_INIT(acq_list);
static int ref_counter[360] = {0, };

struct rot_lock_t {
	int degree;
	int range;
	int type;
	pid_t pid;
	struct list_head now; 
};

DEFINE_SPINLOCK(rot_spinlock);

static int lock_available(struct rot_lock_t *p) {
	struct list_head now = p->now;
	int lower = (p->degree - p->range)%360;
	if(lower < 0)
		lower = lower + 360;
	int upper = (p->degree + p->range)%360;
	int i, j = lower;
	bool avail = true;
	struct rot_lock_t *data;
	
	if(p->type == TYPE_WRITE) {
		do {
			avail = avail && (ref_counter[i] == 0);
			i = (i+1)%360;
		}
		while(i != upper);

		if(avail == true) {
			//list add to acq_list
			data = kmalloc(sizeof(struct rot_lock_t), GFP_KERNEL);
			data = &p;
			list_add_tail(&now, &acq_list);
			do {
				ref_counter[j] = -1;
			}
			while(j != upper);
		}

	}
	else {
		do {
			avail = avail && (ref_counter[i] >= 0);
			i = (i+1)%360;
		}
		while (i != upper);
		
		if(avail == true) {
			list_add_tail(&now, &acq_list);
			do {
				ref_counter[j] = -1;
			}
			while (j != upper);
		}
	}
	return avail; // true: 1 false: 0
}

static int lock_exists(struct rot_lock_t *p) {
	pid_t pid = p->pid
	int type = p->type;
	struct list_head now = p->now;
	int lower = (p->degree - p->range)%360;
	if(lower < 0)
		lower = lower + 360;
	int upper = (p->degree + p->range)%360;
	
	if(type == TYPE_WRITE) {
		//soft filter
		if((ref_counter[lower] == -1) && (ref_counter[upper] == -1) && (ref_counter[degree] == -1)) {
			list_for_each_prev_safe(pos, n, head) {
			
			}
		}
	}
	else {
		//soft filter
		if((ref_counter[lower] >0) && (ref_counter[upper] >0) && (ref_counter[degree] > 0)) {
			list_for_each_prev_safe(pos, n, head) {
			}
		}

	}


	
}
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


