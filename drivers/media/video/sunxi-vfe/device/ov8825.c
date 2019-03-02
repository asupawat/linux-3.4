/*
 * A V4L2 driver for OV8825 cameras.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>
#include <linux/clk.h>
#include <media/v4l2-device.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-mediabus.h>
#include <linux/io.h>


#include "camera.h"


MODULE_AUTHOR("lwj");
MODULE_DESCRIPTION("A low-level driver for OV8825 sensors");
MODULE_LICENSE("GPL");


#define DEV_DBG_EN      1 
#if(DEV_DBG_EN == 1)    
#define vfe_dev_dbg(x,arg...) printk("[OV8825]"x,##arg)
#else
#define vfe_dev_dbg(x,arg...) 
#endif
#define vfe_dev_err(x,arg...) printk("[OV8825]"x,##arg)
#define vfe_dev_print(x,arg...) printk("[OV8825]"x,##arg)

#define LOG_ERR_RET(x)  { \
                          int ret;  \
                          ret = x; \
                          if(ret < 0) {\
                            vfe_dev_err("error at %s\n",__func__);  \
                            return ret; \
                          } \
                        }

#define MCLK              (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x8825
int ov8825_sensor_vts;

#define CSI_STBY_ON     0
#define CSI_STBY_OFF    1
#define CSI_RST_ON      0
#define CSI_RST_OFF     1
#define CSI_PWR_ON      1
#define CSI_PWR_OFF     0
#define CSI_AF_PWR_ON   1
#define CSI_AF_PWR_OFF  0
#define regval_list reg_list_a16_d8


#define REG_TERM 0xfffe
#define VAL_TERM 0xfe
#define REG_DLY  0xffff

/*
 * Our nominal (default) frame rate.
 */

#define SENSOR_FRAME_RATE 30


/*
 * The ov8825 sits on i2c with ID 0x6c
 */
#define I2C_ADDR 0x6c
#define  SENSOR_NAME "ov8825"
static struct v4l2_subdev *glb_sd;

/*
 * Information we maintain about a known sensor.
 */
struct sensor_format_struct;  /* coming later */

struct cfg_array { /* coming later */
	struct regval_list * regs;
	int size;
};

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}


/*
 * The default register settings
 *
 */


static struct regval_list sensor_default_regs[] = {
	{0x0103,0x01},
	{0x3000,0x16},
	{0x3001,0x00},
	{0x3002,0x6c},
	{0x300d,0x00},
	{0x301f,0x09},
	{0x3010,0x00},
	{0x3011,0x01},
	{0x3018,0x00},
	{0x3104,0x20},
	{0x3300,0x00},
	{0x3509,0x10},
	{0x3603,0x5c},
	{0x3604,0x98},
	{0x3605,0xf5},
	{0x3609,0xb4},
	{0x360a,0x7c},
	{0x360b,0xc9},
	{0x360c,0x0b},
	{0x3612,0x00},
	{0x3613,0x02},
	{0x3614,0x0f},
	{0x3615,0x00},
	{0x3616,0x03},
	{0x3617,0xa1},
	{0x3618,0x00},
	{0x3619,0x00},
	{0x361a,0xB0},
	{0x361b,0x04},
	{0x3701,0x44},
	{0x3707,0x63},
	{0x370b,0x01},
	{0x370c,0x50},
	{0x370d,0x00},
	{0x3816,0x02},
	{0x3817,0x40},
	{0x3818,0x00},
	{0x3819,0x40},
	{0x3b1f,0x00},
	{0x3d00,0x00},
	{0x3d01,0x00},
	{0x3d02,0x00},
	{0x3d03,0x00},
	{0x3d04,0x00},
	{0x3d05,0x00},
	{0x3d06,0x00},
	{0x3d07,0x00},
	{0x3d08,0x00},
	{0x3d09,0x00},
	{0x3d0a,0x00},
	{0x3d0b,0x00},
	{0x3d0c,0x00},
	{0x3d0d,0x00},
	{0x3d0e,0x00},
	{0x3d0f,0x00},
	{0x3d10,0x00},
	{0x3d11,0x00},
	{0x3d12,0x00},
	{0x3d13,0x00},
	{0x3d14,0x00},
	{0x3d15,0x00},
	{0x3d16,0x00},
	{0x3d17,0x00},
	{0x3d18,0x00},
	{0x3d19,0x00},
	{0x3d1a,0x00},
	{0x3d1b,0x00},
	{0x3d1c,0x00},
	{0x3d1d,0x00},
	{0x3d1e,0x00},
	{0x3d1f,0x00},
	{0x3d80,0x00},
	{0x3d81,0x00},
	{0x3d84,0x00},
	{0x3f01,0xfc},
	{0x3f05,0x10},
	{0x3f06,0x00},
	{0x3f07,0x00},
	{0x4000,0x29},
	{0x4001,0x02},
	{0x4002,0x45},
	{0x4003,0x08},
	{0x4004,0x04},
	{0x4005,0x18},
	{0x4300,0xff},
	{0x4303,0x00},
	{0x4304,0x08},
	{0x4307,0x00},
	{0x4800,0x04},
	{0x4801,0x0f},
	{0x4843,0x02},
	{0x485c,0x1f},
	{0x5000,0x06},
	{0x5001,0x00},
	{0x5002,0x00},
	{0x501f,0x00},
	{0x5780,0xfc},
	{0x5c00,0x80},
	{0x5c01,0x00},
	{0x5c02,0x00},
	{0x5c03,0x00},
	{0x5c04,0x00},
	{0x5c05,0x00},
	{0x5c06,0x00},
	{0x5c07,0x80},
	{0x5c08,0x10},
	{0x6700,0x05},
	{0x6701,0x19},
	{0x6702,0xfd},
	{0x6703,0xd7},
	{0x6704,0xff},
	{0x6705,0xff},
	{0x6800,0x10},
	{0x6801,0x02},
	{0x6802,0x90},
	{0x6803,0x10},
	{0x6804,0x59},
	{0x6900,0x60},
	{0x6901,0x04},
	{0x5800,0x0f},
	{0x5801,0x0d},
	{0x5802,0x09},
	{0x5803,0x0a},
	{0x5804,0x0d},
	{0x5805,0x14},
	{0x5806,0x0a},
	{0x5807,0x04},
	{0x5808,0x03},
	{0x5809,0x03},
	{0x580a,0x05},
	{0x580b,0x0a},
	{0x580c,0x05},
	{0x580d,0x02},
	{0x580e,0x00},
	{0x580f,0x00},
	{0x5810,0x03},
	{0x5811,0x05},
	{0x5812,0x09},
	{0x5813,0x03},
	{0x5814,0x01},
	{0x5815,0x01},
	{0x5816,0x04},
	{0x5817,0x09},
	{0x5818,0x09},
	{0x5819,0x08},
	{0x581a,0x06},
	{0x581b,0x06},
	{0x581c,0x08},
	{0x581d,0x06},
	{0x581e,0x33},
	{0x581f,0x11},
	{0x5820,0x0e},
	{0x5821,0x0f},
	{0x5822,0x11},
	{0x5823,0x3f},
	{0x5824,0x08},
	{0x5825,0x46},
	{0x5826,0x46},
	{0x5827,0x46},
	{0x5828,0x46},
	{0x5829,0x46},
	{0x582a,0x42},
	{0x582b,0x42},
	{0x582c,0x44},
	{0x582d,0x46},
	{0x582e,0x46},
	{0x582f,0x60},
	{0x5830,0x62},
	{0x5831,0x42},
	{0x5832,0x46},
	{0x5833,0x46},
	{0x5834,0x44},
	{0x5835,0x44},
	{0x5836,0x44},
	{0x5837,0x48},
	{0x5838,0x28},
	{0x5839,0x46},
	{0x583a,0x48},
	{0x583b,0x68},
	{0x583c,0x28},
	{0x583d,0xae},
	{0x5842,0x00},
	{0x5843,0xef},
	{0x5844,0x01},
	{0x5845,0x3f},
	{0x5846,0x01},
	{0x5847,0x3f},
	{0x5848,0x00},
	{0x5849,0xd5},
	{0x3503,0x17},
	{0x3500,0x00},
	{0x3501,0xff},
	{0x3502,0x00},
	{0x350a,0x00},
	{0x350b,0x1f},
	{0x3400,0x04},
	{0x3401,0x00},
	{0x3402,0x04},
	{0x3403,0x00},
	{0x3404,0x04},
	{0x3405,0x00},
	{0x3406,0x00},
	{0x4805,0x10},
	{0x4824,0x03},
	{0x4825,0xff},
	{0x4830,0x3f},
	{0x4826,0x00},
	{0x4827,0x78},
	{0x4831,0x04},
};;

static struct regval_list sensor_quxga_regs[] = {
	{0x0100,0x00},
	{REG_DLY,0x32},
	{0x3003,0xce},
	{0x3004,0xd8},
	{0x3005,0x00},
	{0x3006,0x10},
	{0x3007,0x3b},
	{0x3012,0x81},
	{0x3013,0x39},
	{0x3106,0x11},
	{0x3600,0x07},
	{0x3601,0x33},
	{0x3602,0x42},
	{0x3700,0x10},
	{0x3702,0x28},
	{0x3703,0x6c},
	{0x3704,0x8d},
	{0x3705,0x0a},
	{0x3706,0x27},
	{0x3708,0x40},
	{0x3709,0x20},
	{0x370a,0x31},
	{0x370e,0x00},
	{0x3711,0x07},
	{0x3712,0x4e},
	{0x3724,0x00},
	{0x3725,0xd4},
	{0x3726,0x00},
	{0x3727,0xe1},
	{0x3800,0x00},
	{0x3801,0x00},
	{0x3802,0x00},
	{0x3803,0x00},
	{0x3804,0x0c},
	{0x3805,0xdf},
	{0x3806,0x09},
	{0x3807,0x9b},
	{0x3808,0x0c},
	{0x3809,0xc0},
	{0x380a,0x09},
	{0x380b,0x90},
	{0x380c,0x0e},
	{0x380d,0x00},
	{0x380e,0x09},
	{0x380f,0xb0},
	{0x3810,0x00},
	{0x3811,0x10},
	{0x3812,0x00},
	{0x3813,0x06},
	{0x3814,0x11},
	{0x3815,0x11},
	{0x3820,0x80},
	{0x3821,0x16},
	{0x3f00,0x02},
	{0x4005,0x1A},
	{0x404f,0x7F},
	{0x4600,0x04},
	{0x4601,0x00},
	{0x4602,0x78},
	{0x4837,0x28},
	{0x5068,0x00},
	{0x506a,0x00},
	{0x0100,0x01},
};


static struct regval_list sensor_1080p_regs[] = { 
	{0x0100,0x00},
	{REG_DLY,0x32},
	{0x3003,0xce},
	{0x3004,0xd4},
	{0x3005,0x00},
	{0x3006,0x00},
	{0x3007,0x3b},
	{0x3012,0x80},
	{0x3013,0x39},
	{0x3106,0x15},
	{0x3600,0x06},
	{0x3601,0x34},
	{0x3602,0x42},
	{0x3700,0x20},
	{0x3702,0x50},
	{0x3703,0xcc},
	{0x3704,0x19},
	{0x3705,0x14},
	{0x3706,0x4b},
	{0x3708,0x84},
	{0x3709,0x40},
	{0x370a,0x31},
	{0x370e,0x00},
	{0x3711,0x0f},
	{0x3712,0x9c},
	{0x3724,0x01},
	{0x3725,0x92},
	{0x3726,0x01},
	{0x3727,0xa9},
	{0x3800,0x00},
	{0x3801,0x00},
	{0x3802,0x01},
	{0x3803,0x30},
	{0x3804,0x0c},
	{0x3805,0xdf},
	{0x3806,0x08},
	{0x3807,0x67},
	{0x3808,0x07},
	{0x3809,0x80},
	{0x380a,0x04},
	{0x380b,0x38},
	{0x380c,0x0d},
	{0x380d,0xf0},
	{0x380e,0x07},
	{0x380f,0x4c},
	{0x3810,0x00},
	{0x3811,0x10},
	{0x3812,0x00},
	{0x3813,0x06},
	{0x3814,0x11},
	{0x3815,0x11},
	{0x3820,0x80},
	{0x3821,0x16},
	{0x3f00,0x02},
	{0x4005,0x18},
	{0x404f,0x8F},
	{0x4600,0x04},
	{0x4601,0x01},
	{0x4602,0x00},
	{0x4837,0x28},
	{0x5068,0x53},
	{0x506a,0x53},
	{0x0100,0x01},
};

static struct regval_list sensor_sxga_regs[] = { 
	{0x0100,0x00},
	{REG_DLY,0x32},
	{0x3003,0xce},
	{0x3004,0xd4},
	{0x3005,0x00},
	{0x3006,0x10},
	{0x3007,0x3b},
	{0x3012,0x81},
	{0x3013,0x39},
	{0x3106,0x11},
	{0x3600,0x07},
	{0x3601,0x33},
	{0x3602,0xc2},
	{0x3700,0x10},
	{0x3702,0x28},
	{0x3703,0x6c},
	{0x3704,0x8d},
	{0x3705,0x0a},
	{0x3706,0x27},
	{0x3708,0x40},
	{0x3709,0x20},
	{0x370a,0x33},
	{0x370e,0x08},
	{0x3711,0x07},
	{0x3712,0x4e},
	{0x3724,0x00},
	{0x3725,0xd4},
	{0x3726,0x00},
	{0x3727,0xe1},
	{0x3800,0x00},
	{0x3801,0x00},
	{0x3802,0x00},
	{0x3803,0x00},
	{0x3804,0x0c},
	{0x3805,0xdf},
	{0x3806,0x09},
	{0x3807,0x9b},
	{0x3808,0x06},
	{0x3809,0x60},
	{0x380a,0x04},
	{0x380b,0xc8},
	{0x380c,0x0d},
	{0x380d,0xbc},
	{0x380e,0x04},
	{0x380f,0xf0},
	{0x3810,0x00},
	{0x3811,0x08},
	{0x3812,0x00},
	{0x3813,0x04},
	{0x3814,0x31},
	{0x3815,0x31},
	{0x3820,0x80},
	{0x3821,0x17},
	{0x3f00,0x00},
	{0x4005,0x18},
	{0x404f,0x8F},
	{0x4600,0x04},
	{0x4601,0x00},
	{0x4602,0x78},
	{0x4837,0x28},
	{0x5068,0x00},
	{0x506a,0x00},
	{0x0100,0x01},
};

static struct regval_list sensor_720p_regs[] = {
  {0x0100,0x00},
  {REG_DLY,0x32},
	{0x3003,0xce},
	{0x3004,0xc8},
	{0x3005,0x10},
	{0x3006,0x01},
	{0x3007,0x3b},
	{0x3012,0x80},
	{0x3013,0x39},
	{0x3106,0x15},
	{0x3600,0x06},
	{0x3601,0x34},
	{0x3602,0x42},
	{0x3700,0x20},
	{0x3702,0x50},
	{0x3703,0xcc},
	{0x3704,0x19},
	{0x3705,0x14},
	{0x3706,0x4b},
	{0x3708,0x84},
	{0x3709,0x40},
	{0x370a,0x33},
	{0x370e,0x08},
	{0x3711,0x0f},
	{0x3712,0x9c},
	{0x3724,0x01},
	{0x3725,0x92},
	{0x3726,0x01},
	{0x3727,0xa9},
	{0x3800,0x00},
	{0x3801,0x28},
	{0x3802,0x01},
	{0x3803,0x40},
	{0x3804,0x0c},
	{0x3805,0xb7},
	{0x3806,0x08},
	{0x3807,0x57},
	{0x3808,0x05},
	{0x3809,0x00},
	{0x380a,0x02},
	{0x380b,0xd0},
	{0x380c,0x0d},
	{0x380d,0xe0},
	{0x380e,0x03},
	{0x380f,0xa0},
	{0x3810,0x00},
	{0x3811,0x04},
	{0x3812,0x00},
	{0x3813,0x04},
	{0x3814,0x31},
	{0x3815,0x31},
	{0x3820,0x80},
	{0x3821,0x17},
	{0x3f00,0x00},
	{0x3f05,0x50},
	{0x4005,0x18},
	{0x404f,0x8F},
	{0x4600,0x14},
	{0x4601,0x14},
	{0x4602,0x00},
	{0x4837,0x1e},
	{0x5068,0x5A},
	{0x506a,0x5A},
	{0x0100,0x01},
};


static struct regval_list sensor_vga_regs[] = {
	{0x0100,0x00},
	{REG_DLY,0x32},
	{0x3003,0x8c},
	{0x3004,0xe3},
	{0x3005,0x10},
	{0x3006,0x00},
	{0x3007,0x3b},
	{0x3012,0x83},
	{0x3013,0x39},
	{0x3106,0x15},
	{0x3600,0x06},
	{0x3601,0x34},
	{0x3602,0xc2},
	{0x3700,0x20},
	{0x3702,0x50},
	{0x3703,0xcc},
	{0x3704,0x19},
	{0x3705,0x14},
	{0x3706,0x4b},
	{0x3708,0x84},
	{0x3709,0x40},
	{0x370a,0xf4},
	{0x370e,0x00},
	{0x3711,0x0f},
	{0x3712,0x9c},
	{0x3724,0x01},
	{0x3725,0x92},
	{0x3726,0x01},
	{0x3727,0xa9},
	{0x3800,0x00},
	{0x3801,0x08},
	{0x3802,0x00},
	{0x3803,0x00},
	{0x3804,0x0c},
	{0x3805,0xdf},
	{0x3806,0x09},
	{0x3807,0x9b},
	{0x3808,0x02},
	{0x3809,0x80},
	{0x380a,0x01},
	{0x380b,0xe0},
	{0x380c,0x0d},
	{0x380d,0xbc},
	{0x380e,0x02},
	{0x380f,0x78},
	{0x3810,0x00},
	{0x3811,0x0c},
	{0x3812,0x00},
	{0x3813,0x08},
	{0x3814,0x71},
	{0x3815,0x71},
	{0x370e,0x00},
	{0x3820,0x80},
	{0x3821,0x16},
	{0x3f00,0x00},
	{0x4005,0x18},
	{0x404f,0x8F},
	{0x4600,0x14},
	{0x4601,0x14},
	{0x4602,0x00},
	{0x4837,0x1e},
	{0x5068,0x59},
	{0x506a,0x5A},
	{0x0100,0x01},
};


/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 * 
 */

static struct regval_list sensor_fmt_raw[] = {

};

/*
 * Low-level register I/O.
 *
 */


/*
 * On most platforms, we'd rather do straight i2c I/O.
 */
static int sensor_read(struct v4l2_subdev *sd, unsigned short reg,
    unsigned char *value)
{
	int ret=0;
	int cnt=0;
	
  ret = cci_read_a16_d8(sd,reg,value);
  while(ret!=0&&cnt<2)
  {
  	ret = cci_read_a16_d8(sd,reg,value);
  	cnt++;
  }
  if(cnt>0)
  	vfe_dev_dbg("sensor read retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write(struct v4l2_subdev *sd, unsigned short reg,
    unsigned char value)
{
	int ret=0;
	int cnt=0;
	
  ret = cci_write_a16_d8(sd,reg,value);
  while(ret!=0&&cnt<2)
  {
  	ret = cci_write_a16_d8(sd,reg,value);
  	cnt++;
  }
  if(cnt>0)
  	vfe_dev_dbg("sensor write retry=%d\n",cnt);
  
  return ret;
}

/*
 * Write a list of register settings;
 */
static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
	int i=0;
	
  if(!regs)
  	return 0;
  
  while(i<array_size)
  {
    if(regs->addr == REG_DLY) {
      msleep(regs->data);
    } 
    else {
      LOG_ERR_RET(sensor_write(sd, regs->addr, regs->data))
    }
    i++;
    regs++;
  }
  return 0;
}

/* 
 * Code for dealing with controls.
 * fill with different sensor module
 * different sensor module has different settings here
 * if not support the follow function ,retrun -EINVAL
 */

/* *********************************************begin of ******************************************** */
/*
static int sensor_g_hflip(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  unsigned char rdval;
    
  LOG_ERR_RET(sensor_read(sd, 0x3821, &rdval))
  
  rdval &= (1<<1);
  rdval >>= 1;
    
  *value = rdval;

  info->hflip = *value;
  return 0;
}

static int sensor_s_hflip(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);
  unsigned char rdval;
  
  if(info->hflip == value)
    return 0;
    
  LOG_ERR_RET(sensor_read(sd, 0x3821, &rdval))
  
  switch (value) {
    case 0:
      rdval &= 0xf9;
      break;
    case 1:
      rdval |= 0x06;
      break;
    default:
      return -EINVAL;
  }
  
  LOG_ERR_RET(sensor_write(sd, 0x3821, rdval))
  
  mdelay(10);
  info->hflip = value;
  return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  unsigned char rdval;
  
  LOG_ERR_RET(sensor_read(sd, 0x3820, &rdval))
  
  rdval &= (1<<1);  
  *value = rdval;
  rdval >>= 1;
  
  info->vflip = *value;
  return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);
  unsigned char rdval;
  
  if(info->vflip == value)
    return 0;
  
  LOG_ERR_RET(sensor_read(sd, 0x3820, &rdval))

  switch (value) {
    case 0:
      rdval &= 0xf9;
      break;
    case 1:
      rdval |= 0x06;
      break;
    default:
      return -EINVAL;
  }

  LOG_ERR_RET(sensor_write(sd, 0x3820, rdval))
  
  mdelay(10);
  info->vflip = value;
  return 0;
}
*/

static int sensor_g_exp(struct v4l2_subdev *sd, __s32 *value)
{
	struct sensor_info *info = to_state(sd);
	
	*value = info->exp;
	vfe_dev_dbg("sensor_get_exposure = %d\n", info->exp);
	return 0;
}

static int sensor_s_exp_gain(struct v4l2_subdev *sd, struct sensor_exp_gain *exp_gain)
{
  int exp_val, gain_val,frame_length,shutter;
  unsigned char explow=0,expmid=0,exphigh=0;
  unsigned char gainlow=0,gainhigh=0;  
  struct sensor_info *info = to_state(sd);

  exp_val = exp_gain->exp_val;
  gain_val = exp_gain->gain_val;

  
  if(gain_val<1*16)
	  gain_val=16;
  if(gain_val>64*16-1)
	  gain_val=64*16-1;
  
  if(exp_val>0xfffff)
	  exp_val=0xfffff;
  
  gainlow=(unsigned char)(gain_val&0xff);
  gainhigh=(unsigned char)((gain_val>>8)&0x3);
  
  exphigh	= (unsigned char) ( (0x0f0000&exp_val)>>16);
  expmid	= (unsigned char) ( (0x00ff00&exp_val)>>8);
  explow	= (unsigned char) ( (0x0000f0&exp_val)	 );
  shutter = exp_val/16;  
  if(shutter  > ov8825_sensor_vts- 4)
        frame_length = shutter + 4;
  else
        frame_length = ov8825_sensor_vts;
  
  sensor_write(sd, 0x3208, 0x00);//enter group write
  
  sensor_write(sd, 0x3503, 0x17);
  
  sensor_write(sd, 0x380f, (frame_length & 0xff));
  sensor_write(sd, 0x380e, (frame_length >> 8));
  
  sensor_write(sd, 0x350b, gainlow);
  sensor_write(sd, 0x350a, gainhigh);
  
  sensor_write(sd, 0x3502, explow);
  sensor_write(sd, 0x3501, expmid);
  sensor_write(sd, 0x3500, exphigh);	
  sensor_write(sd, 0x3208, 0x10);//end group write
  sensor_write(sd, 0x3208, 0xa0);//init group write
  info->exp = exp_val;
  info->gain = gain_val;
  return 0;
}

static int sensor_s_exp(struct v4l2_subdev *sd, unsigned int exp_val)
{
	unsigned char explow,expmid,exphigh;
	struct sensor_info *info = to_state(sd);

	vfe_dev_dbg("sensor_set_exposure = %d\n", exp_val>>4);
	if(exp_val>0xfffff)
		exp_val=0xfffff;
	
  
    exphigh = (unsigned char) ( (0x0f0000&exp_val)>>16);
    expmid  = (unsigned char) ( (0x00ff00&exp_val)>>8);
    explow  = (unsigned char) ( (0x0000f0&exp_val)   );
	
	sensor_write(sd, 0x3208, 0x00);//enter group write
	sensor_write(sd, 0x3502, explow);
	sensor_write(sd, 0x3501, expmid);
	sensor_write(sd, 0x3500, exphigh);	
	
	info->exp = exp_val;
	return 0;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
	struct sensor_info *info = to_state(sd);
	
	*value = info->gain;
	vfe_dev_dbg("sensor_get_gain = %d\n", info->gain);
	return 0;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int gain_val)
{
	struct sensor_info *info = to_state(sd);
	unsigned char gainlow=0;
	unsigned char gainhigh=0;
	
	if(gain_val<1*16)
		gain_val=16;
	if(gain_val>64*16-1)
		gain_val=64*16-1;
	vfe_dev_dbg("sensor_set_gain = %d\n", gain_val);

	gainlow=(unsigned char)(gain_val&0xff);
	gainhigh=(unsigned char)((gain_val>>8)&0x3);
	
	
	sensor_write(sd, 0x3503, 0x17);
	sensor_write(sd, 0x350b, gainlow);
	sensor_write(sd, 0x350a, gainhigh);
	sensor_write(sd, 0x3208, 0x10);//end group write
	sensor_write(sd, 0x3208, 0xa0);//init group write
	
	printk("8825 sensor_set_gain = %d, Done!\n", gain_val);
	info->gain = gain_val;
	
	return 0;
}


static int sensor_s_sw_stby(struct v4l2_subdev *sd, int on_off)
{
	int ret;
	unsigned char rdval;
	
	ret=sensor_read(sd, 0x0100, &rdval);
	if(ret!=0)
		return ret;
	
	if(on_off==CSI_STBY_ON)//sw stby on
	{
		ret=sensor_write(sd, 0x0100, rdval&0xfe);
	}
	else//sw stby off
	{
		ret=sensor_write(sd, 0x0100, rdval|0x01);
	}
	return ret;
}

/*
 * Stuff that knows about the sensor.
 */
 
static int sensor_power(struct v4l2_subdev *sd, int on)
{
  int ret;
  
  ret = 0;
  switch(on)
  {
    case CSI_SUBDEV_STBY_ON:
      vfe_dev_dbg("CSI_SUBDEV_STBY_ON!\n");
      ret = sensor_s_sw_stby(sd, CSI_STBY_ON);
      if(ret < 0)
        vfe_dev_err("soft stby falied!\n");
      mdelay(10);
      cci_lock(sd);
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      cci_unlock(sd);  
      vfe_set_mclk(sd,OFF);
      break;
    case CSI_SUBDEV_STBY_OFF:
      vfe_dev_dbg("CSI_SUBDEV_STBY_OFF!\n");
      cci_lock(sd);    
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      mdelay(10);
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      mdelay(10);
      cci_unlock(sd);        
      break;
    case CSI_SUBDEV_PWR_ON:
      vfe_dev_dbg("CSI_SUBDEV_PWR_ON!\n");
      cci_lock(sd);
      vfe_gpio_set_status(sd,PWDN,1);//set the gpio to output
      vfe_gpio_set_status(sd,RESET,1);//set the gpio to output
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      mdelay(1);
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      mdelay(10);
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_ON);
      vfe_set_pmu_channel(sd,IOVDD,ON);
      vfe_set_pmu_channel(sd,AVDD,ON);
      vfe_set_pmu_channel(sd,DVDD,ON);
      vfe_set_pmu_channel(sd,AFVDD,ON);
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      mdelay(10);
      vfe_gpio_write(sd,RESET,CSI_RST_OFF);
      mdelay(30);
      cci_unlock(sd);  
      break;
    case CSI_SUBDEV_PWR_OFF:
      vfe_dev_dbg("CSI_SUBDEV_PWR_OFF!\n");
      cci_lock(sd);
      vfe_set_mclk(sd,OFF);
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_OFF);
      vfe_set_pmu_channel(sd,AFVDD,OFF);
      vfe_set_pmu_channel(sd,DVDD,OFF);
      vfe_set_pmu_channel(sd,AVDD,OFF);
      vfe_set_pmu_channel(sd,IOVDD,OFF);  
      mdelay(10);
      vfe_gpio_write(sd,POWER_EN,CSI_STBY_OFF);
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      vfe_gpio_set_status(sd,RESET,0);//set the gpio to input
      vfe_gpio_set_status(sd,PWDN,0);//set the gpio to input
      cci_unlock(sd);  
      break;
    default:
      return -EINVAL;
  }   

  return 0;
}
 
static int sensor_reset(struct v4l2_subdev *sd, u32 val)
{
  switch(val)
  {
    case 0:
      vfe_gpio_write(sd,RESET,CSI_RST_OFF);
      mdelay(10);
      break;
    case 1:
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      mdelay(10);
      break;
    default:
      return -EINVAL;
  }
    
  return 0;
}

static int sensor_detect(struct v4l2_subdev *sd)
{
  unsigned char rdval;
  
  LOG_ERR_RET(sensor_read(sd, 0x300a, &rdval))
  if(rdval != 0x88)
    return -ENODEV;
  	
  LOG_ERR_RET(sensor_read(sd, 0x300b, &rdval))
  if(rdval != 0x25)
    return -ENODEV;
  
  return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
  int ret;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_init\n");
  
  /*Make sure it is a target sensor*/
  ret = sensor_detect(sd);
  if (ret) {
    vfe_dev_err("chip found is not an target chip.\n");
    return ret;
  }
  
  vfe_get_standby_mode(sd,&info->stby_mode);
  
  if((info->stby_mode == HW_STBY || info->stby_mode == SW_STBY) \
      && info->init_first_flag == 0) {
    vfe_dev_print("stby_mode and init_first_flag = 0\n");
    return 0;
  } 
  
  info->focus_status = 0;
  info->low_speed = 0;
  info->width = QUXGA_WIDTH;
  info->height = QUXGA_HEIGHT;
  info->hflip = 0;
  info->vflip = 0;
  info->gain = 0;

  info->tpf.numerator = 1;            
  info->tpf.denominator = 15;    /* 30fps */    
  
  ret = sensor_write_array(sd, sensor_default_regs, ARRAY_SIZE(sensor_default_regs));  
  if(ret < 0) {
    vfe_dev_err("write sensor_default_regs error\n");
    return ret;
  }
  
  if(info->stby_mode == 0)
    info->init_first_flag = 0;
  
  info->preview_first_flag = 1;
  
  return 0;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
  int ret=0;
  struct sensor_info *info = to_state(sd);
  switch(cmd) {
    case GET_CURRENT_WIN_CFG:
      if(info->current_wins != NULL)
      {
        memcpy( arg,
                info->current_wins,
                sizeof(struct sensor_win_size) );
        ret=0;
      }
      else
      {
        vfe_dev_err("empty wins!\n");
        ret=-1;
      }
      break;
    case SET_FPS:
      ret=0;
      break;
    case ISP_SET_EXP_GAIN:
		ret = sensor_s_exp_gain(sd, (struct sensor_exp_gain *)arg);
      break;
    default:
      return -EINVAL;
  }
  return ret;
}


/*
 * Store information about the video data format. 
 */
static struct sensor_format_struct {
  __u8 *desc;
  enum v4l2_mbus_pixelcode mbus_code;
  struct regval_list *regs;
  int regs_size;
  int bpp;   /* Bytes per pixel */
}sensor_formats[] = {
	{
		.desc				= "Raw RGB Bayer",
		.mbus_code	= V4L2_MBUS_FMT_SBGGR10_10X1,//V4L2_MBUS_FMT_SGRBG10_10X1,
		.regs 			= sensor_fmt_raw,
		.regs_size 	= ARRAY_SIZE(sensor_fmt_raw),
		.bpp				= 1
	},
};
#define N_FMTS ARRAY_SIZE(sensor_formats)

  

/*
 * Then there is the issue of window sizes.  Try to capture the info here.
 */


static struct sensor_win_size sensor_win_sizes[] = {
	  /* quxga: 3264*2448 */
	  {
      .width      = QUXGA_WIDTH,//3280,
      .height     = QUXGA_HEIGHT,//2464,
      .hoffset    = 0,
      .voffset    = 0,
      .hts        = 3584,
      .vts        = 2480,
      .pclk       = 134*1000*1000,
      .mipi_bps	  = 656*1000*1000,
      .fps_fixed  = 2,
      .bin_factor = 1,
      .intg_min   = 16,
      .intg_max   = (2480-4)<<4,
      .gain_min   = 1<<4,
      .gain_max   = 12<<4,
      .regs       = sensor_quxga_regs,
      .regs_size  = ARRAY_SIZE(sensor_quxga_regs),
      .set_size   = NULL,
    },
    
    /* 1080P */
    {
      .width	  = HD1080_WIDTH,
      .height 	  = HD1080_HEIGHT,
      .hoffset    = 0,
      .voffset    = 0,
      .hts        = 3568,
      .vts        = 1868,
      .pclk       = 200*1000*1000,
      .mipi_bps   = 720*1000*1000,
      .fps_fixed  = 1,
      .bin_factor = 1,
      .intg_min   = 1<<4,
      .intg_max   = (1868-4)<<4,
      .gain_min   = 1<<4,
      .gain_max   = 12<<4,
      .regs       = sensor_1080p_regs,//
      .regs_size  = ARRAY_SIZE(sensor_1080p_regs),//
      .set_size		= NULL,
    },

  	/* SXGA */
    {
      .width	  = SXGA_WIDTH,
      .height 	  = SXGA_HEIGHT,
      .hoffset	  = 176,
      .voffset	  = 132,
      .hts        = 3516,
      .vts        = 1264,
      .pclk       = 133*1000*1000,
	  .mipi_bps   = 720*1000*1000,
      .fps_fixed  = 1,
      .bin_factor = 1,
      .intg_min   = 1,
      .intg_max   = (1264-4)<<4,
      .gain_min   = 1<<4,
      .gain_max   = 12<<4,
      .regs		    = sensor_sxga_regs,
      .regs_size	= ARRAY_SIZE(sensor_sxga_regs),
      .set_size		= NULL,
    },

    /* 720p */
    {
      .width      = HD720_WIDTH,
      .height     = HD720_HEIGHT,
      .hoffset    = 0,
      .voffset    = 0,
      .hts        = 3552,
      .vts        = 928,
      .pclk       = 99*1000*1000,
      .mipi_bps	  = 456*1000*1000,
      .fps_fixed  = 1,
      .bin_factor = 1,
      .intg_min   = 16,
      .intg_max   = (928-4)<<4,
      .gain_min   = 1<<4,
      .gain_max   = 12<<4,
      .regs		  = sensor_720p_regs,//
      .regs_size  = ARRAY_SIZE(sensor_720p_regs),//
      .set_size	  = NULL,
    },

  /* VGA */
    {
      .width	  = VGA_WIDTH,
      .height 	  = VGA_HEIGHT,
      .hoffset	  = 0,
      .voffset	  = 0,
      .hts        = 3504,//limited by sensor
      .vts        = 634,
      .pclk       = 67*1000*1000,//67
	  .mipi_bps   = 240*1000*1000,
      .fps_fixed  = 1,
      .bin_factor = 1,
	  .intg_min   = 16,
	  .intg_max   = (634-4)<<4,
	  .gain_min   = 1<<4,
	  .gain_max   = 12<<4,
      .regs       = sensor_vga_regs,
      .regs_size  = ARRAY_SIZE(sensor_vga_regs),
      .set_size   = NULL,
    },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt(struct v4l2_subdev *sd, unsigned index,
                 enum v4l2_mbus_pixelcode *code)
{
  if (index >= N_FMTS)
    return -EINVAL;

  *code = sensor_formats[index].mbus_code;
  return 0;
}

static int sensor_enum_size(struct v4l2_subdev *sd,
                            struct v4l2_frmsizeenum *fsize)
{
  if(fsize->index > N_WIN_SIZES-1)
  	return -EINVAL;
  
  fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
  fsize->discrete.width = sensor_win_sizes[fsize->index].width;
  fsize->discrete.height = sensor_win_sizes[fsize->index].height;
  
  return 0;
}


static int sensor_try_fmt_internal(struct v4l2_subdev *sd,
    struct v4l2_mbus_framefmt *fmt,
    struct sensor_format_struct **ret_fmt,
    struct sensor_win_size **ret_wsize)
{
  int index;
  struct sensor_win_size *wsize;
  struct sensor_info *info = to_state(sd);

  for (index = 0; index < N_FMTS; index++)
    if (sensor_formats[index].mbus_code == fmt->code)
      break;

  if (index >= N_FMTS) 
    return -EINVAL;
  
  if (ret_fmt != NULL)
    *ret_fmt = sensor_formats + index;
    
  /*
   * Fields: the sensor devices claim to be progressive.
   */
  
  fmt->field = V4L2_FIELD_NONE;
  
  /*
   * Round requested image size down to the nearest
   * we support, but not below the smallest.
   */
  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES;
       wsize++)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height)
      break;
    
  if (wsize >= sensor_win_sizes + N_WIN_SIZES)
    wsize--;   /* Take the smallest one */
  if (ret_wsize != NULL)
    *ret_wsize = wsize;
  
  info->current_wins = wsize;  
    
  /*
   * Note the size we'll actually handle.
   */
  fmt->width = wsize->width;
  fmt->height = wsize->height;

  return 0;
}

static int sensor_try_fmt(struct v4l2_subdev *sd, 
             struct v4l2_mbus_framefmt *fmt)
{
  return sensor_try_fmt_internal(sd, fmt, NULL, NULL);
}

static int sensor_g_mbus_config(struct v4l2_subdev *sd,
           struct v4l2_mbus_config *cfg)
{
  cfg->type = V4L2_MBUS_CSI2;
  cfg->flags = 0|V4L2_MBUS_CSI2_2_LANE|V4L2_MBUS_CSI2_CHANNEL_0;
  
  return 0;
}


/*
 * Set a format.
 */
static int sensor_s_fmt(struct v4l2_subdev *sd, 
             struct v4l2_mbus_framefmt *fmt)
{
  int ret;
     
  struct sensor_format_struct *sensor_fmt;
  struct sensor_win_size *wsize;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_s_fmt\n");
  
  ret = sensor_try_fmt_internal(sd, fmt, &sensor_fmt, &wsize);
  if (ret)
    return ret;

  if(info->capture_mode == V4L2_MODE_VIDEO)
  {
  }
  else if(info->capture_mode == V4L2_MODE_IMAGE)
  {
    
  }

  sensor_write_array(sd, sensor_fmt->regs, sensor_fmt->regs_size);

  ret = 0;
  if (wsize->regs)
    LOG_ERR_RET(sensor_write_array(sd, wsize->regs, wsize->regs_size))
  
  if (wsize->set_size)
    LOG_ERR_RET(wsize->set_size(sd))

  info->fmt = sensor_fmt;
  info->width = wsize->width;
  info->height = wsize->height;
  ov8825_sensor_vts = wsize->vts;

  vfe_dev_print("s_fmt = %x, width = %d, height = %d\n",sensor_fmt->mbus_code,wsize->width,wsize->height);

  if(info->capture_mode == V4L2_MODE_VIDEO)
  {
   
  } else {

  }
	
	vfe_dev_print("s_fmt end\n");
	return 0;
}

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int sensor_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
  struct v4l2_captureparm *cp = &parms->parm.capture;
  struct sensor_info *info = to_state(sd);

  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    return -EINVAL;
  
  memset(cp, 0, sizeof(struct v4l2_captureparm));
  cp->capability = V4L2_CAP_TIMEPERFRAME;
  cp->capturemode = info->capture_mode;
     
  return 0;
}

static int sensor_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
  struct v4l2_captureparm *cp = &parms->parm.capture;
  struct sensor_info *info = to_state(sd);
  
  vfe_dev_dbg("sensor_s_parm\n");
  
  if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    return -EINVAL;
  
  if (info->tpf.numerator == 0)
    return -EINVAL;
    
  info->capture_mode = cp->capturemode;
  
  return 0;
}


static int sensor_queryctrl(struct v4l2_subdev *sd,
    struct v4l2_queryctrl *qc)
{
  /* Fill in min, max, step and default value for these controls. */
  /* see include/linux/videodev2.h for details */
  
  switch (qc->id) {
	case V4L2_CID_GAIN:
		return v4l2_ctrl_query_fill(qc, 1*16, 32*16, 1, 16);
	case V4L2_CID_EXPOSURE:
		return v4l2_ctrl_query_fill(qc, 0, 65535*16, 1, 0);
  }
  return -EINVAL;
}

static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  switch (ctrl->id) {
  case V4L2_CID_GAIN:
    return sensor_g_gain(sd, &ctrl->value);
  case V4L2_CID_EXPOSURE:
  	return sensor_g_exp(sd, &ctrl->value);
  }
  return -EINVAL;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  struct v4l2_queryctrl qc;
  int ret;
  
  qc.id = ctrl->id;
  ret = sensor_queryctrl(sd, &qc);
  if (ret < 0) {
    return ret;
  }

  if (ctrl->value < qc.minimum || ctrl->value > qc.maximum) {
    return -ERANGE;
  }
  
  switch (ctrl->id) {
    case V4L2_CID_GAIN:
      return sensor_s_gain(sd, ctrl->value);
    case V4L2_CID_EXPOSURE:
	  return sensor_s_exp(sd, ctrl->value);
  }
  return -EINVAL;
}


static int sensor_g_chip_ident(struct v4l2_subdev *sd,
    struct v4l2_dbg_chip_ident *chip)
{
  struct i2c_client *client = v4l2_get_subdevdata(sd);

  return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_SENSOR, 0);
}


/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_core_ops sensor_core_ops = {
  .g_chip_ident = sensor_g_chip_ident,
  .g_ctrl = sensor_g_ctrl,
  .s_ctrl = sensor_s_ctrl,
  .queryctrl = sensor_queryctrl,
  .reset = sensor_reset,
  .init = sensor_init,
  .s_power = sensor_power,
  .ioctl = sensor_ioctl,
};

static const struct v4l2_subdev_video_ops sensor_video_ops = {
  .enum_mbus_fmt = sensor_enum_fmt,
  .enum_framesizes = sensor_enum_size,
  .try_mbus_fmt = sensor_try_fmt,
  .s_mbus_fmt = sensor_s_fmt,
  .s_parm = sensor_s_parm,
  .g_parm = sensor_g_parm,
  .g_mbus_config = sensor_g_mbus_config,
};

static const struct v4l2_subdev_ops sensor_ops = {
  .core = &sensor_core_ops,
  .video = &sensor_video_ops,
};

/* ----------------------------------------------------------------------- */
static struct cci_driver cci_drv = {
	.name = SENSOR_NAME,
};

static int sensor_probe(struct i2c_client *client,
      const struct i2c_device_id *id)
{
  struct v4l2_subdev *sd;
  struct sensor_info *info;

  info = kzalloc(sizeof(struct sensor_info), GFP_KERNEL);
  if (info == NULL)
    return -ENOMEM;
  sd = &info->sd;
  glb_sd = sd;
  cci_dev_probe_helper(sd, client, &sensor_ops, &cci_drv);

  info->fmt = &sensor_formats[0];
  info->af_first_flag = 1;
  info->init_first_flag = 1;

  return 0;
}


static int sensor_remove(struct i2c_client *client)
{
  struct v4l2_subdev *sd;
  sd = cci_dev_remove_helper(client, &cci_drv);
  kfree(to_state(sd));
  return 0;
}

static const struct i2c_device_id sensor_id[] = {
  { SENSOR_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE(i2c, sensor_id);


static struct i2c_driver sensor_driver = {
  .driver = {
    .owner = THIS_MODULE,
  .name = SENSOR_NAME,
  },
  .probe = sensor_probe,
  .remove = sensor_remove,
  .id_table = sensor_id,
};
static __init int init_sensor(void)
{
	return cci_dev_init_helper(&sensor_driver);
}

static __exit void exit_sensor(void)
{
	cci_dev_exit_helper(&sensor_driver);
}

module_init(init_sensor);
module_exit(exit_sensor);

