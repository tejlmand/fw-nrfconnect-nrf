#
# Copyright (c) 2018 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

zephyr_library()
zephyr_library_sources(bl_crypto.c)
zephyr_library_link_libraries(${IMAGE}nrfxlib_crypto)

if (CONFIG_SB_CRYPTO_OBERON_ECDSA_SECP256R1)
  zephyr_library_sources(bl_crypto_oberon_ecdsa.c)
elseif (CONFIG_SB_CRYPTO_CC310_ECDSA_SECP256R1)
  zephyr_library_sources(
    bl_crypto_cc310_ecdsa.c
    bl_crypto_cc310_common.c
    )
else()
  message(FATAL_ERROR "No signature implementation chosen for bootloader.")
endif()

if (CONFIG_SB_CRYPTO_OBERON_SHA256)
  zephyr_library_sources(bl_crypto_oberon_hash.c)
  target_sources(bl_crypto PRIVATE bl_crypto_oberon_sha256.c)
elseif (CONFIG_SB_CRYPTO_CC310_SHA256)
  zephyr_library_sources(
    bl_crypto_cc310_hash.c
    bl_crypto_cc310_common.c
    bl_crypto_cc310_sha256.c
    )
else()
  message(FATAL_ERROR "No hash implementation chosen for bootloader.")
endif()
set(NRFXLIB_BASE ${ZEPHYR_BASE}/../nrfxlib)
zephyr_include_directories(.)
#TODO: Move this out of this Cmake File? or find a better PATH variable
zephyr_include_directories(${NRFXLIB_BASE}/crypto/nrf_oberon/include)
zephyr_include_directories(${NRFXLIB_BASE}/crypto/nrf_cc310_bl/include)
