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
> >	```.next       		= &fair_sched_class,
	.enqueue_task       = enqueue_task_wrr,
	.dequeue_task       = dequeue_task_wrr,
	.yield_task         = yield_task_wrr,
	.check_preempt_curr = check_preempt_curr_wrr,
	.pick_next_task     = pick_next_task_wrr,
	.put_prev_task      = put_prev_task_wrr,```
	
	#ifdef CONFIG_SMP
	.select_task_rq     = select_task_rq_wrr,
	.rq_online      = rq_online_wrr,
	.rq_offline      = rq_offline_wrr,
	
	#endif
	.set_curr_task      = set_curr_task_wrr,
	.task_tick      = task_tick_wrr,
	.task_fork      = task_fork_wrr,
	.switched_from      = switched_from_wrr,
	.switched_to        = switched_to_wrr,
	.get_rr_interval         = get_rr_interval_wrr,
};```

> > ### About load balancing
> >  To make our scheduler more efficient, we execute load balancing function in every 2 seconds. It is implemented in `wrr.c`, function `wrr_load_balance`. A random processor picks two proccessors, which has the biggest weight and the other has smallest weight. Then, running proccessor picks a task to transfer from biggest to smallest. After the transaction, processor with biggest weight should have bigger weight than another. 

> > ### About System Calls
> > In this project, we implemented two system calls: `__sched_setweight` and `__sched_getweight`. These system calls are implemented in `/kernel/sched/core.c`. Both system call first locks the task of given pid. Then, they call functions in `kernel/sched/wrr.c`, `set_weight_wrr` and `get_weight_wrr`. These functions work just as their names imply. Only root user, or the user who run the process can set weight of process. Additionally, only root user can make weight larger than before.

> ## Investigation
> hahahahaha 

> ## Lessons Learned
> 1. Scheduler is so complicated and delicate.

