cmake_minimum_required(VERSION 3.8.2)

zephyr_add_executable(spu_setup)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(spu_setup)

target_sources(${IMAGE}app PRIVATE src/spu_setup.c)

set_property(
  GLOBAL APPEND PROPERTY
  SECURE_IMAGE_HEX_FILES_TO_MERGE
  ${ZEPHYR_BINARY_DIR}/${KERNEL_HEX_NAME}
)

message("SPU configuration done building")
