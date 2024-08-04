/* $NetBSD$ */

/*-
 * Copyright (c) 2024 Rui-Xiang Guo
 * Copyright (c) 2017 Jared McNeill <jmcneill@invisible.ca>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "opt_soc.h"
#include "opt_multiprocessor.h"
#include "opt_console.h"

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/device.h>
#include <sys/termios.h>

#include <dev/fdt/fdtvar.h>

#include <riscv/fdt/riscv_fdtvar.h>

#include <uvm/uvm_extern.h>

#include <machine/bootconfig.h>

#include <dev/ic/ns16550reg.h>
#include <dev/ic/comreg.h>

#include <riscv/sunxi/sunxi_platform.h>

#include <libfdt.h>

#define	SUNXI_REF_FREQ	24000000

#define	SUN20I_D1_WDT_BASE	0x020500a0
#define	SUN20I_D1_WDT_SIZE	0x20
#define	SUN20I_D1_WDT_CFG	0x14
#define	 SUN20I_D1_WDT_CFG_SYS	__BIT(0)
#define	SUN20I_D1_WDT_MODE	0x18
#define	 SUN20I_D1_WDT_MODE_EN	__BIT(0)

extern struct bus_space riscv_generic_bs_tag;

#define	sunxi_bs_tag		riscv_generic_bs_tag

static bus_space_handle_t reset_bsh;

static const struct pmap_devmap *
sunxi_platform_devmap(void)
{
	static const struct pmap_devmap devmap[] = {
		DEVMAP_ENTRY(SUNXI_CORE_VBASE,
			     SUNXI_CORE_PBASE,
			     SUNXI_CORE_SIZE),
		DEVMAP_ENTRY_END
	};

	return devmap;
}

void sunxi_platform_early_putchar(char);

void __noasan
sunxi_platform_early_putchar(char c)
{
#ifdef CONSADDR
#define CONSADDR_VA	((CONSADDR - SUNXI_CORE_PBASE) + SUNXI_CORE_VBASE)
	volatile uint32_t *uartaddr = cpu_earlydevice_va_p() ?
	    (volatile uint32_t *)CONSADDR_VA :
	    (volatile uint32_t *)CONSADDR;

	while ((le32toh(uartaddr[com_lsr]) & LSR_TXRDY) == 0)
		;

	uartaddr[com_data] = htole32(c);
#endif
}

static u_int
sunxi_platform_uart_freq(void)
{
	return SUNXI_REF_FREQ;
}

static void
sunxi_platform_bootstrap(void)
{
	riscv_fdt_cpu_bootstrap();

	void *fdt_data = __UNCONST(fdtbus_get_data());
	const int chosen_off = fdt_path_offset(fdt_data, "/chosen");
	if (chosen_off < 0)
		return;

	if (match_bootconf_option(boot_args, "console", "fb")) {
		const int framebuffer_off =
		    fdt_path_offset(fdt_data, "/chosen/framebuffer");
		if (framebuffer_off >= 0) {
			const char *status = fdt_getprop(fdt_data,
			    framebuffer_off, "status", NULL);
			if (status == NULL || strncmp(status, "ok", 2) == 0) {
				fdt_setprop_string(fdt_data, chosen_off,
				    "stdout-path", "/chosen/framebuffer");
			}
		}
	} else if (match_bootconf_option(boot_args, "console", "serial")) {
		fdt_setprop_string(fdt_data, chosen_off,
		    "stdout-path", "serial0:115200n8");
	}
}

static void
sun20i_d1_platform_bootstrap(void)
{
	bus_space_tag_t bst = &sunxi_bs_tag;

	sunxi_platform_bootstrap();
	bus_space_map(bst, SUN20I_D1_WDT_BASE, SUN20I_D1_WDT_SIZE, 0, &reset_bsh);
}

static const struct fdt_platform sun20i_d1_platform = {
	.fp_devmap = sunxi_platform_devmap,
	.fp_bootstrap = sun20i_d1_platform_bootstrap,
	.fp_uart_freq = sunxi_platform_uart_freq,
	.fp_mpstart = riscv_fdt_cpu_mpstart,
};

FDT_PLATFORM(sun20i_d1, "allwinner,sun20i-d1", &sun20i_d1_platform);
