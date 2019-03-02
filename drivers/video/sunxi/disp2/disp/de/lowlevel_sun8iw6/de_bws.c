
#include "de_bws_type.h"
#include "de_rtmx.h"
#include "de_fce_type.h"
#include "de_enhance.h"

#define BWS_OFST	0xA2000	

#ifndef put_wvalue
#define put_wvalue(addr, v)	(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
#endif

static volatile __bws_reg_t *bws_dev[DEVICE_NUM][CHN_NUM];
static de_reg_blocks bws_block[DEVICE_NUM][CHN_NUM];
static de_reg_blocks bws_para_block[DEVICE_NUM][CHN_NUM];

static unsigned int bws_hw_base[DEVICE_NUM][CHN_NUM] = {{0}};

__bws_status_t *g_bws_status[DEVICE_NUM][CHN_NUM];

extern unsigned int *g_hist[DEVICE_NUM][CHN_NUM];
extern unsigned int *g_hist_p[DEVICE_NUM][CHN_NUM];
extern unsigned int g_sum[DEVICE_NUM][CHN_NUM];
extern __hist_status_t *g_hist_status[DEVICE_NUM][CHN_NUM];

int de_bws_set_reg_base(unsigned int sel, unsigned int chno, unsigned int base)
{
	bws_dev[sel][chno] = (__bws_reg_t *)base;

	return 0;
}

#ifdef __VEP_DO_IT_IN_VBLANK
int de_bws_update_regs(unsigned int sel, unsigned int chno)
{
	if(bws_block[sel][chno].dirty == 0x1){
		memcpy((void *)bws_block[sel][chno].off,bws_block[sel][chno].val,bws_block[sel][chno].size);
		bws_block[sel][chno].dirty = 0x0;
	}

	return 0;
}
#else
int de_bws_update_regs(unsigned int sel, unsigned int chno)
{
	if(bws_block[sel][chno].dirty == 0x1){
		memcpy((void *)bws_block[sel][chno].off,bws_block[sel][chno].val,bws_block[sel][chno].size);
		bws_block[sel][chno].dirty = 0x0;
	}

	if(bws_para_block[sel][chno].dirty == 0x1){
		memcpy((void *)bws_para_block[sel][chno].off,bws_para_block[sel][chno].val,bws_para_block[sel][chno].size);
		bws_para_block[sel][chno].dirty = 0x0;
	}
	return 0;
}

#endif

int de_bws_init(unsigned int sel, unsigned int chno, unsigned int reg_base)
{
	unsigned int base;
	void *memory;

	base = reg_base + (sel+1)*0x00100000 + BWS_OFST;
	bws_hw_base[sel][chno] = base;

	__inf("sel %d, bws_base[%d]=0x%x\n", sel, chno, base);

	memory = disp_sys_malloc(sizeof(__bws_reg_t));
	if(NULL == memory) {
		__wrn("malloc bws[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__bws_reg_t));
		return -1;
	}

	bws_block[sel][chno].off			= base;
	bws_block[sel][chno].val			= memory;
	bws_block[sel][chno].size			= 0x10;
	bws_block[sel][chno].dirty 			= 0;

	bws_para_block[sel][chno].off		= base + 0x20;
	bws_para_block[sel][chno].val		= memory + 0x20;
	bws_para_block[sel][chno].size		= 0x10;
	bws_para_block[sel][chno].dirty 	= 0;

	de_bws_set_reg_base(sel, chno, (unsigned int)memory);

	g_bws_status[sel][chno] = disp_sys_malloc(sizeof(__bws_status_t));
	if(NULL == g_bws_status[sel][chno]) {
		__wrn("malloc g_bws_status[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__bws_status_t));
		return -1;
	}

	g_bws_status[sel][chno]->IsEnable = 0;
	g_bws_status[sel][chno]->Runtime = 0;
	g_bws_status[sel][chno]->PreSlopeReady = 0;

	return 0;
}

int de_bws_enable(unsigned int sel, unsigned int chno, unsigned int en)
{
	bws_dev[sel][chno]->ctrl.bits.en = en;
	bws_block[sel][chno].dirty 		= 1;
	return 0;
}

int de_bws_set_size(unsigned int sel, unsigned int chno, unsigned int width, unsigned int height)
{
	bws_dev[sel][chno]->size.bits.width = width - 1;
	bws_dev[sel][chno]->size.bits.height = height - 1;
	bws_block[sel][chno].dirty 		= 1;
	return 0;
}

int de_bws_set_window(unsigned int sel, unsigned int chno, unsigned int win_enable, de_rect window)
{
	bws_dev[sel][chno]->ctrl.bits.win_en = win_enable;

	if(win_enable)
	{
		bws_dev[sel][chno]->win0.bits.win_left = window.x;
		bws_dev[sel][chno]->win0.bits.win_top = window.y;
		bws_dev[sel][chno]->win1.bits.win_right = window.x + window.w - 1;
		bws_dev[sel][chno]->win1.bits.win_bot = window.y + window.h - 1;
	}
	bws_block[sel][chno].dirty 		= 1;
	return 0;
}

#ifdef __VEP_DO_IT_IN_VBLANK
int de_bws_set_para(unsigned int sel, unsigned int chno,
						unsigned int min, unsigned int black, unsigned int white, unsigned int max,
						unsigned int slope0, unsigned int slope1, unsigned int slope2, unsigned int slope3)
{
	unsigned int base;
	base = bws_hw_base[sel][chno];

	put_wvalue(base + 0x20, (black<<16) | min);
	put_wvalue(base + 0x24, (max<<16) | white);
	put_wvalue(base + 0x28, (slope1<<16) | slope0);
	put_wvalue(base + 0x2c, (slope3<<16) | slope2);

	return 0;
}
#else
int de_bws_set_para(unsigned int sel, unsigned int chno,
						unsigned int min, unsigned int black, unsigned int white, unsigned int max,
						unsigned int slope0, unsigned int slope1, unsigned int slope2, unsigned int slope3)
{

	bws_dev[sel][chno]->blkthr.bits.min = min;
	bws_dev[sel][chno]->blkthr.bits.black = black;
	bws_dev[sel][chno]->whtthr.bits.max = max;
	bws_dev[sel][chno]->whtthr.bits.white = white;

	bws_dev[sel][chno]->blkslp.bits.slope0 = slope0;
	bws_dev[sel][chno]->blkslp.bits.slope1 = slope1;
	bws_dev[sel][chno]->whtslp.bits.slope2 = slope2;
	bws_dev[sel][chno]->whtslp.bits.slope3 = slope3;

	bws_para_block[sel][chno].dirty = 1;

	return 0;
}

#endif
static void auto_bws_model(unsigned int width, unsigned int height, unsigned int hist[256], unsigned int hist_pre[256], unsigned int sum,
					unsigned int pre_slope_black, unsigned int pre_slope_white,
					unsigned int frame_bld_en, unsigned int bld_high_thr, unsigned int bld_low_thr, unsigned int bld_weight_lmt,
					unsigned int present_black, unsigned int present_white, unsigned int slope_black_lmt, unsigned int slope_white_lmt,
					unsigned int black_prec, unsigned int white_prec, unsigned int lowest_black, unsigned int highest_white,
					unsigned int *ymin, unsigned int *black, unsigned int *white, unsigned int *ymax,
					unsigned int *slope0, unsigned int *slope1, unsigned int *slope2, unsigned int *slope3)
{
	int coeff, diff_hist, total, k;
	int validcnt, validsum;
	int ratio_b, ratio_w, cdf_b, cdf_w;
	int weight;
	int mean;
	int pd_ymin = lowest_black, pd_ymax = highest_white;
	int pd_black, pd_white;
	int pd_ymin_fix, pd_ymax_fix;
	int pd_s0, pd_s1, pd_s2, pd_s3;
	int tmp;
	if(frame_bld_en)
	{
		diff_hist = 0;
		total = 0;
		for(k=0; k<256; k++)
		{
			diff_hist += abs(hist[k] - hist_pre[k]);
			total += hist[k];
		}
		coeff = (100 * diff_hist) / total;

		weight = (coeff >= bld_high_thr)? 256 :
			(coeff < bld_low_thr)? bld_weight_lmt :
			((256 - bld_weight_lmt)/(bld_high_thr - bld_low_thr) * (coeff - bld_low_thr) + bld_weight_lmt);

	}
	else
	{
		total = 0;
		for(k=0; k<256; k++)
		{
			total += hist[k];
		}
		weight = 256;
	}

	validcnt = total;
	for (k=0;k<lowest_black;k++)
	{
		validcnt -= hist[k];
	}
	for (k=255;k>highest_white-1;k--)
	{
		validcnt -= hist[k];
	}

	validsum = sum;
	for (k=0;k<lowest_black;k++)
	{
		validsum -= hist[k]*k;
	}
	for (k=255;k>highest_white-1;k--)
	{
		validsum -= hist[k]*k;
	}
	if(validcnt != 0)
	{
		mean = validsum / validcnt;

		ratio_b = validcnt * black_prec/100;
		cdf_b = 0;
		for(k = lowest_black; k < 255; k++)
		{
			cdf_b += hist[k];
			if(cdf_b > ratio_b) {
				pd_ymin = k;
				break;
			}
		}

		ratio_w = validcnt * white_prec/100;
		cdf_w = 0;
		for(k = highest_white; k >=0; k--)
		{
			cdf_w += hist[k];
			if(cdf_w > ratio_w) {
				pd_ymax = k;
				break;
			}
		}

		pd_black = (present_black<mean)?present_black:mean;
		pd_white = (present_white>mean)?present_white:mean;

		pd_s1 = (pd_ymin < pd_black)?((pd_black<<8)/(pd_black - pd_ymin)):256;
		pd_s1 = (pd_s1 > slope_black_lmt)?slope_black_lmt:pd_s1;

		pd_s2 = (pd_ymax > pd_white)?(((255-pd_white)<<8)/(pd_ymax - pd_white)):256;
		pd_s2 = (pd_s2 > slope_white_lmt)?slope_white_lmt:pd_s2;

		pd_s1 = (pre_slope_black * (256 - weight) + weight * pd_s1)>>8;
		pd_s2 = (pre_slope_white * (256 - weight) + weight * pd_s2)>>8;

		tmp = pd_black + ((pd_s1 * (pd_ymin - pd_black)+128)	>>8	);
		if(		( tmp>0)	&& (pd_ymin < pd_black) && (pd_ymin > 0))
		{
			pd_s0 = ((tmp<<8) + 128) / pd_ymin;
			pd_ymin_fix = pd_ymin;
		}
		else if (pd_ymin >= pd_black)
		{
			pd_s0 = 256;
			pd_ymin_fix = 0;
		}
		else
		{
			pd_s0 = 0;
			pd_ymin_fix = -((pd_black<<8) - 128 ) / pd_s1 + pd_black;
		}

		tmp =  pd_white + ((pd_s2 * (pd_ymax - pd_white)		)>>8);
		if(		(tmp < 255)	&& (pd_ymax > pd_white)  && (pd_ymax < 255))
		{
			pd_s3 = (((255-tmp)<<8) + 128) / (255 - pd_ymax);
			pd_ymax_fix = pd_ymax;
		}
		else if (pd_ymax <= pd_white)
		{
			pd_s3 = 256;
			pd_ymax_fix = 255;
		}
		else
		{
			pd_s3 = 0;
			pd_ymax_fix = (((255-pd_white)<<8) - 128) / pd_s2 + pd_white;
		}
	}
	else
	{
		pd_ymin_fix = 16;
		pd_black = 32;
		pd_white = 224;
		pd_ymax_fix = 240;
		pd_s0 = 0x100;
		pd_s1 = 0x100;
		pd_s2 = 0x100;
		pd_s3 = 0x100;
	}

	*ymin = pd_ymin_fix;
	*black = pd_black;
	*white = pd_white;
	*ymax = pd_ymax_fix;
	*slope0 = pd_s0;
	*slope1 = pd_s1;
	*slope2 = pd_s2;
	*slope3 = pd_s3;


}
int de_bws_info2para(unsigned int auto_contrast, de_rect window, __bws_config_data *para)
{
	para->bws_en = (auto_contrast == 2 || auto_contrast == 3)?1:0;


	return 0;
}

int de_bws_apply(unsigned int screen_id, unsigned int chno, unsigned int bws_en, unsigned int auto_contrast_dirty)
{
	if(bws_en==1 && auto_contrast_dirty)
	{
		g_bws_status[screen_id][chno]->IsEnable = 1;
		g_bws_status[screen_id][chno]->Runtime = 0;
		g_bws_status[screen_id][chno]->PreSlopeReady = 0;
	}
	else if(bws_en==0 && auto_contrast_dirty)
	{
		g_bws_status[screen_id][chno]->IsEnable = 0;
		g_bws_status[screen_id][chno]->Runtime = 0;
		g_bws_status[screen_id][chno]->PreSlopeReady = 0;
	}
	return 0;

}

int de_bws_tasklet(unsigned int screen_id, unsigned int chno, unsigned int frame_cnt)
{
	unsigned int pre_slope_black, pre_slope_white;
	unsigned int th0, th1, th2, th3, s0, s1, s2, s3;
	if(g_bws_status[screen_id][chno]->IsEnable &&  ((BWS_FRAME_MASK==(frame_cnt%2)) || (BWS_FRAME_MASK==0x2)))
	{
		if(g_hist_status[screen_id][chno]->TwoHistReady)
		{
			if(g_bws_status[screen_id][chno]->PreSlopeReady)
			{
				pre_slope_black = g_bws_status[screen_id][chno]->slope_black;
				pre_slope_white = g_bws_status[screen_id][chno]->slope_white;
			}
			else
			{
				pre_slope_black = BWS_DEFAULT_SLOPE;
				pre_slope_white = BWS_DEFAULT_SLOPE;
			}

			auto_bws_model(g_bws_status[screen_id][chno]->width, g_bws_status[screen_id][chno]->height,
				g_hist[screen_id][chno], g_hist_p[screen_id][chno], g_sum[screen_id][chno],
				pre_slope_black, pre_slope_white, 1, 90, 74, 8, 53, 235, 512, 384, 5, 2, 3, 252,
				&th0, &th1, &th2, &th3, &s0, &s1, &s2, &s3);
			th0=32;
			th1=64;
			th2=224;
			th3=240;
			s0=192 ;
			s1=320;
			s2=384;
			s3=128;
		}
		else
		{
			th0 = 16;
			th1 = 32;
			th2 = 224;
			th3 = 240;
			s0 = BWS_DEFAULT_SLOPE;
			s1 = BWS_DEFAULT_SLOPE;
			s2 = BWS_DEFAULT_SLOPE;
			s3 = BWS_DEFAULT_SLOPE;
		}

		de_bws_set_para(screen_id, chno, th0, th1, th2, th3, s0, s1, s2, s3);

		g_bws_status[screen_id][chno]->slope_black = s1;
		g_bws_status[screen_id][chno]->slope_white = s2;

		if(g_bws_status[screen_id][chno]->Runtime < 1)
		{
			g_bws_status[screen_id][chno]->Runtime ++;
		}
		else
		{
			g_bws_status[screen_id][chno]->PreSlopeReady = 1;
		}
	}
	return 0;

}

