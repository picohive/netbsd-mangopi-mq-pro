### File list
* /usr/src/sys/arch/arm/sunxi/sun6i_dma.c
* /usr/src/sys/arch/arm/sunxi/sunxi_gpio.c
* /usr/src/sys/arch/arm/sunxi/sunxi_gpio.h
* /usr/src/sys/arch/arm/sunxi/sunxi_sid.c
* /usr/src/sys/arch/arm/sunxi/sunxi_usbphy.c
* /usr/src/sys/arch/riscv/conf/Makefile.riscv.inc
* /usr/src/sys/arch/riscv/conf/SUN20I_D1
* /usr/src/sys/arch/riscv/conf/files.generic64
* /usr/src/sys/arch/riscv/conf/mk.sun20i_d1
* /usr/src/sys/arch/riscv/conf/std.sun20i_d1
* /usr/src/sys/arch/riscv/dev/plic_fdt.c
* /usr/src/sys/arch/riscv/include/bootconfig.h
* /usr/src/sys/arch/riscv/riscv/cpu.c
* /usr/src/sys/arch/riscv/sunxi/files.sunxi
* /usr/src/sys/arch/riscv/sunxi/sun20i_d1_ccu.c
* /usr/src/sys/arch/riscv/sunxi/sun20i_d1_ccu.h
* /usr/src/sys/arch/riscv/sunxi/sun20i_d1_gpio.c
* /usr/src/sys/arch/riscv/sunxi/sunxi_platform.c
* /usr/src/sys/arch/riscv/sunxi/sunxi_platform.h

### Build kernel
./build.sh -U -u -O /usr/builds/obj.riscv64 -T /usr/builds/tool.riscv64 -j2 -m riscv -a riscv64 kernel=SUN20I_D1

### Write image
dd if=riscv64.img of=/dev/rld0d bs=1m conv=sync

### U-Boot
cd /usr/pkgsrc/wip/u-boot-mangopi-mq-pro ; make install
dd if=/usr/pkg/share/u-boot/mangopi-mq-pro/u-boot-sunxi-with-spl.bin of=/dev/rld0d bs=1k seek=8 conv=sync

### TODO
* SD/MMC debug
