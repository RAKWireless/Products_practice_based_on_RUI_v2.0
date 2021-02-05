# Pulse Width Modulation (PWM)

## Overview

This example shows how to use Pulse Width Modulation (PWM) functionality of a module



## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */` and `/* End User Code */`. 

Basically, these are the parameters you need to change：

* **pwm_ping.frequency**

    Frequency range is 50KHZ ~ 500KHZ

    Example: Changing frequency to 500KHZ  
    
    `pwm_ping.frequency = 500000; //in hertz`
    
* **pwm_ping.dutycycle**  
  
    Duty cycle range is 0% ~ 100%

    Example: Changing the duty cycle to 25%

    `pwm_ping.dutycycle = 25;`

Once the firmware is running, the user can measure the actual frequency and duty cycle. This can be done by connecting the probe of an oscilloscope to pin #2 of RAK811 module.





