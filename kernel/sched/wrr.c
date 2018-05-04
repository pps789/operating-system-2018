#include <linux/slab.h>

#include "sched.h"

const struct sched_class wrr_sched_class = {
  .next                = &fair_sched_class,
	.enqueue_task        = enequeue_task_wrr,
	.dequeue_task        = dequqeue_tesk_wrr,
	.yield_task          = yied_task_wrr,
	.check_preempt_curr  = check_preempt_curr_wrr,
	.pick_next_task      = pick_next_task_wrr,
	.put_prev_task       = put_prev_task_wrr,

#ifdef CONFIG_SMP
	.select_task_rq      = select_task_rq_wrr,

	.switched_from       = switched_from_wrr,

#endif 
	.set_curr_task       = set_curr_task_wrr,
	.task_tick           = task_tick_wrr,
	.get_rr_interval     = get_rr_interval_wrr,
	.prio_changed        = prio_changed_wrr,
	.switched_to         = switched_to_wrr,
};
