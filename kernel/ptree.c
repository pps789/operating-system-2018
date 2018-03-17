#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/prinfo.h>

void traverse_process(struct task_struct* task, struct trav_result* tvr) {
+	struct prinfo* pri;
+	struct task_struct* child;
+	struct task_struct* sibling;
+	int number = tvr->nr;
+	/* task_struct
+	   char comm[]
+	   volatile long state
+	   pid_t pid
+	   struct task_struct __rcu *real_parent
+	   struct list_head children
+	   struct list_head sibling
+	   task_struct->cred->uid
+	   */
+
+	/* from list.h
+	   list_entry(ptr, type, member)
+	   list_first_entry(ptr, type, member)
+	   list_for_each(post, head)
+	*/
+
+	//setting prinfo struct
+	*(pri+number).state = task->state;
+	*(pri+number).pid = task->pid;
+	*(pri+number).parent_pid = task->parent->pid;
+	*(pri+number).first_child_pid = list_first_entry();
+	*(pri+number).next_sibling_pid = list_first_entry();
+	*(pri+number).uid = task->cred->uid;
+	*(pri+number).comm = ;
}

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
