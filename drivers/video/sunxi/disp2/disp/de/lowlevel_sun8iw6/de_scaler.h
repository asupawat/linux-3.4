
#ifndef __DE_SCALER_H__
#define __DE_SCALER_H__

#include "de_rtmx.h"

#define GSU_PHASE_NUM	16
#define GSU_PHASE_FRAC_BITWIDTH 18
#define GSU_PHASE_FRAC_REG_SHIFT 2
#define GSU_FB_FRAC_BITWIDTH	32

#define VSU_PHASE_NUM			32
#define VSU_PHASE_FRAC_BITWIDTH 19
#define VSU_PHASE_FRAC_REG_SHIFT 1
#define VSU_FB_FRAC_BITWIDTH	32

#define SC_MIN_WIDTH		32
#define SC_MIN_HEIGHT		8

typedef enum{
	VSU_FORMAT_YUV422 				   	= 0x00,
	VSU_FORMAT_YUV420					= 0x01,
	VSU_FORMAT_YUV411					= 0x02,
	VSU_FORMAT_RGB						= 0x03
}vsu_pixel_format;

int de_vsu_init(unsigned int sel, unsigned int reg_base);
int de_vsu_update_regs(unsigned int sel);
int de_vsu_set_reg_base(unsigned int sel, unsigned int chno, void *base);
int de_vsu_enable(unsigned int sel, unsigned int chno, unsigned int en);
int de_vsu_set_para(unsigned int sel, unsigned int chno, unsigned int enable, unsigned char fmt,
					unsigned int in_w, unsigned int in_h,unsigned int out_w, unsigned int out_h,
					scaler_para *ypara,scaler_para *cpara);
int de_vsu_calc_scaler_para(unsigned char fmt, de_rect64 crop, de_rect frame, de_rect *crop_fix,
							scaler_para *ypara,scaler_para *cpara);
int de_vsu_sel_ovl_scaler_para(unsigned char *en, scaler_para *layer_luma_scale_para, scaler_para *layer_chroma_scale_para,
							   scaler_para *ovl_luma_scale_para, scaler_para *ovl_chroma_scale_para);
int de_vsu_recalc_scale_para(int coarse_status, unsigned int vsu_outw, unsigned int vsu_outh,
							 unsigned int vsu_inw, unsigned int vsu_inh, unsigned int vsu_inw_c, unsigned int vsu_inh_c,
							 scaler_para *fix_y_para, scaler_para *fix_c_para);
int de_recalc_ovl_bld_for_scale(unsigned int scaler_en, unsigned char *lay_en, int laynum, scaler_para *step,
						 de_rect *layer, de_rect *bld_rect, unsigned int *ovlw, unsigned int *ovlh,
						 unsigned int gsu_sel, unsigned int scn_w, unsigned scn_h);

int de_gsu_init(unsigned int sel, unsigned int reg_base);
int de_gsu_update_regs(unsigned int sel);
int de_gsu_set_reg_base(unsigned int sel, unsigned int chno, void *base);
int de_gsu_enable(unsigned int sel, unsigned int chno, unsigned int en);
int de_gsu_set_para(unsigned int sel, unsigned int chno, unsigned int enable,
					unsigned int in_w, unsigned int in_h, unsigned int out_w, unsigned int out_h, scaler_para *para);
int de_gsu_calc_scaler_para(de_rect64 crop, de_rect frame, de_rect *crop_fix,scaler_para *para);
int de_calc_ovl_coord(unsigned int frame_coord, unsigned int scale_step, int gsu_sel);
int de_gsu_sel_ovl_scaler_para(unsigned char *en, scaler_para *layer_scale_para, scaler_para *ovl_scale_para);

#endif
