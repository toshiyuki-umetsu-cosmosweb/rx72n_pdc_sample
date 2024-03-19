/**
 * @file i2cコマンドインタフェース定義
 * @author Cosmosweb Co.,Ltd. 2024
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"
#include "i2c.h"
#include "command_i2c.h"

#define I2C_MAX_IOLEN (16)

/**
 * @brief I2C送信バッファ
 */
static uint8_t s_i2c_tx_buf[I2C_MAX_IOLEN];

/**
 * @brief I2C 受信バッファ
 */
static uint8_t s_i2c_rx_buf[I2C_MAX_IOLEN];

static void cmd_i2c_bit_rate(int ac, char** av);
static void cmd_i2c_process(int ac, char** av);

/**
 * @brief i2cコマンドを処理する
 * @param ac 引数の数
 * @param av 引数配列
 */
void cmd_i2c(int ac, char** av)
{
    if ((ac >= 2) && (strcmp(av[1], "bit-rate") == 0))
    {
        cmd_i2c_bit_rate(ac, av);
    }
    else if (ac >= 2)
    {
        cmd_i2c_process(ac, av);
    }
    else
    {
        printf("i2c bit-rate [rate#] - Set/get bit-rate.\n");
        printf("i2c slave_addr# [ send tx0# [ tx1# [ ... ] ] ] [ recv rx_len# ] - Do transaction.\n");
    }
    return;
}

/**
 * @brief i2c bit-rate コマンドを処理する。
 *        i2c bit-rate [rate#]
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_i2c_bit_rate(int ac, char** av)
{
    if (ac >= 3)
    {
        int32_t bit_rate;
        char* p;

        bit_rate = strtol(av[2], &p, 0);
        if ((p != NULL) && (*p != '\0'))
        {
            if (((*p) == 'M') || ((*p) == 'm')) // Mbps, mbps
            {
                bit_rate *= 1E6;
            }
            else if (((*p) == 'K') || ((*p) == 'k')) // Kbps, kbps
            {
                bit_rate *= 1E3;
            }
            else
            {
                printf("Invalid bit rate. %s\n", av[2]);
                return;
            }
        }
        if (bit_rate < 0)
        {
            printf("Invalid bit rate. %s\n", av[2]);
            return;
        }

        int s = i2c_set_bitrate((uint32_t)(bit_rate));
        if (s != 0)
        {
            printf("Could not set bit-rate. (%d)\n", s);
            return;
        }

        printf("%u\n", i2c_get_bitrate());
    }
    else
    {
        printf("%u\n", i2c_get_bitrate());
    }

    return;
}

/**
 * @brief i2c トランザクション処理をする。
 * @param ac 引数の数
 * @param av 引数配列
 */
static void cmd_i2c_process(int ac, char** av)
{
    uint8_t slave_addr = 0;
    uint8_t tx_len = 0;
    uint8_t rx_len = 0;

    if (!parse_u8(av[1], &slave_addr) || (slave_addr >= 0x80))
    {
        printf("Invalid slave address. : %s\n", av[1]);
        return;
    }

    int i = 2;
    if ((i < ac) && (strcasecmp(av[i], "send") == 0))
    {

        i++;
        while (i < ac)
        {
            uint8_t d;
            if (parse_u8(av[i], &d))
            {
                s_i2c_tx_buf[tx_len] = d;
                tx_len++;
                i++;
                if (tx_len >= I2C_MAX_IOLEN)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    if ((i < ac) && (strcasecmp(av[i], "recv") == 0))
    {
        i++;
        if (i >= ac)
        {
            printf("Receive count not specified.\n");
            return;
        }

        if (!parse_u8(av[i], &rx_len) || (rx_len > I2C_MAX_IOLEN))
        {
            printf("Invalid rx count. : %s\n", av[i]);
            return;
        }
        i++;
    }
    if (i != ac)
    {
        printf("usage:\n");
        printf("  i2c slave_addr# [ send tx0# [ tx1# [ ... ] ] ] [ recv rx_len# ]\n");
        return;
    }

    if (tx_len > 0)
    {
        if (rx_len > 0)
        {
            int s = i2c_master_send_and_receive_sync(slave_addr, s_i2c_tx_buf, tx_len, s_i2c_rx_buf, rx_len, 1000);
            if (s != 0)
            {
                printf("transaction failure. (%d)\n", s);
                return;
            }
            for (uint8_t i = 0; i < rx_len; i++)
            {
                printf("%02x ", s_i2c_rx_buf[i]);
            }
            printf("\n");
        }
        else
        {
            int s = i2c_master_send_sync(slave_addr, s_i2c_tx_buf, tx_len, 1000);
            if (s != 0)
            {
                printf("transaction failure. (%d)\n", s);
                return;
            }
            printf("transmit succeed.\n");
        }
    }
    else
    {
        if (rx_len > 0)
        {
            int s = i2c_master_receive_sync(slave_addr, s_i2c_rx_buf, rx_len, 1000);
            if (s != 0)
            {
                printf("transaction failure. (%d)\n", s);
                return;
            }

            for (uint8_t i = 0; i < rx_len; i++)
            {
                printf("%02x ", s_i2c_rx_buf[i]);
            }
            printf("\n");
        }
        else
        {
            printf("no transaction.\n");
        }
    }
    return;
}
