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
* File Name        : Config_DMAC3_user.c
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
#include <stddef.h>
#include <errno.h>
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
static void (*s_dma_done_callback)(int) = NULL;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_DMAC3_Create_UserInit
* Description  : This function adds user code after initializing the DMAC3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DMAC3_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    s_dma_done_callback = NULL;
    DMAC3.DMSAR = &(PDC.PCDR.LONG);
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_DMAC3_dmac3i_interrupt
* Description  : This function is dmac3i interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_DMAC3_dmac3i_interrupt(void)
{
    if (DMAC3.DMSTS.BIT.DTIF == 1U)
    {
        DMAC3.DMSTS.BIT.DTIF = 0U;
        r_dmac3_callback_transfer_end();
    }
}

/***********************************************************************************************************************
* Function Name: r_dmac3_callback_transfer_end
* Description  : This function is dmac3 transfer end callback function
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_dmac3_callback_transfer_end(void)
{
    /* Start user code for r_dmac3_callback_transfer_end. Do not edit comment generated here */
    if (s_dma_done_callback != NULL)
    {
        s_dma_done_callback(0);
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/**
 * @brief Setup DMAC3 transfer destination.
 *        DMAC3 is used for PDC.
 *        Total transfer size is 'unit * block_size * block_count'.
 * @param addr Destination address.
 * @param unit Transfer unit. (1, 2, 4)
 * @param block_size Block size.
 * @param block_count Block count.
 * @param pcallback Callback function which called at transfer done.
 * @return On success, return 0. Otherwise, error number returned.
 */
int R_Config_DMAC3_Setup(uintptr_t addr, uint8_t unit, uint16_t block_size, uint16_t block_count,
        void (*pcallback)(int status))
{
    if (DMAC3.DMCNT.BIT.DTE != 0) // Transfer enable ?
    {
        return EBUSY;
    }
    if (((unit != 1) && (unit != 2) && (unit != 4)) // Invalid transfer unit ?
            || (block_size == 0) || (block_size > 1024) // Invalid block size ?
            || (block_count == 0)) // Invalid block count ?
    {
        return EINVAL;
    }

    DMAC3.DMDAR = addr;
    DMAC3.DMTMD.BIT.SZ = unit >> 1;
    DMAC3.DMCRA = ((uint32_t)(block_size) << 16u) | (uint32_t)(block_size);
    DMAC3.DMCRB = block_count;
    s_dma_done_callback = pcallback;

    return 0;
}

/**
 * @brief Get left transfer size.
 * @return Left transfer size in bytes returned.
 */
uint32_t R_Config_DMAC3_Get_LeftSize(void)
{
    uint32_t unit = DMAC3.DMTMD.BIT.SZ << 1;
    if (unit == 0) {
        unit = 1;
    }

    uint32_t left_size = DMAC3.DMCRA & 0x3FF; // DMCRL
    uint32_t left_block = DMAC3.DMCRB;

    return (unit * left_size * left_block);
}

/**
 * @brief Getting transfer state.
 * @return In transferrring, return true. Otherwise, return false.
 */
bool R_Config_DMAC3_IsTransferring(void)
{
    return DMAC3.DMCNT.BIT.DTE != 0;
}

/* End user code. Do not edit comment generated here */
