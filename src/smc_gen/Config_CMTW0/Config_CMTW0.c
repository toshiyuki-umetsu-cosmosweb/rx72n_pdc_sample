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
* File Name        : Config_CMTW0.c
* Component Version: 2.3.0
* Device(s)        : R5F572NNDxFC
* Description      : This file implements device driver for Config_CMTW0.
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
#include "Config_CMTW0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_CMTW0_Create
* Description  : This function initializes the CMTW0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMTW0_Create(void)
{
    /* Cancel CMTW stop state in LPC */
    MSTP(CMTW0) = 0U;

    /* Stop CMTW0 count */
    CMTW0.CMWSTR.BIT.STR = 0U;

    /* Set timer I/O control register */
    CMTW0.CMWIOR.WORD = _0000_CMTW_CMWIOR_IC0E_DISABLE | _0000_CMTW_CMWIOR_IC1E_DISABLE | 
                        _0000_CMTW_CMWIOR_OC0E_DISABLE | _0000_CMTW_CMWIOR_OC1E_DISABLE | 
                        _8000_CMTW_CMWIOR_CMWE_ENABLE;

    /* Set compare match register */
    CMTW0.CMWCOR = _FFFFFFFE_CMTW0_CMWCOR_VALUE;

    /* Set control registers */
    CMTW0.CMWCR.WORD = _0000_CMTW_CMWCR_CLOCK_PCLK8 | _0000_CMTW_CMWCR_COUNTER_SIZE_32 | _2000_CMTW_CMWCR_CCLR_DISABLE;

    R_Config_CMTW0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_CMTW0_Start
* Description  : This function starts the CMTW0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMTW0_Start(void)
{
    /* Start CMTW0 count */
    CMTW0.CMWSTR.BIT.STR = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_CMTW0_Stop
* Description  : This function stop the CMT0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMTW0_Stop(void)
{
    /* Stop CMTW0 count */
    CMTW0.CMWSTR.BIT.STR = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
