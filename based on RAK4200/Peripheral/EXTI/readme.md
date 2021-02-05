# GPIO External Interrupt

## Overview

This example shows the General Purpose Input-Output (GPIO) external interrupt. When the RAK4200 node receives a rising edge or falling edge signal, the corresponding log will be printed.




## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */`, `/* End User Code */`

  1. The pin is set to double-edge detection, which can accurately detect the edge signal and print the corresponding log description.
  
        For example, the auxiliary test module generates a 1-second interval square wave signal. The RAK4200 node will print the edge signal it received, whether it is a rising edge or falling edge signal.

  2. The pins that control the GPIO can be modified. 
  
        In this example, you only need to modify the declaration "gpio_input.pin_num = 3", which specify the pin of RAK4200 to be used. These are the configurable pins for RAK4200: 3,6,9,10
        
        Please refer to the hardware schematics:
        
        <img src="../../../assets/rui/RAK4200.png" alt="schematics" style="max-width:100%;">
        



