# Flash

## Overview

This example shows how to write and read flash to a node.




## Description

The example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */` and `/* End User Code */`. 

* The flash contains the data "000 000102030405060708090 a0b0cxxxxxxxx7d7e7f" by default, with total length of 128 bytes.

* Data on the flash can be modified with the following steps:

    1. Uncomment `#define FLASH_WRITE_TEST`
    2. Declare initial size of buffer
    
        Example: Setting the initial size to 128 bytes
        
        `uint_t flash_buf[128] = {0}`
        
    3. Declare the actual size of data to be written
    
        Example: Setting the actual size to 128 bytes
        
        `uint16_t flash_len = 128`  
  
    Note: Data size can be changed. For example, we want the size to be 1024 bytes:  
    `uint_t flash_buf[1024] = {0}`<br>
    `uint16_t flash_len = 1024`
    
    Since `FLASH_WRITE_TEST` was defined, code inside `#ifdef FLASH_WRITE_TEST` will be executed which writes data into the flash


* After the node is powered on, press the restart key and open the serial port tool to see the result logs
