#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

# Do not add source using zephyr cmake extensions.
# This is because our library should not be added to the 'ZEPHYR_LIBS' list,
# since it should not be linked to the zephyr elf itself.

# Bump IMAGE number, allowing us to create a bootloader using app lib.
zephyr_add_multi_image()

# Standard Zephyr application boilerplate:
# http://docs.zephyrproject.org/application/application.html
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

zephyr_sources(bootloader.c)
zephyr_link_libraries($bl_crypto)
add_subdirectory(c_runtime_setup)
zephyr_link_libraries(c_runtime_setup)

add_subdirectory(provision)
zephyr_link_libraries(provision)

if(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR OR CONFIG_SECURE_BOOT_DEBUG)
  add_subdirectory(libc)
  zephyr_link_libraries(libc)
endif()

add_subdirectory(debug)
zephyr_link_libraries(debuglib)
add_subdirectory_ifdef(CONFIG_SB_FLASH_PROTECT fprotect)
zephyr_link_libraries_ifdef(CONFIG_SB_FLASH_PROTECT fprotect)
add_subdirectory(fw_metadata)
zephyr_link_libraries(fw_metadata)
zephyr_include_directories(include)
zephyr_include_directories(CONFIG_SECURE_BOOT_DEBUG include/debug)

set(NRF_BOOTLOADER_SCRIPTS ${NRF_DIR}/scripts/bootloader)

include(${NRF_DIR}/subsys/bootloader/cmake/fw_metadata.cmake)

set(BOOTLOADER_HEX_NAME bootloader.hex)
set(BOOTLOADER_HEX ${PROJECT_BINARY_DIR}/${BOOTLOADER_HEX_NAME})
set(KERNEL_HEX ${PROJECT_BINARY_DIR}/${KERNEL_HEX_NAME})
set(SIGNED_KERNEL_HEX_NAME signed_${KERNEL_HEX_NAME})
set(SIGNED_KERNEL_HEX ${PROJECT_BINARY_DIR}/${SIGNED_KERNEL_HEX_NAME})
set(PROVISION_HEX_NAME provision_data.hex)
set(PROVISION_HEX ${PROJECT_BINARY_DIR}/${PROVISION_HEX_NAME})

message(INFO ${BOOTLOADER_HEX})

# Append all hex files to merge to list.
list(APPEND to_merge ${BOOTLOADER_HEX})
list(APPEND to_merge ${SIGNED_KERNEL_HEX})
list(APPEND to_merge ${PROVISION_HEX})
set_property(GLOBAL APPEND PROPERTY HEX_FILES_TO_MERGE ${to_merge})

include (${CMAKE_CURRENT_LIST_DIR}/cmake/debug_keys.cmake)

set(sign_depends kernel_elf ${key_file_depends})

add_custom_command(
  OUTPUT
  ${SIGNED_KERNEL_HEX}
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${NRF_BOOTLOADER_SCRIPTS}/sign.py
  --input ${PROJECT_BINARY_DIR}/${KERNEL_HEX_NAME}
  --output ${SIGNED_KERNEL_HEX}
  --offset ${CONFIG_SB_VALIDATION_METADATA_OFFSET}
  --pem ${signature_private_key_file}
  --magic-value "${VALIDATION_INFO_MAGIC}"
  --pk-hash-len ${CONFIG_SB_PUBLIC_KEY_HASH_LEN}
  DEPENDS ${sign_depends}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT
  "Creating validation for ${KERNEL_HEX_NAME}, storing to ${SIGNED_KERNEL_HEX_NAME}"
  USES_TERMINAL
  )
add_custom_target(
  sign_target
  DEPENDS
  ${PROJECT_BINARY_DIR}/${SIGNED_KERNEL_HEX_NAME}
  offsets_h
  )

add_custom_command(
  OUTPUT
  ${PROVISION_HEX}
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${NRF_BOOTLOADER_SCRIPTS}/provision.py
  --generated-conf-file ${PROJECT_BINARY_DIR}/include/generated/generated_dts_board.conf
  --signature-private-key-file "${signature_private_key_file}"
  --public-key-files "${public_key_files}"
  --output ${PROVISION_HEX}
  DEPENDS ${provision_depends}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT
  "Creating provision data for Bootloader, storing to ${PROVISION_HEX_NAME}"
  USES_TERMINAL
  )
add_custom_target(
  provision_target
  DEPENDS
  ${PROJECT_BINARY_DIR}/${PROVISION_HEX_NAME}
  offsets_h
  )

