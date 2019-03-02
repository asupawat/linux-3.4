/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * File: wctl.c
 *
 * Purpose: handle WMAC duplicate filter & defragment
 *
 * Author: Jerry Chen
 *
 * Date: Jun. 27, 2002
 *
 * Functions:
 *      WCTLbIsDuplicate - Test if duplicate packet
 *      WCTLuSearchDFCB - Search DeFragment Control Database
 *      WCTLuInsertDFCB - Insert DeFragment Control Database
 *      WCTLbHandleFragment - Handle received fragment packet
 *
 * Revision History:
 *
 */

#include "wctl.h"
#include "device.h"
#include "card.h"
#include "tmacro.h"

/*---------------------  Static Definitions -------------------------*/

/*---------------------  Static Classes  ----------------------------*/

/*---------------------  Static Variables  --------------------------*/
/*---------------------  Static Functions  --------------------------*/

/*---------------------  Export Variables  --------------------------*/



/*
 * Description:
 *      Scan Rx cache.  Return TRUE if packet is duplicate, else
 *      inserts in receive cache and returns FALSE.
 *
 * Parameters:
 *  In:
 *      pCache      - Receive packets history
 *      pMACHeader  - 802.11 MAC Header of received packet
 *  Out:
 *      none
 *
 * Return Value: TRUE if packet duplicate; otherwise FALSE
 *
 */

BOOL WCTLbIsDuplicate (PSCache pCache, PS802_11Header pMACHeader)
{
    unsigned int            uIndex;
    unsigned int            ii;
    PSCacheEntry    pCacheEntry;

    if (IS_FC_RETRY(pMACHeader)) {

        uIndex = pCache->uInPtr;
        for (ii = 0; ii < DUPLICATE_RX_CACHE_LENGTH; ii++) {
            pCacheEntry = &(pCache->asCacheEntry[uIndex]);
            if ((pCacheEntry->wFmSequence == pMACHeader->wSeqCtl) &&
		(!compare_ether_addr(&(pCacheEntry->abyAddr2[0]),
				     &(pMACHeader->abyAddr2[0]))) &&
                (LOBYTE(pCacheEntry->wFrameCtl) == LOBYTE(pMACHeader->wFrameCtl))
                ) {
                /* Duplicate match */
                return TRUE;
            }
            ADD_ONE_WITH_WRAP_AROUND(uIndex, DUPLICATE_RX_CACHE_LENGTH);
        }
    }
    /* Not fount in cache - insert */
    pCacheEntry = &pCache->asCacheEntry[pCache->uInPtr];
    pCacheEntry->wFmSequence = pMACHeader->wSeqCtl;
    memcpy(&(pCacheEntry->abyAddr2[0]), &(pMACHeader->abyAddr2[0]), ETH_ALEN);
    pCacheEntry->wFrameCtl = pMACHeader->wFrameCtl;
    ADD_ONE_WITH_WRAP_AROUND(pCache->uInPtr, DUPLICATE_RX_CACHE_LENGTH);
    return FALSE;
}

/*
 * Description:
 *      Found if sequence number of received fragment packet in Defragment Database
 *
 * Parameters:
 *  In:
 *      pDevice     - Pointer to adapter
 *      pMACHeader  - 802.11 MAC Header of received packet
 *  Out:
 *      none
 *
 * Return Value: index number in Defragment Database
 *
 */

unsigned int WCTLuSearchDFCB(PSDevice pDevice, PS802_11Header pMACHeader)
{
	unsigned int ii;

	for (ii = 0; ii < pDevice->cbDFCB; ii++) {
		if ((pDevice->sRxDFCB[ii].bInUse == TRUE) &&
		    (!compare_ether_addr(&(pDevice->sRxDFCB[ii].abyAddr2[0]),
					  &(pMACHeader->abyAddr2[0])))) {
			return ii;
		}
	}
	return pDevice->cbDFCB;
}

/*
 * Description:
 *      Insert received fragment packet in Defragment Database
 *
 * Parameters:
 *  In:
 *      pDevice     - Pointer to adapter
 *      pMACHeader  - 802.11 MAC Header of received packet
 *  Out:
 *      none
 *
 * Return Value: index number in Defragment Database
 *
 */
unsigned int WCTLuInsertDFCB(PSDevice pDevice, PS802_11Header pMACHeader)
{
	unsigned int ii;

    if (pDevice->cbFreeDFCB == 0)
        return(pDevice->cbDFCB);
    for (ii = 0; ii < pDevice->cbDFCB; ii++) {
        if (pDevice->sRxDFCB[ii].bInUse == FALSE) {
            pDevice->cbFreeDFCB--;
            pDevice->sRxDFCB[ii].uLifetime = pDevice->dwMaxReceiveLifetime;
            pDevice->sRxDFCB[ii].bInUse = TRUE;
            pDevice->sRxDFCB[ii].wSequence = (pMACHeader->wSeqCtl >> 4);
            pDevice->sRxDFCB[ii].wFragNum = (pMACHeader->wSeqCtl & 0x000F);
	    memcpy(&(pDevice->sRxDFCB[ii].abyAddr2[0]),
		   &(pMACHeader->abyAddr2[0]),
		   ETH_ALEN);
            return(ii);
        }
    }
    return(pDevice->cbDFCB);
}


/*
 * Description:
 *      Handle received fragment packet
 *
 * Parameters:
 *  In:
 *      pDevice         - Pointer to adapter
 *      pMACHeader      - 802.11 MAC Header of received packet
 *      cbFrameLength   - Frame length
 *      bWEP            - is WEP packet
 *  Out:
 *      none
 *
 * Return Value: TRUE if it is valid fragment packet and we have resource to defragment; otherwise FALSE
 *
 */
BOOL WCTLbHandleFragment(PSDevice pDevice, PS802_11Header pMACHeader,
			 unsigned int cbFrameLength, BOOL bWEP, BOOL bExtIV)
{
unsigned int            uHeaderSize;


    if (bWEP == TRUE) {
        uHeaderSize = 28;
        if (bExtIV)
            uHeaderSize +=4;
    }
    else {
        uHeaderSize = 24;
    }

    if (IS_FIRST_FRAGMENT_PKT(pMACHeader)) {
        pDevice->uCurrentDFCBIdx = WCTLuSearchDFCB(pDevice, pMACHeader);
        if (pDevice->uCurrentDFCBIdx < pDevice->cbDFCB) {
            pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].uLifetime = pDevice->dwMaxReceiveLifetime;
            pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wSequence = (pMACHeader->wSeqCtl >> 4);
            pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wFragNum = (pMACHeader->wSeqCtl & 0x000F);
        }
        else {
            pDevice->uCurrentDFCBIdx = WCTLuInsertDFCB(pDevice, pMACHeader);
            if (pDevice->uCurrentDFCBIdx == pDevice->cbDFCB) {
                return(FALSE);
            }
        }
        pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].pbyRxBuffer = (PBYTE) (pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].skb->data + 8);
        memcpy(pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].pbyRxBuffer, pMACHeader, cbFrameLength);
        pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].cbFrameLength = cbFrameLength;
        pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].pbyRxBuffer += cbFrameLength;
        pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wFragNum++;
        return(FALSE);
    }
    else {
        pDevice->uCurrentDFCBIdx = WCTLuSearchDFCB(pDevice, pMACHeader);
        if (pDevice->uCurrentDFCBIdx != pDevice->cbDFCB) {
            if ((pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wSequence == (pMACHeader->wSeqCtl >> 4)) &&
                (pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wFragNum == (pMACHeader->wSeqCtl & 0x000F)) &&
                ((pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].cbFrameLength + cbFrameLength - uHeaderSize) < 2346)) {

                memcpy(pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].pbyRxBuffer, ((PBYTE) (pMACHeader) + uHeaderSize), (cbFrameLength - uHeaderSize));
                pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].cbFrameLength += (cbFrameLength - uHeaderSize);
                pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].pbyRxBuffer += (cbFrameLength - uHeaderSize);
                pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].wFragNum++;
            }
            else {
                pDevice->cbFreeDFCB++;
                pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].bInUse = FALSE;
                return(FALSE);
            }
        }
        else {
            return(FALSE);
        }
        if (IS_LAST_FRAGMENT_PKT(pMACHeader)) {
            pDevice->cbFreeDFCB++;
            pDevice->sRxDFCB[pDevice->uCurrentDFCBIdx].bInUse = FALSE;
            return(TRUE);
        }
        return(FALSE);
    }
}


