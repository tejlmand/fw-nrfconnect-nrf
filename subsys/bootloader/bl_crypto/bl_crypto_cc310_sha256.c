#include <zephyr/types.h>
#include <string.h>
#include <misc/util.h>
#include <nrf_cc310_bl_hash_sha256.h>
#include <generated_dts_board.h>
#include "bl_crypto_cc310_common.h" 

#define CHUNK_LEN_STACK 0x400
#define STACK_BUFFER_LEN_WORDS ((CHUNK_LEN_STACK) / 4)

int bl_sha256_init(nrf_cc310_bl_hash_context_sha256_t * ctx)
{
	cc310_bl_backend_enable();
	int retval = nrf_cc310_bl_hash_sha256_init(ctx);
	return retval;
}

int bl_sha256_update(nrf_cc310_bl_hash_context_sha256_t * ctx, const u8_t * data, u32_t data_len)
{
	int retval;
	int err = 0x57AC0BF0;

	if(data_len > 0x400)
	{
		return err;
	}

	if((u32_t) data < CONFIG_SRAM_BASE_ADDRESS)
	{
		u8_t stack_buffer[data_len];
		u32_t block_len = data_len;
		memcpy(stack_buffer, data, block_len);
		retval = nrf_cc310_bl_hash_sha256_update(ctx, stack_buffer, block_len);
	}
	else
	{
		retval = nrf_cc310_bl_hash_sha256_update(ctx, data, data_len);
	}
	return retval;
}

int bl_sha256_finish(nrf_cc310_bl_hash_context_sha256_t * ctx, u8_t * output)
{
	int retval = nrf_cc310_bl_hash_sha256_finalize(ctx,(nrf_cc310_bl_hash_digest_sha256_t *) output);
	cc310_bl_backend_disable();
	return retval;
}
