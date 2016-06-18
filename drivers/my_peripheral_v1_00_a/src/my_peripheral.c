/*****************************************************************************
* Filename:          D:\users\ra123_2013\basic_system/drivers/my_peripheral_v1_00_a/src/my_peripheral.c
* Version:           1.00.a
* Description:       my_peripheral Driver Source File
* Date:              Tue Mar 04 14:14:01 2014 (by Create and Import Peripheral Wizard)
*****************************************************************************/


/***************************** Include Files *******************************/

#include "my_peripheral.h"

/************************** Function Definitions ***************************/

void MY_PERIPHERAL_ClearScreen(){
	xil_printf("%c[2J",27);
}

