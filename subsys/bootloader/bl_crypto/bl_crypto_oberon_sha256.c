#include <zephyr/types.h>
#include <occ_sha256.h>
#include <occ_constant_time.h>

int bl_sha256_init(occ_sha256_ctx * ctx)
{
	occ_sha256_init(ctx);
	return 0;
}

int bl_sha256_update(occ_sha256_ctx * ctx, const void * data, u32_t data_len)
{
	occ_sha256_update(ctx, data, (size_t) data_len);
	return 0;
}

int bl_sha256_finish(occ_sha256_ctx * ctx, u8_t * output)
{
	occ_sha256_final(output, ctx); 
	return 0;
}
