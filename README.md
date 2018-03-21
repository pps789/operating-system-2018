OS-team6
========
Project1
-----------

> ## Introduction
>  This project is the first project of Operating System, 2018 spring.
> ## How to build
> ./build
> ## High-level design & Implementation
>  We designed our system call to follow the steps below:
>> 1. `sys_ptree(380)` is called from user, `test_ptree.c`.
>> 2. `sys_ptree(380)` calls  `traverse_process` with initial values
>> 3. `traverse_process` runs recursively in pre-order, calling `record_process` on each recursion cycle. 
>> 4. `record_process` transforms `struct task_struct` form into `struct prinfo` form.

> ## Process tree investigation
> As you build kernel and run `test_ptree.c`,  you can see  process tree printed on the screen.
>  
> ## Lessons learned
>> 1. Be familiar with git.
>> 2. I should have took this  course before Call of Duty
