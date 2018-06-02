#include <linux/gps.h>
#include <linux/spinlock.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm-generic/uaccess.h>
#include <linux/unistd.h>

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

    spin_lock(&gps_spinlock);
    curr_gps = gps;
    spin_unlock(&gps_spinlock);

    return 0;
}

int get_gps_location(const char __user *pathname, struct gps_location __user *loc) {
    // TODO: implement this!
    return 0;
}
