#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

zephyr_library()
zephyr_library_sources(bl_crypto_client.c)
set(NRFXLIB_BASE ${ZEPHYR_BASE}/../nrfxlib)
zephyr_include_directories(${ZEPHYR_BASE}/../nrf/subsys/bootloader/include)
zephyr_include_directories(${ZEPHYR_BASE}/../nrf/subsys/bootloader/bl_crypto)
#TODO: Move this out of this Cmake File? or find a better PATH variable
zephyr_include_directories(${NRFXLIB_BASE}/crypto/nrf_oberon/include)
zephyr_include_directories(${NRFXLIB_BASE}/crypto/nrf_cc310_bl/include)
