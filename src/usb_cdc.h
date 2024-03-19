/**
 * @file USB ファンクション CDCインタフェース
 * @author Cosmosweb Co.,Ltd. 2024
 */

#ifndef USB_CDC_H_
#define USB_CDC_H_

#include <stdbool.h>

void usb_cdc_init(void);
void usb_cdc_update(void);

bool usb_cdc_get_DSR(void);
bool usb_cdc_get_CTS(void);

int usb_cdc_read(void* bufp, uint16_t bufsize);
int usb_cdc_write(const void* data, uint16_t length);

#endif /* USB_CDC_H_ */
