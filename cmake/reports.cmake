#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

add_custom_target(
  partition_manager_report
  COMMAND
  ${PYTHON_EXECUTABLE}
  # If partition manager does not exist, this line of code will cause python to
  # simply print an empty line when running:
  # ninja partition_manager_report
  $<$<NOT:$<BOOL:$<TARGET_PROPERTY:partition_manager,ENABLED>>>:-cprint>
  ${ZEPHYR_BASE}/../nrf/scripts/partition_manager_report.py
  --input $<TARGET_PROPERTY:partition_manager,PM_CONFIG_FILES>
  DEPENDS
  $<TARGET_PROPERTY:partition_manager,PM_DEPENDS>
  )

set_property(TARGET zephyr_property_target
             APPEND PROPERTY rom_report_DEPENDENCIES
             partition_manager_report
	     )
