#include "rui.h"
#include "at.h"

#ifndef __RUI_LOG_PRINT_MERGE
#define __RUI_LOG_PRINT_MERGE
#define RUI_LOG_PRINTF_MERGE(fmt, args...);  {uart_log_printf(fmt, ##args);RUI_LOG_PRINTF(fmt, ##args);}
#endif



/* User Code */
#include "timer.h"

/*!
 * User sleep timer related variables.
 */
RUI_TIMER_ST user_timer;    // Timer instance.
uint32_t timer_interval = 1; // Timer timeout, in seconds.
bool timeout_flag = false;

/*!
 * BLE related definition
 */
#define BLE_AD_TYPE_MANUFACTURER_SPECIFIC_DATA  0xFF


void uart_log_printf_hex(uint8_t *pdata, uint32_t len)
{
    while(len--)
        uart_log_printf("%02X", *pdata++);
}

uint16_t ble_advdata_search_extra(uint8_t const * p_encoded_data,
                                    uint16_t        data_len,
                                    uint16_t      * p_offset,
                                    uint8_t         ad_type)
{
    if ((p_encoded_data == NULL) || (p_offset == NULL))
        return 0;

    uint16_t i = 0;

    while (((i < *p_offset) || (p_encoded_data[i + 1] != ad_type)) && (i < data_len))
    {
        // Jump to next data.
        i += (p_encoded_data[i] + 1);
    }

    if (i >= data_len)
        return 0;
    else
    {
        *p_offset = i + 2;
        return (p_encoded_data[i] - 1);
    }
}

uint8_t * ble_advdata_parse_extra(uint8_t  * p_encoded_data,
                                uint16_t   data_len,
                                uint8_t    ad_type)
{
    uint16_t offset = 0;
    uint16_t len    = ble_advdata_search_extra(p_encoded_data, data_len, &offset, ad_type);

    if (len == 0)
        return NULL;
    else
        return &p_encoded_data[offset];
}

/*
    BLE scan callback function
*/
void rui_ble_scan_adv(int8_t rssi_value, uint8_t *p_adv_data, uint16_t adv_data_len, uint8_t *p_device_mac)
{
    // The function of the following flags is to "slow down" the log output
    if (timeout_flag != true)
        return ;

    timeout_flag = false;
    rui_timer_start(&user_timer);


    /* According to the manufacturer specific data filtering equipment in the broadcast data.
       Company: Apple, Inc.<0x004C>
       Type: Beacon <0x02>
     */
    uint8_t filter_data[]={0x4C, 0x00, 0x02};

    // Search the pointer of manufacturer specific data
    uint8_t *p_manu_data = ble_advdata_parse_extra(p_adv_data, adv_data_len, BLE_AD_TYPE_MANUFACTURER_SPECIFIC_DATA);
    if (NULL == p_manu_data)
        return ;

    // If the broadcast information matches, print its MAC address and complete broadcast information.
    if (0x00 == memcmp(filter_data, p_manu_data, 3))
    {
        uart_log_printf("The MAC of target device is: %02X-%02X-%02X-%02X-%02X-%02X\r\n",
                        p_device_mac[5], p_device_mac[4], p_device_mac[3],
                        p_device_mac[2], p_device_mac[1], p_device_mac[0]);

        uart_log_printf("The broadcast data of the target device is: ");
        uart_log_printf_hex(p_adv_data, adv_data_len);
        uart_log_printf("\r\n");
    }
}


void user_timer_callback(void)
{
    timeout_flag = true;
}

void setup(void)
{
    RUI_RETURN_STATUS ret_code;

    user_timer.timer_mode = RUI_TIMER_MODE_SINGLE_SHOT;
    ret_code = rui_timer_init(&user_timer, user_timer_callback);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Timer init error! %d\r\n", ret_code);
    rui_timer_setvalue(&user_timer, timer_interval*1000);
    rui_timer_start(&user_timer);

    // Set BLE working mode
    if (BLE_MODE_OBSERVER != g_rui_cfg_t.g_ble_cfg_t.work_mode)
    {
        ret_code = rui_ble_set_work_mode(BLE_MODE_OBSERVER, false);
        if (ret_code != RUI_STATUS_OK)
            uart_log_printf("BLE set work mode error! %d\r\n", ret_code);
        else
            rui_device_reset();
    }

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

