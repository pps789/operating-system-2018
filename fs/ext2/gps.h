#ifndef _EXT2_GPS_H
#define _EXT2_GPS_H

#include <linux/gps.h>

extern int is_near(const struct gps_location *g1, const struct gps_location *g2);

#endif /* _EXT2_GPS_H */
