# RUI Open Source
![RAKwireless](assets/rui/RAK-Whirls.png)

RUI open source examples

## Introduction

This repository collects open source examples of the RAKWireless LoRa module. It includes the application layer code of the module's standard firmware, and examples of using the RUI API based on the standard firmware. Customers can choose the corresponding sample code according to module models and application requirements. Hence, facilitate customer functional prototype verification and improve development efficiency.



## Modules and examples

The examples supported by each module are shown below:

|                 Example                  |                           RAK811                            |                           RAK4200                            |                           RAK4270                            |                           RAK4600                            |
| :--------------------------------------: | :---------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|          LoRaWAN configuration           | [Done](/based%20on%20RAK811/LoRaWAN/LoRaWAN_Configuration)  | [Done](/based%20on%20RAK4200/LoRaWAN/LoRaWAN_Configuration)  | [Done](/based%20on%20RAK4270/LoRaWAN/LoRaWAN_Configuration)  | [Done](/based%20on%20RAK4600/LoRaWAN/LoRaWAN_Configuration)  |
| LoRaWAN automatically join and send data | [Done](/based%20on%20RAK811/LoRaWAN/LoRaWAN_Auto_Join_Send) | [Done](/based%20on%20RAK4200/LoRaWAN/LoRaWAN_Auto_Join_Send) | [Done](/based%20on%20RAK4270/LoRaWAN/LoRaWAN_Auto_Join_Send) | [Done](/based%20on%20RAK4600/LoRaWAN/LoRaWAN_Auto_Join_Send) |
|             LoRaWAN downlink             |    [Done](/based%20on%20RAK811/LoRaWAN/LoRaWAN_Downlink)    |    [Done](/based%20on%20RAK4200/LoRaWAN/LoRaWAN_Downlink)    |    [Done](/based%20on%20RAK4270/LoRaWAN/LoRaWAN_Downlink)|    [Done](/based%20on%20RAK4600/LoRaWAN/LoRaWAN_Downlink)    |
|           LoRaWAN long packet            |  [Done](/based%20on%20RAK811/LoRaWAN/LoRaWAN_Long_Packet)    |   [Done](/based%20on%20RAK4200/LoRaWAN/LoRaWAN_Long_Packet)  | [Done](/based%20on%20RAK4270/LoRaWAN/LoRaWAN_Long_Packet)    |   [Done](/based%20on%20RAK4600/LoRaWAN/LoRaWAN_Long_Packet)                                             |         
|              LoRa P2P Send               |  [Done](/based%20on%20RAK811/LoRa%20P2P/LoRa_P2P_Send)         |     [Done](/based%20on%20RAK4200/LoRa%20P2P/LoRa_P2P_Send)     |        [Done](/based%20on%20RAK4270/LoRa%20P2P/LoRa_P2P_Send)  |     [Done](/based%20on%20RAK4600/LoRa%20P2P/LoRa_P2P_Send)                                                   |
|              LoRa P2P Recv               |      [Done](/based%20on%20RAK811/LoRa%20P2P/LoRa_P2P_Recv)     |        [Done](/based%20on%20RAK4200/LoRa%20P2P/LoRa_P2P_Recv)|     [Done](/based%20on%20RAK4270/LoRa%20P2P/LoRa_P2P_Recv)  |        [Done](/based%20on%20RAK4600/LoRa%20P2P/LoRa_P2P_Recv)                                     |
|              LoRa P2P Send_Recv              |      [Done](/based%20on%20RAK811/LoRa%20P2P/LoRa_P2P_Send_Recv)    |       [Done](/based%20on%20RAK4200/LoRa%20P2P/LoRa_P2P_Send_Recv) |    [Done](/based%20on%20RAK4270/LoRa%20P2P/LoRa_P2P_Send_Recv)   |      [Done](/based%20on%20RAK4600/LoRa%20P2P/LoRa_P2P_Send_Recv)                                      |
|             UART                         |       [Done](/based%20on%20RAK811/Peripheral/UART)             |         [Done](/based%20on%20RAK4200/Peripheral/UART)  |[Done](/based%20on%20RAK4270/Peripheral/UART)           |       N/A        |
|             GPIO Input/output             |     [Done](/based%20on%20RAK811/Peripheral/GPIO/)         |   [Done](/based%20on%20RAK4200/Peripheral/GPIO/)     |    [Done](/based%20on%20RAK4270/Peripheral/GPIO/)             |       [Done](/based%20on%20RAK4600/Peripheral/GPIO/)                                   |
|             GPIO Interrupt                  |   [Done](/based%20on%20RAK811/Peripheral/EXTI)         | [Done](/based%20on%20RAK4200/Peripheral/EXTI)     |   [Done](/based%20on%20RAK4270/Peripheral/EXTI)          |  [Done](/based%20on%20RAK4600/Peripheral/EXTI)      |
|            Timer                     |     [Done](/based%20on%20RAK811/Peripheral/Timer/)          |   [Done](/based%20on%20RAK4200/Peripheral/Timer/)      |    [Done](/based%20on%20RAK4270/Peripheral/Timer/)        |     [Done](/based%20on%20RAK4600/Peripheral/Timer/)   |
|            I2C                     |  [Done](/based%20on%20RAK811/Peripheral/I2C)                            |   [Done](/based%20on%20RAK4200/Peripheral/I2C)        |      [Done](/based%20on%20RAK4270/Peripheral/I2C)            |      [Done](/based%20on%20RAK4600/Peripheral/I2C)                                     |
|            FLASH                     |  [Done](/based%20on%20RAK811/Peripheral/Flash)                            |   [Done](/based%20on%20RAK4200/Peripheral/Flash)         |      [Done](/based%20on%20RAK4270/Peripheral/Flash)              |     [Done](/based%20on%20RAK4600/Peripheral/Flash)                                    |
|            ADC                     |  [Done](/based%20on%20RAK811/Peripheral/ADC)                            |   [Done](/based%20on%20RAK4200/Peripheral/ADC)        |      [Done](/based%20on%20RAK4270/Peripheral/ADC)            |     N/A                                   |
|            PWM                     |  [Done](/based%20on%20RAK811/Peripheral/PWM)                            |  N/A        |     N/A           |     N/A                                   |
|            AT_Command                     |  [Done](/based%20on%20RAK811/Peripheral/AT_Command)                            |   [Done](/based%20on%20RAK4200/Peripheral/AT_Command)    |  [Done](/based%20on%20RAK4270/Peripheral/AT_Command)   |    [Done](/based%20on%20RAK4600/Peripheral/AT_Command) |                           
|            BLE-Scanner                    |  N/A                         |  N/A        |     N/A           |     [Done](/based%20on%20RAK4600/BLE/BLE_Scanner)             |    
|            BLE-Central                    |  N/A                         |  N/A        |     N/A           |     [Done](/based%20on%20RAK4600/BLE/BLE_Central)             |   
## How to use

This repository is open to application layer code, which needs to be compiled using the online compilation environment provided by RAKWireless.

The URL of the online compilation environment is [https://build.rakwireless.com](https://build.rakwireless.com). <br>
Please refer to the following document for the method of compilation and firmware update: [https://docs.rakwireless.com/RUI/](https://docs.rakwireless.com/RUI/) 

**Please note: The `rui.h` file in the common folder must be packaged and compiled together with the source code file.**

