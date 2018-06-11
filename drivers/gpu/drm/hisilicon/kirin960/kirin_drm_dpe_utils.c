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
#include <drm/drmP.h>

#include "drm_mipi_dsi.h"
#include "kirin_drm_dpe_utils.h"

extern u32 g_dss_module_ovl_base[DSS_MCTL_IDX_MAX][MODULE_OVL_MAX];

void set_reg(char __iomem *addr, uint32_t val, uint8_t bw, uint8_t bs)
{
	u32 mask = (1UL << bw) - 1UL;
	u32 tmp = 0;

	tmp = inp32(addr);
	tmp &= ~(mask << bs);

	outp32(addr, tmp | ((val & mask) << bs));
}

static void init_ldi_pxl_div(struct dss_crtc *acrtc)
{
	char __iomem *ldi_base = acrtc->ctx->base + DSS_LDI0_OFFSET;

	set_reg(ldi_base + LDI_PXL0_DIV2_GT_EN, PXL0_DIV2_GT_EN_CLOSE, 1, 0);
	set_reg(ldi_base + LDI_PXL0_DIV4_GT_EN, PXL0_DIV4_GT_EN_CLOSE, 1, 0);
	set_reg(ldi_base + LDI_PXL0_GT_EN, 0x1, 1, 0);
	set_reg(ldi_base + LDI_PXL0_DSI_GT_EN, PXL0_DSI_GT_EN_1, 2, 0);
	set_reg(ldi_base + LDI_PXL0_DIVXCFG, PXL0_DIVCFG_0, 3, 0);
}

void init_ldi(struct dss_crtc *acrtc)
{
	struct dss_hw_ctx *ctx = acrtc->ctx;
	char __iomem *ldi_base = ctx->base + DSS_LDI0_OFFSET;
	struct drm_display_mode *mode = &acrtc->base.state->mode;

	dss_rect_t rect = {0, 0, mode->hdisplay, mode->vdisplay};
	u32 hfp, hbp, hsw, vfp, vbp, vsw;

	hfp = mode->hsync_start - mode->hdisplay;
	hbp = mode->htotal - mode->hsync_end;
	hsw = mode->hsync_end - mode->hsync_start;
	vfp = mode->vsync_start - mode->vdisplay;
	vbp = mode->vtotal - mode->vsync_end;
	vsw = mode->vsync_end - mode->vsync_start;

	init_ldi_pxl_div(acrtc);

	outp32(ldi_base + LDI_DPI0_HRZ_CTRL0,
		hfp | ((hbp + DSS_WIDTH(hsw)) << 16));
	outp32(ldi_base + LDI_DPI0_HRZ_CTRL1, 0);
	outp32(ldi_base + LDI_DPI0_HRZ_CTRL2, DSS_WIDTH(rect.w));
	outp32(ldi_base + LDI_VRT_CTRL0, vfp | (vbp << 16));
	outp32(ldi_base + LDI_VRT_CTRL1, DSS_HEIGHT(vsw));
	outp32(ldi_base + LDI_VRT_CTRL2, DSS_HEIGHT(rect.h));
	outp32(ldi_base + LDI_PLR_CTRL, 0);

	/* bpp*/
	set_reg(ldi_base + LDI_CTRL, acrtc->out_format, 2, 3);
	/* bgr*/
	set_reg(ldi_base + LDI_CTRL, acrtc->bgr_fmt, 1, 13);

	/* for ddr pmqos*/
	outp32(ldi_base + LDI_VINACT_MSK_LEN, vfp);

	/*cmd event sel*/
	outp32(ldi_base + LDI_CMD_EVENT_SEL, 0x1);

	/* for 1Hz LCD and mipi command LCD*/
	set_reg(ldi_base + LDI_DSI_CMD_MOD_CTRL, 0x1, 1, 1);

	/*ldi_data_gate(hisifd, true);*/

	/* no colorbar */
	set_reg(ldi_base + LDI_WORK_MODE, 0x1, 1, 0);

	/* ldi disable*/
	set_reg(ldi_base + LDI_CTRL, 0x0, 1, 0);
}

void init_dbuf(struct dss_crtc *acrtc)
{
	struct dss_hw_ctx *ctx = acrtc->ctx;
	struct drm_display_mode *mode = &acrtc->base.state->mode;
	struct drm_display_mode *adj_mode = &acrtc->base.state->adjusted_mode;
	char __iomem *dbuf_base = ctx->base + DSS_DBUF0_OFFSET;

	int sram_valid_num = 0;
	int sram_max_mem_depth = 0;
	int sram_min_support_depth = 0;

	u32 thd_rqos_in = 0;
	u32 thd_rqos_out = 0;
	u32 thd_cg_in = 0;
	u32 thd_cg_out = 0;
	u32 thd_flux_req_aftdfs_in = 0;
	u32 thd_flux_req_aftdfs_out = 0;
	u32 hfp, hbp, hsw, vfp, vbp, vsw;

	int dfs_time = DFS_TIME;
	int dfs_time_min = DFS_TIME_MIN;
	int depth = DBUF0_DEPTH;

	hfp = mode->hsync_start - mode->hdisplay;
	hbp = mode->htotal - mode->hsync_end;
	hsw = mode->hsync_end - mode->hsync_start;
	vfp = mode->vsync_start - mode->vdisplay;
	vbp = mode->vtotal - mode->vsync_end;
	vsw = mode->vsync_end - mode->vsync_start;

	if (mode->hdisplay * mode->vdisplay >= RES_4K_PHONE)
		dfs_time_min = DFS_TIME_MIN_4K;

	/*
	** int K = 0;
	** int Tp = 1000000  / adj_mode->clock;
	** K = (hsw + hbp + mode->hdisplay +
	**	hfp) / mode->hdisplay;
	** thd_cg_out = dfs_time / (Tp * K * 6);
	*/
	thd_cg_out = (dfs_time * adj_mode->clock * 1000UL * mode->hdisplay) /
		(((hsw + hbp + hfp) + mode->hdisplay) * 6 * 1000000UL);

	sram_valid_num = thd_cg_out / depth;
	thd_cg_in = (sram_valid_num + 1) * depth - 1;

	sram_max_mem_depth = (sram_valid_num + 1) * depth;

	thd_rqos_in = thd_cg_out * 85 / 100;
	thd_rqos_out = thd_cg_out;

	sram_min_support_depth = dfs_time_min * mode->hdisplay / (1000000 / 60 / (mode->vdisplay +
		vbp + vfp + vsw) * (DBUF_WIDTH_BIT / 3 / BITS_PER_BYTE));

	/*thd_flux_req_aftdfs_in   =[(sram_valid_num+1)*depth - 50*HSIZE/((1000000/60/(VSIZE+VFP+VBP+VSW))*6)]/3*/
	thd_flux_req_aftdfs_in = (sram_max_mem_depth - sram_min_support_depth) / 3;
	/*thd_flux_req_aftdfs_out  =  2*[(sram_valid_num+1)* depth - 50*HSIZE/((1000000/60/(VSIZE+VFP+VBP+VSW))*6)]/3*/
	thd_flux_req_aftdfs_out = 2 * (sram_max_mem_depth - sram_min_support_depth) / 3;

	outp32(dbuf_base + DBUF_FRM_SIZE, mode->hdisplay * mode->vdisplay);
	outp32(dbuf_base + DBUF_FRM_HSIZE, DSS_WIDTH(mode->hdisplay));
	outp32(dbuf_base + DBUF_SRAM_VALID_NUM, sram_valid_num);

	outp32(dbuf_base + DBUF_THD_RQOS, (thd_rqos_out << 16) | thd_rqos_in);
	outp32(dbuf_base + DBUF_THD_WQOS, 0);
	outp32(dbuf_base + DBUF_THD_CG, (thd_cg_out << 16) | thd_cg_in);
	outp32(dbuf_base + DBUF_THD_OTHER, 0);
	outp32(dbuf_base + DBUF_THD_FLUX_REQ_BEF, (GET_FLUX_REQ_OUT(sram_max_mem_depth) << 16) | GET_FLUX_REQ_IN(sram_max_mem_depth));
	outp32(dbuf_base + DBUF_THD_FLUX_REQ_AFT, (thd_flux_req_aftdfs_out << 16) | thd_flux_req_aftdfs_in);
	outp32(dbuf_base + DBUF_THD_DFS_OK, GET_FLUX_REQ_IN(sram_max_mem_depth));

	outp32(dbuf_base + DBUF_FLUX_REQ_CTRL, 1);
	outp32(dbuf_base + DBUF_DFS_LP_CTRL, 1);
}

void init_dpp(struct dss_crtc *acrtc)
{
	struct dss_hw_ctx *ctx = acrtc->ctx;
	struct drm_display_mode *mode = &acrtc->base.state->mode;
	char __iomem *dpp_base = ctx->base + DSS_DPP_OFFSET;

	outp32(dpp_base + DPP_IMG_SIZE_BEF_SR,
		(DSS_HEIGHT(mode->vdisplay) << 16) | DSS_WIDTH(mode->hdisplay));
	outp32(dpp_base + DPP_IMG_SIZE_AFT_SR,
		(DSS_HEIGHT(mode->vdisplay) << 16) | DSS_WIDTH(mode->hdisplay));
}

void enable_ldi(struct dss_crtc *acrtc)
{
	char __iomem *ldi_base = acrtc->ctx->base + DSS_LDI0_OFFSET;

	set_reg(ldi_base + LDI_CTRL, 0x1, 1, 0);
}

void disable_ldi(struct dss_crtc *acrtc)
{
	char __iomem *ldi_base = acrtc->ctx->base + DSS_LDI0_OFFSET;

	set_reg(ldi_base + LDI_CTRL, 0x0, 1, 0);
}

void dpe_interrupt_clear(struct dss_crtc *acrtc)
{
	char __iomem *dss_base = acrtc->ctx->base;
	const u32 clear = ~0;

	outp32(dss_base + GLB_CPU_PDP_INTS, clear);
	outp32(dss_base + DSS_LDI0_OFFSET + LDI_CPU_ITF_INTS, clear);
	outp32(dss_base + DSS_DPP_OFFSET + DPP_INTS, clear);

	outp32(dss_base + DSS_DBG_OFFSET + DBG_MCTL_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_WCH0_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_WCH1_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH0_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH1_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH2_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH3_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH4_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH5_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH6_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH7_INTS, clear);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_DSS_GLB_INTS, clear);
}

void dpe_interrupt_unmask(struct dss_crtc *acrtc)
{
	char __iomem *dss_base = acrtc->ctx->base;
	u32 unmask = ~0;

	unmask &= ~(BIT_DPP_INTS | BIT_ITF0_INTS | BIT_MMU_IRPT_NS);
	outp32(dss_base + GLB_CPU_PDP_INT_MSK, unmask);

	unmask = ~0;
	unmask &= ~(BIT_VSYNC | BIT_VACTIVE0_END | BIT_LDI_UNFLOW);
	outp32(dss_base + DSS_LDI0_OFFSET + LDI_CPU_ITF_INT_MSK, unmask);
}

void dpe_interrupt_mask(struct dss_crtc *acrtc)
{
	char __iomem *dss_base = acrtc->ctx->base;
	const u32 mask = ~0;

	outp32(dss_base + GLB_CPU_PDP_INT_MSK, mask);
	outp32(dss_base + DSS_LDI0_OFFSET + LDI_CPU_ITF_INT_MSK, mask);
	outp32(dss_base + DSS_DPP_OFFSET + DPP_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_DSS_GLB_INT_MSK, 0);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_MCTL_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_WCH0_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_WCH1_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH0_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH1_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH2_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH3_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH4_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH5_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH6_INT_MSK, mask);
	outp32(dss_base + DSS_DBG_OFFSET + DBG_RCH7_INT_MSK, mask);
}

static void init_qos(struct dss_hw_ctx *ctx)
{
	char __iomem *noc_dss_base;

	if (!ctx) {
		DRM_ERROR("ctx is NULL!\n");
		return;
	}

	noc_dss_base = ctx->noc_dss_base;

	outp32(noc_dss_base + 0xc, 0x2);
	outp32(noc_dss_base + 0x8c, 0x2);
	outp32(noc_dss_base + 0x10c, 0x2);
	outp32(noc_dss_base + 0x18c, 0x2);
}

int dpe_init(struct dss_crtc *acrtc)
{
	struct drm_display_mode *mode = &acrtc->base.state->mode;

	init_dbuf(acrtc);
	init_dpp(acrtc);
	init_ldi(acrtc);

	init_qos(acrtc->ctx);
	hisi_dss_mctl_on(acrtc->ctx);
	hisi_dss_mctl_mutex_lock(acrtc->ctx);
	hisi_dss_ovl_base_config(acrtc->ctx, mode->hdisplay, mode->vdisplay);
	hisi_dss_mctl_mutex_unlock(acrtc->ctx);

	return 0;
}

void dss_inner_clk_pdp_enable(struct dss_crtc *acrtc)
{
	char __iomem *dss_base = acrtc->ctx->base;

	outp32(dss_base + DSS_IFBC_OFFSET + IFBC_MEM_CTRL, 0x00000088);
	outp32(dss_base + DSS_DSC_OFFSET + DSC_MEM_CTRL, 0x00000888);
	outp32(dss_base + DSS_LDI0_OFFSET + LDI_MEM_CTRL, 0x00000008);
	outp32(dss_base + DSS_DBUF0_OFFSET + DBUF_MEM_CTRL, 0x00000008);
	outp32(dss_base + DSS_DPP_DITHER_OFFSET + DITHER_MEM_CTRL, 0x00000008);
}

void dss_inner_clk_common_enable(struct dss_crtc *acrtc)
{
	char __iomem *dss_base = acrtc->ctx->base;

	/*core/axi/mmbuf*/
	outp32(dss_base + DSS_CMDLIST_OFFSET + CMD_MEM_CTRL, 0x00000008);  /*cmd mem*/

	outp32(dss_base + DSS_RCH_VG0_SCL_OFFSET + SCF_COEF_MEM_CTRL, 0x00000088);/*rch_v0 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG0_SCL_OFFSET + SCF_LB_MEM_CTRL, 0x00000008);/*rch_v0 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG0_ARSR_OFFSET + ARSR2P_LB_MEM_CTRL, 0x00000008);/*rch_v0 ,arsr2p mem*/
	outp32(dss_base + DSS_RCH_VG0_DMA_OFFSET + VPP_MEM_CTRL, 0x00000008);/*rch_v0 ,vpp mem*/
	outp32(dss_base + DSS_RCH_VG0_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_v0 ,dma_buf mem*/

	outp32(dss_base + DSS_RCH_VG1_SCL_OFFSET + SCF_COEF_MEM_CTRL, 0x00000088);/*rch_v1 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG1_SCL_OFFSET + SCF_LB_MEM_CTRL, 0x00000008);/*rch_v1 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG1_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_v1 ,dma_buf mem*/

	outp32(dss_base + DSS_RCH_VG2_SCL_OFFSET + SCF_COEF_MEM_CTRL, 0x00000088);/*rch_v2 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG2_SCL_OFFSET + SCF_LB_MEM_CTRL, 0x00000008);/*rch_v2 ,scf mem*/
	outp32(dss_base + DSS_RCH_VG2_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_v2 ,dma_buf mem*/

	outp32(dss_base + DSS_RCH_G0_SCL_OFFSET + SCF_COEF_MEM_CTRL, 0x00000088);/*rch_g0 ,scf mem*/
	outp32(dss_base + DSS_RCH_G0_SCL_OFFSET + SCF_LB_MEM_CTRL, 0x0000008);/*rch_g0 ,scf mem*/
	outp32(dss_base + DSS_RCH_G0_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_g0 ,dma_buf mem*/

	outp32(dss_base + DSS_RCH_G1_SCL_OFFSET + SCF_COEF_MEM_CTRL, 0x00000088);/*rch_g1 ,scf mem*/
	outp32(dss_base + DSS_RCH_G1_SCL_OFFSET + SCF_LB_MEM_CTRL, 0x0000008);/*rch_g1 ,scf mem*/
	outp32(dss_base + DSS_RCH_G1_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_g1 ,dma_buf mem*/

	outp32(dss_base + DSS_RCH_D0_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_d0 ,dma_buf mem*/
	outp32(dss_base + DSS_RCH_D1_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_d1 ,dma_buf mem*/
	outp32(dss_base + DSS_RCH_D2_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_d2 ,dma_buf mem*/
	outp32(dss_base + DSS_RCH_D3_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*rch_d3 ,dma_buf mem*/

	outp32(dss_base + DSS_WCH0_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*wch0 DMA/AFBCE mem*/
	outp32(dss_base + DSS_WCH0_DMA_OFFSET + ROT_MEM_CTRL, 0x00000008);/*wch0 rot mem*/
	outp32(dss_base + DSS_WCH1_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*wch1 DMA/AFBCE mem*/
	outp32(dss_base + DSS_WCH1_DMA_OFFSET + ROT_MEM_CTRL, 0x00000008);/*wch1 rot mem*/
	outp32(dss_base + DSS_WCH2_DMA_OFFSET + DMA_BUF_MEM_CTRL, 0x00000008);/*wch2 DMA/AFBCE mem*/
	outp32(dss_base + DSS_WCH2_DMA_OFFSET + ROT_MEM_CTRL, 0x00000008);/*wch2 rot mem*/
}

int dpe_irq_enable(struct dss_crtc *acrtc)
{
	struct dss_hw_ctx *ctx = acrtc->ctx;

	if (ctx->irq)
		enable_irq(ctx->irq);

	return 0;
}

int dpe_irq_disable(struct dss_crtc *acrtc)
{
	struct dss_hw_ctx *ctx = acrtc->ctx;

	if (ctx->irq)
		disable_irq(ctx->irq);

	/*disable_irq_nosync(ctx->irq);*/

	return 0;
}
