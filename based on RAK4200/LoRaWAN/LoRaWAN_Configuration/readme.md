# LoRaWAN Configuration

## Overview

This example shows how to configure the LoRaWAN parameters of the module by calling the API, including region, class, join_mode, dr, etc.



## Description

The example is developed on the basis of standard firmware, and the added code is in the comment range of `/* User Code */`, `/* End User Code */`.

The key function in the example is `setup()`, which is called during initialization. LoRaWAN parameters are set in the function, and these parameters are described below:

**1. LoRa Work Mode**

   Set LoRa working mode.<br>
   Modes: LoRaWAN, LoRa P2P
   
   In the example, we set it to LoRaWAN
   
   **rui_lora_set_work_mode()**

**2. Region**

   Set the Region. <br>
   Supported Regions: AS923, AU915, CN470, EU433, EU868, KR920, IN865, US915.

   **rui_lora_set_region()**

**3. Join Mode**

   Set LoRaWAN Join mode.<br>
   Modes: OTAA, ABP
   
   The example always defaults to the OTAA mode, and you can comment the ʻUSER_LORAWAN_OTAA` macro definition to switch to the ABP mode. <br>
   OTAA mode needs to set dev_eui, app_eui, app_key. ABP mode needs to set dev_addr, apps_key, nwks_key.
   
   **rui_lora_set_dev_eui(), rui_lora_set_app_eui(), rui_lora_set_app_key()** <br>
   **rui_lora_set_dev_addr(), rui_lora_set_apps_key(), rui_lora_set_nwks_key()**

**4. Class**

   Set LoRaWAN Class.<br>
   Classes: ClassA, ClassC
   
   The example is set to ClassA.

   **rui_lora_set_class()**

**5. Packet Type**

   Set the data packet type: Confirmed packet and Unconfirmed Packet. 
   
   The example is set to unconfirmed package.

  **rui_lora_set_confirm()**

**6. Data Rate**

   Set the data rate. <br>
   The data rate range is different in different regions, please refer to the `LoRaWAN Regional Parameters v1.0.2` document according to your region.

   **rui_lora_set_dr()**

**7. Transmission (TX) Power**

   Set the transmission power. <br>
   The transmit power range in different regions is different, please refer to the `LoRaWAN Regional Parameters v1.0.2` document.

  **rui_lora_set_tx_power()**

**8. Adaptive Data Rate (ADR)**

   Enable/disable adaptive data rate. 
   
   Set to disable in the example.

  **rui_lora_set_adr()**

