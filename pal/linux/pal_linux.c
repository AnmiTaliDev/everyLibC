/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <elibc/pal.h>

#if !defined(__x86_64__) && !defined(__amd64__)
#  error "pal/linux/pal_linux.c is currently only supported on x86-64."
#endif

static long syscall1(long n, long a1)
{
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(n), "D"(a1)
        : "rcx", "r11", "memory"
    );
    return ret;
}

static long syscall3(long n, long a1, long a2, long a3)
{
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(n), "D"(a1), "S"(a2), "d"(a3)
        : "rcx", "r11", "memory"
    );
    return ret;
}

static long syscall6(long n, long a1, long a2, long a3,
                     long a4, long a5, long a6)
{
    long ret;
    register long r10 __asm__("r10") = a4;
    register long r8  __asm__("r8")  = a5;
    register long r9  __asm__("r9")  = a6;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "0"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory"
    );
    return ret;
}

#define PROT_READ      0x1
#define PROT_WRITE     0x2
#define MAP_PRIVATE    0x02
#define MAP_ANONYMOUS  0x20
#define MAP_FAILED     ((void *)(-1L))

static void *linux_mmap(size_t length)
{
    long ret = syscall6(9,
                        0L,
                        (long)length,
                        (long)(PROT_READ | PROT_WRITE),
                        (long)(MAP_PRIVATE | MAP_ANONYMOUS),
                        -1L,
                        0L);
    if (ret < 0 && ret > -4096L) {
        return MAP_FAILED;
    }
    return (void *)ret;
}


typedef struct AllocBlock {
    size_t            size;
    int               free;
    struct AllocBlock *next;
} AllocBlock;

#define ALLOC_ALIGN  16UL
#define HEADER_SIZE  (sizeof(AllocBlock))
#define CHUNK_SIZE   (1024UL * 1024UL)

#define ALIGN_UP(n) \
    (((size_t)(n) + (ALLOC_ALIGN - 1UL)) & ~(ALLOC_ALIGN - 1UL))

static AllocBlock *heap_head = (AllocBlock *)0;

static void alloc_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) { *d++ = *s++; }
}

static AllocBlock *heap_extend(size_t min_size)
{
    size_t needed = ALIGN_UP(HEADER_SIZE + min_size);
    size_t chunk  = (needed < CHUNK_SIZE) ? CHUNK_SIZE : needed;

    void *mem = linux_mmap(chunk);
    if (mem == MAP_FAILED || mem == (void *)0) {
        return (AllocBlock *)0;
    }

    AllocBlock *b = (AllocBlock *)mem;
    b->size = chunk - HEADER_SIZE;
    b->free = 1;
    b->next = (AllocBlock *)0;
    return b;
}

void *pal_alloc(size_t size)
{
    if (size == 0) { return NULL; }
    size = ALIGN_UP(size);

    AllocBlock *prev = (AllocBlock *)0;
    AllocBlock *cur  = heap_head;

    while (cur != (AllocBlock *)0) {
        if (cur->free && cur->size >= size) {
            size_t remainder = cur->size - size;
            if (remainder >= HEADER_SIZE + ALLOC_ALIGN) {
                AllocBlock *split = (AllocBlock *)((char *)cur + HEADER_SIZE + size);
                split->size = remainder - HEADER_SIZE;
                split->free = 1;
                split->next = cur->next;
                cur->size   = size;
                cur->next   = split;
            }
            cur->free = 0;
            return (char *)cur + HEADER_SIZE;
        }
        prev = cur;
        cur  = cur->next;
    }

    AllocBlock *nb = heap_extend(size);
    if (nb == (AllocBlock *)0) { return NULL; }

    if (prev != (AllocBlock *)0) { prev->next = nb; }
    else                         { heap_head  = nb; }

    size_t remainder = nb->size - size;
    if (remainder >= HEADER_SIZE + ALLOC_ALIGN) {
        AllocBlock *split = (AllocBlock *)((char *)nb + HEADER_SIZE + size);
        split->size = remainder - HEADER_SIZE;
        split->free = 1;
        split->next = nb->next;
        nb->size    = size;
        nb->next    = split;
    }
    nb->free = 0;
    return (char *)nb + HEADER_SIZE;
}

void pal_free(void *ptr)
{
    if (ptr == NULL) { return; }
    AllocBlock *b = (AllocBlock *)((char *)ptr - HEADER_SIZE);
    b->free = 1;

    while (b->next != (AllocBlock *)0 && b->next->free) {
        b->size += HEADER_SIZE + b->next->size;
        b->next  = b->next->next;
    }
}

void *pal_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) { return pal_alloc(size); }
    if (size == 0)   { pal_free(ptr); return NULL; }

    AllocBlock *b = (AllocBlock *)((char *)ptr - HEADER_SIZE);
    size = ALIGN_UP(size);

    if (b->size >= size) { return ptr; }

    void *new_ptr = pal_alloc(size);
    if (new_ptr == NULL) { return NULL; }

    size_t copy_size = (b->size < size) ? b->size : size;
    alloc_memcpy(new_ptr, ptr, copy_size);
    pal_free(ptr);
    return new_ptr;
}

ssize_t pal_write(int fd, const void *buf, size_t count)
{
    if (buf == NULL || count == 0) { return 0; }
    return (ssize_t)syscall3(1, (long)fd, (long)buf, (long)count);
}

ssize_t pal_read(int fd, void *buf, size_t count)
{
    if (buf == NULL || count == 0) { return 0; }
    return (ssize_t)syscall3(0, (long)fd, (long)buf, (long)count);
}


void pal_exit(int status)
{
    syscall1(60, (long)status);
    while (1) { }
}
