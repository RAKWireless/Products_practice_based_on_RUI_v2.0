# LoRa_P2P_Send

## Overview

The example shows how LoRa P2P data is sent periodically. <br>
**Note: When using RAK4200-H module, SF10 only supports sending 128-byte packets**



## Description

The example is developed on the basis of standard firmware, and the added code is in the comment range of `/* User Code */`, `/* End User Code */`. 

The example implements the function of sending LoRa P2P data periodically, and becomes idle until data is ready to be sent again.

**Mode: Set to LORa P2P sending mode**

Example：**ret_code = rui_lorap2p_set_work_mode(2)**

**Periodicity: Sets the sending period（The unit is seconds)**

Example：**uint32_t send_interval = 10**

The following parameters need to be configured in the send content：

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

Sets the tx power. <br>
Range: 0 - 20 dBm

Example：**uint8_t Powerdbm = 20**

**7. LoRa Data**

Sets the data length. <br>
Range: 1 - 254

Example：**AppLen = 254**

