#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

add_custom_command(
  TARGET rom_report
  POST_BUILD
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${ZEPHYR_BASE}/../nrf/scripts/partition_manager_report.py
  --input ${CMAKE_BINARY_DIR}/partitions.yml
  "$<$<NOT:$<TARGET_EXISTS:partition_manager>>:--quiet>"
  )
