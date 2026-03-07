# everyLibC — Architecture

## Overview

everyLibC is a portable, freestanding C library subset structured in three
independent layers. Each layer has strict dependency rules: lower layers never
import from higher layers.

```
┌─────────────────────────────────────────────┐
│  Application / tests                        │
├─────────────────────────────────────────────┤
│  io/          Formatted I/O (stdio.c)       │  ← depends on core + pal
├─────────────────────────────────────────────┤
│  core/        Portable algorithms           │  ← no OS calls
│    string.c   ctype.c   math.c   stdlib.c   │
├─────────────────────────────────────────────┤
│  pal/         Platform Abstraction Layer    │  ← only file that calls OS
│    linux/pal_linux.c                        │
└─────────────────────────────────────────────┘
```

---

## Layer 1 — core/

Pure portable C. No system headers, no syscalls, no PAL calls.

| File | Implements |
|------|-----------|
| `string.c` | `memcpy`, `memmove`, `memset`, `memcmp`, `memchr`, `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`, `strcmp`, `strncmp`, `strchr`, `strrchr`, `strstr` |
| `ctype.c` | `isalpha`, `isdigit`, `isalnum`, `isupper`, `islower`, `isspace`, `isprint`, `ispunct`, `isxdigit`, `iscntrl`, `toupper`, `tolower` |
| `math.c` | `fabs`, `abs`, `labs`, `floor`, `ceil`, `round`, `sqrt`, `pow`, `exp`, `log`, `log2`, `log10`, `sin`, `cos`, `tan`, `isinf`, `isnan` |
| `stdlib.c` | `strtol`, `atoi`, `atol`, `strtod`, `atof`, `itoa`, `malloc`, `calloc`, `realloc`, `free`, `exit`, `abort`, `qsort`, `bsearch` |

### Algorithms

- **sqrt** — Newton-Raphson (64 iterations, early exit on convergence).
- **sin / cos** — Range-reduce to `[0, π/2]` then Taylor series (12 terms,
  > 15 significant digits for `|x| ≤ π/2`).
- **log** — Reduce `x` to `m · 2^k` where `m ∈ [1, 2)`, then
  `ln(m) = 2·atanh(u)` via Σ `u^(2n+1)/(2n+1)` (30 terms).
- **exp** — Split `x = n + r`, `n` integer; compute `e^r` via Taylor (20
  terms), then multiply by `e^n` via fast exponentiation.
- **pow** — Fast integer path (repeated squaring); general path via
  `exp(y · log(x))`.
- **qsort** — Lomuto partition quicksort (recursive, in-place).
- **bsearch** — Standard iterative binary search.

### NULL pointer policy

All core functions guard against `NULL` input at the function boundary and
return a safe default (0, `NULL`, or the destination pointer) rather than
crashing. This is a non-standard extension — standard C would invoke undefined
behavior on `NULL` arguments to these functions.

---

## Layer 2 — pal/

The Platform Abstraction Layer is the **only** place in everyLibC that
interacts with the OS. Every other module calls `pal_*` functions.

### pal/linux/pal_linux.c (x86-64 only)

Implements six functions:

| Function | Syscall | Notes |
|----------|---------|-------|
| `pal_write` | `write` (1) | Full write loop handled by caller |
| `pal_read` | `read` (0) | Returns 0 on EOF |
| `pal_alloc` | `mmap` (9) | First-fit free-list heap |
| `pal_realloc` | — | `pal_alloc` + byte copy + `pal_free` |
| `pal_free` | — | Mark free + forward-coalesce |
| `pal_exit` | `exit` (60) | Does not return |

Raw syscalls use inline assembly with the x86-64 Linux ABI
(`rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`; clobbers `rcx`, `r11`).

### Heap allocator

```
mmap region:  [ Block | usable data | Block | usable data | ... ]
```

- **Header:** `size` (usable bytes), `free` flag, `*next` pointer.
- **Alignment:** All allocations aligned to 16 bytes (`ALLOC_ALIGN`).
- **Chunk size:** 1 MiB per `mmap` call (`CHUNK_SIZE`).
- **Strategy:** First-fit; split if remainder ≥ `HEADER_SIZE + ALLOC_ALIGN`.
- **Free:** Mark free + forward-coalesce consecutive free blocks.
- **Limitation:** Memory is never returned to the OS (`munmap` not called).

### Adding a new PAL target

1. Create `pal/<target>/pal_<target>.c` implementing the six `pal_*` functions.
2. Create `pal/<target>/meson.build` that sets `<target>_pal_sources`.
3. Add the target name to `meson_options.txt` choices.
4. Add a branch in `pal/meson.build`.

---

## Layer 3 — io/

Formatted I/O built exclusively on `pal_write` and `vsnprintf`.

### io/stdio.c

- **`vsnprintf`** is the central engine. It parses the format string,
  formats each argument into a temporary `char tmp[128]`, applies width and
  padding, and writes the result into the caller-supplied buffer with strict
  bounds (`size - 1` usable bytes + NUL).
- **`printf` / `dprintf`** format into a 4096-byte stack buffer,
  then call `write_all()` which loops over `pal_write` to handle partial
  writes.
- **`sprintf`** is a thin wrapper over `vsnprintf` capped at 4096 bytes.
  Use `snprintf` when the output length is uncertain.

---

## Header dependency graph

```
<stddef.h>   (compiler built-in)
  ↑
<elibc/pal.h>  → <stddef.h>
<string.h>     → <stddef.h>
<ctype.h>      (no deps)
<math.h>       (no deps)
<stdlib.h>     → <stddef.h>
<stdio.h>      → <stddef.h>, <stdarg.h>
<limits.h>     (no deps)
```

`<stdarg.h>` and `<stddef.h>` are compiler built-ins (not system library
headers) and are permitted in freestanding C11 environments (ISO/IEC
9899:2011 §4).

---

## Compile flags

| Flag | Reason |
|------|--------|
| `-std=c11` | Requires C11 for `_Static_assert`, `//` comments |
| `-ffreestanding` | Do not assume a hosted C runtime is available |
| `-Wall -Wextra -Wpedantic` | Enable all reasonable warnings |
| `-fno-builtin` | Prevent the compiler from replacing calls with libc equivalents |

---

## IEEE 754 requirement

All floating-point code requires IEEE 754 double-precision arithmetic.
The `isnan` and `isinf` predicates rely on the identities:
- `x != x` is true only for NaN.
- `x + x == x && x != 0.0` is true only for ±∞.

These identities are guaranteed by IEEE 754 and are universally true on
x86-64, ARM64, and RISC-V. They may not hold on non-IEEE platforms.
