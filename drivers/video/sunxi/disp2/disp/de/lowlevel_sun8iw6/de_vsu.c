#include "de_vsu_type.h"
#include "de_scaler.h"
#include "de_scaler_table.h"

#define VSU_OFST	0x20000	
static volatile __vsu_reg_t *vsu_dev[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_glb_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_out_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_yscale_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_cscale_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_yhcoeff0_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_yhcoeff1_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_yvcoeff_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_chcoeff0_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_chcoeff1_block[DEVICE_NUM][VI_CHN_NUM];
static de_reg_blocks vsu_cvcoeff_block[DEVICE_NUM][VI_CHN_NUM];

int de_vsu_update_regs(unsigned int sel)
{
	int i,chno;

	chno=de_feat_get_num_vi_chns(sel);

	for(i=0;i<chno;i++)
	{
		if(vsu_glb_block[sel][i].dirty == 0x1){
			memcpy((void *)vsu_glb_block[sel][i].off,vsu_glb_block[sel][i].val,vsu_glb_block[sel][i].size);
			vsu_glb_block[sel][i].dirty = 0x0;}
		if(vsu_out_block[sel][i].dirty == 0x1){
			memcpy((void *)vsu_out_block[sel][i].off,vsu_out_block[sel][i].val,vsu_out_block[sel][i].size);
			vsu_out_block[sel][i].dirty = 0x0;}
		if(vsu_yscale_block[sel][i].dirty == 0x1){
			memcpy((void *)vsu_yscale_block[sel][i].off,vsu_yscale_block[sel][i].val,vsu_yscale_block[sel][i].size);
			vsu_yscale_block[sel][i].dirty = 0x0;}
		if(vsu_cscale_block[sel][i].dirty == 0x1){
			memcpy((void *)vsu_cscale_block[sel][i].off,vsu_cscale_block[sel][i].val,vsu_cscale_block[sel][i].size);
			vsu_cscale_block[sel][i].dirty = 0x0;}
		if(vsu_yhcoeff0_block[sel][i].dirty == 0x1){
			memcpy((void *)vsu_yhcoeff0_block[sel][i].off,vsu_yhcoeff0_block[sel][i].val,vsu_yhcoeff0_block[sel][i].size);
			memcpy((void *)vsu_yhcoeff1_block[sel][i].off,vsu_yhcoeff1_block[sel][i].val,vsu_yhcoeff1_block[sel][i].size);
			memcpy((void *)vsu_yvcoeff_block[sel][i].off,vsu_yvcoeff_block[sel][i].val,vsu_yvcoeff_block[sel][i].size);
			memcpy((void *)vsu_chcoeff0_block[sel][i].off,vsu_chcoeff0_block[sel][i].val,vsu_chcoeff0_block[sel][i].size);
			memcpy((void *)vsu_chcoeff1_block[sel][i].off,vsu_chcoeff1_block[sel][i].val,vsu_chcoeff1_block[sel][i].size);
			memcpy((void *)vsu_cvcoeff_block[sel][i].off,vsu_cvcoeff_block[sel][i].val,vsu_cvcoeff_block[sel][i].size);
			vsu_yhcoeff0_block[sel][i].dirty = 0x0;}
	}


	return 0;
}

int de_vsu_init(unsigned int sel, unsigned int reg_base)
{
	int j, chno;
	unsigned int vsu_base;
	void *memory;

	chno = de_feat_get_num_vi_chns(sel);

	for(j=0;j<chno;j++)
	{
		vsu_base = reg_base + (sel+1)*0x00100000 + VSU_OFST + j*0x20000;
		__inf("sel %d, vsu_base[%d]=0x%x\n", sel, j, vsu_base);

		memory = disp_sys_malloc(sizeof(__vsu_reg_t));
		if(NULL == memory) {
			__wrn("malloc video scaler[%d][%d] memory fail! size=0x%x\n", sel, j, sizeof(__vsu_reg_t));
			return -1;
		}

		vsu_glb_block[sel][j].off			= vsu_base;
		vsu_glb_block[sel][j].val			= memory;
		vsu_glb_block[sel][j].size			= 0x10;
		vsu_glb_block[sel][j].dirty 		= 0;

		vsu_out_block[sel][j].off			= vsu_base + 0x40;
		vsu_out_block[sel][j].val			= memory + 0x40;
		vsu_out_block[sel][j].size			= 0x4;
		vsu_out_block[sel][j].dirty 		= 0;

		vsu_yscale_block[sel][j].off		= vsu_base + 0x80;
		vsu_yscale_block[sel][j].val		= memory + 0x80;
		vsu_yscale_block[sel][j].size		= 0x20;
		vsu_yscale_block[sel][j].dirty 		= 0;

		vsu_cscale_block[sel][j].off		= vsu_base + 0xc0;
		vsu_cscale_block[sel][j].val		= memory + 0xc0;
		vsu_cscale_block[sel][j].size		= 0x20;
		vsu_cscale_block[sel][j].dirty 		= 0;

		vsu_yhcoeff0_block[sel][j].off		= vsu_base + 0x200;
		vsu_yhcoeff0_block[sel][j].val		= memory + 0x200;
		vsu_yhcoeff0_block[sel][j].size		= 0x80;

		vsu_yhcoeff1_block[sel][j].off		= vsu_base + 0x300;
		vsu_yhcoeff1_block[sel][j].val		= memory + 0x300;
		vsu_yhcoeff1_block[sel][j].size		= 0x80;

		vsu_yvcoeff_block[sel][j].off		= vsu_base + 0x400;
		vsu_yvcoeff_block[sel][j].val		= memory + 0x400;
		vsu_yvcoeff_block[sel][j].size		= 0x80;

		vsu_chcoeff0_block[sel][j].off		= vsu_base + 0x600;
		vsu_chcoeff0_block[sel][j].val		= memory + 0x600;
		vsu_chcoeff0_block[sel][j].size		= 0x80;

		vsu_chcoeff1_block[sel][j].off		= vsu_base + 0x700;
		vsu_chcoeff1_block[sel][j].val		= memory + 0x700;
		vsu_chcoeff1_block[sel][j].size		= 0x80;

		vsu_cvcoeff_block[sel][j].off		= vsu_base + 0x800;
		vsu_cvcoeff_block[sel][j].val		= memory + 0x800;
		vsu_cvcoeff_block[sel][j].size		= 0x80;

		vsu_yhcoeff0_block[sel][j].dirty 	= 0;

		de_vsu_set_reg_base(sel, j, memory);
	}

	return 0;
}

int de_vsu_set_reg_base(unsigned int sel, unsigned int chno, void *base)
{
	__inf("sel=%d, chno=%d, base=0x%x\n", sel, chno, (u32)base);
	vsu_dev[sel][chno] = (__vsu_reg_t *)base;

	return 0;
}

int de_vsu_enable(unsigned int sel, unsigned int chno, unsigned int en)
{
	vsu_dev[sel][chno]->ctrl.bits.en = en;
	vsu_glb_block[sel][chno].dirty 	 = 1;

	return 0;
}

static unsigned int de_vsu_calc_fir_coef(unsigned int step)
{
	unsigned int pt_coef;
	unsigned int scale_ratio, int_part, float_part, fir_coef_ofst;

	scale_ratio = step>>(VSU_PHASE_FRAC_BITWIDTH-3);
	int_part = scale_ratio>>3;
	float_part = scale_ratio & 0x7;
	fir_coef_ofst = (int_part==0)  ? VSU_ZOOM0_SIZE :
		(int_part==1)  ? VSU_ZOOM0_SIZE + float_part :
		(int_part==2)  ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE + (float_part>>1) :
		(int_part==3)  ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE + VSU_ZOOM2_SIZE :
		(int_part==4)  ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE + VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE :
		VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE + VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE + VSU_ZOOM4_SIZE;

	pt_coef = fir_coef_ofst * VSU_PHASE_NUM ;

	return pt_coef;
}

int de_vsu_set_para(unsigned int sel, unsigned int chno, unsigned int enable, unsigned char fmt,
					unsigned int in_w, unsigned int in_h,unsigned int out_w, unsigned int out_h,
					scaler_para *ypara,scaler_para *cpara)
{
	unsigned int pt_coef,in_cw,in_ch,format;

	__inf("sel%d, ch %d, en=%d, in<%dx%d>,out<%dx%d>\n", sel, chno, enable, in_w, in_h, out_w, out_h);
	vsu_dev[sel][chno]->ctrl.bits.en = enable;
	if (0==enable)
	{
		vsu_glb_block[sel][chno].dirty = 1;
		return 0;
	}

	switch(fmt)
	{
		case DE_FORMAT_YUV422_I_YVYU:
		case DE_FORMAT_YUV422_I_YUYV:
		case DE_FORMAT_YUV422_I_UYVY:
		case DE_FORMAT_YUV422_I_VYUY:
		case DE_FORMAT_YUV422_P:
		case DE_FORMAT_YUV422_SP_UVUV:
		case DE_FORMAT_YUV422_SP_VUVU:
		{
			in_cw =(in_w+1)>>1;
			in_ch =in_h;
			format = VSU_FORMAT_YUV422;
			break;
		}
		case DE_FORMAT_YUV420_P:
		case DE_FORMAT_YUV420_SP_UVUV:
		case DE_FORMAT_YUV420_SP_VUVU:
		{
			in_cw =(in_w+1)>>1;
			in_ch =(in_h+1)>>1;
			format = VSU_FORMAT_YUV420;
			break;
		}
		case DE_FORMAT_YUV411_P:
		case DE_FORMAT_YUV411_SP_UVUV:
		case DE_FORMAT_YUV411_SP_VUVU:
		{
			in_cw =(in_w+3)>>2;
			in_ch =in_h;
			format = VSU_FORMAT_YUV411;
			break;
		}
		default:
			in_cw =in_w;
			in_ch =in_h;
			format = VSU_FORMAT_RGB;
			break;
	}

	vsu_dev[sel][chno]->outsize.dwval = ((out_h - 1)<<16) | (out_w - 1);
	vsu_dev[sel][chno]->ysize.dwval = ((in_h - 1)<<16) | (in_w - 1);
	vsu_dev[sel][chno]->csize.dwval = ((in_ch - 1)<<16) | (in_cw - 1);
	vsu_dev[sel][chno]->yhstep.dwval = ypara->hstep<<VSU_PHASE_FRAC_REG_SHIFT;
	vsu_dev[sel][chno]->yvstep.dwval = ypara->vstep<<VSU_PHASE_FRAC_REG_SHIFT;
	vsu_dev[sel][chno]->chstep.dwval = cpara->hstep<<VSU_PHASE_FRAC_REG_SHIFT;
	vsu_dev[sel][chno]->cvstep.dwval = cpara->vstep<<VSU_PHASE_FRAC_REG_SHIFT;
	vsu_dev[sel][chno]->yhphase.dwval = ypara->hphase<<VSU_PHASE_FRAC_REG_SHIFT;
	vsu_dev[sel][chno]->yvphase0.dwval = ypara->vphase<<VSU_PHASE_FRAC_REG_SHIFT;

	pt_coef = de_vsu_calc_fir_coef(ypara->hstep);
	memcpy(&vsu_dev[sel][chno]->yhcoeff0, lan3coefftab32_left+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);
	memcpy(&vsu_dev[sel][chno]->yhcoeff1, lan3coefftab32_right+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);

	pt_coef = de_vsu_calc_fir_coef(ypara->vstep);
	memcpy(&vsu_dev[sel][chno]->yvcoeff, lan2coefftab32+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);

	if(VSU_FORMAT_RGB == format)
	{
		pt_coef = de_vsu_calc_fir_coef(cpara->hstep);
		memcpy(&vsu_dev[sel][chno]->chcoeff0, lan3coefftab32_left+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);
		memcpy(&vsu_dev[sel][chno]->chcoeff1, lan3coefftab32_right+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);

		pt_coef = de_vsu_calc_fir_coef(cpara->vstep);
		memcpy(&vsu_dev[sel][chno]->cvcoeff, lan2coefftab32+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);
	}
	else
	{
		pt_coef = de_vsu_calc_fir_coef(cpara->hstep);
		memcpy(&vsu_dev[sel][chno]->chcoeff0, bicubic8coefftab32_left+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);
		memcpy(&vsu_dev[sel][chno]->chcoeff1, bicubic8coefftab32_right+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);

		pt_coef = de_vsu_calc_fir_coef(cpara->vstep);
		memcpy(&vsu_dev[sel][chno]->cvcoeff, bicubic4coefftab32+pt_coef, sizeof(unsigned int)*VSU_PHASE_NUM);
	}
	vsu_dev[sel][chno]->ctrl.bits.coef_switch_rdy = 1;
	vsu_glb_block[sel][chno].dirty 			= 1;
	vsu_out_block[sel][chno].dirty 			= 1;
	vsu_yscale_block[sel][chno].dirty 		= 1;
	vsu_cscale_block[sel][chno].dirty 		= 1;
	vsu_yhcoeff0_block[sel][chno].dirty 	= 1;

	return 0;
}

int de_vsu_calc_scaler_para(unsigned char fmt, de_rect64 crop, de_rect frame, de_rect *crop_fix,
							scaler_para *ypara,scaler_para *cpara)
{
	int format;
	unsigned long long tmp = 0;

	switch(fmt)
	{
		case DE_FORMAT_YUV422_I_YVYU:
		case DE_FORMAT_YUV422_I_YUYV:
		case DE_FORMAT_YUV422_I_UYVY:
		case DE_FORMAT_YUV422_I_VYUY:
		case DE_FORMAT_YUV422_P:
		case DE_FORMAT_YUV422_SP_UVUV:
		case DE_FORMAT_YUV422_SP_VUVU:
		{
			format = VSU_FORMAT_YUV422;break;
		}
		case DE_FORMAT_YUV420_P:
		case DE_FORMAT_YUV420_SP_UVUV:
		case DE_FORMAT_YUV420_SP_VUVU:
		{
			format = VSU_FORMAT_YUV420;break;
		}
		case DE_FORMAT_YUV411_P:
		case DE_FORMAT_YUV411_SP_UVUV:
		case DE_FORMAT_YUV411_SP_VUVU:
		{
			format = VSU_FORMAT_YUV411;break;
		}
		default:
			format = VSU_FORMAT_RGB;break;
	}

	tmp = (N2_POWER(crop.w,VSU_PHASE_FRAC_BITWIDTH));
	do_div(tmp, frame.w);
	ypara->hstep= (unsigned int)(tmp>>VSU_FB_FRAC_BITWIDTH);

	tmp = (N2_POWER(crop.h,VSU_PHASE_FRAC_BITWIDTH));
	do_div(tmp, frame.h);
	ypara->vstep= (unsigned int)(tmp>>VSU_FB_FRAC_BITWIDTH);

	ypara->hphase = ((crop.x & 0xffffffff)>>(32-VSU_PHASE_FRAC_BITWIDTH));
	ypara->vphase = ((crop.y & 0xffffffff)>>(32-VSU_PHASE_FRAC_BITWIDTH));

	crop_fix->w = (unsigned int)(((crop.w & 0xffffffff) + (crop.x & 0xffffffff))>= N2_POWER(1,VSU_FB_FRAC_BITWIDTH)?
								 ((crop.w>>VSU_FB_FRAC_BITWIDTH) + 1) : (crop.w>>VSU_FB_FRAC_BITWIDTH));

	crop_fix->h = (unsigned int)(((crop.h & 0xffffffff) + (crop.y & 0xffffffff))>= N2_POWER(1,VSU_FB_FRAC_BITWIDTH)?
								 ((crop.h>>VSU_FB_FRAC_BITWIDTH) + 1) : (crop.h>>VSU_FB_FRAC_BITWIDTH));

	crop_fix->x = (int)(crop.x>>VSU_FB_FRAC_BITWIDTH);
	crop_fix->y = (int)(crop.y>>VSU_FB_FRAC_BITWIDTH);

	if(VSU_FORMAT_RGB == format)
	{
		cpara->hstep = ypara->hstep;
		cpara->vstep = ypara->vstep;
		cpara->hphase = ypara->hphase;
		cpara->vphase = ypara->vphase;
	}
	else if(VSU_FORMAT_YUV422 == format)
	{
		if((crop_fix->x & 0x1) == 0x0 && (crop_fix->w & 0x1) == 0x1)
		{
			crop_fix->w = (crop_fix->w>>1)<<1;
		}
		else if((crop_fix->x & 0x1) == 0x1 && (crop_fix->w & 0x1) == 0x0)
		{
			crop_fix->x = (crop_fix->x>>1)<<1;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
		}
		else if((crop_fix->x & 0x1) == 0x1 && (crop_fix->w & 0x1) == 0x1)
		{
			crop_fix->x = (crop_fix->x>>1)<<1;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
			crop_fix->w = ((crop_fix->w + 1)>>1)<<1;
		}


		cpara->hstep = ypara->hstep>>1;
		cpara->vstep = ypara->vstep;
		cpara->hphase = ypara->hphase;
		cpara->vphase = ypara->vphase;

	}
	else if(VSU_FORMAT_YUV420 == format)
	{
		if((crop_fix->x & 0x1) == 0x0 && (crop_fix->w & 0x1) == 0x1)
		{
			crop_fix->w = (crop_fix->w>>1)<<1;
		}
		else if((crop_fix->x & 0x1) == 0x1 && (crop_fix->w & 0x1) == 0x0)
		{
			crop_fix->x = (crop_fix->x>>1)<<1;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
		}
		else if((crop_fix->x & 0x1) == 0x1 && (crop_fix->w & 0x1) == 0x1)
		{
			crop_fix->x = (crop_fix->x>>1)<<1;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
			crop_fix->w = ((crop_fix->w + 1)>>1)<<1;
		}

		if((crop_fix->y & 0x1) == 0x0 && (crop_fix->h & 0x1) == 0x1)
		{
			crop_fix->h = (crop_fix->h>>1)<<1;
		}
		else if((crop_fix->y & 0x1) == 0x1 && (crop_fix->h & 0x1) == 0x0)
		{
			crop_fix->y = (crop_fix->y>>1)<<1;
			ypara->vphase = ypara->vphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
		}
		else if((crop_fix->y & 0x1) == 0x1 && (crop_fix->h & 0x1) == 0x1)
		{
			crop_fix->y = (crop_fix->y>>1)<<1;
			ypara->vphase = ypara->vphase + (unsigned int)(N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH));
			crop_fix->h = ((crop_fix->h + 1)>>1)<<1;
		}

		cpara->hstep = ypara->hstep>>1;
		cpara->vstep = ypara->vstep>>1;
		cpara->hphase = ypara->hphase>>1;
		cpara->vphase = (ypara->vphase>>1) - ((N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH))>>2);

	}
	else if(VSU_FORMAT_YUV411 == format)
	{
		if((crop_fix->x & 0x3) == 0x0 && (crop_fix->w & 0x3) != 0x0)
		{
			crop_fix->w = (crop_fix->w>>2)<<2;
		}
		else if((crop_fix->x & 0x3) != 0x0 && (crop_fix->w & 0x3) == 0x0)
		{
			crop_fix->x = (crop_fix->x>>2)<<2;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(crop_fix->x & 0x3,VSU_PHASE_FRAC_BITWIDTH));
		}
		else if((crop_fix->x & 0x3) != 0x0 && (crop_fix->w & 0x3) != 0x0)
		{
			crop_fix->x = (crop_fix->x>>2)<<2;
			ypara->hphase = ypara->hphase + (unsigned int)(N2_POWER(crop_fix->x & 0x3,VSU_PHASE_FRAC_BITWIDTH));
			crop_fix->w = ((crop_fix->w + (crop_fix->x & 0x3))>>2)<<2;
		}

		cpara->hstep = ypara->hstep>>2;
		cpara->vstep = ypara->vstep;
		cpara->hphase = ypara->hphase;
		cpara->vphase = ypara->vphase;
	}
	return 0;
}

int de_vsu_sel_ovl_scaler_para(unsigned char *en, scaler_para *layer_luma_scale_para, scaler_para *layer_chroma_scale_para,
									scaler_para *ovl_luma_scale_para, scaler_para *ovl_chroma_scale_para)
{
	int i, j, layer_en_num;
	int used_layer_idx[LAYER_MAX_NUM_PER_CHN];

	layer_en_num = 0;
	j = 0;
	for(i=0; i<LAYER_MAX_NUM_PER_CHN; i++)
	{
		if(*(en + i) == 1)
		{
			layer_en_num++;
			used_layer_idx[j++] = i;
		}
	}

	if(layer_en_num==1)
	{
		ovl_luma_scale_para->hphase = layer_luma_scale_para[used_layer_idx[0]].hphase;
		ovl_luma_scale_para->vphase = layer_luma_scale_para[used_layer_idx[0]].vphase;
		ovl_luma_scale_para->hstep = layer_luma_scale_para[used_layer_idx[0]].hstep;
		ovl_luma_scale_para->vstep = layer_luma_scale_para[used_layer_idx[0]].vstep;
		ovl_chroma_scale_para->hphase = layer_chroma_scale_para[used_layer_idx[0]].hphase;
		ovl_chroma_scale_para->vphase = layer_chroma_scale_para[used_layer_idx[0]].vphase;
		ovl_chroma_scale_para->hstep = layer_chroma_scale_para[used_layer_idx[0]].hstep;
		ovl_chroma_scale_para->vstep = layer_chroma_scale_para[used_layer_idx[0]].vstep;
	}
	else if(layer_en_num>1)
	{
		ovl_luma_scale_para->hstep = layer_luma_scale_para[used_layer_idx[0]].hstep;
		ovl_luma_scale_para->vstep = layer_luma_scale_para[used_layer_idx[0]].vstep;
		ovl_chroma_scale_para->hstep = layer_chroma_scale_para[used_layer_idx[0]].hstep;
		ovl_chroma_scale_para->vstep = layer_chroma_scale_para[used_layer_idx[0]].vstep;

		for(i=0; i<layer_en_num; i++)
		{
			if(layer_luma_scale_para[used_layer_idx[i]].hphase != 0)
			{
				ovl_luma_scale_para->hphase = layer_luma_scale_para[used_layer_idx[i]].hphase;
				ovl_chroma_scale_para->hphase = layer_chroma_scale_para[used_layer_idx[i]].hphase;
				break;
			}
		}

		if(i == layer_en_num)
		{
			ovl_luma_scale_para->hphase = layer_luma_scale_para[used_layer_idx[0]].hphase;
			ovl_chroma_scale_para->hphase = layer_chroma_scale_para[used_layer_idx[0]].hphase;
		}

		for(i=0; i<layer_en_num; i++)
		{
			if(layer_luma_scale_para[used_layer_idx[i]].vphase != 0)
			{
				ovl_luma_scale_para->vphase = layer_luma_scale_para[used_layer_idx[i]].vphase;
				ovl_chroma_scale_para->vphase  = layer_chroma_scale_para[used_layer_idx[i]].vphase;
				break;
			}
		}

		if(i == layer_en_num)
		{
			ovl_luma_scale_para->vphase = layer_luma_scale_para[used_layer_idx[0]].vphase;
			ovl_chroma_scale_para->vphase = layer_chroma_scale_para[used_layer_idx[0]].vphase;
		}

	}
	else
	{
		ovl_luma_scale_para->hphase = 0;
		ovl_luma_scale_para->vphase = 0;
		ovl_luma_scale_para->hstep = (unsigned int)N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH);
		ovl_luma_scale_para->vstep = (unsigned int)N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH);
		ovl_chroma_scale_para->hphase = 0;
		ovl_chroma_scale_para->vphase = 0;
		ovl_chroma_scale_para->hstep = (unsigned int)N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH);
		ovl_chroma_scale_para->vstep = (unsigned int)N2_POWER(1,VSU_PHASE_FRAC_BITWIDTH);
	}

	return 0;
}

int de_vsu_recalc_scale_para(int coarse_status, unsigned int vsu_outw, unsigned int vsu_outh,
							 unsigned int vsu_inw, unsigned int vsu_inh, unsigned int vsu_inw_c, unsigned int vsu_inh_c,
							 scaler_para *fix_y_para, scaler_para *fix_c_para)
{
	unsigned int tmp = 0;
	if(coarse_status & DE_CS_HORZ)
	{
		tmp = N2_POWER(vsu_inw, VSU_PHASE_FRAC_BITWIDTH);
		tmp = tmp / vsu_outw;
		fix_y_para->hstep =  tmp;

		tmp = N2_POWER(vsu_inw_c, VSU_PHASE_FRAC_BITWIDTH);
		tmp = tmp / vsu_outw;
		fix_c_para->hstep =  tmp;
		fix_y_para->hphase = 0;
		fix_c_para->hphase = 0;
	}

	if(coarse_status & DE_CS_VERT)
	{
		tmp = N2_POWER(vsu_inh, VSU_PHASE_FRAC_BITWIDTH);
		tmp = tmp / vsu_outh;
		fix_y_para->vstep =  tmp;

		tmp = N2_POWER(vsu_inh_c, VSU_PHASE_FRAC_BITWIDTH);
		tmp = tmp / vsu_outh;
		fix_c_para->vstep =  tmp;
		fix_y_para->vphase = 0;
		fix_c_para->vphase = 0;
	}

	return 0;
}

int de_recalc_ovl_bld_for_scale(unsigned int scaler_en, unsigned char *lay_en, int laynum, scaler_para *step,
						 de_rect *layer, de_rect *bld_rect, unsigned int *ovlw, unsigned int *ovlh,
						 unsigned int gsu_sel, unsigned int scn_w, unsigned scn_h)
{
	unsigned int shift, i;
	unsigned int org_bld_w, org_bld_h;

	if(scaler_en==0)
		return 0;

	if(*ovlw != 0 && *ovlh != 0 && bld_rect->w != 0 && bld_rect->h != 0 && step->hstep != 0 && step->vstep != 0)
	{
		if(*ovlw < SC_MIN_WIDTH || bld_rect->w < SC_MIN_WIDTH)
		{
			shift = (gsu_sel==0)?VSU_PHASE_FRAC_BITWIDTH : GSU_PHASE_FRAC_BITWIDTH;
			org_bld_w = bld_rect->w;
			if(step->hstep > (1<<shift))
			{
				bld_rect->w = SC_MIN_WIDTH;
				*ovlw = (step->hstep*SC_MIN_WIDTH)>>shift;
			}
			else
			{
				*ovlw = SC_MIN_WIDTH;
				bld_rect->w = SC_MIN_WIDTH*(1<<shift)/step->hstep;
			}

			if(bld_rect->w + bld_rect->x > scn_w)
			{
				bld_rect->x -= (bld_rect->w - org_bld_w);

				for(i=0;i<laynum;i++)
				{
					if(lay_en[i])
					{
						layer[i].x += ((step->hstep*(bld_rect->w - org_bld_w))>>shift);
					}
				}
			}
		}

		if(*ovlh < SC_MIN_HEIGHT || bld_rect->h< SC_MIN_HEIGHT)
		{
			shift = (gsu_sel==0)?VSU_PHASE_FRAC_BITWIDTH : GSU_PHASE_FRAC_BITWIDTH;
			org_bld_h = bld_rect->h;
			if(step->vstep > (1<<shift))
			{
				bld_rect->h = SC_MIN_HEIGHT;
				*ovlh = (step->vstep*SC_MIN_HEIGHT)>>shift;
			}
			else
			{
				*ovlh = SC_MIN_HEIGHT;
				bld_rect->h = SC_MIN_HEIGHT*(1<<shift)/step->vstep;
			}

			if(bld_rect->h + bld_rect->y > scn_h)
			{
				bld_rect->y -= (bld_rect->h - org_bld_h);

				for(i=0;i<laynum;i++)
				{
					if(lay_en[i])
					{
						layer[i].y += ((step->vstep*(bld_rect->h - org_bld_h))>>shift);
					}
				}
			}
		}

	}

	return 0 ;
}

