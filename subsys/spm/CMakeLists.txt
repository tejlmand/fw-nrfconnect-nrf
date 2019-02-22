cmake_minimum_required(VERSION 3.8.2)

zephyr_add_executable(secure_partition)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(secure_partition)

target_sources(${IMAGE}app PRIVATE src/secure_partition_manager.c)

set_property(
  GLOBAL APPEND PROPERTY
  SECURE_IMAGE_HEX_FILES_TO_MERGE
  ${ZEPHYR_BINARY_DIR}/${KERNEL_HEX_NAME}
)

message("SPU configuration done building")
