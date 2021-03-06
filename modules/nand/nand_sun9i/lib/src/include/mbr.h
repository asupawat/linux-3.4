/*
 * Copyright (C) 2013 Allwinnertech
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef    __ND_MBR_H__
#define    __ND_MBR_H__

#include "nand_drv_cfg.h"
#include "nand_logic.h"


#define ND_MAX_PART_COUNT		120	 								
#define ND_MBR_COPY_NUM        4    								

#define ND_MBR_START_ADDRESS	0x0									
#define ND_MBR_SIZE			   (16*1024)								
#define ND_MBR_RESERVED        (ND_MBR_SIZE - 32 - (ND_MAX_PART_COUNT * sizeof(ND_PARTITION)))

extern struct __NandDriverGlobal_t NandDriverInfo;

extern struct __NandStorageInfo_t  NandStorageInfo;

#define DiskSize  (SECTOR_CNT_OF_SINGLE_PAGE * PAGE_CNT_OF_PHY_BLK * BLOCK_CNT_OF_DIE * \
            DIE_CNT_OF_CHIP * NandStorageInfo.ChipCnt  / 1024 * DATA_BLK_CNT_OF_ZONE)


struct nand_disk{
	unsigned long size;
	unsigned long offset;
	unsigned char type;
};

/* part info */
typedef struct nand_tag_PARTITION{
	unsigned  int       addrhi;				//起始地址, 以扇区为单位
	unsigned  int       addrlo;			
	unsigned  int       lenhi;				//长度
	unsigned  int       lenlo;			
	unsigned  char      classname[16];		//次设备名
	unsigned  char      name[16];			//主设备名
	unsigned  int       user_type;          //用户类型
	unsigned  int       keydata;            //关键数据，要求量产不丢失
	unsigned  int       ro;                 //读写属性
	unsigned  char      reserved[68];		//保留数据，匹配分区信息128字节
}__attribute__ ((packed))ND_PARTITION;

/* mbr info */
typedef struct nand_tag_MBR{
        unsigned  int       crc32;                     
        unsigned  int       version;                    // 版本信息， 0x00000100
        unsigned  char      magic[8];                  
        unsigned  int       copy;                       //分数
        unsigned  int       index;                      //第几个MBR备份
        unsigned  int       PartCount;                  //分区个数
        unsigned  int       stamp[1];                   //对齐
        ND_PARTITION        array[ND_MAX_PART_COUNT]; 
        unsigned  char      res[ND_MBR_RESERVED];
}__attribute__ ((packed)) ND_MBR;

int mbr2disks(struct nand_disk* disk_array);
int NAND_PartInit(void);

#endif   
