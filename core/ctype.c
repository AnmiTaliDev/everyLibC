/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

/*
 * core/ctype.c — character classification and conversion for the ASCII
 * subset (code points 0x00–0x7F).
 *
 * No system headers, no system calls, no lookup tables; all logic is
 * expressed as integer range comparisons so the compiler can inline
 * these trivially.
 */

#include <ctype.h>

int isalpha(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}

int isupper(int c)
{
    return (c >= 'A' && c <= 'Z');
}

int islower(int c)
{
    return (c >= 'a' && c <= 'z');
}

int isspace(int c)
{
    /* ' ', '\t', '\n', '\r', '\f', '\v' */
    return (c == ' ') || (c == '\t') || (c == '\n') ||
           (c == '\r') || (c == '\f') || (c == '\v');
}

int isprint(int c)
{
    /* Printable ASCII: 0x20 (space) through 0x7E (tilde) */
    return (c >= 0x20 && c <= 0x7E);
}

int ispunct(int c)
{
    /* Printable, not alphanumeric, not space */
    return isprint(c) && !isalnum(c) && (c != ' ');
}

int isxdigit(int c)
{
    return isdigit(c) ||
           (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

int iscntrl(int c)
{
    /* Control characters: 0x00–0x1F and DEL (0x7F) */
    return (c >= 0x00 && c <= 0x1F) || (c == 0x7F);
}

int toupper(int c)
{
    if (islower(c)) {
        return c - ('a' - 'A');
    }
    return c;
}

int tolower(int c)
{
    if (isupper(c)) {
        return c + ('a' - 'A');
    }
    return c;
}
