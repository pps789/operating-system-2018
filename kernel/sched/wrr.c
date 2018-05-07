#include <linux/slab.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/irq_work.h>
#include <sched.h>

#define LB_INTERVAL 2*HZ

void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {

}

void set_time_slice_wrr(struct sched_wrr_entity *wrr_se) {
	wrr_se->time_slice = mecs_to_jiffies(wrr_se->weight * 10)
}

static struct task_struct *pick_next_task_wrr(struct rq *rq) {
}

static inline int on_wrr_rq(struct sched_wrr_entity *wrr_se) {
	return !list_empty(&wrr_se->run_list);
}

static void enqueue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
	//set_time_slice_wrr(wrr_se);
	list_add_tail_rcu(&wrr_se->run_list, &rq->wrr.queue);
	(rq->wrr.wrr_nr_running)++;
	//TODO: if lock is needed, please add another function to lock
	//wrr before reading weight value
	rq->wrr.wrr_weight_total += wrr_se->weight;
}
static void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->rt;

	if(flags & ENQUEUE_WAKEUP)
		wrr_se->timeout = 0;

	enque_wrr_entity(rq, wrr_se);
	inc_nr_running(rq);
}
static void dequeue_wrr_entity(struct rq *rq, struct sched_wrr_entity *wrr_se) {
	list_del_rcu(&wrr_se->run_list);
	(rq->wrr.wrr_nr_running)--;

	//TODO: if lock is needed, please add another function to lock
	//wrr before reading weight value
	rq->wrr.total_weight -= wrr_se->weight;
}
static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags) {
	struct sched_wrr_entity *wrr_se = &p->rt;
	
	update_curr_wrr(rq);
	dequeue_wrr_entity(rq, wrr_se);

	dec_nr_running(rq);
}

static void requeue_task_wrr(struct rq *rq) {
	struct wrr_rq *wrr_rq = &rq->wrr;
	list_rotate_left(&wrr_rq->wrr_rq_list);
}

static void yield_task_wrr(struct rq *rq) {
	requeue_task_wrr(rq, rq->curr);
}
static void check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags) {
}

static task_struct *pick_next_task_wrr(struct rq *rq) {
}
static void set_curr_task_wrr(struct rq *rq) {
}
static void put_prev_task_wrr(struct rq *rq) {
}
static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued) {
}

static void switched_from_wrr(struct rq *rq, struct task_struct *p) {
	if(!p->on_rq || rq->wrr.wrr_nr_running)
		return;
}
static void switched_to_wrr(struct rq *rq, struct task_struct *p) {
	if(!p->se.on_rq)
		return;
	//we need to preempt first, but we don't
	struct sched_wrr_entity *wrr_entity = &p->wrr;
	wrr_entity->task = p;
	//TODO: have to set weight of this task
}

//load balancing
void wrr_trigger_load_balance(struct rq *rq, int cpu) {
}
statid void wrr_load_balance() {
}
//struct
const struct sched_class wrr_sched_class = {
	.enqueue_task 		= enqueue_task_wrr,
	.dequeue_task 		= dequeue_task_wrr,
	.requeue_task		= requeue_task_wrr,
	.yield_task 		= yield_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task 		= put_prev_task_wrr,
	.set_curr_task		= set_curr_task_wrr,
	.task_tick_wrr		= set_task_tick_wrr,
	.switched_from		= switched_from_wrr,
	.switched_to		= switched_to_wrr,
}
