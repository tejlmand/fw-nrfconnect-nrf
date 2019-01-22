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

/* Returns 0 for succes or -1 for failure */
int bl_ecdsa_verify_secp256r1(const u8_t * hash,
							  u32_t hash_len,
							  const u8_t * public_key,
							  const u8_t * signature)
{
	int retval;
	/* maybe we need internal data
	 * u8_t hash[CONFIG_SB_HASH_LEN];
	 */
	if(hash_len != 32)
	{
		return 0xf00dbabe;
	}

	retval = occ_ecdsa_p256_verify_hash(signature, hash, public_key);
	
	if(retval == 0)
	{
		return retval;
	}

	return retval;
}
