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
	print_header((struct bl_crypto_abi*)(*abi_getter_in.abis));

	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.
		crypto_root_of_trust(pk, pk_hash, sig, fw, fw_len);
}


int test_print(int i){
	printk("Test print: Ret val = %d\n\r", ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.test_print(i));
}

bool verify_sig(const u8_t *data, u32_t data_len, const u8_t *sig,
		const u8_t *pk)
{
	return ((struct bl_crypto_abi*)(*abi_getter_in.abis))->abi.
		verify_sig(data, data_len, sig, pk);
}

