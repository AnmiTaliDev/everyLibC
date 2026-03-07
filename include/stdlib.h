/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

int  abs (int x);
long labs(long x);

int    atoi(const char *s);
long   atol(const char *s);
double atof(const char *s);

long   strtol(const char *s, char **endptr, int base);
double strtod(const char *s, char **endptr);

char *itoa(int n, char *buf, int base);

void *malloc (size_t size);
void *calloc (size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void  free   (void *ptr);

_Noreturn void exit (int status);
_Noreturn void abort(void);

void  qsort  (void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));

#endif /* STDLIB_H */
