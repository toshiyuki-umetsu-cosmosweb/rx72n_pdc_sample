/***********************************************************************
*
*  FILE        : main.c
*  DATE        : 2024-03-04
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "r_smc_entry.h"

#include "hwtick.h"
#include "usb_cdc.h"
#include "command_io.h"
#include "test_signal.h"

void main(void);

/**
 * @brief アプリケーションのエントリポイント
 */
void main(void)
{
    hwtick_init();
    usb_cdc_init();
    command_io_init();
    test_signal_init();
    i2c_init();

    while (1) {
        usb_cdc_update();
        command_io_update();

        // TODO :
    }

}
