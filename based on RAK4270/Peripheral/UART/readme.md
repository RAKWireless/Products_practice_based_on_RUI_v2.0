# UART

## Overview

This example shows how a node can send and receive data through Universal Asynchronous Receiver-Transmitter (UART).




## Description

This example is developed on the basis of standard firmware. The added code is in the comment range of `/* User Code */` and `/* End User Code */`. In general, you only need to modify the following two parameters to achieve the desired function.

1. **#define UART_BUFFER_SIZE   128**

    Buffer size can be changed according to the project. However, the maximum buffer size is 128 bytes.

2. **ret_code = rui_uart_init(RUI_UART3, BAUDRATE_115200)**  

    Baud rate can be changed from 9600 to 115200.

Detailed introduction：

1. Data can be sent to the module ending with "\r\n", and it will return the same data. The maximum length of test data is 128 bytes as defined (including "\r\n").

   **Case 1:** (128 bytes)

   Send：
   at+send=uart:3:0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678\r\n                         

   Receive：
   at+send=uart:3:0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678\r\n     
   
   *Changing the baud rate or reception time affects the received data. You can experiment with different values to achieve preferred result*

   Send：
   at+send=uart:3:0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678\r\n.  

   Receive：<br>
   at+send=uart:3:01234567890123456<br>
   78901234567890123456789012345678<br>
   90123456789012345678901234567890<br>
   1234567890123456789012345678\r\n.    

   **Case 2:** (81 bytes)

   Send:<br>
   012345678901234567890123456789012345678901234567890123456789012345678901234567890

   Receive:<br>
   012345678901234567890123456789012345678901234567890123456789012345678901234567890

2. Serial port configuration can be modified.

    Example: N81 mode<br>
    (Parity = N (None), Data bit = 8, Stop bit = 1)

3. Serial port of the working mode can be modified.

     RAK4270: uart2 (pin 2: tx2, pin 1: rx2) 
     
     <img src="../../../assets/rui/RAK4270 .png" alt="schematic" style="max-width:100%;">
