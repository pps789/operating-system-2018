#include<iostream>
#include<cmath>
#include<algorithm>
#include<vector>
#include<cstdio>
using namespace std;

typedef long long ll;
struct myfloat{
    ll integer, fractional;
    void print() const{
        printf("%lld.%09lld", integer, fractional);
    }
};

const ll PRE = 1e9;

void add(myfloat *m, const myfloat *rhs) {
    //m->print(); printf("+"); rhs->print();
    m->integer += rhs->integer;
    m->fractional += rhs->fractional;
    if (m->fractional >= PRE) {
        m->fractional -= PRE;
        m->integer++;
    }
    //printf("="); m->print(); printf("\n");
}

void neg(myfloat *m) {
    m->fractional = -m->fractional;
    m->integer = -m->integer;
    if (m->fractional < 0) {
        m->fractional += PRE;
        m->integer--;
    }
}

void mult(myfloat *m, const myfloat *rhs) {
    //m->print(); printf("*"); rhs->print();
    ll last = m->fractional * rhs->fractional;

    m->fractional = (m->integer)*(rhs->fractional) + (m->fractional)*(rhs->integer);
    m->integer *= rhs->integer;

    ll carry = (m->fractional) < 0 ? -((-m->fractional-1)/PRE)-1 : m->fractional/PRE;
    m->integer += carry;
    m->fractional -= carry * PRE;

    m->fractional += last / PRE;
    carry = (m->fractional) < 0 ? -((-m->fractional-1)/PRE)-1 : m->fractional/PRE;
    m->integer += carry;
    m->fractional -= carry * PRE;

    if (last%PRE >= PRE/2) {
        m->fractional++;
        if (m->fractional >= PRE) {
            m->fractional -= PRE;
            m->integer++;
        }
    }
    //printf("="); m->print(); printf("\n");
}

const int APPROX_DEGREE = 20;

const myfloat DEG_TO_RAD = {0, 17453293};
const myfloat INVERSE_N[APPROX_DEGREE] = {
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
const myfloat ONE = {1, 0}, ZERO = {0, 0}, MONE = {-1, 0};
const myfloat TAYLOR_SIN[APPROX_DEGREE] = {
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE,
    ZERO, ONE, ZERO, MONE
};
const myfloat TAYLOR_COS[APPROX_DEGREE] = {
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO,
    ONE, ZERO, MONE, ZERO
};

myfloat do_taylor(const myfloat *m, const myfloat *taylor) {
    myfloat ret = {0, 0};
    myfloat acc = {1, 0}; // x^n/n!
    for(int i=0;i<APPROX_DEGREE;i++){
        myfloat cur = taylor[i];
        if (cur.integer || cur.fractional) {
            mult(&cur, &acc);
            add(&ret, &cur);
        }

        if(i+1<APPROX_DEGREE) {
            mult(&acc, m);
            mult(&acc, &INVERSE_N[i+1]);
        }
    }
    return ret;
}

myfloat mysin(const myfloat *);
myfloat mycos(const myfloat *);

// handle [0, 180].
myfloat mysin(const myfloat *m) {
    myfloat x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        return mycos(&x);
    }
    if (x.integer >= 45 && x.fractional) {
        myfloat ninety = {90, 0};
        neg(&x);
        add(&x, &ninety);
        return mycos(&x);
    }

    mult(&x, &DEG_TO_RAD);
    return do_taylor(&x, TAYLOR_SIN);
}

myfloat mycos(const myfloat *m) {
    myfloat x = *m;
    if (x.integer >= 90) {
        x.integer -= 90;
        myfloat ret = mysin(&x);
        neg(&ret);
        return ret;
    }
    if (x.integer >= 45 && x.fractional) {
        myfloat ninety = {90, 0};
        neg(&x);
        add(&x, &ninety);
        return mysin(&x);
    }
    mult(&x, &DEG_TO_RAD);
    return do_taylor(&x, TAYLOR_COS);
}
const double d_PI = atan2(1,0)*2;

myfloat spherical_cos(myfloat lat1, myfloat lng1, myfloat lat2, myfloat lng2) {
    myfloat beta, gamma;
    beta = gamma = {90, 0};
    neg(&lat1), neg(&lat2);
    add(&beta, &lat1), add(&gamma, &lat2);

    myfloat A = lng1;
    neg(&lng2);
    add(&A, &lng2);

    if (A.integer < 0) A.integer += 360;
    if (A.integer >= 180) {
        myfloat tmp = {360, 0};
        neg(&A);
        add(&A, &tmp);
    }

    myfloat cb = mycos(&beta), cg = mycos(&gamma);
    myfloat sb = mysin(&beta), sg = mysin(&gamma);
    myfloat cA = mycos(&A);

    myfloat f1 = cb, f2 = sb;
    mult(&f1, &cg);
    mult(&f2, &sg);
    mult(&f2, &cA);

    add(&f1, &f2);
    return f1;
}

int main() {
    myfloat lat1, lng1, lat2, lng2;

    cout<<"lat1"<<endl;
    cin>>lat1.integer>>lat1.fractional;

    cout<<"lng1"<<endl;
    cin>>lng1.integer>>lng1.fractional;

    cout<<"lat2"<<endl;
    cin>>lat2.integer>>lat2.fractional;

    cout<<"lng2"<<endl;
    cin>>lng2.integer>>lng2.fractional;

    myfloat result = spherical_cos(lat1, lng1, lat2, lng2);

    float real_lat1 = lat1.integer + 1. * lat1.fractional / PRE;
    float real_lat2 = lat2.integer + 1. * lat2.fractional / PRE;
    float real_lng1 = lng1.integer + 1. * lng1.fractional / PRE;
    float real_lng2 = lng2.integer + 1. * lng2.fractional / PRE;

    float beta = 90-real_lat1, gamma = 90-real_lat2;
    float A = abs(real_lng1 - real_lng2);
    A = min(A, 360-A);

    cout << "beta: " << beta << endl;
    cout << "gamma: " << gamma << endl;
    cout << "A: " << A << endl;

    beta *= d_PI/180, gamma *= d_PI/180, A *= d_PI/180;

    cout << "mine: "; result.print(); cout << endl;
    cout << "ref.: "; cout << cos(beta)*cos(gamma) + sin(beta)*sin(gamma)*cos(A);
    cout << endl;
}

