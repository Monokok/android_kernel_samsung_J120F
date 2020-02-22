/*
 * SAMSUNG EXYNOS3475 Flattened Device Tree enabled machine
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <linux/memblock.h>
#include <linux/io.h>
#include <linux/clocksource.h>
#include <linux/exynos_ion.h>
#include <linux/types.h>

#include <asm/mach/arch.h>
#include <mach/regs-pmu.h>

#include <plat/cpu.h>
#include <plat/mfc.h>

#include <asm/mach/map.h>
#include <asm/setup.h>

#include "common.h"

#ifdef CONFIG_USE_DIRECT_IS_CONTROL
static phys_addr_t dico_phys_addr;
#endif
#ifdef CONFIG_USE_HOST_FD_LIBRARY
static phys_addr_t fd_phys_addr;
#endif

const struct of_device_id of_iommu_bus_match_table[] = {
	{ .compatible = "samsung,exynos-iommu-bus", },
	{} /* Empty terminated list */
};

static void __init universal3475_dt_map_io(void)
{
#ifdef CONFIG_USE_DIRECT_IS_CONTROL
	struct map_desc dico_iodesc;
#endif
#ifdef CONFIG_USE_HOST_FD_LIBRARY
	struct map_desc fd_iodesc;
#endif

	exynos_init_io(NULL, 0);

#ifdef CONFIG_USE_DIRECT_IS_CONTROL
	dico_iodesc.virtual = (ulong)S5P_VA_SDK_LIB;
	dico_iodesc.pfn = __phys_to_pfn(dico_phys_addr);
	dico_iodesc.length = SZ_2M;
	dico_iodesc.type = MT_MEMORY_RW;

	iotable_init(&dico_iodesc, 1);
#endif
#ifdef CONFIG_USE_HOST_FD_LIBRARY
	fd_iodesc.virtual = (ulong)S5P_VA_FD_LIB;
	fd_iodesc.pfn = __phys_to_pfn(fd_phys_addr);
	fd_iodesc.length = SZ_512K;
	fd_iodesc.type = MT_MEMORY_RW;

	iotable_init(&fd_iodesc, 1);
#endif
}

static void __init universal3475_dt_machine_init(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
	of_platform_bus_probe(NULL, of_iommu_bus_match_table, NULL);
}

static char const *universal3475_dt_compat[] __initdata = {
	"samsung,exynos3475",
	NULL
};

static void __init universal3475_reserve(void)
{
#if defined(CONFIG_LINK_DEVICE_SHMEM)
	phys_addr_t last_bank_base = meminfo.bank[meminfo.nr_banks-1].start;
	phys_addr_t last_bank_size = meminfo.bank[meminfo.nr_banks-1].size;
	u32 cp_base;

	cp_base = (last_bank_base + last_bank_size) & 0xF0000000;

	pr_err("last_bank_base: 0x%08X, cp_base: 0x%08X\n", last_bank_base, cp_base);
	memblock_reserve(cp_base, 0x5c00000);

#endif
#ifdef CONFIG_USE_DIRECT_IS_CONTROL
	dico_phys_addr = memblock_alloc(SZ_2M, SZ_4K);
#endif
#ifdef CONFIG_USE_HOST_FD_LIBRARY
	fd_phys_addr = memblock_alloc(SZ_512K, SZ_4K);
#endif
}

DT_MACHINE_START(UNIVERSAL3475, "UNIVERSAL3475")
	.init_irq	= exynos5_init_irq,
	.smp		= smp_ops(exynos_smp_ops),
	.map_io		= universal3475_dt_map_io,
	.init_machine	= universal3475_dt_machine_init,
	.init_late	= exynos_init_late,
	.init_time	= exynos_init_time,
	.dt_compat	= universal3475_dt_compat,
	.restart        = exynos5_restart,
	.reserve	= universal3475_reserve,
MACHINE_END
