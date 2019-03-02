#include "hdmi_bsp.h"
#include "hdmi_core.h"
#include <asm/cacheflush.h>

int (*hdmi_main)(int type, uintptr_t* para) = NULL;

int bsp_hdmi_main(unsigned int type, uintptr_t* para)
{
	int ret = -1;

	if (hdmi_main)
		ret = hdmi_main(type, para);

	return ret;
}

int bsp_hdmi_set_func(hdmi_bsp_func *func)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)func;
	return bsp_hdmi_main(HDMI_CODE_SET_FUNC, para);
}

void bsp_hdmi_set_version(unsigned int version)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)version;
	bsp_hdmi_main(HDMI_CODE_VERSION, para);
}

void bsp_hdmi_set_addr(unsigned int base_addr)
{
	uintptr_t para[5];
	int ret;

	memcpy((void *)HDMI_MAIN_START, (void *)hdmi_mc_table, hdmi_mc_table_size);
	hdmi_main = (int (*)(int type, uintptr_t* para))HDMI_MAIN_START;
	__cpuc_flush_dcache_area((void *)HDMI_MAIN_START, hdmi_mc_table_size);
	__cpuc_flush_icache_all();

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)base_addr;
	ret = bsp_hdmi_main(HDMI_CODE_SET_ADDR, para);
}

void bsp_hdmi_init(void)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	bsp_hdmi_main(HDMI_CODE_INIT, para);
}

void bsp_hdmi_set_video_en(unsigned char enable)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)enable;
	bsp_hdmi_main(HDMI_CODE_VIDEO_EN, para);
}

int bsp_hdmi_video(struct video_para *video)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)video;
	return bsp_hdmi_main(HDMI_CODE_VIDEO, para);
}

int bsp_hdmi_audio(struct audio_para *audio)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)audio;
	return bsp_hdmi_main(HDMI_CODE_AUDIO, para);
}

int bsp_hdmi_ddc_read(char cmd,char pointer,char offset,int nbyte,char * pbuf)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	para[0] = (uintptr_t)cmd;
	para[1] = (uintptr_t)pointer;
	para[2] = (uintptr_t)offset;
	para[3] = (uintptr_t)nbyte;
	para[4] = (uintptr_t)pbuf;
	return bsp_hdmi_main(HDMI_CODE_DDC_RD, para);
}

unsigned int bsp_hdmi_get_hpd(void)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	return bsp_hdmi_main(HDMI_CODE_HPD, para);
}

void bsp_hdmi_standby(void)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	bsp_hdmi_main(HDMI_CODE_STANDBY, para);
}

void bsp_hdmi_hrst(void)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	bsp_hdmi_main(HDMI_CODE_HRST, para);
}

void bsp_hdmi_hdl(void)
{
	uintptr_t para[5];

	memset(para, 0, 5*sizeof(uintptr_t));
	bsp_hdmi_main(HDMI_CODE_HDL, para);
}
