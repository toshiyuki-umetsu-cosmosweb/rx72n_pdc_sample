/**
 * @file ユーティリティ関数定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/**
 * @brief 文字列sをON/OFF値として解析する。
 *        許容する入力は、大文字/小文字区別なしの ["on", "off", "true", "false", 数値 ]である。
 *        数値は0以外をtrue, 0をfalseとして解析する。
 *
 * @param s 文字列
 * @param pbool ON/OFFを取得する変数のアドレス
 * @return 解析成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool parse_boolean(const char* s, bool* pbool)
{
    bool is_parse_succeed = false;

    char* p;
    int d = strtol(s, &p, 0);
    if ((p != NULL) && (*p == '\0'))
    {
        (*pbool) = (d != 0);
        is_parse_succeed = true;
    }
    else
    {
        if ((strcasecmp(s, "on") == 0) || (strcasecmp(s, "true") == 0))
        {
            (*pbool) = true;
            is_parse_succeed = true;
        }
        else if ((strcasecmp(s, "off") == 0) || (strcasecmp(s, "false") == 0))
        {
            (*pbool) = false;
            is_parse_succeed = true;
        }
        else
        {
            is_parse_succeed = false;
        }
    }

    return is_parse_succeed;
}

/**
 * @brief 文字列sを符号なし8bit整数として解析する。
 * @param s 文字列
 * @param pval 値を取得する変数のアドレス
 * @return 解析成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool parse_u8(const char* s, uint8_t* pval)
{
    bool is_parse_succeed = false;
    char* p;
    uint32_t d = strtoul(s, &p, 0);
    if ((p != NULL) && (*p == '\0'))
    {
        if (d <= UINT8_MAX)
        {
            (*pval) = (uint8_t)(d & 0xFF);
            is_parse_succeed = true;
        }
    }

    return is_parse_succeed;
}
/**
 * @brief 文字列sを符号なし16bit整数として解析する。
 * @param s 文字列
 * @param pval 値を取得する変数のアドレス
 * @return 解析成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool parse_u16(const char* s, uint16_t* pval)
{
    bool is_parse_succeed = false;
    char* p;
    uint32_t d = strtoul(s, &p, 0);
    if ((p != NULL) && (*p == '\0'))
    {
        if (d <= UINT16_MAX)
        {
            (*pval) = (uint16_t)(d & 0xFFFF);
            is_parse_succeed = true;
        }
    }

    return is_parse_succeed;
}
/**
 * @brief 文字列sを符号なし32bit整数として解析する。
 * @param s 文字列
 * @param pval 値を取得する変数のアドレス
 * @return 解析成功した場合にはtrue, 失敗した場合にはfalse.
 */
bool parse_u32(const char* s, uint32_t* pval)
{
    bool is_parse_succeed = false;
    char* p;
    uint32_t d = strtoul(s, &p, 0);
    if ((p != NULL) && (*p == '\0'))
    {
        (*pval) = d;
        is_parse_succeed = true;
    }

    return is_parse_succeed;
}
