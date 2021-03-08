#include "rui.h"
#include "board.h"

static RUI_RETURN_STATUS rui_return_status;
//join cnt
#define JOIN_MAX_CNT 6
static uint8_t JoinCnt=0;
RUI_LORA_STATUS_T app_lora_status; //record status 

/*******************************************************************************************
 * The BSP user functions.
 * 
 * *****************************************************************************************/ 

const uint8_t level[2]={0,1};
#define low     &level[0]
#define high    &level[1]

volatile static bool autosend_flag = false;    //auto send flag
static uint8_t a[80]={};    // Data buffer to be sent by lora
static uint8_t sensor_data_cnt=0;  //send data counter by LoRa 
bool IsJoiningflag= false;  //flag whether joining or not status
bool sample_flag = false;  //flag sensor sample record for print sensor data by AT command 
bool sample_status = false;  //current whether sample sensor completely
bool sendfull = true;  //flag whether send all sensor data 



void rui_lora_autosend_callback(void)  //auto_send timeout event callback
{
    autosend_flag = true;
    IsJoiningflag = false;      
    Pressure_init();
}


void bsp_init(void)
{
    Pressure_init();
}

uint8_t lpp_cnt=0;  //record lpp package count
typedef struct 
{   uint8_t startcnt;
    uint8_t size;
}lpp_data_t;
lpp_data_t lpp_data[10];
void user_lora_send(void)
{
    uint8_t dr;
    uint16_t ploadsize;
    static uint16_t temp_cnt=0;
    uint16_t temp_size=0;  //send package size 
    uint8_t* Psend_start;
    if(autosend_flag)
    {
        autosend_flag = false;
        rui_lora_get_dr(&dr,&ploadsize);
        if(ploadsize < sensor_data_cnt)
        {
            sendfull = false;  //need subcontract send
            Psend_start = &a[lpp_data[temp_cnt].startcnt];  
            if(lpp_data[temp_cnt].size > ploadsize)
            {
                RUI_LOG_PRINTF("ERROR: RUI_AT_LORA_LENGTH_ERROR %d\r\n",RUI_AT_LORA_LENGTH_ERROR);
                sample_status = false;
                sendfull = true;
                lpp_cnt = 0;
                temp_cnt = 0;
                sensor_data_cnt=0; 
                rui_lora_get_status(false,&app_lora_status); 
                switch(app_lora_status.autosend_status)
                {
                    case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                        break;
                    case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                        break;
                    default:break;
                }
                return; 
            }                        
            for(;temp_cnt <= lpp_cnt; temp_cnt++)
            {
                if(ploadsize < (temp_size + lpp_data[temp_cnt].size))
                {                                                              
                    rui_return_status = rui_lora_send(8,Psend_start,temp_size);
                    switch(rui_return_status)
                    {
                        case RUI_STATUS_OK:return;
                        default: RUI_LOG_PRINTF("[LoRa]: send error %d\r\n",rui_return_status);  
                            autosend_flag = true;                                      
                            break;
                    }                
                }else
                {                   
                    if(temp_cnt == lpp_cnt)
                    {
                        rui_return_status = rui_lora_send(8,Psend_start,temp_size);
                        switch(rui_return_status)
                        {
                            case RUI_STATUS_OK:RUI_LOG_PRINTF("[LoRa]: send out\r\n");
                                sample_status = false;
                                sendfull = true;
                                lpp_cnt = 0;
                                temp_cnt = 0;
                                sensor_data_cnt=0; 
                                return;
                                break;
                            default: RUI_LOG_PRINTF("[LoRa]: send error %d\r\n",rui_return_status);  
                                autosend_flag = true;                                      
                                break;
                        } 
                    }else 
                    {
                        temp_size += lpp_data[temp_cnt].size; 
                    }
                }                   
            }
        }else
        {
            rui_return_status = rui_lora_send(8,a,sensor_data_cnt);
            switch(rui_return_status)
            {
                case RUI_STATUS_OK:RUI_LOG_PRINTF("[LoRa]: send out\r\n");
                    sample_status = false;
                    sendfull = true;
                    lpp_cnt = 0;
                    sensor_data_cnt=0; 
                    break;
                default: RUI_LOG_PRINTF("[LoRa]: send error %d\r\n",rui_return_status);
                    rui_lora_get_status(false,&app_lora_status); 
                    switch(app_lora_status.autosend_status)
                    {
                        case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        default:break;
                    }
                    break;
            }               
        }
    }                    
}

double P_PSI,Temperature;
void app_loop(void)
{
    rui_lora_get_status(false,&app_lora_status);
    if(app_lora_status.IsJoined)  //if LoRaWAN is joined
    {
        if(autosend_flag)
        {
            autosend_flag=false;             
            rui_delay_ms(5);               
            Get_Pressure(&P_PSI,&Temperature);
            lpp_data[lpp_cnt].startcnt = sensor_data_cnt;
            a[sensor_data_cnt++]=0x02;
            a[sensor_data_cnt++]=0x67;
            a[sensor_data_cnt++]=((uint16_t)(Temperature*10)>>8) & 0xff;
            a[sensor_data_cnt++]=((uint16_t)(Temperature*10)) & 0xff;
            lpp_data[lpp_cnt].size = sensor_data_cnt - lpp_data[lpp_cnt].startcnt;	
            lpp_cnt++;

            lpp_data[lpp_cnt].startcnt = sensor_data_cnt;
            a[sensor_data_cnt++]=0x03;
            a[sensor_data_cnt++]=0x02;
            a[sensor_data_cnt++]=((uint16_t)(P_PSI*100)>>8) & 0xff;
            a[sensor_data_cnt++]=((uint16_t)(P_PSI*100)) & 0xff;
            lpp_data[lpp_cnt].size = sensor_data_cnt - lpp_data[lpp_cnt].startcnt;	
            lpp_cnt++;

            if(sensor_data_cnt != 0)
            { 
                sample_status = true;                   
                sample_flag = true;
                RUI_LOG_PRINTF("\r\n");
                autosend_flag = true;
                user_lora_send();                               
            }	
            else 
            {                
                rui_lora_set_send_interval(RUI_AUTO_DISABLE,0);  //stop it auto send data if no sensor data.
            }                     
        }
    }else if(IsJoiningflag == false)
    {
        IsJoiningflag = true;
        if(app_lora_status.join_mode == RUI_OTAA)
        {
            rui_return_status = rui_lora_join();
            switch(rui_return_status)
            {
                case RUI_STATUS_OK:RUI_LOG_PRINTF("OTAA Join Start...\r\n");break;
                case RUI_LORA_STATUS_PARAMETER_INVALID:RUI_LOG_PRINTF("ERROR: RUI_AT_PARAMETER_INVALID %d\r\n",RUI_AT_PARAMETER_INVALID);
                    rui_lora_get_status(false,&app_lora_status);  //The query gets the current status 
                    switch(app_lora_status.autosend_status)
                    {
                        case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        default:break;
                    } 
                    break;
                default: RUI_LOG_PRINTF("ERROR: LORA_STATUS_ERROR %d\r\n",rui_return_status);
                    if(app_lora_status.lora_dr > 1)rui_lora_set_dr(app_lora_status.lora_dr-1);
                    rui_lora_get_status(false,&app_lora_status); 
                    switch(app_lora_status.autosend_status)
                    {
                        case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                            break;
                        default:break;
                    }
                    break;
            }            
        }
    }		
}

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
        JoinCnt = 0;
        IsJoiningflag = false;
        RUI_LOG_PRINTF("[LoRa]:Join Success\r\nOK\r\n");
        rui_lora_get_status(false,&app_lora_status);
        if(app_lora_status.autosend_status != RUI_AUTO_DISABLE)
        {
            autosend_flag = true;  //set autosend_flag after join LoRaWAN succeeded 
        }       
    }else 
    {        
        if(JoinCnt<JOIN_MAX_CNT) // Join was not successful. Try to join again
        {
            JoinCnt++;
            RUI_LOG_PRINTF("[LoRa]:Join retry Cnt:%d\r\n",JoinCnt);
            rui_lora_get_status(false,&app_lora_status);
            if(app_lora_status.lora_dr > 0)
            {
                app_lora_status.lora_dr -= 1;
            }else app_lora_status.lora_dr = 0;
            rui_lora_set_dr(app_lora_status.lora_dr);
            rui_lora_join();                    
        }
        else   //Join failed
        {
            RUI_LOG_PRINTF("ERROR: RUI_AT_LORA_INFO_STATUS_JOIN_FAIL %d\r\n",RUI_AT_LORA_INFO_STATUS_JOIN_FAIL); 
			rui_lora_get_status(false,&app_lora_status); 
            switch(app_lora_status.autosend_status)
            {
                case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                    break;
                case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
                    break;
                default:break;
            }
            JoinCnt=0;   
        }          
    }    
}
void LoRaWANSendsucceed_callback(RUI_MCPS_T mcps_type,RUI_RETURN_STATUS status)
{
    if(sendfull == false)
    {
        autosend_flag = true;
        return;
    }

    if(status == RUI_STATUS_OK)
    {
        switch( mcps_type )
        {
            case RUI_MCPS_UNCONFIRMED:
            {
                RUI_LOG_PRINTF("[LoRa]: RUI_MCPS_UNCONFIRMED send success\r\nOK\r\n");
                break;
            }
            case RUI_MCPS_CONFIRMED:
            {
                RUI_LOG_PRINTF("[LoRa]: RUI_MCPS_CONFIRMED send success\r\nOK\r\n");
                break;
            }
            case RUI_MCPS_PROPRIETARY:
            {
                RUI_LOG_PRINTF("[LoRa]: RUI_MCPS_PROPRIETARY send success\r\nOK\r\n");
                break;
            }
            case RUI_MCPS_MULTICAST:
            {
                RUI_LOG_PRINTF("[LoRa]: RUI_MCPS_MULTICAST send success\r\nOK\r\n");
                break;           
            }
            default:             
                break;
        } 
    }else RUI_LOG_PRINTF("ERROR: LORA_STATUS_ERROR %d\r\n",status);    
    
	
    rui_lora_get_status(false,&app_lora_status);//The query gets the current status 
    switch(app_lora_status.autosend_status)
    {
        case RUI_AUTO_ENABLE_SLEEP:rui_lora_set_send_interval(RUI_AUTO_ENABLE_SLEEP,app_lora_status.lorasend_interval);  //start autosend_timer after send success
            rui_delay_ms(5);  
            break;
        case RUI_AUTO_ENABLE_NORMAL:rui_lora_set_send_interval(RUI_AUTO_ENABLE_NORMAL,app_lora_status.lorasend_interval);  //start autosend_timer after send success
            break;
        default:break;
    }  
}

void LoRaP2PSendsucceed_callback(void)
{
    RUI_LOG_PRINTF("[LoRa] P2PTxDone.\r\n");    
}

/*******************************************************************************************
 * The RUI is used to receive data from uart.
 * 
 * *****************************************************************************************/ 
void rui_uart_recv(RUI_UART_DEF uart_def, uint8_t *pdata, uint16_t len)
{
    switch(uart_def)
    {
        case RUI_UART1://process code if RUI_UART1 work at RUI_UART_UNVARNISHED
            rui_lora_get_status(false,&app_lora_status);
            if(app_lora_status.IsJoined)  //if LoRaWAN is joined
            {
                rui_lora_send(8,pdata,len);
            }else
            {
                RUI_LOG_PRINTF("ERROR: RUI_AT_LORA_NO_NETWORK_JOINED %d",RUI_AT_LORA_NO_NETWORK_JOINED);
            }
             
            break;
        case RUI_UART3://process code if RUI_UART3 received data ,the len is always 1
            /*****************************************************************************
             * user code 
            ******************************************************************************/
            break;
        default:break;
    }
}

/*******************************************************************************************
 * the app_main function
 * *****************************************************************************************/ 
void main(void)
{
    rui_init();
    bsp_init();
    
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
     *The query gets the current status 
    * 
    * *****************************************************************************************/ 
    rui_lora_get_status(false,&app_lora_status);

	if(app_lora_status.autosend_status)RUI_LOG_PRINTF("autosend_interval: %us\r\n", app_lora_status.lorasend_interval);

    /*******************************************************************************************    
     *Init OK ,print board status and auto join LoRaWAN
    * 
    * *****************************************************************************************/  
    switch(app_lora_status.work_mode)
	{
		case RUI_LORAWAN:
            RUI_LOG_PRINTF("Initialization OK,Current work_mode:LoRaWAN,");
            if(app_lora_status.join_mode == RUI_OTAA)
            {
                RUI_LOG_PRINTF(" join_mode:OTAA,");  
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
            }else if(app_lora_status.join_mode == RUI_ABP)
            {
                RUI_LOG_PRINTF(" join_mode:ABP,\r\n");
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
                if(rui_lora_join() == RUI_STATUS_OK)//join LoRaWAN by ABP mode
                {
                    LoRaWANJoined_callback(1);  //ABP mode join success
                }
            }
			break;
		case RUI_P2P:RUI_LOG_PRINTF("Current work_mode:P2P\r\n");
			break;
		default: break;
	}   
    RUI_LOG_PRINTF("\r\n");

    while(1)
    {       
        rui_lora_get_status(false,&app_lora_status);//The query gets the current status 
        rui_running();
        switch(app_lora_status.work_mode)
        {
            case RUI_LORAWAN:
                if(!sample_status)app_loop();
                else user_lora_send();			
                break;
            case RUI_P2P:
                /*************************************************************************************
                 * user code at LoRa P2P mode
                *************************************************************************************/
                break;
            default :break;
        }
    }
}
