#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/prinfo.h>
#include <linux/slab.h>

struct trav_result {
    struct prinfo *data;
    int nr;
    int nr_max;
};

/*
   record process information into buf.
*/
static void record_process(struct task_struct* task, struct prinfo* buf){
    struct task_struct
        *parent = task->real_parent,
        *first_child = list_first_entry_or_null(&task->children, struct task_struct, sibling);

    struct task_struct *next_sibling
        = (parent && (&(parent->children) != (task->sibling).next))
        ? (list_entry((task->sibling).next, struct task_struct, sibling))
        : NULL;

    /* always exist */
    buf->state = task->state;
    buf->pid = task->pid;
    buf->uid = task->cred->uid;
    get_task_comm(buf->comm, task);

    /* conditional fields */
    buf->parent_pid = buf->first_child_pid = buf->next_sibling_pid = 0;
    if(parent) buf->parent_pid = parent->pid;
    if(first_child) buf->first_child_pid = first_child->pid;
    if(next_sibling) buf->next_sibling_pid = next_sibling->pid;
}

static void traverse_process(struct task_struct* task, struct trav_result* tvr) {
	struct list_head* child_list; 

	// record process down here
    if(tvr->nr < tvr->nr_max) {
        record_process(task, tvr->data + tvr->nr);
    }
	(tvr->nr)++;
	// recursion and iteration start
	list_for_each(child_list, &task->children) {
		struct task_struct* child = list_entry(child_list, struct task_struct, sibling);
		traverse_process(child, tvr);
	}
}

asmlinkage int sys_ptree(struct prinfo __user *buf, int __user *_nr) {
    int nr;
    struct prinfo* data;
    struct trav_result tvr;
	int nr_tot;
	// check whether buff or nr is NULL
    if(buf == NULL || _nr == NULL) return -EINVAL;

    // check accessibility of user memory
    if(access_ok(VERIFY_READ, _nr, sizeof(int)) == 0) return -EFAULT;
    if(access_ok(VERIFY_WRITE, _nr, sizeof(int)) == 0) return -EFAULT;

    if(copy_from_user(&nr, _nr, sizeof(int)) != 0) return -EFAULT;
    if(nr < 1) return -EINVAL;
    if(access_ok(VERIFY_WRITE, buf, sizeof(struct prinfo) * nr) == 0) return -EFAULT;

    // memory allocation
	data = kmalloc(sizeof(struct prinfo)*nr, GFP_KERNEL);
    if(data == NULL) return -EFAULT;

	// set trav_result
	tvr.data = data;
	tvr.nr = 0;
	tvr.nr_max = nr;

	// ready to traverse
    read_lock(&tasklist_lock);
    traverse_process(&init_task, &tvr);
    read_unlock(&tasklist_lock);

    // return the total number of process
	if (tvr.nr > tvr.nr_max)
	{
        // # of process is greater than buffer size
        nr_tot = tvr.nr;
        tvr.nr = tvr.nr_max;
	}
	else {
        nr_tot = tvr.nr;
	}

    // copy to user
    if(copy_to_user(buf, tvr.data, sizeof(struct prinfo) * tvr.nr) != 0) return -EFAULT;
    if(copy_to_user(_nr, &tvr.nr, sizeof(int)) != 0) return -EFAULT;

    kfree(data);

	return nr_tot;
}
