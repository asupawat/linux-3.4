/*
 * drivers/devfreq/dramfreq/sunxi-mdfs.h
 *
 * Copyright(c) 2013-2015 Allwinnertech Co., Ltd.
 *
 * Author: Pan Nan <pannan@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __MDFS_H__
#define __MDFS_H__

#include <asm/memory.h>
#include <asm/sizes.h>

#ifdef CONFIG_ARCH_SUN9IW1P1
#define SRAM_DDRFREQ_SIZE       (SZ_16K)

/* register define */
#define MCTL_COM_BASE           (0xf1c62000)
#define MCTL_CTL_BASE           (0xf1c63000)
#define MCTL_PHY_BASE           (0xf1c65000)
#define CCM_PLL_BASE            (0xf6000000)
#define CCM_MOD_BASE            (0xf6000400)
#define UART_BASE               (0xf7000000)
#define R_PRCM_BASE             (0xf8001400)
#define CNT64_CTRL_REG          (0xf8001c00)

#define MC_CR                   (MCTL_COM_BASE  +  0x00)
#define MC_CCR                  (MCTL_COM_BASE  +  0x04)
#define MC_RMCR                 (MCTL_COM_BASE  +  0x10)
#define M0_STATR                (MCTL_CTL_BASE  + 0x004)
#define M0_MRCTRL0              (MCTL_CTL_BASE  + 0x010)
#define M0_PWRCTL               (MCTL_CTL_BASE  + 0x030)
#define M0_RFSHCTL3             (MCTL_CTL_BASE  + 0x060)
#define M0_RFSHTMG              (MCTL_CTL_BASE  + 0x064)
#define M0_RANKCTL              (MCTL_CTL_BASE  + 0x0f4)
#define M0_ODTMAP               (MCTL_CTL_BASE  + 0x244)
#define M0_DCMDAPC              (MCTL_CTL_BASE  + 0x304)
#define P0_PIR                  (MCTL_PHY_BASE  + 0x004)
#define P0_PGCR0                (MCTL_PHY_BASE  + 0x008)
#define P0_PGCR3                (MCTL_PHY_BASE  + 0x014)
#define P0_PGSR0                (MCTL_PHY_BASE  + 0x018)
#define P0_PLLGCR               (MCTL_PHY_BASE  + 0x020)
#define P0_PTR1                 (MCTL_PHY_BASE  + 0x028)
#define P0_MR1                  (MCTL_PHY_BASE  + 0x0a0)
#define P0_DTCR                 (MCTL_PHY_BASE  + 0x0B0)
#define P0_DSGCR                (MCTL_PHY_BASE  + 0x084)
#define P0_DX0LCDLR2            (MCTL_PHY_BASE  + 0x2C0)
#define P0_DX0GTR               (MCTL_PHY_BASE  + 0x2C8)
#define P0_DX0MDLR              (MCTL_PHY_BASE  + 0x2C4)
#define M0_DRAMTMG8             (MCTL_CTL_BASE  + 0x120)
#define P0_ZQ0CR                (MCTL_PHY_BASE  + 0x240)

#define CCM_PLL6_DDR_REG        (CCM_PLL_BASE   + 0x014)
#define CCM_DRAMCLK_CFG_REG     (CCM_MOD_BASE   + 0x084)
#define CCM_AHB0_SOFT_RST_REG   (CCM_MOD_BASE   + 0x1a0)
#define CCM_AHB0_CLK_GAT_REG    (CCM_MOD_BASE   + 0x180)
#define SUART_THR               (UART_BASE      +  0x00)
#define SUART_USR               (UART_BASE      +  0x7c)
#define VDD_SYS_POFF_GATING     (R_PRCM_BASE    + 0x110)
#define CNT64_LOW_REG           (CNT64_CTRL_REG +  0x04)
#define CNT64_HIGH_REG          (CNT64_CTRL_REG +  0x08)

#elif defined(CONFIG_ARCH_SUN8IW3P1)
#define SRAM_DDRFREQ_SIZE       (0x1560)

/* register define */
#define MCTL_COM_BASE           (0xf1c62000)
#define MCTL_CTL_BASE           (0xf1c63000)
#define MCTL_PHY_BASE           (0xf1c65000)
#define SUNXI_CCM_VBASE         (0xf1c20000)
#define SUNXI_R_PRCM_VBASE      (0xf1f01400)
#define SUNXI_R_CPUCFG_VBASE    (0xf1f01c00)
#define SUART_BASE_VA           (0xf1c28000)

#define MC_CCR                  (MCTL_COM_BASE + 0x04)
#define MX_MSTR                 (MCTL_CTL_BASE + 0x00)
#define MX_STATR                (MCTL_CTL_BASE + 0x04)
#define MX_PWRCTL               (MCTL_CTL_BASE + 0x30)
#define MX_PWRTMG               (MCTL_CTL_BASE + 0x34)
#define MX_RFSHCTL3             (MCTL_CTL_BASE + 0x60)
#define MX_RFSHTMG              (MCTL_CTL_BASE + 0x64)
#define MX_ODTMAP               (MCTL_CTL_BASE + 0x244)
#define MX_DBG                  (MCTL_CTL_BASE + 0x304)
#define MP_PIR                  (MCTL_PHY_BASE + 0x04)
#define MP_PGCR(x)              (MCTL_PHY_BASE + 0x08 + 0x4*(x))
#define MP_PGSR(x)              (MCTL_PHY_BASE + 0x10 + 0x4*(x))
#define MP_DLLGCR               (MCTL_PHY_BASE + 0x18)
#define MP_ODTCR                (MCTL_PHY_BASE + 0x64)
#define MP_DXGCR(x)             (MCTL_PHY_BASE + 0x1C0 + 0x40*(x))
#define MP_DXMDLR(x)            (MCTL_PHY_BASE + 0x1EC + 0x40*(x))
#define MP_DXGTR(x)             (MCTL_PHY_BASE + 0x1F0 + 0x40*(x))
#define CCM_PLL_DDR_CTRL        (SUNXI_CCM_VBASE + 0x020)
#define VDD_SYS_PWROFF_GATING   (SUNXI_R_PRCM_VBASE + 0x110)
#define CNT64_CTRL_REG          (SUNXI_R_CPUCFG_VBASE + 0x280)
#define CNT64_LOW_REG           (SUNXI_R_CPUCFG_VBASE + 0x284)
#define CNT64_HIGH_REG          (SUNXI_R_CPUCFG_VBASE + 0x288)
#define SUART_THR               (SUART_BASE_VA + 0x00)
#define SUART_USR               (SUART_BASE_VA + 0x7c)

#elif defined(CONFIG_ARCH_SUN8IW5P1)
#define SRAM_DDRFREQ_SIZE           (SZ_8K)

/* register define */
#define MCTL_COM_BASE               (0xf1c62000)
#define MCTL_CTL_BASE               (0xf1c63000)
#define CCM_PLL_BASE                (0xf1c20000)
#define UART_BASE                   (0xf1c28000)
#define R_PRCM_BASE                 (0xf1f01400)
#define R_CPU_CFG_REG               (0xf1f01c00)

#define MC_WORK_MODE                (MCTL_COM_BASE  +  0x00)
#define MC_MAER                     (MCTL_COM_BASE  +  0x94)
#define MC_MDFSCR                   (MCTL_COM_BASE  + 0x100)
#define MC_MDFSMER                  (MCTL_COM_BASE  + 0x104)
#define MC_MDFSMRMR                 (MCTL_COM_BASE  + 0x108)
#define PIR                         (MCTL_CTL_BASE  +  0x00)
#define MC_CLKEN                    (MCTL_CTL_BASE  +  0x0c)
#define PWRCTL                      (MCTL_CTL_BASE  +  0x04)
#define PGSR0                       (MCTL_CTL_BASE  +  0x10)
#define STATR                       (MCTL_CTL_BASE  +  0x18)
#define PLLGCR                      (MCTL_CTL_BASE  +  0x40)
#define PTR1                        (MCTL_CTL_BASE  +  0x48)
#define RFSHTMG                     (MCTL_CTL_BASE  +  0x90)
#define DTCR                        (MCTL_CTL_BASE  +  0xc0)
#define ODTMAP                      (MCTL_CTL_BASE  + 0x120)
#define DXnGCR0(x)                  (MCTL_CTL_BASE  +  0x344 + 0x80*(x))
#define _CCM_PLL_DDR0_REG           (CCM_PLL_BASE   +  0x20)
#define _CCM_PLL_DDR1_REG           (CCM_PLL_BASE   +  0x4C)
#define CCM_DRAM_CFG_REG            (CCM_PLL_BASE   +  0xf4)
#define _CCM_PLL_DDR_CFG_REG        (CCM_PLL_BASE   +  0xf8)
#define _CCM_PLL_DDR1_PATTERN_REG   (CCM_PLL_BASE   + 0x2ac)
#define _CCM_PLL_DDR0_PATTERN_REG   (CCM_PLL_BASE   + 0x290)
#define SUART_THR                   (UART_BASE      +  0x00)
#define SUART_USR                   (UART_BASE      +  0x7c)
#define VDD_SYS_POFF_GATING         (R_PRCM_BASE    + 0x110)
#define CNT64_CTRL_REG              (R_CPU_CFG_REG  + 0x280)
#define CNT64_LOW_REG               (R_CPU_CFG_REG  + 0x284)
#define CNT64_HIGH_REG              (R_CPU_CFG_REG  + 0x288)

#elif defined(CONFIG_ARCH_SUN8IW6P1)
#define SRAM_DDRFREQ_SIZE           (SZ_8K)

/* register define */
#define MCTL_COM_BASE               (0xf1c62000)
#define MCTL_CTL_BASE               (0xf1c63000)
#define CCM_PLL_BASE                (0xf1c20000)
#define R_CPU_CFG_REG               (0xf1f01c00)
#define UART_BASE                   (0xf1c28000)

#define MC_WORK_MODE                (MCTL_COM_BASE +  0x00)
#define MC_MAER                     (MCTL_COM_BASE +  0x94)
#define MC_MDFSCR                   (MCTL_COM_BASE + 0x100)
#define MC_MDFSMRMR                 (MCTL_COM_BASE + 0x108)
#define PWRCTL                      (MCTL_CTL_BASE +  0x04)
#define RFSHTMG                     (MCTL_CTL_BASE +  0x90)
#define PGCR0                       (MCTL_CTL_BASE + 0x100)
#define PGCR1                       (MCTL_CTL_BASE + 0x104)
#define ODTMAP                      (MCTL_CTL_BASE + 0x120)
#define DXnLCDLR1(x)                (MCTL_CTL_BASE + 0x308 + 0x80*(x))
#define DATX0IOCR(x)                (MCTL_CTL_BASE + 0x310 +  0x4*(x))
#define DATX1IOCR(x)                (MCTL_CTL_BASE + 0x390 +  0x4*(x))
#define DATX2IOCR(x)                (MCTL_CTL_BASE + 0x410 +  0x4*(x))
#define DATX3IOCR(x)                (MCTL_CTL_BASE + 0x490 +  0x4*(x))
#define _CCM_PLL_DDR_REG            (CCM_PLL_BASE  +  0x20)
#define CCM_DRAM_CFG_REG            (CCM_PLL_BASE  +  0xf4)
#define _CCM_PLL_DDR_CFG_REG        (CCM_PLL_BASE  +  0xf8)
#define _CCM_PLL_DDR_PATTERN_REG    (CCM_PLL_BASE  + 0x290)
#define SUART_THR                   (UART_BASE     +  0x00)
#define SUART_USR                   (UART_BASE     +  0x7c)
#define CNT64_CTRL_REG              (R_CPU_CFG_REG + 0x280)
#define CNT64_LOW_REG               (R_CPU_CFG_REG + 0x284)
#define CNT64_HIGH_REG              (R_CPU_CFG_REG + 0x288)

#elif defined(CONFIG_ARCH_SUN8IW7P1)
#define SRAM_DDRFREQ_SIZE           (SZ_8K)

/* register define */
#define MCTL_COM_BASE               (0xf1c62000)
#define MCTL_CTL_BASE               (0xf1c63000)
#define CCM_PLL_BASE                (0xf1c20000)
#define R_CPU_CFG_REG               (0xf1f01c00)
#define UART_BASE                   (0xf1c28000)

#define MC_WORK_MODE                (MCTL_COM_BASE +  0x00)
#define PIR                         (MCTL_CTL_BASE +  0x00)
#define PWRCTL                      (MCTL_CTL_BASE +  0x04)
#define PGSR0                       (MCTL_CTL_BASE +  0x10)
#define STATR                       (MCTL_CTL_BASE +  0x18)
#define DTCR                        (MCTL_CTL_BASE +  0xc0)
#define ODTMAP                      (MCTL_CTL_BASE + 0x120)
#define DXnGCR0(x)                  (MCTL_CTL_BASE + 0x344 + 0x80*(x))
#define _CCM_PLL_DDR_REG            (CCM_PLL_BASE  +  0x20)
#define CCM_DRAM_CFG_REG            (CCM_PLL_BASE  +  0xf4)
#define SUART_THR                   (UART_BASE     +  0x00)
#define SUART_USR                   (UART_BASE     +  0x7c)
#define CNT64_CTRL_REG              (R_CPU_CFG_REG + 0x280)
#define CNT64_LOW_REG               (R_CPU_CFG_REG + 0x284)
#define CNT64_HIGH_REG              (R_CPU_CFG_REG + 0x288)

#elif defined(CONFIG_ARCH_SUN8IW8P1)

/* register define */
#define MCTL_COM_BASE               (0xf1c62000)
#define MCTL_CTL_BASE               (0xf1c63000)
#define CCM_PLL_BASE                (0xf1c20000)

#elif defined(CONFIG_ARCH_SUN8IW9P1)
#define SRAM_DDRFREQ_SIZE           (SZ_8K)

/* register define */
#define MCTL_COM_BASE               (0xf1c62000)
#define MCTL_CTL_BASE               (0xf1c63000)
#define CCM_PLL_BASE                (0xf1c20000)

#define MC_WORK_MODE                (MCTL_COM_BASE +  0x00)
#define MC_MDFSCR                   (MCTL_COM_BASE + 0x100)
#define RFSHTMG                     (MCTL_CTL_BASE +  0x90)
#define PGCR0                       (MCTL_CTL_BASE + 0x100)
#define ODTMAP                      (MCTL_CTL_BASE + 0x120)
#define DXnGCR0(x)                  (MCTL_CTL_BASE + 0x344 + 0x80*(x))
#define CCM_DRAM_CFG_REG            (CCM_PLL_BASE  +  0xf4)
#define _CCM_PLL_DDR1_REG           (CCM_PLL_BASE  +  0x4C)

#endif

typedef struct __DRAM_PARA
{
	unsigned int dram_clk;
	unsigned int dram_type;    
	unsigned int dram_zq;      
	unsigned int dram_odt_en;
	unsigned int dram_para1;   
	unsigned int dram_para2;   
	unsigned int dram_mr0;
	unsigned int dram_mr1;
	unsigned int dram_mr2;
	unsigned int dram_mr3;
	unsigned int dram_tpr0;
	unsigned int dram_tpr1;
	unsigned int dram_tpr2;
	unsigned int dram_tpr3;
	unsigned int dram_tpr4;    
	unsigned int dram_tpr5;
	unsigned int dram_tpr6;    
	unsigned int dram_tpr7;
	unsigned int dram_tpr8;
	unsigned int dram_tpr9;
	unsigned int dram_tpr10;
	unsigned int dram_tpr11;
	unsigned int dram_tpr12;
	unsigned int dram_tpr13;
#ifdef CONFIG_ARCH_SUN8IW3P1
	unsigned int high_freq;
#endif
} __dram_para_t;

static inline unsigned long ddr_save_sp(unsigned long new_sp)
{
	unsigned long old_sp;

	asm volatile ("mov %0, sp" : "=r" (old_sp));
	asm volatile ("mov sp, %0" :: "r" (new_sp));
	return old_sp;
}

/* sp define */
#define SRAM_DDRFREQ_SP_ADDR        (SRAM_DDRFREQ_OFFSET + SRAM_DDRFREQ_SIZE)
#define DDR_SAVE_SP(save_sp)        do { save_sp = ddr_save_sp(((unsigned long)SRAM_DDRFREQ_SP_ADDR)); } while (0)
#define DDR_RESTORE_SP(save_sp)     do { ddr_save_sp(save_sp); } while (0)

#define mctl_read_w(n)      (*((volatile unsigned int *)(n)))
#ifdef CONFIG_ARCH_SUN8IW3P1
	#define mctl_write_w(n,c)   (*((volatile unsigned int *)(n)) = (c))
#else
	#define mctl_write_w(c,n)   (*((volatile unsigned int *)(n)) = (c))
#endif

#if defined(CONFIG_ARCH_SUN9IW1P1) || defined(CONFIG_ARCH_SUN8IW7P1) || defined(CONFIG_DEVFREQ_DRAM_FREQ_LOW_POWER_SW) || defined(CONFIG_DEVFREQ_DRAM_FREQ_CFS_SW)
extern int __sram mdfs_main(unsigned int jump, __dram_para_t *dram_para,
				unsigned int pll_para_from_dram, unsigned int div);
#endif

#ifdef CONFIG_ARCH_SUN9IW1P1
extern void prefetch_and_prediction_disable(void);
extern void prefetch_and_prediction_enable(void);
#endif

#endif /* __MDFS_H__ */
