zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
add_subdirectory(fw_metadata)
set(NRF_BOOTLOADER_SCRIPTS ${NRF_DIR}/scripts/bootloader)

set(PROVISION_HEX_NAME     provision_data.hex)
set(SIGNED_KERNEL_HEX_NAME signed_${KERNEL_HEX_NAME})
set(PROVISION_HEX          ${PROJECT_BINARY_DIR}/${PROVISION_HEX_NAME})
set(SIGNED_KERNEL_HEX      ${PROJECT_BINARY_DIR}/${SIGNED_KERNEL_HEX_NAME})

set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${SIGNED_KERNEL_HEX_NAME}
  ${PROVISION_HEX}
)
set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE_TARGET
  sign_target
  provision_target
)

# TODO add support for multiple linker passes
set(kernel_elf ${IMAGE}kernel_elf)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/debug_keys.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fw_info_magic.cmake)

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
  DEPENDS ${kernel_elf} ${key_file_depends}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT
  "Creating validation for ${KERNEL_HEX_NAME}, storing to ${SIGNED_KERNEL_HEX_NAME}"
  USES_TERMINAL
  )
add_custom_target(
  sign_target
  DEPENDS
  ${PROJECT_BINARY_DIR}/${SIGNED_KERNEL_HEX_NAME}
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
  )

