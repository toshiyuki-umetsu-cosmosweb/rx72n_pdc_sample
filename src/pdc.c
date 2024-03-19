/**
 * @file PDC操作インタフェース定義
 * @author Cosmosweb Co.,Ltd. 2024
 * @note PDCとDMACを使用してパラレルデータキャプチャを行うモジュール。
 */
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <platform.h>
#include <r_smc_entry.h>

#include "hwtick.h"
#include "rx_driver_pdc.h"
#include "pdc.h"

#define RAM1_START_ADDR (0x00000000UL)
#define RAM1_END_ADDR (0x0007FFFFUL)
#define RAM1_SIZE (RAM1_END_ADDR + 1UL - RAM1_START_ADDR)
#define RAM2_START_ADDR (0x00800000UL)
#define RAM2_END_ADDR (0x0087FFFFUL)
#define RAM2_SIZE (RAM2_END_ADDR + 1UL - RAM2_START_ADDR)

#define DMA_AREA_COUNT (2)

#define RAM_USEAREA1_SIZE (RAM1_SIZE / 2UL)

#define RAM_USEAREA2_SIZE (RAM2_SIZE)

/**
 * @brief PDC割り込みプライオリティ
 */
#define PDC_INTERRUPT_PRIORITY (2)

/**
 * @brief RXマイコンPDC転送要求が発行されるバイト数
 */
#define RX_PDC_TRANSFER_REQ_UNIT (32)
/**
 * @brief RXマイコン PDC PDCRレジスタのデータ幅(32bit = 4byte)
 */
#define RX_PDC_TRANSFER_DATA_SIZE (4)

struct dma_param {
    uintptr_t addr;
    uint8_t unit; // PCDRが32bit幅で4バイト単位なので、転送単位は4のみ。
    uint16_t block_size; // PDCでは32バイトごとにDMA転送要求が発行されるので、32/4=8固定。
    uint16_t block_count; // 必要な領域に合わせて変更

};

static uint32_t calc_dma_area_total_size(const struct dma_param *paramp);
static uint32_t calc_received_length(void);
static bool set_transfer_irqs_enable(bool is_enabled);
static bool update_transfer_size(uint32_t hsize, uint32_t vsize, uint32_t bpw);
static bool setup_dmac_request(int area);
static void on_dma_request_end(int status);
static void on_frame_end(const pdc_event_arg_t *arg);
static void on_error(const pdc_event_arg_t *arg);
static int convert_pdc_event_to_error(int event, uint32_t errors);

/**
 * @brief PDC設定
 */
static pdc_config_t s_pdc_config;
/**
 * @brief 1ピクセルあたりのバイト数
 */
static uint8_t s_bpp;

/**
 * @brief DMA転送情報
 */
//@formatter:off
static struct dma_param s_dma_param[DMA_AREA_COUNT] = {
    { // RAM1割り当て
        .addr = (uintptr_t)(RAM1_END_ADDR + 1UL - RAM_USEAREA1_SIZE),
        .unit = RX_PDC_TRANSFER_DATA_SIZE,
        .block_size = (RX_PDC_TRANSFER_REQ_UNIT / RX_PDC_TRANSFER_DATA_SIZE),
        .block_count = 1,
    },
    { // RAM2割り当て
        .addr = (uintptr_t)(RAM2_END_ADDR + 1UL - RAM_USEAREA2_SIZE),
        .unit = RX_PDC_TRANSFER_DATA_SIZE,
        .block_size = (RX_PDC_TRANSFER_REQ_UNIT / RX_PDC_TRANSFER_DATA_SIZE),
        .block_count = 1,
    }
};
//@formatter:on
/**
 * @brief DMA転送中のエリア番号
 */
static int s_dma_area;

/**
 * @brief 転送データのトータルサイズ[byte]
 */
static uint32_t s_data_size;

/**
 * @brief フレームキャプチャ完了時コールバック
 */
static void (*s_end_callback)(const struct pdc_status *pstat);

/**
 * @brief PDC初期化処理を行う。
 */
void pdc_init(void)
{
    for (int i = 0; i < DMA_AREA_COUNT; i++)
    {
        uint32_t len = calc_dma_area_total_size(&s_dma_param[i]);
        memset((void*)(s_dma_param[0].addr), 0, len);
    }

    s_bpp = 2; // YUV 4:2:2

    memset(&s_pdc_config, 0, sizeof(s_pdc_config));
    s_pdc_config.int_priority_pcdfi = PDC_INTERRUPT_PRIORITY;
    s_pdc_config.int_priority_pcefi = PDC_INTERRUPT_PRIORITY;
    s_pdc_config.int_priority_pceri = PDC_INTERRUPT_PRIORITY;
    s_pdc_config.interrupt_setting.dfie_ien = false; // 受信データ割り込みしない（DMAを使用する）
    s_pdc_config.interrupt_setting.feie_ien = false; // フレームエンド割り込みしない（キャプチャ中だけONにする）
    s_pdc_config.interrupt_setting.ovie_ien = false; // オーバーランエラー割り込みしない（キャプチャ中だけONにする）
    s_pdc_config.interrupt_setting.udrie_ien = false; // アンダーランエラー割り込みしない（キャプチャ中だけONにする）
    s_pdc_config.interrupt_setting.verie_ien = false; // 垂直方向ライン数設定エラー割り込みしない（キャプチャ中だけONにする）
    s_pdc_config.interrupt_setting.herie_ien = false; // 水平方向バイト数設定エラー割り込みしない（キャプチャ中だけONにする）

    s_pdc_config.is_vsync_hactive = false; // Vsync Low Active
    s_pdc_config.is_hsync_hactive = true; // Hsync High Active
    s_pdc_config.capture_pos.vst_position = 10; // 垂直方向ライン開始 = 垂直方向バックポーチ。
                                                // VSync検出後、バックポーチ分のライン数後にアクティブデータが入る。
    s_pdc_config.capture_size.vsz_size = 480; // 480ライン
    s_pdc_config.capture_pos.hst_position = 612; // 水平方向開始位置 = 水平方向のバックポーチ
    s_pdc_config.capture_size.hsz_size = 640 * 2; // 640 * 2 = 1280バイト (YUVUなので2倍)

    s_pdc_config.p_callback.pcb_receive_data_ready = NULL;
    s_pdc_config.p_callback.pcb_frame_end = on_frame_end;
    s_pdc_config.p_callback.pcb_error = on_error;

    rx_driver_pdc_open(&s_pdc_config);

    // DMAC設定
    // DAMC3の初期化は CG ドライバがHardwareSetup内で呼ばれて実行されるので、
    // ここで何かをする必要はない。
    // もし、FITドライバを使うなら、ここで設定をする。

    s_dma_area = 0;
    update_transfer_size(640, 480, 2); // 640 * 480 * YUV 4:2:2(2byte/pixel)

    return ;
}

/**
 * @brief PDCを更新する
 */
void pdc_update(void)
{
    rx_driver_pdc_update();

    return ;
}


/**
 * @brief PDCがキャプチャ動作中かどうかを取得する。
 * @return キャプチャ動作中の場合にはtrue, キャプチャ動作中でない場合にはfalse
 */
bool pdc_is_running(void)
{
    return rx_driver_pdc_is_receiving();
}

/**
 * @brief PDCをリセットする
 * @param timeout_millis タイムアウト時間[ミリ秒]
 * @return リセット完了した場合にはtrue, 失敗した場合にはfalse.
 */
bool pdc_reset(uint16_t timeout_millis)
{
    if (rx_driver_pdc_reset() != 0)
    {
        return false;
    }
    uint32_t begin = hwtick_get();
    while (rx_driver_pdc_is_resetting()
            && ((hwtick_get() - begin) < timeout_millis))
    {
        // do nothing.
    }

    return !rx_driver_pdc_is_resetting();
}

/**
 * @brief 同期信号の極性設定を設定する。
 * @param is_hsync_hactive 水平同期信号極性の設定(true:H-Active, false:L-Active)
 * @param is_vsync_hactive 垂直同期信号極性の設定(true:H-Active, false:L-Active)
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
bool pdc_set_signal_polarity(bool is_hsync_hactive, bool is_vsync_hactive)
{
    return rx_driver_pdc_set_signal_polarity(is_hsync_hactive, is_vsync_hactive) == 0;
}
/**
 * @brief 同期信号の極性設定を取得する。
 * @param is_hsync_hactive 水平同期信号極性の設定を取得する変数
 * @param is_vsync_hactive 垂直同期信号極性の設定を取得する変数
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
bool pdc_get_signal_polarity(bool *is_hsync_hactive, bool *is_vsync_hactive)
{
    return rx_driver_pdc_get_signal_polarity(is_hsync_hactive, is_vsync_hactive) == 0;
}

/**
 * @brief キャプチャ範囲を設定する
 * @param xst X開始位置(HSyncを抜ける位置を0としたオフセット)
 * @param xsize Xキャプチャサイズ
 * @param yst Y開始位置(VSyncを抜ける位置を0としたオフセット)
 * @param ysize Yキャプチャサイズ
 * @param bpp 1ピクセルあたりのバイト数
 * @return 成功した場合には0, 失敗した場合にはエラー番号。
 */
bool pdc_set_capture_range(uint16_t xst, uint16_t xsize, uint16_t yst, uint16_t ysize, uint8_t bpp)
{
    pdc_position_t pos;
    pdc_capture_size_t size;

    if ((bpp != 1) && (bpp != 2) && (bpp != 3))
    {
        return false;
    }

    // 32の正数倍かどうかを調べる。
    uint32_t total = xsize * bpp * ysize;
    if ((total % RX_PDC_TRANSFER_REQ_UNIT) != 0)
    {
        return false;
    }
    pos.hst_position = xst * bpp;
    pos.vst_position = yst;
    size.hsz_size = xsize * bpp;
    size.vsz_size = ysize;

    int retval = rx_driver_pdc_set_position_size(&pos, &size);
    if (retval != 0)
    {
        return false;
    }

    update_transfer_size(xsize, ysize, bpp);
    s_bpp = bpp;

    return true;
}

/**
 * @brief キャプチャ範囲を取得する
 * @param xst X開始位置
 * @param xsize Xキャプチャサイズ
 * @param yst Y開始位置
 * @param ysize Yキャプチャサイズ
 * @param bpp 1ピクセルあたりのバイト数
 * @return 成功した場合には0, 失敗した場合にはエラー番号。
 */
bool pdc_get_capture_range(uint16_t *xst, uint16_t *xsize, uint16_t *yst, uint16_t *ysize, uint8_t *bpp)
{
    pdc_position_t pos;
    pdc_capture_size_t size;
    if (rx_driver_pdc_get_position_size(&pos, &size) != 0)
    {
        return false;
    }

    if (xst != NULL)
    {
        (*xst) = pos.hst_position / s_bpp;
    }
    if (yst != NULL)
    {
        (*yst) = pos.vst_position;
    }
    if (xsize != NULL)
    {
        (*xsize) = size.hsz_size / s_bpp;
    }
    if (ysize != NULL)
    {
        (*ysize) = size.vsz_size;
    }
    if (bpp != NULL)
    {
        *bpp = s_bpp;
    }

    return true;
}


/**
 * @brief キャプチャを開始する。
 * @param callback キャプチャ完了時に通知を受け取るコールバック関数
 * @return 成功した場合にはtrue, 失敗した場合にはfalseを返す。
 */
bool pdc_start_capture(void (*callback)(const struct pdc_status *pstat))
{
    if (pdc_is_running()) {
        return false;
    }

    s_dma_area = 0;
    if (!setup_dmac_request(s_dma_area))
    {
        return false;
    }

    if (!set_transfer_irqs_enable(true))
    {
        return false;
    }

    R_Config_DMAC3_Start();

    bool is_succeed = rx_driver_pdc_capture_start() == 0;
    if (is_succeed)
    {
        s_end_callback = callback;
    }
    else
    {
        R_Config_DMAC3_Stop();
    }
    return is_succeed;
}

/**
 * @brief キャプチャを停止する
 * @return 成功した場合にはtrue, 失敗した場合にはfalseを返す。
 */
bool pdc_stop_capture(void)
{
    bool is_succeed = true;

    if (rx_driver_pdc_is_resetting())
    {
        R_Config_DMAC3_Stop(); // DMA転送停止
        if (rx_driver_pdc_set_receive_enable(false) != 0)
        {
            is_succeed = false;
        }
        if (!set_transfer_irqs_enable(false)) // 割り込み通知停止
        {
            is_succeed = false;
        }
    }

    return is_succeed;
}

/**
 * @brief PDCのステータスを得る
 * @param pstat ステータスを取得する構造体
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool pdc_get_status(struct pdc_status *pstat)
{
    if (pstat == NULL)
    {
        return false;
    }

    (*pstat).is_receiving = PDC.PCCR1.BIT.PCE != 0;
    (*pstat).is_resetting = PDC.PCCR0.BIT.PRST != 0;
    (*pstat).is_data_receiving = PDC.PCSR.BIT.FBSY != 0;
    (*pstat).is_fifo_empty = PDC.PCSR.BIT.FEMPF != 0;
    (*pstat).is_frame_end = PDC.PCSR.BIT.FEF != 0;
    (*pstat).has_overrun = PDC.PCSR.BIT.OVRF != 0;
    (*pstat).has_underrun = PDC.PCSR.BIT.UDRF != 0;
    (*pstat).has_vline_err = PDC.PCSR.BIT.VERF != 0;
    (*pstat).has_hsize_err = PDC.PCSR.BIT.HERF != 0;

    (*pstat).received_len = calc_received_length();
    (*pstat).total_len = s_data_size;
    return true;
}

/**
 * @brief parampで指定したDMAリクエストの総転送サイズを取得する。
 * @param paramp DMAリクエストパラメータ
 * @return 総転送サイズ
 */
static uint32_t calc_dma_area_total_size(const struct dma_param *paramp)
{
    return ((uint32_t)(paramp->unit) * (uint32_t)(paramp->block_size) * (uint32_t)(paramp->block_count));
}

/**
 * @brief 受信済みバイト数を得る。
 * @return 受信済みバイト数
 */
static uint32_t calc_received_length(void)
{
    uint32_t received_len = 0;
    int dma_area = s_dma_area;

    for (int i = 0; i < dma_area; i++)
    {
        const struct dma_param *paramp = &(s_dma_param[i]);
        received_len += calc_dma_area_total_size(paramp);
    }
    if (s_dma_area < DMA_AREA_COUNT)
    {
        uint32_t area_total = calc_dma_area_total_size(&(s_dma_param[dma_area]));
        uint32_t left_size = R_Config_DMAC3_Get_LeftSize();

        if (left_size <= area_total)
        {
            received_len += (area_total - left_size);
        }
    }

    return received_len;
}


/**
 * @brief 転送関連のIRQをまとめて有効/無効設定する
 * @param is_enabled 割り込みを有効にする場合にはtrue, それ以外はfalse.
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
static bool set_transfer_irqs_enable(bool is_enabled)
{
#if 1
    // お試しで割り込みしてみる。
    s_pdc_config.interrupt_setting.dfie_ien = is_enabled; // データレディ割り込み許可
#endif
    s_pdc_config.interrupt_setting.feie_ien = is_enabled; // フレームエンド割り込み
    s_pdc_config.interrupt_setting.ovie_ien = is_enabled; // オーバーフロー割り込み
    s_pdc_config.interrupt_setting.udrie_ien = is_enabled; // アンダーフロー割り込み
    s_pdc_config.interrupt_setting.verie_ien = is_enabled; // 垂直ライン数設定エラー割り込み
    s_pdc_config.interrupt_setting.herie_ien = is_enabled; // 水平ライン数設定エラー割り込み

    return rx_driver_pdc_set_interrupt_setting(&s_pdc_config.interrupt_setting) == 0;
}

/**
 * @brief 転送サイズを更新する
 * @param hsize キャプチャする水平方向画素数
 * @param vsize キャプチャする垂直方向ライン数
 * @param bpw 1画素あたりのバイト数
 * @return 設定できた場合には0, 設定できなかった場合にはfalse.
 */
static bool update_transfer_size(uint32_t hsize, uint32_t vsize, uint32_t bpw)
{
    uint32_t total = hsize * vsize * bpw;

    if ((total < 1) || (total > (RAM_USEAREA1_SIZE + RAM_USEAREA2_SIZE)))
    {
        return false;
    }


    if (total <= RAM_USEAREA1_SIZE) // サイズはRAM1領域だけで十分？
    {
        s_dma_param[0].block_count = total / RX_PDC_TRANSFER_REQ_UNIT;
        s_dma_param[1].block_count = 0;
    }
    else
    {
        s_dma_param[0].block_count = RAM_USEAREA1_SIZE / RX_PDC_TRANSFER_REQ_UNIT;
        s_dma_param[1].block_count = (total - RAM_USEAREA1_SIZE) / RX_PDC_TRANSFER_REQ_UNIT;
    }

    s_dma_area = 0;
    s_data_size = total;

    if (!setup_dmac_request(s_dma_area))
    {
        return false;
    }

    return true;
}

/**
 * @brief DMAリクエストをセットアップする。
 * @param area DMAエリア番号
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
static bool setup_dmac_request(int area)
{
    if (area >= DMA_AREA_COUNT)
    {
        return false;
    }

    struct dma_param *paramp = &(s_dma_param[area]);
    if (paramp->block_count == 0)
    {
        return false;
    }

    R_Config_DMAC3_Setup(paramp->addr, paramp->unit, paramp->block_size, paramp->block_count, on_dma_request_end);
    R_Config_DMAC3_Start();

    return true;
}


/**
 * @brief DMA要求が完了したときに通知を受け取る
 * @param status ステータス
 */
static void on_dma_request_end(int status)
{
    if (s_dma_area < DMA_AREA_COUNT)
    {
        s_dma_area++;
        setup_dmac_request(s_dma_area);
    }

    return ;
}

/**
 * @brief PDCのフレーム終了を検知したときに通知を受け取る。
 * @param arg PDCイベントデータ
 */
static void on_frame_end(const pdc_event_arg_t * arg)
{
    // 残りデータがあったら追加する(たぶん必要だと思う)
    if (PDC.PCSR.BIT.FEMPF != 0) // FIFOはエンプティでない？
    {
        while (PDC.PCSR.BIT.FEMPF == 0)
        {
            volatile uint32_t word = PDC.PCDR.LONG;

            // TODO : バッファに追加(リニアじゃないと面倒な処理がある？？)

        }
    }

    rx_driver_pdc_set_receive_enable(false); // 受信停止
    R_Config_DMAC3_Stop();
    set_transfer_irqs_enable(false);
    if (s_end_callback != NULL)
    {
        struct pdc_status status;
        pdc_get_status(&status);
        status.is_frame_end = true;
        if (!R_Config_DMAC3_IsTransferring()) // 転送してない？
        {
            status.received_len = status.total_len; // 全部転送した判定する
        }
        s_end_callback(&status);
    }

    return ;
}
/**
 * @brief PDCのエラーを検知したときに通知を受け取る。
 * @param arg PDCイベントデータ
 */
static void on_error(const pdc_event_arg_t *arg)
{
    R_Config_DMAC3_Stop();
    set_transfer_irqs_enable(false);
    if (s_end_callback != NULL)
    {
        struct pdc_status state;
        pdc_get_status(&state);
        if (arg->errors & PDC_ERROR_OVERRUN)
        {
            state.has_overrun = true;
        }
        if (arg->errors & PDC_ERROR_UNDERRUN)
        {
            state.has_underrun = true;
        }
        if (arg->errors & PDC_ERROR_HPARAM)
        {
            state.has_hsize_err = true;
        }
        if (arg->errors & PDC_ERROR_VPARAM)
        {
            state.has_vline_err = true;
        }

        s_end_callback(&state);
    }

    return ;
}

/**
 * @brief PDCのイベントコードをエラー番号に変換する。
 * @param event イベントコード
 * @return エラー番号
 */
static int convert_pdc_event_to_error(int event, uint32_t errors)
{
    int retval;
    switch (event)
    {
        case PDC_EVT_ID_ERROR:
        {
            if ((errors & PDC_ERROR_OVERRUN) != 0)
            {
                retval = EOVERFLOW;
            }
            else if ((errors & PDC_ERROR_UNDERRUN) != 0)
            {
                retval = ENODATA;
            }
            else if ((errors & (PDC_ERROR_HPARAM | PDC_ERROR_VPARAM)) != 0)
            {
                retval = EINVAL;
            }
            else
            {
                retval = EIO;
            }
            break;
        }
        case PDC_EVT_ID_TRANSFER_TIMEOUT:
        {
            retval = ETIMEDOUT;
            break;
        }
        case PDC_EVT_ID_DATAREADY:
        case PDC_EVT_ID_FRAMEEND:
        default:
        {
            retval = 0;
            break;
        }
    }

    return retval;
}
