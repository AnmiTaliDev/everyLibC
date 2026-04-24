/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef ELIBC_PAL_H
#define ELIBC_PAL_H

#include <stddef.h>

typedef long ssize_t;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t pal_write(int fd, const void *buf, size_t count);

ssize_t pal_read(int fd, void *buf, size_t count);

void *pal_alloc(size_t size);

void *pal_realloc(void *ptr, size_t size);

void pal_free(void *ptr);

_Noreturn void pal_exit(int status);

#endif
