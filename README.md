OS-team6
========
Project2
---------

> ## Introduction
> This project is the second project of Operating System class, 2018 spring. The goal of this project is to build rotational lock.

> ## How to build
> 1. To build kernel, type `./build`
> 2. To test, move to `test` folder, and type `make`. Then, runnable file `trial` and `selector` will be generated.
> 3. Import `selector` and `trial` to ARTIK using sdb instructions.
> 4. To run ,type `/root/selector [int1] & /root/trial [int2]` on ARTIK screen.

> ## High-level design & Implementation
> We designed and implemented our system call following the steps below:
> 1. Inside `rotation.c`, We implemented 
