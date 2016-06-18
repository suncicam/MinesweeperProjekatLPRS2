/*****************************************************************************
* Filename:          D:\work\lprs2\2013_2014\Resene_vezbe\lab56\basic_system/drivers/vga_periph_mem_v1_00_a/src/vga_periph_mem.h
* Version:           1.00.a
* Description:       vga_periph_mem Driver Header File
* Date:              Wed Mar 05 10:25:21 2014 (by Create and Import Peripheral Wizard)
*****************************************************************************/

#ifndef VGA_PERIPH_MEM_H
#define VGA_PERIPH_MEM_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/

#define GRAPHICS_MEM_OFF 0x02000000
#define TEXT_MEM_OFF 0x1000000


/**************************** Type Definitions *****************************/


/**
 *
 * Write/Read 32 bit value to/from VGA_PERIPH_MEM user logic memory (BRAM).
 *
 * @param   Address is the memory address of the VGA_PERIPH_MEM device.
 * @param   Data is the value written to user logic memory.
 *
 * @return  The data from the user logic memory.
 *
 * @note
 * C-style signature:
 * 	void VGA_PERIPH_MEM_mWriteMemory(Xuint32 Address, Xuint32 Data)
 * 	Xuint32 VGA_PERIPH_MEM_mReadMemory(Xuint32 Address)
 *
 */
#define VGA_PERIPH_MEM_mWriteMemory(Address, Data) \
 	Xil_Out32(Address, (Xuint32)(Data))
#define VGA_PERIPH_MEM_mReadMemory(Address) \
 	Xil_In32(Address)

/************************** Function Prototypes ****************************/


/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the VGA_PERIPH_MEM instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus VGA_PERIPH_MEM_SelfTest(void * baseaddr_p);

void set_cursor(Xuint32 new_value);

void clear_graphics_screen(Xuint32 BaseAddress);
void clear_text_screen(Xuint32 BaseAddress);

void draw_square(Xuint32 BaseAddress);
void print_string(Xuint32 BaseAddress, unsigned char string_s[], int lenght);

#endif /** VGA_PERIPH_MEM_H */
