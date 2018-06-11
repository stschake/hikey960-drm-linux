/* Copyright (c) 2013-2014, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 */

#ifndef KIRIN_DRM_DPE_UTILS_H
#define KIRIN_DRM_DPE_UTILS_H

#include "kirin_dpe_reg.h"

void set_reg(char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs);

void init_dbuf(struct dss_crtc *acrtc);
void init_dpp(struct dss_crtc *acrtc);
void init_ldi(struct dss_crtc *acrtc);

void enable_ldi(struct dss_crtc *acrtc);
void disable_ldi(struct dss_crtc *acrtc);

void dss_inner_clk_pdp_enable(struct dss_crtc *acrtc);
void dss_inner_clk_common_enable(struct dss_crtc *acrtc);
void dpe_interrupt_clear(struct dss_crtc *acrtc);
void dpe_interrupt_unmask(struct dss_crtc *acrtc);
void dpe_interrupt_mask(struct dss_crtc *acrtc);

int dpe_irq_enable(struct dss_crtc *acrtc);
int dpe_irq_disable(struct dss_crtc *acrtc);

int dpe_init(struct dss_crtc *acrtc);

void hisi_dss_mctl_on(struct dss_hw_ctx *ctx);
void hisi_dss_mctl_mutex_lock(struct dss_hw_ctx *ctx);
void hisi_dss_mctl_mutex_unlock(struct dss_hw_ctx *ctx);
int hisi_dss_ovl_base_config(struct dss_hw_ctx *ctx, u32 xres, u32 yres);

void hisi_fb_pan_display(struct drm_plane *plane);
void hisi_fb_pan_display_disable(struct drm_plane *plane);

u32 dss_get_format(u32 pixel_format);

#endif
