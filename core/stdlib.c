/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <elibc/pal.h>


int abs(int x)
{
    return (int)((x < 0) ? -(unsigned int)x : (unsigned int)x);
}

long labs(long x)
{
    return (long)((x < 0L) ? -(unsigned long)x : (unsigned long)x);
}

long strtol(const char *s, char **endptr, int base)
{
    if (s == NULL) {
        if (endptr != NULL) { *endptr = NULL; }
        return 0L;
    }

    while (isspace((unsigned char)*s)) { s++; }

    int negative = 0;
    if (*s == '-')      { negative = 1; s++; }
    else if (*s == '+') { s++; }

    if (base == 0) {
        if (*s == '0') {
            s++;
            if (*s == 'x' || *s == 'X') { base = 16; s++; }
            else                         { base = 8; }
        } else {
            base = 10;
        }
    } else if (base == 16) {
        if (*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')) {
            s += 2;
        }
    }

    const char    *start   = s;
    unsigned long  uresult = 0UL;
    unsigned long  ubase   = (unsigned long)base;
    unsigned long  ulimit  = negative
                             ? (unsigned long)LONG_MAX + 1UL
                             : (unsigned long)LONG_MAX;
    int            overflow = 0;

    while (*s != '\0') {
        int digit;
        if      (*s >= '0' && *s <= '9') { digit = *s - '0'; }
        else if (*s >= 'a' && *s <= 'z') { digit = *s - 'a' + 10; }
        else if (*s >= 'A' && *s <= 'Z') { digit = *s - 'A' + 10; }
        else                             { break; }

        if (digit >= base) { break; }
        if (!overflow) {
            if (uresult > (ulimit - (unsigned long)digit) / ubase) {
                overflow = 1;
            } else {
                uresult = uresult * ubase + (unsigned long)digit;
            }
        }
        s++;
    }

    if (endptr != NULL) {
        *endptr = (char *)((s == start) ? start : s);
    }
    if (overflow) { return negative ? LONG_MIN : LONG_MAX; }
    return negative ? -(long)uresult : (long)uresult;
}

int atoi(const char *s)
{
    return (int)strtol(s, NULL, 10);
}

long atol(const char *s)
{
    return strtol(s, NULL, 10);
}

double strtod(const char *s, char **endptr)
{
    if (s == NULL) {
        if (endptr != NULL) { *endptr = NULL; }
        return 0.0;
    }

    while (isspace((unsigned char)*s)) { s++; }

    int negative = 0;
    if (*s == '-')      { negative = 1; s++; }
    else if (*s == '+') { s++; }

    {
        char c0 = (*s >= 'a' && *s <= 'z') ? (char)(*s - 32) : *s;
        char c1 = (*(s+1) >= 'a' && *(s+1) <= 'z') ? (char)(*(s+1) - 32) : *(s+1);
        char c2 = (*(s+2) >= 'a' && *(s+2) <= 'z') ? (char)(*(s+2) - 32) : *(s+2);
        if (c0 == 'N' && c1 == 'A' && c2 == 'N') {
            if (endptr != NULL) { *endptr = (char *)(s + 3); }
            double inf = 1e300 * 1e300;
            double nan = inf - inf;
            return negative ? -nan : nan;
        }
        if (c0 == 'I' && c1 == 'N' && c2 == 'F') {
            const char *p = s + 3;
            if ((*p == 'i' || *p == 'I') &&
                (*(p+1) == 'n' || *(p+1) == 'N') &&
                (*(p+2) == 'i' || *(p+2) == 'I') &&
                (*(p+3) == 't' || *(p+3) == 'T') &&
                (*(p+4) == 'y' || *(p+4) == 'Y')) {
                p += 5;
            }
            if (endptr != NULL) { *endptr = (char *)p; }
            double inf = 1e300 * 1e300;
            return negative ? -inf : inf;
        }
    }

    double result = 0.0;

    while (*s >= '0' && *s <= '9') {
        result = result * 10.0 + (double)(*s - '0');
        s++;
    }

    if (*s == '.') {
        s++;
        double factor = 0.1;
        while (*s >= '0' && *s <= '9') {
            result += (double)(*s - '0') * factor;
            factor *= 0.1;
            s++;
        }
    }

    if (*s == 'e' || *s == 'E') {
        const char *exp_start = s;
        s++;
        int exp_neg = 0;
        if (*s == '-')      { exp_neg = 1; s++; }
        else if (*s == '+') { s++; }
        if (*s < '0' || *s > '9') {
            s = exp_start;
        } else {
            int exp = 0;
            while (*s >= '0' && *s <= '9') {
                if (exp < 400) {
                    exp = exp * 10 + (*s - '0');
                }
                s++;
            }
            double factor = 1.0;
            int    i;
            for (i = 0; i < exp; i++) { factor *= 10.0; }
            if (exp_neg) { result /= factor; }
            else         { result *= factor; }
        }
    }

    if (endptr != NULL) { *endptr = (char *)s; }
    return negative ? -result : result;
}

double atof(const char *s)
{
    return strtod(s, NULL);
}

char *itoa(int n, char *buf, int base)
{
    if (buf == NULL)          { return NULL; }
    if (base < 2 || base > 36) { buf[0] = '\0'; return buf; }
    if (n == 0)                { buf[0] = '0'; buf[1] = '\0'; return buf; }

    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char tmp[34];
    int  pos      = 0;
    int  negative = 0;

    unsigned int u;
    if (n < 0 && base == 10) {
        negative = 1;
        u = (unsigned int)(-(n + 1)) + 1U;
    } else {
        u = (unsigned int)n;
    }

    while (u > 0) {
        tmp[pos++] = digits[u % (unsigned int)base];
        u /= (unsigned int)base;
    }

    int j = 0;
    if (negative) { buf[j++] = '-'; }
    while (pos > 0) { buf[j++] = tmp[--pos]; }
    buf[j] = '\0';
    return buf;
}

void *malloc(size_t size)
{
    return pal_alloc(size);
}

void *calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0)         { return NULL; }
    if (nmemb > (size_t)(-1) / size)     { return NULL; }
    size_t total = nmemb * size;
    void  *ptr   = pal_alloc(total);
    if (ptr != NULL) { memset(ptr, 0, total); }
    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    return pal_realloc(ptr, size);
}

void free(void *ptr)
{
    pal_free(ptr);
}


void exit(int status)
{
    pal_exit(status);
}

void abort(void)
{
    pal_exit(134);
}


static void swap_bytes(char *a, char *b, size_t size)
{
    char tmp;
    while (size--) {
        tmp  = *a;
        *a++ = *b;
        *b++ = tmp;
    }
}

static void qsort_r(char *base, size_t lo, size_t hi, size_t size,
                    int (*compar)(const void *, const void *))
{
    if (lo >= hi) { return; }

    char   *pivot = base + hi * size;
    size_t  i     = lo;
    size_t  j;

    for (j = lo; j < hi; j++) {
        if (compar(base + j * size, pivot) <= 0) {
            swap_bytes(base + i * size, base + j * size, size);
            i++;
        }
    }
    swap_bytes(base + i * size, base + hi * size, size);

    if (i > 0) { qsort_r(base, lo, i - 1, size, compar); }
    qsort_r(base, i + 1, hi, size, compar);
}

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *))
{
    if (base == NULL || nmemb <= 1 || size == 0 || compar == NULL) {
        return;
    }
    qsort_r((char *)base, 0, nmemb - 1, size, compar);
}


void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *))
{
    if (key == NULL || base == NULL ||
        nmemb == 0 || size == 0 || compar == NULL) {
        return NULL;
    }

    const char *arr = (const char *)base;
    size_t      lo  = 0;
    size_t      hi  = nmemb;

    while (lo < hi) {
        size_t      mid  = lo + (hi - lo) / 2;
        const char *elem = arr + mid * size;
        int         cmp  = compar(key, (const void *)elem);

        if (cmp == 0) { return (void *)elem; }
        if (cmp < 0)  { hi = mid; }
        else          { lo = mid + 1; }
    }
    return NULL;
}
