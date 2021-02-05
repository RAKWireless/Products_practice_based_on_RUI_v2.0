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
RUI_TIMER_ST user_timer;  // Timer instance.
uint32_t timer_interval = 1; // Timer setting time, in seconds
bool timeout_flag = false;

/*!
 * I2C related variables.
 */
#define I2C_SCL_PIN     12  // 12 means the P0.12 pin of the MCU, Pin 5 of the RAK4600 module. Fill in the MCU pin number here.
#define I2C_SDA_PIN     13  // 13 means the P0.13 pin of the MCU, Pin 4 of the RAK4600 module. Fill in the MCU pin number here.

//#define I2C_WRITE_TEST  // If it is commented out, no write operation is performed.

RUI_I2C_ST user_i2c; // I2C instance
uint8_t i2c_data[3]; // I2C read and write buffer
uint16_t data_len = 3;

/*!
 * I2C module address.
 */
#define DS3231_ADDR             0x68 // 7-bit address
#define DA3231_ADDR_WRITE       (DS3231_ADDR<<1)
#define DS3231_ADDR_READ        (DS3231_ADDR<<1 | 0x01)


void user_timer_callback(void)
{
    timeout_flag = true;

    rui_timer_start(&user_timer);
}

void setup(void)
{
    RUI_RETURN_STATUS ret_code;

    ret_code = rui_timer_init(&user_timer, user_timer_callback);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("Timer init error! %d\r\n", ret_code);
    rui_timer_setvalue(&user_timer, timer_interval*1000);
    rui_timer_start(&user_timer);

    user_i2c.INSTANCE_ID = 1;
    user_i2c.PIN_SDA = I2C_SDA_PIN;
    user_i2c.PIN_SCL = I2C_SCL_PIN;
    user_i2c.FREQUENCY = RUI_I2C_FREQ_100K;
    ret_code = rui_i2c_init(&user_i2c);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("I2C init error! %d\r\n", ret_code);

#ifdef I2C_WRITE_TEST
    i2c_data[2] = 0x09; // Set hours. More information can be found in the DS3231 datasheet.
    i2c_data[1] = 0x05; // Set minutes
    i2c_data[0] = 0x00; // Set seconds
    ret_code = rui_i2c_rw(&user_i2c, RUI_IF_WRITE, DS3231_ADDR, 0x00, i2c_data, 3);
    if (ret_code != RUI_STATUS_OK)
        uart_log_printf("I2C write error! %d\r\n", ret_code);
    else
    {
        uart_log_printf("I2C write success.\r\n");
        uart_log_printf("Set the time to %02X:%02X:%02X.\r\n", i2c_data[2], i2c_data[1], i2c_data[0]);
    }
#endif
}

void loop(void)
{
    RUI_RETURN_STATUS ret_code;

    if (timeout_flag)
    {
        ret_code = rui_i2c_rw(&user_i2c, RUI_IF_READ, DS3231_ADDR, 0x00, i2c_data, data_len);
        if (ret_code != RUI_STATUS_OK)
            uart_log_printf("I2C read error! %d\r\n", ret_code);
        else
            uart_log_printf("Time is %02X:%02X:%02X.\r\n", i2c_data[2], i2c_data[1], i2c_data[0]);

        timeout_flag = false;
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

        /* User Code */
        loop();
        /* End User Code */
    }
}

