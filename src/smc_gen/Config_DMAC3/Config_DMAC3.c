/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_DMAC3.c
* Component Version: 1.8.0
* Device(s)        : R5F572NNDxFC
* Description      : This file implements device driver for Config_DMAC3.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_DMAC3.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_DMAC3_Create
* Description  : This function initializes the DMAC3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DMAC3_Create(void)
{
    /* Cancel DMAC/DTC module stop state in LPC */
    MSTP(DMAC) = 0U;

    /* Disable DMAC interrupts */
    IEN(DMAC,DMAC3I) = 0U;

    /* Disable DMAC3 transfer */
    DMAC3.DMCNT.BIT.DTE = 0U;

    /* Set DMAC3 activation source */
    ICU.DMRSR3 = _61_DMAC3_ACTIVATION_SOURCE;

    /* Set DMAC3 transfer address update and extended repeat setting */
    DMAC3.DMAMD.WORD = _0000_DMAC_SRC_ADDR_UPDATE_FIXED | _0080_DMAC_DST_ADDR_UPDATE_INCREMENT | 
                       _0000_DMAC3_SRC_EXT_RPT_AREA | _0000_DMAC3_DST_EXT_RPT_AREA;

    /* Set DMAC3 transfer mode, data size and repeat area */
    DMAC3.DMTMD.WORD = _8000_DMAC_TRANS_MODE_BLOCK | _0000_DMAC_REPEAT_AREA_DESTINATION | 
                       _0200_DMAC_TRANS_DATA_SIZE_32 | _0001_DMAC_TRANS_REQ_SOURCE_INT;

    /* Set DMAC3 interrupt flag control */
    DMAC3.DMCSL.BYTE = _00_DMAC_INT_TRIGGER_FLAG_CLEAR;

    /* Set DMAC3 source address */
    DMAC3.DMSAR = (void *)_00000000_DMAC3_SRC_ADDR;

    /* Set DMAC3 destination address */
    DMAC3.DMDAR = (void *)_00000000_DMAC3_DST_ADDR;

    /* Set DMAC3 block size */
    DMAC3.DMCRA = _00010001_DMAC3_DMCRA_COUNT;

    /* Set DMAC3 block transfer count */
    DMAC3.DMCRB = _0001_DMAC3_DMCRB_BLK_RPT_COUNT;

    /* Set DMAC3 interrupt settings */
    DMAC3.DMINT.BIT.DTIE = 1U;

    /* Set DMAC3 priority level */
    IPR(DMAC,DMAC3I) = _02_DMAC_PRIORITY_LEVEL2;

    /* Enable DMAC activation */
    DMAC.DMAST.BIT.DMST = 1U;
    
    R_Config_DMAC3_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC3_Start
* Description  : This function enable the DMAC3 activation
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DMAC3_Start(void)
{
    /* Enable DMAC3 interrupt in ICU */
    IR(DMAC,DMAC3I) = 0U;
    IEN(DMAC,DMAC3I) = 1U;
    DMAC3.DMCNT.BIT.DTE = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_DMAC3_Stop
* Description  : This function disable the DMAC3 activation
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DMAC3_Stop(void)
{
    /* Disable CMI3 interrupt in ICU */
    IR(DMAC,DMAC3I) = 0U;
    IEN(DMAC,DMAC3I) = 0U;
    DMAC3.DMCNT.BIT.DTE = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
