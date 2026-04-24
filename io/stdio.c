/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <stdio.h>
#include <string.h>
#include <elibc/pal.h>

static int write_all(int fd, const char *buf, size_t count)
{
    size_t written = 0;
    while (written < count) {
        ssize_t w = pal_write(fd, buf + written, count - written);
        if (w <= 0) { return -1; }
        written += (size_t)w;
    }
    return 0;
}

static int fmt_uint(char *buf, size_t bufsz, size_t pos,
                    unsigned long long val, unsigned int base, int upper_case)
{
    static const char lo[] = "0123456789abcdef";
    static const char up[] = "0123456789ABCDEF";
    const char *digs = upper_case ? up : lo;

    char tmp[64];
    int  len = 0;

    if (val == 0) {
        tmp[len++] = '0';
    } else {
        while (val > 0) {
            tmp[len++] = digs[val % base];
            val /= base;
        }
    }

    int written = 0;
    while (len > 0) {
        if (pos + (size_t)written + 1 < bufsz) {
            buf[pos + (size_t)written] = tmp[--len];
        } else {
            len--;
        }
        written++;
    }
    return written;
}

static int fmt_int(char *buf, size_t bufsz, size_t pos,
                   long long val, int flag_plus)
{
    int written = 0;

    if (val < 0) {
        if (pos + (size_t)written + 1 < bufsz) {
            buf[pos + (size_t)written] = '-';
        }
        written++;
        unsigned long long u = (unsigned long long)(-(val + 1)) + 1ULL;
        written += fmt_uint(buf, bufsz, pos + (size_t)written, u, 10, 0);
    } else {
        if (flag_plus) {
            if (pos + (size_t)written + 1 < bufsz) {
                buf[pos + (size_t)written] = '+';
            }
            written++;
        }
        written += fmt_uint(buf, bufsz, pos + (size_t)written,
                            (unsigned long long)val, 10, 0);
    }
    return written;
}

static int fmt_double(char *buf, size_t bufsz, size_t pos,
                      double val, int prec)
{
    int written = 0;

    if (val != val) {
        const char *s = "nan";
        while (*s) {
            if (pos + (size_t)written + 1 < bufsz) {
                buf[pos + (size_t)written] = *s;
            }
            written++;
            s++;
        }
        return written;
    }

    if (val < 0.0) {
        if (pos + (size_t)written + 1 < bufsz) {
            buf[pos + (size_t)written] = '-';
        }
        written++;
        val = -val;
    } else if (val + val == val && val != 0.0) {
        const char *s = "inf";
        while (*s) {
            if (pos + (size_t)written + 1 < bufsz) {
                buf[pos + (size_t)written] = *s;
            }
            written++;
            s++;
        }
        return written;
    }

    unsigned long long ipart = (unsigned long long)val;
    double             fpart = val - (double)ipart;

    written += fmt_uint(buf, bufsz, pos + (size_t)written, ipart, 10, 0);

    if (prec > 0) {
        if (pos + (size_t)written + 1 < bufsz) {
            buf[pos + (size_t)written] = '.';
        }
        written++;
        int p = prec;
        while (p-- > 0) {
            fpart *= 10.0;
            unsigned int digit = (unsigned int)fpart;
            if (pos + (size_t)written + 1 < bufsz) {
                buf[pos + (size_t)written] = (char)('0' + digit);
            }
            written++;
            fpart -= (double)digit;
        }
    }
    return written;
}

static int write_pad(char *buf, size_t bufsz, size_t pos,
                     int pad_char, int count)
{
    int written = 0;
    while (count-- > 0) {
        if (pos + (size_t)written + 1 < bufsz) {
            buf[pos + (size_t)written] = (char)pad_char;
        }
        written++;
    }
    return written;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    if (buf == NULL || size == 0) { return 0; }

    size_t pos = 0;

#define PUTC(c) \
    do { if (pos + 1 < size) { buf[pos] = (char)(c); } pos++; } while (0)

    while (*fmt != '\0') {
        if (*fmt != '%') { PUTC(*fmt++); continue; }
        fmt++;

        int flag_left = 0, flag_zero = 0, flag_plus = 0;
        for (;;) {
            if      (*fmt == '-') { flag_left = 1; fmt++; }
            else if (*fmt == '0') { flag_zero = 1; fmt++; }
            else if (*fmt == '+') { flag_plus = 1; fmt++; }
            else                  { break; }
        }
        if (flag_left) { flag_zero = 0; }

        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        int precision = -1;
        if (*fmt == '.') {
            fmt++;
            precision = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                precision = precision * 10 + (*fmt - '0');
                fmt++;
            }
        }

        int mod_ll = 0, mod_l = 0, mod_z = 0;
        if (*fmt == 'l') {
            fmt++;
            if (*fmt == 'l') { mod_ll = 1; fmt++; }
            else             { mod_l  = 1; }
        } else if (*fmt == 'z') {
            mod_z = 1;
            fmt++;
        }

        char spec = *fmt++;

        char   tmp[128];
        size_t tmp_len = 0;
        int    i;

        switch (spec) {
        case 'd':
        case 'i': {
            long long val;
            if (mod_ll)     { val = va_arg(ap, long long); }
            else if (mod_l) { val = (long long)va_arg(ap, long); }
            else            { val = (long long)va_arg(ap, int); }
            tmp_len = (size_t)fmt_int(tmp, sizeof(tmp), 0, val, flag_plus);
            break;
        }
        case 'u': {
            unsigned long long val;
            if (mod_ll)     { val = va_arg(ap, unsigned long long); }
            else if (mod_l) { val = (unsigned long long)va_arg(ap, unsigned long); }
            else if (mod_z) { val = (unsigned long long)va_arg(ap, size_t); }
            else            { val = (unsigned long long)va_arg(ap, unsigned int); }
            tmp_len = (size_t)fmt_uint(tmp, sizeof(tmp), 0, val, 10, 0);
            break;
        }
        case 'x':
        case 'X': {
            unsigned long long val;
            if (mod_ll)     { val = va_arg(ap, unsigned long long); }
            else if (mod_l) { val = (unsigned long long)va_arg(ap, unsigned long); }
            else            { val = (unsigned long long)va_arg(ap, unsigned int); }
            tmp_len = (size_t)fmt_uint(tmp, sizeof(tmp), 0, val, 16, spec == 'X');
            break;
        }
        case 'p': {
            unsigned long long val = (unsigned long long)(size_t)va_arg(ap, void *);
            tmp[0] = '0'; tmp[1] = 'x';
            int hex_len = fmt_uint(tmp + 2, sizeof(tmp) - 2, 0, val, 16, 0);
            tmp_len = (size_t)(2 + hex_len);
            break;
        }
        case 's': {
            const char *str = va_arg(ap, const char *);
            if (str == NULL) { str = "(null)"; }
            size_t slen = strlen(str);
            if (precision >= 0 && (size_t)precision < slen) {
                slen = (size_t)precision;
            }
            if (!flag_left && width > (int)slen) {
                pos += (size_t)write_pad(buf, size, pos, ' ', width - (int)slen);
            }
            for (size_t si = 0; si < slen; si++) { PUTC(str[si]); }
            if (flag_left && width > (int)slen) {
                pos += (size_t)write_pad(buf, size, pos, ' ', width - (int)slen);
            }
            continue;
        }
        case 'c': {
            char ch = (char)va_arg(ap, int);
            if (!flag_left && width > 1) {
                pos += (size_t)write_pad(buf, size, pos, ' ', width - 1);
            }
            PUTC(ch);
            if (flag_left && width > 1) {
                pos += (size_t)write_pad(buf, size, pos, ' ', width - 1);
            }
            continue;
        }
        case 'f': {
            double val  = va_arg(ap, double);
            int    prec = (precision >= 0) ? precision : 6;
            tmp_len = (size_t)fmt_double(tmp, sizeof(tmp), 0, val, prec);
            break;
        }
        case '%':
            PUTC('%');
            continue;
        default:
            PUTC('%');
            PUTC(spec);
            continue;
        }

        int  pad_needed = width - (int)tmp_len;
        char pad_char   = (flag_zero && !flag_left) ? '0' : ' ';

        if (!flag_left && pad_needed > 0) {
            if (flag_zero && tmp_len > 0 && (tmp[0] == '-' || tmp[0] == '+')) {
                PUTC(tmp[0]);
                for (i = 1; i < pad_needed; i++) { PUTC('0'); }
                for (size_t ti = 1; ti < tmp_len; ti++) { PUTC(tmp[ti]); }
            } else {
                pos += (size_t)write_pad(buf, size, pos, pad_char, pad_needed);
                for (size_t ti = 0; ti < tmp_len; ti++) { PUTC(tmp[ti]); }
            }
        } else {
            for (size_t ti = 0; ti < tmp_len; ti++) { PUTC(tmp[ti]); }
            if (flag_left && pad_needed > 0) {
                pos += (size_t)write_pad(buf, size, pos, ' ', pad_needed);
            }
        }
    }

#undef PUTC

    if (pos < size)     { buf[pos]        = '\0'; }
    else if (size > 0)  { buf[size - 1]   = '\0'; }

    return (int)pos;
}

int vprintf(const char *fmt, va_list ap)
{
    char buf[4096];
    int  n   = vsnprintf(buf, sizeof(buf), fmt, ap);
    size_t len = (n > 0 && (size_t)n < sizeof(buf))
                 ? (size_t)n : sizeof(buf) - 1;
    return (write_all(STDOUT_FILENO, buf, len) < 0) ? -1 : n;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vprintf(fmt, ap);
    va_end(ap);
    return n;
}

int dprintf(int fd, const char *fmt, ...)
{
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    size_t len = (n > 0 && (size_t)n < sizeof(buf))
                 ? (size_t)n : sizeof(buf) - 1;
    return (write_all(fd, buf, len) < 0) ? -1 : n;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, 4096, fmt, ap);
    va_end(ap);
    return n;
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return n;
}

int putchar(int c)
{
    char ch = (char)c;
    ssize_t r = pal_write(STDOUT_FILENO, &ch, 1);
    return (r == 1) ? (int)(unsigned char)ch : -1;
}

int puts(const char *s)
{
    if (s == NULL) { return -1; }
    size_t len = strlen(s);
    if (write_all(STDOUT_FILENO, s, len) < 0) { return -1; }
    char nl = '\n';
    if (write_all(STDOUT_FILENO, &nl, 1) < 0) { return -1; }
    return (int)len;
}
