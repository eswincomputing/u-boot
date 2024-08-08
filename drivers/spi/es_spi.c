// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN Boot SPI Flash driver
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Huangyifeng <huangifeng@eswincomputing.com>
 */

#define LOG_CATEGORY UCLASS_SPI
#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <errno.h>
#include <fdtdec.h>
#include <log.h>
#include <malloc.h>
#include <reset.h>
#include <spi.h>
#include <spi_flash.h>
#include <spi-mem.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/compat.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <command.h>

/* Register offsets */
#define ES_SPI_CSR_00			0x00	/*WRITE_STATUS_REG_TIME*/
#define ES_SPI_CSR_01			0x04	/*SPI_BUS_MODE*/
#define ES_SPI_CSR_02			0x08	/*ERASE_COUNTER_TAP*/
#define ES_SPI_CSR_03			0x0c	/*DMA_EN_HCLK_STATUS*/
#define ES_SPI_CSR_04			0x10	/*FAST_READ_CONTROL*/
#define ES_SPI_CSR_05			0x14	/*SPI_FLASH_WR_NUM*/
#define ES_SPI_CSR_06			0x18	/*SPI_FLASH_COMMAND*/
#define ES_SPI_CSR_07			0x1c	/*INTERRUPT_CONTROL*/
#define ES_SPI_CSR_08			0x20	/*DMA_REQUEST_TAP*/
#define ES_SPI_CSR_09			0x24	/*SPI_FLASH_WR_ADDRESS*/
#define ES_SPI_CSR_10			0x28	/*PAGE_PROGRAM_TIME*/
#define ES_SPI_CSR_11			0x2c	/*SECTOR_ERASE_TIME*/
#define ES_SPI_CSR_12			0x30	/*SMALL_BLOCK_ERASE_TIME*/
#define ES_SPI_CSR_13			0x34	/*LARGE_BLOCK_ERASE_TIME*/
#define ES_SPI_CSR_14			0x38	/*CHIP_ERASE_TIME*/
#define ES_SPI_CSR_15			0x3c	/*CHIP_DESELECT_TIME*/
#define ES_SPI_CSR_16			0x40	/*POWER_DOWN_TIME*/
#define ES_SPI_DR				0x5c000000ul

#define ES_SYSCSR_SPICLK_CTR			0x104
#define ES_SYSCSR_SPICFGCLK_CTR			0x108
#define ES_SYSCSR_SPIMODECFG			0x340
#define ES_SYSCSR_RSTCTR				0x490

#define ES_CONCSR_SPI_INTSEL			0x3c0

#define SPI_COMMAND_VALID				0x01
#define SPI_COMMAND_MOVE_VALUE			0x00
#define SPI_COMMAND_CODE_FIELD_POSITION 0X06
#define SPI_COMMAND_MOVE_FIELD_POSITION 0X05
#define SPI_COMMAND_TYPE_FIELD_POSITION 0X01

#define SR_BUSY				BIT(0)

/* Bit fields in CTRLR0 */
/*
 * Only present when SSI_MAX_XFER_SIZE=16. This is the default, and the only
 * option before version 3.23a.
 */
#define SPI_INTSEL_MASK			GENMASK(11, 10)
#define INT_ROUTED_U84			0x0
#define INT_ROUTED_LPCPU		0x1
#define INT_ROUTED_SCPU			0x3u

#define RX_TIMEOUT			5000		/* timeout in ms */

#define SPI_COMMAND_INIT_VALUE       0XFFFFC000
#define FLASH_PAGE_SIZE      0x100
typedef enum {
    SPI_FLASH_WR_BYTE  = 1,
    SPI_FLASH_WR_2BYTE = 2,
    SPI_FLASH_WR_WORD  = 4,
} SPI_FLASH_WR_NUM_T;

typedef enum {
    SPI_FAST_READ_DEFAULT = 0,
    SPI_FAST_READ_ENABLE  = 3 /*WHEN SPI QUAD0 OR DUAL MODE*/
} SPI_FAST_READ_CTL_T;


typedef enum { STANDARD_SPI = 0, DUAL_SPI, QUAD_SPI } SPI_BUS_MODE_T;

typedef enum {
    SPIC_CMD_TYPE_SPI_PROGRAM = 0,
    SPIC_CMD_TYPE_WRITE_STATUS_REGISTER,
    SPIC_CMD_TYPE_READ_STATUS_REGISTER,
    SPIC_CMD_TYPE_SECTOR_ERASE,
    SPIC_CMD_TYPE_BLOCK_ERASE_TYPE1,
    SPIC_CMD_TYPE_BLOCK_ERASE_TYPE2,
    SPIC_CMD_TYPE_CHIP_ERASE,
    SPIC_CMD_TYPE_POWER_DOWN,
    SPIC_CMD_TYPE_RELEASE_POWER_DOWM,
    SPIC_CMD_TYPE_ENTER_OR_EXIT_32BIT_MODE,
    SPIC_CMD_TYPE_READ_SECURITY_REG,
    SPIC_CMD_TYPE_ERASE_SECURITY_REG,
    SPIC_CMD_TYPE_WRITE_SECURITY_REG,
    SPIC_CMD_TYPE_READ_DATA,
    SPIC_CMD_TYPE_READ_MANUFACTURED_ID,
    SPIC_CMD_TYPE_READ_JEDEC_ID
} SPI_FLASH_COMMAND_TYPE_T;

/* Flash opcodes. */
#define SPINOR_OP_WREN		0x06	/* Write enable */
#define SPINOR_OP_WRDIS		0x04	/* Write disable */
#define SPINOR_OP_RDSR		0x05	/* Read status register */
#define SPINOR_OP_WRSR		0x01	/* Write status register 1 byte */
#define SPINOR_OP_RDSR2		0x35	/* Read status register 2 */
#define SPINOR_OP_WRSR2		0x31	/* Write status register 2 */
#define SPINOR_OP_RDSR3		0x15	/* Read status register 2 */
#define SPINOR_OP_WRSR3		0x11	/* Write status register 2 */

#define SPINOR_OP_READ		0x03	/* Read data bytes (low frequency) */
#define SPINOR_OP_READ_FAST	0x0b	/* Read data bytes (high frequency) */
#define SPINOR_OP_READ_1_1_2	0x3b	/* Read data bytes (Dual Output SPI) */
#define SPINOR_OP_READ_1_2_2	0xbb	/* Read data bytes (Dual I/O SPI) */
#define SPINOR_OP_READ_1_1_4	0x6b	/* Read data bytes (Quad Output SPI) */
#define SPINOR_OP_READ_1_4_4	0xeb	/* Read data bytes (Quad I/O SPI) */
#define SPINOR_OP_READ_1_1_8	0x8b    /* Read data bytes (Octal Output SPI) */
#define SPINOR_OP_READ_1_8_8	0xcb    /* Read data bytes (Octal I/O SPI) */
#define SPINOR_OP_PP		0x02	/* Page program (up to 256 bytes) */
#define SPINOR_OP_PP_1_1_4	0x32	/* Quad page program */
#define SPINOR_OP_PP_1_4_4	0x38	/* Quad page program */
#define SPINOR_OP_PP_1_1_8	0x82    /* Octal page program */
#define SPINOR_OP_PP_1_8_8	0xc2    /* Octal page program */
#define SPINOR_OP_BE_4K		0x20	/* Erase 4KiB block */
#define SPINOR_OP_BE_4K_PMC	0xd7	/* Erase 4KiB block on PMC chips */
#define SPINOR_OP_BE_32K	0x52	/* Erase 32KiB block */
#define SPINOR_OP_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define SPINOR_OP_SE		0xd8	/* Sector erase (usually 64KiB) */
#define SPINOR_OP_RDID		0x9f	/* Read JEDEC ID */
#define SPINOR_OP_RDSFDP	0x5a	/* Read SFDP */
#define SPINOR_OP_RDCR		0x35	/* Read configuration register */
#define SPINOR_OP_RDFSR		0x70	/* Read flag status register */
#define SPINOR_OP_CLFSR		0x50	/* Clear flag status register */
#define SPINOR_OP_RDEAR		0xc8	/* Read Extended Address Register */
#define SPINOR_OP_WREAR		0xc5	/* Write Extended Address Register */

#define SPIC_CMD_CODE_POWER_DOWN              0xb9
#define SPIC_CMD_CODE_RELEASE_POWER_DOWN      0xab
#define SPIC_CMD_CODE_ENABLE_RESET            0x66
#define SPIC_CMD_CODE_RESET                   0x99



struct es_spi_plat {
	s32 frequency;		/* Default clock frequency, -1 for none */
	void __iomem *sys_regs;
	void __iomem *regs;
	s32 write_status_reg_time;
	s32 page_program_time;
	s32 sector_erase_time;
	s32 small_block_erase_time;
	s32 large_block_erase_time;
	s32 chip_erase_time;
};

struct es_spi_priv {
	struct clk clk;
	struct reset_ctl_bulk resets;
	struct gpio_desc *cs_gpio;	/* External chip-select gpio */
	struct gpio_desc *wp_gpio;	/* External wp gpio */

	void __iomem *regs;
	void __iomem *sys_regs;
	void __iomem *flash_base;
	unsigned long bus_clk_rate;
	unsigned int freq;		/* Default frequency */
	unsigned int mode;

	const void *tx;
	u32 opcode;
	u32 cmd_type;
	u64 addr;
	void *rx;
	u32 fifo_len;			/* depth of the FIFO buffer */
	u32 max_xfer;			/* Maximum transfer size (in bits) */

	int bits_per_word;
	int len;
	u8 cs;				/* chip select pin */
	bool wp_enabled;
	u8 tmode;			/* TR/TO/RO/EEPROM */
	u8 type;			/* SPI/SSP/MicroWire */
	s32 write_status_reg_time;
	s32 page_program_time;
	s32 sector_erase_time;
	s32 small_block_erase_time;
	s32 large_block_erase_time;
	s32 chip_erase_time;
};

struct es_spi_priv g_priv;
uint8_t es_read_flash_status_register(struct es_spi_priv *priv, uint8_t *register_data, int flash_cmd);

static inline u32 es_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->regs + offset);
}

static inline void es_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->regs + offset);
}

static inline u32 es_syscrg_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->sys_regs + offset);
}

static inline void es_syscrg_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->sys_regs + offset);
}

static inline void es_concrg_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->sys_regs + offset - 0x18000);
}

static inline u32 es_data_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->flash_base + offset);
}

static inline void es_data_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->flash_base + offset);
}

static int es_spi_dwc_init(struct udevice *bus, struct es_spi_priv *priv)
{
	priv->max_xfer = 32;
	return 0;
}

static int spi_wait_over(struct es_spi_priv *priv)
{
	uint8_t register_data = 0;

	while(es_read(priv, ES_SPI_CSR_06) & 0x1);

	//check flash status register' busy bit to make sure operation is finish.
	while (register_data & 0x1) {
		es_read_flash_status_register(priv, &register_data, SPINOR_OP_RDSR);
	}

	return 0;
}

static int request_gpio_cs(struct udevice *bus)
{

	struct es_spi_priv *priv = dev_get_priv(bus);

	priv->cs_gpio = devm_gpiod_get(bus, "cs", GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (IS_ERR(priv->cs_gpio)) {
		dev_err(bus, "Couldn't request gpio! (error %ld)\n", PTR_ERR(priv->cs_gpio));
		return PTR_ERR(priv->cs_gpio);
	}
	if (dm_gpio_is_valid(priv->cs_gpio)) {
		dm_gpio_set_dir_flags(priv->cs_gpio, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	}

	dev_dbg(bus, "Using external gpio for CS management\n");
	return 0;
}

static int es_spi_of_to_plat(struct udevice *bus)
{
	struct es_spi_plat *plat = bus->plat_;

	plat->regs = dev_read_addr_ptr(bus);
	if (!plat->regs)
		return -EINVAL;

	/* Use 500KHz as a suitable default */
	plat->frequency = dev_read_u32_default(bus, "spi-max-frequency",
					       500000);

	if (dev_read_bool(bus, "spi-slave"))
		return -EINVAL;

	dev_info(bus, "max-frequency=%d\n", plat->frequency);
	plat->sys_regs = (void *)(uintptr_t)dev_read_u32_default(bus, "es,sysscr_reg",
					       0x51828000);
	plat->write_status_reg_time = (uintptr_t)dev_read_u32_default(bus, "write-status-reg-time", 15);
	plat->page_program_time = (uintptr_t)dev_read_u32_default(bus, "page-program-time", 3);
	plat->sector_erase_time = (uintptr_t)dev_read_u32_default(bus, "sector-erase-time", 400);
	plat->small_block_erase_time = (uintptr_t)dev_read_u32_default(bus, "small-block-erase-time", 1600);
	plat->large_block_erase_time = (uintptr_t)dev_read_u32_default(bus, "large-block-erase-time", 2000);
	plat->chip_erase_time = (uintptr_t)dev_read_u32_default(bus, "chip-erase-time", 200);

	dev_dbg(bus, "write_status_reg_time %d, page_program_time %d "
			"sector_erase_time %d, small_block_erase_time %d "
			"large_block_erase_time %d, chip_erase_time %d\n",
			plat->write_status_reg_time, plat->page_program_time,
			plat->sector_erase_time, plat->small_block_erase_time,
			plat->large_block_erase_time, plat->chip_erase_time);

	return request_gpio_cs(bus);
}

/* Restart the controller, disable all interrupts, clean rx fifo */
static void spi_hw_init(struct udevice *bus, struct es_spi_priv *priv)
{
	// Convert times from ms to cycles
	unsigned long freq = priv->bus_clk_rate;

	uint32_t write_status_reg_time_cycles = priv->write_status_reg_time * (freq / 1000);
	uint32_t page_program_time_cycles = priv->page_program_time * (freq / 1000);
	uint32_t sector_erase_time_cycles = priv->sector_erase_time * (freq / 1000);
	uint32_t small_block_erase_time_cycles = priv->small_block_erase_time * (freq / 1000);
	uint32_t large_block_erase_time_cycles = priv->large_block_erase_time * (freq / 1000);
	uint32_t chip_erase_time_cycles = priv->chip_erase_time * (freq / 1000);

	es_write(priv, ES_SPI_CSR_00, write_status_reg_time_cycles);
	es_write(priv, ES_SPI_CSR_10, page_program_time_cycles);
	es_write(priv, ES_SPI_CSR_11, sector_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_12, small_block_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_13, large_block_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_14, chip_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_08, 0x0);

	if (!priv->fifo_len) {
		priv->fifo_len = 256;
	}
	dev_dbg(bus, "write_status_reg_time_cycles 0x%x, page_program_time_cycles 0x%x\n"
			"\tsector_erase_time_cycles 0x%x, small_block_erase_time_cycles 0x%x\n"
			"\tlarge_block_erase_time_cycles 0x%x, chip_erase_time_cycles 0x%x\n"
			"\tfreq %ld\n",
			write_status_reg_time_cycles, page_program_time_cycles,
			sector_erase_time_cycles, small_block_erase_time_cycles,
			large_block_erase_time_cycles, chip_erase_time_cycles, freq);
}

/*
 * We define es_spi_get_clk function as 'weak' as some targets
 * (like SOCFPGA_GEN5 and SOCFPGA_ARRIA10) don't use standard clock API
 * and implement es_spi_get_clk their own way in their clock manager.
 */
__weak int es_spi_get_clk(struct udevice *bus, ulong *rate)
{
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	ret = clk_get_by_index(bus, 0, &priv->clk);
	if (ret)
		return ret;

	ret = clk_enable(&priv->clk);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		return ret;
	/*
	ret = clk_set_rate(&priv->clk, 50000000);
	if (ret)
		return ret;
	*/
	*rate = clk_get_rate(&priv->clk);
	if (!*rate)
		goto err_rate;

	dev_dbg(bus, "Got clock via device tree: %lu Hz\n", *rate);

	return 0;

err_rate:
	clk_disable(&priv->clk);
	clk_free(&priv->clk);

	return -EINVAL;
}

static int es_spi_reset(struct udevice *bus)
{
	int ret;
	struct es_spi_priv *priv = dev_get_priv(bus);

	ret = reset_get_bulk(bus, &priv->resets);
	if (ret) {
		/*
		 * Return 0 if error due to !CONFIG_DM_RESET and reset
		 * DT property is not present.
		 */
		if (ret == -ENOENT || ret == -ENOTSUPP)
			return 0;

		dev_warn(bus, "Couldn't find/assert reset device (error %d)\n",
			 ret);
		return ret;
	}

	ret = reset_deassert_bulk(&priv->resets);
	if (ret) {
		reset_release_bulk(&priv->resets);
		dev_err(bus, "Failed to de-assert reset for SPI (error %d)\n",
			ret);
		return ret;
	}

	return 0;
}

typedef int (*es_spi_init_t)(struct udevice *bus, struct es_spi_priv *priv);


static int es_spi_probe(struct udevice *bus)
{
	es_spi_init_t init = (es_spi_init_t)dev_get_driver_data(bus);
	struct es_spi_plat *plat = dev_get_plat(bus);
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	priv->regs = plat->regs;
	priv->sys_regs = plat->sys_regs;
	priv->freq = plat->frequency;
	priv->flash_base = (void *)(uintptr_t)ES_SPI_DR;
	priv->write_status_reg_time = plat->write_status_reg_time;
	priv->page_program_time = plat->page_program_time;
	priv->sector_erase_time = plat->sector_erase_time;
	priv->small_block_erase_time = plat->small_block_erase_time;
	priv->large_block_erase_time = plat->large_block_erase_time;
	priv->chip_erase_time = plat->chip_erase_time ;
	ret = es_spi_get_clk(bus, &priv->bus_clk_rate);
	if (ret)
		return ret;

	ret = es_spi_reset(bus);
	if (ret)
		return ret;

	if (!init)
		return -EINVAL;
	ret = init(bus, priv);
	if (ret)
		return ret;

	dev_dbg(bus, "ssi_max_xfer_size=%u\n", priv->max_xfer);

	/* Currently only bits_per_word == 8 supported */
	priv->bits_per_word = 8;

	priv->tmode = 0; /* Tx & Rx */

	/* Basic HW init */
	spi_hw_init(bus, priv);

	priv->wp_gpio = devm_gpiod_get(bus, "wp", GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (IS_ERR(priv->wp_gpio)) {
		dev_err(bus, "%s %d: couldn't request wp gpio! (error %ld)\n", __func__,__LINE__,
			PTR_ERR(priv->wp_gpio));
		return PTR_ERR(priv->wp_gpio);
	}

	memcpy(&g_priv, priv, sizeof(struct es_spi_priv));
	return 0;
}

/**
 * @brief spi read and write cfg
 */
static void spi_read_write_cfg(struct es_spi_priv *priv, u32 byte, u32 addr)
{
    es_write(priv, ES_SPI_CSR_05, byte);
    es_write(priv, ES_SPI_CSR_09, addr);
    es_write(priv, ES_SPI_CSR_04, SPI_FAST_READ_DEFAULT);
    es_write(priv, ES_SPI_CSR_01, STANDARD_SPI);
}

/**
 *  @brief write data from dest address to flash
 */
static void spi_send_data(struct es_spi_priv *priv, u8 *dest, u32 size)
{
    u32  offset          = 0;
    u8  *write_byte_buff = NULL;
    u32  flash_end_data  = 0;

    while (size >= SPI_FLASH_WR_WORD) {
        es_data_write(priv, offset, *dest++);
        offset = offset + SPI_FLASH_WR_WORD;
        size   = size - SPI_FLASH_WR_WORD;
    }

    if (size != 0) {
        write_byte_buff = dest;
        for (int i = 0; i < size; i++) {
            flash_end_data |= (*write_byte_buff) << (8 * i);
            write_byte_buff++;
        }
        es_data_write(priv, offset, flash_end_data);
    }
}

/**
 *  @brief Read data from flash to dest address
 */
static void spi_recv_data(struct es_spi_priv *priv, u32 *dest, u32 size)
{
    u32  offset         = 0;
    u8  *read_byte_buff = NULL;
    u32  flash_end_data = 0xFFFFFFFF;

    while (size >= SPI_FLASH_WR_WORD) {
        *dest++ = es_data_read(priv, offset);
        offset = offset + SPI_FLASH_WR_WORD;
        size   = size - SPI_FLASH_WR_WORD;
    }
    if (size != 0) {
        read_byte_buff = (u8 *)dest;
        flash_end_data = es_data_read(priv, offset);
        for (int i = 0; i < size; i++) {
            *read_byte_buff = (u8)(flash_end_data >> (8 * i));
            read_byte_buff++;
        }
    }
}


/**
 * @brief spi send command
 */
static void spi_command_cfg(struct es_spi_priv *priv, u32 code, u32 type)
{
    u32 command = es_read(priv, ES_SPI_CSR_06);

	command &= ~((0xFF << 6) | (0x1 << 5) | (0xF << 1) | 0x1);
    // command &= SPI_COMMAND_INIT_VALUE;
    command |= ((code << SPI_COMMAND_CODE_FIELD_POSITION) |
                (SPI_COMMAND_MOVE_VALUE << SPI_COMMAND_MOVE_FIELD_POSITION) |
                (type << SPI_COMMAND_TYPE_FIELD_POSITION) | SPI_COMMAND_VALID);

    es_write(priv, ES_SPI_CSR_06, command);
	// printk("ES_SPI_CSR_06 command %x, read command %x\n",command, es_read(priv, ES_SPI_CSR_06));
}
#define SPI_PAGE_MASK                0XFF
/**
 * @brief  spi write flash
 * @param [in]  offset: address of flash to be write
 * @param [in]  wr_dest: Address of data to be sent
 * @param [in]  size: size of flash to be write
 */
void es_writer(struct es_spi_priv *priv)
{
	u32 write_size = 0, offset, cmd_code;

	offset = priv->addr;
	cmd_code = priv->opcode;
	u32 cmd_type = priv->cmd_type;
	u8 *wr_dest = priv->tx;
	int size = priv->len;
	if (size == 0)
	{
		// if(SPIC_CMD_TYPE_SECTOR_ERASE == cmd_type)
		{
			spi_read_write_cfg(priv, write_size, offset);
			spi_command_cfg(priv, cmd_code, cmd_type);
			spi_wait_over(priv);
		}
	}
	while (size > 0) {
		write_size = size;

		if (write_size > FLASH_PAGE_SIZE) {
			write_size = FLASH_PAGE_SIZE;
		}

		spi_read_write_cfg(priv, write_size, offset);
		spi_send_data(priv, wr_dest, write_size);
		spi_command_cfg(priv, cmd_code, cmd_type);
		spi_wait_over(priv);
		wr_dest += write_size;
		offset += write_size;
		size = size - write_size;
	}
}


static void es_reader(struct es_spi_priv *priv)
{
	int       read_size = 0;
	u32 offset = priv->addr;
	u32 cmd_code = priv->opcode;
	u32 cmd_type = priv->cmd_type;
	u8 *mem_dest = priv->rx;
	int size = priv->len;


	while (size > 0) {
		read_size = size;
		if (read_size > FLASH_PAGE_SIZE) {
			read_size = FLASH_PAGE_SIZE;
		}

		spi_read_write_cfg(priv, read_size, offset);

		spi_command_cfg(priv, cmd_code, cmd_type);
		spi_wait_over(priv);
		spi_recv_data(priv, (u32 *)mem_dest, read_size);
		mem_dest += read_size;
		offset += read_size;
		size = size - read_size;
	}
}

static int poll_transfer(struct es_spi_priv *priv)
{
	es_writer(priv);
	es_reader(priv);

	return 0;
}

/*
 * We define es_external_cs_manage function as 'weak' as some targets
 * (like MSCC Ocelot) don't control the external CS pin using a GPIO
 * controller. These SoCs use specific registers to control by
 * software the SPI pins (and especially the CS).
 */
__weak void es_external_cs_manage(struct es_spi_priv *priv, bool on)
{
	if (!dm_gpio_is_valid(priv->cs_gpio))
		return;

	dm_gpio_set_value(priv->cs_gpio, on ? 1 : 0);
}

static int es_spi_xfer(struct udevice *dev, unsigned int bitlen,
		       const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct es_spi_priv *priv = dev_get_priv(bus);
	const u8 *tx = dout;
	u8 *rx = din;
	int ret = 0;

	/* spi core configured to do 8 bit transfers */
	if (bitlen % 8) {
		dev_err(dev, "Non byte aligned SPI transfer.\n");
		return -1;
	}

	/* Start the transaction if necessary. */
	if (flags & SPI_XFER_BEGIN)
		es_external_cs_manage(priv, false);

	priv->len = bitlen >> 3;

	priv->tx = (void *)tx;
	priv->rx = rx;

	/* wait the spi idle before writing control registers */
	spi_wait_over(priv);

	dev_dbg(dev, "rx=%p tx=%p len=%d [bytes]\n", rx, tx,
		priv->len);

	/* Start transfer in a polling loop */
	ret = poll_transfer(priv);

	/*
	 * Wait for current transmit operation to complete.
	 * Otherwise if some data still exists in Tx FIFO it can be
	 * silently flushed, i.e. dropped on disabling of the controller,
	 * which happens when writing 0 to es_SPI_SSIENR which happens
	 * in the beginning of new transfer.
	 */
	if (spi_wait_over(priv) < 0) {
		ret = -ETIMEDOUT;
	}

	/* Stop the transaction if necessary */
	if (flags & SPI_XFER_END)
		es_external_cs_manage(priv, true);

	return ret;
}

uint8_t es_read_flash_status_register(struct es_spi_priv *priv, uint8_t *register_data, int flash_cmd)
{
	u32 command;

	memset(register_data, 0, sizeof(uint8_t));
	//Flash status register-2 is 1byte
	spi_read_write_cfg(priv, 1, 0);

	//Set SPI_FLASH_COMMAND
	command = es_read(priv, ES_SPI_CSR_06);
	command &= ~((0xFF << 6) | (0x1 << 5) | (0xF << 1) | 0x1);
	command |= ((flash_cmd << SPI_COMMAND_CODE_FIELD_POSITION) |
			(SPI_COMMAND_MOVE_VALUE << SPI_COMMAND_MOVE_FIELD_POSITION) |
			(SPIC_CMD_TYPE_READ_STATUS_REGISTER << SPI_COMMAND_TYPE_FIELD_POSITION) | SPI_COMMAND_VALID);
	es_write(priv, ES_SPI_CSR_06, command);

	mdelay(10);

	//Read back data
	spi_recv_data(priv, (u32 *)register_data, 1);
	//printf("[%s %d]: command 0x%x, status register_data 0x%x\n",__func__,__LINE__, command, *register_data);
	return 0;
}

void es_write_flash_status_register(struct es_spi_priv *priv, uint8_t register_data, int flash_cmd)
{
	u32 command;

	//Flash status register-2 is 1byte
	spi_read_write_cfg(priv, 1, 0);
	spi_send_data(priv, &register_data, 1);

	command = es_read(priv, ES_SPI_CSR_06);
	command &= ~((0xFF << 6) | (0x1 << 5) | (0xF << 1) | 0x1);
	command |= ((flash_cmd << SPI_COMMAND_CODE_FIELD_POSITION) |
			(SPI_COMMAND_MOVE_VALUE << SPI_COMMAND_MOVE_FIELD_POSITION) |
			(SPIC_CMD_TYPE_WRITE_STATUS_REGISTER << SPI_COMMAND_TYPE_FIELD_POSITION) | SPI_COMMAND_VALID);
	es_write(priv, ES_SPI_CSR_06, command);

	//Wait command finish
	spi_wait_over(priv);
	//printf("[%s %d]: command 0x%x, status register_data 0x%x\n",__func__,__LINE__, command, register_data);
}

void es_flash_write_protection_cfg(int enable)
{
	uint8_t register_data, request_register_data;
	struct es_spi_priv *priv = &g_priv;

	es_external_cs_manage(priv, false);

	//Update status register1
	es_read_flash_status_register(priv, &register_data, SPINOR_OP_RDSR);
	/*
	  SRP SEC TB BP2 BP1 BP0 WEL BUSY
	 */
	if (enable) {
		request_register_data = register_data | ((1 << 2) | (1 << 3) | (1 << 4) | (1 << 7));
		priv->wp_enabled = 1;
	} else {
		request_register_data = register_data & (~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 7)));
		priv->wp_enabled = 0;
	}
	if (request_register_data != register_data) {
		es_write_flash_status_register(priv, request_register_data, SPINOR_OP_WRSR);
	}
	//es_read_flash_status_register(priv, &register_data, SPINOR_OP_RDSR);
	es_external_cs_manage(priv, true);
}

/*
	0: disable write_protection
	1: enable write_protection
*/
extern void gpio_force(int num, int inout);
extern void gpio_level_cfg(int index, int level);
void es_bootspi_wp_cfg(int enable)
{
	struct es_spi_priv *priv = &g_priv;

	if (enable) {
		es_flash_write_protection_cfg(enable);
		dm_gpio_set_value(priv->wp_gpio, enable); //gpio output low, enable protection
	} else {
		dm_gpio_set_value(priv->wp_gpio, enable); //gpio output high, disable protection
		es_flash_write_protection_cfg(enable);
	}
	// printf("Bootspi flash write protection %s\n", enable ? "enabled" : "disabled");
}

int es_bootspi_write_protection_init(void)
{
	int ret;

	unsigned int bus = CONFIG_SF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SF_DEFAULT_CS;
	/* In DM mode, defaults speed and mode will be taken from DT */
	struct udevice *new;

	ret = spi_flash_probe_bus_cs(bus, cs, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
			bus, cs, ret);
		return -1;
	}
	dev_get_uclass_priv(new);

	es_bootspi_wp_cfg(1);
	return 0;
}

static int es_spi_exec_op(struct spi_slave *slave, const struct spi_mem_op *op)
{
	bool read = op->data.dir == SPI_MEM_DATA_IN;
	int ret = 0;
	struct udevice *bus = slave->dev->parent;
	struct es_spi_priv *priv = dev_get_priv(bus);


	priv->addr = op->addr.val;
	priv->opcode = op->cmd.opcode;

	// dev_err(bus, "addr=%lx opcode=%lx\n", priv->addr, priv->opcode);
	if( priv->opcode == SPINOR_OP_WREN || priv->opcode == SPINOR_OP_WRDIS)
		return 0;
	switch(priv->opcode) {
		case SPINOR_OP_RDID:
			priv->cmd_type = SPIC_CMD_TYPE_READ_JEDEC_ID;
			break;
		case SPINOR_OP_BE_4K:
		case SPINOR_OP_BE_4K_PMC:
			priv->opcode = SPINOR_OP_BE_4K;
			priv->cmd_type = SPIC_CMD_TYPE_SECTOR_ERASE;
			break;
		case SPINOR_OP_BE_32K:
			priv->cmd_type = SPIC_CMD_TYPE_BLOCK_ERASE_TYPE1;
			break;
		case SPINOR_OP_SE:
			priv->cmd_type = SPIC_CMD_TYPE_BLOCK_ERASE_TYPE2;
			break;
		case SPINOR_OP_CHIP_ERASE:
			priv->cmd_type = SPIC_CMD_TYPE_CHIP_ERASE;
			break;
		case SPINOR_OP_PP:
		case SPINOR_OP_PP_1_1_4:
		case SPINOR_OP_PP_1_4_4:
		case SPINOR_OP_PP_1_1_8:
		case SPINOR_OP_PP_1_8_8:
			priv->opcode = SPINOR_OP_PP;
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
		case SPINOR_OP_READ:
		case SPINOR_OP_READ_FAST:
		case SPINOR_OP_READ_1_1_2:
		case SPINOR_OP_READ_1_2_2:
		case SPINOR_OP_READ_1_1_4:
		case SPINOR_OP_READ_1_4_4:
		case SPINOR_OP_READ_1_1_8:
		case SPINOR_OP_READ_1_8_8:
			priv->opcode = SPINOR_OP_READ;
			priv->cmd_type = SPIC_CMD_TYPE_READ_DATA;
			break;
		case SPINOR_OP_RDSR:
		case SPINOR_OP_RDSR2:
			priv->cmd_type = SPIC_CMD_TYPE_READ_STATUS_REGISTER;
			break;
		case SPINOR_OP_WRSR:
		case SPINOR_OP_WRSR2:
			priv->cmd_type = SPIC_CMD_TYPE_WRITE_STATUS_REGISTER;
			break;
		case SPIC_CMD_CODE_POWER_DOWN:
			priv->cmd_type = SPIC_CMD_TYPE_POWER_DOWN;
			break;
		case SPIC_CMD_CODE_RELEASE_POWER_DOWN:
			priv->cmd_type = SPIC_CMD_TYPE_RELEASE_POWER_DOWM;
			break;
		case SPIC_CMD_CODE_ENABLE_RESET:
		case SPIC_CMD_CODE_RESET:
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
		default:
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
	}

	if (g_priv.wp_enabled) {
		switch(priv->opcode) {
			case SPINOR_OP_BE_4K:
			case SPINOR_OP_BE_4K_PMC:
			case SPINOR_OP_BE_32K:
			case SPINOR_OP_SE:
			case SPINOR_OP_CHIP_ERASE:
			case SPINOR_OP_PP:
			case SPINOR_OP_PP_1_1_4:
			case SPINOR_OP_PP_1_4_4:
			case SPINOR_OP_PP_1_1_8:
			case SPINOR_OP_PP_1_8_8:
				return -EACCES;
		}
	}

	// dev_err(bus, "opcode = %x cmd_type %x\n", priv->opcode, priv->cmd_type);
	es_external_cs_manage(priv, false);

	// dev_err(bus, "read=%lx\n", read);
	if (read) {
		priv->rx = op->data.buf.in;

		priv->len = op->data.nbytes;
		//dev_err(bus, "read len = %u\n", op->data.nbytes);
		es_reader(priv);
	} else {
		priv->tx = op->data.buf.out;
		priv->len = op->data.nbytes;
		//dev_err(bus, "write len = 0x%x  tx_addr %x\n", op->data.nbytes,priv->tx);
		/* Fill up the write fifo before starting the transfer */
		es_writer(priv);

		if (spi_wait_over(priv) < 0) {
			printf("spi_wait_over ETIMEDOUT\n");
			ret = -ETIMEDOUT;
		}
	}

	es_external_cs_manage(priv, true);

	dev_dbg(bus, "%u bytes xfered\n", op->data.nbytes);
	return ret;
}

/* The size of ctrl1 limits data transfers to 64K */
static int es_spi_adjust_op_size(struct spi_slave *slave, struct spi_mem_op *op)
{
	op->data.nbytes = min(op->data.nbytes, (unsigned int)SZ_64K);

	return 0;
}

static const struct spi_controller_mem_ops es_spi_mem_ops = {
	.exec_op = es_spi_exec_op,
	.adjust_op_size = es_spi_adjust_op_size,
};

static int es_spi_set_speed(struct udevice *bus, uint speed)
{
	struct es_spi_plat *plat = dev_get_plat(bus);
	struct es_spi_priv *priv = dev_get_priv(bus);

	if (speed > plat->frequency)
		speed = plat->frequency;

	priv->freq = speed;

	return 0;
}

static int es_spi_set_mode(struct udevice *bus, uint mode)
{
	struct es_spi_priv *priv = dev_get_priv(bus);

	/*
	 * Can't set mode yet. Since this depends on if rx, tx, or
	 * rx & tx is requested. So we have to defer this to the
	 * real transfer function.
	 */
	priv->mode = mode;
	dev_dbg(bus, "mode=%d\n", priv->mode);

	return 0;
}

static int es_spi_remove(struct udevice *bus)
{
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	ret = reset_release_bulk(&priv->resets);
	if (ret)
		return ret;

#if CONFIG_IS_ENABLED(CLK)
	ret = clk_disable(&priv->clk);
	if (ret)
		return ret;
	clk_free(&priv->clk);
#endif
	return 0;
}

static const struct dm_spi_ops es_spi_ops = {
	.xfer		= es_spi_xfer,
	.mem_ops	= &es_spi_mem_ops,
	.set_speed	= es_spi_set_speed,
	.set_mode	= es_spi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id es_spi_ids[] = {
	/* Generic compatible strings */

	{ .compatible = "eswin,es-apb-spi-1.0", .data = (ulong)es_spi_dwc_init },
};

U_BOOT_DRIVER(es_spi) = {
	.name = "es_spi",
	.id = UCLASS_SPI,
	.of_match = es_spi_ids,
	.ops = &es_spi_ops,
	.of_to_plat = es_spi_of_to_plat,
	.plat_auto = sizeof(struct es_spi_plat),
	.priv_auto = sizeof(struct es_spi_priv),
	.probe = es_spi_probe,
	.remove = es_spi_remove,
};

static int do_bootspi_wp_cfg(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	if (argc != 2) {
		printf("Usage: bootspi_wp <0|1>\n");
		return CMD_RET_USAGE;
	}

	int enable = simple_strtoul(argv[1], NULL, 10);

	if (enable != 0 && enable != 1) {
		printf("Invalid argument. Use 0 to disable and 1 to enable write protection.\n");
		return CMD_RET_USAGE;
	}

	es_bootspi_wp_cfg(enable);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	bootspi_wp, 2, 1, do_bootspi_wp_cfg,
	"Enable or disable BootSPI write protection",
	"<0|1> - 0 to disable, 1 to enable write protection"
);

