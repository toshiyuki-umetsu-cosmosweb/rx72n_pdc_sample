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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_pdc_rx_pinset.c
* Version      : 1.0.2
* Device(s)    : R5F572NNDxFC
* Tool-Chain   : RXC toolchain
* Description  : Setting of port and mpc registers
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_pdc_rx_pinset.h"
#include "platform.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name: R_PDC_PinSet
* Description  : This function initializes pins for r_pdc_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_PDC_PinSet()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set PIXCLK pin */
    MPC.P24PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B4 = 1U;

    /* Set VSYNC pin */
    MPC.P32PFS.BYTE = 0x1CU;
    PORT3.PMR.BIT.B2 = 1U;

    /* Set HSYNC pin */
    MPC.P25PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B5 = 1U;

    /* Set PIXD7 pin */
    MPC.P23PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B3 = 1U;

    /* Set PIXD6 pin */
    MPC.P22PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B2 = 1U;

    /* Set PIXD5 pin */
    MPC.P21PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B1 = 1U;

    /* Set PIXD4 pin */
    MPC.P20PFS.BYTE = 0x1CU;
    PORT2.PMR.BIT.B0 = 1U;

    /* Set PIXD3 pin */
    MPC.P17PFS.BYTE = 0x1CU;
    PORT1.PMR.BIT.B7 = 1U;

    /* Set PIXD2 pin */
    MPC.P87PFS.BYTE = 0x1CU;
    PORT8.PMR.BIT.B7 = 1U;

    /* Set PIXD1 pin */
    MPC.P86PFS.BYTE = 0x1CU;
    PORT8.PMR.BIT.B6 = 1U;

    /* Set PIXD0 pin */
    MPC.P15PFS.BYTE = 0x1CU;
    PORT1.PMR.BIT.B5 = 1U;

    /* Set PCKO pin */
    MPC.P33PFS.BYTE = 0x1CU;
    PORT3.PMR.BIT.B3 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

