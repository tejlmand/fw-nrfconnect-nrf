#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

set(ARM ${ZEPHYR_BASE}/arch/arm/core)
set(CORTEX_M ${ARM}/cortex_m)
set(EXTS ${ZEPHYR_BASE}/ext)
set(HAL ${EXTS}/hal)

zephyr_library()

if(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_CUSTOM)
  zephyr_library_sources(startup.c)
elseif(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR)
  # TODO Use zephyr build code for this directly?
  zephyr_library_sources(
    ${CORTEX_M}/prep_c.c
    ${CORTEX_M}/reset.S
    ${ARM}/irq_init.c
    ${CORTEX_M}/vector_table.S
    zephyr_dummy_interrupt_handlers.c
    )
elseif(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK)
  # Set start symbol of MDK to point to the main_bl function
  add_definitions(-D__START=main_bl)
  add_definitions(-D__STARTUP_CLEAR_BSS)
else()
  assert(0 "Unreachable code")
endif()

if(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR)
  add_library(kernel_init STATIC ${ZEPHYR_BASE}/kernel/init.c)
  target_link_libraries(kernel_init zephyr_interface)
  target_compile_definitions(kernel_init PRIVATE _Cstart=UNUSED_Cstart)
  add_dependencies(kernel_init offsets_h)
  target_link_libraries(c_runtime_setup PRIVATE kernel_init)
endif()

# Nordic specific for startup code
if(CONFIG_SOC_SERIES_NRF91X)
  zephyr_library_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
    gcc_startup_nrf9160.S
    )
elseif(CONFIG_SOC_SERIES_NRF52X)
  zephyr_library_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
    gcc_startup_nrf52840.S
    )
endif()
