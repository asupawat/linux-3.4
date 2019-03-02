#ifndef __HDMI_BSP_H_
#define __HDMI_BSP_H_

#define LINUX_OS

#define HDMI_MAIN_START    (0xCB110800)
#define HDMI_CODE_SET_ADDR (0x1 << 0x00)
#define HDMI_CODE_INIT     (0x1 << 0x01)
#define HDMI_CODE_HPD      (0x1 << 0x02)
#define HDMI_CODE_DDC_RD   (0x1 << 0x03)
#define HDMI_CODE_VIDEO    (0x1 << 0x04)
#define HDMI_CODE_AUDIO    (0x1 << 0x05)
#define HDMI_CODE_VIDEO_EN (0x1 << 0x06)
#define HDMI_CODE_STANDBY  (0x1 << 0x07)
#define HDMI_CODE_HRST     (0x1 << 0x08)
#define HDMI_CODE_HDL      (0x1 << 0x09)
#define HDMI_CODE_VERSION  (0x1 << 0x0a)
#define HDMI_CODE_SET_FUNC (0x1 << 0x0b)

typedef struct
{
	void (*delay_us) (unsigned long us);
	void (*delay_ms) (unsigned long ms);
} hdmi_bsp_func;

#ifndef NULL
#define NULL 0
#endif

enum color_space
{
	BT601 = 1,
	BT709,
	EXT_CSC,
};

struct video_para
{
	unsigned int 			vic;
	enum color_space	csc;
	unsigned char			is_hdmi;
	unsigned char			is_yuv;
	unsigned char			is_hcts;
};

enum audio_type
{
	PCM = 1,
	AC3,
	MPEG1,
	MP3,
	MPEG2,
	AAC,
	DTS,
	ATRAC,
	OBA,
	DDP,
	DTS_HD,
	MAT,
	DST,
	WMA_PRO,
};

struct audio_para
{
	enum	audio_type	type;
	unsigned char			ca;
	unsigned int			sample_rate;
	unsigned int			sample_bit;
	unsigned int			ch_num;
};

extern unsigned char hdmi_mc_table[];
extern unsigned int hdmi_mc_table_size;

int bsp_hdmi_set_func(hdmi_bsp_func *func);
void bsp_hdmi_set_addr(unsigned int base_addr);
void bsp_hdmi_init(void);
void bsp_hdmi_set_video_en(unsigned char enable);
int bsp_hdmi_video(struct video_para *video);
int bsp_hdmi_audio(struct audio_para *audio);
int bsp_hdmi_ddc_read(char cmd,char pointer,char offset,int nbyte,char * pbuf);
unsigned int bsp_hdmi_get_hpd(void);
void bsp_hdmi_standby(void);
void bsp_hdmi_hrst(void);
void bsp_hdmi_hdl(void);
void bsp_hdmi_set_version(unsigned int version);
int bsp_hdmi_hdcp_err_check(void);

#endif