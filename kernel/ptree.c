#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/prinfo.h>


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
