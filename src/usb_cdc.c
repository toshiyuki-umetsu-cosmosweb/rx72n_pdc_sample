/**
 * @file USB CDCファンクション 定義
 * @author Cosmosweb Co.,Ltd. 2024
 * @note
 * 依存モジュール
 *     FIT usb_basic
 *     FIT usb_pcdc
 *     FIT byteq
 * 使用ボード
 *     Alpha Project社 AP-RX72N-0A
 *     上記以外のボードに移植して使う場合には、USB_VENDOR_ID と USB_PRODUCT_ID を変更すること。
 * 本モジュールは以下のように動作するようデザインしている。
 * ・USB 接続状態による挙動
 *     CONFIGUREDされたときに送受信キューを初期化し、DETACH時に解放する。
 *     結果的に、USBが物理的に接続されない限り、送受信キューが存在しないので
 *     読み出し/書き込みのインタフェースはエラーになる。
 *
 * ・受信処理
 *     USB-CDC 受信 -> RX_QUEUE_SIZE バイトの byteq キューで一次受けする。
 *                     読み出し処理で先頭から順に指定バイト数を読み出せる。
 *
 * ・送信処理
 *     USB-CDC 送信 -> TX_QUEUE_SIZE バイトの byteq キューを介して送信する。
 *
 * ・その他
 *     相手との接続状態は、 ControlLineState にて判定できるようにインタフェースを設けた。
 *     usb_cdc_get_DSR(), usb_cdc_get_CTS()
 *     ただし、相手のドライバが、ちゃんと SetControlLineState で設定してくれる必要がある。
 *     Windows 10のTeraTeramで試す限りでは、接続時にDSR=1, RTS=1が設定され、切断時にDSR=1, RTS=0が設定された。
 *     Windows 10上でSerialPortを使ったソフトを組んだとき、
 *     同様に接続時にDSR=1, RTS=1が設定され、切断時にDSR=1, RTS=0が設定された。
 *
 * ・SCIブリッジにしてRTS/CTSをサポートする場合、クラスノーティフィケーションをサポートする必要がある。
 *     USBの規格書か、FIT usb_pcdcのドキュメントを参照。
 */
#include <r_usb_basic_pinset.h>
#include <r_usb_basic_if.h>
#include <r_usb_pcdc_if.h>
#include <r_byteq_if.h>

#include <stddef.h>
#include <string.h>

/**
 * @brief USB Vendor ID (libusb共用ID)
 */
#define USB_VENDOR_ID (0x27dd)
/**
 * @brief Product ID (libusb共用ID)
 */
#define USB_PRODUCT_ID (0x16c0)

/**
 * @brief USB バージョン
 */
#define USB_VERSION (0x0120u)
/**
 * @brief デバイスバージョン番号
 */
#define DEVICE_RELEASE (0x0200u)
/**
 * @brief Control Pipe 最大パケットサイズ
 */
#define USB_DCPMAXP (64u)
/**
 * @brief 受信キューサイズ
 */
#define RX_QUEUE_SIZE (256)
/**
 * @brief 送信キューサイズ
 */
#define TX_QUEUE_SIZE (512)

/**
 * @brief デバイスディスクリプタ
 */
//@formatter:off
static const uint8_t s_device_descriptor[USB_DD_BLENGTH + (USB_DD_BLENGTH & 1)] = {
    USB_DD_BLENGTH,                                    //  0:bLength
    USB_DT_DEVICE,                                     //  1:bDescriptorType
    (USB_VERSION &(uint8_t)0xffu),                     //  2:bcdUSB (下位8bit)
    ((uint8_t)(USB_VERSION >> 8) & (uint8_t)0xffu),    //  3:bcdUSB (上位8bit)
    USB_IFCLS_CDCC,                                    //  4:bDeviceClass
    0,                                                 //  5:bDeviceSubClass
    0,                                                 //  6:bDeviceProtocol
    (uint8_t)USB_DCPMAXP,                              //  7:bMAXPacketSize
    (USB_VENDOR_ID &(uint8_t)0xffu),                   //  8:idVendor (下位8bit)
    ((uint8_t)(USB_VENDOR_ID >> 8) & (uint8_t)0xffu),  //  9:idVendor (上位8bit)
    ((uint16_t)USB_PRODUCT_ID &(uint8_t)0xffu),        // 10:idProduct (下位8bit)
    ((uint8_t)(USB_PRODUCT_ID >> 8) & (uint8_t)0xffu), // 11:idProduct (上位8bit)
    (DEVICE_RELEASE &(uint8_t)0xffu),                  // 12:bcdDevice (下位8bit)
    ((uint8_t)(DEVICE_RELEASE >> 8) & (uint8_t)0xffu), // 13:bcdDevice (上位8bit)
    1,                                                 // 14:iManufacturer
    2,                                                 // 15:iProduct
    3,                                                 // 16:iSerialNumber
    1u                                                 // 17:bNumConfigurations
};
//@formatter:on

/**
 * @brief Configuration Descriptor 長
 *         Configuration Descriptor 9 byte
 *         Interface Descriptor 9 byte
 *         Communication Class Functional Descriptr 5 byte
 *         Communication Class Functional Descriptr 4 byte
 *         Communication Class Functional Descriptr 5 byte
 *         Communication Class Functional Descriptr 5 byte
 *         Endpoint Descriptor 7 byte
 *         Interface Descriptor 9 byte
 *         Endpoint Descriptor 7 byte
 *         Endpoint Descriptor 7 byte
 *         合計 67 byte
 */
#define USB_PCDC_CD1_LEN (67)
/**
 * @brief Configuration Descriptor Descriptor Type CS_INTERFACE
 * @note See USB Communication Device Class Specification.
 */
#define USB_CD_DT_CS_INTERFACE (0x24)

/**
 * @brief USB コンフィグレーションディスクリプタ
 *        USB CDC規格書 PSTNデバイスのACMを参照。
 * @note このディスクリプタは内蔵RAM上に配置しないと上手く動かないようだ。
 */
//@formatter:off
static uint8_t s_configuration_descriptor[USB_PCDC_CD1_LEN + (USB_PCDC_CD1_LEN % 2)] = {
    USB_CD_BLENGTH,                 //  0:bLength */
    USB_SOFT_CHANGE,                //  1:bDescriptorType
    USB_PCDC_CD1_LEN & 0xFF,        //  2:wTotalLength(下位8bit)
    (USB_PCDC_CD1_LEN >> 8) & 0xFF, //  3:wTotalLength(上位8bit)
    2,                              //  4:bNumInterfaces
    1,                              //  5:bConfigurationValue
    0,                              //  6:iConfiguration
    USB_CF_RESERVED | USB_CF_SELFP, //  7:bmAttributes
    (10 / 2),                       //  8:MAXPower (2mA unit)

    /* Interface Descriptor */
    USB_ID_BLENGTH,   //  0:bLength
    USB_DT_INTERFACE, //  1:bDescriptor
    0,                //  2:bInterfaceNumber
    0,                //  3:bAlternateSetting
    1,                //  4:bNumEndpoints
    USB_IFCLS_CDCC,   //  5:bInterfaceClass = 2 : CDC-Control Class
    0x02,             //  6:bInterfaceSubClass = 2 : ACM(=Abstract Control Model)
    1,                //  7:bInterfaceProtocol
    0,                //  8:iInterface

    /* Communications Class Functional Descriptor #1
     *   SubType = 0x00 Header Functional.
     * */
    5,                      //  0:bLength
    USB_CD_DT_CS_INTERFACE, //  1:bDescriptorType
    0x00,                   //  2:bDescriptorSubtype ヘッダー
    0x10,                   //  3:bcdCDC (下位8bit)  CDC Version 1.2
    0x01,                   //  4:bcdCDC (上位8bit)  CDC Version 1.2

    /* Communications Class Functional Descriptor #2
     *   SubType = 0x02 Abstract Control Management
     */
    4,                      //  0:bLength
    USB_CD_DT_CS_INTERFACE, //  1:bDescriptorType
    0x02,                   //  2:bDescriptorSubtype = Abstract Control Management
    2,                      //  3:bmCapabilities
                            //       D1 デバイスは下記リクエストの組み合わせをサポートする。
                            //          Set_Line_Coding / Set_Control_Line_State
                            //          / Get_Line_Coding / Serial_State notification

    /* Communications Class Functional Descriptors #3
     *   SubType = 0x06 Union Functional
     */
    5,                      //  0:bLength
    USB_CD_DT_CS_INTERFACE, //  1:bDescriptorType
    0x06,                   //  2:bDescriptorSubtype = Union Functional
    0,                      //  3:bControlInterface = 0 制御用インタフェース番号
    1,                      //  4:bSlaveInterface0 = 1 データ用インタフェース番号

    /* Communications Class Functional Descriptor #4
     *   SubType = 0x01 Call Management Functional
     */
    5,                      //  0:bLength
    USB_CD_DT_CS_INTERFACE, //  1:bDescriptorType
    0x01,                   //  2:bDescriptorSubtype = Call Management Functional
    3,                      //  3:bmCapabilities = 3
                            //      D0:デバイス自身で呼び出し管理できる
                            //      D1:データインタフェースにて多重化されたコマンドを処理できる
    1,                      //  4:bDataInterface = 1 データ用インタフェース番号
                            //                      (Union Functionalディスクリプタと同じ)

    /* Endpoint Descriptor 0 */
    USB_ED_BLENGTH,      //  0:bLength
    USB_DT_ENDPOINT,     //  1:bDescriptorType
    USB_EP_IN | USB_EP3, //  2:bEndpointAddress
    USB_EP_INT,          //  3:bmAttribute
    16,                  //  4:wMAXPacketSize (下位8bit)
    0,                   //  5:wMAXPacketSize (上位8bit)
    0x10,                //  6:bInterval

    /* Interface Descriptor */
    USB_ID_BLENGTH,   //  0:bLength
    USB_DT_INTERFACE, //  1:bDescriptor
    1,                //  2:bInterfaceNumber
    0,                //  3:bAlternateSetting
    2,                //  4:bNumEndpoints
    USB_IFCLS_CDCD,   //  5:bInterfaceClass = 0x0A : CDC-Data Class
    0,                //  6:bInterfaceSubClass
    0,                //  7:bInterfaceProtocol
    0,                //  8:iInterface

    /* Endpoint Descriptor 0 */
    USB_ED_BLENGTH,      //  0:bLength
    USB_DT_ENDPOINT,     //  1:bDescriptorType
    USB_EP_IN | USB_EP1, //  2:bEndpointAddress
    USB_EP_BULK,         //  3:bmAttribute
    64,                  //  4:wMAXPacketSize (下位8bit)
    0,                   //  5:wMAXPacketSize (上位8bit)
    0,                   //  6:bInterval

    /* Endpoint Descriptor 1 */
    USB_ED_BLENGTH,       //  0:bLength
    USB_DT_ENDPOINT,      //  1:bDescriptorType
    USB_EP_OUT | USB_EP2, //  2:bEndpointAddress
    USB_EP_BULK,          //  3:bmAttribute
    64,                   //  4:wMAXPacketSize (下位8bit)
    0,                    //  5:wMAXPacketSize (上位8bit)
    0,                    //  6:bInterval
};
//@formatter:on

/**
 * @brief STRING ディスクリプタ#0
 * @note サポートするLANG IDを返す。
 */
//@formatter:off
static const uint8_t s_string_descriptor0[4] = {
    4,             //  0:bLength
    USB_DT_STRING, //  1:bDescriptorType
    0x09, 0x04     //  2:wLANGID[0] = 0x0409 (English)
};
//@formatter:on

/**
 * @brief STRING ディスクリプタ#1
 */
//@formatter:off
static const uint8_t s_string_descriptor1[26] = {
    26,            //  0:bLength
    USB_DT_STRING, //  1:bDescriptorType
    (uint8_t)('M'),
    0x00,
    (uint8_t)('a'),
    0x00,
    (uint8_t)('n'),
    0x00,
    (uint8_t)('u'),
    0x00,
    (uint8_t)('f'),
    0x00,
    (uint8_t)('a'),
    0x00,
    (uint8_t)('c'),
    0x00,
    (uint8_t)('t'),
    0x00,
    (uint8_t)('u'),
    0x00,
    (uint8_t)('r'),
    0x00,
    (uint8_t)('e'),
    0x00,
    (uint8_t)('r'),
    0x00,
};
//@formatter:on

/**
 * @brief STRING ディスクリプタ#2
 */
static const uint8_t s_string_descriptor2[42] = {26,            /*  0:bLength */
                                                 USB_DT_STRING, /*  1:bDescriptorType */
                                                 'R',
                                                 0x00,
                                                 'X',
                                                 0x00,
                                                 '7',
                                                 0x00,
                                                 '2',
                                                 0x00,
                                                 'N',
                                                 0x00,
                                                 ' ',
                                                 0x00,
                                                 'P',
                                                 0x00,
                                                 'D',
                                                 0x00,
                                                 'C',
                                                 0x00,
                                                 ' ',
                                                 0x00,
                                                 'T',
                                                 0x00,
                                                 'e',
                                                 0x00,
                                                 's',
                                                 0x00,
                                                 't',
                                                 0x00,
                                                 ' ',
                                                 0x00,
                                                 'B',
                                                 0x00,
                                                 'o',
                                                 0x00,
                                                 'a',
                                                 0x00,
                                                 'r',
                                                 0x00,
                                                 'd',
                                                 0x00};
/**
 * @brief STRING ディスクリプタ#3
 */
static const uint8_t s_string_descriptor3[32] = {32,            /*  0:bLength */
                                                 USB_DT_STRING, /*  1:bDescriptorType */
                                                 'c',
                                                 0x00,
                                                 'o',
                                                 0x00,
                                                 's',
                                                 0x00,
                                                 'm',
                                                 0x00,
                                                 'o',
                                                 0x00,
                                                 's',
                                                 0x00,
                                                 'w',
                                                 0x00,
                                                 'e',
                                                 0x00,
                                                 'b',
                                                 0x00,
                                                 '.',
                                                 0x00,
                                                 'c',
                                                 0x00,
                                                 'o',
                                                 0x00,
                                                 '.',
                                                 0x00,
                                                 'j',
                                                 0x00,
                                                 'p',
                                                 0x00};
/**
 * @brief STRING ディスクリプタ
 */
//@formatter:off
static const uint8_t* s_string_descriptors[] = {
    s_string_descriptor0, // LANG ID
    s_string_descriptor1, // iManufacturer
    s_string_descriptor2, // iProduct
    s_string_descriptor3  // iSerialNumber
};
//@formatter:on

/**
 * @brief ディスクリプタ定義
 * @note このディスクリプタ定義は、RAM上に置かないと正しく動かないようだ。
 */
static usb_descriptor_t s_descriptor = {.p_device = (uint8_t*)(s_device_descriptor),          // Device descriptor.
                                        .p_config_f = (uint8_t*)(s_configuration_descriptor), // Configuration Descriptor for Full speed
                                        .p_config_h = USB_NULL,                               // Configuration Descriptor for Hi speed
                                        .p_qualifier = USB_NULL,                              // Qualifier Descriptor なし
                                        .p_string = (uint8_t**)(s_string_descriptors),        // String descriptor table.
                                        .num_string = sizeof(s_string_descriptors) / sizeof(uint8_t*)};
/**
 * @brief USBコントローラ
 */
static usb_ctrl_t s_usb_ctrl;
/**
 * @brief USB設定
 */
static usb_cfg_t s_usb_cfg;
/**
 * @brief USB CDC Line Coding 設定
 * @note USB CDC PSTN Subclass specification 'Line Coding Structure' 参照。
 *       または
 *       Renesas社のアプリケーションノート
 *       USB Peripheral Communication Device Driver Firmware Integration technology
 *       のクラスリクエストを参照。( smc_gen/r_usb_pcdc/docの下にコピーされる )
 *       RenesasのPCDCヘッダでは具体的な値が定義されてない。残念！
 */
static usb_pcdc_linecoding_t s_cdc_line_coding;
/**
 * @brief フロー制御信号のSetControllineStateデータ
 * @note FITドライバではRTS/DTRはサポートされていないので、
 *       RTS, DTRの状態を見て何かしらの操作をするなら、
 *       APIを使う側でやる必要がある。
 */
static usb_pcdc_ctrllinestate_t s_cdc_line_state;

/**
 * @brief 受信要求を出したかどうか
 */
static bool s_is_rx_requirled;
/**
 * @brief 送信中かどうか
 */
static bool s_is_tx_transferring;

/**
 * @brief 制御データバッファ
 */
static uint8_t s_ctrl_buf[64];
/**
 * @brief 送信データバッファ
 */
static uint8_t s_tx_buf[64];
/**
 * @brief 受信データバッファ
 */
static uint8_t s_rx_buf[64];
/**
 * @brief 受信データ長
 */
static uint32_t s_rx_length;

/**
 * @brief 受信キューバッファ
 */
static uint8_t s_rx_queue_buf[TX_QUEUE_SIZE];
/**
 * @brief 受信キューハンドル
 */
static byteq_hdl_t s_rx_queue;
/**
 * @brief 送信キューバッファ
 */
static uint8_t s_tx_queue_buf[RX_QUEUE_SIZE];
/**
 * @brief 送信キューハンドル
 */
static byteq_hdl_t s_tx_queue;

static void proc_usb_event(int event);
static void proc_class_request(void);
static void open_queues(void);
static void close_queues(void);
static void send_ACK(void);
static void proc_tx(void);
static void proc_rx(void);
static void request_receive_if_idle(void);

/**
 * @brief CDC初期化する
 */
void usb_cdc_init(void)
{
    s_is_tx_transferring = false;
    s_is_rx_requirled = false;
    s_rx_length = 0u;
    s_rx_queue = NULL;
    s_tx_queue = NULL;

    R_USB_PinSet_USB0_PERI();

    // Note : USB-UARTブリッジでもやらない限り、ここの設定に意味はない。
    memset(&s_cdc_line_coding, 0, sizeof(s_cdc_line_coding));
    s_cdc_line_coding.dw_dte_rate = 115200; // dwDTERate = 115200 : 115200bps.
    s_cdc_line_coding.b_char_format = 0u;   // bCharFormat = 0 : 1 stop bit
    s_cdc_line_coding.b_parity_type = 0u;   // bParityType = 0 : Parity None
    s_cdc_line_coding.b_data_bits = 8;      // bDataBits = 8 : 8bit.

    memset(&s_cdc_line_state, 0, sizeof(s_cdc_line_state));
    s_cdc_line_state.BIT.bdtr = 0;
    s_cdc_line_state.BIT.brts = 0;

    memset(&s_usb_ctrl, 0, sizeof(s_usb_ctrl));
    s_usb_ctrl.module = USB_IP0;
    s_usb_ctrl.type = USB_PCDC;

    memset(&s_usb_cfg, 0, sizeof(s_usb_cfg));
    s_usb_cfg.usb_mode = USB_PERI;
    s_usb_cfg.usb_speed = USB_FS; // 12Mbps
    s_usb_cfg.p_usb_reg = &s_descriptor;

    usb_err_t ret = R_USB_Open(&s_usb_ctrl, &s_usb_cfg);
    if (ret != USB_SUCCESS)
    {
        // TODO : Error handling.
    }
    return;
}

/**
 * @brief USB CDC更新処理
 */
void usb_cdc_update(void)
{
    usb_status_t status = R_USB_GetEvent(&s_usb_ctrl);
    if (status != USB_STS_NONE)
    {
        proc_usb_event(status);
    }

    proc_tx();
    proc_rx();

    return;
}

/**
 * @brief USBイベントを処理する。
 * @param event イベント
 */
static void proc_usb_event(int event)
{
    switch (event)
    {
    case USB_STS_CONFIGURED: // Config完了
    {
        s_is_tx_transferring = false;
        s_is_rx_requirled = false;
        s_usb_ctrl.type = USB_PCDC;
        open_queues();
        R_USB_Read(&s_usb_ctrl, s_ctrl_buf, sizeof(s_ctrl_buf));
        break;
    }
    case USB_STS_WRITE_COMPLETE: {
        if (s_usb_ctrl.type == USB_PCDC)
        {
            s_is_tx_transferring = false;
        }
        else
        {
            // do nothing.
        }
        break;
    }
    case USB_STS_READ_COMPLETE: {
        if (s_usb_ctrl.type == USB_PCDC)
        {
            s_rx_length = s_usb_ctrl.size;
            s_is_rx_requirled = false;
        }
        break;
    }
    case USB_STS_REQUEST: // Class request.
    {
        proc_class_request();
        break;
    }
    case USB_STS_REQUEST_COMPLETE: {
        // Request done.
        // uint16_t req_code = s_usb_ctrl.setup.type & USB_BREQUEST;
        break;
    }
    case USB_STS_SUSPEND: {
        break;
    }
    case USB_STS_DETACH: {
        s_cdc_line_state.BIT.bdtr = 0;
        s_cdc_line_state.BIT.brts = 0;
        close_queues();
        break;
    }
    default: {
        // do nothing.
        break;
    }
    }

    return;
}

/**
 * @brief クラスリクエストを処理する。
 */
static void proc_class_request(void)
{
    uint16_t req_code = s_usb_ctrl.setup.type & USB_BREQUEST;
    switch (req_code)
    {
    case USB_PCDC_SET_LINE_CODING: {
        s_usb_ctrl.type = USB_REQUEST;
        s_usb_ctrl.module = USB_IP0;
        uint16_t io_len = (uint16_t)((s_usb_ctrl.setup.length < sizeof(s_cdc_line_coding)) ? s_usb_ctrl.setup.length : sizeof(s_cdc_line_coding));
        R_USB_Read(&s_usb_ctrl, (uint8_t*)(&s_cdc_line_coding), io_len);
        break;
    }
    case USB_PCDC_GET_LINE_CODING: {
        s_usb_ctrl.type = USB_REQUEST;
        s_usb_ctrl.module = USB_IP0;
        uint16_t io_len = (uint16_t)((s_usb_ctrl.setup.length < sizeof(s_cdc_line_coding)) ? s_usb_ctrl.setup.length : sizeof(s_cdc_line_coding));
        R_USB_Write(&s_usb_ctrl, (uint8_t*)(&s_cdc_line_coding), io_len);
        break;
    }
    case USB_PCDC_SET_CONTROL_LINE_STATE: {
        // USB CDC PSTN 仕様によると、SetControlLineState では wValue に
        // 制御ラインデータが渡されることになっている。
        s_cdc_line_state.WORD = s_usb_ctrl.setup.value;
        send_ACK();
        break;
    }
    default: {
        send_ACK();
        break;
    }
    }

    return;
}

/**
 * @brief 送受信のキューを開く
 */
static void open_queues(void)
{
    if (s_rx_queue == NULL)
    {
        if (R_BYTEQ_Open(s_rx_queue_buf, sizeof(s_rx_queue_buf), &s_rx_queue) != BYTEQ_SUCCESS)
        {
            // TODO : Error handling.
        }
    }
    if (s_tx_queue == NULL)
    {
        if (R_BYTEQ_Open(s_tx_queue_buf, sizeof(s_tx_queue_buf), &s_tx_queue) != BYTEQ_SUCCESS)
        {
            // TODO : Error handling.
        }
    }

    return;
}

/**
 * @brief 送受信のキューを閉じる
 */
static void close_queues(void)
{
    if (s_rx_queue != NULL)
    {
        R_BYTEQ_Close(s_rx_queue);
        s_rx_queue = NULL;
    }

    if (s_tx_queue != NULL)
    {
        R_BYTEQ_Close(s_tx_queue);
        s_tx_queue = NULL;
    }

    return;
}

/**
 * @brief USB ACKを送信する。
 */
static void send_ACK(void)
{
    s_usb_ctrl.type = USB_REQUEST;
    s_usb_ctrl.status = USB_ACK;
    R_USB_Write(&s_usb_ctrl, (uint8_t*)((uintptr_t)(USB_NULL)), 0);

    return;
}

/**
 * @brief 送信データを処理する。
 */
static void proc_tx(void)
{
    uint16_t tx_data_len = 0;
    if (R_BYTEQ_Used(s_tx_queue, &tx_data_len) != BYTEQ_SUCCESS)
    {
        return;
    }
    if (tx_data_len == 0) // 送信データなし？
    {
        return;
    }

    if (s_is_tx_transferring) // 送信中？
    {
        return;
    }

    uint8_t* wp = s_tx_buf;
    uint16_t tx_count = 0;
    while ((tx_data_len > 0) && (tx_count < sizeof(s_tx_buf)))
    {
        if (R_BYTEQ_Get(s_tx_queue, wp) != BYTEQ_SUCCESS)
        {
            break;
        }
        wp++;
        tx_data_len--;
        tx_count++;
    }

    s_usb_ctrl.type = USB_PCDC;
    s_usb_ctrl.module = USB_IP0;
    if (R_USB_Write(&s_usb_ctrl, s_tx_buf, tx_count) == USB_SUCCESS)
    {
        s_is_tx_transferring = true;
    }

    return;
}

/**
 * @brief 読み出しデータを処理する。
 *        USB受信バッファから受信キューに入れる。
 *        受信要求を出していない場合には、新しく受信要求を出す。
 */
static void proc_rx(void)
{
    if (s_rx_length == 0) // 受信データなし？
    {
        request_receive_if_idle();
        return;
    }

    uint16_t blank_count;
    if (R_BYTEQ_Unused(s_rx_queue, &blank_count) != BYTEQ_SUCCESS)
    {
        return;
    }

    if (blank_count < s_rx_length) // 受信データを格納するだけの空きがない？
    {
        return;
    }

    uint8_t* rp = s_rx_buf;
    while (s_rx_length > 0)
    {
        if (R_BYTEQ_Put(s_rx_queue, *rp) != BYTEQ_SUCCESS)
        {
            break;
        }
        rp++;
        s_rx_length--;
    }

    request_receive_if_idle();

    return;
}

/**
 * @brief 読み出し処理中でなければ、読み出しを要求する。
 */
static void request_receive_if_idle(void)
{
    if (s_is_rx_requirled) // 読み出し要求済み？
    {
        return;
    }
    if (s_rx_length != 0) // 受信データが残ってる？
    {
        return;
    }

    // Request next receive.
    s_usb_ctrl.type = USB_PCDC;
    s_usb_ctrl.module = USB_IP0;
    if (R_USB_Read(&s_usb_ctrl, s_rx_buf, sizeof(s_rx_buf)) == USB_SUCCESS)
    {
        s_is_rx_requirled = true;
    }

    return;
}

/**
 * @brief USB CDC の DSR(ホスト側でのDTR, 端末準備OK) 状態を得る。
 * @note USB CDC の SetControlLineState により DTR が操作される相手だけ正常に判定できる。
 * @return 相手の準備ができている場合にはtrue, それ以外はfalse.
 */
bool usb_cdc_get_DSR(void)
{
    return (s_cdc_line_state.BIT.bdtr != 0);
}

/**
 * @brief USB CDC の CTS (ホスト側でのRTS、送信許可) 状態を得る。
 * @note USB CDC の SetControlLineState により CTS が操作される相手だけ正常に判定できる。
 * @return 送信可能な場合にはtrue, それ以外はfalse.
 */
bool usb_cdc_get_CTS(void)
{
    return (s_cdc_line_state.BIT.brts != 0);
}

/**
 * @brief 読み出しする。
 * @param bufp 読み出したデータを格納するバッファ
 * @param bufsize バッファサイズ
 * @return 成功した場合には読み出したバイト数が返る。
 *         失敗した場合には-1が返る。
 */
int usb_cdc_read(void* bufp, uint16_t bufsize)
{
    if ((bufp == NULL) || (bufsize == 0))
    {
        return -1;
    }

    uint16_t rx_count;
    if (R_BYTEQ_Used(s_rx_queue, &rx_count) != BYTEQ_SUCCESS)
    {
        return -1;
    }
    if (rx_count == 0)
    {
        return 0;
    }

    uint16_t io_len = (rx_count < bufsize) ? rx_count : bufsize;
    int retval = 0;
    uint8_t* wp = (uint8_t*)(bufp);
    while (io_len > 0)
    {
        if (R_BYTEQ_Get(s_rx_queue, wp) != BYTEQ_SUCCESS)
        {
            break;
        }
        wp++;
        retval++;
        io_len--;
    }

    return retval;
}

/**
 * @brief 送信する
 * @param data 送信データのアドレス
 * @param length 送信データ長
 * @return 成句した場合、バッファに書き込んだバイト数が返る。
 *         失敗した場合、-1を返す。
 */
int usb_cdc_write(const void* data, uint16_t length)
{
    if (data == NULL)
    {
        return -1;
    }
    if (length == 0)
    {
        return 0;
    }

    uint16_t blank_count;
    if (R_BYTEQ_Unused(s_tx_queue, &blank_count) != BYTEQ_SUCCESS)
    {
        return -1;
    }

    int retval = 0;
    uint16_t io_len = (blank_count < length) ? blank_count : length;
    const uint8_t* rp = (const uint8_t*)(data);
    while (io_len > 0)
    {
        if (R_BYTEQ_Put(s_tx_queue, *rp) != BYTEQ_SUCCESS)
        {
            break;
        }

        rp++;
        retval++;
        io_len--;
    }

    return retval;
}
