#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/prinfo.h>

struct trav_result {
	struct prinfo *data;
	int nr;
	int nr_max;
}

void traverse_process(struct task_struct* task, struct trav_result* tvr) {
	
	struct prinfo* buf;
	//measure size
	if(tvr->nr > tvr->nr_max)
		return;
	//record process down here
	record_process(task, buf);
	(tvr->nr)++;
	//recursion and iteration start
	struct list_head* child_list; 
	list_for_each(child_list, &task->children) {
		struct task_struct* child = list_entry(child_list, struct task_struct, sibling);
		traverse_process(child, tvr);
	}
}

asmlinkage int sys_ptree(struct prinfo* buf, int* _nr) {
    int nr;
    struct prinfo* data;
    struct trav_result tvr;
	// check whether buff or nr is NULL
    if(buf == NULL || _nr == NULL) return -EINVAL;

    // check accessibility of user memory
    if(access_ok(VERIFY_READ, _nr, sizeof(int))) return -EFAULT;
    if(access_ok(VERIFY_WRITE, _nr, sizeof(int))) return -EFAULT;

    if(copy_from_user(&nr, _nr, sizeof(int)) != 0) return -EFAULT;
    if(nr < 1) return -EINVAL;
    if(access_ok(VERIFY_WRITE, buf, sizeof(struct prinfo) * nr) == 0) return -EFAULT;
    //memory allocation
	data = kmalloc(sizeof(struct prinfo)*nr, GFP_KERNEL);
	//set trav_result
	tvr.data = dat;
	tvr.nr = 0;
	tvr.nr_max = nr;

	//ready to traverse
    read_lock(&tasklist_lock);
    traverse_process();
    read_unlock(&tasklist_lock);

    return 0;
}
