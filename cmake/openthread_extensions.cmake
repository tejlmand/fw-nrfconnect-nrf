#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

set(NRFXLIB_DIR $ENV{ZEPHYR_BASE}/../nrfxlib)
assert_exists(NRFXLIB_DIR)
include(${NRFXLIB_DIR}/common.cmake)

set(OT_WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Store the configuration of the compiled OpenThread libraries
# and set source and destination paths.
function(openthread_libs_configuration_write)
  unset(OPENTHREAD_SRC_DIR CACHE)
  unset(OPENTHREAD_CONFIG_FILE CACHE)
  unset(OPENTHREAD_LIB_FTD CACHE)
  unset(OPENTHREAD_LIB_MTD CACHE)
  unset(OPENTHREAD_LIB_RADIO CACHE)
  unset(OPENTHREAD_LIB_CLI_FTD CACHE)
  unset(OPENTHREAD_LIB_CLI_MTD CACHE)
  unset(OPENTHREAD_LIB_NCP_FTD CACHE)
  unset(OPENTHREAD_LIB_NCP_MTD CACHE)
  unset(OPENTHREAD_LIB_RCP CACHE)
  unset(OPENTHREAD_DST_DIR CACHE)

  message(STATUS "OPENTHREAD_SRC_DIR=${OPENTHREAD_SRC_DIR}")
  set(OPENTHREAD_SRC_DIR "${CMAKE_BINARY_DIR}/modules/openthread/build/src"
    CACHE STRING "Directory containg the OpenThread source code.")
  message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")
  message(STATUS "OPENTHREAD_SRC_DIR=${OPENTHREAD_SRC_DIR}")

  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND           ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY ${OPENTHREAD_SRC_DIR}
      RESULT_VARIABLE   git_return
      OUTPUT_VARIABLE   openthread_git_hash
      )

    list(APPEND OPENTHREAD_SETTINGS "OpenThread_commit=${openthread_git_hash}\n\n")
  endif()

  # Store all OT related variables
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
    if("${_variableName}" MATCHES "^CONFIG_OPENTHREAD_.*|^OT_.*")
      list(APPEND OPENTHREAD_SETTINGS "${_variableName}=${${_variableName}}\n")
    endif()
  endforeach()

  set(OPENTHREAD_CONFIG_FILE
    "${CMAKE_BINARY_DIR}/openthread_lib_configuration.txt"
    CACHE STRING "File containg OpenThread build configuration parameters.")
  FILE(WRITE ${OPENTHREAD_CONFIG_FILE} ${OPENTHREAD_SETTINGS})

  set(OPENTHREAD_LIB_FTD "${OPENTHREAD_SRC_DIR}/core/libopenthread-ftd.a"
    CACHE STRING "FTD")
  message(STATUS "OPENTHREAD_LIB_FTD=${OPENTHREAD_LIB_FTD}")

  set(OPENTHREAD_LIB_MTD "${OPENTHREAD_SRC_DIR}/core/libopenthread-mtd.a"
    CACHE STRING "MTD")

  set(OPENTHREAD_LIB_RADIO "${OPENTHREAD_SRC_DIR}/core/libopenthread-radio.a"
    CACHE STRING "RADIO")

  set(OPENTHREAD_LIB_CLI_FTD "${OPENTHREAD_SRC_DIR}/cli/libopenthread-cli-ftd.a"
    CACHE STRING "CLI-FTD")

  set(OPENTHREAD_LIB_CLI_MTD "${OPENTHREAD_SRC_DIR}/cli/libopenthread-cli-mtd.a"
    CACHE STRING "CLI-MTD")

  set(OPENTHREAD_LIB_NCP_FTD "${OPENTHREAD_SRC_DIR}/ncp/libopenthread-ncp-ftd.a"
    CACHE STRING "NCP-FTD")

  set(OPENTHREAD_LIB_NCP_MTD "${OPENTHREAD_SRC_DIR}/ncp/libopenthread-ncp-mtd.a"
    CACHE STRING "NCP-MTD")

  set(OPENTHREAD_LIB_RCP "${OPENTHREAD_SRC_DIR}/ncp/libopenthread-rcp.a"
    CACHE STRING "RCP")

  nrfxlib_calculate_lib_path(lib_path)

  set(OPENTHREAD_DST_DIR
    "${NRFXLIB_DIR}/openthread/${lib_path}/v${CONFIG_OPENTHREAD_THREAD_VERSION}"
    CACHE STRING "The nrfxlib directory with OpenThread Libraries to be overwritten.")
endfunction(openthread_libs_configuration_write)

if(CONFIG_NET_L2_OPENTHREAD AND CONFIG_OPENTHREAD_SOURCES)
  message(DEBUG "Building OT from sources, config file will be generated.")
  openthread_libs_configuration_write()

  set(OPENTHREAD_HEADERS_DIR "${ZEPHYR_OPENTHREAD_MODULE_DIR}/../include")

  add_custom_target(install_openthread_libraries
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_FTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_MTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_RADIO}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_CLI_FTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_CLI_MTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_NCP_FTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_NCP_MTD}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "${OPENTHREAD_LIB_RCP}" "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${OPENTHREAD_HEADERS_DIR}
      ${NRFXLIB_DIR}/openthread/include
    COMMAND ${CMAKE_COMMAND} -E copy
     ${OPENTHREAD_CONFIG_FILE}
     ${NRFXLIB_DIR}/openthread/
    DEPENDS ${OPENTHREAD_LIB_FTD}
    )

    set_property(TARGET zephyr_property_target
    APPEND PROPERTY install_openthread_libraries_DEPENDENCIES
    install_openthread_libraries
    )
endif()