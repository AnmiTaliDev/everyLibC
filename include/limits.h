/*
 * everyLibC - A maximally portable subset implementation of libc
 * Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/AnmiTaliDev/elibc
 */

#ifndef LIMITS_H
#define LIMITS_H

#define INT8_MIN    (-128)
#define INT8_MAX    127
#define UINT8_MAX   255U

#define INT16_MIN   (-32768)
#define INT16_MAX   32767
#define UINT16_MAX  65535U

#define INT32_MIN   (-2147483648)
#define INT32_MAX   2147483647
#define UINT32_MAX  4294967295U

#define INT64_MIN   (-9223372036854775807LL - 1LL)
#define INT64_MAX   9223372036854775807LL
#define UINT64_MAX  18446744073709551615ULL

#define INT_MIN     INT32_MIN
#define INT_MAX     INT32_MAX
#define UINT_MAX    UINT32_MAX

#define LONG_MIN    INT64_MIN
#define LONG_MAX    INT64_MAX
#define ULONG_MAX   UINT64_MAX

#endif /* LIMITS_H */
