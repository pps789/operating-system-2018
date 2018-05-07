#include <linux/slab.h>
#include <uapi/asm-generic/errno-base.h>
#include <linux/irq_work.h>
#include <sched.h>

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


//load balancing
void wrr_trigger_load_balance(struct rq *rq, int cpu) {
}
statid void wrr_load_balance() {
}
//struct
const struct sched_class wrr_sched_class = {
	.next = ,
	.enqueue_task 		= enqueue_task_wrr,
	.dequeue_task 		= dequeue_task_wrr,
	.yield_task 		= yield_task_wrr,
	.check_preempt_curr	= check_preempt_curr_wrr,
	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task 		= put_prev_task_wrr,
