/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#ifndef BOOTLOADER_CRYPTO_H__
#define BOOTLOADER_CRYPTO_H__

#include <zephyr/types.h>
#include <fw_metadata.h>

/* Placeholder defines. Values should be updated, if no existing errors can be
 * used instead. */
#define EPKHASHINV 101
#define ESIGINV    102



/**
 * @brief Initialize bootloader crypto module.
 *
 * @retval 0        On success.
 * @retval -EFAULT  If crypto module reported an error.
 */
int crypto_init(void);


/**
 * @brief Verify a signature using configured signature and SHA-256
 *
 * Verifies the public key against the public key hash, then verifies the hash
 * of the signed data against the signature using the public key.
 *
 * @param[in]  pk            Public key.
 * @param[in]  pk_hash       Expected hash of the public key. This is the root
 *                           of trust.
 * @param[in]  sig           Signature
 * @param[in]  fw            Firmware
 * @param[in]  fw_len        Length of firmware.
 *
 * @retval 0            On success.
 * @retval -EPKHASHINV  If pk_hash didn't match pk.
 * @retval -ESIGINV     If signature validation failed.
 *
 * @remark No parameter can be NULL.
 */
#if _LIBOBERON
#include <occ_sha256.h>
typedef occ_sha256_ctx bl_sha256_ctx_t;
#else
#include <nrf_cc310_bl_hash_sha256.h>
typedef nrf_cc310_bl_hash_context_sha256_t bl_sha256_ctx_t;
#endif

TYPE_AND_DECL(int, crypto_root_of_trust, const u8_t *pk,
					 const u8_t *pk_hash,
					 const u8_t *sig,
					 const u8_t *fw,
					 const u32_t fw_len);

TYPE_AND_DECL(bool, verify_sig,
		const u8_t * data,
		u32_t data_len, 
		const u8_t * sig,
		const u8_t * pk); 
TYPE_AND_DECL(int, bl_sha256_init,
		bl_sha256_ctx_t * ctx);
TYPE_AND_DECL(int, bl_sha256_update,
		bl_sha256_ctx_t * ctx, 
		const u8_t * data, 
		u32_t data_len);
TYPE_AND_DECL(int, bl_sha256_finish, 
		bl_sha256_ctx_t * ctx, 
		u8_t * output);
TYPE_AND_DECL(int, bl_ecdsa_verify_secp256r1,
		const u8_t * hash, 
		u32_t hash_len,
		const u8_t * sig, 
		const u8_t * public_key
		);

struct bl_crypto_abi {
	struct fw_abi_info header;
	struct {
		crypto_root_of_trust_t crypto_root_of_trust;
		verify_sig_t verify_sig; 
		bl_sha256_init_t bl_sha256_init;
		bl_sha256_update_t bl_sha256_update;
		bl_sha256_finish_t bl_sha256_finish;
		bl_ecdsa_verify_secp256r1_t bl_ecdsa_verify_secp256r1;
	} abi;
};

#endif

