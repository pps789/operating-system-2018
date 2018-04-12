#include<linux/types.h>
#include<linux/uaccess.h>
#include<linux/spinlock.h>
#include<linux/spinlock_types.h>
#include<linux/wait.h>
#include<linux/sched.h>
#include<uapi/asm-generic/errno-base.h>
#include<linux/list.h>
#include<linux/signal.h>
#include<linux/slab.h>

#include<linux/rotation.h>


#define TYPE_READ 1
#define TYPE_WRITE 2

static struct list_head pend_head = LIST_HEAD_INIT(pend_head);
static struct list_head acq_head = LIST_HEAD_INIT(acq_head);
/* 
 * for ref_counter, -1 means write lock is already locked at that area
 * 0 means there is no lock at that area
 * positives means that there are number of read locks equal to the positive
 */
static int ref_counter[360] = {0, };
static int rotation;

struct rot_lock_t {
    int degree;
    int range;
    int type;
    pid_t pid;
    struct list_head loc; 
};

DEFINE_SPINLOCK(rot_spinlock);

/* checking if two rot_lock_t are equal or not */
static int rot_lock_t_equals(struct rot_lock_t *f, struct rot_lock_t *s) {
    if(f->degree == s->degree) {
        if(f->range == s->range) {
            if(f->type == s->type) {
                if(f->pid == s->pid) {
                    return 1; // true
                }
            }
        }
    }
    return 0; // false
}

/*
 * checking if rot_lock_t has rotation
 * Call this function AFTER grabbing the rot_spinlock
 */
static int rot_lock_t_has_rotation(struct rot_lock_t *rot_lock) {
    int dist = rotation - rot_lock->degree;
    if(dist < 0) dist = -dist;
    // Now dist is in [0, 360)
    if(dist >= 180) dist = 360 - dist;
    // Now dist is in [0, 180]
    return dist <= rot_lock->range;
}

/*
 * This function insert new rot_lock_t into acq_head.
 *
 * If success, return 1.
 * Otherwise, return 0.
 * You should call this function with rot_spinlock
 */
static void rot_lock_t_add_into_acq(struct rot_lock_t *p) {
    int degree = p->degree;
    int range = p->range;
    int type = p->type;
    int lower = degree - range;
    if(lower < 0) lower = lower + 360;
    if(type == TYPE_WRITE) {
        int i;
        for(i=0; i<=range*2; i++) {
            ref_counter[(lower+i)%360] = -1;
        }
    }
    else if(type == TYPE_READ) {
        int i;
        for(i=0; i<=range*2; i++) {
            ref_counter[(lower+i)%360]++;
        }
    }
    list_add_tail(&p->loc, &acq_head);
}

/*
 * This function checks if new rot_lock_t can be inserted into acq_list.
 * If possible, return 1.
 * Otherwise, return 0.
 */
static int lock_available(struct rot_lock_t *p) {
    // variables
    int degree = p->degree;
    int range = p->range;
    int type = p->type;
    // lower: 'beginning' degree (inclusive)
    int lower = degree - range;
    // return value
    int avail = 1;
    if(lower<0) lower = lower+360;

    if(!rot_lock_t_has_rotation(p)) return 0;

    if(type == TYPE_WRITE) {
        // if type is WRITE, all value of ref_counter in range should be 0
        int i;
        for(i=0; i<=range*2; i++) {
            if(ref_counter[(lower+i)%360] != 0){
                avail = 0;
            }
        }
    }
    else {
        // if type is READ, all value of ref_counter in range should be at least 0
        int i;
        for(i=0; i<=range*2; i++){
            if(ref_counter[(lower+i)%360] < 0){
                avail = 0;
            }
        }
    }

    // prevent write starvation
    if(type == TYPE_READ && avail) {
        // EXCEPTION: if all ref_counter is 0,
        // and no writer CAN grab the lock,
        // then reader can grab the lock!
        int immediately = 1;
        int i;
        for(i=0; i<=range*2; i++){
            if(ref_counter[(lower+i)%360] != 0){
                immediately = 0;
            }
        }

        if(immediately) {
            // if writer CAN grab, should yield
            struct rot_lock_t *pending;
            list_for_each_entry(pending, &pend_head, loc) {
                if(pending->type == TYPE_WRITE
                        && rot_lock_t_has_rotation(pending)) {
                    int writer_can_grab = 1;
                    int i;
                    int writer_lower = pending->degree - pending->range;
                    if(writer_lower < 0) writer_lower += 360;
                    for(i=0; i<=pending->range*2; i++){
                        if(ref_counter[(writer_lower + i)%360] != 0) {
                            writer_can_grab = 0;
                            break;
                        }
                    }

                    if(writer_can_grab) {
                        avail = 0;
                        break;
                    }
                }
            }
        }
        else {
            // Check another writer is pending...
            struct rot_lock_t *pending;
            list_for_each_entry(pending, &pend_head, loc) {
                if(pending->type == TYPE_WRITE
                        && rot_lock_t_has_rotation(pending)) {
                    avail = 0;
                    break;
                }
            }
        }
    }

    return avail; // true: 1 false: 0
}

/*
 * this function checks if the same rot_lock_t exists in acq_list
 * if it exists, delete and return 1
 */
static int rot_lock_t_remove(struct rot_lock_t *p) {
    //variables
    int degree = p->degree;
    int range = p->range;
    int type = p->type;
    int lower = p->degree - p->range;
    int upper = p->degree + p->range;
    if(lower < 0) lower = lower + 360;
    if(upper >= 360) upper = upper - 360;

    if(type == TYPE_WRITE) {
        // soft filter
        // checks degree, degree+range, degree-range has write lock or not
        if((ref_counter[lower] == -1) && (ref_counter[upper] == -1) && (ref_counter[degree] == -1)) {
            struct rot_lock_t *acquired;
            // iterate in reverse order
            list_for_each_entry_reverse(acquired, &acq_head, loc) {
                if(rot_lock_t_equals(p, acquired)) {
                    // FOUND! delete entry and update ref_counter
                    int i;
                    for(i=0; i<=range*2; i++) ref_counter[(lower+i)%360] = 0;
                    list_del(&acquired->loc);
                    kfree(acquired);
                    return 1; //success
                }
            }
        }
    }
    else {
        // soft filter
        // checks degree, degree+range, degree-range have read lock
        if((ref_counter[lower] > 0) && (ref_counter[upper] > 0) && (ref_counter[degree] > 0)) {
            struct rot_lock_t *acquired;
            list_for_each_entry_reverse(acquired, &acq_head, loc) {
                if(rot_lock_t_equals(p, acquired)) {
                    // FOUND! delete entry and update ref_counter
                    int i;
                    for(i=0; i<=range*2; i++) ref_counter[(lower+i)%360]--;
                    list_del(&acquired->loc);
                    kfree(acquired);
                    return 1; //success
                }
            }
        }
    }
    return 0; //fail - nothing happens
}

/*
 * This function will find proper lock and wake such process.
 * Call this function AFTER grabbing the rot_spinlock
 */
static void wake_up_candidate(void) {
    // Writer already grabbed a lock.
    if(ref_counter[rotation] < 0) return;

    // Reader already grabbed a lock.
    else if(ref_counter[rotation] > 0) {
        // Check another writer is pending...
        int writer_exists = 0;
        struct rot_lock_t *pending;
        list_for_each_entry(pending, &pend_head, loc) {
            if(pending->type == TYPE_WRITE
                    && rot_lock_t_has_rotation(pending)) {
                writer_exists = 1;
                break;
            }
        }

        // Prevent writer starvation.
        if(writer_exists) return;

        // Now we can wake readers!
        list_for_each_entry(pending, &pend_head, loc) {
            if(pending->type == TYPE_READ
                    && rot_lock_t_has_rotation(pending)) {
                // This pending lock can grab lock iff ref_counter >= 0.
                int i;
                int lower = pending->degree - pending->range;
                int can_grab = 1;
                if(lower < 0) lower += 360;
                for(i=0; i<(pending->range)*2; i++) {
                    if(ref_counter[(lower+i)%360] < 0){
                        can_grab = 0;
                        break;
                    }
                }

                // Found proper pending lock!
                if(can_grab) {
                    struct task_struct *target = pid_task(find_vpid(pending->pid), PIDTYPE_PID);
                    if(target == NULL) continue;
                    wake_up_process(target);
                    return;
                }
            }
        }

        return;
    }

    // Nobody grabbed a lock.
    else {
        // Find proper writer first, and try to find reader.
        struct rot_lock_t *pending;
        list_for_each_entry(pending, &pend_head, loc) {
            if(pending->type == TYPE_WRITE
                    && rot_lock_t_has_rotation(pending)) {
                // Writer can grab iff ref_counter == 0.
                int i;
                int lower = pending->degree - pending->range;
                int can_grab = 1;
                if(lower < 0) lower += 360;
                for(i=0; i<=(pending->range)*2; i++) {
                    if(ref_counter[(lower+i)%360] != 0){
                        can_grab = 0;
                        break;
                    }
                }

                // Found writer!
                if(can_grab) {
                    struct task_struct *target = pid_task(find_vpid(pending->pid), PIDTYPE_PID);
                    if(target == NULL) continue;
                    wake_up_process(target);
                    return;
                }
            }
        }

        // Now find proper reader.
        list_for_each_entry(pending, &pend_head, loc) {
            if(pending->type == TYPE_READ
                    && rot_lock_t_has_rotation(pending)) {
                // Reader can grab iff ref_counter >= 0.
                int i;
                int lower = pending->degree - pending->range;
                int can_grab = 1;
                if(lower < 0) lower += 360;
                for(i=0; i<=(pending->range)*2; i++) {
                    if(ref_counter[(lower+i)%360] < 0){
                        can_grab = 0;
                        break;
                    }
                }

                // Found reader!
                if(can_grab) {
                    struct task_struct *target = pid_task(find_vpid(pending->pid), PIDTYPE_PID);
                    if(target == NULL) continue;
                    wake_up_process(target);
                    return;
                }
            }
        }

        // Nobody found.
        return;
    }
}

int sys_set_rotation(int degree) {
    int ret = 0;
    struct rot_lock_t *acq;
    
    // validate input value
    if(degree < 0 || degree >= 360) return -EINVAL;

    spin_lock(&rot_spinlock);

    rotation = degree;
    wake_up_candidate();
    list_for_each_entry(acq, &acq_head, loc) ret++;
    
    spin_unlock(&rot_spinlock);

    return ret;
}

int sys_rotlock_read(int degree, int range) {
    struct rot_lock_t *rotation_lock;
    if(degree < 0 || degree >= 360) return -EINVAL;
    if(range <= 0 || range >= 180) return -EINVAL;

    rotation_lock = kmalloc(sizeof(struct rot_lock_t), GFP_KERNEL);
    if(rotation_lock == NULL) return -EFAULT;

    rotation_lock->degree = degree;
    rotation_lock->range = range;
    rotation_lock->type = TYPE_READ;
    rotation_lock->pid = current->pid;

    // At first, just put into pending list.
    spin_lock(&rot_spinlock);
    list_add_tail(&rotation_lock->loc, &pend_head);
    spin_unlock(&rot_spinlock);

    spin_lock(&rot_spinlock);
    while(1) {
        // Check if I can grab the lock
        if(lock_available(rotation_lock)) {
            // remove from pending list
            list_del(&rotation_lock->loc);
            rot_lock_t_add_into_acq(rotation_lock);
            wake_up_candidate();
            spin_unlock(&rot_spinlock);

            return 0; // success
        }

        // For liveness, wake up proper process
        wake_up_candidate();

        // Go sleep.
        set_current_state(TASK_INTERRUPTIBLE);
        spin_unlock(&rot_spinlock);
        schedule();
        spin_lock(&rot_spinlock);
        set_current_state(TASK_RUNNING); // TODO: do we need this?
    }
}

int sys_rotlock_write(int degree, int range) {
    struct rot_lock_t *rotation_lock;
    if(degree < 0 || degree >= 360) return -EINVAL;
    if(range <= 0 || range >= 180) return -EINVAL;

    rotation_lock = kmalloc(sizeof(struct rot_lock_t), GFP_KERNEL);
    if(rotation_lock == NULL) return -EFAULT;

    rotation_lock->degree = degree;
    rotation_lock->range = range;
    rotation_lock->type = TYPE_WRITE;
    rotation_lock->pid = current->pid;

    // At first, just put into pending list.
    spin_lock(&rot_spinlock);
    list_add_tail(&rotation_lock->loc, &pend_head);
    spin_unlock(&rot_spinlock);

    spin_lock(&rot_spinlock);
    while(1) {
        // Check if I can grab the lock
        if(lock_available(rotation_lock)) {
            // remove from pending list
            list_del(&rotation_lock->loc);
            rot_lock_t_add_into_acq(rotation_lock);
            wake_up_candidate();
            spin_unlock(&rot_spinlock);

            return 0; // success
        }

        // For liveness, wake up proper process
        wake_up_candidate();

        // Go sleep.
        set_current_state(TASK_INTERRUPTIBLE);
        spin_unlock(&rot_spinlock);
        schedule();
        spin_lock(&rot_spinlock);
        set_current_state(TASK_RUNNING); // TODO: do we need this?
    }
}

int sys_rotunlock_read(int degree, int range) {
    struct rot_lock_t rotation_lock;
    int success;
    if(degree < 0 || degree >= 360) return -EINVAL;
    if(range <= 0 || range >= 180) return -EINVAL;

    rotation_lock.degree = degree;
    rotation_lock.range = range;
    rotation_lock.type = TYPE_READ;
    rotation_lock.pid = current->pid;

    spin_lock(&rot_spinlock);
    success = rot_lock_t_remove(&rotation_lock);
    wake_up_candidate();
    spin_unlock(&rot_spinlock);
    
    if(success) return 0;
    return -EFAULT;
}

int sys_rotunlock_write(int degree, int range) {
    struct rot_lock_t rotation_lock;
    int success;
    if(degree < 0 || degree >= 360) return -EINVAL;
    if(range <= 0 || range >= 180) return -EINVAL;

    rotation_lock.degree = degree;
    rotation_lock.range = range;
    rotation_lock.type = TYPE_WRITE;
    rotation_lock.pid = current->pid;

    spin_lock(&rot_spinlock);
    success = rot_lock_t_remove(&rotation_lock);
    wake_up_candidate();
    spin_unlock(&rot_spinlock);
    
    if(success) return 0;
    return -EFAULT;
}

void exit_rotlock(void) {
    struct rot_lock_t *rotation_lock, *tmp;

    spin_lock(&rot_spinlock);

    // clean acq list.
    list_for_each_entry_safe(rotation_lock, tmp, &acq_head, loc) {
        if(rotation_lock->pid == current->pid) {
            int i;
            int lower = rotation_lock->degree - rotation_lock->range;
            int ref_delta =
                (rotation_lock->type == TYPE_WRITE) ? 1 : -1;
            if(lower < 0) lower += 360;

            for(i=0; i<=(rotation_lock->range)*2; i++) {
                ref_counter[(lower+i)%360] += ref_delta;
            }
            list_del(&rotation_lock->loc);
            kfree(rotation_lock);
        }
    }

    // clean pend list.
    list_for_each_entry_safe(rotation_lock, tmp, &pend_head, loc) {
        if(rotation_lock->pid == current->pid) {
            list_del(&rotation_lock->loc);
            kfree(rotation_lock);
        }
    }

    wake_up_candidate(); // TODO: ???
    spin_unlock(&rot_spinlock);
}
