/*
 * Copyright (C) 2013 Allwinnertech, kevin.z.m <kevin@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "../include/nand_drv_cfg.h"
#include "../include/nand_type.h"
#include "../include/nand_physic.h"
#include "../include/nfc.h"



extern  struct __NandStorageInfo_t  NandStorageInfo;
extern __s32 BOOT_NandGetPara(boot_nand_para_t *param);


void _InitNandPhyInfo(boot_nand_para_t *nand_info)
{
    __u32 i;

    NandStorageInfo.ChipCnt          = nand_info->ChipCnt          ;
    NandStorageInfo.ChipConnectInfo  = nand_info->ChipConnectInfo  ;
    NandStorageInfo.RbCnt            = nand_info->RbCnt            ;
    NandStorageInfo.RbConnectInfo	 = nand_info->RbConnectInfo	   ;
    NandStorageInfo.RbConnectMode	 = nand_info->RbConnectMode	   ;
    NandStorageInfo.BankCntPerChip   = nand_info->BankCntPerChip   ;
    NandStorageInfo.DieCntPerChip    = nand_info->DieCntPerChip    ;
    NandStorageInfo.PlaneCntPerDie   = nand_info->PlaneCntPerDie   ;
    NandStorageInfo.SectorCntPerPage = nand_info->SectorCntPerPage ;
    NandStorageInfo.PageCntPerPhyBlk = nand_info->PageCntPerPhyBlk ;
    NandStorageInfo.BlkCntPerDie     = nand_info->BlkCntPerDie     ;
    NandStorageInfo.OperationOpt     = (nand_info->OperationOpt)&(NAND_RANDOM|NAND_READ_RETRY)     ;
    NandStorageInfo.FrequencePar     = nand_info->FrequencePar     ;
    NandStorageInfo.EccMode          = nand_info->EccMode          ;
    NandStorageInfo.ValidBlkRatio    = nand_info->ValidBlkRatio    ;
	NandStorageInfo.ReadRetryType    = nand_info->ReadRetryType    ;
	NandStorageInfo.DDRType          = nand_info->DDRType          ;

	for(i=0;i<8;i++)
	    NandStorageInfo.NandChipId[i]  = nand_info->NandChipId[i]    ;

}

/*
************************************************************************************************************************
*                           SEARCH NAND PHYSICAL ARCHITECTURE PARAMETER
*
*Description: Search the nand flash physical architecture parameter from the parameter table
*             by nand chip ID.
*
*Arguments  : pNandID           the pointer to nand flash chip ID;
*             pNandArchiInfo    the pointer to nand flash physical architecture parameter.
*
*Return     : search result;
*               = 0     search successful, find the parameter in the table;
*               < 0     search failed, can't find the parameter in the table.
************************************************************************************************************************
*/
__s32 _CheckNandID(__u8 *pNandID)
{
    __s32 j=0;

    while(1)
    {
        for(j=0; j<6; j++)
        {
            if((pNandID[j] != NandStorageInfo.NandChipId[j])&&(NandStorageInfo.NandChipId[j]!=0xff))
            break;
        }

        if(j==6)
        {
           /*6 bytes of the nand chip ID are all matching, search parameter successful*/
            return 0;
        }

        break;
    }

    return -1;
}


/*
************************************************************************************************************************
*                           ANALYZE NAND FLASH STORAGE SYSTEM
*
*Description: Analyze nand flash storage system, generate the nand flash physical
*             architecture parameter and connect information.
*
*Arguments  : none
*
*Return     : analyze result;
*               = 0     analyze successful;
*               < 0     analyze failed, can't recognize or some other error.
************************************************************************************************************************
*/
__s32  BOOT_AnalyzeNandSystem(void)
{
    __s32 result;
    __u32 i;
    __u8  tmpChipID[8];
    boot_nand_para_t nand_info;

    if( BOOT_NandGetPara( &nand_info ) != 0 ){
		return -1;
	}

    _InitNandPhyInfo(&nand_info);


    result = PHY_ResetChip(BOOT_CHIP_SELECT_NUM);
    result |= PHY_SynchBank(BOOT_CHIP_SELECT_NUM, SYNC_CHIP_MODE);
    if(result)
      return -1;

    result = PHY_ReadNandId(BOOT_CHIP_SELECT_NUM, tmpChipID);
    if(result)
        return -1;

    result = _CheckNandID(tmpChipID);
    if(result)
        return -1;


	/*configure page size*/
	{
		NFC_INIT_INFO nfc_info;
		nfc_info.bus_width = 0x0;
		nfc_info.ce_ctl = 0x0;
		nfc_info.ce_ctl1 = 0x0;
		nfc_info.debug = 0x0;
		nfc_info.pagesize = SECTOR_CNT_OF_SINGLE_PAGE;
		nfc_info.rb_sel = 1;
		nfc_info.serial_access_mode = 1;
		nfc_info.ddr_type = DDR_TYPE;
		NFC_ChangMode(&nfc_info);
	}

	PHY_ChangeMode(1);

	if(SUPPORT_READ_RETRY&&(((READ_RETRY_TYPE>>16)&0xff) <0x10)) 
	{
		SCAN_DBG("NFC Read Retry Init. \n");
		NFC_ReadRetryInit(READ_RETRY_TYPE);
	    PHY_GetDefaultParam(0);
	}
	else
	{
	    NandStorageInfo.ReadRetryType = 0;
	}

    return 0;
}
