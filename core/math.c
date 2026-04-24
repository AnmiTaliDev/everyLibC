/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <math.h>

#define ELIBC_HUGE 1e300


int isnan(double x)
{
    return x != x;
}

int isinf(double x)
{
    return (x != 0.0) && (x + x == x);
}

double fabs(double x)
{
    return (x < 0.0) ? -x : x;
}

#define FLOOR_LIMIT 9.22337203685477580800e+18

double floor(double x)
{
    if (isnan(x) || isinf(x) || x >= FLOOR_LIMIT || x <= -FLOOR_LIMIT) {
        return x;
    }
    long long trunc = (long long)x;
    double    d     = (double)trunc;
    if (x < 0.0 && d != x) {
        d -= 1.0;
    }
    return d;
}

double ceil(double x)
{
    if (isnan(x) || isinf(x) || x >= FLOOR_LIMIT || x <= -FLOOR_LIMIT) {
        return x;
    }
    long long trunc = (long long)x;
    double    d     = (double)trunc;
    if (x > 0.0 && d != x) {
        d += 1.0;
    }
    return d;
}

double round(double x)
{
    if (x >= 0.0) {
        return floor(x + 0.5);
    }
    return ceil(x - 0.5);
}


double sqrt(double x)
{
    if (x < 0.0) {
        double inf = ELIBC_HUGE * ELIBC_HUGE;
        return inf - inf;
    }
    if (x == 0.0) {
        return 0.0;
    }

    double guess = x * 0.5;
    double prev;
    int    i;

    for (i = 0; i < 64; i++) {
        prev  = guess;
        guess = (guess + x / guess) * 0.5;
        if (fabs(guess - prev) < 1e-15 * guess) {
            break;
        }
    }
    return guess;
}

static double ipow(double base, int n)
{
    double result = 1.0;
    while (n > 0) {
        if (n & 1) {
            result *= base;
        }
        base *= base;
        n >>= 1;
    }
    return result;
}

double exp(double x)
{
    if (isnan(x)) {
        return x;
    }
    if (x > 709.0) {
        return ELIBC_HUGE * ELIBC_HUGE;
    }
    if (x < -745.0) {
        return 0.0;
    }

    int    n = (int)x;
    double r = x - (double)n;
    if (r < 0.0) {
        n--;
        r += 1.0;
    }

    double er   = 1.0;
    double term = 1.0;
    int    k;
    for (k = 1; k <= 20; k++) {
        term *= r / (double)k;
        er   += term;
    }

    double en = (n >= 0) ? ipow(M_E, n) : 1.0 / ipow(M_E, -n);
    return en * er;
}

double log(double x)
{
    if (isnan(x)) {
        return x;
    }
    if (x == 0.0) {
        return -(ELIBC_HUGE * ELIBC_HUGE);
    }
    if (x < 0.0) {
        double inf = ELIBC_HUGE * ELIBC_HUGE;
        return inf - inf;
    }
    if (x == 1.0) {
        return 0.0;
    }

    int    k = 0;
    double m = x;
    while (m >= 2.0) { m *= 0.5; k++; }
    while (m < 1.0)  { m *= 2.0; k--; }

    double u   = (m - 1.0) / (m + 1.0);
    double u2  = u * u;
    double sum = 0.0;
    double pw  = u;
    int    n;
    for (n = 0; n < 30; n++) {
        sum += pw / (double)(2 * n + 1);
        pw  *= u2;
    }

    return 2.0 * sum + (double)k * M_LN2;
}

double log2(double x)
{
    return log(x) * M_LOG2E;
}

double log10(double x)
{
    return log(x) * M_LOG10E;
}


double pow(double base, double exponent)
{
    if (isnan(exponent)) { return exponent; }
    if (exponent == 0.0) { return 1.0; }
    if (isnan(base))     { return base; }
    if (base == 0.0)     { return (exponent > 0.0) ? 0.0 : (ELIBC_HUGE * ELIBC_HUGE); }
    if (base == 1.0)     { return 1.0; }

    long long iexp = (long long)exponent;
    if ((double)iexp == exponent && iexp >= 0 && iexp < 10000) {
        return ipow(base, (int)iexp);
    }
    if ((double)iexp == exponent && iexp < 0 && iexp > -10000) {
        return 1.0 / ipow(base, (int)(-iexp));
    }

    if (base < 0.0) {
        double inf = ELIBC_HUGE * ELIBC_HUGE;
        return inf - inf;
    }
    return exp(exponent * log(base));
}

static double reduce_angle(double x)
{
    double two_pi = 2.0 * M_PI;
    if (x >= 0.0 && x < two_pi) {
        return x;
    }
    double k = floor(x / two_pi);
    x = x - k * two_pi;
    if (x < 0.0) {
        x += two_pi;
    }
    return x;
}

static double sin_core(double x)
{
    double result = 0.0;
    double term   = x;
    double x2     = x * x;
    int    n;
    for (n = 0; n < 12; n++) {
        result += term;
        term   *= -x2 / ((double)(2 * n + 2) * (double)(2 * n + 3));
    }
    return result;
}

double sin(double x)
{
    if (isnan(x) || isinf(x)) {
        return x - x;
    }

    double r        = reduce_angle(x);
    double half_pi  = M_PI * 0.5;
    double sign     = 1.0;

    if (r > M_PI) {
        r    = r - M_PI;
        sign = -1.0;
    }
    if (r > half_pi) {
        r = M_PI - r;
    }

    return sign * sin_core(r);
}

double cos(double x)
{
    if (isnan(x) || isinf(x)) {
        return x - x;
    }
    return sin(M_PI * 0.5 - x);
}

double tan(double x)
{
    double s = sin(x);
    double c = cos(x);
    if (fabs(c) < 1e-10) {
        return (s >= 0.0) ? ELIBC_HUGE * ELIBC_HUGE : -(ELIBC_HUGE * ELIBC_HUGE);
    }
    return s / c;
}
