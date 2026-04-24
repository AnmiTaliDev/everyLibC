# everyLibC

everyLibC (`elibc`) is a implementation of a C standard library subset. 

---

## Dependencies

- Meson >= 0.56
- Ninja
- A C11 compiler (GCC or Clang)

---

## Build

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

---

## Static linking

`meson install` installs `libelibc.a`, the headers, and a pkg-config file:

```sh
meson install -C build
gcc $(pkg-config --cflags elibc) myprogram.c $(pkg-config --libs elibc) -o myprogram
```

`pkg-config --cflags elibc` points at a private header directory, so
everyLibC's headers only apply to builds that request them explicitly, and
`libelibc.a` is placed ahead of the system libc on the link line, so matching
symbols such as `printf` or `malloc` resolve to everyLibC's implementations.

To drop the system libc entirely, link with `-nostdlib -static` and supply
your own process entry point (everyLibC does not provide a CRT/`_start`):

```sh
gcc -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -static \
    $(pkg-config --cflags elibc) myprogram.c $(pkg-config --libs elibc) \
    -e _start -o myprogram
```

To add a PAL for a new target, see [`docs/porting.md`](docs/porting.md).

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

## Acknowledgments

The layered `core`/`pal`/`io` design and the freestanding build approach
take cues from musl, dietlibc, and Cosmopolitan libc.

---

## Documentation

- [Architecture](docs/architecture.md)
- [API reference](docs/api.md)
- [Porting guide](docs/porting.md)
- [Roadmap](ROADMAP.md)

---

## License

BSD 3-Clause. See [LICENSE](LICENSE).
