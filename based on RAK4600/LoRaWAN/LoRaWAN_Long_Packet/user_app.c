#include "rui.h"
#include "at.h"

#ifndef __RUI_LOG_PRINT_MERGE
#define __RUI_LOG_PRINT_MERGE
#define RUI_LOG_PRINTF_MERGE(fmt, args...);  {uart_log_printf(fmt, ##args);RUI_LOG_PRINTF(fmt, ##args);}
#endif



/* User Code */
#include "timer.h"

void lora_send_subcontract(void);

/*!
 * User sleep timer related variables.
 */
RUI_TIMER_ST user_sleep_timer;  // Timer instance.
uint32_t join_interval = 10;     // If the join fails, the retry interval, in seconds.
uint32_t send_interval = 20;     // Data sending interval time, in seconds.
bool sleep_enable = false;      // Sleep enable flag.

/*!
 * loop function enable flag.
 */
bool loop_enable = true;

/*!
 * Data parameters of Subcontracting
 */
uint8_t AppPort = 2;
uint8_t AppData[256] = {0};
uint8_t AppLen = 0;

/*!
 * Long packet data parameters
 */
uint8_t lp_data[512];
uint32_t lp_len = 512;

/*!
 * Internal variable, set to 1 at wake-up, 
 * and set to 0 after wake-up initialization. 
 * Do not change it.
 */
extern uint8_t wakeup_enable;

/*!
 * Packet transfer parameter structure
 */
typedef struct _LoRaPacket_LHDR_s{
    uint8_t  LType : 1;     // 0 - fixed length,  1 - variable length
    uint8_t  RetryFlag : 1; // 0 - normal packet,  1 - the current packet is a retransmission packet
    uint8_t  RSV : 6;       // Reserved
} __attribute__ ((packed)) LHDR_st;

typedef struct {
    LHDR_st header;             // LHDR_st
    uint8_t magic;              // A random data
    uint8_t fcnt;               // Total number of subcontracting
    uint8_t fser;               // Current number of packets to be subcontracted
    uint8_t fdata_len;          // Data filed length in current packet

    uint8_t *pdata;             // Data buffer pointer
    uint32_t data_total_len;    // Total data length
    uint32_t data_finish_len;   // The length of the data sent in the total data
    uint32_t data_packet_len;   // Maximum length of data field in single packet
}LP_State_st;

LP_State_st lp_state;

void lora_join_callback(uint32_t status)
{
    loop_enable = true;

    if (status)
        rui_timer_setvalue(&user_sleep_timer, send_interval*1000);
    else
        sleep_enable = true;  // If join fails, go to sleep.
}

void lora_send_callback(RUI_RETURN_STATUS status)
{
    if (RUI_STATUS_OK == status)
    {
        if (0 == lp_state.fcnt)
        {
            // If the transmission is completed, it goes to sleep.
            loop_enable = true;
            sleep_enable = true;  // After the data is sent, go to sleep.
        }
        else
        {
            // If the transmission is successful, send the next packet.
            lp_state.header.RetryFlag = 0;
            lp_state.fser++;
            lp_state.data_finish_len += lp_state.fdata_len;
            lora_send_subcontract();
        }
    }
    else
    {   // If the transmission is fails, resend the current packet
        lp_state.header.RetryFlag = 1;
        lora_send_subcontract();
    }
}

void user_sleep_timer_callback(void)
{
    wakeup_enable = true;
    loop_enable = true;
}

void lora_send_subcontract(void)
{
    uint32_t data_offset, data_remain_len;
    uint8_t dr;
    uint16_t payload_len;

    memset(AppData, 0x00, sizeof(AppData));
    AppLen = 0;

    rui_lora_get_dr(&dr, &payload_len);
    if (payload_len > 200) payload_len -= 20;

    lp_state.data_packet_len = payload_len - 6;

    if (lp_state.data_total_len <= lp_state.data_packet_len)
    {
        lp_state.fcnt = 0;

        data_offset = 0;
        lp_state.fdata_len = lp_state.data_total_len;
    }
    else
    {
        data_remain_len = lp_state.data_total_len - lp_state.data_finish_len;

        lp_state.fcnt = data_remain_len / lp_state.data_packet_len;
        if (0 != (data_remain_len % lp_state.data_packet_len))
            lp_state.fcnt++;

        lp_state.fcnt--;

        data_offset = lp_state.data_finish_len;
        lp_state.fdata_len = data_remain_len < lp_state.data_packet_len ? data_remain_len : lp_state.data_packet_len;
    }

    AppData[AppLen++] = *(uint8_t *)&lp_state.header;
    AppData[AppLen++] = lp_state.magic;
    AppData[AppLen++] = lp_state.fcnt;
    AppData[AppLen++] = lp_state.fser;
    AppData[AppLen++] = lp_state.fdata_len;
    memcpy(AppData+AppLen, &lp_state.pdata[data_offset], lp_state.fdata_len);
    AppLen += lp_state.fdata_len;
    
    // FCS
    for (uint8_t i=0; i<AppLen; i++)
        AppData[AppLen] += AppData[i];

    AppLen++;

    RUI_RETURN_STATUS ret_code = rui_lora_send(AppPort, AppData, AppLen);
    if (RUI_STATUS_OK != ret_code)
    {
        uart_log_printf("LoRa send error, error code %d\r\n", ret_code);
    }
}

void lora_send_long_packet(uint8_t *pdata, uint16_t len)
{
    lp_state.header.RetryFlag = 0;
    lp_state.header.LType = 1;
    lp_state.magic = NRF_RTC1->COUNTER; // Radom
    lp_state.fser = 1;

    lp_state.pdata = pdata;
    lp_state.data_finish_len = 0;
    lp_state.data_total_len = len;

    lora_send_subcontract();
}

void setup(void)
{
    user_sleep_timer.timer_mode = RUI_TIMER_MODE_SINGLE_SHOT;
    rui_timer_init(&user_sleep_timer, user_sleep_timer_callback);
    rui_timer_setvalue(&user_sleep_timer, join_interval*1000);

    // Initialize long packet data for testing
    for (uint32_t i=0; i<sizeof(lp_data); i++)
        lp_data[i] = i;
}

void loop(void)
{
    RUI_RETURN_STATUS ret_code;
    RUI_LORA_STATUS_T user_lora_status; //record status

    if (loop_enable != true)
        return ;

    if (wakeup_enable == true)
        return ;

    loop_enable = false;

    if (sleep_enable)
    {
        /* Device go to sleep */
        rui_timer_start(&user_sleep_timer);
        rui_device_sleep(1);    // go to sleep
        sleep_enable = false;
    }
    else
    {
        rui_lora_get_status(false, &user_lora_status);
        if (user_lora_status.IsJoined == false)
        {
            /* Join the network */
            uart_log_printf("Start join...\r\n");
            ret_code = rui_lora_join();
            if (ret_code != RUI_STATUS_OK)
            {
                uart_log_printf("Error code is 0x%X\r\n", ret_code);
                rui_timer_start(&user_sleep_timer);   // If join fails, start the timer and wait for the next join.
            }
            else
            {
                if (user_lora_status.join_mode == RUI_ABP)
                    LoRaWANJoined_callback(1);
            }
        }
        else
        {
            /* Send LoRa data */
            AppPort = 2;
            AppData[0]++;
            AppLen = 1;

            uart_log_printf("Send data\r\n");
            lora_send_long_packet(lp_data, lp_len);
        }
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
    /* User Code */
    lora_join_callback(status);
    /* End User Code */

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
    /* User Code */
    lora_send_callback(status);
    /* End User Code */

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

        /* User Code */
        loop();
        /* End User Code */
    }
}

