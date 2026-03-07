# everyLibC — API Reference

All public symbols use standard C names. Headers are placed at `include/` and
are designed to shadow the system libc when everyLibC is linked statically.

---

## Types

everyLibC relies on the compiler-provided `<stddef.h>` for `size_t` and
`NULL`, and `<stdarg.h>` (a freestanding compiler built-in) for `va_list`.
No additional custom types are exposed in public headers.

The PAL header (`<elibc/pal.h>`) provides one POSIX type that is absent in
freestanding mode:

```c
typedef long ssize_t;
```

### Limits (`<limits.h>`)

| Constant | Value |
|----------|-------|
| `INT_MIN` | −2 147 483 648 |
| `INT_MAX` | 2 147 483 647 |
| `LONG_MIN` | −9 223 372 036 854 775 808 |
| `LONG_MAX` | 9 223 372 036 854 775 807 |

---

## String and memory (`<string.h>`)

All functions guard against `NULL` input at the function boundary and return a
safe default (0, `NULL`, or the destination pointer) rather than crashing.
This is a non-standard extension — standard C invokes undefined behavior on
`NULL` arguments.

### Memory

```c
void *memcpy (void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset (void *s,   int c,           size_t n);
int   memcmp (const void *a, const void *b, size_t n);
void *memchr (const void *s, int c,        size_t n);
```

**`memcmp` NULL extension:** if either pointer is `NULL`, returns 0 when both
are `NULL`, −1 when only `a` is `NULL`, +1 when only `b` is `NULL`.

### Strings

```c
size_t strlen (const char *s);
char  *strcpy (char *dst, const char *src);
char  *strncpy(char *dst, const char *src, size_t n);
char  *strcat (char *dst, const char *src);
char  *strncat(char *dst, const char *src, size_t n);
int    strcmp (const char *a, const char *b);
int    strncmp(const char *a, const char *b, size_t n);
char  *strchr (const char *s, int c);
char  *strrchr(const char *s, int c);
char  *strstr (const char *haystack, const char *needle);
```

**`strncpy` note:** when `src` is longer than `n` bytes, the result is **not**
null-terminated (POSIX semantics). Always verify the terminator if `src`
length is not known.

---

## Character classification (`<ctype.h>`)

All predicates return non-zero (true) or 0 (false). Pass `(unsigned char)c` to
avoid sign-extension issues.

```c
int isalpha (int c);   /* [A-Za-z] */
int isdigit (int c);   /* [0-9] */
int isalnum (int c);   /* [A-Za-z0-9] */
int isupper (int c);   /* [A-Z] */
int islower (int c);   /* [a-z] */
int isspace (int c);   /* space, \t, \n, \r, \f, \v */
int isprint (int c);   /* printable (0x20–0x7E) */
int ispunct (int c);   /* printable but not alnum or space */
int isxdigit(int c);   /* [0-9A-Fa-f] */
int iscntrl (int c);   /* control characters (< 0x20 or 0x7F) */

int toupper(int c);    /* convert lowercase to uppercase */
int tolower(int c);    /* convert uppercase to lowercase */
```

---

## Mathematics (`<math.h>`)

Requires IEEE 754 double-precision floating point.

### Constants

| Constant | Value |
|----------|-------|
| `M_PI` | π ≈ 3.14159265358979323846 |
| `M_E` | e ≈ 2.71828182845904523536 |
| `M_LN2` | ln(2) ≈ 0.69314718055994530942 |
| `M_LN10` | ln(10) ≈ 2.30258509299404568402 |
| `M_LOG2E` | log₂(e) ≈ 1.44269504088896340736 |
| `M_LOG10E` | log₁₀(e) ≈ 0.43429448190325182765 |
| `M_SQRT2` | √2 ≈ 1.41421356237309504880 |
| `HUGE_VAL` | +∞ (IEEE 754 positive infinity) |

### Functions

```c
double fabs (double x);            /* |x| */
int    abs  (int x);               /* |x|; safe on INT_MIN via unsigned cast */
long   labs (long x);              /* |x|; safe on LONG_MIN via unsigned cast */

double floor(double x);            /* largest integer ≤ x */
double ceil (double x);            /* smallest integer ≥ x */
double round(double x);            /* round half away from zero */

double sqrt(double x);             /* NaN for x < 0 */
double pow (double base, double exponent);

double exp  (double x);            /* e^x; +Inf for x > 709, 0 for x < -745 */
double log  (double x);            /* −Inf for x == 0, NaN for x < 0 */
double log2 (double x);            /* log₂(x) */
double log10(double x);            /* log₁₀(x) */

double sin(double x);              /* argument in radians */
double cos(double x);
double tan(double x);              /* ±Inf near π/2 + kπ */

int isinf(double x);               /* non-zero if x is ±∞ */
int isnan(double x);               /* non-zero if x is NaN */
```

**`abs(INT_MIN)` and `labs(LONG_MIN)`:** The mathematical absolute value
exceeds the range of the return type. The implementation avoids UB by casting
through the corresponding unsigned type; the result on two's-complement systems
wraps back to the minimum value — the same behavior as standard `abs(INT_MIN)`.

---

## Standard utilities (`<stdlib.h>`)

### String conversion

```c
int    atoi(const char *s);
long   atol(const char *s);
double atof(const char *s);

long   strtol(const char *s, char **endptr, int base);
double strtod(const char *s, char **endptr);

char  *itoa(int n, char *buf, int base);
```

**`strtol`** — base 0 auto-detects (0x prefix → 16, 0 prefix → 8, else 10).
Overflow clamps to `LONG_MAX` / `LONG_MIN` (no errno).

**`strtod`** — supports `[+-]digits[.digits][eE[+-]digits]`. Exponent clamped
to ±400 to prevent integer overflow. If no digit follows `e` or `eE±`, the `e`
is not consumed and `endptr` points before it.

**`itoa`** — non-standard. Base must be in [2, 36]; `buf` must hold at least
34 bytes for the base-2 representation of `INT_MIN`.

### Memory

```c
void *malloc (size_t size);
void *calloc (size_t nmemb, size_t size);  /* zeroed */
void *realloc(void *ptr, size_t size);
void  free   (void *ptr);
```

Memory is obtained from the PAL layer via `pal_alloc`. The heap is **not
thread-safe**. `calloc` returns `NULL` when `nmemb == 0` or `size == 0`.

### Process control

```c
void exit (int status);   /* calls pal_exit(status); does not return */
void abort(void);         /* calls pal_exit(134); does not return */
```

### Algorithms

```c
void  qsort  (void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));

void *bsearch(const void *key, const void *base,
              size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));
```

**`qsort`** — Lomuto partition quicksort. Not stable. Average O(n log n),
worst case O(n²) for adversarial input.

**`bsearch`** — Standard binary search on a sorted array. Returns a pointer to
a matching element, or `NULL` if not found.

---

## Formatted I/O (`<stdio.h>`)

### File descriptor constants

```c
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
```

### Format specifiers

| Specifier | Type | Notes |
|-----------|------|-------|
| `%d`, `%i` | `int` | signed decimal |
| `%u` | `unsigned int` | unsigned decimal |
| `%ld`, `%li` | `long` | |
| `%lu` | `unsigned long` | |
| `%lld`, `%lli` | `long long` | |
| `%llu` | `unsigned long long` | |
| `%zu` | `size_t` | |
| `%x` | `unsigned int` | lowercase hex |
| `%X` | `unsigned int` | uppercase hex |
| `%p` | `void *` | `0x`-prefixed lowercase hex |
| `%s` | `const char *` | `NULL` prints as `(null)` |
| `%c` | `int` | single character |
| `%f` | `double` | decimal notation |
| `%%` | — | literal `%` |

### Flags, width, precision

| Flag | Effect |
|------|--------|
| `-` | Left-align (overrides `0`) |
| `0` | Zero-pad (numeric only) |
| `+` | Always print sign |
| `N` | Minimum field width |
| `.N` | Precision (digits after `.` for `%f`) |

### Functions

```c
int printf (const char *fmt, ...);
int dprintf(int fd, const char *fmt, ...);

/* UNSAFE — no bounds check; caps at 4096 bytes. Use snprintf instead. */
int sprintf (char *buf, const char *fmt, ...);

/* Always null-terminates; returns full intended length (snprintf semantics). */
int snprintf(char *buf, size_t size, const char *fmt, ...);

int vprintf  (const char *fmt, va_list ap);
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

int putchar(int c);           /* write one byte to stdout */
int puts   (const char *s);   /* write string + newline to stdout */
```

**Return value:** `printf` / `dprintf` return the number of characters that
would have been written (before the 4096-byte cap), or −1 on write error.
`snprintf` / `vsnprintf` return the full intended length regardless of buffer
size (standard `snprintf` semantics).

---

## PAL interface (`<elibc/pal.h>`)

```c
ssize_t pal_write(int fd, const void *buf, size_t count);
ssize_t pal_read (int fd, void *buf,       size_t count);

void *pal_alloc  (size_t size);            /* uninitialized */
void *pal_realloc(void *ptr, size_t size);
void  pal_free   (void *ptr);

void  pal_exit(int status);                /* does not return */
```

`pal_write` and `pal_read` may return fewer bytes than requested (short
write/read on pipes, signals, etc.). The I/O layer handles this by retrying
via an internal `write_all()` loop.

---

## Known limitations

| Limitation | Detail |
|------------|--------|
| Thread safety | Heap is not thread-safe; no locking |
| Memory return | Heap never calls `munmap`; RSS grows monotonically |
| Float precision | `sprintf %f` loses subnormal values (< ~5e-324) |
| `sprintf` cap | Output silently truncated at 4096 bytes |
| errno | Not implemented (freestanding environment) |
| Locale | Not implemented; assumes ASCII/C locale |
| Signals | Not handled by the library |
