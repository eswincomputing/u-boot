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
 */

#ifndef _ESWIN_DW_HDMI_REG_H_
#define _ESWIN_DW_HDMI_REG_H_

/*
 * Eswin connector callbacks.
 * If you want to know the details, please refer to eswin_connector.h
 */
int eswin_dw_hdmi_init(struct display_state *state);
void eswin_dw_hdmi_deinit(struct display_state *state);
int eswin_dw_hdmi_prepare(struct display_state *state);
int eswin_dw_hdmi_enable(struct display_state *state);
int eswin_dw_hdmi_disable(struct display_state *state);
int eswin_dw_hdmi_get_timing(struct display_state *state);
int eswin_dw_hdmi_detect(struct display_state *state);
int eswin_dw_hdmi_get_edid(struct display_state *state);

enum drm_connector_status
inno_dw_hdmi_phy_read_hpd(struct dw_hdmi *hdmi,
			  void *data);
void inno_dw_hdmi_phy_disable(struct dw_hdmi *dw_hdmi,
			      void *data);
int inno_dw_hdmi_phy_init(struct dw_hdmi *dw_hdmi,
			  void *data);
void inno_dw_hdmi_mode_valid(struct dw_hdmi *hdmi, void *data);

#endif /* _ESWIN_DW_HDMI_REG_H_ */
