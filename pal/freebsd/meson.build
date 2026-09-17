# Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
# SPDX-License-Identifier: BSD-3-Clause

if host_machine.cpu_family() != 'x86_64'
    error('pal/freebsd currently only supports x86_64. Got: ' + host_machine.cpu_family())
endif

freebsd_pal_sources = files('pal_freebsd.c')
