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

#define ND_MAX_PART_COUNT		120	 								
#define ND_MBR_COPY_NUM        4    								

#define ND_MBR_START_ADDRESS	0x0									
#define ND_MBR_SIZE			   (16*1024)								
#define ND_MBR_RESERVED        (ND_MBR_SIZE - 32 - (ND_MAX_PART_COUNT * sizeof(ND_PARTITION)))

extern struct __NandDriverGlobal_t NandDriverInfo;

extern struct __NandStorageInfo_t  NandStorageInfo;

#define DiskSize  (SECTOR_CNT_OF_SINGLE_PAGE * PAGE_CNT_OF_PHY_BLK * BLOCK_CNT_OF_DIE * \
            DIE_CNT_OF_CHIP * NandStorageInfo.ChipCnt*CHANNEL_CNT  / 1024 * DATA_BLK_CNT_OF_ZONE)


struct nand_disk{
	unsigned long size;
	unsigned long offset;
	unsigned char type;
};

/* part info */
typedef struct nand_tag_PARTITION{
	unsigned  int       addrhi;				//��ʼ��ַ, ������Ϊ��λ
	unsigned  int       addrlo;			
	unsigned  int       lenhi;				//����
	unsigned  int       lenlo;			
	unsigned  char      classname[16];		//���豸��
	unsigned  char      name[16];			//���豸��
	unsigned  int       user_type;          //�û�����
	unsigned  int       keydata;            //�ؼ����ݣ�Ҫ����������ʧ
	unsigned  int       ro;                 //��д����
	unsigned  char      reserved[68];		//�������ݣ�ƥ�������Ϣ128�ֽ�
}ND_PARTITION;

/* mbr info */
typedef struct nand_tag_MBR{
	unsigned  int       crc32;				       
	unsigned  int       version;			        // �汾��Ϣ�� 0x00000100
	unsigned  char 	    magic[8];			       
	unsigned  int 	    copy;				        //����
	unsigned  int 	    index;				        //�ڼ���MBR����
	unsigned  int       PartCount;			        //��������
	unsigned  int       stamp[1];					//����
	ND_PARTITION           array[ND_MAX_PART_COUNT];
	unsigned  char      res[ND_MBR_RESERVED];
}ND_MBR;

int mbr2disks(struct nand_disk* disk_array);
int NAND_PartInit(void);

#endif   
