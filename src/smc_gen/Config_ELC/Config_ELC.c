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
* File Name        : Config_ELC.c
* Component Version: 1.9.0
* Device(s)        : R5F572NNDxFC
* Description      : This file implements device driver for Config_ELC.
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
#include "Config_ELC.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_ELC_Create
* Description  : This function initializes the ELC module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ELC_Create(void)
{
    /* Cancel module stop state */
    MSTP(ELC) = 0U;

    /* Set event link for CMTW0 */
    ELC.ELSR33.BYTE = _AC_ELC_EVENT_SIG_TPU0_MATCHA;
    ELC.ELOPH.BIT.CMTW0MD = _02_ELC_OPERATION_EVENT_COUNTER;

    R_Config_ELC_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_ELC_Start
* Description  : This function enables all ELC event links
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ELC_Start(void)
{
    /* Enable all ELC event links */
    ELC.ELCR.BIT.ELCON = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_ELC_Stop
* Description  : This function disables all ELC event links
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ELC_Stop(void)
{
    /* Disable all ELC event links */
    ELC.ELCR.BIT.ELCON = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_ELC_GenerateSoftwareEvent
* Description  : This function triggers a software event
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ELC_GenerateSoftwareEvent(void)
{
    ELC.ELSEGR.BYTE = _3E_ELC_SOFTWARE_EVENT_GENERATION_UNLOCK;
    ELC.ELSEGR.BYTE = _7E_ELC_SOFTWARE_EVENT_GENERATION_ENABLE;
    ELC.ELSEGR.BYTE = _7F_ELC_GENERATE_SOFTWARE_EVENT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
