/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

/*
 * core/math.c — algorithmic floating-point mathematics.
 *
 * All functions are implemented without FPU-specific intrinsics or
 * libm symbols.  Techniques used:
 *   sqrt  — Newton-Raphson iteration
 *   sin/cos — range reduction to [0, π/2] then Taylor series
 *   log   — range reduction to [1, 2), then atanh series
 *   exp   — range reduction to [0, 1), then Taylor series
 *   pow   — fast integer path; general path via exp(y·log(x))
 *
 * No system headers, no system calls.
 */

#include <math.h>

/* Large finite value used internally for overflow results. */
#define ELIBC_HUGE 1e300

/* Classification */

int isnan(double x)
{
    /* IEEE 754: NaN is the only value not equal to itself. */
    return x != x;
}

int isinf(double x)
{
    /* Infinity doubled equals itself; exclude zero explicitly. */
    return (x != 0.0) && (x + x == x);
}

/* Absolute value */

double fabs(double x)
{
    return (x < 0.0) ? -x : x;
}

/* Rounding */

/* 2^63: values at or above this threshold are already integral in double
 * (52-bit mantissa cannot represent a fractional part at this magnitude). */
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

/* Square root — Newton-Raphson */

double sqrt(double x)
{
    if (x < 0.0) {
        double inf = ELIBC_HUGE * ELIBC_HUGE; /* +Inf */
        return inf - inf;                      /* NaN (IEEE 754: Inf - Inf) */
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

/* Internal: base^n for non-negative integer n. */
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

/* Exponential e^x */

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

    /* Split x = n + r, n integer, r ∈ [0, 1). */
    int    n = (int)x;
    double r = x - (double)n;
    if (r < 0.0) {
        n--;
        r += 1.0;
    }

    /*
     * Taylor series for e^r, r ∈ [0, 1):
     *   e^r = 1 + r + r^2/2! + r^3/3! + ...
     * 20 terms give full double precision for r ∈ [0, 1).
     */
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

/* Natural logarithm ln(x) */

double log(double x)
{
    if (isnan(x)) {
        return x;
    }
    if (x == 0.0) {
        return -(ELIBC_HUGE * ELIBC_HUGE); /* -Inf: log(0) = -∞ */
    }
    if (x < 0.0) {
        double inf = ELIBC_HUGE * ELIBC_HUGE;
        return inf - inf; /* NaN: log of negative is undefined */
    }
    if (x == 1.0) {
        return 0.0;
    }

    /*
     * Reduce x to m * 2^k where m ∈ [1, 2).
     * ln(x) = ln(m) + k * ln(2)
     */
    int    k = 0;
    double m = x;
    while (m >= 2.0) { m *= 0.5; k++; }
    while (m < 1.0)  { m *= 2.0; k--; }

    /*
     * ln(m) via atanh identity for m ∈ [1, 2):
     *   ln(m) = 2 * atanh(u) = 2 * Σ u^(2n+1)/(2n+1)
     * where u = (m-1)/(m+1) ∈ [0, 1/3).
     * 30 terms give full double precision.
     */
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

/* Power base^exponent */

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
        /* C standard: pow(negative, non-integer) → NaN. */
        double inf = ELIBC_HUGE * ELIBC_HUGE;
        return inf - inf;
    }
    return exp(exponent * log(base));
}

/* Trigonometric functions
 *
 * Precision note: range reduction uses floating-point division by 2π.
 * For |x| > ~4.5e15 the 52-bit mantissa cannot represent the fractional
 * part of x/2π, so sin/cos/tan results become meaningless.
 * Full-range accuracy requires Payne-Hanek reduction (not implemented).
 */

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

/*
 * Taylor series for sin on [0, π/2]:
 *   sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ...
 * 12 terms give > 15 significant digits for |x| ≤ π/2.
 */
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
        return x - x; /* NaN */
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
        return x - x; /* NaN */
    }
    return sin(M_PI * 0.5 - x);
}

double tan(double x)
{
    double s = sin(x);
    double c = cos(x);
    /* Use a small threshold; Taylor series never gives exact 0.0 near π/2. */
    if (fabs(c) < 1e-10) {
        return (s >= 0.0) ? ELIBC_HUGE * ELIBC_HUGE : -(ELIBC_HUGE * ELIBC_HUGE);
    }
    return s / c;
}
