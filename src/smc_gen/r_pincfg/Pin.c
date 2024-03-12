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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Pin.c
* Version          : 1.0.2
* Device(s)        : R5F572NNDxFC
* Description      : This file implements SMC pin code generation.
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
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Pins_Create(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set HSYNC pin */
    MPC.P25PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x20U;

    /* Set LCD_CLK pin */
    MPC.P14PFS.BYTE = 0x25U;
    PORT1.PMR.BYTE |= 0x10U;

    /* Set LCD_DATA0 pin */
    MPC.PJ0PFS.BYTE = 0x25U;
    PORTJ.PMR.BYTE |= 0x01U;

    /* Set LCD_DATA1 pin */
    MPC.P85PFS.BYTE = 0x25U;
    PORT8.PMR.BYTE |= 0x20U;

    /* Set LCD_DATA2 pin */
    MPC.P84PFS.BYTE = 0x25U;
    PORT8.PMR.BYTE |= 0x10U;

    /* Set LCD_DATA3 pin */
    MPC.P57PFS.BYTE = 0x25U;
    PORT5.PMR.BYTE |= 0x80U;

    /* Set LCD_DATA4 pin */
    MPC.P56PFS.BYTE = 0x25U;
    PORT5.PMR.BYTE |= 0x40U;

    /* Set LCD_DATA5 pin */
    MPC.P55PFS.BYTE = 0x25U;
    PORT5.PMR.BYTE |= 0x20U;

    /* Set LCD_DATA6 pin */
    MPC.P54PFS.BYTE = 0x25U;
    PORT5.PMR.BYTE |= 0x10U;

    /* Set LCD_DATA7 pin */
    MPC.P11PFS.BYTE = 0x25U;
    PORT1.PMR.BYTE |= 0x02U;

    /* Set LCD_TCON0 pin */
    MPC.P13PFS.BYTE = 0x25U;
    PORT1.PMR.BYTE |= 0x08U;

    /* Set LCD_TCON1 pin */
    MPC.P12PFS.BYTE = 0x25U;
    PORT1.PMR.BYTE |= 0x04U;

    /* Set PCKO pin */
    MPC.P33PFS.BYTE = 0x1CU;
    PORT3.PMR.BYTE |= 0x08U;

    /* Set PIXCLK pin */
    MPC.P24PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x10U;

    /* Set PIXD0 pin */
    MPC.P15PFS.BYTE = 0x1CU;
    PORT1.PMR.BYTE |= 0x20U;

    /* Set PIXD1 pin */
    MPC.P86PFS.BYTE = 0x1CU;
    PORT8.PMR.BYTE |= 0x40U;

    /* Set PIXD2 pin */
    MPC.P87PFS.BYTE = 0x1CU;
    PORT8.PMR.BYTE |= 0x80U;

    /* Set PIXD3 pin */
    MPC.P17PFS.BYTE = 0x1CU;
    PORT1.PMR.BYTE |= 0x80U;

    /* Set PIXD4 pin */
    MPC.P20PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x01U;

    /* Set PIXD5 pin */
    MPC.P21PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x02U;

    /* Set PIXD6 pin */
    MPC.P22PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x04U;

    /* Set PIXD7 pin */
    MPC.P23PFS.BYTE = 0x1CU;
    PORT2.PMR.BYTE |= 0x08U;

    /* Set SSCL6 pin */
    MPC.P01PFS.BYTE = 0x0AU;
    PORT0.PMR.BYTE |= 0x02U;

    /* Set SSDA6 pin */
    MPC.P00PFS.BYTE = 0x0AU;
    PORT0.PMR.BYTE |= 0x01U;

    /* Set USB0_VBUS pin */
    MPC.P16PFS.BYTE = 0x11U;
    PORT1.PMR.BYTE |= 0x40U;

    /* Set VSYNC pin */
    MPC.P32PFS.BYTE = 0x1CU;
    PORT3.PMR.BYTE |= 0x04U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

