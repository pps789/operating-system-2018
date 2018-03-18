#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/prinfo.h>

/*
   record process information into buf.
*/
void record_process(struct task_struct* task, struct prinfo* buf){
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

/*
void traverse_process(struct task_struct* task, struct trav_result* tvr) {
}
*/

asmlinkage int sys_ptree(struct prinfo* buf, int* _nr) {
    int nr;
    
    // check whether buff or nr is NULL
    if(buf == NULL || _nr == NULL) return -EINVAL;

    // check accessibility of user memory
    if(access_ok(VERIFY_READ, _nr, sizeof(int))) return -EFAULT;
    if(access_ok(VERIFY_WRITE, _nr, sizeof(int))) return -EFAULT;

    if(copy_from_user(&nr, _nr, sizeof(int)) != 0) return -EFAULT;
    if(nr < 1) return -EINVAL;
    if(access_ok(VERIFY_WRITE, buf, sizeof(struct prinfo) * nr) == 0) return -EFAULT;
	
    read_lock(&tasklist_lock);
    // do some jobs...
    // traverse_process();
    // traverse ends
    read_unlock(&tasklist_lock);

    return 0;
}
