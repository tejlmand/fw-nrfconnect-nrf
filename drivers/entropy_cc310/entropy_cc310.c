/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr.h>

#if CONFIG_ENTROPY_CC310

#if defined(CONFIG_MBEDTLS)
#if !defined(CONFIG_MBEDTLS_CFG_FILE)
#include "mbedtls/config.h"
#else
#include CONFIG_MBEDTLS_CFG_FILE
#endif /* CONFIG_MBEDTLS_CFG_FILE */

#include "mbedtls/entropy.h"

LOG_MODULE_REGISTER(entropy_cc310, ENTROPY_CC310_LOG_LEVEL);

struct entropy_cc310_rng_dev_data {
	mbedtls_entropy_context context;
}

#define DEV_DATA(dev) \
	((struct entropy_cc310_rng_dev_data*)(dev)->driver_data)

static int entropy_cc310_rng_get_entropy(struct device *dev, u8_t *buffer,
					 u16_t length)
{
	struct entropy_cc310_rng_dev_data *dev_data;
	int res;

	__ASSERT_NO_MSG(dev != NULL);
	__ASSERT_NO_MSG(buffer != NULL);

	dev_data = DEV_DATA(dev);

	__ASSERT_NO_MSG(dev_data != NULL);

	/* Get entropy data */
	res = mbedtls_entropy_func(dev_data, buffer, length);
	return res;
}

static int entropy_cc310_rng_init(struct device *dev)
{
	struct entropy_cc310_rng_dev_data *dev_data;
	struct entropy_cc310_rng_dev_cfg *cfg_data;

	__ASSERT_NO_MSG(dev != NULL);

	dev_data = DEV_DATA(dev);

	__ASSERT_NO_MSG(dev_data != NULL);

	mbedtls_entropy_init(dev_data->context);

	return 0;
}

static const struct entropy_driver_api entropy_cc310_rng_api = {
	.get_entropy = entropy_cc310_rng_get_entropy
};

static const struct entropy_cc310_rng_dev_cfg entropy_cc310_rng_config = {0,};

static struct entropy_cc310_rng_dev_data entropy_cc310_rng_data = {0,};

DEVICE_AND_API_INIT(entropy_cc310_rng, CONFIG_ENTROPY_NAME,
		    entropy_cc310_rng_init,
		    &entropy_cc310_rng_data,
		    &entropy_cc310_rng_config,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    &entropy_cc310_rng_api);

#endif /* CONFIG_ENTROPY_CC310 */
