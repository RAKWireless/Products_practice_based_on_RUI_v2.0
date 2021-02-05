# LoRaWAN Downlink

## Overview

The example shows how to process the data sent by the gateway to the node.



## Description

The example is developed on the basis of standard firmware, and the added code is in the comment range of `/* User Code */`, `/* End User Code */`. 

In LoRa applications, in addition to sending data, nodes will also receive data from the gateway. It can be understood from the LoRaWAN protocol that in Class A mode, after the node sends data to the gateway, it can receive the data issued by the gateway in the receiving window. In Class C mode, the gateway can send data at any time when the node is idle.

In this example, the data obtained and processed by the gateway is shown. The data sent by the gateway can be obtained in the callback function of `LoRaReceive_callback()`. The data buffer is `Receive_datapackage->Buffer`, and the data length is `Receive_datapackage->BufferSize`.

In this example, the node will check the content of the data sent by the gateway each time. If the string in the content is `restart`, it will send the same string to the gateway and restart the device. If you want to add custom data processing, it is recommended that you add it to the ʻuser_downlink_process()` function.
