#
# Copyright (c) 2019 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(http_application_update)

# NORDIC SDK APP START
target_sources(app PRIVATE
src/main.c
)
target_compile_definitions(app PRIVATE
TEST
zephyr_library_compile_definitions
ZTEST
zephyr_include_directories
src
)
target_compile_options(app PRIVATE
src
)
# NORDIC SDK APP END
