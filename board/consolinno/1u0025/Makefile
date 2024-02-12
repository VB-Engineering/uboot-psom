# SPDX-License-Identifier: GPL-2.0+
# (C) Copyright 2023 Sigma Chemnitz GmbH

obj-y := consolinno-1u0025.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing.o
endif
