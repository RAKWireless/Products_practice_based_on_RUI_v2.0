# Analog to Digital Converter (ADC)

## Overview

This example shows how to use the Analog to Digital Converter (ADC) functionality of a module



## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */` and `/* End User Code */`. 

This demonstrates how to assign a pin to enable ADC functionality. Once it was successfully initialized, the voltage value at the set pin will be printed to the serial port.

* Assigning a pin to enable ADC function  
  
  Example: Using Pin #4

    `gpio_adc.pin_num = 4 \\Pin #4 ADC enabled`

* Based on testing, difference between the actual voltage and the printed voltage to the serial port is within 50mV (for RAK811 module)

Pin support for different modules:

**for RAK811(L)： 2，3，4，5，15，20，22，23**<br>
**for RAK811(H)： 2，3，4，20，22，23**
