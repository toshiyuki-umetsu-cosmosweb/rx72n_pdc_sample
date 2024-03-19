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
* File Name        : Config_DMAC3.h
* Component Version: 1.8.0
* Device(s)        : R5F572NNDxFC
* Description      : This file implements device driver for Config_DMAC3.
***********************************************************************************************************************/

#ifndef CFG_Config_DMAC3_H
#define CFG_Config_DMAC3_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_dmac.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _61_DMAC3_ACTIVATION_SOURCE                             (0x61U) /* Activation source select */
#define _00000000_DMAC3_SRC_ADDR                                (0x00000000UL) /* Source address */
#define _00000000_DMAC3_DST_ADDR                                (0x00000000UL) /* Destination address */
#define _00010001_DMAC3_DMCRA_COUNT                             (0x00010001UL) /* Repeat or Block size */
#define _0001_DMAC3_DMCRB_BLK_RPT_COUNT                         (0x0001U) /* Repeat or Transfer-block count */
#define _0000_DMAC3_SRC_EXT_RPT_AREA                            (0x0000U) /* Source address extended repeat area */
#define _0000_DMAC3_DST_EXT_RPT_AREA                            (0x0000U) /* Destination address extended repeat area */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_DMAC3_Create(void);
void R_Config_DMAC3_Create_UserInit(void);
void R_Config_DMAC3_Start(void);
void R_Config_DMAC3_Stop(void);
static void r_dmac3_callback_transfer_end(void);
/* Start user code for function. Do not edit comment generated here */
#include <stdint.h>
#include <stdbool.h>
int R_Config_DMAC3_Setup(uintptr_t addr, uint8_t unit, uint16_t block_size, uint16_t block_count,
        void (*pcallback)(int status));
uint32_t R_Config_DMAC3_Get_LeftSize(void);
bool R_Config_DMAC3_IsTransferring(void);
/* End user code. Do not edit comment generated here */
#endif
