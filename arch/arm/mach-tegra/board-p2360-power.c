/*
 * arch/arm/mach-tegra/board-p2360-power.c
 * Based on arch/arm/mach-tegra/board-vcm30_t124-power.c
 *
 * Copyright (c) 2013-2014, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/mfd/max77663-core.h>
#include <linux/regulator/max77663-regulator.h>
#include <linux/regulator/max15569-regulator.h>
#include <linux/gpio.h>

#include "pm.h"
#include "board.h"
#include "gpio-names.h"
#include "board-common.h"
#include "board-p2360.h"
#include "tegra_cl_dvfs.h"
#include "devices.h"
#include <mach/board_id.h>

static struct max77663_gpio_config max77663_gpio_cfgs[] = {
	{
		.gpio = MAX77663_GPIO5,
		.dir = GPIO_DIR_OUT,
		.dout = GPIO_DOUT_HIGH,
		.out_drv = GPIO_OUT_DRV_PUSH_PULL,
		.alternate = GPIO_ALT_DISABLE,
	},
};

static struct max77663_platform_data max77663_pdata = {
	.irq_base	= MAX77663_IRQ_BASE,
	.gpio_base	= MAX77663_GPIO_BASE,

	.num_gpio_cfgs	= ARRAY_SIZE(max77663_gpio_cfgs),
	.gpio_cfgs	= max77663_gpio_cfgs,

	.use_power_off	= false,
};

static struct i2c_board_info __initdata max77663_regulators[] = {
	{
		/* The I2C address was determined by OTP factory setting */
		I2C_BOARD_INFO("max77663", 0x3c),
		.irq		= -1,
		.platform_data	= &max77663_pdata,
	},
};

/* MAX15569 switching regulator for vdd_cpu */
static struct regulator_consumer_supply max15569_vddcpu_supply[] = {
	REGULATOR_SUPPLY("vdd_cpu", NULL),
};

static struct regulator_init_data max15569_vddcpu_init_data = {
	.constraints = {
		.min_uV = 500000,
		.max_uV = 1520000,
		.valid_modes_mask = (REGULATOR_MODE_NORMAL |
				REGULATOR_MODE_STANDBY),
		.valid_ops_mask = (REGULATOR_CHANGE_MODE |
				REGULATOR_CHANGE_STATUS |
				REGULATOR_CHANGE_CONTROL |
				REGULATOR_CHANGE_VOLTAGE),
		.always_on = 1,
		.boot_on =  1,
		.apply_uV = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(max15569_vddcpu_supply),
	.consumer_supplies = max15569_vddcpu_supply,
};

static struct max15569_regulator_platform_data max15569_vddcpu_pdata = {
	.reg_init_data = &max15569_vddcpu_init_data,
	.max_voltage_uV = 1520000,
	.slew_rate_mv_per_us = 44,
};

static struct i2c_board_info __initdata max15569_vddcpu_boardinfo[] = {
	{
		I2C_BOARD_INFO("max15569", 0x3a),
		.platform_data  = &max15569_vddcpu_pdata,
	},
};

/* MAX15569 switching regulator for vdd_gpu */
static struct regulator_consumer_supply max15569_vddgpu_supply[] = {
	REGULATOR_SUPPLY("vdd_gpu", NULL),
};

static struct regulator_init_data max15569_vddgpu_init_data = {
	.constraints = {
		.min_uV = 500000,
		.max_uV = 1520000,
		.valid_modes_mask = (REGULATOR_MODE_NORMAL |
				REGULATOR_MODE_STANDBY),
		.valid_ops_mask = (REGULATOR_CHANGE_MODE |
				REGULATOR_CHANGE_STATUS |
				REGULATOR_CHANGE_CONTROL |
				REGULATOR_CHANGE_VOLTAGE),
		.always_on = 0,
		.boot_on =  0,
		.apply_uV = 0,
	},
	.num_consumer_supplies = ARRAY_SIZE(max15569_vddgpu_supply),
	.consumer_supplies = max15569_vddgpu_supply,
};

static struct max15569_regulator_platform_data max15569_vddgpu_pdata = {
	.reg_init_data = &max15569_vddgpu_init_data,
	.max_voltage_uV = 1400000,
	.slew_rate_mv_per_us = 44,
};

static struct i2c_board_info __initdata max15569_vddgpu_boardinfo[] = {
	{
		I2C_BOARD_INFO("max15569", 0x3b),
		.platform_data  = &max15569_vddgpu_pdata,
	},
};

static int __init p2360_max77663_regulator_init(void)
{
	i2c_register_board_info(4, max77663_regulators,
				ARRAY_SIZE(max77663_regulators));

	return 0;
}

int __init p2360_regulator_init(void)
{
	int err;

	p2360_max77663_regulator_init();
	i2c_register_board_info(4, max15569_vddcpu_boardinfo, 1);
	i2c_register_board_info(4, max15569_vddgpu_boardinfo, 1);

	/* Assert GPU_PWR_REQ_R GPIO */
	/* FIXME: should rather go to a GPU init callback */
	err = gpio_request(TEGRA_GPIO_GPUPWR, "gpu_pwr_req");
	if (err < 0) {
		pr_err("Err %d: GPU Power GPIO request failed\n", err);
		return err;
	}
	gpio_direction_output(TEGRA_GPIO_GPUPWR, 1);

	return 0;
}

static struct tegra_suspend_platform_data p2360_suspend_data = {
	.cpu_timer	= 2000,
	.cpu_off_timer	= 2000,
	.suspend_mode	= TEGRA_SUSPEND_LP0,
	.core_timer	= 0xfefe,
	.core_off_timer = 2000,
	.corereq_high	= true,
	.sysclkreq_high	= true,
	.cpu_lp2_min_residency = 1000,
};

int __init p2360_suspend_init(void)
{
	tegra_init_suspend(&p2360_suspend_data);
	return 0;
}