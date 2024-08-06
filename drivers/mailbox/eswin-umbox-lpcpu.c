// SPDX-License-Identifier: GPL-2.0-only
/*
 * Transfer the som uart console through telenet
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *   Authors:
 *    HuJiamiao<hujiamiao@eswincomputing.com>
 *
 */

#include <asm/io.h>
#include <asm/mbox.h>
#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <dm/of.h>
#include <dm/of_addr.h>
#include <eswin/ipc.h>
#include <eswin/eswin-uMsg.h>
#include <eswin/eswin-umbox-func.h>
#include <eswin/eswin-umbox-srvc.h>
#include <eswin/eswin-umbox.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <log.h>
#include <mailbox-uclass.h>
#include <malloc.h>
#include <fs.h>
#include <update_init.h>
#include <linux/math64.h>

#define FW_FILE_IMG   "/lib/firmware/eic7x/lpcpu_fw.bin"
#define BOOT_FILE_IMG   "/lib/firmware/eic7x/lpcpu_boot.bin"
#define LPCPU_FW_LOAD_ADDR  0xDFFF0000
#define LPCPU_BOOT_FW_LOAD_ADDR  0x58800000
#define FW_LOAD_SUCC   0xacce55
#define DDR_BASE			0x80000000
#define SYSTEM_PORT_BASE	0xc000000000
#define SYSTEM_PORT_ADDR(mem_port_addr)		(mem_port_addr - DDR_BASE + SYSTEM_PORT_BASE)

#define MBOX_MSG_LEN 4  // unit 4bytes,total=4*4bytes

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Mailbox Registers */
#define MBOX_OFF(m) (0x40 * (m))
#define MBOX_MODE_REG(m) (MBOX_OFF(m) + 0x0)
#define MBOX_DATA_REG(m) (MBOX_OFF(m) + 0x4)

#define MBOX_STATE_MASK (0xF << 4)
#define MBOX_STATE_IDLE (0x1 << 4)
#define MBOX_STATE_TX (0x2 << 4)
#define MBOX_STATE_RX (0x4 << 4)
#define MBOX_STATE_ACK (0x8 << 4)
#define MBOX_ACK_CONFIG_MASK (0x1 << 0)
#define MBOX_ACK_AUTOMATIC (0x1 << 0)
#define MBOX_ACK_IRQ (0x0 << 0)

#define syscrg_csr_base 0x51828000
#define lpcpu_boot_address      0x314
#define lpcpu_rst_ctrl          0x454
#define lpcpu_coreclk_ctrl      0x114
#define lpcpu_busclk_ctrl       0x118


MSG_RECV_T gSessionMsgRcv_lpcpu;
MSG_SEND_T gSessionMsgSnd_lpcpu;

extern struct device_node* of_parse_phandle(const struct device_node *np, const char *phandle_name, int index);

typedef struct {
    u8 *sysport_addr;
    u8 *addr;
    u32 size;
} MBOX_BUF_T;

MBOX_BUF_T g_shared_buff_lpcpu;

#define FIFO_LEN 2
u32 g_u32_RD_Data_lpcpu[FIFO_LEN * 2];
u32 g_u32_RD_Index_lpcpu = 0;
u32 g_u32_RD_Cnt_lpcpu = 0;

extern int lock_session(struct eswin_umbox_session *session);
extern int unlock_session(struct eswin_umbox_session *session);

#ifndef CONFIG_SIM
static void mbox_set_mode(struct eswin_umbox *umbox, unsigned int slot, u32 val)
{
    u32 mode;

    mode = readl(umbox->tx_base + MBOX_MODE_REG(slot));
    mode = (mode & ~MBOX_ACK_CONFIG_MASK) | val;
    writel(mode, umbox->tx_base + MBOX_MODE_REG(slot));
}

static void mbox_set_state(struct eswin_umbox *umbox, unsigned int slot, u32 val)
{
    u32 status;

    status = readl(umbox->tx_base + MBOX_MODE_REG(slot));
    status = (status & ~MBOX_STATE_MASK) | val;
    writel(status, umbox->tx_base + MBOX_MODE_REG(slot));
}
#endif

extern u32 get_recv_buff_size(IPC_REQ_T* ipc_req);


extern int PackRData(u8 *pData0, u8 *pData1, REGISTER_DATA_T *pRData);

extern int ChkResData(IPC_RES_T *pIpcResData, u8 *pRecBuf, int len0);

extern int ChkReqData(IPC_REQ_T *pIpcReqData, u8 *pRecBuf, int len0);


static int eswin_umbox_rx_callback(struct mbox_chan *chan, void *msg)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    struct eswin_umbox_session *soc_session = umbox->umbox_session;

#ifdef CONFIG_SIM
    //eswin_local_msg_from_user    MsgFromUser;
    MSG_SEND_T *hdr = (MSG_SEND_T *)msg;
    MSG_RECV_T *hdr0 = (MSG_RECV_T *)msg;

    REQ_DATA_DOMAIN_T ReqDataDomain;
    if(soc_session->num != hdr->num)
    {
        printk("eswin_umbox_rx_callback: soc_session->num(%d)!=hdr->num(%d),error!\r\n",
        soc_session->num,hdr->num );
        return 1;
    }
    if(soc_session->curSerType!=hdr->srv_type)
    {
        printk("eswin_umbox_rx_callback: curSerType(%d)!=hdr->srv_type(%d),error!\r\n",
        soc_session->curSerType,hdr->srv_type );
        return 1;
    }

    memset(&ReqDataDomain,0,sizeof(ReqDataDomain));
    printk("eswin_umbox_rx_callback: recv num=%d,serv_type=%d,size=%d.\r\n",
    hdr->num, hdr->srv_type, hdr->size);
    DecodeReqData(hdr->srv_type,hdr->pData,hdr->size,&ReqDataDomain);
    //DbgPrintSerDataU84(hdr->srv_type,&ReqDataDomain);

#else
    // MSG_RECV_T *hdr = (MSG_RECV_T*) msg;

    // if (soc_session->num != hdr->num)
    // {
    //     printk("eswin_umbox_rx_callback: soc_session->num(%d)!=hdr->num(%d),error!\r\n", soc_session->num, hdr->num);
    //     return 1;
    // }
    // if (soc_session->curSerType != hdr->dwServiceID)
    // {
    //     printk("eswin_umbox_rx_callback: curSerType(%d)!=hdr->dwServiceID(%d),error!\r\n", soc_session->curSerType, hdr->dwServiceID);
    //     return 1;
    // }

    // debug("eswin_umbox_rx_callback: recv num=%u,ServiceID=%u,IpcStatus=%u,ServiceStatus=%u,size=%u.\r\n", hdr->num, hdr->dwServiceID, hdr->IpcStatus, hdr->ServiceStatus, hdr->size);

    DbgPrintSerDataE21(msg);

    debug("eswin_umbox_rx_callback: end.\n");
#endif

    return 0;
}
// TODO timeout ......
#define MAX_REPEAT_TIMES 100000
static int eswin_umbox_recv(struct mbox_chan *chan, void *data)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    struct eswin_umbox_session *umbox_session;
    unsigned int state0, i;
    u8 regData0[8];
    u32 msg[MBOX_MSG_LEN];
    int count;
    umbox_session = umbox->umbox_session;

    u32 dwVal;
    unsigned int state;
    unsigned int ack_irq = 0;

    debug("\r\nwin2030_umbox_recv: start...\n");
    while (1) {
        count = 0;
        while (count < MAX_REPEAT_TIMES) {
            state = readl(umbox->rx_base + REG_MB_INT);
            state0 = state;
            if (state0 == E21_IRQ_BIT) {
                break;
            }
            count++;
        }

        if (!state0) {
            printk("win2030_umbox_recv timeout!\r\n");
            return -3;
        }
        i = 0;
        while (1) {
            dwVal = readl(umbox->rx_base + REG_FIFO_STATUS);  // step-7
            dwVal = (dwVal & 0x02);
            if (0 != dwVal)
                break;
            msg[i] = readl(umbox->rx_base + REG_RD_DATA_0);      // step-8
            msg[i + 1] = readl(umbox->rx_base + REG_RD_DATA_1);  // step-8
            i += 2;
            writel(0x00000000, umbox->rx_base + REG_RD_DATA_1);  // step-9
        }
        writel(0, umbox->rx_base + REG_MB_INT);
        if (!dwVal) {
            printk("win2030_umbox_recv read REG_MB_ERR failed.\r\n");
            return IRQ_NONE;
        }

        mdelay(5);
        u32_to_u8(regData0, &msg[0]);
        u32_to_u8(regData0 + 4, &msg[1]);

        printk("eswin_umbox_recv: read REG_RD_DATA_0 return\r\n "
                "msg[0]=0x%x,msg[1]=0x%x.\r\n", msg[0], msg[1]);

        /* clear IRQ source */
        writel(BIT(ack_irq), umbox->rx_base + REG_MB_INT);  // step-11
        writel(0x00000000, umbox->rx_base + REG_WR_LOCK);   // step-12

        mbox_set_state(umbox, 0, MBOX_STATE_IDLE);
        unlock_session(umbox_session);
        debug("eswin_umbox_recv: done.\r\n");
        return 0;
    }
}

static int umbox_send_data(struct eswin_umbox *umbox, void *msg)
{
    u32 uL32, uH32;
    u8_to_u32(&uL32, msg);
    u8_to_u32(&uH32, msg + 4);
    unsigned int slot = 0;
    mbox_set_state(umbox, slot, MBOX_STATE_TX);
    mbox_set_mode(umbox, slot, MBOX_ACK_AUTOMATIC);
    writel(uL32, umbox->tx_base + REG_WR_DATA_0);
    writel(uH32, umbox->tx_base + REG_WR_DATA_1);
    writel(0, umbox->rx_base + REG_MB_INT);
    return 0;
}

static int eswin_umbox_send(struct mbox_chan *chan, const void *data)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    int max_send_size, max_recv_size, ret = 0;
    u8 regData0[8];
    u8 regData1[8];
    REGISTER_DATA_T rData;
    IPC_REQ_T IpcReqData;
    MSG_SEND_T *pSendMsg;

    if (IsChanBusy(umbox, chan)) {
        printk("win2030_umbox_send: IsChanBusy return true, failed.\r\n");
        ret = -EINVAL;
        goto FAILED;
    }

    u8 * testaddr= (u8 *)data;
    printf("testaddr = %p\n", testaddr);
    ret = Fill_RData(&rData, chan, testaddr);
    if (0 != ret) {
        printk("win2030_umbox_send: Fill_RData return %d, failed.\r\n", ret);
        goto FAILED;
    }

    SliceRData(&rData, regData0, regData1);

    ret = umbox_send_data(umbox, regData0);  // step-4
    if (ret < 0) {
        printk("win2030-umbox: in win2030_umbox_send,umbox_send_data "
                "failed.\r\n");
        goto FAILED;
    }
    mb();
    TriggerRemoteIrqLpcpu(umbox);  // step-5

    debug("win2030_umbox_send: done.\n");
    return 0;

FAILED:
    // unlock_session(umbox->umbox_session);
    return ret;
}

static int eswin_umbox_request(struct mbox_chan *chan)
{
    debug("%s(chan=%p)\n", __func__, chan);

    if (chan->id >= ESWIN_MBOX_CHANNELS)
        return -EINVAL;

    return 0;
}

static int eswin_umbox_free(struct mbox_chan *chan)
{
    debug("%s(chan=%p)\n", __func__, chan);

    return 0;
}

static int eswin_umbox_bind(struct udevice *dev)
{
    debug("%s(dev=%p)\n", __func__, dev);

    return 0;
}

static void eswin_lpcpu_coreclk_ctrl(uint8_t divisor)
{
    u32 val = 0;
    // close gate
    writel(0x0, (void __iomem*)(syscrg_csr_base + lpcpu_coreclk_ctrl));
    // set divisor & selected clk source
    val = (divisor & 0xfu) << 4;
    writel(val, (void __iomem*)(syscrg_csr_base + lpcpu_coreclk_ctrl));
    // enable gate
    val |=  0x1u << 31;
    writel(val, (void __iomem*)(syscrg_csr_base + lpcpu_coreclk_ctrl));
}

/*
 * lpcpu bus clock ratio selection. 1 - 1:1 ratio, 0(default) - 2:1 ratio
 */
static void eswin_lpcpu_busclk_ctrl(uint8_t ratio)
{
    u32 val = 0;
    // close gate
    writel(0x0, (void __iomem*)(syscrg_csr_base + lpcpu_busclk_ctrl));
    // set divisor & selected clk source
    val = (ratio & 0x1u) << 16;
    writel(val, (void __iomem*)(syscrg_csr_base + lpcpu_busclk_ctrl));
    // enable gate
    val =  0x1u << 31;
    writel(val, (void __iomem*)(syscrg_csr_base + lpcpu_busclk_ctrl));
}

static void eswin_lpcpu_rst_ctrl(unsigned int val)
{
    writel(val, (void __iomem*)(syscrg_csr_base + lpcpu_rst_ctrl));
}

static int eswin_lpcpu_boot_status(struct eswin_umbox *umbox)
{
    unsigned int state0, i;
    u8 regData0[8];
    u32 msg[MBOX_MSG_LEN];
    int count;

    u32 dwVal;
    unsigned int state;

    count = 0;
    while (count < 500000) {
        state = readl(umbox->rx_base + REG_MB_INT);
        state0 = state;
        if (state0 == E21_IRQ_BIT) {
            break;
        }
        count++;
    }

    if (!state0) {
        printk("win2030_umbox_recv read REG_MB_INT failed.\r\n");
        return -1;
    }
    i = 0;
    while (1) {
        dwVal = readl(umbox->rx_base + REG_FIFO_STATUS);  // step-7
        dwVal = (dwVal & 0x02);
        if (0 != dwVal)
            break;
        msg[i] = readl(umbox->rx_base + REG_RD_DATA_0);      // step-8
        msg[i + 1] = readl(umbox->rx_base + REG_RD_DATA_1);  // step-8
        i += 2;
        writel(0x00000000, umbox->rx_base + REG_RD_DATA_1);  // step-9
    }
    writel(0, umbox->rx_base + REG_MB_INT);
    if (!dwVal) {
        printk("win2030_umbox_recv read REG_MB_ERR failed.\r\n");
        return -1;
    }
    /*Bootrom did not do mb operation before trigger mailbox irq,
        So Here we make a delay to ensure the bootrom memory operation is finished.
    */
    mdelay(5);
    if(msg[0] != FW_LOAD_SUCC)
    {
        printk("Lpcpu boot failed\r\n");
        return -1;
    }

    return 0;
}

static int eswin_umbox_probe(struct udevice *dev)
{
    // printf("Lpcpu driver probe...\n");
    struct eswin_umbox *umbox = dev_get_priv(dev);
    const struct device_node *node;
    fdt_addr_t tx_addr;
    fdt_addr_t rx_addr;
    uint32_t msg;
    const char *dev_part_str;
    const char *filename;
    unsigned long time;
    unsigned long fw_addr;
    uint64_t len_read;
    int32_t ret = 0;

    debug("%s(dev=%p)\n", __func__, dev);
    node = (const struct device_node*) dev->node_.np;

    tx_addr=dev_read_addr_index(dev,0);
    if (tx_addr == FDT_ADDR_T_NONE)
        return -EINVAL;
    umbox->tx_base = (void __iomem*)tx_addr;
    debug("umbox->tx_base=0x%p, from dts.\r\n", umbox->tx_base);

    rx_addr=dev_read_addr_index(dev,1);
    if (rx_addr == FDT_ADDR_T_NONE)
        return -EINVAL;
    umbox->rx_base = (void __iomem*)rx_addr;
    debug("umbox->rx_base=0x%p, from dts.\r\n", umbox->rx_base);

    // lpcpu firmware load
    printf("start lpcpu boot\n");
    dev_part_str = UPDATE_ROOT_DEV_PART;

	if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_EXT)) {
		log_err("Can't set block device for lpcpu\n");
		return -1;
	}

    filename = BOOT_FILE_IMG;
    fw_addr = LPCPU_FW_LOAD_ADDR;
    time = get_timer(0);
	ret = fs_read(filename, fw_addr, 0, 0, &len_read);
	time = get_timer(time);
	if (ret < 0) {
		log_err("Failed to load '%s'\n", filename);
		return -1;
	}

    // printf("Lpcpu boot file read in %lu ms", time);
	// if (time > 0) {
	// 	puts(" (");
	// 	print_size(div_u64(len_read, time) * 1000, "/s");
	// 	puts(")");
	// }
	// puts("\n");

    // lpcpu bringup from ddr
    flush_cache(LPCPU_FW_LOAD_ADDR, len_read);
    eswin_lpcpu_rst_ctrl(0x0);
    eswin_lpcpu_coreclk_ctrl(0x2);
    eswin_lpcpu_busclk_ctrl(0x0);

	writel(LPCPU_FW_LOAD_ADDR, (void __iomem*)(syscrg_csr_base + lpcpu_boot_address));
    eswin_lpcpu_rst_ctrl(0x7);
    mdelay(5);

	if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_EXT)) {
		log_err("Can't set block device for lpcpu\n");
		return -1;
	}
    filename = FW_FILE_IMG;
    fw_addr = LPCPU_BOOT_FW_LOAD_ADDR;
    time = get_timer(0);
	ret = fs_read(filename, fw_addr, 0, 0, &len_read);
	time = get_timer(time);
	if (ret < 0) {
		log_err("Failed to load '%s'\n", filename);
		return -1;
	}

    // printf("Lpcpu firmware read in %lu ms\n", time);

    // lpcpu run from itim
    writel(0x80000000, umbox->tx_base + 0x4);
    writel(0x4, umbox->tx_base + 0x18);
    mdelay(10);
    ret = eswin_lpcpu_boot_status(umbox);
    if(ret < 0)
    {
		return -1;
    }

	printf("Finish lpcpu boot\n");
    return 0;
}

static const struct udevice_id eswin_mbox_lpcpu_ids[] =
{
    { .compatible = "eswin,lpcpu" },
    { }
};

struct mbox_ops eswin_mbox_lpcpu_mbox_ops =
{
    .request = eswin_umbox_request,
    .rfree = eswin_umbox_free,
    .send = eswin_umbox_send,
    .recv = eswin_umbox_recv,
};

U_BOOT_DRIVER(eswin_umbox_lpcpu) =
{
    .name = "eswin_umbox_lpcpu",
    .id = UCLASS_MAILBOX,
    .of_match = eswin_mbox_lpcpu_ids,
    .bind = eswin_umbox_bind,
    .probe = eswin_umbox_probe,
    .priv_auto = sizeof(struct eswin_umbox),
    .ops = &eswin_mbox_lpcpu_mbox_ops,
};
