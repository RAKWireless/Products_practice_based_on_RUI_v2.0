#include "rui.h"
#include "board.h"



/* User Code */

/*!
 * Sets the channel frequency
 * 150 - 960 MHz
 * 150000000 - 960000000
 */
uint32_t Frequency = 868100000;

/*!
 * Sets the spreading factor
 * 7 - 12
 */
uint8_t Spreadfact = 7;

/*!
 * Sets the bandwidth
 * 0: 125 kHz, 1: 250 kHz, 2: 500 kHz
 */
uint8_t Bandwidth = 0;

/*!
 * Sets the coding rate
 * 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8
 */
uint8_t Codingrate = 1;

/*!
 * Sets the Preamble length
 * 5 - 65535
 */
uint16_t Preamlen = 8;

/*!
 * Sets the tx power
 * 0 - 20 dBm
 */
uint8_t Powerdbm = 20;

void setup(void)
{
    RUI_RETURN_STATUS ret_code;
    RUI_LORA_STATUS_T user_lora_status;
    bool reset_flag = false;

    rui_lora_get_status(false, &user_lora_status);
    if (user_lora_status.work_mode != RUI_P2P)
        reset_flag = true;

    ret_code = rui_lora_set_work_mode(RUI_P2P);
    if (ret_code != RUI_STATUS_OK)
        RUI_LOG_PRINTF("Work mode set error!\r\n");

    if (reset_flag == true)
        rui_device_reset(); // Restart the device and initialize LoRa P2P.


    ret_code = rui_lorap2p_config(Frequency, Spreadfact, Bandwidth, Codingrate, Preamlen, Powerdbm);
    if (ret_code != RUI_STATUS_OK)
        RUI_LOG_PRINTF("LoRa p2p configuration set error!\r\n");

    ret_code = rui_lorap2p_set_work_mode(1);
    if (ret_code != RUI_STATUS_OK)
        RUI_LOG_PRINTF("Transfer mode set error!\r\n");
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
    RUI_LOG_PRINTF("at+recv=%d,%d,%d,%d", Receive_datapackage->Port, Receive_datapackage->Rssi, Receive_datapackage->Snr, Receive_datapackage->BufferSize);   
    
    if ((Receive_datapackage->Buffer != NULL) && Receive_datapackage->BufferSize) {
        RUI_LOG_PRINTF(":");
        for (int i = 0; i < Receive_datapackage->BufferSize; i++) {
            sprintf(hex_str, "%02x", Receive_datapackage->Buffer[i]);
            RUI_LOG_PRINTF("%s", hex_str); 
        }
    }
    RUI_LOG_PRINTF("\r\n");
}

void LoRaP2PReceive_callback(RUI_LORAP2P_RECEIVE_T *Receive_P2Pdatapackage)
{
    char hex_str[3]={0};
    RUI_LOG_PRINTF("at+recv=%d,%d,%d:", Receive_P2Pdatapackage -> Rssi, Receive_P2Pdatapackage -> Snr, Receive_P2Pdatapackage -> BufferSize); 
    for(int i=0;i < Receive_P2Pdatapackage -> BufferSize; i++)
    {
        sprintf(hex_str, "%02X", Receive_P2Pdatapackage -> Buffer[i]);
        RUI_LOG_PRINTF("%s",hex_str);
    }
    RUI_LOG_PRINTF("\r\n");    
}

void LoRaWANJoined_callback(uint32_t status)
{
    static int8_t dr; 
    if(status)  //Join Success
    {
        RUI_LOG_PRINTF("OK Join Success\r\n");
    }else 
    {        
        RUI_LOG_PRINTF("ERROR: %d\r\n",RUI_AT_LORA_INFO_STATUS_JOIN_FAIL);
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
                RUI_LOG_PRINTF("OK\r\n");
                break;
            }
            case RUI_MCPS_CONFIRMED:
            {
                RUI_LOG_PRINTF("OK\r\n");
                break;
            }
            case RUI_MCPS_PROPRIETARY:
            {
                RUI_LOG_PRINTF("OK\r\n");
                break;
            }
            case RUI_MCPS_MULTICAST:
            {
                RUI_LOG_PRINTF("OK\r\n");
                break;           
            }
            default:
                break;
        }
    }
    else if(status != RUI_AT_LORA_INFO_STATUS_ADDRESS_FAIL)
    {
        RUI_LOG_PRINTF("ERROR: %d\r\n",status); 
    }
}

void LoRaP2PSendsucceed_callback(void)
{
    RUI_LOG_PRINTF("OK \r\n");    
}

/*******************************************************************************************
 * If the uart is in RUI_UART_USER mode, this interface will be called.
 * 
 * *****************************************************************************************/ 
void rui_uart_recv(RUI_UART_DEF uart_def, uint8_t *pdata, uint16_t len)
{
    switch(uart_def)
    {
        case RUI_UART1:
            break;
        case RUI_UART2:
            break;
    }
}

/*******************************************************************************************
 * sleep and wakeup callback
 * 
 * *****************************************************************************************/
void bsp_sleep(void)
{
    /*****************************************************************************
             * user process code before enter sleep
    ******************************************************************************/
} 
void bsp_wakeup(void)
{
    /*****************************************************************************
             * user process code after exit sleep
    ******************************************************************************/
}


/*******************************************************************************************
 * the app_main function
 * *****************************************************************************************/ 
void main(void)
{
    RUI_LORA_STATUS_T app_lora_status;

    rui_init();
    
    /*******************************************************************************************
     * Register LoRaMac callback function
     * 
     * *****************************************************************************************/
    rui_lora_register_recv_callback(LoRaReceive_callback);  
    rui_lorap2p_register_recv_callback(LoRaP2PReceive_callback);
    rui_lorajoin_register_callback(LoRaWANJoined_callback); 
    rui_lorasend_complete_register_callback(LoRaWANSendsucceed_callback); 
    rui_lorap2p_complete_register_callback(LoRaP2PSendsucceed_callback);

    /*******************************************************************************************
     * Register Sleep and Wakeup callback function
     * 
     * *****************************************************************************************/
    rui_sensor_register_callback(bsp_wakeup, bsp_sleep);

    /*******************************************************************************************    
     * Obtain status information and print.
    * 
    * *****************************************************************************************/  
    rui_lora_get_status(false, &app_lora_status);
    switch(app_lora_status.work_mode)
	{
		case RUI_LORAWAN:
            RUI_LOG_PRINTF("Current work_mode:LoRaWAN,");
            if(app_lora_status.join_mode == RUI_OTAA)
            {
                RUI_LOG_PRINTF(" join_mode:OTAA,");  
                if(app_lora_status.MulticastEnable)
                {
                    RUI_LOG_PRINTF(" MulticastEnable:true.\r\n");
                }
                else
                {
                    RUI_LOG_PRINTF(" MulticastEnable: false,");
                    switch(app_lora_status.class_status)
                    {
                        case RUI_CLASS_A:RUI_LOG_PRINTF(" Class: A\r\n");
                            break;
                        case RUI_CLASS_B:RUI_LOG_PRINTF(" Class: B\r\n");
                            break;
                        case RUI_CLASS_C:RUI_LOG_PRINTF(" Class: C\r\n");
                            break;
                        default:break;
                    }              
                }
            }
            else if(app_lora_status.join_mode == RUI_ABP)
            {
                RUI_LOG_PRINTF(" join_mode:ABP,");
                if(app_lora_status.MulticastEnable)
                {
                    RUI_LOG_PRINTF(" MulticastEnable:true.\r\n");
                }
                else
                {
                    RUI_LOG_PRINTF(" MulticastEnable: false,");
                    switch(app_lora_status.class_status)
                    {
                        case RUI_CLASS_A:RUI_LOG_PRINTF(" Class: A\r\n");
                            break;
                        case RUI_CLASS_B:RUI_LOG_PRINTF(" Class: B\r\n");
                            break;
                        case RUI_CLASS_C:RUI_LOG_PRINTF(" Class: C\r\n");
                            break;
                        default:break;
                    } 
                }
            }
			break;
		case RUI_P2P:
            RUI_LOG_PRINTF("Current work_mode:P2P\r\n");
            if (g_lora_config.LoraP2PParams.p2p_workmode == 1)
            {
                RUI_LOG_PRINTF("LoRa P2P Transfer_mode: Receiver\r\n");
            }
            else
            {
                RUI_LOG_PRINTF("LoRa P2P Transfer_mode: Sender\r\n");
            }
			break;
		default: break;
	}   
    RUI_LOG_PRINTF("Initialization OK\r\n\r\n");

    /* User Code */
    setup();
    /* End User Code */

    while(1)
    {       
        rui_running();
    }
}
