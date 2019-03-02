/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __NAND_OSAL_H__
#define __NAND_OSAL_H__

#include <linux/kernel.h>
#include <linux/slab.h>
#include <mach/platform.h>

#define __OS_LINUX_SYSTEM__
#define __OS_SUPPORT_RB_INT__

#define NAND_IO_BASE_ADDR		SW_VA_NANDFLASHC_IO_BASE

extern void *NAND_IORemap(unsigned int base_addr, unsigned int size);

extern int NAND_ClkRequest(void);
extern void NAND_ClkRelease(void);
extern int NAND_AHBEnable(void);
extern void NAND_AHBDisable(void);
extern int NAND_ClkEnable(void);
extern void NAND_ClkDisable(void);
extern int NAND_SetClk(unsigned int nand_clk);
extern int NAND_GetClk(void);

extern int NAND_RequestDMA(void);
extern int NAND_ReleaseDMA(void);
extern void NAND_DMAConfigStart(int rw, unsigned int buff_addr, int len);
extern int NAND_QueryDmaStat(void);
extern int NAND_WaitDmaFinish(void);

extern void NAND_PIORequest(void);
extern void NAND_PIORelease(void);

extern void NAND_EnRbInt(void);
extern void NAND_ClearRbInt(void);
extern int NAND_WaitRbReady(void);
extern void NAND_RbInterrupt(void);

extern void* NAND_Malloc(unsigned int Size);
extern void NAND_Free(void *pAddr, unsigned int Size);
extern int NAND_Print(const char * str, ...);

#define MEMSET(x,y,z)            			memset((x),(y),(z))

#define MEMCPY(x,y,z)                   	memcpy((x),(y),(z))

#define MALLOC(x)                       	NAND_Malloc((x))

#define FREE(x,size)                    	NAND_Free((x),(size))

#define PRINT(...)							NAND_Print(__VA_ARGS__)

#define DBUG_MSG(...)

#define DBUG_INF(...)                       NAND_Print(__VA_ARGS__)


#endif