#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "inner.h"
#include "at.h"

/***************below is the inner rui_config_struct**********************
typedef struct {
        uint8_t sof;
        uint8_t join_mode;
        uint8_t work_mode;
        uint8_t class;
        uint8_t confirm;
        uint8_t region;
        uint8_t dev_eui[8];
        uint8_t app_eui[8];
        uint8_t app_key[16];
        uint32_t dev_addr;
        uint8_t nwkskey[16];
        uint8_t appskey[16];
} lora_cfg_t;

typedef struct {
        uint8_t server_ip[20];
        uint8_t server_port[20];
        uint8_t operator_long_data[20];
        uint8_t operator_short_data[20];
        uint8_t operator_apn_data[20];
        uint8_t operator_net_data[20];
        uint8_t hologram_card_num[20];
} cellular_cfg_t;


typedef struct{
    uint8_t work_mode;  // 0:ble peripheral  1:ble central  2:ble observer
    uint8_t long_range_enable;
    uint16_t reserve;
}ble_central_cfg_t;


typedef struct {
        uint8_t sof;
        uint8_t sleep_enable;
        uint32_t sleep_period;
        lora_cfg_t g_lora_cfg_t;
        S_LORAP2P_PARAM g_lora_p2p_cfg_t;
        cellular_cfg_t g_cellular_cfg_t;
        ble_central_cfg_t g_ble_cfg_t;
        uint8_t user_data[128];
} rui_cfg_t;
******************************************************************************/

#ifndef __RUI_LOG_PRINT_MERGE
#define __RUI_LOG_PRINT_MERGE
#define RUI_LOG_PRINTF_MERGE(fmt, args...);  {uart_log_printf(fmt, ##args);RUI_LOG_PRINTF(fmt, ##args);}
#endif

rui_cfg_t g_rui_cfg_t;

extern RUI_LORA_STATUS_T lora_status;

int at_flag = 0;
int power_flag = 0;

void StrToHex(char *pbDest, char *pbSrc, int nLen)
{
    char h1,h2;
    char s1,s2;
    int i;

    for (i=0; i<nLen; i++)
    {
        h1 = pbSrc[2*i];
        h2 = pbSrc[2*i+1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
            s1 -= 7;

        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;

        pbDest[i] = s1*16 + s2;
    }

}

void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
    char ddl,ddh;
    int i;
    
    for (i=0; i<nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
    }
    
    pbDest[nLen*2] = '\0';
}

void at_response_string(uint8_t *msg)
{
    rui_uart_send(RUI_UART1, msg, strlen(msg));
    RUI_LOG_PRINTF("%s", msg);
}

bool at_param_digital_verify(uint8_t *p_data, uint16_t len)
{
    uint32_t i;

    /* Digital check */
    for(i=0; i<len; i++)    // Traversal parameters
    {
        if(!isdigit(p_data[i]))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return false ;
        } 
    }

    /* Length verification */
    if((p_data[i] != '\0') && (p_data[i] != ':'))   // Check whether the parameter ends after the specified length
    {
        rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
        return false;
    }
    else
        return true;
}

bool at_check_hex_param_length(uint8_t *p_data, uint16_t len)
{
    uint16_t param_len = strlen(p_data);
    uint16_t i;

    for (i=0; i<len; i++)   // hex string check
    {
        if (!isxdigit(p_data[i]))
            return false;
    }

    if (param_len < len){
        return false;
    }
    else{
        if ((p_data[len] != '\0') && (p_data[len] != '\r'))
        {
            return false;
        }
    }

    return true;

}

/*
    ×Ö·û´®³¤¶ÈÅÐ¶Ï
    ×Ö·û´®hex¸ñÊ½
    ×Ö·û´®×ªhex
*/ 
bool at_param_hex2string_verify(uint8_t *p_str, uint32_t str_len, uint8_t *p_hex)
{
    uint32_t len = strlen(p_str);
    uint16_t i;

    /* Length verification */
    if ((len != str_len) || (str_len%2 != 0))
    {
        rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
        return false;
    }
    if ((p_str[len] != '\0') && (p_str[len] != '\r'))
    {
        rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
        return false;
    }

    /* Hex check */
    for (i=0; i<len; i++)   // hex string check
    {
        if (!isxdigit(p_str[i]))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return false;
        }
    }
    
    /* String to hex */
    StrToHex(p_hex, p_str, str_len);

    return true;
}

void at_param_location(uint8_t *p_cmd, uint8_t *str, uint8_t **p_param)
{
    *p_param = strstr(p_cmd, str);
    *p_param += strlen(str);
    //uart_log_printf("%s\r\n", *p_param);
}

void uart_log_printf(const char *fmt, ...)
{
    char print_buf[512];
    va_list aptr;
    int ret;
 
    va_start (aptr, fmt);
    ret = vsprintf (print_buf, fmt, aptr);
    va_end (aptr);
 
    rui_uart_send(RUI_UART1, print_buf, strlen(print_buf));
}

void at_response_help(void)
{
    // at+help
    uart_log_printf("OK Device AT commands:\r\n");
    uart_log_printf("  at+version\r\n");
    uart_log_printf("  at+help\r\n");
    uart_log_printf("  at+set_config=device:restart\r\n");
    uart_log_printf("  at+set_config=device:sleep:X\r\n");
    uart_log_printf("  at+get_config=device:status\r\n");
    uart_log_printf("  at+set_config=device:uart:X:Y\r\n");
    uart_log_printf("  at+set_config=device:uart_mode:X:Y\r\n");
    uart_log_printf("\r\n");

    uart_log_printf("LoRaWAM AT commands:\r\n");
    uart_log_printf("  at+join\r\n");
    uart_log_printf("  at+send=lora:X:YYY\r\n");
    uart_log_printf("  at+set_config=lora:region:XXX\r\n");
    uart_log_printf("  at+get_config=lora:channel\r\n");
    uart_log_printf("  at+set_config=lora:dev_eui:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:app_eui:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:app_key:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:dev_addr:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:apps_key:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:nwks_key:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:multicastenable:X\r\n");
    uart_log_printf("  at+set_config=lora:multicast_dev_addr:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:multicast_apps_key:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:multicast_nwks_key:XXXX\r\n");
    uart_log_printf("  at+set_config=lora:default_parameters\r\n");
    uart_log_printf("  at+set_config=lora:join_mode:X\r\n");
    uart_log_printf("  at+set_config=lora:work_mode:X\r\n");
    uart_log_printf("  at+set_config=lora:ch_mask:X:Y\r\n");
    uart_log_printf("  at+set_config=lora:class:X\r\n");
    uart_log_printf("  at+set_config=lora:confirm:X\r\n");
    uart_log_printf("  at+set_config=lora:dr:X\r\n");
    uart_log_printf("  at+set_config=lora:tx_power:X\r\n");
    uart_log_printf("  at+set_config=lora:adr:X\r\n");
    uart_log_printf("  at+get_config=lora:status\r\n");
    uart_log_printf("  at+set_config=lora:dutycycle_enable:X\r\n"); 
    uart_log_printf("  at+set_config=lora:send_repeat_cnt:X\r\n");   
    uart_log_printf("\r\n");

    uart_log_printf("LoRa P2P AT commands:\r\n");
    uart_log_printf("  at+set_config=lorap2p:XXX:Y:Z:A:B:C\r\n");
    uart_log_printf("  at+set_config=lorap2p:transfer_mode:X\r\n");
    uart_log_printf("  at+send=lorap2p:XXX\r\n");
    uart_log_printf("\r\n");

    uart_log_printf("BLE AT commands:\r\n");
    uart_log_printf("  at+set_config=ble:work_mode:X:Y\r\n");
}

void at_parse(char *cmd)
{
    char  *ptr = NULL;
    uint8_t send_data[256] = {0};
    uint8_t lora_port[64] = {0};
    uint8_t sleep_data[10] = {0};
    uint8_t index = 0;
    uint32_t param_len = 0;
    uint8_t lora_config_data[10] = {0};
    RUI_RETURN_STATUS ret_code;

    // at+version
    if(strcmp(cmd,"at+version") == 0)
    {
        char Firmware_version[48]="V";
        strcat(Firmware_version, RUI_VERSION);
        rui_at_response(true, Firmware_version, RUI_AT_OK);

        return;
    }

    // at+set_config=device:sleep:x
    if(strstr(cmd,"at+set_config=device:sleep") != NULL)
    {
        at_param_location(cmd, "sleep:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if (*ptr == '1')
        {
            ret_code = rui_device_sleep(1);
            switch(ret_code)
            {
                case RUI_STATUS_OK:
                    break;
                case RUI_STATUS_PARAMETER_INVALID:
                    uart_log_printf("ERROR: %d\r\n",RUI_AT_PARAMETER_INVALID);
                    break;
                default:
                    uart_log_printf("ERROR: %d\r\n",ret_code);
                    break;
            }
        }
        else if (*ptr == '0')
        {
            rui_device_sleep(0);
            power_flag = 0;
            
            rui_at_response(true, "Wake Up", RUI_AT_OK);
        }
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
        }

        return;
    }

    // at+set_config=device:restart
    if(strcmp(cmd,"at+set_config=device:restart") == NULL)
    {
        rui_delay_ms(50);
		rui_device_reset();
        return;
    }

    // at+get_config=device:status
    if(strcmp(cmd,"at+get_config=device:status") == NULL)
    {
        rui_at_response(true, "Board Core:RAK4600\r\nMCU:nRF52832\r\nLoRa chip:SX1276", RUI_AT_OK);
        return;
    }

    // at+set_config=lora:dev_eui:XXXX
    if(strstr(cmd,"at+set_config=lora:dev_eui:")!= NULL)
    {
        uint8_t dev_eui[8];
        at_param_location(cmd, "dev_eui:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 16, dev_eui))
            return ;

        ret_code = rui_lora_set_dev_eui(dev_eui);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:app_eui:XXXX
    if(strstr(cmd,"at+set_config=lora:app_eui:")!= NULL)
    {
        uint8_t app_eui[8];
        at_param_location(cmd, "app_eui:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 16, app_eui))
            return ;

        ret_code = rui_lora_set_app_eui(app_eui);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK);
        }
        return;
    }

    // at+set_config=lora:app_key:XXXX
    if(strstr(cmd,"at+set_config=lora:app_key:")!= NULL)
    {
        uint8_t app_key[16];
        at_param_location(cmd, "app_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, app_key))
            return ;

        ret_code = rui_lora_set_app_key(app_key);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:dev_addr:XXXX
    if(strstr(cmd,"at+set_config=lora:dev_addr:")!= NULL)
    {
        uint8_t dev_addr[4];
        at_param_location(cmd, "dev_addr:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 8, dev_addr))
            return ;

        ret_code = rui_lora_set_dev_addr(dev_addr);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:apps_key:XXXX
    if(strstr(cmd,"at+set_config=lora:apps_key:")!= NULL)
    {
        uint8_t appskey[16];
        at_param_location(cmd, "apps_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, appskey))
            return ;

        ret_code = rui_lora_set_apps_key(appskey);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:nwkskey:XXXX
    if(strstr(cmd,"at+set_config=lora:nwks_key:")!= NULL)
    {
        uint8_t nwkskey[16];
        at_param_location(cmd, "nwks_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, nwkskey))
            return ;

        ret_code = rui_lora_set_nwks_key(nwkskey);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:region:XXX
    if(strstr(cmd,"at+set_config=lora:region")!= NULL)
    {
        LORA_REGION region;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "region:", &ptr);
        region = rui_lora_region_convert(ptr);
        if (region == 0xFF)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        ret_code = rui_lora_set_region(region);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_LORA_STATUS_REGION_NOT_SUPPORTED); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:join_mode:XXX
    if(strstr(cmd,"at+set_config=lora:join_mode")!= NULL)
    {
        uint8_t join_mode = RUI_OTAA;
        ptr = NULL;
        index = 0;

        at_param_location(cmd, "join_mode:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if(*ptr == '0')
            join_mode = RUI_OTAA;
        else if (*ptr == '1')
            join_mode = RUI_ABP;
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_join_mode(join_mode);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+join
    if(strcmp(cmd,"at+join") == NULL)
    {
        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        ret_code = rui_lora_join();
        switch(ret_code)
        {
            case RUI_STATUS_OK: 
                if (RUI_ABP == g_rui_cfg_t.g_lora_cfg_t.join_mode) 
                    LoRaWANJoined_callback(1);
                break;

            case RUI_STATUS_PARAMETER_INVALID:
                RUI_LOG_PRINTF_MERGE("ERROR: %d\r\n",RUI_AT_PARAMETER_INVALID);
                break;

            default: 
                RUI_LOG_PRINTF_MERGE("ERROR: %d\r\n", ret_code);
                break;
        }

        return;
    }
    
    // at+send=lora:X:YYY
    if(strstr(cmd,"at+send=lora:")!= NULL)
    {
        uint32_t port;
        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        memset(lora_port, 0, sizeof(lora_port));
        memset(send_data, 0, sizeof(send_data));
        index = 0;

        at_param_location(cmd, "lora:", &ptr);
        for(ptr; *ptr !=':'; ptr++)
        {
            lora_port[index++] = *ptr;
        }

        port = atoi(lora_port);
        if (port < 10)
        {
            if (false == at_param_digital_verify(lora_port, 1))
                return ;
        }
        else if (port < 100)
        {
            if (false == at_param_digital_verify(lora_port, 2))
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(lora_port, 3))
                return ;
        }

        ptr++;
        index = strlen(ptr);
        if ((index%2) != 0)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else
            StrToHex(send_data, ptr, index);

        ret_code = rui_lora_send(port, send_data, index/2);
        if (ret_code != RUI_STATUS_OK)
            rui_at_response(false, NULL, ret_code);

        return;
    }
    
    // at+set_config=lora:work_mode:X
    if(strstr(cmd,"at+set_config=lora:work_mode")!= NULL)
    {
        uint32_t work_mode = 0;

        at_param_location(cmd, "work_mode:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        work_mode = atoi(ptr);
        if (work_mode > 1)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        
        ret_code = rui_lora_set_work_mode(work_mode);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK);
            
            rui_delay_ms(50);
            rui_device_reset(); // reset device
        }
        return;
    }
    
    // at+set_config=lora:class:X
    if(strstr(cmd,"at+set_config=lora:class")!= NULL)
    {
        uint8_t class;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "class:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if(*ptr == '0')
        {
            class = 0;
        }
        else if(*ptr == '2')
        {
            class = 2;
        }
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        ret_code = rui_lora_set_class(class);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:confirm:X
    if(strstr(cmd,"at+set_config=lora:confirm")!= NULL)
    {
        uint8_t confirm;

        at_param_location(cmd, "confirm:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        confirm = atoi(ptr);
        if (confirm > 1)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_confirm(confirm);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:adr:X
    if(strstr(cmd,"at+set_config=lora:adr")!= NULL)
    {
        bool adr;

        at_param_location(cmd, "adr:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if(*ptr == '0')
            adr = false;
        else if(*ptr == '1')
            adr = true;
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_adr(adr);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_STATUS_PARAMETER_INVALID);
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK);
        }

        return;
    }

    // at+set_config=lora:dr:X
    if(strstr(cmd,"at+set_config=lora:dr")!= NULL)
    {
        uint8_t dr;

        at_param_location(cmd, "dr:", &ptr);
        dr = atoi(ptr);
        if (dr < 10)
        {
            if (false == at_param_digital_verify(ptr, 1))
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(ptr, 2))
                return ;
        }

        ret_code = rui_lora_set_dr(dr);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_LORA_STATUS_DATARATE_INVALID); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }

        return;
    }

    // at+set_config=lora:tx_power:X
    if(strstr(cmd,"at+set_config=lora:tx_power:")!= NULL)
    {
        uint8_t tx_power;

        at_param_location(cmd, "tx_power:", &ptr);
        tx_power = atoi(ptr);
        if (tx_power < 10)
        {
            if (false == at_param_digital_verify(ptr, 1))
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(ptr, 2))
                return ;
        }

        ret_code = rui_lora_set_tx_power(tx_power);
        switch(ret_code)
        {
            case RUI_STATUS_OK: 
                rui_at_response(true, NULL, RUI_AT_OK); 
                break;
            case RUI_STATUS_PARAMETER_INVALID:
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                break;
            default: 
                rui_at_response(false, NULL, ret_code); 
                break;
        }

        return;
    }
    
    // at+get_config=lora:status
    if(strcmp(cmd,"at+get_config=lora:status") == NULL)
    {
        rui_lora_get_status(true, &lora_status);
        return;
    }
    
    // at+get_config=lora:channel
    if(strcmp(cmd,"at+get_config=lora:channel") == NULL)
    {
        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        rui_get_channel_list();  // print lora channel list via uart
        return;
    }

    // at+set_config=lora:ch_mask:X:Y
    if(strstr(cmd,"at+set_config=lora:ch_mask")!= NULL)
    {
        uint8_t channel_num[16] = {0};
        uint8_t channel;
        uint8_t status = 0;
        index = 0;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "ch_mask:", &ptr);
        index = 0;
        for(ptr; *ptr !=':'; ptr++)
        {
            channel_num[index++] = *ptr;
            if (index >= sizeof(channel_num))
            {
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                return ;
            }
        }
        channel = atoi(channel_num);
        if (channel < 10)
        {
            if (false == at_param_digital_verify(channel_num, 1))
                return ;
        }
        else if (channel < 100)
        {
            if (false == at_param_digital_verify(channel_num, 2))
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(channel_num, 3))
                return ;
        }

        /* 0-close 1-open */
        ptr++;
        if (false == at_param_digital_verify(ptr, 1))
            return ;
        if(*ptr == '1') {status = 1;}
        else if (*ptr == '0') {status = 0;}
        else {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_channel_mask(channel, status);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK);
                break;
            case RUI_STATUS_PARAMETER_INVALID:
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID);
                break;
            default:
                rui_at_response(false, NULL, ret_code);
                break;
        }

        return;
    }

    // at+set_config=lorap2p:transfer_mode:x
    if(strstr(cmd,"at+set_config=lorap2p:transfer_mode")!= NULL)
    {
        uint32_t transfer_mode;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_P2P)  // P2P support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "transfer_mode:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return;

        if ((*ptr != '1') && (*ptr != '2'))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        transfer_mode = atoi(ptr);
        switch(transfer_mode)
        {
            case 1:
                rui_lorap2p_set_work_mode(1);   // receive mode
                rui_at_response(true, NULL, RUI_AT_OK); 
                break;
            case 2:
                rui_lorap2p_set_work_mode(2);   // send mode
                rui_at_response(true, NULL, RUI_AT_OK); 
                break;
            default: 
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                break;;
        }

        return ;
    }

    // at+set_config=lorap2p:XXX:Y:Z:A:B:C
    if(strstr(cmd,"at+set_config=lorap2p:")!= NULL)
    {
        uint32_t Frequency;
        uint8_t  Spreadfact;
        uint8_t  Bandwidth; 
        uint8_t  Codingrate; 
        uint16_t  Preamlen; 
        uint8_t  Powerdbm;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_P2P)  // P2P support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "lorap2p:", &ptr);

        /* Frequency: 130,000,000 - 1,020,000,000 */
        if((atoi(ptr)>>(sizeof(uint32_t))*8) != 0)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else  
            Frequency = atoi(ptr);

        if ((Frequency < 130000000) || (1020000000 < Frequency))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        
        if (Frequency < 1000000000)
        {
            if(at_param_digital_verify(ptr, 9) == false)
                return ;
        }
        else
        {
            if(at_param_digital_verify(ptr, 10) == false)
                return ;
        }
        
        
        /* Spreadfact: 7-12 */
        ptr = strstr(ptr,":");
        ptr++;
        if((atoi(ptr)>12) || (atoi(ptr)<7))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        } else 
            Spreadfact = atoi(ptr);

        if (Spreadfact < 10)
        {
            if(false == at_param_digital_verify(ptr, 1))
                return ;
        }
        else
        {
            if(false == at_param_digital_verify(ptr, 2))
                return ;
        }
        
        
        /* Bandwidth: 0-2 */
        ptr = strstr(ptr,":");
        ptr++;
        if((atoi(ptr)>2) || (atoi(ptr)<0))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        } 
        else 
            Bandwidth = atoi(ptr);

        if(false == at_param_digital_verify(ptr, 1))
            return ;
        
        
        /* Codingrate: 1-4 */
        ptr = strstr(ptr,":");
        ptr++;
        if((atoi(ptr)>4) || (atoi(ptr)<1))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else 
            Codingrate = atoi(ptr);

        if(false == at_param_digital_verify(ptr, 1))
            return ;
        
        
        /* Preamlen: 5-65535 */
        ptr = strstr(ptr,":");
        ptr++;
        if((atoi(ptr)>65535) || (atoi(ptr)<5))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else 
            Preamlen = atoi(ptr);

        uint32_t len = 1;
        for (uint8_t i=1; i<6; i++)
        {
            len = len * 10;
            if (Preamlen < len)
            {
                if(false == at_param_digital_verify(ptr, i))
                    return ;
                else
                    break ;
            }
        }
        
        /* Powerdbm: 5-20 */
        ptr = strstr(ptr,":");
        ptr++;
        if((atoi(ptr)>20) || (atoi(ptr)<0))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else 
            Powerdbm = atoi(ptr);

        if (Powerdbm < 10)
        {
            if(false == at_param_digital_verify(ptr, 1))
                return ;
        }
        else
        {
            if(false == at_param_digital_verify(ptr, 2))
                return ;
        }

//        uart_log_printf("Frequency is %d\r\n", Frequency);
//        uart_log_printf("Spreadfact is %d\r\n", Spreadfact);
//        uart_log_printf("Bandwidth is %d\r\n", Bandwidth);
//        uart_log_printf("Codingrate is %d\r\n", Codingrate);
//        uart_log_printf("Preamlen is %d\r\n", Preamlen);
//        uart_log_printf("Powerdbm is %d\r\n", Powerdbm);

        if(rui_lorap2p_config(Frequency,Spreadfact,Bandwidth,Codingrate,Preamlen,Powerdbm) == RUI_STATUS_OK)
        {
            rui_at_response(true, NULL, RUI_AT_OK); 
            return ;
        }
        else 
        {
            rui_at_response(false, NULL, RUI_AT_LORA_BUSY); 
            return ;
        }


        return ;
    }
    

    // at+send=lorap2p:XXX
    if(strstr(cmd,"at+send=lorap2p:")!= NULL)
    {
        uint32_t send_length;
        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_P2P)  // P2P support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        if(g_rui_cfg_t.g_lora_p2p_cfg_t.p2p_workmode != 2)
        {
            rui_at_response(false, NULL, RUI_AT_LORA_BUSY); 
            return ;
        }

        memset(lora_port, 0, sizeof(lora_port));
        memset(send_data, 0, sizeof(send_data));
        at_param_location(cmd, "lorap2p:", &ptr);

        send_length = strlen(ptr);
        if (((send_length%2) != 0) || (false == at_check_hex_param_length(ptr, send_length)))
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else
            StrToHex(send_data, ptr, send_length);

        ret_code = rui_lorap2p_send(send_data, send_length/2);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                break;
            case RUI_STATUS_PARAMETER_INVALID:
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID);
                break;
            default:
                rui_at_response(false, NULL, ret_code);
                break;
        }

        return;
    }
    
    // at+set_config=lora:multicastenable:X
    if(strstr(cmd,"at+set_config=lora:multicastenable")!= NULL)
    {
        bool enable;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "multicastenable:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if(*ptr == '0')
            enable = false;
        else if(*ptr == '1')
            enable = true;
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_multicastenable(enable);
        switch(ret_code)
        {
            case RUI_STATUS_OK: 
                rui_at_response(true, NULL, RUI_AT_OK); 
                break;
            default:
                rui_at_response(false, NULL, ret_code);
                break;
        }               

        return;
    }

    // at+set_config=lora:multicast_dev_addr:XXXX
    if(strstr(cmd,"at+set_config=lora:multicast_dev_addr")!= NULL)
    {
        uint8_t dev_addr[4];

        at_param_location(cmd, "multicast_dev_addr:", &ptr);
        if (false == at_param_hex2string_verify(ptr, 8, dev_addr))
            return ;

        ret_code = rui_lora_set_multicast_dev_addr(dev_addr);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return ;
            default: 
                rui_at_response(false, NULL, ret_code); 
                return ;
        }

        return ;
    }

    // at+set_config=lora:multicast_apps_key:XXXX
    if(strstr(cmd,"at+set_config=lora:multicast_apps_key")!= NULL)
    {
        uint8_t appskey[16];
        at_param_location(cmd, "multicast_apps_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, appskey))
            return ;

        ret_code = rui_lora_set_multicast_apps_key(appskey);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return SUCCESS;
            default:
                rui_at_response(false, NULL, ret_code); 
                return FAIL;
        }

        return ;
    }

    // at+set_config=lora:multicast_nwks_key:XXXX
    if(strstr(cmd,"at+set_config=lora:multicast_nwks_key")!= NULL)
    {
        uint8_t nwkskey[16];
        at_param_location(cmd, "multicast_nwks_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, nwkskey))
            return ;

        ret_code = rui_lora_set_multicast_nwks_key(nwkskey);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return ;
            default: 
                rui_at_response(false, NULL, ret_code); 
                return ;
        }

        return ;
    }

    // at+set_config=device:uart:X:Y
    if (strstr(cmd, "at+set_config=device:uart:")!=NULL)
    {
        uint32_t baud;
        RUI_UART_BAUDRATE baudrate;

        at_param_location(cmd, "uart:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if (*ptr != '1')
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ptr += 2;
        baud = atoi(ptr);
        if (baud < 10000)
        {
            if (false == at_param_digital_verify(ptr, 4)) 
                return ;
        }
        else if (baud < 100000)
        {
            if (false == at_param_digital_verify(ptr, 5)) 
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(ptr, 6)) 
                return ;
        }
        baudrate = user_uart_baudrate_itoe(baud);
        if (baudrate == 0xFF)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        else
        {
            rui_uart_init(RUI_UART1, baudrate);
        }

        rui_at_response(true, NULL, RUI_AT_OK);
        return ;
    }

    // at+set_config=device:uart_mode:X:Y
    if (strstr(cmd, "at+set_config=device:uart_mode:")!=NULL)
    {
        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "uart_mode:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if (false == at_param_digital_verify(ptr+2, 1))
            return ;

        if ((*ptr == '1') && (*(ptr+2) == '1'))
        {
            rui_uart_mode_config(RUI_UART1, RUI_UART_UNVARNISHED);
        }
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        rui_at_response(true, NULL, RUI_AT_OK);
        return ;
    }

    // at+set_config=ble:work_mode:X:Y
    if (strstr(cmd, "at+set_config=ble:work_mode:") != NULL)
    {
        uint8_t work_mode;
        bool long_range_enable;
        at_param_location(cmd, "work_mode:", &ptr);

        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if (false == at_param_digital_verify(ptr+2, 1))
            return ;

        if (*ptr == '0')
        {
            work_mode = BLE_MODE_PERIPHERAL;
        }
        else if (*ptr == '1')
        {
            work_mode = BLE_MODE_CENTRAL;
        }
        else if (*ptr == '2')
        {
            work_mode = BLE_MODE_OBSERVER;
        }
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        ptr += 2;

        #ifdef S140
            if (*ptr == '1') { long_range_enable = 1; }
            else if (*ptr == '0'){ long_range_enable = 0; }
            else
            {
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                return ;
            }
        #endif

        #ifdef S132
            if (*ptr == '0')
                { long_range_enable = 0; }
            else
            {
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                return ;
            }
        #endif

        ret_code = rui_ble_set_work_mode(work_mode, long_range_enable);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+help
    if (strcmp(cmd, "at+help") == NULL)
    {
        rui_at_response(true, NULL, RUI_AT_OK);
        at_response_help();
        return;
    }

    // at+set_config=lora:sys_dev_eui:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_dev_eui:")!= NULL)
    {
        uint8_t dev_eui[8];
        at_param_location(cmd, "sys_dev_eui:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 16, dev_eui))
            return ;

        ret_code = rui_lora_set_sys_dev_eui(dev_eui);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    
    // at+set_config=lora:sys_app_eui:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_app_eui:")!= NULL)
    {
        uint8_t app_eui[8];
        at_param_location(cmd, "sys_app_eui:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 16, app_eui))
            return ;

        ret_code = rui_lora_set_sys_app_eui(app_eui);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK);
        }
        return;
    }

    // at+set_config=lora:sys_app_key:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_app_key:")!= NULL)
    {
        uint8_t app_key[16];
        at_param_location(cmd, "sys_app_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, app_key))
            return ;

        ret_code = rui_lora_set_sys_app_key(app_key);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:sys_dev_addr:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_dev_addr:")!= NULL)
    {
        uint8_t dev_addr[4];
        at_param_location(cmd, "sys_dev_addr:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 8, dev_addr))
            return ;

        ret_code = rui_lora_set_sys_dev_addr(dev_addr);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:sys_apps_key:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_apps_key:")!= NULL)
    {
        uint8_t appskey[16];
        at_param_location(cmd, "sys_apps_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, appskey))
            return ;

        ret_code = rui_lora_set_sys_apps_key(appskey);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }
    
    // at+set_config=lora:sys_nwks_key:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_nwks_key:")!= NULL)
    {
        uint8_t nwkskey[16];
        at_param_location(cmd, "sys_nwks_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, nwkskey))
            return ;

        ret_code = rui_lora_set_sys_nwks_key(nwkskey);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, RUI_AT_RW_FLASH_ERROR); 
        }
        else { 
            rui_at_response(true, NULL, RUI_AT_OK); 
        }
        return;
    }

    // at+set_config=lora:sys_multicast_dev_addr:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_multicast_dev_addr")!= NULL)
    {
        uint8_t dev_addr[4];
        at_param_location(cmd, "sys_multicast_dev_addr:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 8, dev_addr))
            return ;

        ret_code = rui_lora_set_sys_multicast_dev_addr(dev_addr);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return ;
            default: 
                rui_at_response(false, NULL, ret_code); 
                return ;
        }

        return ;
    }

    // at+set_config=lora:sys_multicast_apps_key:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_multicast_apps_key")!= NULL)
    {
        uint8_t appskey[16];
        at_param_location(cmd, "sys_multicast_apps_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, appskey))
            return ;

        ret_code = rui_lora_set_sys_multicast_apps_key(appskey);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return SUCCESS;
            default:
                rui_at_response(false, NULL, ret_code); 
                return FAIL;
        }

        return ;
    }

    // at+set_config=lora:sys_multicast_nwks_key:XXXX
    if(strstr(cmd,"at+set_config=lora:sys_multicast_nwks_key")!= NULL)
    {
        uint8_t nwkskey[16];
        at_param_location(cmd, "sys_multicast_nwks_key:", &ptr);

        if (false == at_param_hex2string_verify(ptr, 32, nwkskey))
            return ;

        ret_code = rui_lora_set_sys_multicast_nwks_key(nwkskey);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return SUCCESS;
            default: 
                rui_at_response(false, NULL, ret_code); 
                return FAIL;
        }

        return ;
    }

    // at+set_config=lora:default_parameters
    if(strcmp(cmd,"at+set_config=lora:default_parameters") == NULL)
    {
        ret_code = rui_lora_set_sys_default_parameters();
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                return SUCCESS;
            default: 
                rui_at_response(false, NULL, ret_code); 
                return FAIL;
        }

        return ;
    }

    // at+set_config=lora:dutycycle_enable:X
    if(strstr(cmd,"at+set_config=lora:dutycycle_enable")!= NULL)
    {
        bool dutycycle_enable;

        if (g_rui_cfg_t.g_lora_cfg_t.work_mode != RUI_LORAWAN)  // LoRaWAN support only
        {
            rui_at_response(false, NULL, RUI_AT_LORA_SERVICE_UNKNOWN);
            return ;
        }

        at_param_location(cmd, "dutycycle_enable:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        if(*ptr == '0')
            dutycycle_enable = false;
        else if(*ptr == '1')
            dutycycle_enable = true;
        else
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }

        ret_code = rui_lora_set_dutycycle_enable(dutycycle_enable);
        if (ret_code != RUI_STATUS_OK) {
            rui_at_response(false, NULL, ret_code);
        }
        else {
            rui_at_response(true, NULL, RUI_AT_OK);
        }

        return;
    }

    // at+set_config=lora:send_repeat_cnt:X
    if(strstr(cmd,"at+set_config=lora:send_repeat_cnt")!= NULL)
    {
        uint8_t send_repeat_cnt;

        at_param_location(cmd, "send_repeat_cnt:", &ptr);
        if (false == at_param_digital_verify(ptr, 1))
            return ;

        send_repeat_cnt = atoi(ptr);
        if (send_repeat_cnt > 7)
        {
            rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
            return ;
        }
        
        ret_code = rui_lora_set_send_repeat_cnt(send_repeat_cnt);
        switch(ret_code)
        {
            case RUI_STATUS_OK:
                rui_at_response(true, NULL, RUI_AT_OK); 
                break;
            case RUI_STATUS_PARAMETER_INVALID:
                rui_at_response(false, NULL, RUI_AT_PARAMETER_INVALID); 
                break;
            default:
                rui_at_response(false, NULL, ret_code); 
                break;
        }

        return;
    }

    // at+test=sleep_delay:X
    if(strstr(cmd,"at+test=sleep_delay")!= 0)
    {
        uint32_t delay_time;
        at_param_location(cmd, "sleep_delay:", &ptr);

        delay_time = atoi(ptr);
        if (delay_time < 10)
        {
            if (false == at_param_digital_verify(ptr, 1))
                return ;
        }
        else
        {
            if (false == at_param_digital_verify(ptr, 2))
                return ;
        }
        delay_time = (delay_time < 30) ? delay_time : 30;
        uart_log_printf("Device will sleep after %ds.\r\n", delay_time);
        uart_log_printf("Please disconnect the connector of the module pin.\r\n");

        rui_delay_ms(delay_time*1000);
        rui_device_sleep(1);

        return;
    }

    // at+test=crystal_check
    if(strstr(cmd,"at+test=crystal_check")!= 0)
    {
        uint32_t tick = rui_crystal_check();
        if ((tick < 950) || (1050 < tick))
            uart_log_printf("Crystal check failed.\r\n", tick);
        else
            uart_log_printf("Crystal check success. Tick is %dms.\r\n", tick);
        return;
    }

    rui_at_response(false, NULL, RUI_AT_UNSUPPORT);
    return;
}

