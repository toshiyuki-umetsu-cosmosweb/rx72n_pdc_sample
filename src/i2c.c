/**
 * @file I2Cインタフェース定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <platform.h>
#include <r_sci_rx_if.h>
#include <r_sci_iic_rx_if.h>

#include "hwtick.h"
#include "i2c.h"

/**
 * r_sci_iic_rx_if.h ではsci_iic_mcu_status_t のフラグが定義されていない。
 * BITがunion定義されてるように見えるが、
 * なんと関連ヘッダファイルをインクルードされていないと定義されない。
 */
#define SCI_IIC_STATUS_BUSY (1 << 0)
#define SCI_IIC_STATUS_MODE (1 << 1)
#define SCI_IIC_STATUS_NACK (1 << 2)

/**
 * @brief ボーレート計算時、CKS設定値に依存した係数
 *        RX72Nハードウェアマニュアルより。
 *        I2C時の計算式係数になる。
 */
//@formatter:off
static const uint32_t s_cks_coefs[4] = {
    32,  // 64*2^(2*0-1) = 32
    128, // 64*2^(2*1-1) = 128
    512, // 64*2^(2*2-1) = 512
    2048 // 64*2^(2*3-1) = 2048
};
//@formatter:on

static int set_bitrate(volatile struct st_sci0* reg, uint32_t bit_rate);
static float calc_brr_value(uint32_t bit_rate, uint8_t cks);
static uint32_t get_bit_rate(const volatile struct st_sci0* reg);
static float calc_bit_rate(uint8_t cks, uint8_t brr);
static int convert_status_to_errno(sci_iic_ch_dev_status_t status);
static int convert_iic_return_to_errno(sci_iic_return_t ret_code);
static int wait_transaction_done(uint32_t timeout_millis);
static void on_transaction_done(void);

/**
 * @brief SCI IIC制御データ
 */
static sci_iic_info_t s_sci_iic_info;
/**
 * @brief Slaveアドレス
 */
static uint8_t s_slave_addr[1];

static i2c_callback_func_t s_callback;

/**
 * @brief I2Cを初期化する
 */
void i2c_init(void)
{
    s_callback = NULL;

    memset(&s_sci_iic_info, 0, sizeof(s_sci_iic_info));
    s_sci_iic_info.dev_sts = SCI_IIC_NO_INIT;
    s_sci_iic_info.ch_no = SCI_CH6;

    if (R_SCI_IIC_Open(&s_sci_iic_info) == SCI_IIC_SUCCESS)
    {
        set_bitrate(&SCI6, 10000);
    }
    else
    {
        // Error.
    }

    return;
}

/**
 * @brief I2Cのビットレートを設定する。
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int i2c_set_bitrate(uint32_t bit_rate)
{
    int retval;
    bool is_succeed = false;
    sci_iic_mcu_status_t st;

    if (i2c_is_busy())
    {
        retval = EBUSY;
    }
    else
    {
        retval = set_bitrate(&SCI6, bit_rate);
    }
    return retval;
}

/**
 * @brief ビットレートを設定する。
 * @param reg レジスタセット
 * @param bit_rate ビットレート[bps] (bit_rate > 0)
 * @return 成功した場合には0, 失敗した場合にはエラー番号。
 */
static int set_bitrate(volatile struct st_sci0* reg, uint32_t bit_rate)
{
    if ((reg->SCR.BIT.RE != 0) // 受信許可がON？
        || (reg->SCR.BIT.TE != 0))
    { // 送信許可がON?
        return EBUSY;
    }

    uint8_t cks_value = 0xFF;
    uint8_t brr_value = 0xFF;
    float diff = 1.0f;

    for (uint8_t cks = 0; cks < 4; cks++)
    {
        float actual;
        float brr_real = calc_brr_value(bit_rate, cks);
        if (brr_real < 0.0f)
        {
            // cksが大きくなるほど分母が大きくなり、brr設定値は小さくなる。
            // そのため、これ以上のCKSは評価しても意味がない。
            break;
        }
        uint32_t brr = (uint32_t)(brr_real);

        float brr_err = fabs(brr_real - (float)(brr)); // BRRの実数と整数の誤差を取得
        if ((brr <= 255)                               // brrは設定可能な範囲？
            && (brr_err < diff))                       // BRRの浮動小数と整数の差は、現在値より小さい？
        {
            brr_value = (uint8_t)(brr);
            cks_value = cks;
            diff = brr_err;
        }
    }
    if (cks_value >= 4) // cksは設定可能値でない？
    {
        return EINVAL;
    }

    uint32_t mddr = bit_rate * 256.0f / calc_bit_rate(cks_value, brr_value);
    if ((mddr >= 0x80) && (mddr <= 0xFF))
    {
        reg->MDDR = (uint8_t)(mddr);
        reg->SEMR.BIT.BRME = 1u;
    }
    else
    {
        reg->SEMR.BIT.BRME = 0u;
    }

    reg->SMR.BIT.CKS = cks_value;
    reg->BRR = brr_value;

    return 0;
}

/**
 * @brief BRR値を計算する
 * @param bit_rate ビットレート[bps]
 * @param cks CKS設定値
 * @return BRR値
 */
static float calc_brr_value(uint32_t bit_rate, uint8_t cks)
{
    return ((float)(BSP_PCLKB_HZ) / ((float)(s_cks_coefs[cks]) * (float)(bit_rate)) - 1.0f);
}

/**
 * @brief I2Cのビットレート[Hz]を得る
 * @return ビットレート
 */
uint32_t i2c_get_bitrate(void)
{
    return get_bit_rate(&SCI6);
}

/**
 * @brief SCI簡易SCIモードでのビットレートを得る。
 * @param reg レジスタセット
 * @return ビットレート
 */
static uint32_t get_bit_rate(const volatile struct st_sci0* reg)
{
    uint8_t cks = reg->SMR.BIT.CKS & 0x3;
    uint8_t brr = reg->BRR;

    float bit_rate;
    if (reg->SEMR.BIT.BRME != 0)
    {
        uint8_t mddr = reg->MDDR;
        bit_rate = (float)(mddr) / 256.0f * calc_bit_rate(cks, brr);
    }
    else
    {
        bit_rate = calc_bit_rate(cks, brr);
    }

    return (uint32_t)(bit_rate);
}

/**
 * @brief ビットレート補正抜きのビットレートを計算する
 * @param cks CKS設定値(0,1,2,3)
 * @param brr BRRレジスタ値
 * @return ビットレート[bps]
 */
static float calc_bit_rate(uint8_t cks, uint8_t brr)
{
    return BSP_PCLKB_HZ / ((float)(s_cks_coefs[cks]) * ((float)(brr) + 1));
}

/**
 * @brief 同期I/OでI2C送信する。
 * @param slave_addr スレーブアドレス
 * @param tx_data 送信データ
 * @param tx_len 送信データ長
 * @param timeout_millis タイムアウト時間[ミリ秒]
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
int i2c_master_send_sync(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint32_t timeout_millis)
{
    int retval;

    retval = i2c_master_send_async(slave_addr, tx_data, tx_len, NULL);
    if (retval == 0)
    {
        retval = wait_transaction_done(timeout_millis);
    }

    return retval;
}

/**
 * @brief 同期I/Oで指定バイト数をI2C受信する。
 * @param slave_addr スレーブアドレス
 * @param rx_bufp 受信バッファ
 * @param rx_len 受信サイズ
 * @param timeout_millis タイムアウト時間[ミリ秒]
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
int i2c_master_receive_sync(uint8_t slave_addr, uint8_t* rx_bufp, uint16_t rx_len, uint32_t timeout_millis)
{
    int retval;

    retval = i2c_master_receive_async(slave_addr, rx_bufp, rx_len, NULL);
    if (retval == 0)
    {
        retval = wait_transaction_done(timeout_millis);
    }

    return retval;
}

/**
 * @brief 同期I/OでI2Cバスに送信し、その後指定バイト数を受信する。
 * @param slave_addr スレーブアドレス
 * @param tx_data 送信データ
 * @param tx_len 送信データ長
 * @param rx_bufp 受信バッファ
 * @param rx_len 受信サイズ
 * @param timeout_millis タイムアウト時間[ミリ秒]
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
int i2c_master_send_and_receive_sync(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint8_t* rx_bufp, uint16_t rx_len, uint32_t timeout_millis)
{
    int retval;

    retval = i2c_master_send_and_receive_async(slave_addr, tx_data, tx_len, rx_bufp, rx_len, NULL);
    if (retval == 0)
    {
        retval = wait_transaction_done(timeout_millis);
    }
    return retval;
}

/**
 * @brief I2Cステータスをエラー番号に変換する
 * @param status I2Cステータス
 * @return エラー番号
 */
static int convert_status_to_errno(sci_iic_ch_dev_status_t status)
{
    int retval;

    switch (status)
    {
    case SCI_IIC_NO_INIT: {
        retval = ENOTSUP;
        break;
    }
    case SCI_IIC_COMMUNICATION: {
        retval = EBUSY;
        break;
    }
    case SCI_IIC_NACK: {
        retval = EACCES;
        break;
    }
    case SCI_IIC_ERROR: {
        retval = EIO;
        break;
    }
    case SCI_IIC_IDLE:
    case SCI_IIC_FINISH:
    default: {
        retval = 0;
        break;
    }
    }
    return retval;
}

/**
 * @brief I2C API リターンコードをerrnoに変換して返す。
 * @param ret_code リターンコード
 * @return エラー番号
 */
static int convert_iic_return_to_errno(sci_iic_return_t ret_code)
{
    int retval;
    switch (ret_code)
    {
    case SCI_IIC_ERR_INVALID_ARG:  // Invalid argument.
    case SCI_IIC_ERR_INVALID_CHAN: // Invalid channel.
    {
        retval = EINVAL;
        break;
    }
    case SCI_IIC_ERR_LOCK_FUNC: // Specified channel is operating for other.
    case SCI_IIC_ERR_BUS_BUSY: {
        retval = EBUSY;
        break;
    }
    case SCI_IIC_ERR_NO_INIT: {
        retval = ENOTSUP;
        break;
    }
    case SCI_IIC_ERR_OTHER: {
        retval = EIO;
        break;
    }
    case SCI_IIC_SUCCESS:
    default: {
        retval = 0;
        break;
    }
    }
    return retval;
}

/**
 * @brief バストランザクションが完了するまで待つ。
 * @param timeout_millis タイムアウト時間[ミリ秒]
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
static int wait_transaction_done(uint32_t timeout_millis)
{
    int retval;
    uint32_t begin = hwtick_get();
    while (((hwtick_get() - begin) < timeout_millis) && (s_sci_iic_info.dev_sts != SCI_IIC_IDLE) // 待機状態でない？
           && (s_sci_iic_info.dev_sts != SCI_IIC_NACK)                                           // NACK受信してない？
           && (s_sci_iic_info.dev_sts != SCI_IIC_ERROR)                                          // エラーになってない？
           && (s_sci_iic_info.dev_sts != SCI_IIC_FINISH))                                        // トランザクション完了してない？
    {
        // do nothing.
    }
    sci_iic_ch_dev_status_t status = s_sci_iic_info.dev_sts;
    if (status == SCI_IIC_COMMUNICATION)
    {
        R_SCI_IIC_Control(&s_sci_iic_info, SCI_IIC_GEN_RESET);
        retval = ETIMEDOUT;
    }
    else
    {
        retval = convert_status_to_errno(status);
    }

    return retval;
}

/**
 * @brief 非同期I/Oで送信する。
 *        関数が成功した場合、コールバック関数で送信完了の通知を受け取るか、
 *        i2c_is_busy()で完了を待ちます。
 * @param slave_addr スレーブアドレス
 * @param tx_data 送信データ
 * @param tx_len 送信データ長
 * @param pcallback 完了時に通知を受けるコールバック関数。通知不要な場合にはNULL
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
int i2c_master_send_async(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, i2c_callback_func_t pcallback)
{
    int retval;
    if ((slave_addr >= 0x80) // スレーブアドレスが不正？
        || (tx_data == NULL) // 送信データがNULL？
        || (tx_len == 0))    // 送信データサイズが0？
    {
        retval = EINVAL;
    }
    else
    {
        s_slave_addr[0] = slave_addr;
        s_sci_iic_info.p_slv_adr = s_slave_addr;
        s_sci_iic_info.p_data1st = &(tx_data[0]);
        s_sci_iic_info.cnt1st = 1;
        if (tx_len >= 2)
        {
            s_sci_iic_info.p_data2nd = &(tx_data[1]);
            s_sci_iic_info.cnt2nd = tx_len - 1;
        }
        else
        {
            s_sci_iic_info.p_data2nd = NULL;
            s_sci_iic_info.cnt2nd = 0;
        }
        s_sci_iic_info.callbackfunc = on_transaction_done;
        sci_iic_return_t status = R_SCI_IIC_MasterSend(&s_sci_iic_info);
        if (status == SCI_IIC_SUCCESS)
        {
            s_callback = pcallback;
        }

        retval = convert_iic_return_to_errno(status);
    }
    return retval;
}
/**
 * @brief 非同期I/Oで受信します。
 *        関数が成功した場合、コールバック関数で送信完了の通知を受け取るか、
 *        i2c_is_busy()で完了を待ちます。
 * @param slave_addr スレーブアドレス
 * @param rx_bufp 受信バッファ
 * @param rx_len 受信バッファサイズ
 * @param pcallback 完了時に通知を受けるコールバック関数。通知不要な場合にはNULL
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
bool i2c_master_receive_async(uint8_t slave_addr, uint8_t* rx_bufp, uint16_t rx_len, i2c_callback_func_t pcallback)
{
    return i2c_master_send_and_receive_async(slave_addr, NULL, 0, rx_bufp, rx_len, pcallback);
}
/**
 * @brief 非同期I/Oで送信後、指定バイト数受信します。
 *        関数が成功した場合、コールバック関数で送信完了の通知を受け取るか、
 *        i2c_is_busy()で完了を待ちます。
 * @param slave_addr スレーブアドレス
 * @param tx_data 送信データ (送信データが無い場合にはNULL)
 * @param tx_len 送信データ長 (送信データが無い場合には0)
 * @param rx_bufp 受信バッファ
 * @param rx_len 受信バッファサイズ
 * @param pcallback 完了時に通知を受けるコールバック関数。通知不要な場合にはNULL
 * @return 成功した場合にはtrue, 失敗した場合にはfalseを返します。
 */
int i2c_master_send_and_receive_async(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint8_t* rx_bufp, uint16_t rx_len, i2c_callback_func_t pcallback)
{
    int retval;

    if ((slave_addr >= 0x80)                    // スレーブアドレスが不正？
        || ((tx_len > 0) && (tx_data == NULL))  // 送信指定があり、送信データがNULL？
        || ((rx_len > 0) && (rx_bufp == NULL))) // 受信指定があり、受信バッファがNULL？
    {
        retval = EINVAL;
    }
    else
    {
        s_slave_addr[0] = slave_addr;
        s_sci_iic_info.p_slv_adr = s_slave_addr;
        s_sci_iic_info.p_data1st = (tx_len > 0) ? tx_data : NULL;
        s_sci_iic_info.cnt1st = tx_len;
        s_sci_iic_info.p_data2nd = &(rx_bufp[0]);
        s_sci_iic_info.cnt2nd = rx_len;
        s_sci_iic_info.callbackfunc = on_transaction_done;

        sci_iic_return_t status = R_SCI_IIC_MasterReceive(&s_sci_iic_info);
        if (status == SCI_IIC_SUCCESS)
        {
            s_callback = pcallback;
        }
        retval = convert_iic_return_to_errno(status);
    }

    return retval;
}

/**
 * @brief トランザクションが完了したときに通知を受け取る。
 */
static void on_transaction_done(void)
{
    if (s_callback != NULL)
    {
        sci_iic_ch_dev_status_t st = s_sci_iic_info.dev_sts;
        int status = convert_status_to_errno(status);
        s_callback(status);
    }
}

/**
 * @brief バスビジーかどうかを判定する。
 * @return バスビジーの場合にはtrue, それ以外はfalse.
 */
bool i2c_is_busy(void)
{
    bool is_busy = false;

    sci_iic_mcu_status_t st;
    if (R_SCI_IIC_GetStatus(&s_sci_iic_info, &st) == SCI_IIC_SUCCESS)
    {
        is_busy = ((st.LONG & SCI_IIC_STATUS_BUSY) != 0) ? true : false;
    }
    return is_busy;
}
