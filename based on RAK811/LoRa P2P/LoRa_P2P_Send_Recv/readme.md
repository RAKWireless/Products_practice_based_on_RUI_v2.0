# LoRa_P2P_Send_Recv

## Overview

This example shows the receiving and sending of LoRa P2P data .



## Description

The example is developed on the basis of standard firmware, and the added code is in the comment range of `/* User Code */`, `/* End User Code */`. 

1. After power on, the module will be automatically configured into P2P mode.

2. By default, the module P2P mode is receiving and can receive data.

3. After receiving the command "at+send=lorap2p:xx", the module will automatically switch to send mode and send data out.After the data is sent, the module returns to the received state.

The following parameters can be modified as you wish.

**1. Frequency**

Sets the channel frequency. <br>
Range: 150000000HZ - 960000000HZ （150MHz - 960MHz）

Example：**uint32_t Frequency = 868100000**

**2. Spreading Factor**

Sets the spreading factor. <br>
Range: 7 - 12

Example：**uint8_t Spreadfact = 7**

**3. Bandwidth**

Sets the bandwidth. <br>
Frequencies: 0: 125 kHz, 1: 250 kHz, 2: 500 kHz

Example：**uint8_t Bandwidth = 0**

**4. Coding Rate**

Sets the coding rate. <br>
Options: 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8

Example：**uint8_t Codingrate = 1**

**5. Preamble Length**

Sets the preamble length. <br>
Range: 5 - 65535

Example：**uint16_t Preamlen = 8**

**6. TX (Transmission) Power**

Sets the TX (Transmission Power). <br>
Range: 0 - 20 dBm

Example：**uint8_t Powerdbm = 20**

