# LoRaWAN Join and Send data

## Overview

The example uses the API provided by RUI to automatically run, join and send data operations at regular intervals.



## Description

The example is developed on the basis of standard firmware, and the added code is in the comment range of `/* User Code */`, `/* End User Code */`. The current example only shows basic data transmission, without considering larger data sizes. If you want to send more data, you can refer to the example of packet sending.

In the example, it will automatically Join after power on. If joining was successful, it will begin to periodically send data, sleep, and wake up. Customers can modify the code based on this example to meet their own needs.

The two key functions are `setup()` and `loop()`. The `setup()` function is only executed once during initialization. In the example, the timer is initialized. The `loop()` function will be executed repeatedly. In addition, there are two callback functions that handle the execution results of join and data sending.



In this example, you may only need to focus on these two parameters:

**1. Time Interval**

In the code, modify the values of `join_interval` and `send_interval` to change the interval between Join and data sending, in seconds.

If Join fails, it will automatically go to sleep, wake up within a certain period of time, and continue the Join operation. After the data is sent, the module will automatically go to sleep.

**2. Data Transmission**

In the `loop()` function, the data transmission interface is `rui_lora_send()`. The variables corresponding to data port, buffer and data length are "AppPort`, ʻAppData[]`, ʻAppLen`.

Before sending data, update the data and length to be sent to the corresponding variables.

