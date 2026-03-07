/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef CTYPE_H
#define CTYPE_H

int isalpha (int c);
int isdigit (int c);
int isalnum (int c);
int isupper (int c);
int islower (int c);
int isspace (int c);
int isprint (int c);
int ispunct (int c);
int isxdigit(int c);
int iscntrl (int c);

int toupper(int c);
int tolower(int c);

#endif /* CTYPE_H */
