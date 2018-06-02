#include <linux/gps.h>
#include <linux/spinlock.h>

DEFINE_SPINLOCK(gps_spinlock);
struct gps_location curr_gps;

int set_gps_location(struct gps_location __user *loc) {
    // TODO: implement this!
    return 0;
}

int get_gps_location(const char __user *pathname, struct gps_location __user *loc) {
    // TODO: implement this!
    return 0;
}
