#ifndef __AT_H__
#define __AT_H__
#include "board_basic.h"


#define AT_BLE_MASK     0x01
#define AT_UART_MARK    0x02
#define AT_USBD_MASK	0x04

extern rui_cfg_t g_rui_cfg_t;

void uart_log_printf(const char *fmt, ...);


#endif

