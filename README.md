OS-team6
========
Project4
--------

## Introduction
 This is the fourth project of the Operation Systems class, 2018 Spring.
 The goal of this project is to implement a special ext2 file system which authorize users with their gps location.


## How to build
1. To build kernel, type `./build`
1. To test, move on to `test` folder and type `make all`
1. Push proj4.fs file to ARTIK device and mount it. Use `mount -o loop -t ext2 [path]/proj4.fs [path]/proj4` command.

## High-Level Design
1. Add `gps.h` to `include/linux`. Construct `struct gps_location`.
1. Add `gps.c` to `kernel`. Implement system calls, `set_gps_location`, `get_gps_location`.
Also, define `struct gps_location curr_gps` which indicates current gps location in this file.
1. In `/include/linux/fs.h`, define two inode operations, `set_gps_location` and `get_gps_location` (different with system calls).
It makes bridge between linux VFS and ext2 (our stuff).
1. Add `gps.h` and `gps.c` to `fs/ext2` folder. Implement gps adjacency checking function in this c file.
1. Modify `namei.c`, `inode.c`, `file.c` in `fs/ext2` to suit for our new ext2 file system depending on gps location.
After this, files can be accessed by only users who are adjancent enough.
If files are modified, its gps information will be updated to `curr_gps`.
1. Modify `e2fsprogs` to support our own ext2 file system.

## Implementation

### About ext2 File System
Ext2 file system is a kind of multi-level indexed allocation file system.
Each Inode has pointers to indirect blocks, which contain pointers to data blocks.
Each indirect block contains (Block size/4) pointers. 
You can check what informations are inside the inode of ext2 file system by reading `fs/ext2/ext2.h`.

#### Storing Location Information
To implement our own, special ext2 file system, we added information about `struct gps_location` inside the ext2 inode in file `fs/ext2/ext2.h`.
In the `struct ext2_inode_info`, we add five fields.
```C
__u32 i_lat_integer;
__u32 i_lat_fractional;
__u32 i_lng_integer;
__u32 i_lng_fractional;
__u32 i_accuracy;
```
These five 32bit fields are information of files gps location.
Only users who are adjacent enough can open or modify the file.
Above struct is in-memory ext2 inode struct.
We have to store those values into disk, so we modified `struct ext2_inode`.
```C
__le32 i_lat_integer;
__le32 i_lat_fractional;
__le32 i_lng_integer;
__le32 i_lng_fractional;
__le32 i_accuracy;
```
Note that above five fields are all 32bit, little endian.
Logics of loading disk inode into in-memory inode is in function `ext2_iget` in the file `fs/ext2/inode.c`.
We modified it, and to support both little and big endian cpus, we used `le32_to_cpu` macros.

Conversely, logics of storing in-memory inode into disk inode is in function `ext2_write_inode` in the same file.
We also modified it, and used `cpu_to_le32` macros for same reason.

We also implemented two inode operation `set_gps_location` and `get_gps_location` in the file `fs/ext2/file.c`.

#### Permission Checking
Details of adjancey checking will be described later.
We added function `ext2_permission` in the file `fs/ext2/file.c` and inject to `const struct inode_operations ext2_file_inode_operations`.
In this function, we did both general permission and adjacency checking.

#### Updating Location Information
We tracked when `i_mtime` field of inode is changed, and inject for calling inode operation `set_gps_location`.
We changed few files in `fs`, `fs/ext2` folder.

### About Adjacency Check
  To check adjacency of two locations, we have calculate distance between two locations on sphere.
  We used taylor series to approximate the value of trigonometric functions: sin(x) and cos(x).
## Lessons Learned
1. File system is full of pointers

