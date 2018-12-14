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
print(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_CUSTOM)

if(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_CUSTOM)
  zephyr_library_sources(startup.c)
elseif(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR)
  # TODO Use zephyr build code for this directly?
  zephyr_library_sources(
    ${CORTEX_M}/prep_c.c
    ${CORTEX_M}/reset.S
    ${ARM}/irq_init.c
    ${CORTEX_M}/vector_table.S
    zephyr_kernel_init_cpy.c
    zephyr_dummy_interrupt_handlers.c
    )
elseif(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK)
  # Set start symbol of MDK to point to the main_bl function
  add_definitions(-D__START=main_bl)
  add_definitions(-D__STARTUP_CLEAR_BSS)
else()
  assert(0 "Unreachable code")
endif()

# Nordic specific for startup code
if(CONFIG_SOC_SERIES_NRF91X)
  zephyr_library_sources_ifdef(CONFIG_SB_VENDOR_SYSTEM_INIT
    ${HAL}/nordic/nrfx/mdk/system_nrf9160.c
    )
  zephyr_library_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
    gcc_startup_nrf9160.S
    )
elseif(CONFIG_SOC_SERIES_NRF52X)
  zephyr_library_sources_ifdef(CONFIG_SB_VENDOR_SYSTEM_INIT
    ${HAL}/nordic/nrfx/mdk/system_nrf52840.c
    )
  zephyr_library_sources_ifdef(CONFIG_SB_C_RUNTIME_SETUP_VARIANT_MDK
    gcc_startup_nrf52840.S
    )
endif()
