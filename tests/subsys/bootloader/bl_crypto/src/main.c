/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <ztest.h>

#include "bl_crypto.h"
#include "test_vector.c"
#include "test_vectors.h"

/*
const uint8_t const_fw_hash[] = image_fw_hash;
const uint8_t const_fw_sig[] = image_fw_sig;
const uint8_t const_fw_pubk[] = image_public_key;
*/

void test_ecdsa_verify(void)
{	
	u32_t hash_len = ARRAY_SIZE(hash_sha256);	
	/* All is good */
	int retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_hash_sha256, hash_len, pub_concat, sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, const_pub_concat, sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, const_sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, const_pub_concat, const_sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_hash_sha256, hash_len, const_pub_concat, const_sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_hash_sha256, hash_len, const_pub_concat, const_sig_concat);
	zassert_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, image_gen_sig);
	zassert_equal(CRYS_OK, retval, "gen sig failed retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, image_fw_sig);
	zassert_equal(CRYS_OK, retval, "fw_sig retval: 0x%x", retval);
	image_fw_hash[0]++;	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, image_fw_sig);
	image_fw_hash[0]--;	
	zassert_not_equal(CRYS_OK, retval, "fw_sig retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_fw_hash, hash_len, image_public_key, image_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_fw retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, const_public_key, image_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_pubk retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, const_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_sig retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, const_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_sig retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, image_public_key, const_gen_sig);
	zassert_equal(CRYS_OK, retval, "const_gen_sig retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(image_fw_hash, hash_len, const_public_key, const_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_sig const_pk retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_fw_hash, hash_len, const_public_key, const_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_sig const_pk const_fw retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_fw_hash, hash_len, image_public_key, const_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_sig const_fw retval: 0x%x", retval);
	
	retval = bl_ecdsa_verify_secp256r1(const_fw_hash, hash_len, const_public_key, image_fw_sig);
	zassert_equal(CRYS_OK, retval, "const_pk const_fw retval: 0x%x", retval);
	
	
	/*
	uint8_t output[32] = {0};
	bl_sha256_ctx_t ctx;
	bl_sha256_init(&ctx);
	printk("len fw_data: %ld\n\r", ARRAY_SIZE(image_fw_data));
	bl_sha256_update(&ctx, const_fw_data, ARRAY_SIZE(image_fw_data));
	bl_sha256_finish(&ctx, output);
	
	retval = bl_ecdsa_verify_secp256r1(output, hash_len, image_public_key, image_fw_sig);
	zassert_equal(CRYS_OK, retval, "hashed fw_data retval: 0x%x", retval);
	
	printk("\n\ro vector: 0x");
	for(size_t i = 0; i < ARRAY_SIZE(output); i++) {
		printk("%x", output[i]);
	}
	printk("\n\r");*/
	
	/* pub key does not match */
	pub_concat[1]++;
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, sig_concat);
	pub_concat[1]--;
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	pub_concat[1]++;
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, const_sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	retval = bl_ecdsa_verify_secp256r1(const_hash_sha256, hash_len, pub_concat, const_sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	pub_concat[1]--;
	
	/* hash does not match */
	hash_sha256[2]++;
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, sig_concat);
	hash_sha256[2]--;
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	/* signature does not match */
	sig_concat[3]++;
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, sig_concat);
	sig_concat[3]--;
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	/* Null pointer passed as hash */
	retval = bl_ecdsa_verify_secp256r1(NULL, hash_len, pub_concat, sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);

	/* Wrong hash LEN */	
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, 24, pub_concat, sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);

	/* Null pointer passed as public key */
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, NULL, sig_concat);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
	
	/* Null pointer passed as signature */
	retval = bl_ecdsa_verify_secp256r1(hash_sha256, hash_len, pub_concat, NULL);
	zassert_not_equal(CRYS_OK, retval, "retval: 0x%x", retval);
}

void test_sha256_string(const uint8_t * input, uint32_t input_len, const uint8_t * test_vector, bool eq)
{
	int rc = CRYS_OK;
	uint32_t tmp_buff_size = 258;
	uint8_t output[32] = {0};
	bl_sha256_ctx_t ctx;
	rc = bl_sha256_init(&ctx);
	if(input_len > tmp_buff_size)
	{
		size_t block_size;
		for(size_t offset = 0; offset < input_len; offset += block_size) { 
			block_size = input_len - offset;
			if(block_size > tmp_buff_size)
			{
				block_size = tmp_buff_size;
				rc = bl_sha256_update(&ctx, input+offset, block_size);
			}
			else
			{
				rc = bl_sha256_update(&ctx, input+offset, block_size);
			}

		}
	}
	else
	{
		rc = bl_sha256_update(&ctx, input, input_len);
	}
	rc = bl_sha256_finish(&ctx, output);
	zassert_equal(CRYS_OK, rc, "hash updated failed retval was: %d");
	/*
	printk("Input length: %d\n\r", input_len);
	printk("0x%x input address", (u32_t) input);

	printk("\n\ri vector: 0x");
	for(size_t i = 0; i < input_len; i++) {
		printk("%x",input[i]);
	}
	printk("\n\rt vector: 0x");
	for(size_t i = 0; i < 32; i++) {
		printk("%x",test_vector[i]);
	}
	printk("\n\ro vector: 0x");
	for(size_t i = 0; i < ARRAY_SIZE(output); i++) {
		printk("%x", output[i]);
	}
	printk("\n\r");
	*/
	for(size_t i = 0; i < ARRAY_SIZE(output); i++) {
		if(eq){
			zassert_equal(test_vector[i], output[i], "positive test failed retval was: 0x%x at position:%d", output[i], i);
		}else {
			zassert_not_equal(test_vector[i], output[i], "negative test failed retval was: 0x%x at position:%d", output[i], i);
		}
	}
}
const uint8_t input3[] = "test vector should fail";
const uint8_t input2[] = "test vector";

void test_sha256(void)
{
	test_sha256_string(NULL, 0, sha256_empty_string, true);
	test_sha256_string(const_fw_data, ARRAY_SIZE(image_fw_data), image_fw_hash, true);
	test_sha256_string(input2, strlen(input2), sha256_test_vector_string, true);
	/* This may be a poor test */
	test_sha256_string(input3, strlen(input3), sha256_test_vector_string, false);
	test_sha256_string(mcuboot_key, ARRAY_SIZE(mcuboot_key), mcuboot_key_hash, true);
	test_sha256_string(mcuboot_key, ARRAY_SIZE(mcuboot_key), sha256_test_vector_string, false);
	test_sha256_string(long_input, ARRAY_SIZE(long_input), long_input_hash, true);
	test_sha256_string(long_input, ARRAY_SIZE(long_input), sha256_test_vector_string, false);
	//
	test_sha256_string(hash_in1, 1, hash_res1, true);
	test_sha256_string(hash_in2, 3, hash_res2, true);
	test_sha256_string(hash_in3, 56, hash_res3, true);

	test_sha256_string(hash_in, 55, hash_res55, true);
	test_sha256_string(hash_in, 56, hash_res56, true);
	test_sha256_string(hash_in, 57, hash_res57, true);
	test_sha256_string(hash_in, 63, hash_res63, true);
	test_sha256_string(hash_in, 64, hash_res64, true);
	test_sha256_string(hash_in, 65, hash_res65, true);
}

void test_verify_signature(void)
{
	u8_t data_len = ARRAY_SIZE(firmware);
	bool retval = verify_sig(firmware, data_len, sig, pk);
	zassert_equal(true, retval, "retval was: %d", retval);
	
	retval = verify_sig(const_firmware, data_len, const_sig, const_pk);
	zassert_equal(true, retval, "retval was: %d", retval);

	/* Hash of FW does not match sig + pk */
	firmware[0]++;	
	retval = verify_sig(firmware, data_len, sig, pk);
	firmware[0]--;	
	zassert_not_equal(true, retval, "retval was: %d", retval);
	
	/* Data len is too short */
	data_len--;	
	retval = verify_sig(firmware, data_len, sig, pk);
	data_len++;	
	zassert_not_equal(true, retval, "retval was: %d", retval);
	
	/* PK doesn't match signature + hash*/	
	pk[2]++;
	retval = verify_sig(firmware, data_len, sig, pk);
	pk[2]--;
	zassert_not_equal(true, retval, "retval was: %d", retval);

	/* Sig doesn't match PK + hash */
	sig[4]++;
	retval = verify_sig(firmware, data_len, sig, pk);
	sig[4]--;
	zassert_not_equal(true, retval, "retval was: %d", retval);
}


void test_crypto_root_of_trust(void)
{

	/* Success. */
	int retval = crypto_root_of_trust(pk, pk_hash, sig, firmware,
			sizeof(firmware));

	zassert_equal(0, retval, "retval was %d", retval);

	retval = crypto_root_of_trust(const_pk, const_pk_hash, const_sig, const_firmware,
			sizeof(const_firmware));
	zassert_equal(0, retval, "retval was %d", retval);

	zassert_equal(0, retval, "retval was %d", retval);
	/* pk doesn't match pk_hash. */
	pk[1]++;
	retval = crypto_root_of_trust(pk, pk_hash, sig, firmware,
			sizeof(firmware));
	pk[1]--;

	zassert_equal(-EPKHASHINV, retval, "retval was %d", retval);

	/* metadata doesn't match signature */
	firmware[0]++;
	retval = crypto_root_of_trust(pk, pk_hash, sig, firmware,
			sizeof(firmware));
	firmware[0]--;

	zassert_equal(-ESIGINV, retval, "retval was %d", retval);
}

void test_main(void)
{
	ztest_test_suite(test_bl_crypto,
			 ztest_unit_test(test_crypto_root_of_trust),
			 ztest_unit_test(test_sha256),
			 ztest_unit_test(test_ecdsa_verify),
			 ztest_unit_test(test_verify_signature)
	);
	ztest_run_test_suite(test_bl_crypto);
}
