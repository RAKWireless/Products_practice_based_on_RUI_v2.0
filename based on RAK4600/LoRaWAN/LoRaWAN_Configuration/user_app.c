#include "rui.h"
#include "at.h"

#ifndef __RUI_LOG_PRINT_MERGE
#define __RUI_LOG_PRINT_MERGE
#define RUI_LOG_PRINTF_MERGE(fmt, args...);  {uart_log_printf(fmt, ##args);RUI_LOG_PRINTF(fmt, ##args);}
#endif



/* User Code */

/*!
 * LoRaWAN activation mode macro definition
 */
//#define USER_LORAWAN_OTAA

/*!
 * OTAA mode parameter. (MSB)
 */
//uint8_t user_dev_eui[8] = {0xE7,0xC0,0x5B,0xE3,0x91,0x25,0x5E,0xC6};
//uint8_t user_app_eui[8] = {0x47,0x27,0xE5,0x1A,0xE8,0x7D,0xA2,0x65};
//uint8_t user_app_key[16] = {0x2E,0x5B,0x04,0xE4,0xF2,0x05,0x3C,0x94,0xB9,0xAD,0xF2,0x80,0x00,0xC7,0xEF,0x81};
uint8_t user_dev_eui[8] = {0x4F,0x54,0x41,0x41,0x00,0x00,0x08,0x68};
uint8_t user_app_eui[8] = {0x4F,0x54,0x41,0x41,0x00,0x00,0x08,0x68};
uint8_t user_app_key[16] = {0xB3,0x8B,0x0D,0xEF,0x40,0x38,0x31,0x63,0x1F,0x81,0x34,0x8E,0xEB,0xE6,0x9D,0x66};

/*!
 * ABP mode parameter. (MSB)
 */
//uint8_t user_dev_addr[4] = {0x00,0xae,0x82,0xb6};
//uint8_t user_apps_key[16] = {0x84,0x36,0x1B,0x8F,0x75,0x67,0x48,0x7F,0xEA,0x0D,0x42,0x19,0xC4,0xF2,0xAB,0x76};
//uint8_t user_nwks_key[16] = {0x64,0x53,0x7A,0x9E,0xAD,0xB8,0xF8,0x06,0x1E,0x5D,0xA9,0x15,0xAD,0xE4,0x66,0xC3};
uint8_t user_dev_addr[4] = {0x01,0x69,0xEF,0x24};
uint8_t user_apps_key[16] = {0xA7,0xE7,0xFF,0xCC,0xEC,0x8D,0xFD,0x21,0x5E,0xA6,0x6E,0xA1,0x69,0x60,0x3D,0xC2};
uint8_t user_nwks_key[16] = {0xF3,0x43,0xCC,0x36,0x1A,0x58,0x6C,0x7A,0x08,0xBE,0x4E,0x66,0xCF,0xE0,0x37,0xA0};


void setup(void)
{
    RUI_RETURN_STATUS ret_code;

    ret_code = rui_lora_set_work_mode(RUI_LORAWAN);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("LoRa work mode set error!\r\n");

    ret_code = rui_lora_set_region(EU868);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Region set error!\r\n");

#ifdef USER_LORAWAN_OTAA
    ret_code = rui_lora_set_join_mode(RUI_OTAA);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Join mode set error!\r\n");

    ret_code = rui_lora_set_dev_eui(user_dev_eui);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Dev eui set error!\r\n");

    ret_code = rui_lora_set_app_eui(user_app_eui);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("App eui set error!\r\n");

    ret_code = rui_lora_set_app_key(user_app_key);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("App key set error!\r\n");

#else
    ret_code = rui_lora_set_join_mode(RUI_ABP);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Join mode set error!\r\n");

    ret_code = rui_lora_set_dev_addr(user_dev_addr);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Dev addr set error!\r\n");

    ret_code = rui_lora_set_apps_key(user_apps_key);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Apps key set error!\r\n");

    ret_code = rui_lora_set_nwks_key(user_nwks_key);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Nwks key set error!\r\n");

#endif

    ret_code = rui_lora_set_class(RUI_CLASS_A);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Class set error!\r\n");

    ret_code = rui_lora_set_confirm(0);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Package type set error!\r\n");

    ret_code = rui_lora_set_dr(0);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("DR set error!\r\n");

    ret_code = rui_lora_set_tx_power(0);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Tx power set error!\r\n");

    ret_code = rui_lora_set_adr(false);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("ADR set error!\r\n");
}

/* End User Code */



/*******************************************************************************************
 * LoRaMac callback functions
 * * void LoRaReceive_callback(RUI_RECEIVE_T* Receive_datapackage);//LoRaWAN callback if receive data 
 * * void LoRaP2PReceive_callback(RUI_LORAP2P_RECEIVE_T *Receive_P2Pdatapackage);//LoRaP2P callback if receive data 
 * * void LoRaWANJoined_callback(uint32_t status);//LoRaWAN callback after join server request
 * * void LoRaWANSendsucceed_callback(RUI_MCPS_T status);//LoRaWAN call back after send data complete
 * *****************************************************************************************/  
void LoRaReceive_callback(RUI_RECEIVE_T* Receive_datapackage)
{
    char hex_str[3] = {0}; 
    RUI_LOG_PRINTF_MERGE("at+recv=%d,%d,%d,%d", Receive_datapackage->Port, Receive_datapackage->Rssi, Receive_datapackage->Snr, Receive_datapackage->BufferSize);   
    
    if ((Receive_datapackage->Buffer != NULL) && Receive_datapackage->BufferSize) {
        RUI_LOG_PRINTF_MERGE(":");
        for (int i = 0; i < Receive_datapackage->BufferSize; i++) {
            sprintf(hex_str, "%02x", Receive_datapackage->Buffer[i]);
            RUI_LOG_PRINTF_MERGE("%s", hex_str); 
        }
    }
    RUI_LOG_PRINTF_MERGE("\r\n");
}

void LoRaP2PReceive_callback(RUI_LORAP2P_RECEIVE_T *Receive_P2Pdatapackage)
{
    char hex_str[3]={0};
    RUI_LOG_PRINTF_MERGE("at+recv=%d,%d,%d:", Receive_P2Pdatapackage -> Rssi, Receive_P2Pdatapackage -> Snr, Receive_P2Pdatapackage -> BufferSize); 
    for(int i=0;i < Receive_P2Pdatapackage->BufferSize; i++)
    {
        sprintf(hex_str, "%02X", Receive_P2Pdatapackage->Buffer[i]);
        RUI_LOG_PRINTF_MERGE("%s",hex_str);
    }
    RUI_LOG_PRINTF_MERGE("\r\n");    
}

void LoRaWANJoined_callback(uint32_t status)
{
    if(status)  //Join Success
    {
        RUI_LOG_PRINTF_MERGE("OK Join Success\r\n");
    }
    else 
    {        
        RUI_LOG_PRINTF_MERGE("ERROR: %d\r\n",RUI_AT_LORA_INFO_STATUS_JOIN_FAIL); 
    }    
}

void LoRaWANSendsucceed_callback(RUI_MCPS_T mcps_type,RUI_RETURN_STATUS status)
{
    if(status == RUI_STATUS_OK)
    {
        switch( mcps_type )
        {
            case RUI_MCPS_UNCONFIRMED:
            {
                RUI_LOG_PRINTF_MERGE("OK\r\n");
                break;
            }
            case RUI_MCPS_CONFIRMED:
            {
                RUI_LOG_PRINTF_MERGE("OK\r\n");
                break;
            }
            case RUI_MCPS_PROPRIETARY:
            {
                RUI_LOG_PRINTF_MERGE("OK\r\n");
                break;
            }
            case RUI_MCPS_MULTICAST:
            {
                RUI_LOG_PRINTF_MERGE("OK\r\n");
                break;           
            }
            default:             
            break;
        } 
    }
    else if(status != RUI_AT_LORA_INFO_STATUS_ADDRESS_FAIL)
    {
        RUI_LOG_PRINTF_MERGE("ERROR: %d\r\n",status); 
    }
}

void LoRaP2PSendsucceed_callback(void)
{
    RUI_LOG_PRINTF_MERGE("OK \r\n");    
}

/*******************************************************************************************
 * If the uart is in RUI_UART_USER mode, this interface will be called.
 * 
 * *****************************************************************************************/ 
void rui_uart_recv(RUI_UART_DEF uart_def, uint8_t *pdata, uint16_t len)
{

    if (uart_def == RUI_UART1)
    {
        // Add user code...
    }
}

/*  the function will run before sleep, 
    user can add code to make sensor into low power mode */
void user_sensor_sleep(void)
{
    // ...
}

/*  the function will run after wake up, 
    user can add code to wake up and init sensor module. */
void user_sensor_wakeup(void)
{
    // ...
}

void main(void)
{
    RUI_LORA_STATUS_T app_lora_status; //record status 

    rui_init();

    /*******************************************************************************************
     * Register LoRaMac callback function
     * 
     * *****************************************************************************************/
    rui_lora_register_recv_callback(LoRaReceive_callback);  
    rui_lorajoin_register_callback(LoRaWANJoined_callback); 
    rui_lorasend_complete_register_callback(LoRaWANSendsucceed_callback); 
    rui_lorap2p_register_recv_callback(LoRaP2PReceive_callback);
    rui_lorap2p_complete_register_callback(LoRaP2PSendsucceed_callback);

    /*******************************************************************************************
     * Register Sleep and Wakeup callback function
     * 
     * *****************************************************************************************/
    rui_sensor_register_callback(user_sensor_wakeup, user_sensor_sleep);

    /*******************************************************************************************    
     * Obtain status information and print.
     * 
     * *****************************************************************************************/  
    rui_lora_get_status(false,&app_lora_status);
    switch(app_lora_status.work_mode)
	{
		case RUI_LORAWAN:
            RUI_LOG_PRINTF_MERGE("Current work_mode:LoRaWAN,");
            if(app_lora_status.join_mode == RUI_OTAA)
            {
                RUI_LOG_PRINTF_MERGE(" join_mode:OTAA,");  
                if(app_lora_status.MulticastEnable)
                {
                    RUI_LOG_PRINTF_MERGE(" MulticastEnable:true.\r\n");
                }
                else
                {
                    RUI_LOG_PRINTF_MERGE(" MulticastEnable: false,");
                    switch(app_lora_status.class_status)
                    {
                        case RUI_CLASS_A:RUI_LOG_PRINTF_MERGE(" Class: A\r\n");
                            break;
                        case RUI_CLASS_B:RUI_LOG_PRINTF_MERGE(" Class: B\r\n");
                            break;
                        case RUI_CLASS_C:RUI_LOG_PRINTF_MERGE(" Class: C\r\n");
                            break;
                        default:break;
                    }
                }
            }
            else if(app_lora_status.join_mode == RUI_ABP)
            {
                RUI_LOG_PRINTF_MERGE(" join_mode:ABP,\r\n");
                if(app_lora_status.MulticastEnable)
                {
                    RUI_LOG_PRINTF_MERGE(" MulticastEnable:true.\r\n");
                }
                else
                {
                    RUI_LOG_PRINTF_MERGE(" MulticastEnable: false,");
                    switch(app_lora_status.class_status)
                    {
                        case RUI_CLASS_A: RUI_LOG_PRINTF_MERGE(" Class: A\r\n");
                            break;
                        case RUI_CLASS_B: RUI_LOG_PRINTF_MERGE(" Class: B\r\n");
                            break;
                        case RUI_CLASS_C: RUI_LOG_PRINTF_MERGE(" Class: C\r\n");
                            break;
                        default:break;
                    } 
                }
            }
			break;
            
		case RUI_P2P:
            RUI_LOG_PRINTF_MERGE("Current work_mode:P2P\r\n");
            if (g_rui_cfg_t.g_lora_p2p_cfg_t.p2p_workmode == 1)
            {
                RUI_LOG_PRINTF_MERGE("LoRa P2P Transfer_mode: Receiver\r\n");
            }
            else
            {
                RUI_LOG_PRINTF_MERGE("LoRa P2P Transfer_mode: Sender\r\n");
            }
			break;

		default: 
		    break;
	}   
    RUI_LOG_PRINTF_MERGE("Initialization OK\r\n");
    rui_delay_ms(100);

    /* User Code */
    setup();
    /* End User Code */

    while(1)
    {
        //here run system work and do not modify
        rui_running();
    }
}

