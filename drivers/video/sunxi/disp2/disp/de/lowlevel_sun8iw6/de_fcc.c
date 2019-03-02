
#include "de_fcc_type.h"
#include "de_rtmx.h"
#include "de_vep_table.h"
#include "de_enhance.h"

#define FCC_OFST 0xAA000

static volatile __fcc_reg_t *fcc_dev[DEVICE_NUM][CHN_NUM];
static de_reg_blocks fcc_para_block[DEVICE_NUM][CHN_NUM];

int de_fcc_set_reg_base(unsigned int sel, unsigned int chno, void *base)
{
	fcc_dev[sel][chno] = (__fcc_reg_t *)base;

	return 0;
}

int de_fcc_init(unsigned int sel, unsigned int chno, unsigned int reg_base)
{
	unsigned int fcc_base;
	void *memory;

	fcc_base = reg_base + (sel+1)*0x00100000 + FCC_OFST;
	__inf("sel %d, fcc_base[%d]=0x%x\n", sel, chno, fcc_base);

	memory = disp_sys_malloc(sizeof(__fcc_reg_t));
	if(NULL == memory) {
		__wrn("malloc vep fcc[%d][%d] memory fail! size=0x%x\n", sel, chno, sizeof(__fcc_reg_t));
		return -1;
	}

	fcc_para_block[sel][chno].off		= fcc_base;
	fcc_para_block[sel][chno].val		= memory;
	fcc_para_block[sel][chno].size		= 0x48;
	fcc_para_block[sel][chno].dirty 	= 0;

	de_fcc_set_reg_base(sel, chno, memory);

	return 0;
}

int de_fcc_update_regs(unsigned int sel, unsigned int chno)
{
	if(fcc_para_block[sel][chno].dirty == 0x1){
		memcpy((void *)fcc_para_block[sel][chno].off,fcc_para_block[sel][chno].val,fcc_para_block[sel][chno].size);
		fcc_para_block[sel][chno].dirty = 0x0;
	}

	return 0;
}

int de_fcc_enable(unsigned int sel, unsigned int chno, unsigned int en)
{
	fcc_dev[sel][chno]->fcc_ctl.bits.en = en;
	fcc_para_block[sel][chno].dirty	= 1;

	return 0;
}

int de_fcc_set_size(unsigned int sel, unsigned int chno, unsigned int width, unsigned int height)
{
	fcc_dev[sel][chno]->fcc_size.bits.width = width==0?0:width-1;
	fcc_dev[sel][chno]->fcc_size.bits.height = height==0?0:height-1;

	fcc_para_block[sel][chno].dirty	= 1;

	return 0;
}

int de_fcc_set_window(unsigned int sel, unsigned int chno, unsigned int win_en, de_rect window)
{
	fcc_dev[sel][chno]->fcc_ctl.bits.win_en = win_en&0x1;

	if(win_en)
	{
		fcc_dev[sel][chno]->fcc_win0.bits.left	= window.x;
		fcc_dev[sel][chno]->fcc_win0.bits.top	= window.y;
		fcc_dev[sel][chno]->fcc_win1.bits.right = window.x + window.w - 1;
		fcc_dev[sel][chno]->fcc_win1.bits.bot	= window.y + window.h - 1;
	}

	fcc_para_block[sel][chno].dirty	= 1;

	return 0;
}

int de_fcc_set_para(unsigned int sel, unsigned int chno, unsigned int sgain[6])
{
	memcpy((void*)fcc_dev[sel][chno]->fcc_range, (void*)&fcc_range_gain[0],sizeof(int)*6);
	memcpy((void*)fcc_dev[sel][chno]->fcc_gain, (void*)&sgain[0],sizeof(int)*6);

	fcc_para_block[sel][chno].dirty	= 1;

	return 0;
}

int de_fcc_info2para(unsigned int sgain0, unsigned int sgain1, unsigned int sgain2, unsigned int sgain3, unsigned int sgain4, unsigned int sgain5,
de_rect window, __fcc_config_data *para)
{
	int fcc_para[FCC_PARA_NUM][FCC_MODE_NUM] ={
	{0,5,10},  
	{0,12,24}, 
	{0,4,8},   
	{0,0,0},
	{0,0,0},
	{0,0,0},
	};

	para->fcc_en = ((sgain0|sgain1|sgain2|sgain3|sgain4|sgain5) == 0)?0:1;

	para->sgain[0] = fcc_para[0][sgain0];
	para->sgain[1] = fcc_para[1][sgain1];
	para->sgain[2] = fcc_para[2][sgain2];
	para->sgain[3] = fcc_para[3][sgain3];
	para->sgain[4] = fcc_para[4][sgain4];
	para->sgain[5] = fcc_para[5][sgain5];

	return 0;
}
