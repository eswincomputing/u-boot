// SPDX-License-Identifier: GPL-2.0
/*
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
 */

#include <common.h>
#include <clk.h>
#include <debug_uart.h>
#include <dm.h>
#include <errno.h>
#include <fdtdec.h>
#include <watchdog.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <serial.h>
#include <linux/err.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * define bit of  LCR register
 */
#define UART_LCR_DLAB          0x80
#define UART_LCR_PARITY_MSK    0x38
#define UART_LCR_PARITY_PEN    0x8
#define UART_LCR_PARITY_EPS    0x10
#define UART_LCR_PARITY_SP     0x20
#define UART_LCR_STOP          0x04
#define UART_LCR_DLS_MSK       0x03
#define UART_LCR_DLS_DW5       0x0
#define UART_LCR_DLS_DW6       0x1
#define UART_LCR_DLS_DW7       0x2
#define UART_LCR_DLS_DW8       0x3

/*
 * define bit of  LSR register
 */
#define UART_LSR_FIFOE      0x80    /* Fifo error */
#define UART_LSR_TEMT       0x40    /* Transmitter empty */
#define UART_LSR_THRE       0x20    /* Transmit-hold-register empty */
#define UART_LSR_BI         0x10    /* Break interrupt indicator */
#define UART_LSR_FE         0x08    /* Frame error indicator */
#define UART_LSR_PE         0x04    /* Parity error indicator */
#define UART_LSR_OE         0x02    /* Overrun error indicator */
#define UART_LSR_DR         0x01    /* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS 0x1E    /* BI, FE, PE, OE bits */

/*
 * struct srbr and sthr register
 */
struct uart_srbr_sthr {
    union {
        u32 srbr0;
        u32 sthr0;
    };
    union {
        u32 srbr1;
        u32 sthr1;
    };
    union {
        u32 srbr2;
        u32 sthr2;
    };
    union {
        u32 srbr3;
        u32 sthr3;
    };
    union {
        u32 srbr4;
        u32 sthr4;
    };
    union {
        u32 srbr5;
        u32 sthr5;
    };
    union {
        u32 srbr6;
        u32 sthr6;
    };
    union {
        u32 srbr7;
        u32 sthr7;
    };
    union {
        u32 srbr8;
        u32 sthr8;
    };
    union {
        u32 srbr9;
        u32 sthr9;
    };
    union {
        u32 srbr10;
        u32 sthr10;
    };
    union {
        u32 srbr11;
        u32 sthr11;
    };
    union {
        u32 srbr12;
        u32 sthr12;
    };
    union {
        u32 srbr13;
        u32 sthr13;
    };
    union {
        u32 srbr14;
        u32 sthr14;
    };
    union {
        u32 srbr15;
        u32 sthr15;
    };
};

/*
 * struct of register
 */
struct uart_synopsys {
    union {
        u32 rbr;
        u32 thr;
        u32 dll;
    };
    union {
        u32 ier;
        u32 dlh;
    };
    union {
        u32 fcr;
        u32 iir;
    };
    u32 lcr;
    u32 mcr;
    u32 lsr;
    u32 msr;
    u32 scr;
    u32 lpdll;
    u32 lpdlh;
    u32 reserved0;
    u32 reserved1;
    u32 far;
    u32 tfr;
    u32 rfw;
    u32 usr;
    u32 tfl;
    u32 rfl;
    u32 srr;
    u32 srts;
    u32 sbcr;
    u32 sdmam;
    u32 sfe;
    u32 srt;
    u32 stet;
    u32 htx;
    u32 dmasa;
    u32 tcr;
    u32 de_en;
    u32 re_en;
    u32 det;
    u32 tat;
    u32 dlf;
};

struct synopsys_uart_platdata {
    unsigned long clock;
    int saved_input_char;
    struct uart_synopsys *regs;
};

static int _synopsys_serial_putc(struct uart_synopsys *regs, const char c)
{
    while ((readl(&regs->lsr) & (UART_LSR_THRE | UART_LSR_TEMT)) != (UART_LSR_THRE | UART_LSR_TEMT))
        ;

    writel(c, &regs->thr);

    return 0;
}

/*
 * Set up the baud rate in gd struct
 */
static int synopsys_serial_setbrg(struct udevice *dev, int baudrate)
{
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);
    u16 bdiv;

    /* if 1, enable fractional baudrate configure */
#if 0
    float f_bdiv;
f_bdiv = (float)(platdata->clock) / (16 * baudrate);
    bdiv = (u32)(f_bdiv);
    /* set dlf to confiture fractional baudrate division,  rounding value */
    writel((u32)((f_bdiv - bdiv) * 16 + 0.5), &platdata->regs->dlf);
#else
    /* clock value from dts node of  serial */
    bdiv = platdata->clock / (16 * baudrate);
#endif

    /* Configure baudrate
     * init baud rate by initialize DLL DLH Register
     * IntegerDivider = ((SysCLK) / (16 * (UART_BaudRate))
     * register DLL DLH can be accessed only when LCR[7](DLAB) is set
     */
    writel(UART_LCR_DLAB, &platdata->regs->lcr);
    writel(bdiv & 0xFF, &platdata->regs->dll);
    writel((bdiv >> 8) & 0xFF, &platdata->regs->dlh);
    writel(0x03, &platdata->regs->lcr);

    return 0;
}

static void _synopsys_serial_init(struct uart_synopsys *regs)
{
    /* enable fifo */
    writel(0x01, &regs->fcr);
    /* Disable interrupts */
    writel(0x00, &regs->ier);
}

static int synopsys_serial_probe(struct udevice *dev)
{
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);

    /* No need to reinitialize the UART after relocation */
    if (gd->flags & GD_FLG_RELOC)
        return 0;

    _synopsys_serial_init(platdata->regs);

    return 0;
}

static int synopsys_serial_getc(struct udevice *dev)
{
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);
    struct uart_synopsys *regs = platdata->regs;
    if (!(readl(&regs->lsr) & UART_LSR_DR))
        return -EAGAIN;

    return readl(&regs->rbr);
}

static int synopsys_serial_putc(struct udevice *dev, const char ch)
{
    int rc;
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);

    while ((rc = _synopsys_serial_putc(platdata->regs, ch)) == -EAGAIN) ;
    return rc;
}

static int synopsys_serial_pending(struct udevice *dev, bool input)
{
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);
    struct uart_synopsys *regs = platdata->regs;

    if (input)
        return (readl(&regs->lsr) & UART_LSR_DR) ? 1 : 0;
    else
        return (readl(&regs->lsr) & UART_LSR_THRE) ? 0 : 1;
}

static int synopsys_serial_of_to_plat(struct udevice *dev)
{
    struct synopsys_uart_platdata *platdata = dev_get_plat(dev);

    platdata->regs = (struct uart_synopsys *)dev_read_addr(dev);
    if (IS_ERR(platdata->regs))
        return PTR_ERR(platdata->regs);

    /* read node property by function ofnode_read_u32_default(dev_ofnode(dev), "clock-frequency", 1);*/
    platdata->clock = dev_read_u32_default(dev, "clock-frequency", 1);
    return 0;
}

static const struct dm_serial_ops synopsys_serial_ops = {
    .putc = synopsys_serial_putc,
    .getc = synopsys_serial_getc,
    .pending = synopsys_serial_pending,
    .setbrg = synopsys_serial_setbrg,
};

static const struct udevice_id synopsys_serial_ids[] = {
    { .compatible = "synopsys,uart0" },
};

U_BOOT_DRIVER(serial_synopsys) = {
    .name   = "serial_synopsys",
    .id = UCLASS_SERIAL,
    .of_match = synopsys_serial_ids,
    .of_to_plat = synopsys_serial_of_to_plat,
    .plat_auto = sizeof(struct synopsys_uart_platdata),
    .probe = synopsys_serial_probe,
    .ops    = &synopsys_serial_ops,
};
