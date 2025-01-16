/* SPDX-License-Identifier: GPL-2.0+ */

#if CONFIG_IS_ENABLED(BOOTFW0_WRITE)
.section .rodata
.global __bootfw_bootfw0_begin
.type   __bootfw_bootfw0_begin, @object

.align 4
__bootfw_bootfw0_begin:
.incbin CONFIG_BOOTFW0_PATH
#endif

#if CONFIG_IS_ENABLED(BOOTFW1_WRITE)
.section .rodata
.global __bootfw_bootfw1_begin
.type   __bootfw_bootfw1_begin, @object

.align 4
__bootfw_bootfw1_begin:
.incbin CONFIG_BOOTFW1_PATH
#endif