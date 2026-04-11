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

#ifndef _RISCV_SUN20I_D1_R_CCU_H
#define _RISCV_SUN20I_D1_R_CCU_H

#define	D1_R_CLK_AHB			0
#define	D1_R_CLK_APB0			1
#define	D1_R_CLK_BUS_TIMER		2
#define	D1_R_CLK_BUS_TWD		3
#define	D1_R_CLK_BUS_PPU		4
#define	D1_R_CLK_IRRX			5
#define	D1_R_CLK_BUS_IRRX		6
#define	D1_R_CLK_BUS_RTC		7
#define	D1_R_CLK_BUS_CPUCFG		8

#define	D1_R_RST_BUS_TIMER		0
#define	D1_R_RST_BUS_TWD		1
#define	D1_R_RST_BUS_PPU		2
#define	D1_R_RST_BUS_IRRX		3
#define	D1_R_RST_BUS_RTC		4
#define	D1_R_RST_BUS_CPUCFG		5

#endif /* _RISCV_SUN20I_D1_R_CCU_H */
