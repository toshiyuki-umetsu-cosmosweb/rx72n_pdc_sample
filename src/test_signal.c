/**
 * @file テスト信号インタフェース
 *       GLCDCを使って640x480@30fps YUYV 信号を出すようなモジュール。
 *       RAMが足りないので、単色データを出すだけとする。
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdint.h>
#include <platform.h>
#include <r_glcdc_rx_if.h>
#include <r_glcdc_rx_pinset.h>
#include "test_signal.h"

/**
 * イベント処理を実行するかどうか。
 * FITモジュールの解説にあるとおり、ソフトウェアリセット解除後、初回のみ意図しない通知が行われるため、
 * 初回だけ無視するためにフラグで管理する。
 */
static bool s_is_lcd_event_processing;

static void glcdc_callback(void *arg);

/**
 * @brief GLCDC Gamma R設定
 */
//@formatter:off
static const gamma_correction_t s_gamma_correction_R = {
    .gain = {
        IMGC_GAMMA_R_GAIN_00,
        IMGC_GAMMA_R_GAIN_01,
        IMGC_GAMMA_R_GAIN_02,
        IMGC_GAMMA_R_GAIN_03,
        IMGC_GAMMA_R_GAIN_04,
        IMGC_GAMMA_R_GAIN_05,
        IMGC_GAMMA_R_GAIN_06,
        IMGC_GAMMA_R_GAIN_07,
        IMGC_GAMMA_R_GAIN_08,
        IMGC_GAMMA_R_GAIN_09,
        IMGC_GAMMA_R_GAIN_10,
        IMGC_GAMMA_R_GAIN_11,
        IMGC_GAMMA_R_GAIN_12,
        IMGC_GAMMA_R_GAIN_13,
        IMGC_GAMMA_R_GAIN_14,
        IMGC_GAMMA_R_GAIN_15,
    },
    .threshold = {
        IMGC_GAMMA_R_TH_01,
        IMGC_GAMMA_R_TH_02,
        IMGC_GAMMA_R_TH_03,
        IMGC_GAMMA_R_TH_04,
        IMGC_GAMMA_R_TH_05,
        IMGC_GAMMA_R_TH_06,
        IMGC_GAMMA_R_TH_07,
        IMGC_GAMMA_R_TH_08,
        IMGC_GAMMA_R_TH_09,
        IMGC_GAMMA_R_TH_10,
        IMGC_GAMMA_R_TH_11,
        IMGC_GAMMA_R_TH_12,
        IMGC_GAMMA_R_TH_13,
        IMGC_GAMMA_R_TH_14,
        IMGC_GAMMA_R_TH_15,
    }
};
//@formatter:on
/**
 * @brief GLCDC Gamma G設定
 */
//@formatter:off
static const gamma_correction_t s_gamma_correction_G = {
    .gain = {
        IMGC_GAMMA_G_GAIN_00,
        IMGC_GAMMA_G_GAIN_01,
        IMGC_GAMMA_G_GAIN_02,
        IMGC_GAMMA_G_GAIN_03,
        IMGC_GAMMA_G_GAIN_04,
        IMGC_GAMMA_G_GAIN_05,
        IMGC_GAMMA_G_GAIN_06,
        IMGC_GAMMA_G_GAIN_07,
        IMGC_GAMMA_G_GAIN_08,
        IMGC_GAMMA_G_GAIN_09,
        IMGC_GAMMA_G_GAIN_10,
        IMGC_GAMMA_G_GAIN_11,
        IMGC_GAMMA_G_GAIN_12,
        IMGC_GAMMA_G_GAIN_13,
        IMGC_GAMMA_G_GAIN_14,
        IMGC_GAMMA_G_GAIN_15,
    },
    .threshold = {
        IMGC_GAMMA_G_TH_01,
        IMGC_GAMMA_G_TH_02,
        IMGC_GAMMA_G_TH_03,
        IMGC_GAMMA_G_TH_04,
        IMGC_GAMMA_G_TH_05,
        IMGC_GAMMA_G_TH_06,
        IMGC_GAMMA_G_TH_07,
        IMGC_GAMMA_G_TH_08,
        IMGC_GAMMA_G_TH_09,
        IMGC_GAMMA_G_TH_10,
        IMGC_GAMMA_G_TH_11,
        IMGC_GAMMA_G_TH_12,
        IMGC_GAMMA_G_TH_13,
        IMGC_GAMMA_G_TH_14,
        IMGC_GAMMA_G_TH_15,
    }
};
//@formatter:on
/**
 * @brief GLCDC Gamma B設定
 */
//@formatter:off
static const gamma_correction_t s_gamma_correction_B = {
    .gain = {
        IMGC_GAMMA_B_GAIN_00,
        IMGC_GAMMA_B_GAIN_01,
        IMGC_GAMMA_B_GAIN_02,
        IMGC_GAMMA_B_GAIN_03,
        IMGC_GAMMA_B_GAIN_04,
        IMGC_GAMMA_B_GAIN_05,
        IMGC_GAMMA_B_GAIN_06,
        IMGC_GAMMA_B_GAIN_07,
        IMGC_GAMMA_B_GAIN_08,
        IMGC_GAMMA_B_GAIN_09,
        IMGC_GAMMA_B_GAIN_10,
        IMGC_GAMMA_B_GAIN_11,
        IMGC_GAMMA_B_GAIN_12,
        IMGC_GAMMA_B_GAIN_13,
        IMGC_GAMMA_B_GAIN_14,
        IMGC_GAMMA_B_GAIN_15,
    },
    .threshold = {
        IMGC_GAMMA_R_TH_01,
        IMGC_GAMMA_R_TH_02,
        IMGC_GAMMA_R_TH_03,
        IMGC_GAMMA_R_TH_04,
        IMGC_GAMMA_R_TH_05,
        IMGC_GAMMA_R_TH_06,
        IMGC_GAMMA_R_TH_07,
        IMGC_GAMMA_R_TH_08,
        IMGC_GAMMA_R_TH_09,
        IMGC_GAMMA_R_TH_10,
        IMGC_GAMMA_R_TH_11,
        IMGC_GAMMA_R_TH_12,
        IMGC_GAMMA_R_TH_13,
        IMGC_GAMMA_R_TH_14,
        IMGC_GAMMA_R_TH_15,
    }
};
//@formatter:on

/**
 * @brief GLCDC設定
 */
//@formatter:off
static glcdc_cfg_t s_lcd_config = {
    .input = {
        {
            .p_base = (void*)((uintptr_t)(LCD_CH0_IN_GR1_PBASE)),
            .hsize = LCD_CH0_IN_GR1_HSIZE,
            .vsize = LCD_CH0_IN_GR1_VSIZE,
            .offset = LCD_CH0_IN_GR1_LINEOFFSET,
            .format = LCD_CH0_IN_GR1_FORMAT,
            .frame_edge = LCD_CH0_IN_GR1_FRAME_EDGE,
            .coordinate = {
                .x = LCD_CH0_IN_GR1_COORD_X,
                .y = LCD_CH0_IN_GR1_COORD_Y
            },
            .bg_color = LCD_CH0_IN_GR1_BG_COLOR
        },
        {
            .p_base = (void*)((uintptr_t)(LCD_CH0_IN_GR2_PBASE)),
            .hsize = LCD_CH0_IN_GR2_HSIZE,
            .vsize = LCD_CH0_IN_GR2_VSIZE,
            .offset = LCD_CH0_IN_GR2_LINEOFFSET,
            .format = LCD_CH0_IN_GR2_FORMAT,
            .frame_edge = LCD_CH0_IN_GR2_FRAME_EDGE,
            .coordinate = {
                .x = LCD_CH0_IN_GR2_COORD_X,
                .y = LCD_CH0_IN_GR2_COORD_Y
            },
            .bg_color = LCD_CH0_IN_GR2_BG_COLOR
        }
     },
     .output = {
         .htiming = {
              .display_cyc = LCD_CH0_DISP_HW,
              .front_porch = LCD_CH0_W_HFP,
              .back_porch = LCD_CH0_W_HBP,
              .sync_width = LCD_CH0_W_HSYNC
         },
         .vtiming = {
              .display_cyc = LCD_CH0_DISP_VW,
              .front_porch = LCD_CH0_W_VFP,
              .back_porch = LCD_CH0_W_VBP,
              .sync_width = LCD_CH0_W_VSYNC
         },
         .format = LCD_CH0_OUT_FORMAT,
         .endian = LCD_CH0_OUT_ENDIAN,
         .color_order = LCD_CH0_OUT_COLOR_ORDER,
         .sync_edge = LCD_CH0_OUT_EDGE,
         .bg_color = LCD_CH0_OUT_BG_COLOR,
         .brightness = {
             .enable = IMGC_BRIGHT_OUTCTL_ACTIVE,
             .r = IMGC_BRIGHT_OUTCTL_OFFSET_R,
             .g = IMGC_BRIGHT_OUTCTL_OFFSET_G,
             .b = IMGC_BRIGHT_OUTCTL_OFFSET_B,
         },
         .contrast = {
             .enable = IMGC_CONTRAST_OUTCTL_ACTIVE,
             .r = IMGC_CONTRAST_OUTCTL_GAIN_R,
             .g = IMGC_CONTRAST_OUTCTL_GAIN_G,
             .b = IMGC_CONTRAST_OUTCTL_GAIN_B,
         },
         .gamma = {
             .enable = IMGC_GAMMA_ACTIVE,
             .p_r = (gamma_correction_t*)(&s_gamma_correction_R),
             .p_g = (gamma_correction_t*)(&s_gamma_correction_G),
             .p_b = (gamma_correction_t*)(&s_gamma_correction_B),
         },
         .correction_proc_order = IMGC_OUTCTL_CALIB_ROUTE,
         .dithering = {
             .dithering_on = IMGC_DITHER_ACTIVE,
             .dithering_mode = IMGC_DITHER_MODE,
             .dithering_pattern_a = IMGC_DITHER_2X2_PA,
             .dithering_pattern_b = IMGC_DITHER_2X2_PB,
             .dithering_pattern_c = IMGC_DITHER_2X2_PC,
             .dithering_pattern_d = IMGC_DITHER_2X2_PD,
         },
         .tcon_hsync = LCD_CH0_TCON_PIN_HSYNC,
         .tcon_vsync = LCD_CH0_TCON_PIN_VSYNC,
         .tcon_de = LCD_CH0_TCON_PIN_DE,
         .data_enable_polarity = LCD_CH0_TCON_POL_DE,
         .hsync_polarity = LCD_CH0_TCON_POL_HSYNC,
         .vsync_polarity = LCD_CH0_TCON_POL_VSYNC,
         .clksrc = GLCDC_CLK_SRC_INTERNAL,
         .clock_div_ratio = LCD_CH0_OUT_CLK_DIV_RATIO,
         .serial_output_delay = 0,
         .serial_scan_direction = 0,

     },
    .blend = {
        {
            .blend_control = LCD_CH0_BLEND_GR1_BLEND_CONTROL,
            .visible = LCD_CH0_BLEND_GR1_VISIBLE,
            .frame_edge = LCD_CH0_BLEND_GR1_FRAME_EDGE,
            .fixed_blend_value = LCD_CH0_BLEND_GR1_FIXED_BLEND_VALUE,
            .fade_speed = LCD_CH0_BLEND_GR1_FADE_SPEED,
            .start_coordinate = {
                .x = LCD_CH0_BLEND_GR1_START_COORD_X,
                .y = LCD_CH0_BLEND_GR1_START_COORD_Y
            },
            .end_coordinate = {
                .x = LCD_CH0_BLEND_GR1_END_COORD_X,
                .y = LCD_CH0_BLEND_GR1_END_COORD_Y
            },
        },
        {
            .blend_control = LCD_CH0_BLEND_GR2_BLEND_CONTROL,
            .visible = LCD_CH0_BLEND_GR2_VISIBLE,
            .frame_edge = LCD_CH0_BLEND_GR2_FRAME_EDGE,
            .fixed_blend_value = LCD_CH0_BLEND_GR2_FIXED_BLEND_VALUE,
            .fade_speed = LCD_CH0_BLEND_GR2_FADE_SPEED,
            .start_coordinate = {
                .x = LCD_CH0_BLEND_GR2_START_COORD_X,
                .y = LCD_CH0_BLEND_GR2_START_COORD_Y
            },
            .end_coordinate = {
                .x = LCD_CH0_BLEND_GR2_END_COORD_X,
                .y = LCD_CH0_BLEND_GR2_END_COORD_Y
            },
        }
    },
    .chromakey = {
        {
            .enable = LCD_CH0_CHROMAKEY_GR1_ENABLE,
            .before = LCD_CH0_CHROMAKEY_GR1_BEFORE_ARGB,
            .after = LCD_CH0_CHROMAKEY_GR1_AFTER_ARGB
        },
        {
            .enable = LCD_CH0_CHROMAKEY_GR2_ENABLE,
            .before = LCD_CH0_CHROMAKEY_GR2_BEFORE_ARGB,
            .after = LCD_CH0_CHROMAKEY_GR2_AFTER_ARGB
        }
    },
    .clut = {
        {
            .enable = LCD_CH0_CLUT_GR1_ENABLE,
            .p_base = NULL,
            .start = LCD_CH0_CLUT_GR1_START,
            .size = LCD_CH0_CLUT_GR1_SIZE
        },
        {
            .enable = LCD_CH0_CLUT_GR2_ENABLE,
            .p_base = NULL,
            .start = LCD_CH0_CLUT_GR2_START,
            .size = LCD_CH0_CLUT_GR2_SIZE
        }
    },
    .detection = {
        .vpos_detect = LCD_CH0_DETECT_VPOS,
        .gr1uf_detect = LCD_CH0_DETECT_GR1UF,
        .gr2uf_detect = LCD_CH0_DETECT_GR2UF
    },
    .interrupt = {
        .vpos_enable = LCD_CH0_INTERRUPT_VPOS_ENABLE,
        .gr1uf_enable = LCD_CH0_INTERRUPT_GR1UF_ENABLE,
        .gr2uf_enable = LCD_CH0_INTERRUPT_GR2UF_ENABLE
    },
    .p_callback = LCD_CH0_PCALLBACK
};
//@formatter:on


/**
 * @brief バックグラウンドカラー
 */
static glcdc_color_t s_bg_color;


/**
 * @brief テスト信号を初期化する
 */
void test_signal_init(void)
{
    s_is_lcd_event_processing = false;
    s_bg_color = s_lcd_config.output.bg_color; // memcpy()に相当。

    if (R_GLCDC_Open(&s_lcd_config) == GLCDC_SUCCESS) {
        R_GLCDC_PinSet();
    }

    PORT1.PDR.BIT.B4 = 1;
    PORT1.PODR.BIT.B4 = 0;
    PORT1.PMR.BIT.B4 = 0;

    R_GLCDC_Control(GLCDC_CMD_STOP_DISPLAY, NULL);

    return ;
}

/**
 * @brief テスト信号を出力するかどうかを設定する。
 * @param is_output 出力する場合にはtrue, 出力しない場合にはfalse
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool test_signal_set_output(bool is_output)
{
    bool is_succeed;
    if (is_output)
    {
        is_succeed = (R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, NULL) == GLCDC_SUCCESS);
        if (is_succeed)
        {
            PORT1.PMR.BIT.B4 = 1; // P14 for Peripheral
        }
    }
    else
    {
        is_succeed = (R_GLCDC_Control(GLCDC_CMD_STOP_DISPLAY, NULL) == GLCDC_SUCCESS);
        if (is_succeed)
        {
            PORT1.PMR.BIT.B4 = 0;
        }
    }
    return is_succeed;
}

/**
 * @brief テスト信号出力中かどうかを判定する。
 * @return テスト信号出力中の場合にはtrue, 出力していない場合にはfalse.
 */
bool test_signal_is_output(void)
{
    return GLCDC.BGEN.BIT.VEN != 0;
}

/**
 * @brief テストデータを設定する。
 * @param data テストデータ
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool test_signal_set_data(uint8_t data)
{
    s_bg_color.byte.b = data;
    s_bg_color.byte.g = data;
    s_bg_color.byte.r = data;
    s_bg_color.byte.a = 0xFF;
    return R_GLCDC_Control(GLCDC_CMD_CHANGE_BG_COLOR, &s_bg_color) == GLCDC_SUCCESS;
}

/**
 * @brief テストデータを得る。
 * @return テストデータ
 */
uint8_t test_signal_get_data(void)
{
    return s_bg_color.byte.b;
}


/**
 * @brief GLCDC のイベントコールバック
 * @param arg パラメータ
 */
static void glcdc_callback(void *arg)
{
    if (!s_is_lcd_event_processing) // 初回のイベント？
    {
        // 2回目以降のイベントのみ処理対象。
        s_is_lcd_event_processing = true;
        return ;
    }

    glcdc_callback_args_t *p = (glcdc_callback_args_t*)(arg);
    switch (p->event) {
        case GLCDC_EVENT_LINE_DETECTION:
        {
            // TODO : VPOS Event
            break;
        }
        default:
        {
            break;
        }
    }

    return ;
}
