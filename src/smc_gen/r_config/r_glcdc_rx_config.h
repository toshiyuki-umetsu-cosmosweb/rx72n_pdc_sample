/* Generated configuration header file - do not edit */
/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2017-2020 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_glcdc_rx_config_reference.h
 * Version      : 1.50
 * Description  : GLCDC configuration header file
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.10.2017 1.00     First Release
 *         : 30.06.2020 1.40     Added definition of GLCDC_CFG_CONFIGURATION_MODE and detailed setting of GLCDC.
 *                               Supports cooperation with QE for Display[RX].
 *         : 11.11.2020 1.50     Remove #if directive switching by the value of GLCDC_CFG_CONFIGURATION_MODE.
 **********************************************************************************************************************/

#ifndef R_GLCDC_RX_CONFIG_H
#define R_GLCDC_RX_CONFIG_H

/***********************************************************************************************************************
 * Configuration Options
 **********************************************************************************************************************/

/* Select whether to perform parameter check processing or not.
 * 1: Enable parameter check
 *    Include parameter checking process when building code
 * 0: Disable parameter check
 *    Exclude parameter checking process when building code
 */
#define GLCDC_CFG_PARAM_CHECKING_ENABLE (0)

/* Interrupt priority level of GLCDC interrupt (group AL1 interrupt)
 *  Setting range: 0 to 15
 */
#define GLCDC_CFG_INTERRUPT_PRIORITY_LEVEL (5)

/* Select whether to set the GLCDC settings programmatically or using the configuration options.
 * 1: Setting by configuration options
 *    Set the value of parameter with the configuration options defined in this file
 * 0: Setting by user programming (default)
 *    Set the value for parameter of the GLCDC structure in the user program.
 */
#define GLCDC_CFG_CONFIGURATION_MODE (1)

/**********************************************************************************************************************
 * Configuration Options for GLCDC parameters setting
 *********************************************************************************************************************/
 
#if defined(QE_DISPLAY_CONFIGURATION)
/*
 *  Header files generated by QE for Display[RX]
 *  Enabled when using QE for Display[RX]
 *  QE for Display[RX] adds the macro definition "QE_DISPLAY_CONFIGURATION" to the compile options.
 */

/* Change the file name if necessary. */
#include "r_image_config.h"
#include "r_lcd_timing.h"

#endif

/*********************************************************************************************************************/

/*
 * Output setting
 */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_W_HFP                       (48)                        /* output.htiming.front_porch */
#define LCD_CH0_W_HBP                       (612)                        /* output.htiming.back_porch */
#define LCD_CH0_DISP_HW                     (1280)                       /* output.htiming.display_cyc */
#define LCD_CH0_W_HSYNC                     (60)                        /* output.htiming.sync_width */

#define LCD_CH0_W_VFP                       (8)                         /* output.vtiming.front_porch */
#define LCD_CH0_W_VBP                       (10)                         /* output.vtiming.back_porch */
#define LCD_CH0_DISP_VW                     (480)                       /* output.vtiming.display_cyc */
#define LCD_CH0_W_VSYNC                     (2)                         /* output.vtiming.sync_width */

#define LCD_CH0_OUT_FORMAT                  (GLCDC_OUT_FORMAT_24BITS_RGB888)  /* output.format */
#define LCD_CH0_OUT_ENDIAN                  (GLCDC_ENDIAN_LITTLE)             /* output.endian */
#define LCD_CH0_OUT_COLOR_ORDER             (GLCDC_COLOR_ORDER_RGB)           /* output.color_order */
#define LCD_CH0_OUT_EDGE                    (GLCDC_SIGNAL_SYNC_EDGE_RISING)   /* output.sync_edge */
#define LCD_CH0_OUT_BG_COLOR                (0x00000000)                      /* output.bg_color */

#define LCD_CH0_TCON_PIN_HSYNC              (GLCDC_TCON_PIN_0)          /* output.tcon_hsync */
#define LCD_CH0_TCON_PIN_VSYNC              (GLCDC_TCON_PIN_1)          /* output.tcon_vsync */
#define LCD_CH0_TCON_PIN_DE                 (GLCDC_TCON_PIN_NON)          /* output.tcon_de */

#define LCD_CH0_TCON_POL_HSYNC              (GLCDC_SIGNAL_POLARITY_LOACTIVE)  /* output.hsync_polarity */
#define LCD_CH0_TCON_POL_VSYNC              (GLCDC_SIGNAL_POLARITY_LOACTIVE)  /* output.vsync_polarity */
#define LCD_CH0_TCON_POL_DE                 (GLCDC_SIGNAL_POLARITY_HIACTIVE)  /* output.data_enable_polarity */

#define LCD_CH0_OUT_CLK_DIV_RATIO           (GLCDC_PANEL_CLK_DIVISOR_8)      /* output.clock_div_ratio */
#endif

/*
 * Graphic layer 2 setting
 */
                                                                        /* Configuration structure member */
/* Input */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_IN_GR2_PBASE                (0x00800000)                /* input[GR2].p_base */
#define LCD_CH0_IN_GR2_HSIZE                (480)                       /* input[GR2].hsize */
#define LCD_CH0_IN_GR2_VSIZE                (272)                       /* input[GR2].vsize */
#define LCD_CH0_IN_GR2_LINEOFFSET           (960)                       /* input[GR2].offset */
#define LCD_CH0_IN_GR2_FORMAT               (GLCDC_IN_FORMAT_16BITS_RGB565)  /* input[GR2].format */
#define LCD_CH0_IN_GR2_COORD_X              (0)                         /* input[GR2].coordinate.x */
#define LCD_CH0_IN_GR2_COORD_Y              (0)                         /* input[GR2].coordinate.y */
#endif

#define LCD_CH0_IN_GR2_FRAME_EDGE           (false)                     /* input[GR2].frame_edge         */
#define LCD_CH0_IN_GR2_BG_COLOR             (0x00000000)                /* input[GR2].bg_color           */


/* Blending */
#define LCD_CH0_BLEND_GR2_VISIBLE           (false)                      /* blend[GR2].visible            */
#define LCD_CH0_BLEND_GR2_BLEND_CONTROL     (GLCDC_BLEND_CONTROL_NONE)  /* blend[GR2].blend_control      */
#define LCD_CH0_BLEND_GR2_FRAME_EDGE        (false)                     /* blend[GR2].frame_edge         */
#define LCD_CH0_BLEND_GR2_FIXED_BLEND_VALUE (255)                       /* blend[GR2].fixed_blend_value  */
#define LCD_CH0_BLEND_GR2_FADE_SPEED        (255)                       /* blend[GR2].fade_speed         */
#define LCD_CH0_BLEND_GR2_START_COORD_X     (0)                         /* blend[GR2].start_coordinate.x */
#define LCD_CH0_BLEND_GR2_START_COORD_Y     (0)                         /* blend[GR2].start_coordinate.y */
#define LCD_CH0_BLEND_GR2_END_COORD_X       (0)                         /* blend[GR2].end_coordinate.x   */
#define LCD_CH0_BLEND_GR2_END_COORD_Y       (0)                         /* blend[GR2].end_coordinate.y   */

/* Chromakey */
#define LCD_CH0_CHROMAKEY_GR2_ENABLE        (false)                     /* chromakey[GR2].enable         */
#define LCD_CH0_CHROMAKEY_GR2_BEFORE_ARGB   (0x00000000)                /* chromakey[GR2].before.argb    */
#define LCD_CH0_CHROMAKEY_GR2_AFTER_ARGB    (0x00000000)                /* chromakey[GR2].after.argb     */

/* Color look-up table */
#define LCD_CH0_CLUT_GR2_ENABLE             (false)                     /* clut[GR2].enable              */
#define LCD_CH0_CLUT_GR2_PBASE              (FIT_NO_PTR)                /* clut[GR2].p_base              */
#define LCD_CH0_CLUT_GR2_START              (0)                         /* clut[GR2].start               */
#define LCD_CH0_CLUT_GR2_SIZE               (256)                       /* clut[GR2].size                */


/*
 * Graphic layer 1 setting
 */

/* Input */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_IN_GR1_PBASE                (NULL)                      /* input[GR1].p_base */
#define LCD_CH0_IN_GR1_HSIZE                (480)                       /* input[GR1].hsize */
#define LCD_CH0_IN_GR1_VSIZE                (272)                       /* input[GR1].vsize */
#define LCD_CH0_IN_GR1_LINEOFFSET           (960)                       /* input[GR1].offset */
#define LCD_CH0_IN_GR1_FORMAT               (GLCDC_IN_FORMAT_16BITS_RGB565)  /* input[GR1].format */
#define LCD_CH0_IN_GR1_COORD_X              (0)                         /* input[GR1].coordinate.x */
#define LCD_CH0_IN_GR1_COORD_Y              (0)                         /* input[GR1].coordinate.y */
#endif

#define LCD_CH0_IN_GR1_FRAME_EDGE           (false)                     /* input[GR1].frame_edge         */
#define LCD_CH0_IN_GR1_BG_COLOR             (0x00000000)                /* input[GR1].bg_color           */


/* Blending */
#define LCD_CH0_BLEND_GR1_VISIBLE           (false)                      /* blend[GR1].visible            */
#define LCD_CH0_BLEND_GR1_BLEND_CONTROL     (GLCDC_BLEND_CONTROL_NONE)  /* blend[GR1].blend_control      */
#define LCD_CH0_BLEND_GR1_FRAME_EDGE        (false)                     /* blend[GR1].frame_edge         */
#define LCD_CH0_BLEND_GR1_FIXED_BLEND_VALUE (255)                       /* blend[GR1].fixed_blend_value  */
#define LCD_CH0_BLEND_GR1_FADE_SPEED        (255)                       /* blend[GR1].fade_speed         */
#define LCD_CH0_BLEND_GR1_START_COORD_X     (0)                         /* blend[GR1].start_coordinate.x */
#define LCD_CH0_BLEND_GR1_START_COORD_Y     (0)                         /* blend[GR1].start_coordinate.y */
#define LCD_CH0_BLEND_GR1_END_COORD_X       (0)                         /* blend[GR1].end_coordinate.x   */
#define LCD_CH0_BLEND_GR1_END_COORD_Y       (0)                         /* blend[GR1].end_coordinate.y   */

/* Chromakey */
#define LCD_CH0_CHROMAKEY_GR1_ENABLE        (false)                     /* chromakey[GR1].enable         */
#define LCD_CH0_CHROMAKEY_GR1_BEFORE_ARGB   (0x00000000)                /* chromakey[GR1].before.argb    */
#define LCD_CH0_CHROMAKEY_GR1_AFTER_ARGB    (0x00000000)                /* chromakey[GR1].after.argb     */

/* Color look-up table */
#define LCD_CH0_CLUT_GR1_ENABLE             (false)                     /* clut[GR1].enable              */
#define LCD_CH0_CLUT_GR1_PBASE              (FIT_NO_PTR)                /* clut[GR1].p_base              */
#define LCD_CH0_CLUT_GR1_START              (0)                         /* clut[GR1].start               */
#define LCD_CH0_CLUT_GR1_SIZE               (256)                       /* clut[GR1].size                */


/*
 * Detection & interrupt setting
 */

/* Detection */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_DETECT_VPOS                 (false)                     /* detection.vpos_detect */
#endif

#define LCD_CH0_DETECT_GR1UF                (false)                     /* detection.gr1uf_detect   */
#define LCD_CH0_DETECT_GR2UF                (false)                     /* detection.gr2uf_detect   */

/* Interrupt */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_INTERRUPT_VPOS_ENABLE       (false)                     /* interrupt.vpos_enable */
#endif

#define LCD_CH0_INTERRUPT_GR1UF_ENABLE      (false)                     /* interrupt.gr1uf_enable   */
#define LCD_CH0_INTERRUPT_GR2UF_ENABLE      (false)                     /* interrupt.gr2uf_enable   */

/* Callback function */
#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */
#define LCD_CH0_CALLBACK_ENABLE             (false)
#define LCD_CH0_PCALLBACK                   (glcdc_callback)            /* pcallback */
#endif


/*
 * Output correction setting
 */

#if !defined(QE_DISPLAY_CONFIGURATION) /* This option is set in QE for Display[RX] when using QE for Display[RX] */

/* Calibration Route Setting */
#define IMGC_OUTCTL_CALIB_ROUTE              (GLCDC_BRIGHTNESS_CONTRAST_TO_GAMMA) /* output.correction_proc_order */

/* Brightness Setting */
#define IMGC_BRIGHT_OUTCTL_ACTIVE           (false)                      /* output.brightness.enable */
#define IMGC_BRIGHT_OUTCTL_OFFSET_G         (512)                       /* output.brightness.g */
#define IMGC_BRIGHT_OUTCTL_OFFSET_B         (512)                       /* output.brightness.b */
#define IMGC_BRIGHT_OUTCTL_OFFSET_R         (512)                       /* output.brightness.r */

/* Contrast Setting */
#define IMGC_CONTRAST_OUTCTL_ACTIVE         (false)                      /* output.contrast.enable */
#define IMGC_CONTRAST_OUTCTL_GAIN_G         (128)                       /* output.contrast.g */
#define IMGC_CONTRAST_OUTCTL_GAIN_B         (128)                       /* output.contrast.b */
#define IMGC_CONTRAST_OUTCTL_GAIN_R         (128)                       /* output.contrast.r */

/* Gamma correction */
#define IMGC_GAMMA_ACTIVE                   (false)                      /* output.gamma.enable */
/* - green data table - */
#define IMGC_GAMMA_G_GAIN_00                (1317)
#define IMGC_GAMMA_G_GAIN_01                (1157)
#define IMGC_GAMMA_G_GAIN_02                (1103)
#define IMGC_GAMMA_G_GAIN_03                (1069)
#define IMGC_GAMMA_G_GAIN_04                (1045)
#define IMGC_GAMMA_G_GAIN_05                (1026)
#define IMGC_GAMMA_G_GAIN_06                (1010)
#define IMGC_GAMMA_G_GAIN_07                (997)
#define IMGC_GAMMA_G_GAIN_08                (986)
#define IMGC_GAMMA_G_GAIN_09                (976)
#define IMGC_GAMMA_G_GAIN_10                (967)
#define IMGC_GAMMA_G_GAIN_11                (959)
#define IMGC_GAMMA_G_GAIN_12                (952)
#define IMGC_GAMMA_G_GAIN_13                (945)
#define IMGC_GAMMA_G_GAIN_14                (939)
#define IMGC_GAMMA_G_GAIN_15                (919)
#define IMGC_GAMMA_G_TH_01                  (64)
#define IMGC_GAMMA_G_TH_02                  (128)
#define IMGC_GAMMA_G_TH_03                  (192)
#define IMGC_GAMMA_G_TH_04                  (256)
#define IMGC_GAMMA_G_TH_05                  (320)
#define IMGC_GAMMA_G_TH_06                  (384)
#define IMGC_GAMMA_G_TH_07                  (448)
#define IMGC_GAMMA_G_TH_08                  (512)
#define IMGC_GAMMA_G_TH_09                  (576)
#define IMGC_GAMMA_G_TH_10                  (640)
#define IMGC_GAMMA_G_TH_11                  (704)
#define IMGC_GAMMA_G_TH_12                  (768)
#define IMGC_GAMMA_G_TH_13                  (832)
#define IMGC_GAMMA_G_TH_14                  (896)
#define IMGC_GAMMA_G_TH_15                  (960)
/* - blue data table - */
#define IMGC_GAMMA_B_GAIN_00                (1317)
#define IMGC_GAMMA_B_GAIN_01                (1157)
#define IMGC_GAMMA_B_GAIN_02                (1103)
#define IMGC_GAMMA_B_GAIN_03                (1069)
#define IMGC_GAMMA_B_GAIN_04                (1045)
#define IMGC_GAMMA_B_GAIN_05                (1026)
#define IMGC_GAMMA_B_GAIN_06                (1010)
#define IMGC_GAMMA_B_GAIN_07                (997)
#define IMGC_GAMMA_B_GAIN_08                (986)
#define IMGC_GAMMA_B_GAIN_09                (976)
#define IMGC_GAMMA_B_GAIN_10                (967)
#define IMGC_GAMMA_B_GAIN_11                (959)
#define IMGC_GAMMA_B_GAIN_12                (952)
#define IMGC_GAMMA_B_GAIN_13                (945)
#define IMGC_GAMMA_B_GAIN_14                (939)
#define IMGC_GAMMA_B_GAIN_15                (919)
#define IMGC_GAMMA_B_TH_01                  (64)
#define IMGC_GAMMA_B_TH_02                  (128)
#define IMGC_GAMMA_B_TH_03                  (192)
#define IMGC_GAMMA_B_TH_04                  (256)
#define IMGC_GAMMA_B_TH_05                  (320)
#define IMGC_GAMMA_B_TH_06                  (384)
#define IMGC_GAMMA_B_TH_07                  (448)
#define IMGC_GAMMA_B_TH_08                  (512)
#define IMGC_GAMMA_B_TH_09                  (576)
#define IMGC_GAMMA_B_TH_10                  (640)
#define IMGC_GAMMA_B_TH_11                  (704)
#define IMGC_GAMMA_B_TH_12                  (768)
#define IMGC_GAMMA_B_TH_13                  (832)
#define IMGC_GAMMA_B_TH_14                  (896)
#define IMGC_GAMMA_B_TH_15                  (960)
/* - red data table - */
#define IMGC_GAMMA_R_GAIN_00                (1317)
#define IMGC_GAMMA_R_GAIN_01                (1157)
#define IMGC_GAMMA_R_GAIN_02                (1103)
#define IMGC_GAMMA_R_GAIN_03                (1069)
#define IMGC_GAMMA_R_GAIN_04                (1045)
#define IMGC_GAMMA_R_GAIN_05                (1026)
#define IMGC_GAMMA_R_GAIN_06                (1010)
#define IMGC_GAMMA_R_GAIN_07                (997)
#define IMGC_GAMMA_R_GAIN_08                (986)
#define IMGC_GAMMA_R_GAIN_09                (976)
#define IMGC_GAMMA_R_GAIN_10                (967)
#define IMGC_GAMMA_R_GAIN_11                (959)
#define IMGC_GAMMA_R_GAIN_12                (952)
#define IMGC_GAMMA_R_GAIN_13                (945)
#define IMGC_GAMMA_R_GAIN_14                (939)
#define IMGC_GAMMA_R_GAIN_15                (919)
#define IMGC_GAMMA_R_TH_01                  (64)
#define IMGC_GAMMA_R_TH_02                  (128)
#define IMGC_GAMMA_R_TH_03                  (192)
#define IMGC_GAMMA_R_TH_04                  (256)
#define IMGC_GAMMA_R_TH_05                  (320)
#define IMGC_GAMMA_R_TH_06                  (384)
#define IMGC_GAMMA_R_TH_07                  (448)
#define IMGC_GAMMA_R_TH_08                  (512)
#define IMGC_GAMMA_R_TH_09                  (576)
#define IMGC_GAMMA_R_TH_10                  (640)
#define IMGC_GAMMA_R_TH_11                  (704)
#define IMGC_GAMMA_R_TH_12                  (768)
#define IMGC_GAMMA_R_TH_13                  (832)
#define IMGC_GAMMA_R_TH_14                  (896)
#define IMGC_GAMMA_R_TH_15                  (960)

/* Dither Process */
#define IMGC_DITHER_ACTIVE                  (false)                         /* output.dithering.dithering_on */
#define IMGC_DITHER_MODE                    (GLCDC_DITHERING_MODE_TRUNCATE) /* output.dithering.dithering_mode */
#define IMGC_DITHER_2X2_PA                  (GLCDC_DITHERING_PATTERN_11)    /* output.dithering.dithering_pattern_a */
#define IMGC_DITHER_2X2_PB                  (GLCDC_DITHERING_PATTERN_00)    /* output.dithering.dithering_pattern_b */
#define IMGC_DITHER_2X2_PC                  (GLCDC_DITHERING_PATTERN_10)    /* output.dithering.dithering_pattern_c */
#define IMGC_DITHER_2X2_PD                  (GLCDC_DITHERING_PATTERN_01)    /* output.dithering.dithering_pattern_d */

#endif /* !defined(QE_DISPLAY_CONFIGURATION) */

#endif /* R_GLCDC_RX_CONFIG_H */

/* End of File */
