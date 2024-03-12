/**
 * @file テスト信号インタフェース
 *       GLCDCを使って640x480@30fps YUYV 信号を出すようなモジュール。
 *       RAMが足りないので、単色データを出すだけとする。
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef TEST_SIGNAL_H_
#define TEST_SIGNAL_H_

#include <stdbool.h>
#include <stdint.h>

void test_signal_init(void);
bool test_signal_set_output(bool is_output);
bool test_signal_is_output(void);

bool test_signal_set_data(uint8_t data);
uint8_t test_signal_get_data(void);

#endif /* TEST_SIGNAL_H_ */
