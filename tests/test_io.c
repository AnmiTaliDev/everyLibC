/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */


#include <stdio.h>
#include <string.h>

#define ASSERT(cond)                                                 \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                \
        }                                                            \
    } while (0)

static int test_sprintf_decimal(void)
{
    char buf[64];

    sprintf(buf, "%d", 0);
    ASSERT(strcmp(buf, "0") == 0);

    sprintf(buf, "%d", 42);
    ASSERT(strcmp(buf, "42") == 0);

    sprintf(buf, "%d", -42);
    ASSERT(strcmp(buf, "-42") == 0);

    sprintf(buf, "%+d", 7);
    ASSERT(strcmp(buf, "+7") == 0);

    sprintf(buf, "%u", 4294967295u);
    ASSERT(strcmp(buf, "4294967295") == 0);

    return 0;
}

static int test_sprintf_string(void)
{
    char buf[64];

    sprintf(buf, "%s", "hello");
    ASSERT(strcmp(buf, "hello") == 0);

    sprintf(buf, "%s", "");
    ASSERT(buf[0] == '\0');

    sprintf(buf, "<%s>", "world");
    ASSERT(strcmp(buf, "<world>") == 0);

    sprintf(buf, "%s", (const char *)NULL);
    ASSERT(strcmp(buf, "(null)") == 0);

    return 0;
}

static int test_sprintf_char(void)
{
    char buf[16];

    sprintf(buf, "%c", 'A');
    ASSERT(buf[0] == 'A' && buf[1] == '\0');

    sprintf(buf, "<%c>", '!');
    ASSERT(strcmp(buf, "<!>") == 0);

    return 0;
}

static int test_sprintf_hex(void)
{
    char buf[32];

    sprintf(buf, "%x", 0);
    ASSERT(strcmp(buf, "0") == 0);

    sprintf(buf, "%x", 255);
    ASSERT(strcmp(buf, "ff") == 0);

    sprintf(buf, "%X", 255);
    ASSERT(strcmp(buf, "FF") == 0);

    sprintf(buf, "%x", 0xDEADBEEFu);
    ASSERT(strcmp(buf, "deadbeef") == 0);

    return 0;
}

static int test_sprintf_float(void)
{
    char buf[64];

    sprintf(buf, "%f", 0.0);
    ASSERT(strcmp(buf, "0.000000") == 0);

    sprintf(buf, "%.2f", 3.14159);
    ASSERT(strcmp(buf, "3.14") == 0);

    sprintf(buf, "%.0f", 2.9);
    ASSERT(strcmp(buf, "2") == 0);

    sprintf(buf, "%f", -1.5);
    ASSERT(strcmp(buf, "-1.500000") == 0);

    return 0;
}

static int test_sprintf_width(void)
{
    char buf[32];

    sprintf(buf, "%5d", 42);
    ASSERT(strcmp(buf, "   42") == 0);

    sprintf(buf, "%-5d|", 42);
    ASSERT(strcmp(buf, "42   |") == 0);

    sprintf(buf, "%05d", 42);
    ASSERT(strcmp(buf, "00042") == 0);

    sprintf(buf, "%8s", "hi");
    ASSERT(strcmp(buf, "      hi") == 0);

    sprintf(buf, "%-8s|", "hi");
    ASSERT(strcmp(buf, "hi      |") == 0);

    return 0;
}

static int test_sprintf_percent(void)
{
    char buf[16];
    sprintf(buf, "100%%");
    ASSERT(strcmp(buf, "100%") == 0);
    return 0;
}

static int test_snprintf_bounds(void)
{
    char buf[8];

    int n = snprintf(buf, sizeof(buf), "%s", "Hello, world!");
    ASSERT(n == 13);
    ASSERT(buf[7] == '\0');
    ASSERT(strncmp(buf, "Hello, ", 7) == 0);

    n = snprintf(buf, 4, "%d", 123);
    ASSERT(n == 3);
    ASSERT(strcmp(buf, "123") == 0);

    return 0;
}

static int test_sprintf_long(void)
{
    char buf[32];

    sprintf(buf, "%ld", 1234567890L);
    ASSERT(strcmp(buf, "1234567890") == 0);

    sprintf(buf, "%ld", -1234567890L);
    ASSERT(strcmp(buf, "-1234567890") == 0);

    sprintf(buf, "%lu", 4294967295UL);
    ASSERT(strcmp(buf, "4294967295") == 0);

    return 0;
}

static int test_sprintf_pointer(void)
{
    char buf[32];
    int dummy = 0;
    sprintf(buf, "%p", (void *)&dummy);
    ASSERT(buf[0] == '0' && buf[1] == 'x');
    ASSERT(strlen(buf) >= 3);
    return 0;
}

static int test_sprintf_zu(void)
{
    char buf[32];
    sprintf(buf, "%zu", (size_t)1024);
    ASSERT(strcmp(buf, "1024") == 0);
    sprintf(buf, "%zu", (size_t)0);
    ASSERT(strcmp(buf, "0") == 0);
    return 0;
}

static int test_printf_smoke(void)
{
    int n = printf("smoke: %d %s %x %.2f\n", 1, "ok", 0xFF, 3.14);
    ASSERT(n > 0);
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_sprintf_decimal();
    r |= test_sprintf_string();
    r |= test_sprintf_char();
    r |= test_sprintf_hex();
    r |= test_sprintf_float();
    r |= test_sprintf_width();
    r |= test_sprintf_percent();
    r |= test_snprintf_bounds();
    r |= test_sprintf_long();
    r |= test_sprintf_pointer();
    r |= test_sprintf_zu();
    r |= test_printf_smoke();

    if (r == 0) {
        printf("PASS  test_io\n");
    }
    return r;
}
