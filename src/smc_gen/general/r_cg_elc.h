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
* File Name        : r_cg_elc.h
* Version          : 1.0.110
* Device(s)        : R5F572NNDxFC
* Description      : General header file for ELC peripheral.
***********************************************************************************************************************/

#ifndef ELC_H
#define ELC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/* 
    Event Link Control Register (ELCR)
*/
/* All Event Link Enable (ELCON) */
#define _FF_ELC_ENABLE_ALL                               (0xFFU) /* Linkage of all the events is enabled */
#define _7F_ELC_DISABLE_ALL                              (0x7FU) /* Linkage of all the events is disabled */

/*
    Event Link Setting Register n (ELSRn)
*/
/* Event Link Select (ELS[7:0]) */
#define _00_ELC_EVENT_SIG_DISABLE                        (0x00U) /* Event link function is disabled */
#define _01_ELC_EVENT_SIG_MTU0_MATCH0A                   (0x01U) /* MTU0 compare match 0A signal */
#define _02_ELC_EVENT_SIG_MTU0_MATCH0B                   (0x02U) /* MTU0 compare match 0B signal */
#define _03_ELC_EVENT_SIG_MTU0_MATCH0C                   (0x03U) /* MTU0 compare match 0C signal */
#define _04_ELC_EVENT_SIG_MTU0_MATCH0D                   (0x04U) /* MTU0 compare match 0D signal */
#define _05_ELC_EVENT_SIG_MTU0_MATCH0E                   (0x05U) /* MTU0 compare match 0E signal */
#define _06_ELC_EVENT_SIG_MTU0_MATCH0F                   (0x06U) /* MTU0 compare match 0F signal */
#define _07_ELC_EVENT_SIG_MTU0_OVF                       (0x07U) /* MTU0 overflow signal */
#define _10_ELC_EVENT_SIG_MTU3_MATCH3A                   (0x10U) /* MTU3 compare match 3A signal */
#define _11_ELC_EVENT_SIG_MTU3_MATCH3B                   (0x11U) /* MTU3 compare match 3B signal */
#define _12_ELC_EVENT_SIG_MTU3_MATCH3C                   (0x12U) /* MTU3 compare match 3C signal */
#define _13_ELC_EVENT_SIG_MTU3_MATCH3D                   (0x13U) /* MTU3 compare match 3D signal */
#define _14_ELC_EVENT_SIG_MTU3_OVF                       (0x14U) /* MTU3 overflow signal */
#define _15_ELC_EVENT_SIG_MTU4_MATCH4A                   (0x15U) /* MTU4 compare match 4A signal */
#define _16_ELC_EVENT_SIG_MTU4_MATCH4B                   (0x16U) /* MTU4 compare match 4B signal */
#define _17_ELC_EVENT_SIG_MTU4_MATCH4C                   (0x17U) /* MTU4 compare match 4C signal */
#define _18_ELC_EVENT_SIG_MTU4_MATCH4D                   (0x18U) /* MTU4 compare match 4D signal */
#define _19_ELC_EVENT_SIG_MTU4_OVF                       (0x19U) /* MTU4 overflow signal */
#define _1A_ELC_EVENT_SIG_MTU4_UDF                       (0x1AU) /* MTU4 underflow signal */
#define _1F_ELC_EVENT_SIG_CMT1_MATCH                     (0x1FU) /* CMT1 compare match 1 signal */
#define _22_ELC_EVENT_SIG_TMR0_MATCHA0                   (0x22U) /* TMR0 compare match A0 signal */
#define _23_ELC_EVENT_SIG_TMR0_MATCHB0                   (0x23U) /* TMR0 compare match B0 signal */
#define _24_ELC_EVENT_SIG_TMR0_OVF                       (0x24U) /* TMR0 overflow signal */
#define _25_ELC_EVENT_SIG_TMR1_MATCHA1                   (0x25U) /* TMR1 compare match A1 signal */
#define _26_ELC_EVENT_SIG_TMR1_MATCHB1                   (0x26U) /* TMR1 compare match B1 signal */
#define _27_ELC_EVENT_SIG_TMR1_OVF                       (0x27U) /* TMR1 overflow signal */
#define _28_ELC_EVENT_SIG_TMR2_MATCHA2                   (0x28U) /* TMR2 compare match A2 signal */
#define _29_ELC_EVENT_SIG_TMR2_MATCHB2                   (0x29U) /* TMR2 compare match B2 signal */
#define _2A_ELC_EVENT_SIG_TMR2_OVF                       (0x2AU) /* TMR2 overflow signal */
#define _2B_ELC_EVENT_SIG_TMR3_MATCHA3                   (0x2BU) /* TMR3 compare match A3 signal */
#define _2C_ELC_EVENT_SIG_TMR3_MATCHB3                   (0x2CU) /* TMR3 compare match B3 signal */
#define _2D_ELC_EVENT_SIG_TMR3_OVF                       (0x2DU) /* TMR3 overflow signal */
#define _2E_ELC_EVENT_SIG_RTC_CYCLE                      (0x2EU) /* RTC cycle */
#define _31_ELC_EVENT_SIG_IWDT_UDF_OR_ERR                (0x31U) /* IWDT underflow or refresh error */
#define _3A_ELC_EVENT_SIG_SCI5_ERR                       (0x3AU) /* SCI5 error signal */
#define _3B_ELC_EVENT_SIG_SCI5_RX_FULL                   (0x3BU) /* SCI5 receive data full signal */
#define _3C_ELC_EVENT_SIG_SCI5_TX_EMPTY                  (0x3CU) /* SCI5 transmit data empty signal */
#define _3D_ELC_EVENT_SIG_SCI5_TX_END                    (0x3DU) /* SCI5 transmit end signal */
#define _4E_ELC_EVENT_SIG_RIIC0_ERR                      (0x4EU) /* RIIC0 communication error or event generation */
#define _4F_ELC_EVENT_SIG_RIIC0_RX_FULL                  (0x4FU) /* RIIC0 receive data full */
#define _50_ELC_EVENT_SIG_RIIC0_TX_EMPTY                 (0x50U) /* RIIC0 transmit data empty signal */
#define _51_ELC_EVENT_SIG_RIIC0_TX_END                   (0x51U) /* RIIC0 transmit end signal */
#define _52_ELC_EVENT_SIG_RSPI0_ERR                      (0x52U) /* RSPI0 error */
#define _53_ELC_EVENT_SIG_RSPI0_IDLE                     (0x53U) /* RSPI0 idle */
#define _54_ELC_EVENT_SIG_RSPI0_RX_FULL                  (0x54U) /* RSPI0 receive data full */
#define _55_ELC_EVENT_SIG_RSPI0_TX_EMPTY                 (0x55U) /* RSPI0 transmit data empty */
#define _56_ELC_EVENT_SIG_RSPI0_TX_END                   (0x56U) /* RSPI0 transmit end */
#define _58_ELC_EVENT_SIG_S12ADC0_CONV_END               (0x58U) /* A/D conversion end signal */
#define _5B_ELC_EVENT_SIG_LVD1_DETECT                    (0x5BU) /* LVD1 voltage detection signal */
#define _5C_ELC_EVENT_SIG_LVD2_DETECT                    (0x5CU) /* LVD2 voltage detection signal */
#define _5D_ELC_EVENT_SIG_DMAC0_TX_END                   (0x5DU) /* DMAC0 transfer end */
#define _5E_ELC_EVENT_SIG_DMAC1_TX_END                   (0x5EU) /* DMAC1 transfer end */
#define _5F_ELC_EVENT_SIG_DMAC2_TX_END                   (0x5FU) /* DMAC2 transfer end */
#define _60_ELC_EVENT_SIG_DMAC3_TX_END                   (0x60U) /* DMAC3 transfer end */
#define _61_ELC_EVENT_SIG_DTC_TX_END                     (0x61U) /* DTC transfer end signal */
#define _62_ELC_EVENT_SIG_CGC_OSC_STOP                   (0x62U) /* Oscillation stop detection of CGC */
#define _63_ELC_EVENT_SIG_PORTGRP1_INPUT_EDGE            (0x63U) /* Input edge detection of input port group 1 */
#define _64_ELC_EVENT_SIG_PORTGRP2_INPUT_EDGE            (0x64U) /* Input edge detection of input port group 2 */
#define _65_ELC_EVENT_SIG_SINGLEPORT0_INPUT_EDGE         (0x65U) /* Input edge detection of single input port 0 */
#define _66_ELC_EVENT_SIG_SINGLEPORT1_INPUT_EDGE         (0x66U) /* Input edge detection of single input port 1 */
#define _67_ELC_EVENT_SIG_SINGLEPORT2_INPUT_EDGE         (0x67U) /* Input edge detection of single input port 2 */
#define _68_ELC_EVENT_SIG_SINGLEPORT3_INPUT_EDGE         (0x68U) /* Input edge detection of single input port 3 */
#define _69_ELC_EVENT_SIG_ELC_SW_EVENT                   (0x69U) /* Software event signal */
#define _6A_ELC_EVENT_SIG_DOC_COND_MET                   (0x6AU) /* DOC data operation condition met signal */
#define _6C_ELC_EVENT_SIG_S12ADC1_CONV_END               (0x6CU) /* S12AD1 A/D conversion end */
#define _7E_ELC_EVENT_SIG_CMTW0_MATCH                    (0x7EU) /* CMTW channel 0 compare match */
#define _80_ELC_EVENT_SIG_GPT0_MATCHA                    (0x80U) /* GPT0 compare match A */
#define _81_ELC_EVENT_SIG_GPT0_MATCHB                    (0x81U) /* GPT0 compare match B */
#define _82_ELC_EVENT_SIG_GPT0_MATCHC                    (0x82U) /* GPT0 compare match C */
#define _83_ELC_EVENT_SIG_GPT0_MATCHD                    (0x83U) /* GPT0 compare match D */
#define _84_ELC_EVENT_SIG_GPT0_MATCHE                    (0x84U) /* GPT0 compare match E */
#define _85_ELC_EVENT_SIG_GPT0_MATCHF                    (0x85U) /* GPT0 compare match F */
#define _86_ELC_EVENT_SIG_GPT0_OVF                       (0x86U) /* GPT0 overflow */
#define _87_ELC_EVENT_SIG_GPT0_UDF                       (0x87U) /* GPT0 underflow */
#define _C6_ELC_EVENT_SIG_GPT0_ADSTRA                    (0xC6U) /* GPT0 A/D converter start request A */
#define _C7_ELC_EVENT_SIG_GPT0_ADSTRB                    (0xC7U) /* GPT0 A/D converter start request B */
#define _88_ELC_EVENT_SIG_GPT1_MATCHA                    (0x88U) /* GPT1 compare match A */
#define _89_ELC_EVENT_SIG_GPT1_MATCHB                    (0x89U) /* GPT1 compare match B */
#define _8A_ELC_EVENT_SIG_GPT1_MATCHC                    (0x8AU) /* GPT1 compare match C */
#define _8B_ELC_EVENT_SIG_GPT1_MATCHD                    (0x8BU) /* GPT1 compare match D */
#define _8C_ELC_EVENT_SIG_GPT1_MATCHE                    (0x8CU) /* GPT1 compare match E */
#define _8D_ELC_EVENT_SIG_GPT1_MATCHF                    (0x8DU) /* GPT1 compare match F */
#define _8E_ELC_EVENT_SIG_GPT1_OVF                       (0x8EU) /* GPT1 overflow */
#define _8F_ELC_EVENT_SIG_GPT1_UDF                       (0x8FU) /* GPT1 underflow */
#define _C8_ELC_EVENT_SIG_GPT1_ADSTRA                    (0xC8U) /* GPT1 A/D converter start request A */
#define _C9_ELC_EVENT_SIG_GPT1_ADSTRB                    (0xC9U) /* GPT1 A/D converter start request B */
#define _90_ELC_EVENT_SIG_GPT2_MATCHA                    (0x90U) /* GPT2 compare match A */
#define _91_ELC_EVENT_SIG_GPT2_MATCHB                    (0x91U) /* GPT2 compare match B */
#define _92_ELC_EVENT_SIG_GPT2_MATCHC                    (0x92U) /* GPT2 compare match C */
#define _93_ELC_EVENT_SIG_GPT2_MATCHD                    (0x93U) /* GPT2 compare match D */
#define _94_ELC_EVENT_SIG_GPT2_MATCHE                    (0x94U) /* GPT2 compare match E */
#define _95_ELC_EVENT_SIG_GPT2_MATCHF                    (0x95U) /* GPT2 compare match F */
#define _96_ELC_EVENT_SIG_GPT2_OVF                       (0x96U) /* GPT2 overflow */
#define _97_ELC_EVENT_SIG_GPT2_UDF                       (0x97U) /* GPT2 underflow */
#define _CA_ELC_EVENT_SIG_GPT2_ADSTRA                    (0xCAU) /* GPT2 A/D converter start request A */
#define _CB_ELC_EVENT_SIG_GPT2_ADSTRB                    (0xCBU) /* GPT2 A/D converter start request B */
#define _98_ELC_EVENT_SIG_GPT3_MATCHA                    (0x98U) /* GPT3 compare match A */
#define _99_ELC_EVENT_SIG_GPT3_MATCHB                    (0x99U) /* GPT3 compare match B */
#define _9A_ELC_EVENT_SIG_GPT3_MATCHC                    (0x9AU) /* GPT3 compare match C */
#define _9B_ELC_EVENT_SIG_GPT3_MATCHD                    (0x9BU) /* GPT3 compare match D */
#define _9C_ELC_EVENT_SIG_GPT3_MATCHE                    (0x9CU) /* GPT3 compare match E */
#define _9D_ELC_EVENT_SIG_GPT3_MATCHF                    (0x9DU) /* GPT3 compare match F */
#define _9E_ELC_EVENT_SIG_GPT3_OVF                       (0x9EU) /* GPT3 overflow */
#define _9F_ELC_EVENT_SIG_GPT3_UDF                       (0x9FU) /* GPT3 underflow */
#define _CC_ELC_EVENT_SIG_GPT3_ADSTRA                    (0xCCU) /* GPT3 A/D converter start request A */
#define _CD_ELC_EVENT_SIG_GPT3_ADSTRB                    (0xCDU) /* GPT3 A/D converter start request B */
#define _AC_ELC_EVENT_SIG_TPU0_MATCHA                    (0xACU) /* TPU0 compare match A */
#define _AD_ELC_EVENT_SIG_TPU0_MATCHB                    (0xADU) /* TPU0 compare match B */
#define _AE_ELC_EVENT_SIG_TPU0_MATCHC                    (0xAEU) /* TPU0 compare match C */
#define _AF_ELC_EVENT_SIG_TPU0_MATCHD                    (0xAFU) /* TPU0 compare match D */
#define _B0_ELC_EVENT_SIG_TPU0_OVF                       (0xB0U) /* TPU0 overflow */
#define _B1_ELC_EVENT_SIG_TPU1_MATCHA                    (0xB1U) /* TPU1 compare match A */
#define _B2_ELC_EVENT_SIG_TPU1_MATCHB                    (0xB2U) /* TPU1 compare match B */
#define _B3_ELC_EVENT_SIG_TPU1_OVF                       (0xB3U) /* TPU1 overflow */
#define _B4_ELC_EVENT_SIG_TPU1_UDF                       (0xB4U) /* TPU1 underflow */
#define _B5_ELC_EVENT_SIG_TPU2_MATCHA                    (0xB5U) /* TPU2 compare match A */
#define _B6_ELC_EVENT_SIG_TPU2_MATCHB                    (0xB6U) /* TPU2 compare match A */
#define _B7_ELC_EVENT_SIG_TPU2_OVF                       (0xB7U) /* TPU2 overflow */
#define _B8_ELC_EVENT_SIG_TPU2_UDF                       (0xB8U) /* TPU2 underflow */
#define _B9_ELC_EVENT_SIG_TPU3_MATCHA                    (0xB9U) /* TPU3 compare match A */
#define _BA_ELC_EVENT_SIG_TPU3_MATCHB                    (0xBAU) /* TPU3 compare match B */
#define _BB_ELC_EVENT_SIG_TPU3_MATCHC                    (0xBBU) /* TPU3 compare match C */
#define _BC_ELC_EVENT_SIG_TPU3_MATCHD                    (0xBCU) /* TPU3 compare match D */
#define _BD_ELC_EVENT_SIG_TPU3_OVF                       (0xBDU) /* TPU3 overflow */

/*
    Event Link Option Setting Register x (ELOPx)
*/
/* Operation Select Bit (_MD[1:0]) */
#define _00_ELC_OPERATION_COUNT_START                    (0x00U) /* Counting is started */
#define _01_ELC_OPERATION_COUNT_RESTART                  (0x01U) /* Counting is restarted */
#define _02_ELC_OPERATION_INPUT_CAPTURE                  (0x02U) /* Input capture */
#define _02_ELC_OPERATION_EVENT_COUNTER                  (0x02U) /* Event counter */
#define _03_ELC_OPERATION_EVENT_DISABLE                  (0x03U) /* Event is disabled */

/*
    Port Group Control Register n (PGCn)
*/
/* Event Output Edge Select (PGCI[1:0]) */
#define _00_ELC_INPUT_PORTGROUP_EDGE_RISING              (0x00U) /* Generate event upon rising edge */
#define _01_ELC_INPUT_PORTGROUP_EDGE_FALLING             (0x01U) /* Generate event upon falling edge */
#define _02_ELC_INPUT_PORTGROUP_EDGE_BOTH                (0x02U) /* Generate event upon both edges */
/* PDBF Overwrite (PGCOVE) */
#define _00_ELC_INPUT_PORTGROUP_BUFFER_OVERWRITE_DISABLE (0x00U) /* Overwriting PDBF register is disabled */
#define _04_ELC_INPUT_PORTGROUP_BUFFER_OVERWRITE_ENABLE  (0x04U) /* Overwriting PDBF register is enabled */
/* Port Group Operation Select (PGCO[2:0]) */
#define _00_ELC_OPERATION_OUTPUT_PORTGROUP_OUT0          (0x00U) /* 0 is output on event */
#define _10_ELC_OPERATION_OUTPUT_PORTGROUP_OUT1          (0x10U) /* 1 is output on event */
#define _20_ELC_OPERATION_OUTPUT_PORTGROUP_TOGGLE        (0x20U) /* The toggled value is output on event */
#define _30_ELC_OPERATION_OUTPUT_PORTGROUP_BUFFER        (0x30U) /* The buffer value is output on event */
#define _40_ELC_OPERATION_OUTPUT_PORTGROUP_ROTATE        (0x40U) /* Rotate the bit value (MSB to LSB)in the group */
#define _88_ELC_PGC_DEFAULT                              (0x88U) /* Default value */

/*
    Event Link Port Setting Register n (PELn)
*/
/* Port Number Specification (PSP[1:0]) */
#define _08_ELC_SINGLE_PORT_PORT_NUMBER_PORTB            (0x08U) /* Port B */
#define _10_ELC_SINGLE_PORT_PORT_NUMBER_PORTE            (0x10U) /* Port E */
/* Event Link Specification (PSM[1:0]) */
#define _00_ELC_OPERATION_SINGLE_PORT_OUT0               (0x00U) /* 0 is output on event */
#define _20_ELC_OPERATION_SINGLE_PORT_OUT1               (0x20U) /* 1 is output on event */
#define _40_ELC_OPERATION_SINGLE_PORT_TOGGLE             (0x40U) /* The toggled value is output on event */
#define _00_ELC_SINGLE_PORT_EDGE_RISING                  (0x00U) /* Generate event upon rising edge */
#define _20_ELC_SINGLE_PORT_EDGE_FALLING                 (0x20U) /* Generate event upon falling edge */
#define _40_ELC_SINGLE_PORT_EDGE_BOTH                    (0x40U) /* Generate event upon both edges */
/* Bit Number Specification (PSB[2:0]) */
#define _00_ELC_SINGLE_PORT_PORT0                        (0x00U) /* Port pin 0 is selected */
#define _01_ELC_SINGLE_PORT_PORT1                        (0x01U) /* Port pin 1 is selected */
#define _02_ELC_SINGLE_PORT_PORT2                        (0x02U) /* Port pin 2 is selected */
#define _03_ELC_SINGLE_PORT_PORT3                        (0x03U) /* Port pin 3 is selected */
#define _04_ELC_SINGLE_PORT_PORT4                        (0x04U) /* Port pin 4 is selected */
#define _05_ELC_SINGLE_PORT_PORT5                        (0x05U) /* Port pin 5 is selected */
#define _06_ELC_SINGLE_PORT_PORT6                        (0x06U) /* Port pin 6 is selected */
#define _07_ELC_SINGLE_PORT_PORT7                        (0x07U) /* Port pin 7 is selected */
#define _80_ELC_PEL_DEFAULT                              (0x80U) /* Default value */

/* 
    Event Link Software Event Generation Register (ELSEGR)
*/
#define _3E_ELC_SOFTWARE_EVENT_GENERATION_UNLOCK         (0x3EU) /* Enable writing to WE bit */
#define _7E_ELC_SOFTWARE_EVENT_GENERATION_ENABLE         (0x7EU) /* Enable writing to SEG bit */
#define _7F_ELC_GENERATE_SOFTWARE_EVENT                  (0x7FU) /* Write this value to generate software event */

/*
    Interrupt Source Priority Register n (IPRn)
*/
#define _00_ELC_PRIORITY_LEVEL0                          (0x00U) /* Level 0 (interrupt disabled) */
#define _01_ELC_PRIORITY_LEVEL1                          (0x01U) /* Level 1 */
#define _02_ELC_PRIORITY_LEVEL2                          (0x02U) /* Level 2 */
#define _03_ELC_PRIORITY_LEVEL3                          (0x03U) /* Level 3 */
#define _04_ELC_PRIORITY_LEVEL4                          (0x04U) /* Level 4 */
#define _05_ELC_PRIORITY_LEVEL5                          (0x05U) /* Level 5 */
#define _06_ELC_PRIORITY_LEVEL6                          (0x06U) /* Level 6 */
#define _07_ELC_PRIORITY_LEVEL7                          (0x07U) /* Level 7 */
#define _08_ELC_PRIORITY_LEVEL8                          (0x08U) /* Level 8 */
#define _09_ELC_PRIORITY_LEVEL9                          (0x09U) /* Level 9 */
#define _0A_ELC_PRIORITY_LEVEL10                         (0x0AU) /* Level 10 */
#define _0B_ELC_PRIORITY_LEVEL11                         (0x0BU) /* Level 11 */
#define _0C_ELC_PRIORITY_LEVEL12                         (0x0CU) /* Level 12 */
#define _0D_ELC_PRIORITY_LEVEL13                         (0x0DU) /* Level 13 */
#define _0E_ELC_PRIORITY_LEVEL14                         (0x0EU) /* Level 14 */
#define _0F_ELC_PRIORITY_LEVEL15                         (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

