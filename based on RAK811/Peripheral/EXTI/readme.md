# GPIO External Interrupt

## Overview

This example shows the General Purpose Input-Output (GPIO) external interrupt. When the RAK811 node receives a rising edge or falling edge signal, the corresponding log will be printed.




## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */`, `/* End User Code */`

  1. The pin is set to double-edge detection, which can accurately detect the edge signal and print the corresponding log description.
  
        For example, the auxiliary test module generates a 1-second interval square wave signal. The RAK811 node will print the edge signal it received, whether it is a rising edge or falling edge signal.

  2. The pins that control the GPIO can be modified. 
  
        In this example, you only need to modify the declaration "gpio_input.pin_num = 2", which specify the pin of RAK811 to be used. These are the configurable pins for RAK811: 2,3,4,5,8,9,14,15,16,18,19,20,22,23
        
        Please refer to the hardware schematics:  
  
<img src="../../../assets/rui/RAK811.png" alt="schematics" style="max-width:100%;">


**Note:**

In external interrupt applications, some pins cannot be used at the same time.<br>
For RAK811-H: pin 2 (PB12) and pin 8 (PA12); pin 4 (PB15) and pin 14 (PA15); pin 5 (PA8) and pin 18 (PB8)

