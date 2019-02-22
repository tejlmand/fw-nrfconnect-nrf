/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */

#include "debug.h"
#include <zephyr/types.h>
#include <toolchain.h>
#include <misc/util.h>
#include <nrf.h>
#include <errno.h>
#include <generated_dts_board.h>
#include <pm_config.h>
#include "bl_crypto.h"
#include "fw_metadata.h"

#ifdef CONFIG_SB_FLASH_PROTECT
#include <fprotect.h>
#endif

#include <provision.h>
#define B0_PREFIX "<bootloader>: "
/* These __weak functions are needed by the cc310_bl and some other code, but
 * not (always) included in the bootloader.
 */
void *__weak memset(void *buf, int c, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		((u8_t *)buf)[i] = c;
	}
	return buf;
}

void *__weak memcpy(void *restrict d, const void *restrict s, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		((u8_t *)d)[i] = ((u8_t *)s)[i];
	}
	return d;
}

void *memcpy32(void *restrict d, const void *restrict s, size_t n)
{
	size_t len_words = ROUND_DOWN(n, 4) / 4;
	for (size_t i = 0; i < len_words; i++) {
		((u32_t *)d)[i] = ((u32_t *)s)[i];
	}
	return d;
}

static bool verify_firmware(u32_t address)
{
	int retval = -EFAULT;
	int err;
	const struct fw_firmware_info *fw_info;
	const struct fw_validation_info *fw_ver_info;
	u8_t key_data[CONFIG_SB_PUBLIC_KEY_HASH_LEN];

	fw_info = firmware_info_get(address);

	printk(B0_PREFIX"Attempting to boot from address 0x%x.\n\r", address);

	if (!fw_info) {
		printk(B0_PREFIX"%s\n\r", "Could not find valid firmware info inside "
				    "firmware. Aborting boot!");
		return false;
	}

	fw_ver_info = validation_info_find(fw_info, 4);

	if (!fw_ver_info) {
		printk(B0_PREFIX"%s\n\r",
			    "Could not find valid firmware validation "
			    "info trailing firmware. Aborting boot!\n\r");
		return false;
	}

	err = crypto_init();
	if (err) {
		printk(B0_PREFIX"crypto_init() returned %d. Aborting boot!\n\r", err);
		return false;
	}

	u32_t num_public_keys = num_public_keys_read();

	for (u32_t key_data_idx = 0; key_data_idx < num_public_keys;
			key_data_idx++) {
		if (public_key_data_read(key_data_idx, &key_data[0],
				CONFIG_SB_PUBLIC_KEY_HASH_LEN) < 0) {
			retval = -EFAULT;
			break;
		}
		retval = crypto_root_of_trust(fw_ver_info->public_key,
					      key_data,
					      fw_ver_info->signature,
					      (u8_t *)address,
					      fw_info->firmware_size);
		if (retval != -ESIGINV) {
			break;
		}
	}

	if (retval != 0) {
		printk(B0_PREFIX"Firmware validation failed with error %d. "
			    "Aborting boot!\n\r",
			    retval);
		return false;
	}

	return true;
}

void uninit_used_peripherals(void)
{
	/* We do not want to uninitialize cryptocell as we want to retain the
	 * root of trust key loaded inside cryptocell.
	 */
#ifdef CONFIG_SB_DEBUG_PORT_UART
	uart_uninit();
#endif
}

#ifdef CONFIG_SW_VECTOR_RELAY
#ifndef CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
_GENERIC_SECTION(.vt_pointer_section) u32_t _vector_table_pointer;
#endif
extern u32_t _vector_table_pointer;
#define VTOR _vector_table_pointer
#else
#define VTOR SCB->VTOR
#endif

static void boot_from(u32_t *address)
{
	if (!verify_firmware((u32_t)address)) {
		return;
	}

	__ASSERT(!(CONTROL_nPRIV_Msk & __get_CONTROL()),
			"Not in Privileged mode");

	/* Allow any pending interrupts to be recognized */
	__ISB();
	__disable_irq();
	NVIC_Type *nvic = NVIC;
	/* Disable NVIC interrupts
	 * TODO: @sigvartmh May be redundant CPSID would maybe clear this
	 */
	for (u8_t i = 0; i < ARRAY_SIZE(nvic->ICER); i++) {
		nvic->ICER[i] = 0xFFFFFFFF;
	}
	/* Clear pending NVIC interrupts */
	for (u8_t i = 0; i < ARRAY_SIZE(nvic->ICPR); i++) {
		nvic->ICPR[i] = 0xFFFFFFFF;
	}

	uninit_used_peripherals();

	SysTick->CTRL = 0;

	/* Disable fault handlers used by the bootloader
	 * TODO: @sigvartmh currently not implemented or used
	 */
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

#ifndef CONFIG_CPU_CORTEX_M0
	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
			SCB_SHCSR_MEMFAULTENA_Msk);
#endif

	/* Activate the MSP if the core is found to currently run with the PSP */
	if (CONTROL_SPSEL_Msk & __get_CONTROL()) {
		__set_CONTROL(__get_CONTROL() & ~CONTROL_SPSEL_Msk);
	}

	__DSB(); /* Force Memory Write before continuing */
	__ISB(); /* Flush and refill pipeline with updated premissions */

	VTOR = (u32_t)address;

	const struct fw_firmware_info *fw_info;
	extern const struct fw_abi_getter_info m_abi_getter;
	fw_info = firmware_info_get((u32_t)address);

	if (fw_info != NULL) {
		memcpy(fw_info->abi_in, &m_abi_getter,
			sizeof(struct fw_abi_getter_info));
	}

	/* Set MSP to the new address and clear any information from PSP */
	__set_MSP(address[0]);
	__set_PSP(0);

	/* Call reset handler. */
	((void (*)(void))address[1])();
	CODE_UNREACHABLE;
}

#if CONFIG_SB_C_RUNTIME_SETUP_VARIANT_ZEPHYR
void _Cstart(void) __attribute__((alias("main_bl")));
#endif
void main_bl(void)
{
#if CONFIG_SB_FLASH_PROTECT
//TODO use proper defines
	int err = fprotect_area(PM_CFG_B0_ADDRESS,
			        PM_CFG_B0_SIZE);
	if (err) {
		printk(B0_PREFIX"Protect B0 flash failed, cancel startup.\n\r");
		return;
	}

#ifndef CONFIG_SOC_NRF9160
	err = fprotect_area(PM_CFG_PROVISION_ADDRESS,
			PM_CFG_PROVISION_SIZE);
	if (err) {
		printk(B0_PREFIX"Protect provision data failed, cancel startup.\n\r");
		return;
	}
#endif /* CONFIG_SOC_NRF9160 */

#endif /* CONFIG_SB_FLASH_PROTECT */

#if defined(CONFIG_SB_DEBUG_PORT_SEGGER_RTT)
	SEGGER_RTT_Init();
#elif defined(CONFIG_SB_DEBUG_PORT_UART)
	uart_init();
#endif /* CONFIG_SB_RTT */
	boot_from((u32_t *)s0_address_read());
	CODE_UNREACHABLE;
}
