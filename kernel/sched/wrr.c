#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include "sched.h"

#define WRR_DEFAULT_WEIGHT 10
#define WRR_MAX_WEIGHT 20
#define WRR_MIN_WEIGHT 1

#define LB_INTERVAL 2*HZ

void init_wrr_rq(struct wrr_rq *wrr_rq) {
    wrr_rq->wrr_nr_running = 0;
    INIT_LIST_HEAD(&wrr_rq->wrr_rq_list);
    wrr_rq->wrr_weight_total = 0;
}

static void set_time_slice_wrr(struct sched_wrr_entity *wrr_se) {
    wrr_se->time_slice = msecs_to_jiffies(wrr_se->weight * 10);
}

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se) {
    return container_of(wrr_se, struct task_struct, wrr);
}

static struct task_struct *pick_next_task_wrr(struct rq *rq) {
    struct wrr_rq *wrr_rq = &rq->wrr;
    if (list_empty(&wrr_rq->wrr_rq_list)) return NULL;
    else {
        struct sched_wrr_entity *wrr_se
            = list_first_entry(&wrr_rq->wrr_rq_list, struct sched_wrr_entity, run_list);
        struct task_struct *task = wrr_task_of(wrr_se);
        set_time_slice_wrr(wrr_se);
        return task;
    }
}

static void enqueue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
	//set_time_slice_wrr(wrr_se);
    // TODO: do we need RCU?
	list_add_tail(&wrr_se->run_list, &rq->wrr.wrr_rq_list);
	(rq->wrr.wrr_nr_running)++;

	// TODO: if lock is needed, please add another function to lock
	// wrr before reading weight value
	rq->wrr.wrr_weight_total += wrr_se->weight;
}

static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->wrr;

	enqueue_wrr_entity(rq, wrr_se);
	inc_nr_running(rq);
}

static void dequeue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
    // TODO: do we need RCU?
	list_del(&wrr_se->run_list);
	(rq->wrr.wrr_nr_running)--;

	// TODO: if lock is needed, please add another function to lock
	// wrr before reading weight value
	rq->wrr.wrr_weight_total -= wrr_se->weight;
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->wrr;
	
	dequeue_wrr_entity(rq, wrr_se);
	dec_nr_running(rq);
}

static void requeue_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	list_rotate_left(&wrr_rq->wrr_rq_list);
}

static void yield_task_wrr(struct rq *rq) {
    // we don't have to implement this.
}

static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags) {
    // nothing to do.
}

static void set_curr_task_wrr(struct rq *rq) {
    // TODO: need to do sth?
}

static void put_prev_task_wrr(struct rq *rq, struct task_struct *prev) {
    // TODO: need to do sth?
}

#ifdef CONFIG_SMP

static int find_lowest_rq(struct task_struct *p) {
    int cpu, ret = -1;
    unsigned long min_weight;

    for_each_online_cpu(cpu) {
        struct rq *rq = cpu_rq(cpu);
        struct wrr_rq *wrr_rq = &rq->wrr;
        if(ret == -1 ||
                (wrr_rq->wrr_weight_total < min_weight &&
                 cpumask_test_cpu(cpu, tsk_cpus_allowed(p)))) {
            ret = cpu;
            min_weight = wrr_rq->wrr_weight_total;
        }
    }

    return ret;
}

static int select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags) {
	struct rq *rq;
	int cpu, target;

	cpu = task_cpu(p);

	if (p->nr_cpus_allowed == 1)
        return cpu;

	rq = cpu_rq(cpu);

	rcu_read_lock();

    target = find_lowest_rq(p);

    if (target != -1)
        cpu = target;

	rcu_read_unlock();

	return cpu;
}

static void rq_online_wrr(struct rq *rq) {
    // TODO: need to do sth?
}

static void rq_offline_wrr(struct rq *rq) {
    // TODO: need to do sth?
}

#endif /* CONFIG_SMP */

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
        requeue_task_wrr(rq);
        set_tsk_need_resched(p);
    }
    else {
        // if p is the only one task of this cpu, simply refill the time_slice.
        set_time_slice_wrr(wrr_se);
    }
}



static void task_fork_wrr(struct task_struct *task) {
    // TODO: set default weight
}

static void switched_from_wrr(struct rq *this_rq, struct task_struct *task) {
    // TODO: need to do sth?
}

static void switched_to_wrr(struct rq *this_rq, struct task_struct *task) {
    // TODO: set default weight
}

static unsigned int get_rr_interval_wrr(struct rq *rq, struct task_struct *p) {
    struct sched_wrr_entity *wrr_se = &p->wrr;
    return wrr_se->weight * 10;
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
    .next       = &fair_sched_class,
	.enqueue_task 		= enqueue_task_wrr,
	.dequeue_task 		= dequeue_task_wrr,
	.yield_task 		= yield_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task 		= put_prev_task_wrr,

#ifdef CONFIG_SMP
    .select_task_rq     = select_task_rq_wrr,
    .rq_online      = rq_online_wrr,
    .rq_offline      = rq_offline_wrr,
#endif

	.set_curr_task		= set_curr_task_wrr,
	.task_tick		= task_tick_wrr,
    .task_fork      = task_fork_wrr,
	.switched_from		= switched_from_wrr,
	.switched_to		= switched_to_wrr,
    .get_rr_interval         = get_rr_interval_wrr,
};
