# AT-COMMAND

## Overview

This example shows how to implement and use a custom AT command.



## Description

This example is developed on the basis of standard firmware. The added code is in the comment range of `/* User code */` and `/ *End User code */` of "at_cmd.c".

Users can add custom commands based on the standard format. The command header format is "at+" and must end with "\r\n". The overall format of a custom AT command should be "at+XXXX=ABCD\r\n", where "XXXX" and "ABCD" are the parts that will be modified.

An AT command is up to 256 bytes in length (including "\r\n").

Creating a custom AT Command

* In the example, a function named "atcmd_custom()" can be found at the "at_cmd.c" and "at_cmd.h" source files. You can use this as a basis to create your very own custom AT command

* Note that there is a struct definition at "at_cmd.c" which defines the "custom" keyword and "atcmd_custom" function call. User must define their additional commands on the struct

* Considering the "at+XXXX=ABCD\r\n" standard format, "XXXX" is replaced with "custom" and "ABCD" is the returned value 
  
  Example Command: "at+custom=1234\r\n"<br>
  
  When the modules receives this command, it will return "1234"
