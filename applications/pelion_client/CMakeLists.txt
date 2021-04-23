#
# Copyright (c) 2021 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

################################################################################

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project("PelionClient" VERSION 0.1)

################################################################################

target_sources(app PRIVATE
  src/main.c
  )

target_sources(app PRIVATE
  configuration/common/mbed_cloud_dev_credentials.c
  )

target_sources_ifdef(CONFIG_PELION_UPDATE_DEVELOPER app PRIVATE
  configuration/common/update_default_resources.c
  )

# Include application events and configuration headers
zephyr_library_include_directories(
  src/util
  src/events
  )

zephyr_include_directories(
  configuration/common
  configuration/${BOARD}
  )

# Application sources
add_subdirectory(src/events)
add_subdirectory(src/modules)
add_subdirectory(src/util)
