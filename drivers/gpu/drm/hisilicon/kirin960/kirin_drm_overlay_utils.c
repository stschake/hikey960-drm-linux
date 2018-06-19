/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_fb_cma_helper.h>

#include "kirin_drm_dpe_utils.h"
#include "kirin_drm_drv.h"

#define ROUND1(x, y)	((x) / (y) + ((x) % (y)  ? 1 : 0))
#define DSS_CHN_MAX_DEFINE (DSS_COPYBIT_MAX)
static int mid_array[DSS_CHN_MAX_DEFINE] = {0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x2, 0x1, 0x3, 0x0};

/*
** dss_chn_idx
** DSS_RCHN_D2 = 0,	DSS_RCHN_D3,	DSS_RCHN_V0,	DSS_RCHN_G0,	DSS_RCHN_V1,
** DSS_RCHN_G1,	DSS_RCHN_D0,	DSS_RCHN_D1,	DSS_WCHN_W0,	DSS_WCHN_W1,
** DSS_RCHN_V2,   DSS_WCHN_W2,
*/
/*lint -e785*/
u32 g_dss_module_base[DSS_CHN_MAX_DEFINE][MODULE_CHN_MAX] = {
	/* D0 */
	{
	MIF_CH0_OFFSET,
	AIF0_CH0_OFFSET,
	AIF1_CH0_OFFSET,
	MCTL_CTL_MUTEX_RCH0,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH0_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH0_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH0_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD0_DBG,
	DSS_RCH_D0_DMA_OFFSET,
	DSS_RCH_D0_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_RCH_D0_CSC_OFFSET,
	},

	/* D1 */
	{
	MIF_CH1_OFFSET,
	AIF0_CH1_OFFSET,
	AIF1_CH1_OFFSET,
	MCTL_CTL_MUTEX_RCH1,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH1_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH1_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH1_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD1_DBG,
	DSS_RCH_D1_DMA_OFFSET,
	DSS_RCH_D1_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_RCH_D1_CSC_OFFSET,
	},

	/* V0 */
	{
	MIF_CH2_OFFSET,
	AIF0_CH2_OFFSET,
	AIF1_CH2_OFFSET,
	MCTL_CTL_MUTEX_RCH2,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH2_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH2_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH2_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD2_DBG,
	DSS_RCH_VG0_DMA_OFFSET,
	DSS_RCH_VG0_DFC_OFFSET,
	DSS_RCH_VG0_SCL_OFFSET,
	DSS_RCH_VG0_SCL_LUT_OFFSET,
	DSS_RCH_VG0_ARSR_OFFSET,
	DSS_RCH_VG0_ARSR_LUT_OFFSET,
	DSS_RCH_VG0_POST_CLIP_OFFSET,
	DSS_RCH_VG0_PCSC_OFFSET,
	DSS_RCH_VG0_CSC_OFFSET,
	},

	/* G0 */
	{
	MIF_CH3_OFFSET,
	AIF0_CH3_OFFSET,
	AIF1_CH3_OFFSET,
	MCTL_CTL_MUTEX_RCH3,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH3_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH3_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH3_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD3_DBG,
	DSS_RCH_G0_DMA_OFFSET,
	DSS_RCH_G0_DFC_OFFSET,
	DSS_RCH_G0_SCL_OFFSET,
	0,
	0,
	0,
	DSS_RCH_G0_POST_CLIP_OFFSET,
	0,
	DSS_RCH_G0_CSC_OFFSET,
	},

	/* V1 */
	{
	MIF_CH4_OFFSET,
	AIF0_CH4_OFFSET,
	AIF1_CH4_OFFSET,
	MCTL_CTL_MUTEX_RCH4,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH4_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH4_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH4_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD4_DBG,
	DSS_RCH_VG1_DMA_OFFSET,
	DSS_RCH_VG1_DFC_OFFSET,
	DSS_RCH_VG1_SCL_OFFSET,
	DSS_RCH_VG1_SCL_LUT_OFFSET,
	0,
	0,
	DSS_RCH_VG1_POST_CLIP_OFFSET,
	0,
	DSS_RCH_VG1_CSC_OFFSET,
	},

	/* G1 */
	{
	MIF_CH5_OFFSET,
	AIF0_CH5_OFFSET,
	AIF1_CH5_OFFSET,
	MCTL_CTL_MUTEX_RCH5,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH5_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH5_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH5_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD5_DBG,
	DSS_RCH_G1_DMA_OFFSET,
	DSS_RCH_G1_DFC_OFFSET,
	DSS_RCH_G1_SCL_OFFSET,
	0,
	0,
	0,
	DSS_RCH_G1_POST_CLIP_OFFSET,
	0,
	DSS_RCH_G1_CSC_OFFSET,
	},

	/* D2 */
	{
	MIF_CH6_OFFSET,
	AIF0_CH6_OFFSET,
	AIF1_CH6_OFFSET,
	MCTL_CTL_MUTEX_RCH6,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH6_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH6_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH6_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD6_DBG,
	DSS_RCH_D2_DMA_OFFSET,
	DSS_RCH_D2_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_RCH_D2_CSC_OFFSET,
	},

	/* D3 */
	{
	MIF_CH7_OFFSET,
	AIF0_CH7_OFFSET,
	AIF1_CH7_OFFSET,
	MCTL_CTL_MUTEX_RCH7,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH7_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH7_OV_OEN,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH7_STARTY,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD7_DBG,
	DSS_RCH_D3_DMA_OFFSET,
	DSS_RCH_D3_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_RCH_D3_CSC_OFFSET,
	},

	/* W0 */
	{
	MIF_CH8_OFFSET,
	AIF0_CH8_OFFSET,
	AIF1_CH8_OFFSET,
	MCTL_CTL_MUTEX_WCH0,
	DSS_MCTRL_SYS_OFFSET + MCTL_WCH0_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_WCH0_OV_IEN,
	0,
	0,
	DSS_WCH0_DMA_OFFSET,
	DSS_WCH0_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_WCH0_CSC_OFFSET,
	},

	/* W1 */
	{
	MIF_CH9_OFFSET,
	AIF0_CH9_OFFSET,
	AIF1_CH9_OFFSET,
	MCTL_CTL_MUTEX_WCH1,
	DSS_MCTRL_SYS_OFFSET + MCTL_WCH1_FLUSH_EN,
	DSS_MCTRL_SYS_OFFSET + MCTL_WCH1_OV_IEN,
	0,
	0,
	DSS_WCH1_DMA_OFFSET,
	DSS_WCH1_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_WCH1_CSC_OFFSET,
	},
	/* V2 */
	{
	MIF_CH10_OFFSET,
	AIF0_CH11_OFFSET,
	AIF1_CH11_OFFSET,
	MCTL_CTL_MUTEX_RCH8,
	DSS_MCTRL_SYS_OFFSET + MCTL_RCH8_FLUSH_EN,
	0,
	0,
	DSS_MCTRL_SYS_OFFSET + MCTL_MOD8_DBG,
	DSS_RCH_VG2_DMA_OFFSET,
	DSS_RCH_VG2_DFC_OFFSET,
	DSS_RCH_VG2_SCL_OFFSET,
	DSS_RCH_VG2_SCL_LUT_OFFSET,
	0,
	0,
	DSS_RCH_VG2_POST_CLIP_OFFSET,
	0,
	DSS_RCH_VG2_CSC_OFFSET,
	},
	/* W2 */
	{
	MIF_CH11_OFFSET,
	AIF0_CH12_OFFSET,
	AIF1_CH12_OFFSET,
	MCTL_CTL_MUTEX_WCH2,
	DSS_MCTRL_SYS_OFFSET + MCTL_WCH2_FLUSH_EN,
	0,
	0,
	0,
	DSS_WCH2_DMA_OFFSET,
	DSS_WCH2_DFC_OFFSET,
	0,
	0,
	0,
	0,
	0,
	0,
	DSS_WCH2_CSC_OFFSET,
	},
};

/*lint +e785*/
u32 g_dss_module_ovl_base[DSS_MCTL_IDX_MAX][MODULE_OVL_MAX] = {
	{DSS_OVL0_OFFSET,
	DSS_MCTRL_CTL0_OFFSET},

	{DSS_OVL1_OFFSET,
	DSS_MCTRL_CTL1_OFFSET},

	{DSS_OVL2_OFFSET,
	DSS_MCTRL_CTL2_OFFSET},

	{DSS_OVL3_OFFSET,
	DSS_MCTRL_CTL3_OFFSET},

	{0,
	DSS_MCTRL_CTL4_OFFSET},

	{0,
	DSS_MCTRL_CTL5_OFFSET},
};

/* number of smrx idx for each channel */
u32 g_dss_chn_sid_num[DSS_CHN_MAX_DEFINE] = {
	4, 1, 4, 4, 4, 4, 1, 1, 3, 3, 3, 2
};

/* start idx of each channel */
/* smrx_idx = g_dss_smmu_smrx_idx[chn_idx] + (0 ~ g_dss_chn_sid_num[chn_idx]) */
u32 g_dss_smmu_smrx_idx[DSS_CHN_MAX_DEFINE] = {
	0, 4, 5, 9, 13, 17, 21, 22, 26, 29, 23, 32
};


static int hisi_pixel_format_hal2dma(int format)
{
	int ret = 0;

	switch (format) {
	case HISI_FB_PIXEL_FORMAT_RGB_565:
	case HISI_FB_PIXEL_FORMAT_BGR_565:
		ret = DMA_PIXEL_FORMAT_RGB_565;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
		ret = DMA_PIXEL_FORMAT_XRGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
		ret = DMA_PIXEL_FORMAT_ARGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
		ret = DMA_PIXEL_FORMAT_XRGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		ret = DMA_PIXEL_FORMAT_ARGB_5551;
		break;

	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
		ret = DMA_PIXEL_FORMAT_XRGB_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
	case HISI_FB_PIXEL_FORMAT_ARGB_8888:
	case HISI_FB_PIXEL_FORMAT_ABGR_8888:
		ret = DMA_PIXEL_FORMAT_ARGB_8888;
		break;

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
	case HISI_FB_PIXEL_FORMAT_YUYV_422_Pkg:
	case HISI_FB_PIXEL_FORMAT_YVYU_422_Pkg:
	case HISI_FB_PIXEL_FORMAT_UYVY_422_Pkg:
	case HISI_FB_PIXEL_FORMAT_VYUY_422_Pkg:
		ret = DMA_PIXEL_FORMAT_YUYV_422_Pkg;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
		ret = DMA_PIXEL_FORMAT_YUV_422_P_HP;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = DMA_PIXEL_FORMAT_YUV_420_P_HP;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = DMA_PIXEL_FORMAT_YUV_422_SP_HP;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		ret = DMA_PIXEL_FORMAT_YUV_420_SP_HP;
		break;

	default:
		DRM_ERROR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_pixel_format_hal2dfc(int format)
{
	int ret = 0;

	switch (format) {
	case HISI_FB_PIXEL_FORMAT_RGB_565:
		ret = DFC_PIXEL_FORMAT_RGB_565;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_4444:
		ret = DFC_PIXEL_FORMAT_XBGR_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_4444:
		ret = DFC_PIXEL_FORMAT_ABGR_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_5551:
		ret = DFC_PIXEL_FORMAT_XBGR_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_5551:
		ret = DFC_PIXEL_FORMAT_ABGR_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBX_8888:
		ret = DFC_PIXEL_FORMAT_XBGR_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_RGBA_8888:
		ret = DFC_PIXEL_FORMAT_ABGR_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_ARGB_8888:
		ret = DFC_PIXEL_FORMAT_ARGB_8888;
		break;

	case HISI_FB_PIXEL_FORMAT_BGR_565:
		ret = DFC_PIXEL_FORMAT_BGR_565;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_4444:
		ret = DFC_PIXEL_FORMAT_XRGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_4444:
		ret = DFC_PIXEL_FORMAT_ARGB_4444;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_5551:
		ret = DFC_PIXEL_FORMAT_XRGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_5551:
		ret = DFC_PIXEL_FORMAT_ARGB_5551;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRX_8888:
		ret = DFC_PIXEL_FORMAT_XRGB_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_BGRA_8888:
		ret = DFC_PIXEL_FORMAT_ARGB_8888;
		break;
	case HISI_FB_PIXEL_FORMAT_ABGR_8888:
		ret = DFC_PIXEL_FORMAT_ABGR_8888;
		break;

	case HISI_FB_PIXEL_FORMAT_YUV_422_I:
	case HISI_FB_PIXEL_FORMAT_YUYV_422_Pkg:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YVYU_422_Pkg:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;
	case HISI_FB_PIXEL_FORMAT_UYVY_422_Pkg:
		ret = DFC_PIXEL_FORMAT_UYVY422;
		break;
	case HISI_FB_PIXEL_FORMAT_VYUY_422_Pkg:
		ret = DFC_PIXEL_FORMAT_VYUY422;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_SP:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_SP:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_SP:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_SP:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;

	case HISI_FB_PIXEL_FORMAT_YCbCr_422_P:
	case HISI_FB_PIXEL_FORMAT_YCbCr_420_P:
		ret = DFC_PIXEL_FORMAT_YUYV422;
		break;
	case HISI_FB_PIXEL_FORMAT_YCrCb_422_P:
	case HISI_FB_PIXEL_FORMAT_YCrCb_420_P:
		ret = DFC_PIXEL_FORMAT_YVYU422;
		break;

	default:
		DRM_ERROR("not support format(%d)!\n", format);
		ret = -1;
		break;
	}

	return ret;
}

static int hisi_dss_aif_ch_config(struct dss_hw_ctx *ctx, int chn_idx)
{
	void __iomem *aif0_ch_base;
	int mid = 0;

	if (!ctx) {
		DRM_ERROR("ctx is NULL!\n");
		return -1;
	}

	mid = mid_array[chn_idx];
	aif0_ch_base = ctx->base + g_dss_module_base[chn_idx][MODULE_AIF0_CHN];

	set_reg(aif0_ch_base, 0x0, 1, 0);
	set_reg(aif0_ch_base, (uint32_t)mid, 4, 4);
	return 0;
}

static int hisi_dss_smmu_config(struct dss_hw_ctx *ctx, int chn_idx)
{
	void __iomem *smmu_base;
	u32 idx = 0, i = 0;

	smmu_base = ctx->base + DSS_SMMU_OFFSET;

	for (i = 0; i < g_dss_chn_sid_num[chn_idx]; i++) {
		idx = g_dss_smmu_smrx_idx[chn_idx] + i;
		set_reg(smmu_base + SMMU_SMRx_NS + idx * 0x4, 1, 32, 0);
	}

	return 0;
}

static int hisi_dss_mif_config(struct dss_hw_ctx *ctx, int chn_idx)
{
	void __iomem *mif_ch_base = ctx->base + g_dss_module_base[chn_idx][MODULE_MIF_CHN];

	set_reg(mif_ch_base + MIF_CTRL1, 0x1, 1, 5);
	return 0;
}

void hisi_dss_mctl_mutex_lock(struct dss_hw_ctx *ctx)
{
	void __iomem *mctl_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_MCTL_BASE];

	set_reg(mctl_base + MCTL_CTL_MUTEX, 0x1, 1, 0);
}

void hisi_dss_mctl_mutex_unlock(struct dss_hw_ctx *ctx)
{
	void __iomem *mctl_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_MCTL_BASE];

	set_reg(mctl_base + MCTL_CTL_MUTEX, 0x0, 1, 0);
}

static int hisi_dss_mctl_ov_config(struct dss_hw_ctx *ctx, int chn_idx)
{
	void __iomem *mctl_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_MCTL_BASE];
	u32 mctl_rch_offset = (uint32_t)(MCTL_CTL_MUTEX_RCH0 + chn_idx * 0x4);

	set_reg(mctl_base + MCTL_CTL_EN, 0x1, 32, 0);
	set_reg(mctl_base + MCTL_CTL_TOP, 0x2, 32, 0); /*auto mode*/
	set_reg(mctl_base + MCTL_CTL_DBG, 0xB13A00, 32, 0);

	set_reg(mctl_base + mctl_rch_offset, 0x1, 32, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_ITF, 0x1, 2, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_DBUF, 0x1, 2, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_OV, 1 << DSS_OVL0, 4, 0);
	return 0;
}

static void hisi_dss_mctl_sys_disable(struct dss_hw_ctx *ctx, int chn_idx, int layer_idx)
{
	void __iomem *mctl_sys_base = ctx->base + DSS_MCTRL_SYS_OFFSET;
	u32 mctl_rch_ov_oen_offset = MCTL_RCH0_OV_OEN + chn_idx * 0x4;
	u32 mctl_rch_flush_en_offset = MCTL_RCH0_FLUSH_EN + chn_idx * 0x4;

	set_reg(mctl_sys_base + MCTL_RCH_OV0_SEL, 0, 4, (layer_idx + 1) * 4);
	set_reg(mctl_sys_base + mctl_rch_ov_oen_offset, 0, 32, 0);
	set_reg(mctl_sys_base + mctl_rch_flush_en_offset, 0, 32, 0);
}

static int hisi_dss_mctl_sys_config(struct dss_hw_ctx *ctx, int chn_idx, int layer_idx)
{
	void __iomem *mctl_sys_base = ctx->base + DSS_MCTRL_SYS_OFFSET;
	u32 mctl_rch_ov_oen_offset = MCTL_RCH0_OV_OEN + chn_idx * 0x4;
	u32 mctl_rch_flush_en_offset = MCTL_RCH0_FLUSH_EN + chn_idx * 0x4;

	set_reg(mctl_sys_base + mctl_rch_ov_oen_offset,
		((1 << (layer_idx + 1)) | (0x100 << DSS_OVL0)), 32, 0);

	set_reg(mctl_sys_base + MCTL_RCH_OV0_SEL, 0x8, 4, 0);
	set_reg(mctl_sys_base + MCTL_RCH_OV0_SEL, chn_idx, 4, (layer_idx + 1) * 4);
	set_reg(mctl_sys_base + MCTL_OV0_FLUSH_EN, 0xd, 4, 0);
	set_reg(mctl_sys_base + mctl_rch_flush_en_offset, 0x1, 32, 0);
	return 0;
}

static int drm_rotation_to_dss_transform(int rotation)
{
	switch (rotation)
	{
		case DRM_MODE_ROTATE_0:
			return DSS_TRANSFORM_NOP;
		case DRM_MODE_ROTATE_180:
			return DSS_TRANSFORM_FLIP_V | DSS_TRANSFORM_FLIP_H;
		case DRM_MODE_REFLECT_X:
			return DSS_TRANSFORM_FLIP_H;
		case DRM_MODE_REFLECT_Y:
			return DSS_TRANSFORM_FLIP_V;
	}

	return -EINVAL;
}

static void swap_left_right(dss_rect_ltrb_t *rect)
{
	u32 temp = rect->left;
	rect->left = rect->right;
	rect->right = temp;
}

static void swap_top_bottom(dss_rect_ltrb_t *rect)
{
	u32 temp = rect->top;
	rect->top = rect->bottom;
	rect->bottom = temp;
}

static void hisi_dss_rdma_disable(struct dss_hw_ctx *ctx, int chn_idx)
{
	void __iomem *rdma_base = ctx->base + g_dss_module_base[chn_idx][MODULE_DMA];

	set_reg(rdma_base + CH_REG_DEFAULT, 0x1, 32, 0);
	set_reg(rdma_base + CH_REG_DEFAULT, 0x0, 32, 0);
}

static int rdma_stretch_ratio(u32 src_h, u32 dst_h)
{
	const u32 rdma_threshold = 2;
	u32 ratio = src_h / dst_h;
	u32 threshold = (src_h + dst_h - 1) / dst_h;
	
	if (threshold <= rdma_threshold)
		ratio = 0;
	return ratio;
}

static int hisi_dss_rdma_config(struct dss_hw_ctx *ctx, const dss_rect_ltrb_t src_rect,
	const dss_rect_ltrb_t dst_rect, dss_rect_ltrb_t *clip_rect, struct drm_framebuffer *fb, u32 hal_format,
	u32 bpp, int chn_idx, dss_rect_t *out_aligned_rect, int rotation)
{
	void __iomem *base = ctx->base + g_dss_module_base[chn_idx][MODULE_DMA];
	struct drm_gem_cma_object *obj = drm_fb_cma_get_gem_obj(fb, 0);
	u32 stride = fb->pitches[0];
	u32 aligned_pixel = DMA_ALIGN_BYTES / bpp;
	u32 v_display = src_rect.bottom - src_rect.top;
	u32 rdma_format = hisi_pixel_format_hal2dma(hal_format);
	u32 stretched_line_num = rdma_stretch_ratio(v_display, dst_rect.bottom - dst_rect.top);
	u32 stretch_size_vrt = 0;
	dss_rect_ltrb_t aligned_rect;
	int rdma_transform;
	u32 display_addr;

	aligned_rect.left = ALIGN_DOWN(src_rect.left, aligned_pixel);
	aligned_rect.right = ALIGN_UP(src_rect.right, aligned_pixel) - 1;
	aligned_rect.top = src_rect.top;
	aligned_rect.bottom = DSS_HEIGHT(src_rect.bottom);

	clip_rect->left = src_rect.left - aligned_rect.left;
	clip_rect->right = aligned_rect.right - DSS_WIDTH(src_rect.right);
	clip_rect->top = src_rect.top - aligned_rect.top;
	clip_rect->bottom = aligned_rect.bottom - DSS_HEIGHT(src_rect.bottom);

	out_aligned_rect->x = out_aligned_rect->y = 0;
	out_aligned_rect->w = aligned_rect.right - aligned_rect.left + 1;
	out_aligned_rect->h = aligned_rect.bottom - aligned_rect.top + 1;

	if (stretched_line_num) {
		stretch_size_vrt = out_aligned_rect->h / stretched_line_num
						   + ((out_aligned_rect->h % stretched_line_num) ? 1 : 0) - 1;
		out_aligned_rect->h = stretch_size_vrt + 1;
	} else {
		stretch_size_vrt = aligned_rect.bottom - aligned_rect.top;
	}

	printk(KERN_INFO "out_alignd_rect: %dx%d\n", out_aligned_rect->w, out_aligned_rect->h);

	rdma_transform = drm_rotation_to_dss_transform(rotation);
	if (rdma_transform < 0) {
		DRM_ERROR("Unsupported rotation: 0x%08x\n", rotation);
		return rdma_transform;
	}
	switch (rotation) {
		case DRM_MODE_ROTATE_0:
			display_addr = (u32)obj->paddr + aligned_rect.top * stride + aligned_rect.left * bpp;
			break;
		case DRM_MODE_ROTATE_180:
			swap_left_right(clip_rect);
			swap_top_bottom(clip_rect);
			display_addr = (u32)obj->paddr + aligned_rect.bottom * stride + aligned_rect.right * bpp;
			break;
		case DRM_MODE_REFLECT_X:
			swap_left_right(clip_rect);
			display_addr = (u32)obj->paddr + aligned_rect.top * stride + aligned_rect.right * bpp;
			break;
		case DRM_MODE_REFLECT_Y:
			swap_top_bottom(clip_rect);
			display_addr = (u32)obj->paddr + aligned_rect.bottom * stride + aligned_rect.left * bpp;
			break;
		default:
			DRM_ERROR("Unsupported rotation: 0x%08x\n", rotation);
			return -EINVAL;
	}

	set_reg(base + DMA_OFT_X0, aligned_rect.left / aligned_pixel, 16, 0);
	set_reg(base + DMA_OFT_X1, aligned_rect.right / aligned_pixel, 16, 0);	
	set_reg(base + DMA_OFT_Y0, aligned_rect.top, 16, 0);
	set_reg(base + DMA_OFT_Y1, aligned_rect.bottom, 16, 0);
	set_reg(base + DMA_STRETCH_SIZE_VRT, stretch_size_vrt | (stretched_line_num << 13), 19, 0);
	set_reg(base + DMA_CTRL, rdma_format, 5, 3);
	set_reg(base + DMA_CTRL, rdma_transform, 3, 9);
	set_reg(base + DMA_CTRL, stretched_line_num ? 1 : 0, 1, 12);
	set_reg(base + DMA_STRETCH_SIZE_VRT, aligned_rect.bottom - aligned_rect.top, 32, 0);
	set_reg(base + DMA_DATA_ADDR0, display_addr, 32, 0);
	set_reg(base + DMA_STRIDE0, stride / DMA_ALIGN_BYTES, 13, 0);
	set_reg(base + DMA_STRETCH_STRIDE0, stretched_line_num * stride / DMA_ALIGN_BYTES, 19, 0);
	set_reg(base + CH_CTL, 0x1, 1, 0);
	return 0;
}

static int hisi_dss_rdfc_config(struct dss_hw_ctx *ctx, dss_rect_t *rect, 
	dss_rect_ltrb_t clip_rect, u32 hal_format, u32 bpp, int chn_idx)
{
	void __iomem *rdfc_base = ctx->base + g_dss_module_base[chn_idx][MODULE_DFC];	
	u32 dfc_pix_in_num = (bpp <= 2) ? 0x1 : 0x0;
	u32 dfc_aligned = (bpp <= 2) ? 4 : 2;
	u32 size_hrz = DSS_WIDTH(rect->w);
	u32 size_vrt = DSS_HEIGHT(rect->h);
	u32 dfc_fmt = hisi_pixel_format_hal2dfc(hal_format);

	set_reg(rdfc_base + DFC_DISP_SIZE, size_vrt | (size_hrz << 16), 29, 0);
	set_reg(rdfc_base + DFC_PIX_IN_NUM, dfc_pix_in_num, 1, 0);
	set_reg(rdfc_base + DFC_DISP_FMT, dfc_fmt, 5, 1);
	set_reg(rdfc_base + DFC_CLIP_CTL_HRZ, clip_rect.right | (clip_rect.left << 16), 32, 0);
	set_reg(rdfc_base + DFC_CLIP_CTL_VRZ, clip_rect.bottom | (clip_rect.top << 16), 32, 0);
	set_reg(rdfc_base + DFC_CTL_CLIP_EN, 0x1, 1, 0);
	set_reg(rdfc_base + DFC_ICG_MODULE, 0x1, 1, 0);

	// adjust rect for the parts we've clipped or we end up scaling
	rect->w -= clip_rect.left + clip_rect.right;
	rect->h -= clip_rect.top + clip_rect.bottom;

	return 0;
}

int hisi_dss_ovl_base_config(struct dss_hw_ctx *ctx, u32 xres, u32 yres)
{
	void __iomem *mctl_sys_base = ctx->base + DSS_MCTRL_SYS_OFFSET;
	void __iomem *mctl_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_MCTL_BASE];
	void __iomem *ovl0_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_OVL_BASE];

	set_reg(ovl0_base + OVL6_REG_DEFAULT, 0x1, 32, 0);
	set_reg(ovl0_base + OVL6_REG_DEFAULT, 0x0, 32, 0);
	set_reg(ovl0_base + OVL_SIZE, (xres - 1) | ((yres - 1) << 16), 32, 0);
	set_reg(ovl0_base + OVL_BG_COLOR, 0xFF000000, 32, 0);
	set_reg(ovl0_base + OVL_DST_STARTPOS, 0x0, 32, 0);
	set_reg(ovl0_base + OVL_DST_ENDPOS, (xres - 1) | ((yres - 1) << 16), 32, 0);
	set_reg(ovl0_base + OVL_GCFG, 0x10001, 32, 0);

	set_reg(mctl_base + MCTL_CTL_MUTEX_ITF, 0x1, 32, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_DBUF, 0x1, 2, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_OV, 1 << DSS_OVL0, 4, 0);

	set_reg(mctl_sys_base + MCTL_RCH_OV0_SEL, 0x8, 4, 0);
	set_reg(mctl_sys_base + MCTL_OV0_FLUSH_EN, 0xd, 4, 0);
	return 0;
}

static void hisi_dss_ovl_disable(struct dss_hw_ctx *ctx, int layer_idx)
{
	void __iomem *ovl0_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_OVL_BASE];

	set_reg(ovl0_base + OVL_LAYER0_CFG, 0, 1, 0);
}

static int hisi_dss_ovl_config(struct dss_hw_ctx *ctx,
	const dss_rect_ltrb_t dst_rect, int layer_idx)
{
	void __iomem *ovl0_base = ctx->base + g_dss_module_ovl_base[DSS_OVL0][MODULE_OVL_BASE];

	set_reg(ovl0_base + OVL_LAYER0_POS, dst_rect.left | (dst_rect.top << 16), 32, 0);
	set_reg(ovl0_base + OVL_LAYER0_SIZE, DSS_WIDTH(dst_rect.right) | (DSS_HEIGHT(dst_rect.bottom) << 16), 32, 0);
	set_reg(ovl0_base + OVL_LAYER0_PSPOS, (dst_rect.top << 16) | dst_rect.left, 32, 0);
	set_reg(ovl0_base + OVL_LAYER0_PEPOS, (DSS_HEIGHT(dst_rect.bottom) << 16) | DSS_WIDTH(dst_rect.right), 32, 0);
	set_reg(ovl0_base + OVL_LAYER0_ALPHA, 0x00ff40ff, 32, 0);
	set_reg(ovl0_base + OVL_LAYER0_CFG, 1, 1, 0);
	return 0;
}

static int hisi_dss_post_clip_config(struct dss_hw_ctx *ctx, int chn_idx, dss_rect_ltrb_t dst_rect)
{
	char __iomem *pclip_base = ctx->base + g_dss_module_base[chn_idx][MODULE_POST_CLIP];

	if (g_dss_module_base[chn_idx][MODULE_POST_CLIP] == 0)
		return 0;
	
	set_reg(pclip_base + POST_CLIP_DISP_SIZE, DSS_HEIGHT(dst_rect.bottom - dst_rect.top) |
		    (DSS_WIDTH(dst_rect.right - dst_rect.left) << 16), 32, 0);
	set_reg(pclip_base + POST_CLIP_CTL_HRZ, 0, 32, 0);
	set_reg(pclip_base + POST_CLIP_CTL_VRZ, 0, 32, 0);
	set_reg(pclip_base + POST_CLIP_EN, 1, 32, 0);
}

static int hisi_dss_scl_config(struct dss_hw_ctx *ctx, int chn_idx, dss_rect_t *aligned_rect,
							   dss_rect_ltrb_t dst_rect)
{
	char __iomem *scl_base = ctx->base + g_dss_module_base[chn_idx][MODULE_SCL];
	u32 dst_height = dst_rect.bottom - dst_rect.top;
	u32 dst_width = dst_rect.right - dst_rect.left;
	u32 h_v_order = (aligned_rect->w > dst_width) ? 0 : 1;
	bool en_hscl = aligned_rect->w != dst_width;
	u32 scf_en_vscl = aligned_rect->h != dst_height ? 1 : 0;
	u32 scf_en_vscl_str = 0;
	u32 h_ratio = ((DSS_WIDTH(aligned_rect->w) * SCF_INC_FACTOR)
				  + SCF_INC_FACTOR/2) / DSS_WIDTH(dst_width);
	u32 v_ratio = ((DSS_HEIGHT(aligned_rect->h) * SCF_INC_FACTOR)
				  + SCF_INC_FACTOR/2) / DSS_HEIGHT(dst_height);

	// TODO: need to atomic check for no scaling without SCL module
	if (g_dss_module_base[chn_idx][MODULE_SCL] == 0)
		return 0;

	// TODO: these all also need to move to atomic check
	if (dst_height > (aligned_rect->h * SCF_UPSCALE_MAX)) {
		printk(KERN_INFO "Upscale height out of range: %d => %d\n", aligned_rect->h, dst_height);
		return -EINVAL;
	}
	if (aligned_rect->h > (dst_height * SCF_DOWNSCALE_MAX)) {
		printk(KERN_INFO "Downscale height out of range: %d => %d\n", dst_height, aligned_rect->h);
		return -EINVAL;
	}
	if (dst_width > (aligned_rect->w * SCF_UPSCALE_MAX)) {
		printk(KERN_INFO "Upscale width out of range: %d => %d\n", aligned_rect->w, dst_width);
		return -EINVAL;
	}
	if (aligned_rect->w > (dst_width * SCF_DOWNSCALE_MAX)) {
		printk(KERN_INFO "Downscale width out of range: %d => %d\n", dst_width, aligned_rect->w);
		return -EINVAL;
	}

	// no scaling needed
	if (!en_hscl && !scf_en_vscl)
	{
		set_reg(scl_base + SCF_EN_MMP, 0, 1, 0);
		return 0;
	}

	if (aligned_rect->h > dst_height)
		scf_en_vscl = 3;

	// TODO: when we have pixel alpha it should be 3
	if (v_ratio >= (2*SCF_INC_FACTOR))
		scf_en_vscl_str = 3 /*1*/;

	printk(KERN_INFO "dst: %dx%d, aligned: %dx%d, h_v_order: %d, en_hscl: %d, scf_en_vscl: %d, h_ratio: %d, v_ratio: %d\n",
		   dst_width, dst_height, aligned_rect->w, aligned_rect->h, h_v_order, en_hscl, scf_en_vscl, h_ratio, v_ratio);

	set_reg(scl_base + SCF_EN_HSCL_STR, 0, 1, 0);
	set_reg(scl_base + SCF_EN_VSCL_STR, scf_en_vscl_str, 32, 0);
	set_reg(scl_base + SCF_H_V_ORDER, h_v_order, 1, 0);
	set_reg(scl_base + SCF_INPUT_WIDTH_HEIGHT, DSS_HEIGHT(aligned_rect->h) |
			(DSS_WIDTH(aligned_rect->w) << 16), 32, 0);
	set_reg(scl_base + SCF_OUTPUT_WIDTH_HEIGHT, DSS_HEIGHT(dst_height) |
			(DSS_WIDTH(dst_width) << 16), 32, 0);
	set_reg(scl_base + SCF_EN_HSCL, en_hscl ? 1 : 0, 1, 0);
	set_reg(scl_base + SCF_EN_VSCL, scf_en_vscl, 2, 0);
	set_reg(scl_base + SCF_ACC_HSCL, 0, 31, 0);
	set_reg(scl_base + SCF_INC_HSCL, h_ratio, 24, 0);
	set_reg(scl_base + SCF_INC_VSCL, v_ratio, 24, 0);
	set_reg(scl_base + SCF_EN_MMP, 1, 1, 0);
	return 0;
}

void hisi_dss_mctl_on(struct dss_hw_ctx *ctx)
{
	char __iomem *mctl_base = ctx->base + g_dss_module_ovl_base[DSS_MCTL0][MODULE_MCTL_BASE];

	set_reg(mctl_base + MCTL_CTL_EN, 0x1, 32, 0);
	set_reg(mctl_base + MCTL_CTL_MUTEX_ITF, 0x1, 32, 0);
	set_reg(mctl_base + MCTL_CTL_DBG, 0xB13A00, 32, 0);
	set_reg(mctl_base + MCTL_CTL_TOP, 0x2, 32, 0);
}

static void hisi_chn_configure(struct dss_hw_ctx *ctx, int chn_idx, struct drm_framebuffer *fb, int crtc_x, int crtc_y, int crtc_w, int crtc_h,
							   int src_x, int src_y, unsigned int src_w, unsigned int src_h, int rotation)
{
	dss_rect_ltrb_t src_rect = { src_x, src_y, src_x + src_w, src_y + src_h };
	dss_rect_ltrb_t dst_rect = { crtc_x, crtc_y, crtc_x + crtc_w, crtc_y + crtc_h };
	dss_rect_ltrb_t clip_rect = { 0, 0, 0, 0 };
	u32 hal_fmt = dss_get_format(fb->format->format);
	u32 bpp = fb->format->cpp[0];
	dss_rect_t aligned_rect;

	hisi_dss_mctl_mutex_lock(ctx);
	hisi_dss_aif_ch_config(ctx, chn_idx);
	hisi_dss_mif_config(ctx, chn_idx);
	hisi_dss_smmu_config(ctx, chn_idx);

	hisi_dss_rdma_config(ctx, src_rect, dst_rect, &clip_rect, fb, hal_fmt, bpp, chn_idx, &aligned_rect, rotation);
	hisi_dss_rdfc_config(ctx, &aligned_rect, clip_rect, hal_fmt, bpp, chn_idx);
	hisi_dss_scl_config(ctx, chn_idx, &aligned_rect, dst_rect);
	hisi_dss_post_clip_config(ctx, chn_idx, dst_rect);
	hisi_dss_ovl_config(ctx, dst_rect, chn_idx);

	hisi_dss_mctl_ov_config(ctx, chn_idx);
	hisi_dss_mctl_sys_config(ctx, chn_idx, 0);
	hisi_dss_mctl_mutex_unlock(ctx);
}

void hisi_fb_pan_display(struct drm_plane *plane)
{
	struct drm_plane_state *state = plane->state;
	struct drm_framebuffer *fb = state->fb;
	struct dss_plane *aplane = to_dss_plane(plane);
	struct dss_crtc *acrtc = aplane->acrtc;

	// ch 3 has the SCL support
	hisi_chn_configure(acrtc->ctx, 3, fb, state->crtc_x, state->crtc_y, state->crtc_w, state->crtc_h,
					   state->src_x >> 16, state->src_y >> 16, state->src_w >> 16, state->src_h >> 16, state->rotation);
}

void hisi_fb_pan_display_disable(struct drm_plane *plane)
{
	struct dss_plane *aplane = to_dss_plane(plane);
	struct dss_crtc *acrtc = aplane->acrtc;
	struct dss_hw_ctx *ctx = acrtc->ctx;
	int chn = aplane->ch;

	// ch 3 has the SCL support
	chn = 3;

	hisi_dss_mctl_mutex_lock(ctx);
	hisi_dss_mctl_sys_disable(ctx, chn, 0);
	hisi_dss_ovl_disable(ctx, chn);
	hisi_dss_rdma_disable(ctx, chn);
	hisi_dss_mctl_mutex_unlock(ctx);
}
