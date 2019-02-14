zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../include)
add_subdirectory(fw_metadata)
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
  sign_target
  provision_target
)

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/debug_keys.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/fw_info_magic.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/sign.cmake)

add_custom_target(
  sign_target
  DEPENDS
  # TODO why does this dependency need to be here? Isnt't it sufficient
  # to depend on SIGNED_KERNEL_HEX, which is a result of
  ${SIGNATURE_PUBLIC_KEY_FILE_TARGET}
  ${SIGNED_KERNEL_HEX}
  )

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
  DEPENDS ${PROVISION_DEPENDS} sign_target
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

