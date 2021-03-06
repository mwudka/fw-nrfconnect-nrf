/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <init.h>
#include <drivers/gpio.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(board_control, CONFIG_BOARD_PCA20035_LOG_LEVEL);


__packed struct pin_config {
	u8_t pin;
	u8_t val;
};

static void chip_reset(struct device *gpio,
		       struct gpio_callback *cb, u32_t pins)
{
	const u32_t stamp = k_cycle_get_32();

	printk("GPIO reset line asserted, device reset.\n");
	printk("Bye @ cycle32 %u\n", stamp);

	NVIC_SystemReset();
}

static void reset_pin_wait_low(struct device *port, u32_t pin)
{
	int err;
	u32_t val;
        printk("reset_pin_wait_low");

	/* Wait until the pin is pulled low */
	do {
		err = gpio_pin_read(port, pin, &val);
	} while (err == 0 && val != 0);
        printk("gpio_pin_read, err: %d with val: %d",err,val);
}

static int reset_pin_configure(struct device *p0, struct device *p1)
{
	int err;
	u32_t pin = 0;
	struct device *port = NULL;

	static struct gpio_callback gpio_ctx;

	port = p0;
	pin = 20;



	__ASSERT_NO_MSG(port != NULL);

	err = gpio_pin_configure(port, pin,
				 GPIO_DIR_IN | GPIO_INT | GPIO_PUD_PULL_DOWN |
				 GPIO_INT_ACTIVE_HIGH | GPIO_INT_EDGE);
	if (err) {
		return err;
	}

	gpio_init_callback(&gpio_ctx, chip_reset, BIT(pin));

	err = gpio_add_callback(port, &gpio_ctx);
	if (err) {
		return err;
	}

	err = gpio_pin_enable_callback(port, pin);
	if (err) {
		return err;
	}

	/* Wait until the pin is pulled low before continuing.
	 * This lets the other side ensure that they are ready.
	 */
	LOG_INF("GPIO reset line enabled on pin %s.%02u, holding..",
		port == p0 ? "P0" : "P1", pin);

	reset_pin_wait_low(port, pin);

	return 0;
}

static int init(struct device *dev)
{
	int rc;
	struct device *p0;
	struct device *p1;

	p0 = device_get_binding(DT_GPIO_P0_DEV_NAME);
	if (!p0) {
		LOG_ERR("GPIO device " DT_GPIO_P0_DEV_NAME "not found!");
		return -EIO;
	}

	p1 = device_get_binding(DT_GPIO_P1_DEV_NAME);
	if (!p1) {
		LOG_ERR("GPIO device " DT_GPIO_P1_DEV_NAME " not found!");
		return -EIO;
	}

	rc = reset_pin_configure(p0, p1);
	if (rc) {
		LOG_ERR("Unable to configure reset pin, err %d", rc);
		return -EIO;
	}


	LOG_INF("Board configured.");

	return 0;
}

SYS_INIT(init, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);