OS-team6
========
Project1
--------

> ## Introduction
>  This project is the first project of Operating System, 2018 spring.

> ## How to build
> 1. To build kernel, type `./build`
> 2. To test, move to `test` folder, and type `make`. Then, runnable file `test_ptree` will be generated. 
> 3. import `test_ptree` to ARTIK using sdb instructions.
> 4. To run, type `/root/test_ptree` on ARTIK screen.

> ## High-level design & Implementation
>  We designed and implemented our system call to follow the steps below:
> 1. `sys_ptree(380)` is called from user, `test_ptree.c`.
> 2. `sys_ptree(380)` calls  `traverse_process` with initial values
> 3. `traverse_process` runs recursively in pre-order, calling `record_process` on each recursion cycle using `list_for_each` macro. 
> 4. `record_process` transforms `struct task_struct` form into `struct prinfo` form.
> 5. After calling `record_process`, `traverse_process` saves current result to `struct trv_result`. `struct trv_result` contains of `int nr`, `int nr_max` and `strcut prinfo *buf`.
> 6. After all `traverse_process` end, `sys_ptree(380)` returns the number of total process entries on process.
> 7. `test_ptree.c` recieves result from `sys_ptree(380)`. Then, it prints process tree on cosole. Additionally it prints number of copied entries and number of total entires.  
> The full out put is recorded in `output` file.

> ## Process tree investigation
> As you build kernel and run `test_ptree.c`,  you can see  process tree printed on the screen.
> There are 173 active processes. `swapper` process is the root of every other processes. And this process has two childeren: `systemd`and `kthreadd`. Evry other process is descendant of these two processes.
> To know about the full content of process tree, read `output`.

> ## Lessons learned
> 1. Be familiar with git.
> 2. I should have took this  course before the Call of Duty
