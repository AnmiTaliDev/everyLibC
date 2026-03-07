/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

/*
 * tests/test_stdlib.c — tests for core/stdlib.c
 *
 * Returns 0 on full pass, 1 on any failure.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define ASSERT(cond)                                                 \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                \
        }                                                            \
    } while (0)

static int test_atoi(void)
{
    ASSERT(atoi("0")     ==  0);
    ASSERT(atoi("42")    ==  42);
    ASSERT(atoi("-42")   == -42);
    ASSERT(atoi("+7")    ==  7);
    ASSERT(atoi("  123") ==  123);
    ASSERT(atoi("99abc") ==  99);
    ASSERT(atoi("")      ==  0);
    return 0;
}

static int test_atol(void)
{
    ASSERT(atol("1000000000")  == 1000000000L);
    ASSERT(atol("-1000000000") == -1000000000L);
    return 0;
}

static int test_strtol(void)
{
    char *end;
    ASSERT(strtol("255",  &end, 10) == 255L);
    ASSERT(strtol("FF",   &end, 16) == 255L);
    ASSERT(strtol("0xFF", &end,  0) == 255L);
    ASSERT(strtol("010",  &end,  0) ==   8L);
    ASSERT(strtol("-1",   &end, 10) ==  -1L);
    ASSERT(strtol("11",   &end,  2) ==   3L);

    strtol("42abc", &end, 10);
    ASSERT(*end == 'a');

    /* Overflow must clamp to LONG_MAX / LONG_MIN, not wrap. */
    ASSERT(strtol("99999999999999999999",  NULL, 10) == LONG_MAX);
    ASSERT(strtol("-99999999999999999999", NULL, 10) == LONG_MIN);
    return 0;
}

static int test_atof(void)
{
    double v;

    v = atof("3.14");
    ASSERT(v > 3.13 && v < 3.15);

    v = atof("-2.5");
    ASSERT(v > -2.51 && v < -2.49);

    v = atof("1e3");
    ASSERT(v > 999.9 && v < 1000.1);

    v = atof("1.5e-2");
    ASSERT(v > 0.014 && v < 0.016);

    v = atof("0");
    ASSERT(v == 0.0);
    return 0;
}

static int test_itoa(void)
{
    char buf[64];

    ASSERT(strcmp(itoa(0,   buf, 10), "0")    == 0);
    ASSERT(strcmp(itoa(42,  buf, 10), "42")   == 0);
    ASSERT(strcmp(itoa(-42, buf, 10), "-42")  == 0);
    ASSERT(strcmp(itoa(255, buf, 16), "ff")   == 0);
    ASSERT(strcmp(itoa(10,  buf,  2), "1010") == 0);

    itoa(42, buf, 1);
    ASSERT(buf[0] == '\0');
    return 0;
}

static int test_malloc_free(void)
{
    void *p = malloc(64);
    ASSERT(p != NULL);

    unsigned char *b = (unsigned char *)p;
    for (int i = 0; i < 64; i++) { b[i] = (unsigned char)i; }
    for (int i = 0; i < 64; i++) { ASSERT(b[i] == (unsigned char)i); }

    free(p);

    ASSERT(malloc(0) == NULL);

    free(NULL); /* must not crash */
    return 0;
}

static int test_calloc(void)
{
    unsigned char *p = (unsigned char *)calloc(32, 1);
    ASSERT(p != NULL);
    for (int i = 0; i < 32; i++) { ASSERT(p[i] == 0); }
    free(p);

    ASSERT(calloc(0, 4) == NULL);
    ASSERT(calloc(4, 0) == NULL);
    return 0;
}

static int test_realloc(void)
{
    char *p = (char *)malloc(8);
    ASSERT(p != NULL);
    memcpy(p, "Hello", 6);

    char *p2 = (char *)realloc(p, 64);
    ASSERT(p2 != NULL);
    ASSERT(strcmp(p2, "Hello") == 0);

    void *p3 = realloc(p2, 0);
    ASSERT(p3 == NULL);

    void *p4 = realloc(NULL, 16);
    ASSERT(p4 != NULL);
    free(p4);

    return 0;
}

static int cmp_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

static int test_qsort(void)
{
    int arr[] = { 5, 3, 1, 4, 2 };
    qsort(arr, 5, sizeof(int), cmp_int);
    for (int i = 0; i < 4; i++) { ASSERT(arr[i] <= arr[i + 1]); }
    ASSERT(arr[0] == 1);
    ASSERT(arr[4] == 5);

    int one = 7;
    qsort(&one, 1, sizeof(int), cmp_int);
    ASSERT(one == 7);

    qsort(NULL, 0, sizeof(int), cmp_int);
    return 0;
}

static int test_bsearch(void)
{
    int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int key;

    key = 5;
    int *found = (int *)bsearch(&key, arr, 10, sizeof(int), cmp_int);
    ASSERT(found != NULL && *found == 5);

    key = 11;
    found = (int *)bsearch(&key, arr, 10, sizeof(int), cmp_int);
    ASSERT(found == NULL);

    key = 1;
    found = (int *)bsearch(&key, arr, 10, sizeof(int), cmp_int);
    ASSERT(found != NULL && *found == 1);

    key = 10;
    found = (int *)bsearch(&key, arr, 10, sizeof(int), cmp_int);
    ASSERT(found != NULL && *found == 10);

    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_atoi();
    r |= test_atol();
    r |= test_strtol();
    r |= test_atof();
    r |= test_itoa();
    r |= test_malloc_free();
    r |= test_calloc();
    r |= test_realloc();
    r |= test_qsort();
    r |= test_bsearch();

    if (r == 0) {
        printf("PASS  test_stdlib\n");
    }
    return r;
}
