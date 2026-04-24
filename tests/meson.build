# Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
# SPDX-License-Identifier: BSD-3-Clause

test_names = [
    'string',
    'ctype',
    'math',
    'stdlib',
    'io',
]

foreach t : test_names
    test_exe = executable(
        'test_' + t,
        'test_' + t + '.c',
        link_with           : elibc,
        include_directories : inc,
        c_args              : common_c_args,
    )
    test(t, test_exe)
endforeach
