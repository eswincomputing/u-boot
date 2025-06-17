# SPDX-License-Identifier: GPL-2.0+

dtb-$(CONFIG_TARGET_ANDES_AE350) += ae350_32.dtb ae350_64.dtb
dtb-$(CONFIG_TARGET_MICROCHIP_ICICLE) += mpfs-icicle-kit.dtb
dtb-$(CONFIG_TARGET_QEMU_VIRT) += qemu-virt32.dtb qemu-virt64.dtb
dtb-$(CONFIG_TARGET_OPENPITON_RISCV64) += openpiton-riscv64.dtb
dtb-$(CONFIG_TARGET_SIFIVE_UNLEASHED) += hifive-unleashed-a00.dtb
dtb-$(CONFIG_TARGET_SIFIVE_UNMATCHED) += hifive-unmatched-a00.dtb
dtb-$(CONFIG_TARGET_SIPEED_MAIX) += k210-maix-bit.dtb
dtb-$(CONFIG_TARGET_STARFIVE_VISIONFIVE2) += jh7110-starfive-visionfive-2.dtb
dtb-$(CONFIG_TARGET_TH1520_LPI4A) += th1520-lichee-pi-4a.dtb
dtb-$(CONFIG_TARGET_ESWIN_EVB_EIC7700) += eic7700-evb.dtb
dtb-$(CONFIG_TARGET_ESWIN_EVB_EIC7700) += eic7700-evb-a2.dtb
dtb-$(CONFIG_TARGET_ESWIN_EVB_EIC7702) += eic7702-evb-a1.dtb
dtb-$(CONFIG_TARGET_HIFIVE_PREMIER_P550) += eic7700-hifive-premier-p550.dtb
dtb-$(CONFIG_TARGET_ESWIN_EIC7700_Z530) += eic7700-z530.dtb
dtb-$(CONFIG_TARGET_ESWIN_EIC7700_D314) += eic7700-d314.dtb
dtb-$(CONFIG_TARGET_ESWIN_EIC7700_SBC) += eic7700-sbc.dtb
dtb-$(CONFIG_TARGET_ESWIN_EIC7700_OSM) += eic7700-osm.dtb
dtb-$(CONFIG_TARGET_FML13V03) += eic7702-deepcomputing-fml13v03.dtb
dtb-$(CONFIG_TARGET_ESWIN_VPU7702_EVB) += vpu7702-evb.dtb
dtb-$(CONFIG_TARGET_ESWIN_VPU7702_PCIE) += vpu7702-pcie.dtb

include $(srctree)/scripts/Makefile.dts

targets += $(dtb-y)

DTC_FLAGS += -R 4 -p 0x1000

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
