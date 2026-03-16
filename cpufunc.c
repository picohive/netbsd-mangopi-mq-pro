/*	$NetBSD$	*/

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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLinIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>

#include <dev/fdt/fdtvar.h>

#include <riscv/cpufunc.h>

static void
cache_nullop(vaddr_t va, vsize_t sz)
{
}

void (*cpu_dcache_wbinv_range)(vaddr_t, vsize_t) = cache_nullop;
void (*cpu_dcache_inv_range)(vaddr_t, vsize_t) = cache_nullop;
void (*cpu_dcache_wb_range)(vaddr_t, vsize_t) = cache_nullop;

static const struct device_compatible_entry compat_data[] = {
	{ .compat = "thead,c906" },
	DEVICE_COMPAT_EOL
};

static void
thead_dcache_wbinv_range(vaddr_t va, vsize_t sz)
{
	const u_int line_size =  riscv_dcache_align;

	KASSERT(powerof2(line_size));
	KASSERT(sz != 0);

	const vaddr_t sva = rounddown2(va, line_size);
	const vaddr_t eva = roundup2(va + sz, line_size);

	for (vaddr_t addr = sva; addr < eva; addr += line_size) {
		asm volatile(
		    "mv a0, %0\n"
		    ".long 0x0275000b\n"	/* dcache.civa a0 */
		    :
		    : "r"(addr)
		    : "a0","memory");
	}

	asm volatile("fence rw, rw" ::: "memory");
}

static void
thead_dcache_inv_range(vaddr_t va, vsize_t sz)
{
	const u_int line_size =  riscv_dcache_align;

	KASSERT(powerof2(line_size));
	KASSERT(sz != 0);

	const vaddr_t sva = rounddown2(va, line_size);
	const vaddr_t eva = roundup2(va + sz, line_size);

	for (vaddr_t addr = sva; addr < eva; addr += line_size) {
		asm volatile(
		    "mv a0, %0\n"
		    ".long 0x0265000b\n"	/* dcache.iva a0 */
		    :
		    : "r"(addr)
		    : "a0","memory");
	}

	asm volatile("fence rw, rw" ::: "memory");
}

static void
thead_dcache_wb_range(vaddr_t va, vsize_t sz)
{
	const u_int line_size =  riscv_dcache_align;

	KASSERT(powerof2(line_size));
	KASSERT(sz != 0);

	const vaddr_t sva = rounddown2(va, line_size);
	const vaddr_t eva = roundup2(va + sz, line_size);

	for (vaddr_t addr = sva; addr < eva; addr += line_size) {
		asm volatile(
		    "mv a0, %0\n"
		    ".long 0x0245000b\n"	/* dcache.cva a0 */
		    :
		    : "r"(addr)
		    : "a0","memory");
	}

	asm volatile("fence rw, rw" ::: "memory");
}

void
cpufunc_init(device_t self, void *aux)
{
	const struct fdt_attach_args * const faa = aux;
	const int phandle = faa->faa_phandle;
	uint32_t val;

	if (of_compatible_match(phandle, compat_data) == 0)
		return;

	if (of_getprop_uint32(phandle, "d-cache-block-size", &val) < 0) {
		aprint_error(": can't get d-cache-block-size\n");
		return;
	}

	riscv_dcache_align = val;
	riscv_dcache_align_mask = val  - 1;

	aprint_naive("\n");
	aprint_verbose_dev(self, "cpu cache line size %uB\n", val);

	cpu_dcache_wbinv_range = thead_dcache_wbinv_range;
	cpu_dcache_inv_range = thead_dcache_inv_range;
	cpu_dcache_wb_range = thead_dcache_wb_range;

	aprint_normal_dev(self, "T-Head cache ops enabled\n");
}
