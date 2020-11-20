#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

cmake_minimum_required(VERSION 3.18)

# Set subdirectory in build dir
set(EI_BASE_DIR ${CMAKE_BINARY_DIR}/ei)
set(EI_UNZIPPED_DIR ${EI_BASE_DIR}/unpacked)
set(EI_BINARY_DIR ${EI_BASE_DIR}/bin)

if(NOT EXISTS ${EI_BASE_DIR})
  file(MAKE_DIRECTORY ${EI_BASE_DIR})
endif()

# Prepare Edge Impulse library zip archive
if(CONFIG_EDGE_IMPULSE_BUILD_FROM_ZIP)

  if(CONFIG_EDGE_IMPULSE_ZIP_PATH STREQUAL "")
    message(FATAL_ERROR "CONFIG_EDGE_IMPULSE_ZIP_PATH must be specified in Kconfig.")
  endif()

  set(EI_ZIP_PATH ${APPLICATION_SOURCE_DIR}/${CONFIG_EDGE_IMPULSE_ZIP_PATH})
  if (NOT EXISTS ${EI_ZIP_PATH})
    message(FATAL_ERROR "Specified Edge Impulse archive (${EI_ZIP_PATH}) does "
             "not exist. Download the archive manually and place it under "
             "defined path or use CONFIG_EDGE_IMPULSE_BUILD_FROM_URI Kconfig "
             "option to let the build system download the library "
             "automatically.")
  else()
    message(STATUS "Using Edge Impulse archive: ${EI_ZIP_PATH}.")
  endif()

elseif(CONFIG_EDGE_IMPULSE_BUILD_FROM_URI)

  if(CONFIG_EDGE_IMPULSE_DOWNLOAD_URI STREQUAL "")
    message(FATAL_ERROR "CONFIG_EDGE_IMPULSE_DOWNLOAD_URI must be specified in Kconfig.")
  endif()

  set(EI_ZIP_PATH ${EI_BASE_DIR}/src.zip)
  set(EI_URI_DUMP ${EI_BASE_DIR}/uri.txt)

  if(EXISTS ${EI_URI_DUMP})
    file(READ ${EI_URI_DUMP} EI_URI)
  else()
    set(EI_URI " ")
  endif()

  if((NOT EXISTS ${EI_ZIP_PATH}) OR
     (NOT CONFIG_EDGE_IMPULSE_DOWNLOAD_URI STREQUAL ${EI_URI}))
    # Start download, use API key that was provided.
    message(STATUS "Downloading Edge Impulse files, please wait...")

    if(DEFINED EI_API_KEY_HEADER)
      file(DOWNLOAD ${CONFIG_EDGE_IMPULSE_DOWNLOAD_URI} ${EI_ZIP_PATH}
           SHOW_PROGRESS
           STATUS download_status
           HTTPHEADER "Accept: application/zip"
           HTTPHEADER ${EI_API_KEY_HEADER})
    else()
      file(DOWNLOAD ${CONFIG_EDGE_IMPULSE_DOWNLOAD_URI} ${EI_ZIP_PATH}
           SHOW_PROGRESS
           STATUS download_status
           HTTPHEADER "Accept: application/zip")
    endif()

    list(GET download_status 0 status_code)
    if (status_code EQUAL 0)
      message(STATUS "Successfully downloaded Edge Impulse archive.")
      file(WRITE ${EI_URI_DUMP} ${CONFIG_EDGE_IMPULSE_DOWNLOAD_URI})
    else()
      file(REMOVE ${EI_ZIP_PATH})
      file(REMOVE ${EI_URI_DUMP})
      message(FATAL_ERROR "Failed to download Edge Impulse files. Please make "
               "sure that CONFIG_EDGE_IMPULSE_DOWNLOAD_URI is properly defined "
               "and EI_API_KEY_HEADER is specified (if needed).")
    endif()
  else()
    message(STATUS "Edge Impulse zip archive is already downloaded.")
  endif()

else()
  message(FATAL_ERROR "Unknown Edge Impulse build source.")
endif()

# Rerun CMake when zip file content changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${EI_ZIP_PATH})

# Unzip Edge impulse source files
file(REMOVE_RECURSE ${EI_UNZIPPED_DIR})
file(MAKE_DIRECTORY ${EI_UNZIPPED_DIR})
file(ARCHIVE_EXTRACT INPUT ${EI_ZIP_PATH} DESTINATION ${EI_UNZIPPED_DIR})

add_subdirectory(${EI_UNZIPPED_DIR} ${EI_BINARY_DIR})
