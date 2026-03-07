# everyLibC

**everyLibC** (`elibc`) is a maximally portable, self-contained C standard
library implementation. It is designed to replace the system libc when linked
statically (`-static`) or used in freestanding environments such as custom
kernels and bare-metal targets. All public symbols use standard C names so
existing code compiles against it without any changes.

---

## Architecture

Three independent layers. Each layer may only call downward, never upward.

```
┌──────────────────────────────────────────────────────┐
│  Layer 3 · io/          Formatted I/O                │
│  (printf, snprintf, dprintf — built on PAL only)     │
├──────────────────────────────────────────────────────┤
│  Layer 2 · pal/         Platform Abstraction Layer   │
│  linux/ — raw Linux x86-64 syscalls + mmap heap      │
├──────────────────────────────────────────────────────┤
│  Layer 1 · core/        Pure portable algorithms     │
│  string.h  math.h  ctype.h  stdlib.h  limits.h       │
│  (no system calls, no platform headers)              │
└──────────────────────────────────────────────────────┘
```

### Layer 1 — `core/`

Completely platform-independent. No system calls, no platform headers.

| Header | Functions |
|--------|-----------|
| `string.h` | `memset`, `memcpy`, `memmove`, `memcmp`, `memchr`, `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`, `strcmp`, `strncmp`, `strchr`, `strrchr`, `strstr` |
| `ctype.h` | `isalpha`, `isdigit`, `isalnum`, `isupper`, `islower`, `isspace`, `isprint`, `ispunct`, `isxdigit`, `iscntrl`, `toupper`, `tolower` |
| `math.h` | `fabs`, `floor`, `ceil`, `round`, `sqrt`, `pow`, `exp`, `log`, `log2`, `log10`, `sin`, `cos`, `tan`, `isinf`, `isnan` |
| `stdlib.h` | `abs`, `labs`, `atoi`, `atol`, `atof`, `strtol`, `strtod`, `itoa`, `malloc`, `calloc`, `realloc`, `free`, `exit`, `abort`, `qsort`, `bsearch` |
| `limits.h` | `INT_MIN`, `INT_MAX`, `LONG_MIN`, `LONG_MAX` |

Math is implemented algorithmically — no libm dependency:

- **`sqrt`** — Newton-Raphson (64 iterations)
- **`sin` / `cos`** — angle reduced to `[0, π/2]`, 12-term Taylor series
- **`log`** — range reduction to `[1, 2)`, atanh series (30 terms)
- **`exp`** — integer/fractional split, 20-term Taylor for fractional part
- **`pow`** — fast integer path; general path via `exp(y·log(x))`

> **Note:** `sin`/`cos`/`tan` lose accuracy for `|x| > ~4.5e15` due to
> double's 52-bit mantissa. Full-range accuracy requires Payne-Hanek
> reduction, which is not yet implemented.

### Layer 2 — `pal/`

The OS boundary. Only files in this layer may use platform headers or
inline assembly.

#### Linux x86-64 (`pal/linux/pal_linux.c`)

Raw `syscall` instruction via inline GCC/Clang assembly.

| Function | Syscall |
|----------|---------|
| `pal_write` | `write` (1) |
| `pal_read` | `read` (0) |
| `pal_exit` | `exit` (60) |
| `pal_alloc` / `pal_free` | `mmap` (9) — first-fit free-list heap, 16-byte aligned, 1 MiB chunks |

### Layer 3 — `io/`

`stdio.c` implements formatted I/O on top of `pal_write`. Core engine: `vsnprintf`.

Supported specifiers:

```
%d %i %u %ld %lu %lld %llu %s %c %x %X %p %f %zu %%
Flags: -  0  +     Width: decimal field     Precision: .N for %f
```

---

## Building

Requirements: **Meson ≥ 0.56**, **Ninja**, GCC or Clang with C11 support.

```sh
meson setup build -Dplatform=linux
meson compile -C build
meson test -C build --print-errorlogs
```

### Manual build (no Meson)

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -fno-builtin -ffreestanding \
    -Iinclude \
    core/string.c core/ctype.c core/math.c core/stdlib.c \
    pal/linux/pal_linux.c io/stdio.c \
    -r -o libelibc.a
```

### Adding a new platform

See [`docs/porting.md`](docs/porting.md). In short: implement the six
`pal_*` functions in `pal/<target>/pal_<target>.c` and wire it into Meson.

---

## Known limitations

| Area | Detail |
|------|--------|
| Thread safety | Heap is not thread-safe |
| Memory return | Heap never calls `munmap`; RSS grows monotonically |
| `sprintf` cap | Output silently truncated at 4096 bytes; use `snprintf` |
| errno | Not implemented |
| Locale | Assumes ASCII/C locale |
| Trig precision | `sin`/`cos`/`tan` inaccurate for `\|x\| > ~4.5e15` |

---

## License

BSD 3-Clause. See individual source files for the full notice.
