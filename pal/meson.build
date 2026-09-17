# Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
# SPDX-License-Identifier: BSD-3-Clause

platform = get_option('platform')

if platform == 'linux'
    subdir('linux')
    pal_sources = linux_pal_sources
elif platform == 'freebsd'
    subdir('freebsd')
    pal_sources = freebsd_pal_sources
else
    error('Unknown platform: ' + platform)
endif
