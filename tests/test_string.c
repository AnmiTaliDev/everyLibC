/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */


#include <string.h>
#include <stdio.h>

#define ASSERT(cond)                                                 \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                \
        }                                                            \
    } while (0)

static int test_memset(void)
{
    char buf[16];
    memset(buf, 0, sizeof(buf));
    for (int i = 0; i < 16; i++) { ASSERT(buf[i] == '\0'); }

    memset(buf, 0xAB, 8);
    for (int i = 0; i < 8;  i++) { ASSERT((unsigned char)buf[i] == 0xABu); }
    for (int i = 8; i < 16; i++) { ASSERT(buf[i] == '\0'); }

    ASSERT(memset(NULL, 0, 4) == NULL);
    return 0;
}

static int test_memcpy(void)
{
    const char src[] = "Hello, world!";
    char       dst[32];

    memset(dst, 0, sizeof(dst));
    memcpy(dst, src, strlen(src) + 1);
    ASSERT(strcmp(dst, src) == 0);

    memcpy(dst, "XY", 2);
    ASSERT(dst[0] == 'X');
    ASSERT(dst[1] == 'Y');
    ASSERT(dst[2] == 'l');

    ASSERT(memcpy(NULL, src, 4) == NULL);
    return 0;
}

static int test_memmove(void)
{
    char buf[32] = "abcdefgh";

    memmove(buf + 2, buf, 6);
    ASSERT(buf[2] == 'a');
    ASSERT(buf[7] == 'f');

    memcpy(buf, "abcdefgh", 9);
    memmove(buf, buf + 2, 6);
    ASSERT(buf[0] == 'c');
    ASSERT(buf[5] == 'h');

    return 0;
}

static int test_memcmp(void)
{
    ASSERT(memcmp("abc", "abc", 3) == 0);
    ASSERT(memcmp("abc", "abd", 3)  < 0);
    ASSERT(memcmp("abd", "abc", 3)  > 0);
    ASSERT(memcmp("abc", "abcX", 3) == 0);
    ASSERT(memcmp(NULL, NULL, 0) == 0);
    return 0;
}

static int test_memchr(void)
{
    const char *s = "Hello";
    ASSERT(memchr(s, 'e', 5) == s + 1);
    ASSERT(memchr(s, 'z', 5) == NULL);
    ASSERT(memchr(s, 'H', 1) == s);
    ASSERT(memchr(NULL, 'a', 0) == NULL);
    return 0;
}

static int test_strlen(void)
{
    ASSERT(strlen("") == 0);
    ASSERT(strlen("a") == 1);
    ASSERT(strlen("Hello, world!") == 13);
    ASSERT(strlen(NULL) == 0);
    return 0;
}

static int test_strcpy(void)
{
    char buf[32];
    strcpy(buf, "Hello");
    ASSERT(strcmp(buf, "Hello") == 0);
    strcpy(buf, "");
    ASSERT(buf[0] == '\0');
    return 0;
}

static int test_strncpy(void)
{
    char buf[16];
    memset(buf, 0xFF, sizeof(buf));
    strncpy(buf, "Hi", 8);
    ASSERT(buf[0] == 'H');
    ASSERT(buf[1] == 'i');
    for (int i = 2; i < 8; i++) { ASSERT(buf[i] == '\0'); }
    return 0;
}

static int test_strcat(void)
{
    char buf[32];
    strcpy(buf, "Hello");
    strcat(buf, ", world!");
    ASSERT(strcmp(buf, "Hello, world!") == 0);
    return 0;
}

static int test_strncat(void)
{
    char buf[32];
    strcpy(buf, "Hello");
    strncat(buf, ", world! (extra)", 8);
    ASSERT(strncmp(buf, "Hello, wo", 9) == 0);
    return 0;
}

static int test_strcmp(void)
{
    ASSERT(strcmp("abc", "abc") == 0);
    ASSERT(strcmp("abc", "abd")  < 0);
    ASSERT(strcmp("abd", "abc")  > 0);
    ASSERT(strcmp("", "")       == 0);
    ASSERT(strcmp("a", "")       > 0);
    ASSERT(strcmp("", "a")       < 0);
    return 0;
}

static int test_strncmp(void)
{
    ASSERT(strncmp("abcd", "abce", 3) == 0);
    ASSERT(strncmp("abcd", "abce", 4)  < 0);
    ASSERT(strncmp("abce", "abcd", 4)  > 0);
    ASSERT(strncmp("abc", "abc", 0)   == 0);
    return 0;
}

static int test_strchr(void)
{
    const char *s = "Hello, world!";
    ASSERT(strchr(s, 'H') == s);
    ASSERT(strchr(s, '!') == s + 12);
    ASSERT(strchr(s, '\0') == s + 13);
    ASSERT(strchr(s, 'z') == NULL);
    return 0;
}

static int test_strrchr(void)
{
    const char *s = "abcabc";
    ASSERT(strrchr(s, 'a') == s + 3);
    ASSERT(strrchr(s, 'c') == s + 5);
    ASSERT(strrchr(s, 'z') == NULL);
    return 0;
}

static int test_strstr(void)
{
    const char *h = "the quick brown fox";
    ASSERT(strstr(h, "quick") == h + 4);
    ASSERT(strstr(h, "fox")   == h + 16);
    ASSERT(strstr(h, "cat")   == NULL);
    ASSERT(strstr(h, "")      == h);
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_memset();
    r |= test_memcpy();
    r |= test_memmove();
    r |= test_memcmp();
    r |= test_memchr();
    r |= test_strlen();
    r |= test_strcpy();
    r |= test_strncpy();
    r |= test_strcat();
    r |= test_strncat();
    r |= test_strcmp();
    r |= test_strncmp();
    r |= test_strchr();
    r |= test_strrchr();
    r |= test_strstr();

    if (r == 0) {
        printf("PASS  test_string\n");
    }
    return r;
}
