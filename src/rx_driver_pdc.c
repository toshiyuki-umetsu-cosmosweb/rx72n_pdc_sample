/**
 * @file Renesas RX PDCペリフェラル用ドライバコード
 * @author Cosmosweb Co.,Ltd. 2024
 * @note Renesas FIT PDCドライバを元に作成。
 *       対象マイコンはRX-72N
 */
#include <errno.h>
#include <platform.h>

#include "hwtick.h"
#include "rx_driver_pdc.h"

/**
 * @brief  初期PCKO 出力分周設定 (PCKDIV)
 *         クロックソースで指定したソースに対して、この分周で設定した出力が行われる。
 *         例えばクロックソースがPLLなら、PLLの周波数を、この分周されたものが出力される。
 */
#define PDC_CFG_PCKO_DIV (8)

/**
 * PDC タイムアウト時間[ミリ秒]
 */
#define PDC_WAIT_TIMEOUT_MILLIS (50u)

#define PDC_DISABLE_OPERATION (0)
#define PDC_ENABLE_OPERATION (1)

#define PDC_DISABLE_PIXCLK_INPUT (0)
#define PDC_ENABLE_PIXCLK_INPUT (1)

#define PDC_RESET_RELEASE (0)
#define PDC_RESET (1)

#define PDC_DISABLE_PCKO_OUTPUT (0)
#define PDC_ENABLE_PCKO_OUTPUT (1)

/**
 * @brief Sync信号極性 = L
 * @note ユーザーズガイドでは、「HSYNC信号はHighアクティブ」と記載されているが、
 *       これはキャプチャ対象のHSYNCレベルがHighの部分、という意味合いで、
 *       ビデオの同期信号とは意味合いが異なる。
 */
#define PDC_SYNC_SIGNAL_POLARITY_LOW (0)
/**
 * @brief Sync信号極性 = H
 * @note PDC_HSYNC_POLARITY_LOW_ACTIVEの説明を参照。
 */
#define PDC_SYNC_SIGNAL_POLARITY_HIGH (1)

/**
 * @brief VST上限
 */
#define PDC_VST_UPPER_LIMIT (0x0FFE)

/**
 * @brief HST上限
 */
#define PDC_HST_UPPER_LIMIT (0x0FFB)

/**
 * @brief VSZ下限
 */
#define PDC_VSZ_LOWER_LIMIT (0x0001)
/**
 * @brief VSZ上限
 */
#define PDC_VSZ_UPPER_LIMIT (0x0FFF)
/**
 * @brief HSZ下限
 */
#define PDC_HSZ_LOWER_LIMIT (0x0004)
/**
 * @brief HSZ上限
 */
#define PDC_HSZ_UPPER_LIMIT (0x0FFF)

/**
 * @brief 垂直方向カウンタ上限
 */
#define PDC_VSTVSZ_MIX_UPPER_LIMIT (0x0FFF)
/**
 * @brief 水平方向カウンタ上限
 */
#define PDC_HSTHSZ_MIX_UPPER_LIMIT (0x0FFF)

/**
 * キャプチャしたデータのエンディアンはリトルエンディアン
 */
#define PDC_EDS_LITTLE_ENDIAN (0)
/**
 * キャプチャしたデータのエンディアンはビッグエンディアン
 */
#define PDC_EDS_BIG_ENDIAN (1)

/**
 * @brief PDCドライバがオープンされているかどうかのフラグ
 */
static bool s_is_opened = false;

/**
 * @brief コールバック関数
 */
//@formatter:off
static pdc_callback_functions_t s_callback_functions = {.pcb_receive_data_ready = NULL, .pcb_frame_end = NULL, .pcb_error = NULL};
//@formatter:on

/**
 * @brief リセット開始時Tick
 */
static uint32_t s_reset_start_tick;
/**
 * @brief リセットDONE検知時に行う処理。
 *        NULLは処理待ちなし。
 */
static void (*s_reset_done_callback)(bool is_reset_done);

static void setup_io_pins(void);
static int setup_interrupts(const pdc_config_t* pcfg);
static int setup_pdc(pdc_config_t* pcfg);
static void on_pcfei_detected(void* pparam);
static void on_pceri_detected(void* pparam);
static void process_errors(void);
static void request_reset(void (*pcallback)(bool is_succeed));
static bool wait_reset_done(void);
static bool is_valid_capture_range(uint16_t hst, uint16_t vst, uint16_t hsz, uint16_t vsz);
static void on_reset_done_before_capture(bool is_reset_done);
static void set_module_stop(bool is_stop);

/**
 * @brief PDCペリフェラルをオープンする。
 * @param p_data_cfg 設定
 * @return 成功した場合には0, 失敗した場合にはエラー番号。
 */
int rx_driver_pdc_open(pdc_config_t* p_data_cfg)
{
    if (s_is_opened)
    {
        return 0;
    }

    setup_io_pins();

    if (p_data_cfg == NULL)
    {
        return EINVAL;
    }

    if (!R_BSP_HardwareLock(BSP_LOCK_PDC))
    {
        return EBUSY;
    }

    set_module_stop(false); // モジュールストップ解除

    if (R_BSP_InterruptWrite(BSP_INT_SRC_BL0_PDC_PCFEI, on_pcfei_detected) != BSP_INT_SUCCESS)
    {
        return EFAULT;
    }

    /* Performs registration of callback function for PCERI interrupt used by the PDC. */
    if (R_BSP_InterruptWrite(BSP_INT_SRC_BL0_PDC_PCERI, on_pceri_detected) != BSP_INT_SUCCESS)
    {
        return EFAULT;
    }

    s_callback_functions = p_data_cfg->p_callback; // memcpy.

    int retval = setup_interrupts(p_data_cfg);
    if (retval != 0)
    {
        R_BSP_HardwareUnlock(BSP_LOCK_PDC);
        return retval;
    }

    retval = setup_pdc(p_data_cfg);
    if (retval != 0)
    {
        R_BSP_HardwareUnlock(BSP_LOCK_PDC);
        return retval;
    }

    s_is_opened = true;

    return retval;
}

/**
 * @brief I/Oピン設定を行う。
 * @note RX-72N用
 */
static void setup_io_pins(void)
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

    return;
}

/**
 * @brief 割り込みをセットアップする。
 * @param pcfg 設定
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
static int setup_interrupts(const pdc_config_t* pcfg)
{
    if ((pcfg->int_priority_pcdfi > BSP_MCU_IPL_MAX)    // PCDFIのプライオリティが設定範囲外？
        || (pcfg->int_priority_pcefi > BSP_MCU_IPL_MAX) // PCEFI割り込みのプライオリティが設定範囲外？
        || (pcfg->int_priority_pceri > BSP_MCU_IPL_MAX) // PCERI割り込みのプライオリティが設定範囲外？
    )
    {
        return EINVAL;
    }

    IPR(PDC, PCDFI) = pcfg->int_priority_pcdfi;
    IEN(PDC, PCDFI) = 1;

    {
        bsp_int_ctrl_t int_ctrl = {
            .ipl = (pcfg->int_priority_pcefi > pcfg->int_priority_pceri) ? pcfg->int_priority_pcefi : pcfg->int_priority_pceri,
        };
        // Note: GROUPBL0割り込みのプライオリティを設定するので、
        //       FIT BSP のAPIを使用して設定する。
        //       こうすると、他でもGROUPBL0を使っていた場合に、上手いこと競合を解決してくれる。
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_PDC_PCFEI, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void*)&int_ctrl);
        // Note: PCFEIはPCFEIと同じグループBL0にマップされているので、
        //       R_BSP_InterruptControlを呼び出す必要はない。
        // R_BSP_InterruptControl(BSP_INT_SRC_BL0_PDC_PCFEI,
        //      BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void*)(&int_ctrl));
    }

    {
        bsp_int_ctrl_t int_ctrl;

        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
        EN(PDC, PCFEI) = 0; // 割り込み許可=0でクリアされる
        EN(PDC, PCFEI) = 1;
        EN(PDC, PCERI) = 0; // 割り込み許可=0でクリアされる
        EN(PDC, PCERI) = 1;
        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
    }

    IR(PDC, PCDFI) = 0; /* Interrupt request is cleared. */

    R_BSP_InterruptRequestEnable(VECT(PDC, PCDFI));

    // Note: PCCR0レジスタはPCE=0にして設定する必要がある
    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION;

    PDC.PCCR0.BIT.DFIE = pcfg->interrupt_setting.dfie_ien ? 1 : 0;
    PDC.PCCR0.BIT.FEIE = pcfg->interrupt_setting.feie_ien ? 1 : 0;
    PDC.PCCR0.BIT.OVIE = pcfg->interrupt_setting.ovie_ien ? 1 : 0;
    PDC.PCCR0.BIT.UDRIE = pcfg->interrupt_setting.udrie_ien ? 1 : 0;
    PDC.PCCR0.BIT.VERIE = pcfg->interrupt_setting.verie_ien ? 1 : 0;
    PDC.PCCR0.BIT.HERIE = pcfg->interrupt_setting.herie_ien ? 1 : 0;

    return 0;
}

/**
 * @brief PDCを設定する
 * @param pcfg 設定
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
static int setup_pdc(pdc_config_t* pcfg)
{
    if (pcfg == NULL)
    {
        return EINVAL;
    }

    uint16_t hst = pcfg->capture_pos.hst_position;
    uint16_t vst = pcfg->capture_pos.vst_position;
    uint16_t hsz = pcfg->capture_size.hsz_size;
    uint16_t vsz = pcfg->capture_size.vsz_size;
    if (!is_valid_capture_range(hst, vst, hsz, vsz))
    {
        return EINVAL;
    }

    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION; // 受信動作禁止

    uint16_t pckdiv_value = ((PDC_CFG_PCKO_DIV / 2) - 1);
    PDC.PCCR0.BIT.PCKDIV = pckdiv_value;          // PCKO分周設定
    PDC.PCCR0.BIT.PCKOE = PDC_ENABLE_PCKO_OUTPUT; // PCKO出力を許可

    PDC.PCCR0.BIT.PCKE = PDC_ENABLE_PIXCLK_INPUT; // PCLKE入力許可

    request_reset(NULL); // PDCリセット。 PIXCLKが入力されていないと、ここは失敗する。
    wait_reset_done();

    PDC.VCR.BIT.VST = pcfg->capture_pos.vst_position;
    PDC.HCR.BIT.HST = pcfg->capture_pos.hst_position;

    PDC.VCR.BIT.VSZ = pcfg->capture_size.vsz_size;
    PDC.HCR.BIT.HSZ = pcfg->capture_size.hsz_size;

    PDC.PCCR0.BIT.VPS = pcfg->is_vsync_hactive ? PDC_SYNC_SIGNAL_POLARITY_HIGH : PDC_SYNC_SIGNAL_POLARITY_LOW;
    PDC.PCCR0.BIT.HPS = pcfg->is_hsync_hactive ? PDC_SYNC_SIGNAL_POLARITY_HIGH : PDC_SYNC_SIGNAL_POLARITY_LOW;

#if (defined(__BIG) || defined(__RX_BIG_ENDIAN__))
    PDC.PCCR0.BIT.EDS = PDC_EDS_BIG_ENDIAN;
#else
    PDC.PCCR0.BIT.EDS = PDC_EDS_LITTLE_ENDIAN;
#endif

    return 0;
}

/**
 * @brief Ends operation by the PDC and puts it into the module stop state.
 * @retval    PDC_SUCCESS      Processing finished successfully.
 * @retval    PDC_ERR_NOT_OPEN R_PDC_Open has not been run.
 * @details   This function is performed to shut down the PDC. See section 3.2 in application note for details.
 * @note      Use this API function after running R_PDC_Open and confirming that the return value is PDC_SUCCESS.
 */
int rx_driver_pdc_close(void)
{
    if (!s_is_opened)
    {
        return 0;
    }

    PDC.PCCR1.BIT.PCE = 0; // 受信停止

    R_BSP_InterruptRequestDisable(VECT(PDC, PCDFI)); // PCDFI(受信データレディ)割り込み停止
    PDC.PCCR0.LONG &= (~0x03F0);
    while (PDC.PCCR0.BIT.DFIE != 0)
    {
        // do nothing.
    }

    IEN(PDC, PCDFI) = 0;
    IR(PDC, PCDFI) = 0;

    bsp_int_ctrl_t int_ctrl;
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
    EN(PDC, PCFEI) = 0;
    EN(PDC, PCERI) = 0;
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);

    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION;
    PDC.PCCR0.BIT.PCKOE = PDC_DISABLE_PCKO_OUTPUT;
    PDC.PCCR0.BIT.PCKE = PDC_DISABLE_PIXCLK_INPUT;

    set_module_stop(true);

    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_PDC));

    s_is_opened = false;

    return 0;
}

/**
 * @brief ドライバの更新処理を行う。
 */
void rx_driver_pdc_update(void)
{
    if (s_reset_done_callback != NULL) // リセット完了待ち処理がある？
    {
        bool is_reset_done = (PDC.PCCR0.BIT.PRST == PDC_RESET_RELEASE);
        if (is_reset_done                                    // リセット完了した？
            || ((hwtick_get() - s_reset_start_tick) >= 500)) // リセット開始してから500ミリ秒経過した？
        {
            s_reset_done_callback(is_reset_done);
            s_reset_done_callback = NULL;
        }
    }

    return;
}
/**
 * @brief PDCの割り込みプライオリティを設定する
 * @param type 割り込み種類(PDC_INTERRUPT_x)
 * @param priority プライオリティ (BSP_MC_IP_MAX以下とすること)
 * @return 成功した場合には0、失敗した場合にはエラー番号
 */
int rx_driver_pdc_set_irq_priority(int type, uint8_t priority)
{
    if ((priority < 0) || (priority > BSP_MCU_IPL_MAX))
    {
        return EINVAL;
    }

    int retval;
    switch (type)
    {
    case PDC_INTERRUPT_PCDFI: {
        IPR(PDC, PCDFI) = priority;
        retval = 0;
        break;
    }
    case PDC_INTERRUPT_PCFEI:
    case PDC_INTERRUPT_PCERI: {
        bsp_int_ctrl_t int_ctrl = {.ipl = priority};

        // Note: GROUPBL0割り込みのプライオリティを設定するので、
        //       FIT BSP のAPIを使用して設定する。
        //       こうすると、他でもGROUPBL0を使っていた場合に、上手いこと競合を解決してくれる。
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_PDC_PCFEI, BSP_INT_CMD_GROUP_INTERRUPT_ENABLE, (void*)&int_ctrl);
        retval = 0;
        break;
    }
    default: {
        retval = EINVAL;
    }
    }

    return retval;
}

/**
 * @brief PDCの割り込みプライオリティを取得する。
 * @param type 割り込み種類(PDC_INTERRUPT_x)
 * @return 割り込みプライオリティ。typeが不正な場合には0を返す。
 */
uint8_t rx_driver_pdc_get_irq_priority(int type)
{
    uint8_t retval;
    switch (type)
    {
    case PDC_INTERRUPT_PCDFI: {
        retval = IPR(PDC, PCDFI);
        break;
    }
    case PDC_INTERRUPT_PCFEI:
    case PDC_INTERRUPT_PCERI: {
        retval = IPR(ICU, GROUPBL0); // PCFEI, PCEFIはGROUPBL0割り込みになっている。
        break;
    }
    default: {
        retval = 0;
        break;
    }
    }

    return retval;
}

/**
 * @brief 割り込み設定をする
 * @param psetting 割り込み設定データ
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_set_interrupt_setting(const pdc_interrupt_setting_t* psetting)
{
    if (psetting == NULL)
    {
        return EINVAL;
    }

    if (PDC.PCCR1.BIT.PCE != 0) // 受信動作中？
    {
        return EBUSY;
    }

    PDC.PCCR0.BIT.DFIE = psetting->dfie_ien ? 1 : 0;
    PDC.PCCR0.BIT.FEIE = psetting->feie_ien ? 1 : 0;
    PDC.PCCR0.BIT.OVIE = psetting->ovie_ien ? 1 : 0;
    PDC.PCCR0.BIT.UDRIE = psetting->udrie_ien ? 1 : 0;
    PDC.PCCR0.BIT.VERIE = psetting->verie_ien ? 1 : 0;
    PDC.PCCR0.BIT.HERIE = psetting->herie_ien ? 1 : 0;

    return 0;
}
/**
 * @brief 割り込み設定を取得する
 * @param psetting 割り込み設定データを格納する変数
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_get_interrupt_setting(pdc_interrupt_setting_t* psetting)
{
    if (psetting == NULL)
    {
        return EINVAL;
    }

    psetting->dfie_ien = PDC.PCCR0.BIT.DFIE != 0;
    psetting->feie_ien = PDC.PCCR0.BIT.FEIE != 0;
    psetting->ovie_ien = PDC.PCCR0.BIT.OVIE != 0;
    psetting->udrie_ien = PDC.PCCR0.BIT.UDRIE != 0;
    psetting->verie_ien = PDC.PCCR0.BIT.VERIE != 0;
    psetting->herie_ien = PDC.PCCR0.BIT.HERIE != 0;

    return 0;
}

/**
 * @brief 受信動作ON/OFFを設定する
 * @param is_enabled 受信動作をONにする場合にはtrue, それ以外はfalse.
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_set_receive_enable(bool is_enabled)
{
    PDC.PCCR1.BIT.PCE = (is_enabled) ? 1 : 0;
    return 0;
}

/**
 * @brief 受信動作中かどうかを判定する
 * @return 受信動作中の場合にはtrue, それ以外はfalse.
 */
bool rx_driver_pdc_is_receiving(void)
{
    return (s_is_opened && (PDC.PCCR1.BIT.PCE != 0));
}
/**
 * @brief リセット開始する
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_reset(void)
{
    PDC.PCCR1.BIT.PCE = 0;
    request_reset(NULL);
    return 0;
}
/**
 * @brief リセット中かどうかを判定する
 * @return リセット中の場合にはtrue, それ以外はfalse.
 */
bool rx_driver_pdc_is_resetting(void)
{
    return s_is_opened && (PDC.PCCR0.BIT.PRST != PDC_RESET_RELEASE);
}

/**
 * フレーム終了割り込み検知時に通知を受け取る。
 * @param pparam パラメータ
 */
static void on_pcfei_detected(void* pparam)
{
    R_BSP_InterruptsEnable(); // 多重割り込み許可(他の高優先度処理を許可する)

    // 転送完了待ち。
    // DMAまたはDTCで転送が行われるので、フレーム終了割り込み後
    // 転送完了までディレイが生じる。
    uint32_t wait_cout = 0;
    while (PDC.PCSR.BIT.FEMPF == 0) // FIFOは空でない？
    {
        if (PDC.PCSR.BIT.UDRF != 0) // アンダーランあり？ (FIFOが空の時に読み出し=バグ)
        {
            if (PDC.PCSR.BIT.FEF != 0) // フレーム末尾？
            {
                PDC.PCSR.BIT.FEF = 0;
            }
            process_errors();
            return;
        }

        if (wait_cout >= 300) // 300回ウォッチして終わってない？
        {
            if (PDC.PCSR.BIT.FEF != 0)
            {
                PDC.PCSR.BIT.FEF = 0;
            }

            /* Check registration frame end callback function */
            if (s_callback_functions.pcb_frame_end != 0)
            {
                pdc_event_arg_t cb_arg = {.event_id = PDC_EVT_ID_TRANSFER_TIMEOUT, .errors = 0};
                s_callback_functions.pcb_frame_end(&cb_arg);
            }
            return;
        }

        wait_cout++;
    }

    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION; // キャプチャ停止

    if (PDC.PCSR.BIT.FEF != 0)
    {
        PDC.PCSR.BIT.FEF = 0;
    }

    if (s_callback_functions.pcb_frame_end != NULL)
    {
        pdc_event_arg_t cb_arg = {.event_id = PDC_EVT_ID_FRAMEEND, .errors = 0};
        s_callback_functions.pcb_frame_end(&cb_arg);
    }

    return;
}

/**
 * @brief PCERI割り込み発生時に通知を受け取る。
 * @param pparam パラメータ
 */
static void on_pceri_detected(void* pparam)
{
    process_errors();
    return;
}

/**
 * @brief エラーを処理する
 */
static void process_errors(void)
{
    pdc_event_arg_t cb_arg = {.event_id = PDC_EVT_ID_ERROR, .errors = 0};

    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION; // 受信停止

    if (PDC.PCSR.BIT.OVRF != 0) // オーバーランあり？
    {
        PDC.PCSR.BIT.OVRF = 0;
        cb_arg.errors |= PDC_ERROR_OVERRUN;
    }
    if (PDC.PCSR.BIT.UDRF != 0) // アンダーランエラーあり？
    {
        PDC.PCSR.BIT.UDRF = 0;
        cb_arg.errors |= PDC_ERROR_UNDERRUN;
    }
    if (PDC.PCSR.BIT.VERF != 0) // 垂直パラメータエラーあり？
    {
        cb_arg.errors |= PDC_ERROR_VPARAM;
        PDC.PCSR.BIT.VERF = 0;
    }
    if (PDC.PCSR.BIT.HERF != 0) // 水平パラメータエラーあり？
    {
        cb_arg.errors |= PDC_ERROR_HPARAM;
        PDC.PCSR.BIT.HERF = 0;
    }

    if (s_callback_functions.pcb_error != NULL) // エラー時コールバックあり？
    {
        (*s_callback_functions.pcb_error)(&cb_arg);
    }

    return;
}

/**
 * @brief リセット要求をする
 * @param pcallback
 */
static void request_reset(void (*pcallback)(bool is_succeed))
{
    s_reset_start_tick = hwtick_get();
    s_reset_done_callback = pcallback;
    PDC.PCCR0.BIT.PRST = PDC_RESET;

    return;
}

/**
 * @brief リセットが完了するのを最大で PDC_WAIT_TIMEOUT_MILLIS [ミリ秒]だけ待つ。
 *        初期化処理以外では、基本的に呼び出さない想定。
 * @return リセット完了した場合にはtrue, 完了しなかった場合にはfalse.
 */
static bool wait_reset_done(void)
{
    while ((PDC.PCCR0.BIT.PRST != PDC_RESET_RELEASE) && ((hwtick_get() - s_reset_start_tick) <= PDC_WAIT_TIMEOUT_MILLIS))
    {
        // do nothing.
    }

    bool is_wait_done = (PDC.PCCR0.BIT.PRST == PDC_RESET_RELEASE);
    if (s_reset_done_callback != NULL)
    {
        s_reset_done_callback(is_wait_done);
        s_reset_done_callback = NULL;
    }

    return is_wait_done;
}

/**
 * @brief 同期信号の極性設定を設定する。
 * @param is_hsync_hactive 水平同期信号極性の設定(true:H-Active, false:L-Active)
 * @param is_vsync_hactive 垂直同期信号極性の設定(true:H-Active, false:L-Active)
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_set_signal_polarity(bool is_hsync_hactive, bool is_vsync_hactive)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }
    if (rx_driver_pdc_is_receiving())
    {
        return EBUSY;
    }

    PDC.PCCR0.BIT.HPS = is_hsync_hactive ? PDC_SYNC_SIGNAL_POLARITY_HIGH : PDC_SYNC_SIGNAL_POLARITY_LOW;
    PDC.PCCR0.BIT.VPS = is_vsync_hactive ? PDC_SYNC_SIGNAL_POLARITY_HIGH : PDC_SYNC_SIGNAL_POLARITY_LOW;

    return 0;
}

/**
 * @brief 同期信号の極性設定を取得する。
 * @param is_hsync_hactive 水平同期信号極性の設定を取得する変数
 * @param is_vsync_hactive 垂直同期信号極性の設定を取得する変数
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_get_signal_polarity(bool* is_hsync_hactive, bool* is_vsync_hactive)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }

    if (is_hsync_hactive != NULL)
    {
        (*is_hsync_hactive) = PDC.PCCR0.BIT.HPS == PDC_SYNC_SIGNAL_POLARITY_HIGH;
    }
    if (is_vsync_hactive != NULL)
    {
        (*is_vsync_hactive) = PDC.PCCR0.BIT.VPS == PDC_SYNC_SIGNAL_POLARITY_HIGH;
    }

    return 0;
}

/**
 * @brief キャプチャ位置設定を設定する。
 *        受信中に実行するとエラーになるため、受信停止してから呼び出すこと。
 * @param ppos 位置設定
 * @param psize サイズ設定
 * @return 成功した場合には0, 失敗した場合にはエラー番号を返す。
 */
int rx_driver_pdc_set_position_size(const pdc_position_t* ppos, const pdc_capture_size_t* psize)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }

    uint16_t hst = (ppos != NULL) ? ppos->hst_position : PDC.HCR.BIT.HST;
    uint16_t vst = (ppos != NULL) ? ppos->vst_position : PDC.VCR.BIT.VST;
    uint16_t hsz = (psize != NULL) ? psize->hsz_size : PDC.HCR.BIT.HSZ;
    uint16_t vsz = (psize != NULL) ? psize->vsz_size : PDC.VCR.BIT.VSZ;

    if (!is_valid_capture_range(hst, vst, hsz, vsz))
    {
        return EINVAL;
    }

    if (rx_driver_pdc_is_receiving())
    {
        return EBUSY;
    }

    PDC.VCR.BIT.VST = vst;
    PDC.HCR.BIT.HST = hst;
    PDC.VCR.BIT.VSZ = vsz;
    PDC.HCR.BIT.HSZ = hsz;

    return 0;
}

/**
 * @brief hst, vst, hsz, vszで指定されるキャプチャ範囲が、このハードウェアで対応可能な設定かどうかを判定する。
 * @param hst 水平方向開始位置。(HSYNC検出後のバイト数)
 * @param vst 垂直方向開始位置。(VSYNC検出後のライン数)
 * @param hsz 水平方向キャプチャバイト数
 * @param vsz 垂直方向キャプチャライン数
 * @return 対応可能な場合にはtrue, それ以外はfalse.
 */
static bool is_valid_capture_range(uint16_t hst, uint16_t vst, uint16_t hsz, uint16_t vsz)
{
    return ((hst <= PDC_HST_UPPER_LIMIT)                   // 水平位置が設定可能値範囲内？
            && (vst <= PDC_VST_UPPER_LIMIT)                // 垂直位置が設定可能値範囲内？
            && (hsz >= PDC_HSZ_LOWER_LIMIT)                // 水平サイズが設定可能最小値以上？
            && (hsz <= PDC_HSZ_UPPER_LIMIT)                // 水平サイズが設定可能最大値以下？
            && (vsz >= PDC_VSZ_LOWER_LIMIT)                // 垂直幅が設定可能最小値以上？
            && (vsz <= PDC_VSZ_UPPER_LIMIT)                // 垂直幅が設定可能最大値以下？
            && ((hst + hsz) <= PDC_HSTHSZ_MIX_UPPER_LIMIT) // 水平方向処理に必要なカウンタ幅が、カウンタ上限以下
            && ((vst + vsz) <= PDC_VSTVSZ_MIX_UPPER_LIMIT) // 垂直方向処理に必要なカウンタ幅が、カウンタ上限以下
    );
}

/**
 * @brief キャプチャ範囲を得る。
 * @param ppos キャプチャ開始位置を取得するオブジェクトのアドレス
 * @param psize キャプチャサイズを取得するオブジェクトのアドレス
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_get_position_size(pdc_position_t* ppos, pdc_capture_size_t* psize)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }

    if (ppos != NULL)
    {
        ppos->hst_position = PDC.HCR.BIT.HST;
        ppos->vst_position = PDC.VCR.BIT.VST;
    }

    if (psize != NULL)
    {
        psize->hsz_size = PDC.HCR.BIT.HSZ;
        psize->vsz_size = PDC.VCR.BIT.VSZ;
    }

    return 0;
}

/**
 * @brief キャプチャを開始する
 * @return 失敗した場合
 */
int rx_driver_pdc_capture_start(void)
{
    bsp_int_ctrl_t int_ctrl;
    PDC.PCCR1.BIT.PCE = PDC_DISABLE_OPERATION;

    uint32_t value = PDC.PCCR0.LONG;
    PDC.PCCR0.BIT.DFIE = 0;
    PDC.PCCR0.BIT.FEIE = 0;
    PDC.PCCR0.BIT.OVIE = 0;
    PDC.PCCR0.BIT.UDRIE = 0;
    PDC.PCCR0.BIT.VERIE = 0;
    PDC.PCCR0.BIT.HERIE = 0;
    PDC.PCCR0.LONG |= (value & 0x000007F0); // PRST, HPS, VPS, PCKEは0を書く

    IR(PDC, PCDFI) = 0;

    if (EN(PDC, PCFEI) != 0)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
        EN(PDC, PCFEI) = 0; // EN=0で割り込みフラグもクリアされる。
        EN(PDC, PCFEI) = 1;
        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
    }

    if (EN(PDC, PCERI) != 0)
    {
        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
        EN(PDC, PCERI) = 0; // EN=0で割り込みフラグもクリアされる。
        EN(PDC, PCERI) = 1;
        R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
    }

    request_reset(on_reset_done_before_capture);

    return 0;
}

/**
 * @brief キャプチャ開始時のリセット完了時処理
 * @param is_reset_done リセットが完了した場合にはtrue
 */
static void on_reset_done_before_capture(bool is_reset_done)
{
    if (is_reset_done)
    {
        // なんかこれを実行すると、USB CDCが通信できなくなるんだけど。
        // CPU取られっぱなしになるのかな？
        PDC.PCCR1.BIT.PCE = PDC_ENABLE_OPERATION;
    }

    return;
}
/**
 * @brief pstatで指定されるステータスをクリアする。
 * @param pstat クリアするステータスを格納した pdc_stat_t オブジェクト
 * @return 成功した場合には0, 失敗した場合にはエラー番号。
 */
int rx_driver_pdc_clear_status(pdc_stat_t* pstat)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }

    if (pstat == NULL)
    {
        return EINVAL;
    }

    // 読み出して0書き込みなので、一度読み出す必要がある。
    uint32_t dummy __attribute__((unused)) = PDC.PCSR.LONG;

    if (pstat->frame_end)
    {
        PDC.PCSR.BIT.FEF = 0;
    }
    if (pstat->overrun)
    {
        PDC.PCSR.BIT.OVRF = 0;
    }
    if (pstat->underrun)
    {
        PDC.PCSR.BIT.UDRF = 0;
    }
    if (pstat->verf_error)
    {
        PDC.PCSR.BIT.VERF = 0;
    }
    if (pstat->herf_error)
    {
        PDC.PCSR.BIT.HERF = 0;
    }
    return 0;
}

/**
 * @brief PDCのステータスを得る。
 * @param pstat ステータスを取得する構造体
 * @return 成功した場合には0, 失敗した場合にはエラー番号
 */
int rx_driver_pdc_get_status(pdc_stat_t* pstat)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }
    if (pstat == NULL)
    {
        return EINVAL;
    }

    pstat->is_frame_busy = PDC.PCSR.BIT.FBSY != 0;
    pstat->fifo_empty = PDC.PCSR.BIT.FEMPF != 0;
    pstat->frame_end = PDC.PCSR.BIT.FEF != 0;
    pstat->overrun = PDC.PCSR.BIT.OVRF != 0;
    pstat->underrun = PDC.PCSR.BIT.UDRF != 0;
    pstat->verf_error = PDC.PCSR.BIT.VERF != 0;
    pstat->herf_error = PDC.PCSR.BIT.HERF != 0;

    return 0;
}

/**
 * @brief モニタステータスを得る。
 * @param pstat モニターステータスを格納する構造体のアドレス
 * @return 成功した場合には0失敗した場合にはエラー番号。
 */
int rx_driver_pdc_get_monitor_stat(pdc_monitor_stat_t* pstat)
{
    if (!s_is_opened)
    {
        return ENOTSUP;
    }
    if (pstat == NULL)
    {
        return EINVAL;
    }

    pstat->vsync = PDC.PCMONR.BIT.VSYNC != 0;
    pstat->hsync = PDC.PCMONR.BIT.HSYNC != 0;

    return 0;
}

/**
 * @brief モジュールストップ状態を設定する。
 * @param is_stop モジュールストップさせる場合にはtrue, ストップ解除する場合にはfalse
 */
static void set_module_stop(bool is_stop)
{
    bsp_int_ctrl_t int_ctrl;

    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
    MSTP(PDC) = (is_stop) ? 1 : 0;
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    return;
}

/**
 * @brief PCDFI(受信データレディ)割り込みハンドラ
 */
R_BSP_PRAGMA_STATIC_INTERRUPT(pdc_pcdfi_isr, VECT(PDC, PCDFI))
R_BSP_ATTRIB_STATIC_INTERRUPT void pdc_pcdfi_isr(void)
{
    if (s_callback_functions.pcb_receive_data_ready != NULL)
    {
        pdc_event_arg_t arg = {.event_id = PDC_EVT_ID_DATAREADY, .errors = 0};
        s_callback_functions.pcb_receive_data_ready(&arg);
    }

    return;
}
