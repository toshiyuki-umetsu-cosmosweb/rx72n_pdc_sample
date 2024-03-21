/**
 * @file pdcコマンド定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "pdc.h"
#include "command_table.h"
#include "command_pdc.h"

static void cmd_pdc_capture(int ac, char** av);
static void on_capture_done(const struct pdc_status* pstat);
static void cmd_pdc_stop(int ac, char** av);
static void cmd_pdc_state(int ac, char** av);
static void print_pdc_status(const struct pdc_status* pstat);
static void cmd_pdc_capture_range(int ac, char** av);
static void cmd_pdc_signal_polarity(int ac, char** av);
static bool parse_polarity(const char* str, bool* polarity);
static void cmd_pdc_reset(int ac, char** av);

/**
 * コマンドエントリテーブル
 */
//@formatter:off
static const struct cmd_entry CommandEntries[] = {
    {"capture", "Capture frame.", cmd_pdc_capture},
    {"stop", "Stop capture.", cmd_pdc_stop},
    {"state", "Get status.", cmd_pdc_state},
    {"capture-range", "Set/Get capture range.", cmd_pdc_capture_range},
    {"signal-polarity", "Set/Get signal polarity setting.", cmd_pdc_signal_polarity},
    {"reset", "Reset status.", cmd_pdc_reset},
};
//@formatter:on
/**
 * コマンドエントリ数
 */
static const int CommandEntryCount = (int)(sizeof(CommandEntries) / sizeof(struct cmd_entry));

/**
 * @brief pdcコマンドを処理する。
 * @param ac 引数の数
 * @param av 引数配列
 */
void cmd_pdc(int ac, char** av)
{
    if (ac >= 2)
    {
        const struct cmd_entry* pentry = command_table_find_cmd(CommandEntries, CommandEntryCount, av[1]);
        if (pentry != NULL)
        {
            pentry->cmd_proc(ac, av);
        }
        else
        {
            printf("Unknown subcommand: %s\n", av[1]);
        }
    }
    else
    {
        for (uint32_t i = 0u; i < CommandEntryCount; i++)
        {
            const struct cmd_entry* pentry = &(CommandEntries[i]);
            if ((pentry->cmd != NULL) && (pentry->desc != NULL))
            {
                printf("pdc %s - %s\n", pentry->cmd, pentry->desc);
            }
        }
    }

    return;
}

/**
 * @brief pdc captureコマンドを処理する。
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_capture(int ac, char** av)
{
    if (!pdc_start_capture(on_capture_done))
    {
        printf("Could not start capture.\n");
        return;
    }

    printf("Capture started.\n");
    return;
}

/**
 * @brief キャプチャが完了したときの処理を行う
 * @param pstat PDCステータス
 */
static void on_capture_done(const struct pdc_status* pstat)
{
    printf("Capture done.\n");
    print_pdc_status(pstat);
    return;
}

/**
 * @brief pdc stop コマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_stop(int ac, char** av)
{
    pdc_stop_capture();

    return;
}

/**
 * @brief pdc stateコマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_state(int ac, char** av)
{
    struct pdc_status status;

    if (!pdc_get_status(&status))
    {
        printf("Could not get state.\n");
        return;
    }

    print_pdc_status(&status);
    return;
}

/**
 * @brief PDCのステータスを表示する
 * @param pstat 表示するステータス
 */
static void print_pdc_status(const struct pdc_status* pstat)
{
    printf("%s\n", (pstat->is_receiving ? "Running" : "Idle"));
    printf("RESET = %d\n", pstat->is_resetting ? 1 : 0);
    printf("FIFO = %s\n", pstat->is_fifo_empty ? "Empty" : "DataExists");
    printf("FBSY = %d\n", pstat->is_data_receiving ? 1 : 0);
    printf("FrameEnd = %d\n", pstat->is_frame_end ? 1 : 0);
    printf("Overrun = %d\n", pstat->has_overrun ? 1 : 0);
    printf("Underrun = %d\n", pstat->has_underrun ? 1 : 0);
    printf("VLineError = %d\n", pstat->has_vline_err ? 1 : 0);
    printf("HSizeError = %d\n", pstat->has_hsize_err ? 1 : 0);
    printf("Captured = %u / %u\n", pstat->received_len, pstat->total_len);

    return;
}

/**
 * @brief pdc capture-range コマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_capture_range(int ac, char** av)
{
    if (ac == 7)
    {
        uint16_t xst, xsize, yst, ysize;
        uint8_t bpp;

        if (!parse_u16(av[2], &xst) || !parse_u16(av[3], &xsize) || !parse_u16(av[4], &yst) || !parse_u16(av[5], &ysize) || !parse_u8(av[6], &bpp))
        {
            printf("Invalid arguments.\n");
            return;
        }

        if (!pdc_set_capture_range(xst, xsize, yst, ysize, bpp))
        {
            printf("Could not set capture range.\n");
        }
        else
        {
            printf("Set capture range.\n");
        }
    }
    else if (ac == 4)
    {
        uint16_t xst, xsize, yst, ysize;
        uint8_t bpp;
        pdc_get_capture_range(&xst, &xsize, &yst, &ysize, &bpp);
        if (!parse_u16(av[2], &xsize) || !parse_u16(av[3], &ysize))
        {
            printf("Invalid arguments.\n");
            return ;
        }
        if (!pdc_set_capture_range(xst, xsize, yst, ysize, bpp))
        {
            printf("Could not set capture range.\n");
        }
        else
        {
            printf("Set capture range.\n");
        }
    }
    else if (ac <= 2)
    {
        uint16_t xst, xsize, yst, ysize;
        uint8_t bpp;
        pdc_get_capture_range(&xst, &xsize, &yst, &ysize, &bpp);
        printf("%d %d %d %d %d\n", xst, xsize, yst, ysize, bpp);
    }
    else
    {
        printf("usage:\n");
        printf("  pdc capture-range xst# xsize# yst# ysize# bpp#\n");
        printf("  pdc capture-range xsize# ysize#\n");
        printf("  pdc capture-range \n");
    }

    return;
}

/**
 * @brief pdc signal-polarityコマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_signal_polarity(int ac, char** av)
{
    if (ac == 4)
    {
        bool h_pol;
        bool v_pol;
        if (!parse_polarity(av[2], &h_pol) || !parse_polarity(av[3], &v_pol))
        {
            printf("Invalid polarity.\n");
            return;
        }

        if (!pdc_set_signal_polarity(h_pol, v_pol))
        {
            printf("Set polarity failure.\n");
        }
    }
    else if (ac == 2)
    {
        bool h_pol;
        bool v_pol;
        if (!pdc_get_signal_polarity(&h_pol, &v_pol))
        {
            printf("Could not get signal polarity.\n");
            return;
        }

        printf("HSync=%s VSync=%s\n", (h_pol ? "H-Active" : "L-Active"), (v_pol ? "H-Active" : "L-Active"));
    }
    else
    {
        printf("usage:\n");
        printf("  pdc signal-polarity [ h-pol$ v-pol$ ]\n");
    }

    return;
}

/**
 * @brief 極性指定を解析する
 *        "H"または"H-Active" で正論理
 *        "L"または"L-Active" で負論理
 *        数値指定の場合には0を超える値で正論理、0以下で負論理と解釈する
 * @param str 解析対象の文字列
 * @param polarity 極性を取得する変数。(true:H-Active, false:L-Active)
 * @return 成功した場合にはtrue, 失敗した場合にはfalse.
 */
static bool parse_polarity(const char* str, bool* polarity)
{
    bool is_succeed = false;
    if ((strcasecmp(str, "h") == 0) || (strcasecmp(str, "h-active") == 0))
    {
        (*polarity) = true;
        is_succeed = true;
    }
    else if ((strcasecmp(str, "l") == 0) || (strcasecmp(str, "l-active") == 0))
    {
        (*polarity) = false;
        is_succeed = true;
    }
    else
    {
        char* p;
        int value = strtol(str, &p, 0);
        if ((p != NULL) && (*p == '\0'))
        {
            (*polarity) = (value > 0);
            is_succeed = true;
        }
    }

    return is_succeed;
}

/**
 * @brief pdc resetコマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_pdc_reset(int ac, char** av)
{
    if (!pdc_reset(500))
    {
        printf("Reset failure.\n");
    }
    else
    {
        printf("Reset done.\n");
    }

    return;
}
