/*
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#include <ctype.h>
#include <stdio.h>

#define ASSERT(cond)                                                 \
    do {                                                             \
        if (!(cond)) {                                               \
            printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond); \
            return 1;                                                \
        }                                                            \
    } while (0)

static int test_isalpha(void)
{
    ASSERT( isalpha('a'));
    ASSERT( isalpha('z'));
    ASSERT( isalpha('A'));
    ASSERT( isalpha('Z'));
    ASSERT(!isalpha('0'));
    ASSERT(!isalpha(' '));
    ASSERT(!isalpha('!'));
    ASSERT(!isalpha('\0'));
    return 0;
}

static int test_isdigit(void)
{
    ASSERT( isdigit('0'));
    ASSERT( isdigit('9'));
    ASSERT(!isdigit('a'));
    ASSERT(!isdigit(' '));
    return 0;
}

static int test_isalnum(void)
{
    ASSERT( isalnum('a'));
    ASSERT( isalnum('Z'));
    ASSERT( isalnum('5'));
    ASSERT(!isalnum('_'));
    ASSERT(!isalnum(' '));
    return 0;
}

static int test_isupper_islower(void)
{
    ASSERT( isupper('A'));
    ASSERT( isupper('Z'));
    ASSERT(!isupper('a'));
    ASSERT(!isupper('0'));

    ASSERT( islower('a'));
    ASSERT( islower('z'));
    ASSERT(!islower('A'));
    ASSERT(!islower('0'));
    return 0;
}

static int test_isspace(void)
{
    ASSERT( isspace(' '));
    ASSERT( isspace('\t'));
    ASSERT( isspace('\n'));
    ASSERT( isspace('\r'));
    ASSERT( isspace('\f'));
    ASSERT( isspace('\v'));
    ASSERT(!isspace('a'));
    ASSERT(!isspace('0'));
    return 0;
}

static int test_isprint(void)
{
    ASSERT( isprint(' '));
    ASSERT( isprint('~'));
    ASSERT( isprint('A'));
    ASSERT(!isprint('\n'));
    ASSERT(!isprint('\0'));
    ASSERT(!isprint(0x7F));
    return 0;
}

static int test_ispunct(void)
{
    ASSERT( ispunct('!'));
    ASSERT( ispunct('.'));
    ASSERT( ispunct('-'));
    ASSERT(!ispunct('a'));
    ASSERT(!ispunct('0'));
    ASSERT(!ispunct(' '));
    return 0;
}

static int test_isxdigit(void)
{
    ASSERT( isxdigit('0'));
    ASSERT( isxdigit('9'));
    ASSERT( isxdigit('a'));
    ASSERT( isxdigit('f'));
    ASSERT( isxdigit('A'));
    ASSERT( isxdigit('F'));
    ASSERT(!isxdigit('g'));
    ASSERT(!isxdigit('G'));
    ASSERT(!isxdigit(' '));
    return 0;
}

static int test_iscntrl(void)
{
    ASSERT( iscntrl('\0'));
    ASSERT( iscntrl('\n'));
    ASSERT( iscntrl('\t'));
    ASSERT( iscntrl(0x1F));
    ASSERT( iscntrl(0x7F));
    ASSERT(!iscntrl(' '));
    ASSERT(!iscntrl('a'));
    return 0;
}

static int test_toupper_tolower(void)
{
    ASSERT(toupper('a') == 'A');
    ASSERT(toupper('z') == 'Z');
    ASSERT(toupper('A') == 'A');
    ASSERT(toupper('0') == '0');

    ASSERT(tolower('A') == 'a');
    ASSERT(tolower('Z') == 'z');
    ASSERT(tolower('a') == 'a');
    ASSERT(tolower('9') == '9');

    ASSERT(tolower(toupper('m')) == 'm');
    return 0;
}

int main(void)
{
    int r = 0;
    r |= test_isalpha();
    r |= test_isdigit();
    r |= test_isalnum();
    r |= test_isupper_islower();
    r |= test_isspace();
    r |= test_isprint();
    r |= test_ispunct();
    r |= test_isxdigit();
    r |= test_iscntrl();
    r |= test_toupper_tolower();

    if (r == 0) {
        printf("PASS  test_ctype\n");
    }
    return r;
}
