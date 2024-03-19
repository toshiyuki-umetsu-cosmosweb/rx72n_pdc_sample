/**
 * @file Renesas RX PDCペリフェラル用ドライバコード
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef RX_DRIVER_PDC_H_
#define RX_DRIVER_PDC_H_

#include <stdint.h>
#include <stdlib.h>

#define PDC_EVT_ID_DATAREADY (0)        // DataReady イベント(DMA/DTC転送するので基本的に通知受け取らない)
#define PDC_EVT_ID_FRAMEEND (1)         // フレーム終了通知
#define PDC_EVT_ID_ERROR (2)            // エラー検知
#define PDC_EVT_ID_TRANSFER_TIMEOUT (3) // フレーム終了通知検知後、所定の時間が経過しても転送完了しなかった

#define PDC_ERROR_OVERRUN (1 << 0)  // オーバーラン
#define PDC_ERROR_UNDERRUN (1 << 1) // アンダーラン
#define PDC_ERROR_VPARAM (1 << 2)   // 垂直方向パラメータエラー
#define PDC_ERROR_HPARAM (1 << 3)   // 水平方向パラメータエラー

/**
 * イベントコールバック用データ
 */
typedef struct
{
    int event_id;    // イベントコード(PDC_EVT_ID 参照)
    uint32_t errors; // ビットマッピングされたエラーフラグ(event_id がPDC_EVT_ID_ERROR時のみ有効。PDC_ERROR_x 参照)
} pdc_event_arg_t;

/**
 * @brief コールバック関数
 * @note 基本的に割り込み処理中に呼び出されるので、コールバック関数は処理時間の設計に注意が必要です。
 */
typedef struct
{
    void (*pcb_receive_data_ready)(const pdc_event_arg_t*); // データレディ通知(DMAC側の設定により、CPU割り込みが行われない場合がある)
    void (*pcb_frame_end)(const pdc_event_arg_t*);          // フレームエンド通知
    void (*pcb_error)(const pdc_event_arg_t*);              // エラー通知
} pdc_callback_functions_t;

/**
 * @brief PDC割り込み通知設定
 */
typedef struct
{
    bool dfie_ien;  // Data Ready 通知 (DMAC側の設定により、CPU割り込みが行われない場合がある)
    bool feie_ien;  // フレーム終了通知
    bool ovie_ien;  // オーバーラン通知(FIFOがフルの状態で受信した)
    bool udrie_ien; // アンダーラン通知(データがないのにFIFOが読み出された)
    bool verie_ien; // 垂直サイズ設定エラー通知(キャプチャが完了しないうちに次のVSyncを検出した)
    bool herie_ien; // 水平サイズ設定エラー通知(キャプチャが完了しないうちに次のHSyncを検出した)
} pdc_interrupt_setting_t;

/**
 * @brief PDCキャプチャサイズ
 */
typedef struct
{
    uint16_t hstart; // 水平開始位置
    uint16_t hsize;  // 水平キャプチャサイズ
    uint16_t vstart; // 垂直開始位置
    uint16_t vsize;  // 垂直キャプチャサイズ
} pdc_capture_range_t;

typedef struct
{
    uint8_t int_priority_pcdfi;                // PCDFI 割り込みプライオリティ
    uint8_t int_priority_pcefi;                // PCEFI 割り込みプライオリティ
    uint8_t int_priority_pceri;                // PCERI 割り込みプライオリティ
    pdc_interrupt_setting_t interrupt_setting; /* PDC interrupt setting */
    bool is_hsync_hactive;                     // HSYNC 極性  true:H-Active, false:L-Active
    bool is_vsync_hactive;                     // Vsync 極性  true:H-Active, false:L-Active
    pdc_capture_range_t capture_size;          // キャプチャサイズ設定
    pdc_callback_functions_t p_callback;       // コールバック関数
} pdc_config_t;

/**
 * @brief PDCステータス
 */
typedef struct st_pdc_pcsr_stat
{
    bool is_frame_busy; // PDC 動作状態。true:受信動作中, false:受信停止中
    bool fifo_empty;    // FIFO 状態。true:データなし, false:データあり
    bool frame_end;     // フレームエンドフラグ。 true:フレーム終了, false:フレーム終了未検知
    bool overrun;       // オーバーランフラグ。 true:オーバーラン検知, false:未検知
    bool underrun;      // アンダーランフラグ。 true:アンダーラン検知, false:未検知
    bool verf_error;    // 垂直ラインエラーフラグ。 true:垂直ラインエラー検知, false:未検知
    bool herf_error;    // 水平ラインエラーフラグ。 true:垂直ラインエラー検知, false:未検知
} pdc_stat_t;

/**
 * @brief PDCモニタステータス
 * @note HSYNC, VSYNCの極性がわかるからどうなの？とは思いますが、
 *       結線が正しいかを確認する時は有効な手段だったりします。
 */
typedef struct
{
    bool vsync; // VSync入力 信号レベル。 (true:High, false:Low)
    bool hsync; // HSync入力 信号レベル。 (true:High, false:Low)
} pdc_monitor_stat_t;

int rx_driver_pdc_open(pdc_config_t* p_data_cfg);
int rx_driver_pdc_close(void);

void rx_driver_pdc_update(void);

#define PDC_INTERRUPT_PCDFI (0) // PCDFI (受信データレディ)
#define PDC_INTERRUPT_PCFEI (1) // PCFEI (フレームエンド割り込み)
#define PDC_INTERRUPT_PCERI (2) // PCERI (エラー割り込み)

int rx_driver_pdc_set_irq_priority(int type, uint8_t priority);
uint8_t rx_driver_pdc_get_irq_priority(int type);
int rx_driver_pdc_set_interrupt_setting(const pdc_interrupt_setting_t* psetting);
int rx_driver_pdc_get_interrupt_setting(pdc_interrupt_setting_t* psetting);

int rx_driver_pdc_set_receive_enable(bool is_enabled);
bool rx_driver_pdc_is_receiving(void);
int rx_driver_pdc_reset(void);
bool rx_driver_pdc_is_resetting(void);

int rx_driver_pdc_set_signal_polarity(bool is_hsync_hactive, bool is_vsync_hactive);
int rx_driver_pdc_get_signal_polarity(bool* is_hsync_hactive, bool* is_vsync_hactive);
int rx_driver_pdc_set_range(const pdc_capture_range_t* prange);
int rx_driver_pdc_get_range(pdc_capture_range_t* prange);
int rx_driver_pdc_capture_start(void);
int rx_driver_pdc_clear_status(pdc_stat_t* pstat);
int rx_driver_pdc_get_status(pdc_stat_t* pstat);

int rx_driver_pdc_get_monitor_stat(pdc_monitor_stat_t* pstat);

#endif /* RX_DRIVER_PDC_H_ */
