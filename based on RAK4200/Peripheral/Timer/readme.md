# TIMER

## Overview

This example shows how to add a timer functionality.



## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */`, `/* End User Code */`

In the example, it demonstrates how to add timer function to the module. Logs are automatically printed at defined timer interval.

1. The timer interval can be modified. However, the interval cannot be less than 1 second.

    Example: **uint32_t timer_interval = 60** <br>
    The timer interval was set to 60 seconds

2. Maximum of five timers can be used.

3. The timer resets once the module is restarted.

