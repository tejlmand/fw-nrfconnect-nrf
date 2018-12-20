add_subdirectory_ifdef(CONFIG_SECURE_BOOT_CRYPTO bl_crypto)
add_subdirectory_ifdef(CONFIG_SECURE_BOOT_DEBUG debug)
add_subdirectory_ifdef(CONFIG_SB_FLASH_PROTECT fprotect)

add_subdirectory(fw_metadata)

if(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR OR CONFIG_SECURE_BOOT_DEBUG)
  add_subdirectory(libc)
endif()

add_subdirectory(c_runtime_setup)
add_subdirectory(provision)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/fw_info_magic.cmake)
zephyr_include_directories(include)

set_property(
  GLOBAL APPEND PROPERTY
  HEX_FILES_TO_MERGE
  ${PROJECT_BINARY_DIR}/zephyr.hex
  )
