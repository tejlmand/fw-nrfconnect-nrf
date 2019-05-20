/**
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr.h>
#include <entropy.h>

#if !defined(CONFIG_MBEDTLS_CFG_FILE)
#include "mbedtls/config.h"
#else
#include CONFIG_MBEDTLS_CFG_FILE
#endif /* CONFIG_MBEDTLS_CFG_FILE */

#include "mbedtls/entropy.h"
#include "mbedtls/platform.h"

struct entropy_cc310_rng_dev_data {
	mbedtls_entropy_context context;
	uint32_t		is_initialized;
};

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

	/** Initialize if this is the first call.
	 *  It is assumed that mbedtls_platform_setup
	 *  is called prior to this
	 */
	if(dev_data->is_initialized == 0) {
		mbedtls_entropy_init(&dev_data->context);
		dev_data->is_initialized = true;
	}

	/* Get entropy data */
	res = mbedtls_entropy_func(dev_data, buffer, length);
	return res;
}

static int entropy_cc310_rng_init(struct device *dev)
{
	/** Real initialization is postponed until the caller
	 *  is able to initialize the CC310 hardware by calling
	 *  mbedtls_platform_setup
	 */
	struct entropy_cc310_rng_dev_data *dev_data;

	__ASSERT_NO_MSG(dev != NULL);

	dev_data = DEV_DATA(dev);

	__ASSERT_NO_MSG(dev_data != NULL);

	return 0;
}

static const struct entropy_driver_api entropy_cc310_rng_api = {
	.get_entropy = entropy_cc310_rng_get_entropy
};

static struct entropy_cc310_rng_dev_data entropy_cc310_rng_data = {0,};

DEVICE_AND_API_INIT(entropy_cc310_rng, CONFIG_ENTROPY_NAME,
		    entropy_cc310_rng_init,
		    &entropy_cc310_rng_data,
		    NULL,
		    PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
		    &entropy_cc310_rng_api);

