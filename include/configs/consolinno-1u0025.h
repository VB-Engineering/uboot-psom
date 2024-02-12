/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2023 Sigma Chemnitz GmbH
 *
 */
#ifndef __CONSOLINNO_1U0025_CONFIG_H
#define __CONSOLINNO_1U0025_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <linux/sizes.h>

#ifdef CONFIG_SPL_BUILD
#include <config.h>
#endif

/* RAM */
#define PHYS_SDRAM			DDR_CSD1_BASE_ADDR
#define PHYS_SDRAM_SIZE			(SZ_4G)
#define CFG_SYS_SDRAM_BASE		PHYS_SDRAM

#define CFG_SYS_INIT_RAM_ADDR	0x40000000
#define CFG_SYS_INIT_RAM_SIZE	0x200000

/* Board and environment settings */

#ifdef CONFIG_USB_EHCI_HCD
#define CFG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CFG_MXC_USB_FLAGS		0
#endif

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 0) \
	func(USB, usb, 0) \
	func(PXE, pxe, na)
#include <config_distro_bootcmd.h>
/* Do not try to probe USB net adapters for net boot */
#undef BOOTENV_RUN_NET_USB_START
#define BOOTENV_RUN_NET_USB_START

#ifdef CONFIG_SPL_BUILD
/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000
#endif

#undef CFG_EXTRA_ENV_SETTINGS

#endif /* __CONSOLINNO_1U0025_CONFIG_H */
