/**
 * @file PDC操作インタフェース宣言
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef PDC_H_
#define PDC_H_

#include <stdbool.h>
#include <stdint.h>

struct pdc_status
{
    bool is_receiving;      // 受信動作中かどうか
    bool is_resetting;      // リセット中かどうか
    bool is_data_receiving; // キャプチャ動作中かどうか(VSyncの有効エッジ検出でONになり、
                            // 1フレーム分のデータ取得 or 受信停止でOFFになる。(つまり、ほとんどOFF)
    bool is_fifo_empty;     // FIFOが空かどうか(転送完了時はTRUEになるはず)
    bool is_frame_end;      // フレームエンド検知
    bool has_overrun;       // オーバーランエラー有無(データの読み出しが間に合わなかった)
    bool has_underrun;      // アンダーランエラー有無(データがない状態でFIFOが読まれた（ソフトバグ）)
    bool has_vline_err;     // 垂直ラインエラー有無(指定したサイズをキャプチャし終わる前にフレームが終了)
    bool has_hsize_err;     // 水平ラインエラー有無(指定したサイズをキャプチャし終わる前にラインが終了)

    uint32_t received_len; // 受信済みサイズ
    uint32_t total_len;    // 総転送サイズ
};

void pdc_init(void);
void pdc_update(void);
bool pdc_is_running(void);
bool pdc_reset(uint16_t timeout_millis);
bool pdc_set_signal_polarity(bool is_hsync_hactive, bool is_vsync_hactive);
bool pdc_get_signal_polarity(bool* is_hsync_hactive, bool* is_vsync_hactive);
bool pdc_set_capture_range(uint16_t xst, uint16_t xsize, uint16_t yst, uint16_t ysize, uint8_t bpp);
bool pdc_get_capture_range(uint16_t* xst, uint16_t* xsize, uint16_t* yst, uint16_t* ysize, uint8_t* bpp);
bool pdc_start_capture(void (*callback)(const struct pdc_status* pstat));
bool pdc_stop_capture(void);

bool pdc_get_status(struct pdc_status* pstat);

#endif /* PDC_H_ */
