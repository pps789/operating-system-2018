#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include "sched.h"

#define LB_INTERVAL 2*HZ


static struct task_struct *pick_next_task_wrr(struct rq *rq) {
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flag) {
}
static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flag) {
}
static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int flag) {
}
static void yield_task_wrr(struct rq *rq) {
}
static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flag) {
}

static void set_curr_task_wrr(struct rq *rq) {
}
static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued) {
}


// load balancing
static void wrr_load_balance(void) {
    // TODO: perform LB
}

// jiffies of NEXT balance time
static unsigned long wrr_next_balance;
static DEFINE_SPINLOCK(wrr_balance_lock);

void wrr_trigger_load_balance(void) {
    if (!time_after_eq(jiffies, wrr_next_balance)) return;

    // for now, we should do load balancing
    spin_lock(&wrr_balance_lock);

    if (!time_after_eq(jiffies, wrr_next_balance)) {
        // LB already performed.
        spin_unlock(&wrr_balance_lock);
        return;
    }

    // before LB, set timeout.
    wrr_next_balance = jiffies + LB_INTERVAL;
    
    spin_unlock(&wrr_balance_lock);

    wrr_load_balance();
}

// our sched_class struct
const struct sched_class wrr_sched_class = {
	// .next = ,
	.enqueue_task 		= enqueue_task_wrr,
	.dequeue_task 		= dequeue_task_wrr,
	.yield_task 		= yield_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	// .put_prev_task 		= put_prev_task_wrr,
};
