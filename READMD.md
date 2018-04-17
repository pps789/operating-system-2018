OS-team6
========
Project1
--------

> ## Introduction
>  This project is the second project of Operating System, 2018 spring.
> The goal of this project is to implement rotaitional lock with our own hand.

> ## How to build
> 1. To build kernel, type `./build`
> 2. To test, move to `test` folder, and type `make all`. Then runnable file `rotd`, `selector` and `trial` will be generated.
> 3. Import `trial` and `selector` to ARTIK using sdb instructions.
> 4. To run two program simultaneously, type `/root/selector & /root/trial` on ARTIK screen.

> ## Lock policy
> 1. Read locks can be overlapped in same area, but write lock doesn't.
> 2. To prevent starvation, if write lock is in `pend_list`, two read locks can acquire lock before write lock acquires lock.

> ## High-level design
> 1. Make `ref_table` referencing degrees from 0 to 359. If certain degree has value larger than 0, that value stands for number of read locks of degree. If value is 0, there is no locks. If value is -1, write lock is occupying that degree. 
> 2. Make two `list_head`s which are head of `acq_list` and `pend_list` each.
> 3. Make `struct rot_lock_t` to store informations about rotational lock.
> 4. Make five system calls, `sys_set_rotation`, `sys_rotlock_read`, `sys_rotlock_write`, `sys_rotunlock_read` and `sys_rotunlock_write` with other supplementary functions.

> ## Implementation
	`spin_lock()` and `spin_unlock()` is used to lock thread during five system calls are executed.
> 1. set rotation
>	Change the current rotational degree to given degree, and call function `wake_up_candidates()` to wake up appropriate process.
> 2. read & write lock
>	Depending on given information about degree, range and lock type, check if lock can acquire certain degree by calling `lock_available()` function. If its available, call `rot_lock_t_add_into_acq()` function to acquire lock. `rot_lock_t_add_into_acq()` function manipulates information of `ref_counter` and add given `rot_lock_t` to `acq_list`. Else, given `rot_lock_t` enters sleep state, and added to `pend_list`. 
> 3. read & write unlock
>	Depending on given information about degree, range, and lock type, check if `rot_lock_t` equal to given information exists in `acq_list`. If exists, than remove it from `acq_list` and manipulates information of `ref_counter`. All these works are done by `rot_lock_remove()` function. 
>	Everytime system call is completed, `wake_up_candidate()` function is called to run next process.

> ## Lessons learned
> 1. Debugging is very very critical part of kernel coding. Do not ignore any doubtful points.
> 2. Time limit to drop is this thursday.
