/**
 * @file test-data コマンド定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdio.h>
#include "utils.h"
#include "test_signal.h"
#include "command_table.h"
#include "command_test_data.h"

static void cmd_test_data_output(int ac, char** av);
static void cmd_test_data_data(int ac, char** av);

/**
 * コマンドエントリテーブル
 */
//@formatter:off
static const struct cmd_entry CommandEntries[] = {
    {"output", "Output On/Off control.", cmd_test_data_output},
    {"data", "Set test data.", cmd_test_data_data},
};
//@formatter:on
/**
 * コマンドエントリ数
 */
static const int CommandEntryCount = (int)(sizeof(CommandEntries) / sizeof(struct cmd_entry));

/**
 * @brief test-data コマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
void cmd_test_data(int ac, char** av)
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
                printf("test-data %s - %s\n", pentry->cmd, pentry->desc);
            }
        }
    }

    return;
}
/**
 * @brief test-data outputコマンドを処理する。
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_test_data_output(int ac, char** av)
{
    if (ac >= 3)
    {
        bool is_on;
        if (!parse_boolean(av[2], &is_on))
        {
            printf("Invalid argument. %s\n", av[2]);
            return;
        }
        if (!test_signal_set_output(is_on))
        {
            printf("Set test signal output failure.\n");
            return;
        }
        printf("%s\n", test_signal_is_output() ? "on" : "off");
    }
    else
    {
        printf("%s\n", test_signal_is_output() ? "on" : "off");
    }
    return;
}

/**
 * @brief test-data data コマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_test_data_data(int ac, char** av)
{

    if (ac >= 3)
    {
        uint8_t d;
        if (!parse_u8(av[2], &d))
        {
            printf("Invalid argument. %s\n", av[2]);
            return;
        }
        if (!test_signal_set_data(d))
        {
            printf("Set test data failure.\n");
            return;
        }
        printf("%xh\n", test_signal_get_data());
    }
    else
    {
        printf("%xh\n", test_signal_get_data());
    }
}
