#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

function(get_board_without_ns_suffix board_in board_out)
  string(REGEX REPLACE "(_?ns)$" "" board_in_without_suffix ${board_in})
  if(NOT ${board_in} STREQUAL ${board_in_without_suffix})
    if (NOT CONFIG_ARM_NONSECURE_FIRMWARE)
      message(FATAL_ERROR "${board_in} is not a valid name for a board without "
      "'CONFIG_ARM_NONSECURE_FIRMWARE' set. This because the 'ns'/'_ns' ending "
      "indicates that the board is the non-secure variant in a TrustZone "
      "enabled system.")
    endif()
    set(${board_out} ${board_in_without_suffix} PARENT_SCOPE)
    message("Changed board to secure ${board_in_without_suffix} (NOT NS)")
  else()
    set(${board_out} ${board_in} PARENT_SCOPE)
  endif()
endfunction()

# Add an overlay file to a child image.
# This can be used by a parent image to set overlay of Kconfig configuration or devicetree
# in its child images. This function must be called before 'add_child_image(image)'
# to have effect.
#
# Parameters:
#   'image' - child image name
#   'overlay_file' - overlay to be added to child image
#   'overlay_type' - 'OVERLAY_CONFIG' or 'DTC_OVERLAY_FILE'
function(add_overlay image overlay_file overlay_type)
  set(old_overlays ${${image}_${overlay_type}})
  string(FIND "${old_overlays}" "${overlay_file}" found)
  if (${found} EQUAL -1)
    set(${image}_${overlay_type} "${old_overlays} ${overlay_file}" CACHE INTERNAL "")
  endif()
endfunction()

# Convenience macro to add configuration overlays to child image.
macro(add_overlay_config image overlay_file)
  add_overlay(${image} ${overlay_file} OVERLAY_CONFIG)
endmacro()

# Convenience macro to add device tree overlays to child image.
macro(add_overlay_dts image overlay_file)
  add_overlay(${image} ${overlay_file} DTC_OVERLAY_FILE)
endmacro()

# Add a partition manager configuration file to the build.
# Note that is only one image is included in the build,
# you must set CONFIG_PM_SINGLE_IMAGE=y for the partition manager
# configuration to take effect.
function(ncs_add_partition_manager_config config_file)
  get_filename_component(pm_path ${config_file} REALPATH)
  get_filename_component(pm_filename ${config_file} NAME)

  if (NOT EXISTS ${pm_path})
    message(FATAL_ERROR
      "Could not find specified partition manager configuration file "
      "${config_file} at ${pm_path}"
      )
  endif()

  set_property(GLOBAL APPEND PROPERTY
    PM_SUBSYS_PATHS
    ${pm_path}
    )
  set_property(GLOBAL APPEND PROPERTY
    PM_SUBSYS_OUTPUT_PATHS
    ${CMAKE_CURRENT_BINARY_DIR}/${pm_filename}
    )
endfunction()

# Usage:
#   ncs_file(<mode> <arg> ...)
#
# NCS file function extension.
# This function extends the zephyr_file(CONF_FILES <arg>) function to support
# switching BOARD for child images.
#
# It also supports lookup of static partition manager files for boards based on
# the board name, revision, and the current build type.
#
# This function currently support the following <modes>.
#
# BOARD <board>: Board name to use when searching for board specific Kconfig
#                fragments.
#
# CONF_FILES <path>: Find all configuration files in path and return them in a
#                    list. Configuration files will be:
#                    - DTS:       Overlay files (.overlay)
#                    - Kconfig:   Config fragments (.conf)
#                    The conf file search will return existing configuration
#                    files for BOARD or the current board if BOARD argument is
#                    not given.
#                    CONF_FILES takes the following additional arguments:
#                    BOARD <board>:             Find configuration files for specified board.
#                    BOARD_REVISION <revision>: Find configuration files for specified board
#                                               revision. Requires BOARD to be specified.
#
#                                               If no board is given the current BOARD and
#                                               BOARD_REVISION will be used.
#
#                    DTS <list>:   List to populate with DTS overlay files
#                    KCONF <list>: List to populate with Kconfig fragment files
#                    PM <list>:    List to populate with board / build / domain specific
#                                  static partition manager files
#                    BUILD <type>: Build type to include for search.
#                                  For example:
#                                  BUILD debug, will look for <board>_debug.conf
#                                  and <board>_debug.overlay, instead of <board>.conf
#                    DOMAIN <domain>: Domain to use. This argument is only effective
#                                     for partition manager configuration files.
#
function(ncs_file)
  set(file_options CONF_FILES)
  if((ARGC EQUAL 0) OR (NOT (ARGV0 IN_LIST file_options)))
    message(FATAL_ERROR "No <mode> given to `ncs_file(<mode> <args>...)` function,\n \
Please provide one of following: CONF_FILES")
  endif()

  set(single_args CONF_FILES PM DOMAIN)
  set(zephyr_conf_single_args BOARD BOARD_REVISION BUILD DTS KCONF)

  cmake_parse_arguments(PREPROCESS_ARGS "" "${single_args};${zephyr_conf_single_args}" "" ${ARGN})
  # Remove any argument that is missing value to ensure proper behavior in situations like:
  # ncs_file(CONF_FILES <path> PM <list> DOMAIN BUILD <type>)
  # where value of DOMAIN could wrongly become BUILD which is another keyword.
  if(DEFINED PREPROCESS_ARGS_KEYWORDS_MISSING_VALUES)
    list(REMOVE_ITEM ARGN ${PREPROCESS_ARGS_KEYWORDS_MISSING_VALUES})
  endif()

  cmake_parse_arguments(NCS_FILE "" "${single_args}" "" ${ARGN})
  cmake_parse_arguments(ZEPHYR_FILE "" "${zephyr_conf_single_args}" "" ${ARGN})

  if(ZEPHYR_FILE_KCONF)
    if(ZEPHYR_FILE_BUILD AND EXISTS ${NCS_FILE_CONF_FILES}/prj_${ZEPHYR_FILE_BUILD}.conf)
      set(${ZEPHYR_FILE_KCONF} ${NCS_FILE_CONF_FILES}/prj_${ZEPHYR_FILE_BUILD}.conf)
    elseif(NOT ZEPHYR_FILE_BUILD AND EXISTS ${NCS_FILE_CONF_FILES}/prj.conf)
      set(${ZEPHYR_FILE_KCONF} ${NCS_FILE_CONF_FILES}/prj.conf)
    endif()
  endif()

  zephyr_file(CONF_FILES ${NCS_FILE_CONF_FILES}/boards ${NCS_FILE_UNPARSED_ARGUMENTS})

  if(ZEPHYR_FILE_KCONF)
    set(${ZEPHYR_FILE_KCONF} ${${ZEPHYR_FILE_KCONF}} PARENT_SCOPE)
  endif()

  if(ZEPHYR_FILE_DTS)
    set(${ZEPHYR_FILE_DTS} ${${ZEPHYR_FILE_DTS}} PARENT_SCOPE)
  endif()

  if(NOT DEFINED ZEPHYR_FILE_BOARD)
    # Defaulting to system wide settings when BOARD is not given as argument
    set(ZEPHYR_FILE_BOARD ${BOARD})
    if(DEFINED BOARD_REVISION)
      set(ZEPHYR_FILE_BOARD_REVISION ${BOARD_REVISION})
    endif()
  endif()

  if(NCS_FILE_PM)
    zephyr_file(FILENAMES filename_list
                BOARD ${ZEPHYR_FILE_BOARD}
                BOARD_REVISION ${ZEPHYR_FILE_BOARD_REVISION}
                BUILD ${ZEPHYR_FILE_BUILD}
    )

    foreach(filename ${filename_list})
      if(DEFINED NCS_FILE_DOMAIN)
        if(EXISTS ${NCS_FILE_CONF_FILES}/pm_static_${filename}_${NCS_FILE_DOMAIN}.yml)
          set(${NCS_FILE_PM} ${NCS_FILE_CONF_FILES}/pm_static_${filename}_${NCS_FILE_DOMAIN}.yml PARENT_SCOPE)
          break()
        endif()
      endif()

      if(EXISTS ${NCS_FILE_CONF_FILES}/pm_static_${filename}.yml)
        set(${NCS_FILE_PM} ${NCS_FILE_CONF_FILES}/pm_static_${filename}.yml PARENT_SCOPE)
        break()
      endif()
    endforeach()
  endif()

endfunction()
