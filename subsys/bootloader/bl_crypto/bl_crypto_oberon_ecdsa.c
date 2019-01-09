/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <stddef.h>
#include <zephyr/types.h>
#include <stdbool.h>
#include <occ_ecdsa_p256.h>
#include "bl_crypto_internal.h"

bool _verify_sig(const u8_t *data, u32_t data_len, const u8_t *sig,
		const u8_t *pk, bool external)
{
	u8_t hash1[CONFIG_SB_HASH_LEN];
	u8_t hash2[CONFIG_SB_HASH_LEN];

	if (!get_hash(hash1, data, data_len, external)) {
		return false;
	}

	if (!get_hash(hash2, hash1, CONFIG_SB_HASH_LEN, external)) {
		return false;
	}

	int retval = occ_ecdsa_p256_verify_hash(sig, hash2, pk);

	return (retval == 0);
}

bool verify_sig_external(const u8_t *data,
			  	u32_t data_len,
				const u8_t sig,
				const u8_t *public_keym
				u32_t hash_len)
{
	/*TODO do we want VLA or should we just do a max hash len for the function ? */
	u8_t hash[CONFIG_SB_HASH_LEN]; 

	if (!get_hash(hash, data, data_len, true)) {
		return false;
	}

	int retval = occ_ecdsa_p256_verify_hash(sig, hash, pk);
	/* TODO: Truncated return value, standarize on return codes(MBEDTLS?) */
	return (retval == 0);
}
