OS-team6
========
Project1
-----------

> ## Introduction
>  This project is the first project of Operating System, 2018 spring.
> ## How to build
> 1. To build kernel, `./build`
> 2. To test, move to test folder, and `make`. Then, runnable file `test_ptree` is generated. 
> 3. import `test_ptree` to ARTIK and run.
> ## High-level design & Implementation
>  We designed and implemented our system call to follow the steps below:
>> 1. `sys_ptree(380)` is called from user, `test_ptree.c`.
>> 2. `sys_ptree(380)` calls  `traverse_process` with initial values
>> 3. `traverse_process` runs recursively in pre-order, calling `record_process` on each recursion cycle using `list_for_each` macro. 
>> 4. `record_process` transforms `struct task_struct` form into `struct prinfo` form.
>> 5. After calling `record_process`, `traverse_process` saves current result to `struct trv_result`. `struct trv_result` contains of `int nr`, `int nr_max` and `strcut prinfo *buf`.
>> 6. After all `traverse_process` end, `sys_ptree(380)` returns the number of total process entries on process.
>> 7. `test_ptree.c` recieves result from `sys_ptree(380)`.  

> ## Process tree investigation
> As you build kernel and run `test_ptree.c`,  you can see  process tree printed on the screen.
>  
> ## Lessons learned
>> 1. Be familiar with git.
>> 2. I should have took this  course before Call of Duty
