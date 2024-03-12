/**
 * @file ハードウェアTICKカウンタのインタフェース定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include "hwtick.h"

#include <r_smc_entry.h>

/**
 * @brief ハードウェアTICKカウンタを初期化する。
 */
void hwtick_init(void)
{
    R_Config_ELC_Start();
    R_Config_CMTW0_Start();
    R_Config_TPU0_Start();

    return ;
}

/**
 * @brief ハードウェアTICKカウンタの値を得る。
 *        差分をとると、1msec単位での経過時間を取得できる。
 * @return ハードウェアTICKカウンタの値。
 */
uint32_t hwtick_get(void)
{
    return CMTW0.CMWCNT;
}


