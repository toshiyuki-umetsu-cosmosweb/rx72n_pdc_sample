/**
 * @file コマンドテーブル
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <string.h>
#include "command_table.h"

/**
 * @brief ptable からcmdに一致するエントリを検索して返す。
 *
 * @param ptable コマンドエントリテーブル
 * @param entry_count コマンドエントリテーブルのコマンド数
 * @param cmd コマンド
 * @return エントリのポインタ。見つからない場合にはNULL。
 */
const struct cmd_entry * command_table_find_cmd(const struct cmd_entry *ptable, int entry_count, const char *cmd) {
    if ((ptable == NULL) || (cmd == NULL)) {
        return NULL;
    }

    for (int i = 0u; i < entry_count; i++)
    {
        if (strcmp (ptable[i].cmd, cmd) == 0L)
        {
            return &(ptable[i]);
        }
    }

    return NULL;
}

