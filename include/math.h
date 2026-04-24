/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef MATH_H
#define MATH_H

#define M_PI     3.14159265358979323846
#define M_E      2.71828182845904523536
#define M_LOG2E  1.44269504088896340736
#define M_LOG10E 0.43429448190325182765
#define M_LN2    0.69314718055994530942
#define M_LN10   2.30258509299404568402
#define M_SQRT2  1.41421356237309504880

#define HUGE_VAL (1e300 * 1e300)

double fabs (double x);

double floor(double x);
double ceil (double x);
double round(double x);

double sqrt(double x);
double pow (double base, double exponent);

double exp  (double x);
double log  (double x);
double log2 (double x);
double log10(double x);

double sin(double x);
double cos(double x);
double tan(double x);

int isinf(double x);
int isnan(double x);

#endif
