/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef ELIBC_PAL_H
#define ELIBC_PAL_H

/*
 * Platform Abstraction Layer (PAL) interface.
 *
 * This header defines the minimum OS-facing operations required by
 * everyLibC.  Only the PAL implementations (pal/linux/, pal/baremetal/,
 * etc.) may call into the host OS.  All other everyLibC code must
 * restrict itself to functions declared here.
 */

#include <stddef.h>

/* ssize_t is POSIX; define it here since we are freestanding. */
typedef long ssize_t;

/* Standard file descriptor numbers */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/*
 * pal_write — write up to `count` bytes from `buf` to file descriptor `fd`.
 * Returns the number of bytes written, or a negative error code.
 */
ssize_t pal_write(int fd, const void *buf, size_t count);

/*
 * pal_read — read up to `count` bytes from `fd` into `buf`.
 * Returns the number of bytes read (0 = EOF), or a negative error code.
 */
ssize_t pal_read(int fd, void *buf, size_t count);

/*
 * pal_alloc — allocate at least `size` bytes of uninitialized memory.
 * Returns NULL on failure or when size == 0.
 */
void *pal_alloc(size_t size);

/*
 * pal_realloc — resize the allocation pointed to by `ptr` to `size` bytes.
 * If `ptr` is NULL, behaves like pal_alloc(size).
 * If `size` is 0, frees `ptr` and returns NULL.
 * Returns NULL on failure.
 * Note: shrink (size < current) is accepted but the block is NOT split;
 * the surplus bytes remain allocated until pal_free is called.
 */
void *pal_realloc(void *ptr, size_t size);

/*
 * pal_free — release an allocation previously obtained from pal_alloc or
 * pal_realloc.  Passing NULL is a no-op.
 */
void pal_free(void *ptr);

/*
 * pal_exit — terminate the process with the given exit status.
 * This function does not return.
 */
_Noreturn void pal_exit(int status);

#endif /* ELIBC_PAL_H */
