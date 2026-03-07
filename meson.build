# everyLibC - A maximally portable subset implementation of libc
# Copyright (c) 2026 AnmiTaliDev <anmitalidev@nuros.org>
# SPDX-License-Identifier: BSD-3-Clause
# https://github.com/AnmiTaliDev/elibc

project(
    'elibc',
    'c',
    version         : '0.1.0',
    license         : 'BSD-3-Clause',
    default_options : [
        'c_std=c11',
        'warning_level=2',
    ],
    meson_version   : '>=0.56.0',
)

cc = meson.get_compiler('c')

common_c_args = [
    '-Wall',
    '-Wextra',
    '-Wpedantic',
    '-fno-builtin',
    '-ffreestanding',
]

inc = include_directories('include')

# Each subdir sets a source-list variable consumed below:
#   core/     -> core_sources
#   pal/      -> pal_sources
#   io/       -> io_sources
subdir('core')
subdir('pal')
subdir('io')

elibc = static_library(
    'elibc',
    sources             : core_sources + pal_sources + io_sources,
    include_directories : inc,
    c_args              : common_c_args,
    install             : false,
)

subdir('tests')
