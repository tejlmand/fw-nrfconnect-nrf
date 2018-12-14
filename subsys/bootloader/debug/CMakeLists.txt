#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

set(EXTS ${ZEPHYR_BASE}/ext)
set(SEGGER ${EXTS}/debug/segger)

zephyr_library()

zephyr_library_sources(printk.c)
zephyr_link_interface_ifdef(CONFIG_SB_DEBUG_PORT_UART uart)
zephyr_link_interface_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT rtt)

add_subdirectory_ifdef(CONFIG_SB_DEBUG_PORT_UART uart)
add_subdirectory_ifdef(CONFIG_SB_DEBUG_PORT_SEGGER_RTT rtt)

zephyr_include_directories(
  ../include/debug
  )

zephyr_include_directories_ifdef(
  CONFIG_SB_DEBUG_PORT_SEGGER_RTT
  ${SEGGER}/rtt
  )
