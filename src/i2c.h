/**
 * @file I2Cインタフェース宣言
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief I2C コールバック型
 * @param status
 */
typedef void (*i2c_callback_func_t)(int status);

void i2c_init(void);

int i2c_set_bitrate(uint32_t bit_rate);
uint32_t i2c_get_bitrate(void);

int i2c_master_send_sync(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint32_t timeout_millis);
int i2c_master_receive_sync(uint8_t slave_addr, uint8_t* rx_bufp, uint16_t rx_len, uint32_t timeout_millis);
int i2c_master_send_and_receive_sync(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint8_t* rx_bufp, uint16_t rx_len, uint32_t timeout_millis);

int i2c_master_send_async(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, i2c_callback_func_t pcallback);
bool i2c_master_receive_async(uint8_t slave_addr, uint8_t* rx_bufp, uint16_t rx_len, i2c_callback_func_t pcallback);
int i2c_master_send_and_receive_async(uint8_t slave_addr, uint8_t* tx_data, uint16_t tx_len, uint8_t* rx_bufp, uint16_t rx_len, i2c_callback_func_t pcallback);
bool i2c_is_busy(void);

#endif /* I2C_H_ */
