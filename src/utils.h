/**
 * @file ユーティリティ関数宣言
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>
#include <stdint.h>

bool parse_boolean(const char *s, bool *pbool);
bool parse_u8(const char *s, uint8_t *pval);
bool parse_u16(const char *s, uint16_t *pval);
bool parse_u32(const char *s, uint32_t *pval);

#endif /* UTILS_H_ */
