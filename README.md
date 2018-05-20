OS-team6
========
Project3
--------

> ## Introduction
>  This project is the Third project of Operating Systems, 2018 spring.
> The goal of this project is to implement weighted round-robin scheduler with our own hands. 

> ## How to build
> 1. To build kernel, type `'./build`
> 2. To test, move to 'test' folder, and type `make all`.
> TODO: needs modification

> ## High-level Design
> 1. In `include/uapi/linux/sched.h`, add `SCHED_WRR` in value of 6 as a new scheduler. 
> 2. Construct new file 'kernel/sched/wrr.h`. Implement functions that consists weighted round robin scheduler.
> 3. In `kernel/sched/sched.h`, add `wrr_rq` as a new member of run queue.
> 4. In `include/linux/sched.h`, add `sched_wrr_entity` as a new element of `task_struct`.
> 5. In `kernel/sched/core.c`, add new functions that works for weighted round robin scheduler.

> ## Implementation
> > ### About Weighted Round Robing Scheduler.
> > We constructed wrr_sched_class in `wrr.c`. Wrr.c contains following functions and additionally load balancing functions.
> >	`.next       		= &fair_sched_class,\n
	.enqueue_task       = enqueue_task_wrr,\n
	.dequeue_task       = dequeue_task_wrr,\n
	.yield_task         = yield_task_wrr,\n
	.check_preempt_curr = check_preempt_curr_wrr,\n
	.pick_next_task     = pick_next_task_wrr,\n
	.put_prev_task      = put_prev_task_wrr,\n
#ifdef CONFIG_SMP\n
	.select_task_rq     = select_task_rq_wrr,\n
	.rq_online      = rq_online_wrr,\n
	.rq_offline      = rq_offline_wrr,\n
#endif\n
	.set_curr_task      = set_curr_task_wrr,\n
	.task_tick      = task_tick_wrr,\n
	.task_fork      = task_fork_wrr,\n
	.switched_from      = switched_from_wrr,\n
	.switched_to        = switched_to_wrr,\n
	.get_rr_interval         = get_rr_interval_wrr,\n
};`

> > ### About load balancing
> >  To make our scheduler more efficient, we execute load balancing function in every 2 seconds. It is implemented in `wrr.c`, `wrr_load_balance`. A random processor picks two proccessors, one has the biggest weight and other has the smallest. Then, running proccessor picks a task which is the biggest among tasks 

> > ### About System Calls
> > In this project, we implemented two system calls: `__sched_setweight` and `__sched_getweight`. These system calls are implemented in `/kernel/sched/core.c`.Both system call first locks the task of given pid. Then,  

> ## Inbestigation
> hahahahaha 

> ## Lessons Learned
> 1. Scheduler is so complicated and delicate.

