# everyLibC — Porting Guide

This guide explains how to add a new PAL target (e.g., FreeBSD, macOS,
bare-metal RISC-V) without touching any code outside `pal/`.

---

## What the PAL must implement

Six functions declared in `<elibc/pal.h>`:

```c
/* I/O */
ssize_t pal_write(int fd, const void *buf, size_t count);
ssize_t pal_read (int fd, void *buf,       size_t count);

/* Memory */
void *pal_alloc  (size_t size);
void *pal_realloc(void *ptr, size_t size);
void  pal_free   (void *ptr);

/* Process */
void pal_exit(int status); /* must not return */
```

### Contracts

| Function | Contract |
|----------|----------|
| `pal_write` | Write up to `count` bytes; return bytes written or negative on error. May return less than `count` (short write). |
| `pal_read` | Read up to `count` bytes; return bytes read (0 = EOF) or negative on error. |
| `pal_alloc(0)` | Return `NULL`. |
| `pal_alloc(n)` | Return a pointer to at least `n` bytes of **uninitialized** memory, or `NULL` on failure. |
| `pal_realloc(NULL, n)` | Behave like `pal_alloc(n)`. |
| `pal_realloc(ptr, 0)` | Free `ptr` and return `NULL`. |
| `pal_free(NULL)` | No-op. |
| `pal_exit` | Terminate the process; never returns. |

---

## Step-by-step

### 1. Create the source directory

```
pal/<target>/
    pal_<target>.c
    meson.build
```

### 2. Implement pal_<target>.c

The file must include only `<elibc/pal.h>`. No system libc headers.

Minimum template:

```c
/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <elibc/pal.h>

ssize_t pal_write(int fd, const void *buf, size_t count)
{
    /* TODO: call write() or equivalent */
    (void)fd; (void)buf; (void)count;
    return -1;
}

ssize_t pal_read(int fd, void *buf, size_t count)
{
    (void)fd; (void)buf; (void)count;
    return -1;
}

void *pal_alloc(size_t size)
{
    if (size == 0) { return NULL; }
    /* TODO: allocate memory */
    return NULL;
}

void *pal_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) { return pal_alloc(size); }
    if (size == 0)   { pal_free(ptr); return NULL; }
    /* TODO: resize */
    return NULL;
}

void pal_free(void *ptr)
{
    if (ptr == NULL) { return; }
    /* TODO: free memory */
}

void pal_exit(int status)
{
    /* TODO: terminate */
    (void)status;
    while (1) { /* prevent noreturn warning */ }
}
```

### 3. Create pal/<target>/meson.build

```meson
# everyLibC - <target> PAL build description
# Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
# SPDX-License-Identifier: BSD-3-Clause

<target>_pal_sources = files('pal_<target>.c')
```

Add an architecture guard if the target is CPU-specific:

```meson
if host_machine.cpu_family() != 'x86_64'
    error('pal/<target> requires x86_64. Got: ' + host_machine.cpu_family())
endif
```

### 4. Register the target in meson_options.txt

```meson
option(
    'platform',
    type    : 'combo',
    choices : ['linux', '<target>'],
    value   : 'linux',
    description : 'Target platform for the PAL.'
)
```

### 5. Add a branch in pal/meson.build

```meson
platform = get_option('platform')

if platform == 'linux'
    subdir('linux')
    pal_sources = linux_pal_sources
elif platform == '<target>'
    subdir('<target>')
    pal_sources = <target>_pal_sources
else
    error('Unknown platform: ' + platform)
endif
```

### 6. Build and test

```bash
meson setup build -Dplatform=<target>
meson compile -C build
meson test -C build --print-errorlogs
```

---

## Platform-specific notes

### Linux x86-64 (implemented)

Uses raw `syscall` instruction via inline assembly (GCC/Clang).
Heap backed by anonymous `mmap` (syscall 9).

### FreeBSD / OpenBSD / NetBSD

Syscall ABI is similar to Linux on x86-64 but numbers differ.
`write` = 4, `read` = 3, `mmap` = 197 (FreeBSD) / 197 (NetBSD).
The `MAP_ANONYMOUS` flag is `MAP_ANON` on BSDs.

### macOS (x86-64 / ARM64)

Syscalls are made via `libSystem` traps or the `syscall` instruction
with different numbering. Alternatively, link against `libSystem.dylib`
and call `write`/`read`/`mmap` via the C ABI — this requires a single
external symbol and avoids raw asm.

### Bare-metal (no OS)

- `pal_write` / `pal_read`: route to UART or semihosting.
- `pal_alloc` / `pal_free`: static arena or linker-section bump allocator.
- `pal_exit`: infinite loop or reset vector.

Example UART write (ARM Cortex-M3 PL011):

```c
#define UART0_DR  ((volatile unsigned int *)0x40011004)
#define UART0_FR  ((volatile unsigned int *)0x40011018)

ssize_t pal_write(int fd, const void *buf, size_t count)
{
    (void)fd;
    const unsigned char *p = (const unsigned char *)buf;
    size_t i;
    for (i = 0; i < count; i++) {
        while (*UART0_FR & (1u << 5)) { /* TX FIFO full */ }
        *UART0_DR = p[i];
    }
    return (ssize_t)count;
}
```

---

## Internal dependency rule

A PAL implementation **must not** include `<string.h>` or other core headers.
If memory copying is needed (e.g., in `pal_realloc`), include a local
byte-loop copy as done in `pal/linux/pal_linux.c`:

```c
static void alloc_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) { *d++ = *s++; }
}
```

This avoids circular link-time dependencies between `core/` and `pal/`.
