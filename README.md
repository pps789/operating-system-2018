OS-team6
========
Project4
--------

## Introduction
 This is the fourth project of the Operation Systems class, 2018 Spring.
 The goal of this project is to implement a special ext2 file system which authorize users with their gps location.


## How to build
1. To build kernel, type `./build`
2. To test, move on to `test` folder and type `make all`
3. Push proj4.fs file to ARTIK device and mount it. Use `mount -o loop -t ext2 [path]/proj4.fs [path]/proj4` command.

## High-Level Design
1. Add `gps.h` to `/include/linux`. Construct `struct gps_location` and `strcut gps_location curr_gps` in this header file.
2. Modify `ext2.h`, `inode.c`, `file,c`, in `/fs/ext2` to suit for our new ext2 file system depending on gps location.
3. In `/include/linux/fs.h`, add two system calls: `int (*set_gps_location)` and `int (*get_gps_location)`.
4. Add `gps.c` to `/fs/ext2` folder. Implement gps adjacency checking function in this c file.

## Implementation
### About ext2 File System
 Ext2 file system is a kind of multi-level indexed allocation file system. Each Inode has pointers to indirect blocks, which contain pointers to data blocks. Each indirect block contains (Block size/4) pointers. 
 You can check what informations are inside the inode of ext2 file system by reading `/fs/ext2/ext2.h`. To implement our own, special ext2 file system, we added information about `struct gps_location` inside the inode.
 ```
 __u32 i_lat_integer;
 __u32 i_lat_fractional;
 __u32 i_lng_integer;
 __u32 i_lng_fractional;
 __u32 i_accuracy;
 ```
  These five unsigned 32bit integers are information of files gps location. Only users who are adjacent enough can open or modify the file.

### About Adjacency Check
  To check adjacency of two locations, we have calculate distance between two locations on sphere.
  We used taylor series to approximate the value of trigonometric functions: sin(x) and cos(x).
## Lessons Learned
1. File system is full of pointers

