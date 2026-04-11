/* $NetBSD$ */

/*-
 * Copyright (c) 2026 Rui-Xiang Guo
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

#include <sys/cdefs.h>

__KERNEL_RCSID(1, "$NetBSD$");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>
#include <sys/systm.h>

#include <dev/fdt/fdtvar.h>

#include <arm/sunxi/sunxi_ccu.h>
#include <riscv/sunxi/sun20i_d1_r_ccu.h>

#define	AHB_CFG_REG		0x000
#define	APB0_CFG_REG		0x00c
#define	IRRX_CFG_REG		0x1c0

#define	TIMER_BGR_REG		0x11c
#define	TWD_BGR_REG		0x12c
#define	PPU_BGR_REG		0x1ac
#define	IRRX_BGR_REG		0x1cc
#define	RTC_BGR_REG		0x20c
#define	CPUCFG_BGR_REG		0x22c

static int sun20i_d1_r_ccu_match(device_t, cfdata_t, void *);
static void sun20i_d1_r_ccu_attach(device_t, device_t, void *);

static const struct device_compatible_entry compat_data[] = {
	{ .compat = "allwinner,sun20i-d1-r-ccu" },
	DEVICE_COMPAT_EOL
};

CFATTACH_DECL_NEW(sunxi_d1_r_ccu, sizeof(struct sunxi_ccu_softc),
	sun20i_d1_r_ccu_match, sun20i_d1_r_ccu_attach, NULL, NULL);

static struct sunxi_ccu_reset sun20i_d1_r_ccu_resets[] = {
	SUNXI_CCU_RESET(D1_R_RST_BUS_TIMER, TIMER_BGR_REG, 16),
	SUNXI_CCU_RESET(D1_R_RST_BUS_TWD, TWD_BGR_REG, 16),
	SUNXI_CCU_RESET(D1_R_RST_BUS_PPU, PPU_BGR_REG, 16),
	SUNXI_CCU_RESET(D1_R_RST_BUS_IRRX, IRRX_BGR_REG, 16),
	SUNXI_CCU_RESET(D1_R_RST_BUS_RTC, RTC_BGR_REG, 16),
	SUNXI_CCU_RESET(D1_R_RST_BUS_CPUCFG, CPUCFG_BGR_REG, 16),
};

static const char *ahb_parents[] = { "hosc", "losc", "iosc", "pll_periph" };
static const char *apb0_parents[] = { "hosc", "losc", "iosc", "pll_periph" };
static const char *mod_parents[] = { "losc", "hosc" };

static struct sunxi_ccu_clk sun20i_d1_r_ccu_clks[] = {
	SUNXI_CCU_NM(D1_R_CLK_AHB, "r-ahb", ahb_parents,
	    AHB_CFG_REG,	/* reg */
	    __BITS(9,8),	/* n */
	    __BITS(4,0),	/* m */
	    __BITS(26,24),	/* sel */
	    0,			/* enable */
	    SUNXI_CCU_NM_POWER_OF_TWO),

	SUNXI_CCU_NM(D1_R_CLK_APB0, "r-apb0", apb0_parents,
	    APB0_CFG_REG,	/* reg */
	    __BITS(9,8),	/* n */
	    __BITS(4,0),	/* m */
	    __BITS(26,24),	/* sel */
	    0,			/* enable */
	    SUNXI_CCU_NM_POWER_OF_TWO),

	SUNXI_CCU_NM(D1_R_CLK_IRRX, "r-irrx", mod_parents,
	    IRRX_CFG_REG,	/* reg */
	    __BITS(9,8),	/* n */
	    __BITS(4,0),	/* m */
	    __BITS(25,24),	/* sel */
	    __BIT(31),		/* enable */
	    SUNXI_CCU_NM_POWER_OF_TWO),

	SUNXI_CCU_GATE(D1_R_CLK_BUS_TIMER, "bus-r-timer", "r-apb0",
	    TIMER_BGR_REG, 0),
	SUNXI_CCU_GATE(D1_R_CLK_BUS_TWD, "bus-r-twd", "r-apb0",
	    TWD_BGR_REG, 0),
	SUNXI_CCU_GATE(D1_R_CLK_BUS_PPU, "bus-r-ppu", "r-apb0",
	    PPU_BGR_REG, 0),
	SUNXI_CCU_GATE(D1_R_CLK_BUS_IRRX, "bus-r-irrx", "r-apb0",
	    IRRX_BGR_REG, 0),
	SUNXI_CCU_GATE(D1_R_CLK_BUS_RTC, "bus-r-rtc", "r-ahb",
	    RTC_BGR_REG, 0),
	SUNXI_CCU_GATE(D1_R_CLK_BUS_CPUCFG, "bus-r-cpucfg", "r-apb0",
	    CPUCFG_BGR_REG, 0),
};

static int
sun20i_d1_r_ccu_match(device_t parent, cfdata_t cf, void *aux)
{
	struct fdt_attach_args * const faa = aux;

	return of_compatible_match(faa->faa_phandle, compat_data);
}

static void
sun20i_d1_r_ccu_attach(device_t parent, device_t self, void *aux)
{
	struct sunxi_ccu_softc * const sc = device_private(self);
	struct fdt_attach_args * const faa = aux;

	sc->sc_dev = self;
	sc->sc_phandle = faa->faa_phandle;
	sc->sc_bst = faa->faa_bst;

	sc->sc_resets = sun20i_d1_r_ccu_resets;
	sc->sc_nresets = __arraycount(sun20i_d1_r_ccu_resets);

	sc->sc_clks = sun20i_d1_r_ccu_clks;
	sc->sc_nclks = __arraycount(sun20i_d1_r_ccu_clks);

	if (sunxi_ccu_attach(sc) != 0)
		return;

	aprint_naive("\n");
	aprint_normal(": D1 PRCM CCU\n");

	sunxi_ccu_print(sc);
}
