
#ifndef  _DRV_TV_AC200_H_
#define  _DRV_TV_AC200_H_
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "asm-generic/int-ll64.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>  
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <mach/sys_config.h>
#include <mach/platform.h>
#include "../disp/disp_sys_intf.h"
#include <linux/mfd/acx00-mfd.h>

#include <video/sunxi_display2.h>

enum hpd_status
{
	STATUE_CLOSE = 0,
	STATUE_OPEN  = 1,

};

struct ac200_tv_priv {
	struct acx00 *acx00;
	struct mutex mlock;
};

extern struct ac200_tv_priv tv_priv;
extern disp_video_timings tv_video_timing[];
#endif