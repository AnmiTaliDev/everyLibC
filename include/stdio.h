/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

int putchar(int c);
int puts   (const char *s);

int printf (const char *fmt, ...);
int dprintf(int fd, const char *fmt, ...);

int sprintf (char *buf, const char *fmt, ...);
int snprintf(char *buf, size_t size, const char *fmt, ...);

int vprintf  (const char *fmt, va_list ap);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

#endif /* STDIO_H */
