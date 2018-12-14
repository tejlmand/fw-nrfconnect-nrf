#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#


# TODO Is it possible to usez zephyr's libc?
set(STD_LIB_C ${ZEPHYR_BASE}/lib/libc/minimal)
set(STD_SOURCE ${STD_LIB_C}/source)
set(STD_INCLUDE ${STD_LIB_C}/include)

zephyr_library()

zephyr_library_include_directories(
  ${STD_INCLUDE}
  ${STD_INCLUDE}/bits
  ${STD_INCLUDE}/sys
  )

zephyr_library_sources(
  ${STD_SOURCE}/stdlib/atoi.c
  ${STD_SOURCE}/stdlib/strtol.c
  ${STD_SOURCE}/stdlib/strtoul.c
  ${STD_SOURCE}/stdlib/malloc.c
  ${STD_SOURCE}/string/strncasecmp.c
  ${STD_SOURCE}/string/strstr.c
  ${STD_SOURCE}/string/string.c
  ${STD_SOURCE}/stdout/prf.c
  ${STD_SOURCE}/stdout/stdout_console.c
  ${STD_SOURCE}/stdout/sprintf.c
  ${STD_SOURCE}/stdout/fprintf.c
)

