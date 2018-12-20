cmake_minimum_required(VERSION 3.8.2)

zephyr_add_multi_image()

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(bootloader)

target_sources(${IMAGE}app PRIVATE src/bootloader.c)

message("Loader done building")
