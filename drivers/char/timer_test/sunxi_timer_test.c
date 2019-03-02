/*
 * drivers/char/timer_test/sunxi_timer_test.c
 *
 * Copyright(c) 2013-2015 Allwinnertech Co., Ltd.
 *      http://www.allwinnertech.com
 *
 * Author: liugang <liugang@allwinnertech.com>
 *
 * sunxi timer test driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sunxi_timer_test_i.h"
#include <linux/sunxi_timer_test.h>

/* rtc_dev_prepare */
#define DEV_NAME 		"timer_test_chrdev"

u32 	g_loopcnt[2] = {0}; /* 0 for timer, 1 for hrtimer, so as g_cur_cnt */
static atomic_t 	g_cur_cnt[2] = {ATOMIC_INIT(0), ATOMIC_INIT(0)};
struct timer_list 	g_timer;

static dev_t 	g_devid ;
static struct cdev	*g_ptimer_cdev = NULL;	/* in /proc/devices */
static ktime_t 		g_ktime;
static struct class *g_timer_class = NULL;

static void timer_handle(unsigned long arg)
{
	unsigned long ms = arg;
	struct timeval tv_cur;

	if(atomic_add_return(1, &g_cur_cnt[0]) >= g_loopcnt[0]) {
		/* print cur time in sec */
		do_gettimeofday(&tv_cur);
		printk("%s: cur sec %d\n", __func__, (int)tv_cur.tv_sec);

		/* clear g_cur_cnt[0] */
		atomic_set(&g_cur_cnt[0], 0);
	}

	/* set next trig */
	mod_timer(&g_timer, jiffies + (HZ * ms) / 1000);
}

u32 case_timer_function(u32 interv_ms, u32 print_gap_s, u32 total_s)
{
	int itemp = -1;
	struct timeval tv_start, tv_cur;

	printk("%s: interv_ms %d, print_gap_s %d, total_s %d\n", __func__, interv_ms, print_gap_s, total_s);

	g_loopcnt[0] = (print_gap_s * 1000) / interv_ms;
	atomic_set(&g_cur_cnt[0], 0);

	/* init and add timer */
	init_timer(&g_timer);
	g_timer.function 	= &timer_handle;
	g_timer.data		= interv_ms;
	g_timer.expires 	= jiffies + (HZ * interv_ms) / 1000;

	add_timer(&g_timer);

	/* wait for total_s */
	do_gettimeofday(&tv_start);
	printk("%s: start sec %d\n", __func__, (int)tv_start.tv_sec);
	while(do_gettimeofday(&tv_cur), tv_cur.tv_sec - tv_start.tv_sec <= total_s)
		msleep_interruptible(0);

	/* del timer */
	printk("%s: before del_timer_sync\n", __func__);
	itemp = del_timer_sync(&g_timer);
	printk("%s: after del_timer_sync(return %d)\n", __func__, itemp);

	return 0;
}

static struct hrtimer 	g_hrtimer;

static enum hrtimer_restart hrtimer_handle(struct hrtimer *cur_timer)
{
	struct timeval tv_cur;

	if(atomic_add_return(1, &g_cur_cnt[1]) >= g_loopcnt[1]) {
		/* print cur time in sec */
		do_gettimeofday(&tv_cur);
		printk("%s: cur sec %d\n", __func__, (int)tv_cur.tv_sec);

		/* clear g_cur_cnt[1] */
		atomic_set(&g_cur_cnt[1], 0);
	}

	/* if not call this, hrtimer_handle will called again and again */
	hrtimer_forward(cur_timer, cur_timer->base->get_time(), g_ktime);
	return HRTIMER_RESTART;
}

u32 case_hrtimer_function(u32 interv_us, u32 print_gap_s, u32 total_s)
{
	int 	itemp = -1;
	struct timeval tv_start, tv_cur;

	printk("%s: interv_us %d, print_gap_s %d, total_s %d\n", __func__, interv_us, print_gap_s, total_s);

	g_loopcnt[1] = (print_gap_s * 1000000) / interv_us;
	atomic_set(&g_cur_cnt[1], 0);
	printk("%s: g_loopcnt %d\n", __func__, g_loopcnt[1]);

	/* init hrtimer */
	g_ktime = ktime_set(0, interv_us * 1000);
	hrtimer_init(&g_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	g_hrtimer.function = &hrtimer_handle;

	/* start hrtimer */
	itemp = hrtimer_start(&g_hrtimer, g_ktime, HRTIMER_MODE_REL);
	printk("%s: hrtimer_start return %d\n", __func__, itemp);

	/* wait for total_s */
	do_gettimeofday(&tv_start);
	printk("%s: start sec %d\n", __func__, (int)tv_start.tv_sec);
	while(do_gettimeofday(&tv_cur), tv_cur.tv_sec - tv_start.tv_sec <= total_s)
		msleep_interruptible(0);

	/* del httimer */
	itemp = hrtimer_cancel(&g_hrtimer);
	printk("%s: hrtimer_cancel return %d\n", __func__, itemp);

	return 0;
}

static int timer_test_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int timer_test_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long timer_test_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long 	ret = 0;
	struct timer_test_para para;

	memset(&para, 0, sizeof(para));
	if(0 != copy_from_user(&para, (struct timer_test_para *)arg, sizeof(struct timer_test_para))) {
		pr_err("%s(%d) err\n", __func__, __LINE__);
		return __LINE__;
	}

	switch(cmd) {
	case TIMER_TEST_CMD_FUNC_NORMAL:
		ret = case_timer_function(para.timer_interv_us / 1000, para.print_gap_s, para.total_test_s);
		break;
	case TIMER_TEST_CMD_FUNC_HRTIMER:
		ret = case_hrtimer_function(para.timer_interv_us, para.print_gap_s, para.total_test_s);
		break;
	default:
		break;
	}

	return ret;
}

static struct file_operations timer_test_fops = {
	.owner			= THIS_MODULE,
	.open			= timer_test_open,
	.release		= timer_test_release,
	.unlocked_ioctl		= timer_test_ioctl,
};

static int timer_test_cdev_init(void)
{
	int 	itemp = 0;

	itemp = alloc_chrdev_region(&g_devid, 0, 1, DEV_NAME);
	if(itemp) {
		printk("%s err, line %d\n", __func__, __LINE__);
		return -1;
	}

	g_ptimer_cdev = cdev_alloc();
	if(NULL == g_ptimer_cdev) {
		printk("%s err, line %d\n", __func__, __LINE__);
		goto err2;
	}

	cdev_init(g_ptimer_cdev, &timer_test_fops);

	g_ptimer_cdev->owner = THIS_MODULE;
	itemp = cdev_add(g_ptimer_cdev, g_devid, 1);
	if(itemp < 0) {
		printk("%s err, line %d\n", __func__, __LINE__);
		goto err1;
	}

	g_timer_class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(g_timer_class)) {
		printk("%s err, line %d\n", __func__, __LINE__);
		goto err1;
	}
	device_create(g_timer_class, NULL, g_devid, 0, DEV_NAME);

	printk("%s success\n", __func__);
	return 0;

err1:
	cdev_del(g_ptimer_cdev);
	g_ptimer_cdev = NULL;
err2:
	unregister_chrdev_region(g_devid, 1);
	return -1;
}

static void timer_test_cdev_deinit(void)
{
	device_destroy(g_timer_class, g_devid);
	class_destroy(g_timer_class);

	cdev_del(g_ptimer_cdev);

	unregister_chrdev_region(g_devid, 1);
}

/**
 * sunxi_timer_test_init - enter the timer test module
 */
static int __init sunxi_timer_test_init(void)
{
	int 	itemp = 0;

	printk("%s enter\n", __func__);

	itemp = timer_test_cdev_init();
	if(itemp < 0) {
		printk("%s err, line %d\n", __func__, __LINE__);
		return -1;
	}

	printk("%s success\n", __func__);
	return 0;
}

/**
 * sunxi_timer_test_exit - exit the timer test module
 */
static void __exit sunxi_timer_test_exit(void)
{
	printk("%s enter\n", __func__);

	timer_test_cdev_deinit();
}

module_init(sunxi_timer_test_init);
module_exit(sunxi_timer_test_exit);
MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("liugang");
MODULE_DESCRIPTION ("sunxi timer test driver code");

