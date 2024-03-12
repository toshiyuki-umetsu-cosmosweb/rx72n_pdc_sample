/**
 * @file ハードウェアTICKのインタフェース
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef HWTICK_H_
#define HWTICK_H_

#include <stdint.h>

void hwtick_init(void);
uint32_t hwtick_get(void);

#endif /* HWTICK_H_ */
