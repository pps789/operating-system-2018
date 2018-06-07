#include <linux/math64.h>
#include "gps.h"

// precision is 1e9.
#define PRECISION 1000000000

// taylor expansion degree
#define APPROX_DEGREE 20

struct myfloat{
    long long integer, fractional;
};

// add @rhs into @m
static void add(struct myfloat *m, const struct myfloat *rhs) {
    m->integer += rhs->integer;
    m->fractional += rhs->fractional;
    if (m->fractional >= PRECISION) {
        m->fractional -= PRECISION;
        m->integer++;
    }
}

// negate @m
static void neg(struct myfloat *m) {
    m->fractional = -m->fractional;
    m->integer = -m->integer;
    if (m->fractional < 0) {
        m->fractional += PRECISION;
        m->integer--;
    }
}

// mutiply @rhs into @m
static void mult(struct myfloat *m, const struct myfloat *rhs) {
    long long last = m->fractional * rhs->fractional;
    long long carry;

    printk("%llu\n", div64_long(last, PRECISION));

    m->fractional = (m->integer)*(rhs->fractional) + (m->fractional)*(rhs->integer);
    m->integer *= rhs->integer;

    carry = (m->fractional) < 0 ? -div64_long(-m->fractional-1, PRECISION)-1 : div64_long(m->fractional, PRECISION);
    m->integer += carry;
    m->fractional -= carry * PRECISION;

    m->fractional += div64_long(last, PRECISION);
    carry = (m->fractional) < 0 ? -div64_long(-m->fractional-1, PRECISION)-1 : div64_long(m->fractional, PRECISION);
    m->integer += carry;
    m->fractional -= carry * PRECISION;

    if (last - div64_long(last, PRECISION)*PRECISION >= PRECISION/2) {
        m->fractional++;
        if (m->fractional >= PRECISION) {
            m->fractional -= PRECISION;
            m->integer++;
        }
    }
}

static const struct myfloat DEG_TO_RAD = {0, 17453293};
static const struct myfloat ACCURACY_TO_RAD = {0, 158};
static const struct myfloat INVERSE_N[APPROX_DEGREE] = {
    {1, 0}, // not use
    {1, 0},
    {0, 500000000},
    {0, 333333333},
    {0, 250000000},
    {0, 166666667},
    {0, 142857143},
    {0, 125000000},
    {0, 111111111},
    {0, 100000000},
    {0, 90909091},
    {0, 83333333},
    {0, 76923077},
    {0, 71428571},
    {0, 66666667},
    {0, 62500000},
    {0, 58823529},
    {0, 55555556},
    {0, 52631579}
};

#define ONE {1, 0}
#define ZERO {0, 0}
#define MONE {-1, 0}
static const struct myfloat TAYLOR_SIN[APPROX_DEGREE] = {
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE
};
static const struct myfloat TAYLOR_COS[APPROX_DEGREE] = {
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO
};

static void do_taylor(
        const struct myfloat *m, const struct myfloat *taylor, struct myfloat *out) {
    struct myfloat acc = {1, 0}; // x^n/n!
    int i;
    out->integer = out->fractional = 0;
    for(i=0;i<APPROX_DEGREE;i++){
        struct myfloat cur = taylor[i];
        if (cur.integer || cur.fractional) {
            mult(&cur, &acc);
            add(out, &cur);
        }

        if(i+1<APPROX_DEGREE) {
            mult(&acc, m);
            mult(&acc, &INVERSE_N[i+1]);
        }
    }
}

static void mysin(const struct myfloat *, struct myfloat *);
static void mycos(const struct myfloat *, struct myfloat *);

// handle [0, 180].
static void mysin(const struct myfloat *m, struct myfloat *out) {
    struct myfloat x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        mycos(&x, out);
        return;
    }
    if (x.integer >= 45 && x.fractional) {
        struct myfloat ninety = {90, 0};
        neg(&x);
        add(&x, &ninety);
        mycos(&x, out);
        return;
    }

    mult(&x, &DEG_TO_RAD);
    do_taylor(&x, TAYLOR_SIN, out);
}

static void mycos(const struct myfloat *m, struct myfloat *out) {
    struct myfloat x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        mysin(&x, out);
        neg(out);
        return;
    }
    if (x.integer >= 45 && x.fractional) {
        struct myfloat ninety = {90, 0};
        neg(&x);
        add(&x, &ninety);
        mysin(&x, out);
        return;
    }

    mult(&x, &DEG_TO_RAD);
    do_taylor(&x, TAYLOR_COS, out);
}

static void spherical_cos(
        const struct myfloat *lat1, const struct myfloat *lng1,
        const struct myfloat *lat2, const struct myfloat *lng2,
        struct myfloat *out) {
    struct myfloat beta = {90, 0};
    struct myfloat gamma = {90, 0};
    struct myfloat nlat1 = *lat1, nlat2 = *lat2;
    struct myfloat A = *lng1;
    struct myfloat B = *lng2;
    struct myfloat cb, cg, sb, sg, cA;
    struct myfloat f1, f2;

    neg(&nlat1), neg(&nlat2);
    add(&beta, &nlat1), add(&gamma, &nlat2);

    neg(&B);
    add(&A, &B);

    if (A.integer < 0) A.integer += 360;
    if (A.integer >= 180) {
        struct myfloat tmp = {360, 0};
        neg(&A);
        add(&A, &tmp);
    }

    mycos(&beta, &cb); mycos(&gamma, &cg);
    mysin(&beta, &sb); mysin(&gamma, &sg);
    mycos(&A, &cA);

    f1 = cb, f2 = sb;
    mult(&f1, &cg);
    mult(&f2, &sg);
    mult(&f2, &cA);

    add(&f1, &f2);

    *out = f1;
}

static void gps_to_myfloat(
        const struct gps_location *gps, struct myfloat *lat, struct myfloat *lng) {
    lat->integer = gps->lat_integer;
    lat->fractional = (long long)(gps->lat_fractional) * (PRECISION / 1000000);
    lng->integer = gps->lng_integer;
    lng->fractional = (long long)(gps->lng_fractional) * (PRECISION / 1000000);
}

static int compare_myfloat(const struct myfloat *a, const struct myfloat *b) {
    if (a->integer > b->integer) return 1;
    if (a->integer < b->integer) return -1;
    if (a->fractional > b->fractional) return 1;
    if (a->fractional < b->fractional) return -1;
    return 0;
}

int is_near(const struct gps_location *g1, const struct gps_location *g2) {
    struct myfloat lat1, lng1, lat2, lng2;
    struct myfloat accuracy;
    struct myfloat cos_len, cos_acc;
    gps_to_myfloat(g1, &lat1, &lng1);
    gps_to_myfloat(g2, &lat2, &lng2);

    accuracy.integer = g1->accuracy + g2->accuracy;
    accuracy.fractional = 0;

    mult(&accuracy, &ACCURACY_TO_RAD);
    if (accuracy.integer >= 180) return 1;

    spherical_cos(&lat1, &lng1, &lat2, &lng2, &cos_len);
    mycos(&accuracy, &cos_acc);

    if (compare_myfloat(&cos_len, &cos_acc) >= 0) return 1;
    return 0;
}
