#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

# This boilerplate is automatically included through ZephyrBuildConfig.cmake, found in
# ${NRF_DIR}/share/zephyrbuild-package/cmake/ZephyrBuildConfig.cmake
# For more information regarding the Zephyr Build Configuration CMake package, please refer to:
# https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/guides/zephyr_cmake_package.html#zephyr-build-configuration-cmake-package

include(${NRF_DIR}/boards/deprecated.cmake)

if(NOT BOARD)
        set(BOARD $ENV{BOARD})
endif()

# Check if selected board is supported.
if(DEFINED NRF_SUPPORTED_BOARDS)
        if(NOT BOARD IN_LIST NRF_SUPPORTED_BOARDS)
                message(FATAL_ERROR "board ${BOARD} is not supported")
        endif()
endif()

# Check if selected build type is supported.
if(DEFINED NRF_SUPPORTED_BUILD_TYPES)
        if(NOT CMAKE_BUILD_TYPE IN_LIST NRF_SUPPORTED_BUILD_TYPES)
                message(FATAL_ERROR "${CMAKE_BUILD_TYPE} variant is not supported")
        endif()
endif()

string(FIND "${BOARD}" "@" REVISION_SEPARATOR_INDEX)
if(NOT (REVISION_SEPARATOR_INDEX EQUAL -1))
  math(EXPR BOARD_REVISION_INDEX "${REVISION_SEPARATOR_INDEX} + 1")
  string(SUBSTRING ${BOARD} ${BOARD_REVISION_INDEX} -1 BOARD_REVISION)
  string(SUBSTRING ${BOARD} 0 ${REVISION_SEPARATOR_INDEX} BOARD)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/configuration)

  if((DEFINED CONF_FILE) AND ("${CONF_FILE}" MATCHES "^configuration/${BOARD}/"))
    # We have a relative conf file pointing inside configuration/<BOARD> folder
    # hence old custom scheme is in use, thus do nothing until all samples have
    # been updated to new scheme.
  else()
    set(APPLICATION_SOURCE_DIR ${CMAKE_SOURCE_DIR}/configuration/${BOARD}
        CACHE PATH "Application Configuration Directory" FORCE
    )
    set(KCONFIG_ROOT ${CMAKE_SOURCE_DIR}/Kconfig)

    if (NOT EXISTS "${APPLICATION_SOURCE_DIR}")
      message(FATAL_ERROR
              "Board ${BOARD} is not supported.\n"
              "Please make sure board specific configuration files are added to "
              "${CMAKE_CURRENT_SOURCE_DIR}/configuration/${BOARD}")
    endif()
  endif()
endif()
