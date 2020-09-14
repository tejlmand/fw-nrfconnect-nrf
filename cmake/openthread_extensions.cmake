#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

set(NRFXLIB_DIR ${ZEPHYR_NRFXLIB_MODULE_DIR})
assert_exists(NRFXLIB_DIR)
include(${NRFXLIB_DIR}/common.cmake)

set(OT_WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Store the configuration of the compiled OpenThread libraries
# and set source and destination paths.
function(openthread_libs_configuration_write)
  set(OPENTHREAD_SRC_DIR "${CMAKE_BINARY_DIR}/modules/openthread/build/src"
    CACHE STRING "Directory containg the OpenThread source code.")

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
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-ftd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-mtd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-radio> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-cli-ftd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-cli-mtd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-ncp-ftd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-ncp-mtd> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:openthread-rcp> "${OPENTHREAD_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${OPENTHREAD_HEADERS_DIR}
      ${NRFXLIB_DIR}/openthread/include
    COMMAND ${CMAKE_COMMAND} -E copy
     ${OPENTHREAD_CONFIG_FILE}
     ${NRFXLIB_DIR}/openthread/
    DEPENDS ${OPENTHREAD_LIB_FTD}
    )

endif()
