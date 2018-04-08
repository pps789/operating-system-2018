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
 * This function checks if new rot_loc_t can be inserted into acq_list
 * and insert it if available
 *
 * If success, return 1.
 * Oterwise, return 0.
 * You should call this function with rot_spinlock
 */
static int lock_available(struct rot_lock_t *p) {
    // variables
    int degree = p->degree;
    int range = p->range;
    int type = p->type;
    struct list_head loc = p->loc;

    // lower: 'beginning' degree (inclusive)
    int lower = (degree - range)%360;
    // upper: 'ending' degree (inclusive)
    int upper = (degree + range)%360;
    // return value
    int avail = 1;
    struct rot_lock_t *data;

    // TODO: We should investigate pending list...


    if(type == TYPE_WRITE) {
        int i;
        for(i=0; i<=range*2; i++) {
            // if type is WRITE, all value of ref_counter in range should be 0
            if(ref_counter[(lower+i)%360] != 0){
                avail = 0;
            }
        }

        // TODO: remove this!
        /*
        if(avail) {
            //list add to acq_list
            data = kmalloc(sizeof(struct rot_lock_t), GFP_KERNEL);
            data = &p;
            list_add_tail(&loc, &acq_head);
            do {
                ref_counter[j] = -1;
            }
            while(j != upper);
        }
        */
    }
    else {
        int i;
        for(i=0; i<=range*2; i++){
            if(ref_counter[(lower+i)%360] >= 0){
                avail = 0;
            }
        }

        // TODO: remove this!
        /*
        if(avail == true) {
            list_add_tail(&loc, &acq_head);
            do {
                ref_counter[j] = -1;
            }
            while (j != upper);
        }
        */
    }

    return avail; // true: 1 false: 0
}

//this function checks if the same rot_lock_t exists in acq_list
//if it exists, delete and return 1
static int lock_exists(struct rot_lock_t *p) {
    //variables
    int degree = p->degree;
    int range = p->range;
    pid_t pid = p->pid;
    int type = p->type;
    struct list_head loc = p->loc;
    int lower = (p->degree - p->range)%360;
    if(lower < 0)
        lower = lower + 360;
    int upper = (p->degree + p->range)%360;
    struct rot_lock_t *cursor;

    if(type == TYPE_WRITE) {
        //soft filter
        //checks degree, degree+range, degree-range has write lock or not
        if((ref_counter[lower] == -1) && (ref_counter[upper] == -1) && (ref_counter[degree] == -1)) {
            //iterate in reverse order
            list_for_each_entry_reverse(cursor, &acq_head, loc) {
                if(rot_lock_t_equals(p, cursor)) {
                    //take this node
                    // list_del_entry(cursor);
                    //break and return
                    return 1; //success
                }
            }
        }
    }
    else {
        //soft filter
        //checks degree, degree+range, degree-range have read lock
        if((ref_counter[lower] >0) && (ref_counter[upper] >0) && (ref_counter[degree] > 0)) {
            list_for_each_entry_reverse(cursor, &acq_head, loc) {
                if(rot_lock_t_equals(p, cursor)) {
                    //take this node
                    // list_del_entry(cursor);
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
                for(i=0; i<(pending->range)*2; i++) {
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
                for(i=0; i<(pending->range)*2; i++) {
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


