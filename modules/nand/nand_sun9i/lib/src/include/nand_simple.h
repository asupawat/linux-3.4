/*
 * Copyright (C) 2013 Allwinnertech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PHY_BOOT__
#define  __PHY_BOOT__

#include "../include/nand_drv_cfg.h"

struct boot_physical_param{
	__u32   chip;
	__u32  block;
	__u32  page;
	__u32  sectorbitmap;
	void   *mainbuf;
	void   *oobbuf;
};

extern __s32 PHY_SimpleErase(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleRead(struct boot_physical_param * readop);
extern __s32 PHY_SimpleWrite(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_1K(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_Seq(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleRead_Seq(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_1K(struct boot_physical_param * readop);

extern __s32 PHY_SimpleErase_2CH(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleErase_CurCH(struct boot_physical_param * eraseop);
extern __s32 PHY_SimpleRead_CurCH(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_2CH(struct boot_physical_param *readop);
extern __s32 PHY_SimpleWrite_CurCH(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_1KCurCH(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleRead_1KCurCH(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_1st1K(struct boot_physical_param * readop);
extern __s32 PHY_SimpleRead_1st1KCurCH(struct boot_physical_param * readop);
extern __s32 PHY_SimpleWrite_Seq_16K(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleRead_Seq_16K(struct boot_physical_param * readop);
extern __s32 PHY_SimpleWrite_0xFF(struct boot_physical_param * writeop);
extern __s32 PHY_SimpleWrite_0xFF_8K(struct boot_physical_param * writeop);

#endif
