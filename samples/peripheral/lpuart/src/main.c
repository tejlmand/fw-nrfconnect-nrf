/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include <zephyr.h>
#include <logging/log.h>
#include <drivers/uart.h>

LOG_MODULE_REGISTER(app);

#define BUF_SIZE 64
static K_MEM_SLAB_DEFINE(uart_slab, BUF_SIZE, 3, 4);

static void uart_callback(const struct device *dev,
			  struct uart_event *evt, void *user_data)
{
	struct device *uart = user_data;
	int err;


	switch (evt->type) {
	case UART_TX_DONE:
		LOG_INF("Tx sent %d bytes", evt->data.tx.len);
		break;

	case UART_TX_ABORTED:
		LOG_ERR("Tx aborted");
		break;

	case UART_RX_RDY:
		LOG_INF("Received data %d bytes", evt->data.rx.len);
		break;

	case UART_RX_BUF_REQUEST:
	{
		uint8_t *buf;

		err = k_mem_slab_alloc(&uart_slab, (void **)&buf, K_NO_WAIT);
		__ASSERT(err == 0, "Failed to allocate slab");

		err = uart_rx_buf_rsp(uart, buf, BUF_SIZE);
		__ASSERT(err == 0, "Failed to provide new buffer");
		break;
	}

	case UART_RX_BUF_RELEASED:
		k_mem_slab_free(&uart_slab, (void **)&evt->data.rx_buf.buf);
		break;

	case UART_RX_DISABLED:
		break;

	case UART_RX_STOPPED:
		break;
	}
}

void main(void)
{
	uint8_t txbuf[5] = {1, 2, 3, 4, 5};
	int err;
	uint8_t *buf;

	k_msleep(1000);

	const struct device *lpuart = device_get_binding("LPUART");
	__ASSERT(lpuart, "Failed to get the device");

	err = k_mem_slab_alloc(&uart_slab, (void **)&buf, K_NO_WAIT);
	__ASSERT(err == 0, "Failed to alloc slab");

	// TODO TORA: upmerge confirmation from KC needed.
	err = uart_callback_set(lpuart, uart_callback, (void *)lpuart);
	__ASSERT(err == 0, "Failed to set callback");

	err = uart_rx_enable(lpuart, buf, BUF_SIZE, 10);
	__ASSERT(err == 0, "Failed to enable RX");

	while (1) {
		err = uart_tx(lpuart, txbuf, sizeof(txbuf), 10);
		__ASSERT(err == 0, "Failed to initiate transmission");

		k_sleep(K_MSEC(500));
	}
}
