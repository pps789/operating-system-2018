OS-team6
========
Project3
--------

> ## Introduction
>  This project is the Third project of Operating Systems, 2018 spring.
> The goal of this project is to implement weighted round-robin scheduler with our own hands. 

> ## How to build
> 1. To build kernel, type `'./build`
>     * It builds our own linux kernel.
> 2. To test, move to 'test' folder, and type `make all`.
>     * It builds few test codes, described below.

> ## High-level Design
> 1. In `include/uapi/linux/sched.h`, add `SCHED_WRR` in value of 6 as a new scheduler. 
> 2. Construct new file 'kernel/sched/wrr.c`. Implement functions that consists weighted round robin scheduler.
> 3. In `kernel/sched/sched.h`, add `struct wrr_rq` as a new member of run queue.
> 4. In `include/linux/sched.h`, add `struct sched_wrr_entity` as a new element of `task_struct`.
> 5. In `kernel/sched/core.c`, add new functions that works for weighted round robin scheduler.
> 6. In `kernel/kthread.c`, modify some codes to change scheduling policy of `kthread`.
> 7. In `include/linux/init_task.h`, modify macro `INIT_TASK` to make our scheduler default.

> ## Implementation
> > ### About Weighted Round Robing Scheduler
> > We constructed `wrr_sched_class` in `wrr.c`.
> >
> > ```C
> > const struct sched_class wrr_sched_class = ...
> > ```
> >
> > `wrr.c` contains following functions assigned to `wrr_sched_class`
> > and additionally load balancing functions.
> > 
> >	```C
> > .next       		= &fair_sched_class,
> >	.enqueue_task       = enqueue_task_wrr,
> >	.dequeue_task       = dequeue_task_wrr,
> >	.yield_task         = yield_task_wrr,
> >	.check_preempt_curr = check_preempt_curr_wrr,
> >	.pick_next_task     = pick_next_task_wrr,
> >	.put_prev_task      = put_prev_task_wrr,
> >	
> >	 #ifdef CONFIG_SMP
> >	.select_task_rq     = select_task_rq_wrr,
> >	.rq_online      = rq_online_wrr,
> >	.rq_offline      = rq_offline_wrr,
> >  #endif
> >
> >	.set_curr_task      = set_curr_task_wrr,
> >	.task_tick      = task_tick_wrr,
> >	.task_fork      = task_fork_wrr,
> >	.switched_from      = switched_from_wrr,
> >	.switched_to        = switched_to_wrr,
> >	.get_rr_interval         = get_rr_interval_wrr,
> > ```

> > Each run queue has a member named `wrr`, which is `struct wrr_rq`.
> > ```C
> > struct wrr_rq {
> >     unsigned int wrr_nr_running; // size of run queue
> >     struct list_head wrr_rq_list; // list of current run queue
> >     unsigned long wrr_weight_total; // total weight of current run queue
> > };
> > ```
> > The first element of `wrr_rq_list` is current running task if only if such list is not empty.
> > That `wrr_rq_list` is linked to `struct task_struct`'s member `struct sched_wrr_entity`, named `wrr`.
> > ```C
> > struct sched_wrr_entity {
> >     struct list_head run_list; // linked to other task_struct, or wrr_rq
> >     unsigned int time_slice;
> >     unsigned int weight;
> > };
> > ```

> > Each tick we decrease `time_slice` of the current running task.
> > If it spent all time slices, we just rotate the run queue to put the task to the back of the run queue.
> > The logic of above is implemented in function `task_tick_wrr`.

> > Function `select_task_rq_wrr` is designed to pick the cpu which has the lowest weight.
> > That function is called during fork, so new tasks can choose cpus properly.

> > For chaning of sched classes, we just modified `rt_sched_class.next` to `&wrr_sched_class` in the file `kernel/sched/rt.c`.

> > We have nothing to do to inherit weights for child processes, because `sched_wrr_entity` will be copied into forked process.

> > ### About Load Balancing
> > To make our scheduler more efficient, we execute load balancing function in every 2 seconds.
> > It is implemented in `wrr.c`, function `wrr_load_balance`.
> > A random processor picks two proccessors, which has the biggest weight and the other has smallest weight.
> > Then, running proccessor picks a task to transfer from biggest to smallest.
> > After the transaction, processor with biggest weight should have bigger or same weight than another.
> > Locking and migrating tasks are implemented in function `wrr_load_balance`.
> > To do this, we did refer to `kernel/sched/fair.c` a lot.
> > Picking processor which performs load balancing is implemented in function `wrr_trigger_load_balance`.
> > To do this, we defined timestamp which is controlled by spin lock,
> > ```C
> > // jiffies of NEXT balance time
> > unsigned long wrr_next_balance;
> > ```

> > ### About System Calls
> > In this project, we implemented two system calls: `sched_setweight` and `sched_getweight`.
> > These system calls are implemented in `/kernel/sched/core.c`.
> > Both system call first locks the task of given pid (kernel perspective).
> > Then, they call functions in `kernel/sched/wrr.c`, `set_weight_wrr` and `get_weight_wrr`.
> > These functions work just as their names imply.
> > Note that both system calls adjust weight of process whose `struct task_struct`'s field `pid` is identical to parameter `pid`,
> > so that the only one task's weight will be changed or accessed.
> > Only root user, or the user who run the process can set weight of process.
> > Additionally, only root user can make weight larger than before.

> > ### About Other Modified Codes
> > In the file `kernel/sched/core.c`, we modified some codes to our new scheduler can work properly.
> > For example, system call `sched_setscheduler` should support our WRR scheduler.

> > We added few test codes in folder `test`.
> > 1. `fork.c` tests if weight is inherited for forked tasks.
> > 1. `multischedule.c` tests system call `sched_setscheduler` and `sched_getscheduler`.
> > 1. `multithread.c` tests system call `sched_setweight` and `sched_getweight` in mutithread programs.

> ## Investigation
> We did trial division method to factorize number `TODO` for each process.
> We measured total execution time of number of forked processes with same weight.
> We did experimented with changing weights and number of processes.
> Test code is in `test/workload.c`, and `test/test_shell.sh`.

> First, we fixed number of processes, and changed weight.

> ![proj3_p1.png](./images/proj3_p1.png)

> If the number of process is 1, they took almost identical times.

> ![proj3_p60.png](./images/proj3_p60.png)

> However, if we increased number of processes, small weight cases took much times.
> The reason we thought is context switch overhead.
> If the weight is small, context switch occures more than large weight cases.
> We also notice that total excution time is not 60 times than one-process case.
> We have 8 cores for artik 10, so it took less time.

> ![proj3_p100.png](./images/proj3_p100.png)

> If we increase number of process much more, gap of excution times between weight 1 and weight 4 increased.
> Total excution time was increased, so number of context switch was also increased.

> For now, we fixed weight, and changed number of processes.

> ![proj3_w1.png](./images/proj3_w1.png)

> We can notice that total execution time is linearly increasing w.r.t. number of processes.

> ![proj3_w12.png](./images/proj3_w12.png)
> ![proj3_w20.png](./images/proj3_w20.png)

> However, if we increased weight, number of context switch is decreased, so total excution times were decreased.
> But we can still notice linearity of execution times.

> ## Lessons Learned
> 1. Scheduler is so complicated and delicate.
> 1. Adding new feature is very hard! I read code much more than I wrote.
> 1. To print a lot of things in kernel leads to kernel panic. It makes hard to debug.