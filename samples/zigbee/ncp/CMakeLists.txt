#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.13.1)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}/pm_static.yml")
  set(PM_STATIC_YML_FILE
      ${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}/pm_static.yml
  )
endif()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project("ZBOSS NCP SoC firmware")

target_sources(app PRIVATE
  src/main.c
)
