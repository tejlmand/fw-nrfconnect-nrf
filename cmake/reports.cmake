#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

set(pm_config
  "$<$<TARGET_EXISTS:partition_manager>:$<TARGET_PROPERTY:partition_manager,PM_CONFIG_FILES>>")

set(pm_depends
  "$<<$<TARGET_EXISTS:partition_manager>:$<TARGET_PROPERTY:partition_manager,PM_DEPENDS>>")

add_custom_target(
  partition_manager_report
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${ZEPHYR_BASE}/../nrf/scripts/partition_manager_report.py
  --input ${pm_config}
  "$<$<NOT:$<TARGET_EXISTS:partition_manager>>:--quiet>"
  DEPENDS
  ${pm_depends}
  )

set_property(TARGET zephyr_property_target
             APPEND PROPERTY rom_report_DEPENDENCIES
             partition_manager_report
	     )
