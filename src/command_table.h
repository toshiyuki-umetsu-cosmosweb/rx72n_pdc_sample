/**
 * @file コマンドテーブル
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef COMMAND_TABLE_H_
#define COMMAND_TABLE_H_

struct cmd_entry
{
    const char *cmd; // コマンド文字列
    const char *desc; // ディスクリプション
    void (*cmd_proc)(int av, char **ac); // コマンド処理
};

const struct cmd_entry * command_table_find_cmd(const struct cmd_entry *ptable, int entry_count, const char *cmd);

#endif /* COMMAND_TABLE_H_ */
