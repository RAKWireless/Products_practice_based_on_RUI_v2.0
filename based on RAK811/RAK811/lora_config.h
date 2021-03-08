#ifndef _LORA_CONFIG_H_
#define _LORA_CONFIG_H_

typedef union 
{ 
    uint8_t  data0[4];
    uint32_t data1;
}union_data;

typedef enum
{
    restart,
    sleep,
    boot,
    status,    
    uart,
    uart_mode,
    gpio,
    adc,
    i2c,
    region,
    channel,    
    dev_eui,
    app_eui,
    app_key,
    dev_addr,
    apps_key,
    nwks_key,
    join_mode,
    work_mode,    
    ch_mask,
    class,
    confirm,
    dr,
    tx_power,
    adr,
    send_interval,
    multicastenable,
    multicast_dev_addr,
    multicast_apps_key,
    multicast_nwks_key,

    sys_dev_eui,
    sys_app_eui,
    sys_app_key,
    sys_dev_addr,
    sys_apps_key,
    sys_nwks_key,
    sys_multicast_dev_addr,
    sys_multicast_apps_key,
    sys_multicast_nwks_key,
    default_parameters,
    dutycycle_enable,
    join_repeat_cnt,
    send_repeat_cnt
}board_config_Enum;

typedef enum DRIVER_MODE
{
	NORMAL_MODE = 0,
	POWER_ON_MODE,
	POWER_OFF_MODE,
	SLEEP_MODE,
	STANDBY_MODE
}DRIVER_MODE;


int read_config(char *in);

#endif