/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <bl_crypto.h>
#include "bl_crypto_internal.h"
#include <fw_metadata.h>

extern struct fw_abi_getter_info abi_getter_in;
void print_header(struct bl_crypto_abi * obj){
	printk("====HEADER====\n\r");
	for (u32_t i = 0; i < 3; i++) {
		printk("Magic values: 0x%x\n\r", obj->header.magic[i]);
	}	
	printk("Flags: %d and version: %d\n\r", obj->header.abi_flags, 	obj->header.abi_version);
	printk("ABI_id: 0x%x\n\r", obj->header.abi_id);
	printk("ABI len: %d\n\r", obj->header.abi_len);
} 


int crypto_root_of_trust(const u8_t *pk, const u8_t *pk_hash,
			 const u8_t *sig, const u8_t *fw,
			 const u32_t fw_len)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.
		crypto_root_of_trust(pk, pk_hash, sig, fw, fw_len);
}

bool verify_sig(const u8_t *data, u32_t data_len, const u8_t *sig,
		const u8_t *pk)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.
		verify_sig(data, data_len, sig, pk);
}

int bl_sha256_init(bl_sha256_ctx_t * ctx)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.bl_sha256_init(ctx);
}

int bl_sha256_update(bl_sha256_ctx_t * ctx, const u8_t * data, u32_t data_len)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.bl_sha256_update(ctx, data, data_len);	
}

int bl_sha256_finish(bl_sha256_ctx_t * ctx, u8_t * output)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.bl_sha256_finish(ctx, output);	
}

int bl_ecdsa_verify_secp256r1(const u8_t * hash, u32_t hash_len, const u8_t * public_key, const u8_t * signature)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.bl_ecdsa_verify_secp256r1(hash, hash_len, public_key, signature);	
}
