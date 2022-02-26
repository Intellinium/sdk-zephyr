/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr.h>
#include <device.h>
#include <init.h>
#include <pm/pm.h>
#include "retained.h"
#include <hal/nrf_gpio.h>

#define BUSY_WAIT_S 2U
#define SLEEP_S 2U

/* Prevent deep sleep (system off) from being entered on long timeouts
 * or `K_FOREVER` due to the default residency policy.
 *
 * This has to be done before anything tries to sleep, which means
 * before the threading system starts up between PRE_KERNEL_2 and
 * POST_KERNEL.  Do it at the start of PRE_KERNEL_2.
 */
static int disable_ds_1(const struct device *dev)
{
	ARG_UNUSED(dev);

	pm_constraint_set(PM_STATE_SOFT_OFF);
	return 0;
}

SYS_INIT(disable_ds_1, PRE_KERNEL_2, 0);

#include <device.h>
#include <drivers/gpio.h>
void main(void)
{
	int rc;
	const struct device *uart0 = device_get_binding("UART_0");
	const struct device *uart1 = device_get_binding("UART_1");
	
	printk("\n%s system off demo\n", CONFIG_BOARD);

	if (IS_ENABLED(CONFIG_APP_RETENTION)) {
		bool retained_ok = retained_validate();

		/* Increment for this boot attempt and update. */
		retained.boots += 1;
		retained_update();

		printk("Retained data: %s\n", retained_ok ? "valid" : "INVALID");
		printk("Boot count: %u\n", retained.boots);
		printk("Off count: %u\n", retained.off_count);
		printk("Active Ticks: %" PRIu64 "\n", retained.uptime_sum);
	} else {
		printk("Retained data not supported\n");
	}

	/* Configure to generate PORT event (wakeup) on button 1 press. */
	nrf_gpio_cfg_input(DT_GPIO_PIN(DT_NODELABEL(button0), gpios),
			   NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_sense_set(DT_GPIO_PIN(DT_NODELABEL(button0), gpios),
			       NRF_GPIO_PIN_SENSE_LOW);

	printk("Busy-wait %u s\n", BUSY_WAIT_S);
	k_busy_wait(BUSY_WAIT_S * USEC_PER_SEC);

	printk("Busy-wait %u s with UART off\n", BUSY_WAIT_S);
	if (uart0) {
		rc = pm_device_state_set(uart0, PM_DEVICE_STATE_SUSPENDED);
	}
	if (uart1) {
		rc = pm_device_state_set(uart1, PM_DEVICE_STATE_SUSPENDED);
	}
	k_busy_wait(BUSY_WAIT_S * USEC_PER_SEC);
	if (uart0) {
		rc = pm_device_state_set(uart0, PM_DEVICE_STATE_ACTIVE);
	}
	if (uart1) {
		rc = pm_device_state_set(uart1, PM_DEVICE_STATE_ACTIVE);
	}

	printk("Sleep %u s\n", SLEEP_S);
	k_sleep(K_SECONDS(SLEEP_S));

	printk("Sleep %u s with UART off\n", SLEEP_S);
	if (uart0) {
		rc = pm_device_state_set(uart0, PM_DEVICE_STATE_SUSPENDED);
	}
	if (uart1) {
		rc = pm_device_state_set(uart1, PM_DEVICE_STATE_SUSPENDED);
	}
	k_sleep(K_SECONDS(SLEEP_S));
	if (uart0) {
		rc = pm_device_state_set(uart0, PM_DEVICE_STATE_ACTIVE);
	}
	if (uart1) {
		rc = pm_device_state_set(uart1, PM_DEVICE_STATE_ACTIVE);
	}

#if defined(NRF_P1)
	const struct device *gpio = device_get_binding("GPIO_1");
	gpio_pin_configure(gpio, 2 , GPIO_OUTPUT);
	gpio_pin_set_raw(gpio, 2, 0);
#else
	const struct device *gpio = device_get_binding("GPIO_0");
	gpio_pin_configure(gpio, 0 , GPIO_OUTPUT);
	gpio_pin_set_raw(gpio, 0, 0);
#endif

	printk("Current GPIO configurations\n");
	printk("GPIO values: 0x%08X\n", NRF_PERIPH(NRF_P0)->IN);

	for (int i = 0; i < 32; i++) {
		printk("GPIO 0.%2d: 0x%08X\n", i,
		       NRF_PERIPH(NRF_P0)->PIN_CNF[i]);
	}

	printk("Entering system off; press BUTTON1 to restart\n");

	k_sleep(K_SECONDS(1));
	if (IS_ENABLED(CONFIG_APP_RETENTION)) {
		/* Update the retained state */
		retained.off_count += 1;
		retained_update();
	}

	/* Above we disabled entry to deep sleep based on duration of
	 * controlled delay.  Here we need to override that, then
	 * force entry to deep sleep on any delay.
	 */
	pm_power_state_force((struct pm_state_info){PM_STATE_SOFT_OFF, 0, 0});

	printk("ERROR: System off failed\n");
	while (true) {
		/* spin to avoid fall-off behavior */
	}
}
