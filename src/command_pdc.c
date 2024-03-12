/**
 * @file pdcコマンド定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdint.h>
#include "command_table.h"
#include "command_pdc.h"

/**
 * コマンドエントリテーブル
 */
//@formatter:off
static const struct cmd_entry CommandEntries[] = {
};
//@formatter:on
/**
 * コマンドエントリ数
 */
static const int CommandEntryCount = (int) (sizeof(CommandEntries) / sizeof(struct cmd_entry));


/**
 * @brief pdcコマンドを処理する。
 * @param ac 引数の数
 * @param av 引数配列
 */
void cmd_pdc(int ac, char **av)
{
    if (ac >= 2)
    {
        const struct cmd_entry *pentry = command_table_find_cmd(CommandEntries, CommandEntryCount, av[1]);
        if (pentry != NULL)
        {
            pentry->cmd_proc (ac, av);
        }
        else
        {
            printf ("Unknown subcommand: %s\n", av[1]);
        }
    }
    else
    {
        for (uint32_t i = 0u; i < CommandEntryCount; i++)
        {
            const struct cmd_entry *pentry = &(CommandEntries[i]);
            if ((pentry->cmd != NULL) && (pentry->desc != NULL))
            {
                printf ("pdc %s - %s\n", pentry->cmd, pentry->desc);
            }
        }
    }

    return ;
}

