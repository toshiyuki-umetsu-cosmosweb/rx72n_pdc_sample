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

/* Pointer to callback functions of PDC */
typedef struct
{
    void (*pcb_receive_data_ready)(const pdc_event_arg_t*); /* Pointer to callback function for receive data-ready interrupt */
    void (*pcb_frame_end)(const pdc_event_arg_t*);          /* Pointer to callback function for FIFO empty after frame-end interrupt */
    void (*pcb_error)(const pdc_event_arg_t*);              /* Pointer to callback function for occurred each error */
} pdc_callback_functions_t;

typedef struct
{
    bool dfie_ien;  /* Receive data-ready interrupt enabled */
    bool feie_ien;  /* Frame-end interrupt enabled */
    bool ovie_ien;  /* Overrun interrupt enabled */
    bool udrie_ien; /* Underrun interrupt enabled */
    bool verie_ien; /* Vertical line count setting error interrupt enabled */
    bool herie_ien; /* Horizontal byte count setting error interrupt enabled */
} pdc_interrupt_setting_t;

typedef struct
{
    uint16_t vst_position; /* Vertical capture start line position */
    uint16_t hst_position; /* Horizontal capture start byte position */
} pdc_position_t;

typedef struct
{
    uint16_t vsz_size; /* Vertical capture size */
    uint16_t hsz_size; /* Horizontal capture size */
} pdc_capture_size_t;

typedef struct
{
    uint8_t int_priority_pcdfi;                // PCDFI 割り込みプライオリティ
    uint8_t int_priority_pcefi;                // PCEFI 割り込みプライオリティ
    uint8_t int_priority_pceri;                // PCERI 割り込みプライオリティ
    pdc_interrupt_setting_t interrupt_setting; /* PDC interrupt setting */
    bool is_hsync_hactive;                     // HSYNC 極性  true:H-Active, false:L-Active
    bool is_vsync_hactive;                     // Vsync 極性  true:H-Active, false:L-Active
    pdc_position_t capture_pos;                // キャプチャ位置設定
    pdc_capture_size_t capture_size;           // キャプチャサイズ設定
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

/* Copy of PDC pin monitor status register (PCMONR) */
typedef struct st_pdc_pcmonr_stat
{
    bool vsync; /* VSYNC signal status (VSYNC flag) */
    bool hsync; /* HSYNC signal status (HSYNC flag) */
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
int rx_driver_pdc_set_position_size(const pdc_position_t* ppos, const pdc_capture_size_t* psize);
int rx_driver_pdc_get_position_size(pdc_position_t* ppos, pdc_capture_size_t* psize);
int rx_driver_pdc_capture_start(void);
int rx_driver_pdc_clear_status(pdc_stat_t* pstat);
int rx_driver_pdc_get_status(pdc_stat_t* pstat);

int rx_driver_pdc_get_monitor_stat(pdc_monitor_stat_t* pstat);

#endif /* RX_DRIVER_PDC_H_ */
