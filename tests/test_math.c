/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

/*
 * tests/test_math.c — tests for core/math.c
 *
 * Returns 0 on full pass, 1 on any failure.
 * Floating-point comparisons use an absolute tolerance of 1e-9.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define ASSERT(cond)                                                 \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                \
        }                                                            \
    } while (0)

#define EPS 1e-9

/* Absolute-value tolerance comparison. */
static int near(double a, double b)
{
    double diff = a - b;
    if (diff < 0.0) { diff = -diff; }
    return diff < EPS;
}

static int test_fabs(void)
{
    ASSERT(near(fabs(3.14),  3.14));
    ASSERT(near(fabs(-3.14), 3.14));
    ASSERT(near(fabs(0.0),   0.0));
    return 0;
}

static int test_abs_labs(void)
{
    ASSERT(abs(5)   ==  5);
    ASSERT(abs(-5)  ==  5);
    ASSERT(abs(0)   ==  0);
    /* Near INT_MIN: verify no signed-overflow UB (fixed via unsigned cast). */
    ASSERT(abs(-2147483647) == 2147483647);
    ASSERT(labs(5L)  ==  5L);
    ASSERT(labs(-5L) ==  5L);
    return 0;
}

static int test_floor_ceil(void)
{
    ASSERT(near(floor(2.9),   2.0));
    ASSERT(near(floor(2.0),   2.0));
    ASSERT(near(floor(-2.1), -3.0));
    ASSERT(near(floor(-2.0), -2.0));

    ASSERT(near(ceil(2.1),    3.0));
    ASSERT(near(ceil(2.0),    2.0));
    ASSERT(near(ceil(-2.9),  -2.0));
    ASSERT(near(ceil(-2.0),  -2.0));
    return 0;
}

static int test_round(void)
{
    ASSERT(near(round(2.4),   2.0));
    ASSERT(near(round(2.5),   3.0));
    ASSERT(near(round(-2.4), -2.0));
    ASSERT(near(round(-2.5), -3.0));
    return 0;
}

static int test_sqrt(void)
{
    ASSERT(near(sqrt(0.0),   0.0));
    ASSERT(near(sqrt(1.0),   1.0));
    ASSERT(near(sqrt(4.0),   2.0));
    ASSERT(near(sqrt(9.0),   3.0));
    ASSERT(near(sqrt(2.0),   M_SQRT2));
    ASSERT(near(sqrt(0.25),  0.5));
    return 0;
}

static int test_pow(void)
{
    ASSERT(near(pow(2.0,  0.0),  1.0));
    ASSERT(near(pow(2.0,  1.0),  2.0));
    ASSERT(near(pow(2.0,  8.0),  256.0));
    ASSERT(near(pow(2.0, -1.0),  0.5));
    ASSERT(near(pow(3.0,  3.0),  27.0));
    ASSERT(near(pow(1.0, 1000.0), 1.0));
    return 0;
}

static int test_exp(void)
{
    ASSERT(near(exp(0.0),  1.0));
    ASSERT(near(exp(1.0),  M_E));
    ASSERT(near(exp(-1.0), 1.0 / M_E));
    ASSERT(near(exp(2.0),  M_E * M_E));
    ASSERT(near(exp(0.5),  1.6487212707)); /* sqrt(e) ≈ 1.6487212707 */
    return 0;
}

static int test_log(void)
{
    ASSERT(near(log(1.0),   0.0));
    ASSERT(near(log(M_E),   1.0));
    ASSERT(near(log(2.0),   M_LN2));
    ASSERT(near(log(0.5),  -M_LN2));
    ASSERT(near(log(M_E * M_E), 2.0));
    return 0;
}

static int test_log2_log10(void)
{
    ASSERT(near(log2(1.0),   0.0));
    ASSERT(near(log2(2.0),   1.0));
    ASSERT(near(log2(8.0),   3.0));
    ASSERT(near(log2(0.5),  -1.0));

    ASSERT(near(log10(1.0),    0.0));
    ASSERT(near(log10(10.0),   1.0));
    ASSERT(near(log10(100.0),  2.0));
    ASSERT(near(log10(0.1),   -1.0));
    return 0;
}

static int test_sin_cos(void)
{
    /* sin */
    ASSERT(near(sin(0.0),          0.0));
    ASSERT(near(sin(M_PI / 2.0),   1.0));
    ASSERT(near(sin(M_PI),         0.0));
    ASSERT(near(sin(3.0 * M_PI / 2.0), -1.0));
    ASSERT(near(sin(2.0 * M_PI),   0.0));
    ASSERT(near(sin(-M_PI / 2.0),  -1.0));

    /* cos */
    ASSERT(near(cos(0.0),          1.0));
    ASSERT(near(cos(M_PI / 2.0),   0.0));
    ASSERT(near(cos(M_PI),        -1.0));
    ASSERT(near(cos(2.0 * M_PI),   1.0));

    /* Pythagorean identity */
    double s = sin(1.2345);
    double c = cos(1.2345);
    ASSERT(near(s * s + c * c, 1.0));
    return 0;
}

static int test_tan(void)
{
    ASSERT(near(tan(0.0), 0.0));
    ASSERT(near(tan(M_PI / 4.0), 1.0));
    ASSERT(near(tan(-M_PI / 4.0), -1.0));
    /* Near π/2 the result must be a large finite or infinite value. */
    double t = tan(M_PI / 2.0);
    ASSERT(t > 1e9 || isinf(t));
    return 0;
}

static int test_classification(void)
{
    ASSERT(!isnan(0.0));
    ASSERT(!isnan(1.0));
    ASSERT(!isnan(-1.0));
    /* sqrt(-1) must return NaN (Inf - Inf via IEEE 754). */
    ASSERT(isnan(sqrt(-1.0)));

    ASSERT(!isinf(0.0));
    ASSERT(!isinf(1.0));
    ASSERT( isinf(1e300 * 1e300));   /* +Inf */
    ASSERT( isinf(-(1e300 * 1e300))); /* -Inf */
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_fabs();
    r |= test_abs_labs();
    r |= test_floor_ceil();
    r |= test_round();
    r |= test_sqrt();
    r |= test_pow();
    r |= test_exp();
    r |= test_log();
    r |= test_log2_log10();
    r |= test_sin_cos();
    r |= test_tan();
    r |= test_classification();

    if (r == 0) {
        printf("PASS  test_math\n");
    }
    return r;
}
