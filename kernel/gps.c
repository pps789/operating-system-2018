#include <linux/gps.h>
#include <linux/spinlock.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/unistd.h>
#include <linux/limits.h>
#include <linux/slab.h>

DEFINE_SPINLOCK(gps_spinlock);
struct gps_location curr_gps;

int set_gps_location(struct gps_location __user *loc) {
    struct gps_location gps;
    if (loc == NULL) return -EINVAL;
    if (access_ok(VERIFY_READ, loc, sizeof(struct gps_location)) == 0) return -EFAULT;
    if (copy_from_user(&gps, loc, sizeof(struct gps_location)) != 0) return -EFAULT;

    /* validate input */
    if (gps.lat_integer < -90) return -EINVAL;
    if (gps.lat_integer > 90) return -EINVAL;
    if (gps.lng_integer < -180) return -EINVAL;
    if (gps.lng_integer > 180) return -EINVAL;
    if (gps.lat_fractional < 0) return -EINVAL;
    if (gps.lat_fractional >= 1000000) return -EINVAL;
    if (gps.lng_fractional < 0) return -EINVAL;
    if (gps.lng_fractional >= 1000000) return -EINVAL;
    if ((gps.lat_integer == -90 || gps.lat_integer == 90)
            && (gps.lat_fractional > 0))
        return -EINVAL;
    if ((gps.lng_integer == -180 || gps.lng_integer == 180)
            && (gps.lng_fractional > 0))
        return -EINVAL;
    if (gps.accuracy < 0)
        return -EINVAL;

    spin_lock(&gps_spinlock);
    curr_gps = gps;
    spin_unlock(&gps_spinlock);

    return 0;
}

int get_gps_location(const char __user *pathname, struct gps_location __user *loc) {
    char *path_buff;
    struct path path;
    struct inode *inode;
    struct gps_location gps;
    int retval = 0;

    if (pathname == NULL || loc == NULL) return -EINVAL;
    if (access_ok(VERIFY_WRITE, loc, sizeof(struct gps_location)) == 0) return -EFAULT;

    /* get variable from user space! */
    path_buff = kmalloc(PATH_MAX, GFP_KERNEL);
    if (path_buff == NULL) return -EFAULT;

    if (strncpy_from_user(path_buff, pathname, PATH_MAX) <= 0) {
        retval = -EFAULT;
        goto out;
    }

    kern_path(pathname, LOOKUP_FOLLOW, &path);
    if (path.dentry == NULL) {
        retval = -EACCES;
        goto out;
    }

    inode = path.dentry->d_inode;
    if (inode == NULL) {
        retval = -EACCES;
        goto out;
    }

    if (inode->i_op == NULL) {
        retval = -EINVAL;
        goto out;
    }

    /* found inode. first check permissions... */
    if ((inode_permission(inode, MAY_READ)) < 0) {
        retval = -EACCES;
        goto out;
    }

    if (inode->i_op->get_gps_location == 0) {
        retval = -ENODEV;
        goto out;
    }

    if ((retval = inode->i_op->get_gps_location(inode, &gps)) < 0) {
        goto out;
    }

    if (copy_to_user(loc, &gps, sizeof(struct gps_location)) != 0) {
        retval = -EFAULT;
        goto out;
    }

out:
    kfree(path_buff);
    return retval;
}
