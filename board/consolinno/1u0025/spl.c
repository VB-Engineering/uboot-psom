// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 Sigma Chemnitz GmbH
 */

#include <asm/arch/imx8mm_pins.h>
#include <asm/arch/clock.h>
#include <asm/arch/ddr.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/global_data.h>
#include <asm/gpio.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/iomux-v3.h>
#include <dm/uclass.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <hang.h>
#include <i2c.h>
#include <init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <power/pca9450.h>
#include <power/pmic.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

#define I2C_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_HYS | PAD_CTL_PUE)

static iomux_v3_cfg_t const i2c1_pads[] = {
	IMX8MM_PAD_I2C1_SCL_I2C1_SCL | MUX_PAD_CTRL(I2C_PAD_CTRL) | MUX_MODE_SION,
	IMX8MM_PAD_I2C1_SDA_I2C1_SDA | MUX_PAD_CTRL(I2C_PAD_CTRL) | MUX_MODE_SION
};

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
  printf("spl_board_boot_device %i\n", boot_dev_spl);
  
	switch (boot_dev_spl) {
	case USB_BOOT:
	  printf("bootdev is usb\n");
		return BOOT_DEVICE_BOARD;
	case SPI_NOR_BOOT:
		return BOOT_DEVICE_SPI;
	case SD1_BOOT:
	case MMC1_BOOT:
	  printf("bootdev is mmc1\n");	  
		return BOOT_DEVICE_MMC1;
	case SD2_BOOT:
	case MMC2_BOOT:
	  printf("bootdev is mmc2\n");	  
		return BOOT_DEVICE_MMC2;
	default:
	  printf("bootdev is none\n");	  
		return BOOT_DEVICE_NONE;
	}
}

bool check_ram_available(long size)
{
	long sz = get_ram_size((long *)PHYS_SDRAM, size);

	if (sz == size)
		return true;

	return false;
}

static void spl_dram_init(void)
{
	u32 size = 0;

	/*
	 * Try the default DDR settings in lpddr4_timing.c to
	 * comply with the Micron 4GB DDR.
	 */
	if (!ddr_init(&dram_timing) && check_ram_available(SZ_4G)) {
		size = 4;
	} else {
		/*
		 * Overwrite some values to comply with the Micron 1GB/2GB DDRs.
		 */
		dram_timing.ddrc_cfg[2].val = 0xa1080020;
		dram_timing.ddrc_cfg[37].val = 0x1f;

		dram_timing.fsp_msg[0].fsp_cfg[8].val = 0x110;
		dram_timing.fsp_msg[0].fsp_cfg[20].val = 0x1;
		dram_timing.fsp_msg[1].fsp_cfg[9].val = 0x110;
		dram_timing.fsp_msg[1].fsp_cfg[21].val = 0x1;
		dram_timing.fsp_msg[2].fsp_cfg[10].val = 0x110;
		dram_timing.fsp_msg[2].fsp_cfg[22].val = 0x1;

		if (!ddr_init(&dram_timing)) {
			if (check_ram_available(SZ_2G))
				size = 2;
			else if (check_ram_available(SZ_1G))
				size = 1;
		}
	}

	if (size == 0) {
		printf("Failed to initialize DDR RAM!\n");
		size = 1;
	}

	gd->ram_size = size;

	//ToDo: what is this good for?
	//writel(size, M4_BOOTROM_BASE_ADDR);
}

int do_board_detect(void)
{
  puts("ToDo: do_board_detect?\n");
	return 0;
}

int board_fit_config_name_match(const char *name)
{
  puts("ToDo: board_fit_config_name_match?\n");
  return -1;
}

void spl_board_init(void)
{
	struct udevice *dev;
	int ret;

	arch_misc_init();

	puts("Normal Boot Consolinno\n");

	ret = uclass_get_device_by_name(UCLASS_CLK,
					"clock-controller@30380000",
					&dev);
	if (ret < 0)
		printf("Failed to find clock node. Check device tree\n");
}

static int power_init_board(void)
{
  puts("ToDo: power_init_board\n");
 

	return 0;
}

void debug_led_toggle()
{
	uint32_t *gp_dr=0x30200000;
	uint32_t *gp_gdir=0x30200004;

	uint32_t tmp;
	tmp = *gp_dr;
	tmp |= (1 << 9);
	*gp_dr = tmp;

	tmp = *gp_gdir;
	tmp |= (1 << 9);
	*gp_gdir = tmp;  
}

void board_init_f(ulong dummy)
{
	int ret;

	arch_cpu_init();

	init_uart_clk(1);

	timer_init();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	ret = spl_init();
	if (ret) {
		debug("spl_init() failed: %d\n", ret);
		hang();
	}

	puts("board_init_f: spl_init done\n");
	
	preloader_console_init();

	//puts("ToDo: enable_tzc380?\n");
	//enable_tzc380();

	/* PMIC initialization */
	//power_init_board();

	/* DDR initialization */
	spl_dram_init();

	/* Detect the board type */
	//puts("ToDo: do_board_detect?\n");
	//do_board_detect();

	board_init_r(NULL, 0);
}
