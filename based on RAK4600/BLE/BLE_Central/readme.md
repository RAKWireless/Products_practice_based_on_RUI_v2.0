# BLE-Central

## Overview

This example shows how to use the BLE-Central function of RAK4600.



## Description

This example is developed on the basis of standard firmware. The added code is in the comment range of `/* User code */` and `/ *End User code */` of "user_app.c".

* In this example, auxiliary test modules were used as Bluetooth devices.

* The RAK4600 can automatically scan the Bluetooth devices and only support the devices which are connected to the Nordic UART Service.

*  The RAK4600 can send the **"PING"** string to the auxiliary test modules and print their return messages.   
  
* Note that RAK4600 can only be connected with one auxiliary measurement module at the same time, vice versa.

* The sample code is set print logs every 3 seconds, and will disconnect automatically after printing the logs 15 times and then reconnects again.   

    User can modify the time interval depending on the requirement.

    Example:  **`uint32_t timer_interval = 5; //prints logs every 5 seconds`**