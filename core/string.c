/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <string.h>


void *memset(void *s, int c, size_t n)
{
    if (s == NULL) {
        return NULL;
    }
    unsigned char *p = (unsigned char *)s;
    unsigned char  v = (unsigned char)c;
    while (n--) {
        *p++ = v;
    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    if (dst == NULL || src == NULL) {
        return dst;
    }
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
    if (dst == NULL || src == NULL || n == 0) {
        return dst;
    }
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    if (d < s || d >= s + n) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;
    while (n--) {
        if (*pa != *pb) {
            return (int)*pa - (int)*pb;
        }
        pa++;
        pb++;
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n)
{
    if (s == NULL) {
        return NULL;
    }
    const unsigned char *p   = (const unsigned char *)s;
    unsigned char        val = (unsigned char)c;
    while (n--) {
        if (*p == val) {
            return (void *)p;
        }
        p++;
    }
    return NULL;
}

size_t strlen(const char *s)
{
    if (s == NULL) {
        return 0;
    }
    const char *p = s;
    while (*p != '\0') {
        p++;
    }
    return (size_t)(p - s);
}

char *strcpy(char *dst, const char *src)
{
    if (dst == NULL || src == NULL) {
        return dst;
    }
    char *d = dst;
    while ((*d++ = *src++) != '\0') {
    }
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    if (dst == NULL || src == NULL) {
        return dst;
    }
    char       *d = dst;
    const char *s = src;
    size_t      i;

    for (i = 0; i < n; i++) {
        d[i] = s[i];
        if (s[i] == '\0') {
            i++;
            while (i < n) {
                d[i++] = '\0';
            }
            return dst;
        }
    }
    return dst;
}

char *strcat(char *dst, const char *src)
{
    if (dst == NULL || src == NULL) {
        return dst;
    }
    char *d = dst;
    while (*d != '\0') {
        d++;
    }
    while ((*d++ = *src++) != '\0') {
    }
    return dst;
}

char *strncat(char *dst, const char *src, size_t n)
{
    if (dst == NULL || src == NULL) {
        return dst;
    }
    char       *d = dst;
    const char *s = src;

    while (*d != '\0') {
        d++;
    }
    while (n > 0 && *s != '\0') {
        *d++ = *s++;
        n--;
    }
    *d = '\0';
    return dst;
}

int strcmp(const char *a, const char *b)
{
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    const unsigned char *ua = (const unsigned char *)a;
    const unsigned char *ub = (const unsigned char *)b;
    while (*ua != '\0' && *ua == *ub) {
        ua++;
        ub++;
    }
    return (int)*ua - (int)*ub;
}

int strncmp(const char *a, const char *b, size_t n)
{
    if (n == 0) {
        return 0;
    }
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    const unsigned char *ua = (const unsigned char *)a;
    const unsigned char *ub = (const unsigned char *)b;
    while (n-- > 0) {
        if (*ua != *ub) {
            return (int)*ua - (int)*ub;
        }
        if (*ua == '\0') {
            return 0;
        }
        ua++;
        ub++;
    }
    return 0;
}


char *strchr(const char *s, int c)
{
    if (s == NULL) {
        return NULL;
    }
    unsigned char ch = (unsigned char)c;
    while (*s != '\0') {
        if ((unsigned char)*s == ch) {
            return (char *)s;
        }
        s++;
    }
    if (ch == '\0') {
        return (char *)s;
    }
    return NULL;
}

char *strrchr(const char *s, int c)
{
    if (s == NULL) {
        return NULL;
    }
    unsigned char  ch   = (unsigned char)c;
    const char    *last = NULL;
    while (*s != '\0') {
        if ((unsigned char)*s == ch) {
            last = s;
        }
        s++;
    }
    if (ch == '\0') {
        return (char *)s;
    }
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle)
{
    if (haystack == NULL || needle == NULL) {
        return NULL;
    }
    if (*needle == '\0') {
        return (char *)haystack;
    }

    size_t nlen = strlen(needle);

    while (*haystack != '\0') {
        if (*haystack == *needle &&
            strncmp(haystack, needle, nlen) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }
    return NULL;
}
