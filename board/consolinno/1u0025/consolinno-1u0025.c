// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2023 Sigma Chemnitz GmbH
 */

#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/mach-imx/boot_mode.h>
#include <efi.h>
#include <efi_loader.h>
#include <env_internal.h>
#include <fdt_support.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <net.h>
#include <power/regulator.h>

DECLARE_GLOBAL_DATA_PTR;

void debug_led_toggle_u()
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

int board_phys_sdram_size(phys_size_t *size)
{
  *size = 0x100000000;
  return 0;
}

int fdt_set_usb_eth_addr(void *blob)
{
  printf("ToDo: fdt_set_usb_eth_addr?\n");
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	int ret = fdt_set_usb_eth_addr(blob);

	if (ret)
		return ret;

	return fdt_fixup_memory(blob, PHYS_SDRAM, gd->ram_size);
}

int board_init(void)
{

  debug_led_toggle_u();
  
	regulators_enable_boot_on(false);

	return 0;
}

int board_late_init(void)
{
  printf("ToDo: board_late_init?\n");

	return 0;
}

enum env_location env_get_location(enum env_operation op, int prio)
{
	enum boot_device boot_dev = get_boot_device();

	printf("++env_get_location\n");
	
	if (prio)
		return ENVL_UNKNOWN;

	/*
	 * Make sure that the environment is loaded from
	 * the MMC if we are running from SD card or eMMC.
	 */
	if (CONFIG_IS_ENABLED(ENV_IS_IN_MMC) &&
	    (boot_dev == SD1_BOOT || boot_dev == SD2_BOOT ||
	     boot_dev == MMC1_BOOT || boot_dev == MMC2_BOOT))
		return ENVL_MMC;

	if (CONFIG_IS_ENABLED(ENV_IS_IN_SPI_FLASH))
		return ENVL_SPI_FLASH;

	return ENVL_NOWHERE;
}

#if defined(CONFIG_ENV_IS_IN_MMC)
int board_mmc_get_env_dev(int devno)
{
	return devno;
}

uint mmc_get_env_part(struct mmc *mmc)
{
	/* part 1 (boot0) for eMMC, part 0 (user) for SD card */
	return (mmc_get_env_dev() == 0) ? 1 : 0;
}
#endif
