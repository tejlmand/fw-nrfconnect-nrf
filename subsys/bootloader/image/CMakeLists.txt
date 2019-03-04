set(KERNEL_ELF ${IMAGE}kernel_elf)
zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
add_subdirectory(fw_metadata)
#TODO: Check if CONFIG_SHARED_CRYPTO is set
add_subdirectory(bl_crypto_client)
set(NRF_BOOTLOADER_SCRIPTS ${NRF_DIR}/scripts/bootloader)
set(PROVISION_HEX_NAME     provision_data.hex)
set(PROVISION_HEX          ${PROJECT_BINARY_DIR}/${PROVISION_HEX_NAME})
set(SIGNED_KERNEL_HEX      ${CMAKE_BINARY_DIR}/signed_by_b0_${KERNEL_HEX_NAME})

set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${SIGNED_KERNEL_HEX}
  ${PROVISION_HEX}
)
set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE_TARGET
  signed_kernel_hex_target
  provision_target
)

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/debug_keys.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fw_info_magic.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/sign.cmake)

file(GLOB_RECURSE autoconf_files "${PROJECT_BINARY_DIR}/**/autoconf.h")
add_custom_command(
  OUTPUT
  ${PROVISION_HEX}
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${NRF_BOOTLOADER_SCRIPTS}/provision.py
  --generated-conf-files ${autoconf_files}
  --public-key-files "${SIGNATURE_PUBLIC_KEY_FILE},${PUBLIC_KEY_FILES}"
  --output ${PROVISION_HEX}
  DEPENDS ${PROVISION_KEY_DEPENDS} signature_public_key_file_target
  COMMENT
  "Creating provision data for Bootloader, storing to ${PROVISION_HEX_NAME}"
  USES_TERMINAL
  )
add_custom_target(
  provision_target
  DEPENDS
  ${PROVISION_HEX}
  )

