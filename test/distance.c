#include<stdio.h>
#include<math.h>
#define LEN 1000000 
struct gps_location {
    int lat_integer;
    int lat_fractional;
    int lng_integer;
    int lng_fractional;
    int accuracy;
};
struct format { 
    int integer;
    int fractional;
};

void add(struct format *a, struct format *b) { 
    a->integer += b->integer;
    a->fractional += b->fractional;
    if (a->fractional >= LEN) { 
        a->integer++;
        a->fractional -= LEN;
    }
}
void neg(struct format *a) {
    a->integer = -a->integer;
    a->fractional = -a->fractional;
    if (a->fractional < 0) {
        a->integer--;
        a->fractional += LEN;
    }
}
void mul(struct format *a, const struct format *b) {
    int last = a->fractional * b->fractional;
    a->fractional = (a->integer) * (b->fractional) + (a->fractional) * (b->fractional);
    a->integer *= b->integer;

    int carry = (a->fractional) < 0 ? -((-a->fractional-1)/LEN)-1 : a->fractional/LEN;
    a->integer += carry;
    a->fractional -= carry * LEN;

    a->fractional += last / LEN;
    carry = (a->fractional) < 0 ? -((-a->fractional-1)/LEN)-1 : a->fractional/LEN;
    a->integer += carry;
    a->fractional -= carry * LEN;

    if (last%LEN >= LEN/2) {
        a->fractional++;
        if (a->fractional >= LEN) {
            a->fractional -= LEN;
            a->integer++;
        }
    }
}

#define APPROX_DEGREE 20
const struct format DEG_TO_RAD = {0, 174533};
const struct format INVERSE_N[APPROX_DEGREE] = {
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
struct format mysin(struct format *);
struct format mycos(struct format *);
struct format do_taylor(struct format *m, struct format *taylor) {
    struct format ret = {0, 0};
    struct format acc = {1, 0}; // x^n/n!
    for(int i=0;i<APPROX_DEGREE;i++){
        struct format cur = taylor[i];
        if (cur.integer || cur.fractional) {
            mul(&cur, &acc);
            add(&ret, &cur);
        }
 
        if(i+1<APPROX_DEGREE) {
            mul(&acc, m);
            mul(&acc, &INVERSE_N[i+1]);
        }
    }
    return ret;
}

struct format mysin(struct format *m) {
    struct format ONE = {1, 0}, ZERO = {0, 0}, MONE = {-1, 0};
    struct format TAYLOR_SIN[20] = {
        ZERO, ONE, ZERO, MONE,
        ZERO, ONE, ZERO, MONE,
        ZERO, ONE, ZERO, MONE,
        ZERO, ONE, ZERO, MONE,
        ZERO, ONE, ZERO, MONE
    };
    struct format x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        return mycos(&x);
    }
    if (x.integer >= 45 && x.fractional) {
        struct format ninety = {90, 0};
        neg(&x);
        add(&x, &ninety);
        return mycos(&x);
    }
 
    mul(&x, &DEG_TO_RAD);
    return do_taylor(&x, TAYLOR_SIN);
}
 
struct format mycos(struct format *m) {
    struct format ONE = {1, 0}, ZERO = {0, 0}, MONE = {-1, 0};
    struct format TAYLOR_COS[APPROX_DEGREE] = {
        ONE, ZERO, MONE, ZERO,
        ONE, ZERO, MONE, ZERO,
        ONE, ZERO, MONE, ZERO,
        ONE, ZERO, MONE, ZERO,
        ONE, ZERO, MONE, ZERO
    };
    struct format x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        struct format ret = mysin(&x);
        neg(&ret);
        return ret;
    }
    if (x.integer >= 45 && x.fractional) {
        struct format ninety = {90, 0};
  x      neg(&x);
        add(&x, &ninety);
        return mysin(&x);
    }
    mul(&x, &DEG_TO_RAD);
    return do_taylor(&x, TAYLOR_COS);
}

struct format spherical_cos(struct format lat1, struct format lng1, struct format lat2, struct format lng2) {
    struct format beta = {90, 0};
    struct format gamma = {90, 0};
    neg(&lat1), neg(&lat2);
    add(&beta, &lat1), add(&gamma, &lat2);
 
    struct format A = lng1;
    neg(&lng2);
    add(&A, &lng2);
 
    if (A.integer < 0) A.integer += 360;
    if (A.integer >= 180) {
        struct format tmp = {360, 0};
        neg(&A);
        add(&A, &tmp);
    }
 
    struct format cb = mycos(&beta), cg = mycos(&gamma);
    struct format sb = mysin(&beta), sg = mysin(&gamma);
    struct format cA = mycos(&A);
 
    struct format f1 = cb, f2 = sb;
    mul(&f1, &cg);
    mul(&f2, &sg);
    mul(&f2, &cA);
 
    add(&f1, &f2);
    return f1;
} 
int main() {
    struct format d_PI = {3, 141580};
    struct format lat1, lng1, lat2, lng2;
	
	printf("lat1\n");
	scanf("%d %d", &lat1.integer, &lat1.fractional); 

	printf("lng1\n");
	scanf("%d %d", &lng1.integer, &lng1.fractional); 

 
	printf("lat2\n");
	scanf("%d %d", &lat2.integer, &lat2.fractional); 
    
	printf("lng2\n");
	scanf("%d %d", &lng2.integer, &lng2.fractional); 

	struct format result = spherical_cos(lat1, lng1, lat2, lng2);
    printf("result : %d.%d\n", result.integer, result.fractional);

//    float real_lat1 = lat1.integer + 1. * lat1.fractional / LEN;
//    float real_lat2 = lat2.integer + 1. * lat2.fractional / LEN;
//    float real_lng1 = lng1.integer + 1. * lng1.fractional / LEN;
//    float real_lng2 = lng2.integer + 1. * lng2.fractional / LEN;
 
//    float beta = 90-real_lat1, gamma = 90-real_lat2;
//    float A = real_lng1 - real_lng2;
//    A = (A >= 0) ? A : (-1 * A);
//    A = (A <= 360-A) ? A : (360-A);
    
//    printf("beta: %d\n", beta);
//    printf("gamma: %d\n", gamma);
//    printf("A: %d\n", A);
 
//    beta *= d_PI/180, gamma *= d_PI/180, A *= d_PI/180;
 
  //  cout << "mine: "; result.print(); cout << endl;
  //  cout << "ref.: "; cout << cos(beta)*cos(gamma) + sin(beta)*sin(gamma)*cos(A);
  //  cout << endl;
    return 0;
}

