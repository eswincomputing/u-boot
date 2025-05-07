// SPDX-License-Identifier: GPL-2.0+
/*
 * Eswin PWM Controller driver for U-Boot
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.
 * Adapted from Linux dwc_pwm.c by xuxiang@eswincomputing.com
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <pwm.h>
#include <linux/io.h>
#include <div64.h>

#define DWC_TIM_LD_CNT(n)       ((n) * 0x14)
#define DWC_TIM_LD_CNT2(n)      (((n) * 4) + 0xb0)
#define DWC_TIM_CTRL(n)         (((n) * 0x14) + 0x08)

#define DWC_TIM_CTRL_EN         BIT(0)
#define DWC_TIM_CTRL_MODE       BIT(1)
#define DWC_TIM_CTRL_MODE_USER  (1 << 1)
#define DWC_TIM_CTRL_PWM        BIT(3)

#define DWC_TIMERS_TOTAL        8
#define DWC_CLK_PERIOD_NS       10

struct eswin_pwm_priv {
	void __iomem* base;
	ulong freq;
};

static int eswin_pwm_set_config(struct udevice* dev, uint channel,
	uint period_ns, uint duty_ns)
{
	struct eswin_pwm_priv* priv = dev_get_priv(dev);
	u64 tmp;
	u32 high, low, ctrl;

	debug("%s: Configuring PWM channel %u: period=%u ns, duty=%u ns\n",
		__func__, channel, period_ns, duty_ns);

	if (channel >= DWC_TIMERS_TOTAL) {
		printf("%s: Invalid channel %u (max: %d)\n",
			__func__, channel, DWC_TIMERS_TOTAL - 1);
		return -EINVAL;
	}

	if (duty_ns >= period_ns) {
		printf("%s: Duty (%u ns) >= Period (%u ns), adjusting duty\n",
			__func__, duty_ns, period_ns);
		duty_ns = period_ns - DWC_CLK_PERIOD_NS;
	}

	if (duty_ns == 0) {
		printf("%s: Duty is 0, setting to 50%% of period (%u ns)\n",
			__func__, period_ns / 2);
		duty_ns = period_ns / 2;
	}

	tmp = DIV_ROUND_CLOSEST_ULL((u64)duty_ns, DWC_CLK_PERIOD_NS);
	if (tmp < 1 || tmp >(1ULL << 32)) {
		printf("%s: Duty cycle out of range: %llu cycles (min: 1, max: %llu)\n",
			__func__, tmp, (1ULL << 32));
		return -ERANGE;
	}
	high = tmp - 1;
	debug("%s: High count = %u\n", __func__, high);

	tmp = DIV_ROUND_CLOSEST_ULL((u64)(period_ns - duty_ns), DWC_CLK_PERIOD_NS);
	if (tmp < 1 || tmp >(1ULL << 32)) {
		printf("%s: Low period out of range: %llu cycles (min: 1, max: %llu)\n",
			__func__, tmp, (1ULL << 32));
		return -ERANGE;
	}
	low = tmp - 1;
	debug("%s: Low count = %u\n", __func__, low);

	ctrl = readl(priv->base + DWC_TIM_CTRL(channel));
	debug("%s: Current CTRL reg = 0x%08x\n", __func__, ctrl);
	ctrl &= ~DWC_TIM_CTRL_EN;
	writel(ctrl, priv->base + DWC_TIM_CTRL(channel));
	debug("%s: Disabled PWM, new CTRL reg = 0x%08x\n", __func__, ctrl);

	writel(low, priv->base + DWC_TIM_LD_CNT(channel));
	writel(high, priv->base + DWC_TIM_LD_CNT2(channel));
	debug("%s: Set LD_CNT(%u) = %u, LD_CNT2(%u) = %u\n",
		__func__, channel, low, channel, high);

	ctrl = DWC_TIM_CTRL_MODE_USER | DWC_TIM_CTRL_PWM;
	writel(ctrl, priv->base + DWC_TIM_CTRL(channel));
	debug("%s: Configured PWM mode, CTRL reg = 0x%08x\n", __func__, ctrl);

	return 0;
}

static int eswin_pwm_set_enable(struct udevice* dev, uint channel, bool enable)
{
	struct eswin_pwm_priv* priv = dev_get_priv(dev);
	u32 ctrl;

	debug("%s: %s PWM channel %u\n", __func__,
		enable ? "Enabling" : "Disabling", channel);

	if (channel >= DWC_TIMERS_TOTAL) {
		printf("%s: Invalid channel %u (max: %d)\n",
			__func__, channel, DWC_TIMERS_TOTAL - 1);
		return -EINVAL;
	}

	ctrl = readl(priv->base + DWC_TIM_CTRL(channel));
	debug("%s: Current CTRL reg = 0x%08x\n", __func__, ctrl);

	if (enable)
		ctrl |= DWC_TIM_CTRL_EN;
	else
		ctrl &= ~DWC_TIM_CTRL_EN;
	writel(ctrl, priv->base + DWC_TIM_CTRL(channel));
	debug("%s: Updated CTRL reg = 0x%08x\n", __func__, ctrl);

	return 0;
}

static int eswin_pwm_of_to_plat(struct udevice* dev)
{
	struct eswin_pwm_priv* priv = dev_get_priv(dev);

	priv->base = dev_read_addr_ptr(dev);
	debug("%s: PWM base address = %p\n", __func__, priv->base);
	if (!priv->base) {
		printf("%s: Failed to get base address\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int eswin_pwm_probe(struct udevice* dev)
{
	struct eswin_pwm_priv* priv = dev_get_priv(dev);
	struct clk clk;
	int ret;

	debug("%s: Probing PWM device %s\n", __func__, dev->name);

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret < 0) {
		printf("%s: Failed to get clock: %d\n", __func__, ret);
		return ret;
	}

	priv->freq = clk_get_rate(&clk);
	debug("%s: Clock frequency = %lu Hz (period = %u ns)\n",
		__func__, priv->freq, (uint)(1000000000ULL / priv->freq));

	if (priv->freq == 0) {
		printf("%s: Invalid clock frequency\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static const struct pwm_ops eswin_pwm_ops = {
	.set_config = eswin_pwm_set_config,
	.set_enable = eswin_pwm_set_enable,
};

static const struct udevice_id eswin_pwm_ids[] = {
	{.compatible = "eswin,pwm-eswin" },
	{ }
};

U_BOOT_DRIVER(pwm_eswin) = {
	.name = "pwm_eswin",
	.id = UCLASS_PWM,
	.of_match = eswin_pwm_ids,
	.ops = &eswin_pwm_ops,
	.of_to_plat = eswin_pwm_of_to_plat,
	.probe = eswin_pwm_probe,
	.priv_auto = sizeof(struct eswin_pwm_priv),
	.flags = DM_FLAG_PRE_RELOC,
};
