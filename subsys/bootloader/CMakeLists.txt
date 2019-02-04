add_subdirectory(c_runtime_setup)
add_subdirectory(provision)

set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${PROJECT_BINARY_DIR}/${KERNEL_HEX_NAME}

)
set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE_TARGET
  ${logical_target_for_zephyr_elf}
)

add_subdirectory_ifdef(CONFIG_SECURE_BOOT_CRYPTO bl_crypto)
add_subdirectory_ifdef(CONFIG_SECURE_BOOT_DEBUG debug)
add_subdirectory_ifdef(CONFIG_SB_FLASH_PROTECT fprotect)


include(${CMAKE_CURRENT_LIST_DIR}/cmake/fw_info_magic.cmake)
zephyr_include_directories(include)
