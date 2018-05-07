#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include "sched.h"

#define LB_INTERVAL 2*HZ

void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {

}

static void set_time_slice_wrr(struct sched_wrr_entity *wrr_se) {
    wrr_se->time_slice = mecs_to_jiffies(wrr_se->weight * 10);
}

static struct task_struct *pick_next_task_wrr(struct rq *rq) {
}

void enqueue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
	set_time_slice_wrr(wrr_se);
	list_add_tail_rcu(&wrr_se->run_list, &rq->wrr.queue);
	(rq->wrr.wrr_nr_running)++;
	// TODO: if lock is needed, please add another function to lock
	// wrr before reading weight value
	rq->wrr.wrr_weight_total += wrr_se->weight;
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->rt;

	if(flags & ENQUEUE_WAKEUP)
		wrr_se->timeout = 0;

	enque_wrr_entity(rq, wrr_se);
	inc_nr_running(rq);
}

void dequeue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
	list_del_rcu(&wrr_se->run_list);
	(rq->wrr.wrr_nr_running)--;

	// TODO: if lock is needed, please add another function to lock
	// wrr before reading weight value
	rq->wrr.total_weight -= wrr_se->weight;
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->rt;
	
	update_curr_wrr(rq);
	dequeue_wrr_entity(rq, wrr_se);

	dec_nr_running(rq);
}

static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
}

static void yield_task_wrr(struct rq *rq) {
	struct sched_wrr_entity *wrr_se = &rq->curr->wrr;
	
	//reset rest of the time slice
	set_time_slice_wrr(wrr_se);
	list_move_tail(&wrr_se->run_list, &rq->wrr.queue);
}

static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags) {
}


static struct task_struct *pick_next_task_wrr(struct rq *rq) {
}

static void set_curr_task_wrr(struct rq *rq) {
}

static void put_prev_task_wrr(struct rq *rq) {
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued) {
    struct sched_wrr_entity *wrr_se = &p->wrr;
    struct wrr_rq *wrr_rq = wrr_se->wrr_rq;

    // TODO: need this?
    if (p->policy != SCHED_WRR)
        return;

    if (--wrr_se->time_slice)
        return;

    // now, p spent all its time_slice.

    if (wrr_rq->wrr_nr_running > 1) {
        requeue_task_wrr(rq, p, 0);
        set_tsk_need_resched(p);
    }
    else {
        // if p is the only one task of this cpu, simply refill the time_slice.
        set_time_slice_wrr(wrr_se);
    }
}

static void switched_from_wrr(struct rq *this_rq, struct task_struct *task) {
}

static void switched_to_wrr(struct rq *this_rq, struct task_struct *task) {
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
	.requeue_task		= requeue_task_wrr,
	.yield_task 		= yield_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task 		= put_prev_task_wrr,
	.set_curr_task		= set_curr_task_wrr,
	.task_tick_wrr		= task_tick_wrr,
	.switched_from		= switched_from_wrr,
	.switched_to		= switched_to_wrr,
};
