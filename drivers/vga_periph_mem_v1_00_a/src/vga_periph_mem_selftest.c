/*****************************************************************************
* Filename:          D:\work\lprs2\2013_2014\Resene_vezbe\lab56\basic_system/drivers/vga_periph_mem_v1_00_a/src/vga_periph_mem_selftest.c
* Version:           1.00.a
* Description:       Contains a diagnostic self-test function for the vga_periph_mem driver
* Date:              Wed Mar 05 10:25:21 2014 (by Create and Import Peripheral Wizard)
*****************************************************************************/


/***************************** Include Files *******************************/

#include "vga_periph_mem.h"
#include "xparameters.h"
#include "stdio.h"
#include "xio.h"

/************************** Constant Definitions ***************************/


/************************** Variable Definitions ****************************/


/************************** Function Definitions ***************************/

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
XStatus VGA_PERIPH_MEM_SelfTest(void * baseaddr_p)
{
  int     Index;
  Xuint32 baseaddr;
  Xuint32 Mem32Value;

  /*
   * Check and get the device address
   */
  /*
   * Base Address maybe 0. Up to developer to uncomment line below.
  XASSERT_NONVOID(baseaddr_p != XNULL);
   */
  baseaddr = (Xuint32) baseaddr_p;

  xil_printf("******************************\n\r");
  xil_printf("* User Peripheral Self Test\n\r");
  xil_printf("******************************\n\n\r");

  /*
   * Write data to user logic BRAMs and read back
   */
  xil_printf("User logic memory test...\n\r");
  xil_printf("   - local memory address is 0x%08x\n\r", baseaddr);
  xil_printf("   - write pattern to local BRAM and read back\n\r");
  for ( Index = 0; Index < 256; Index++ )
  {
    VGA_PERIPH_MEM_mWriteMemory(baseaddr+4*Index, (0xDEADBEEF % Index));
  }

  for ( Index = 0; Index < 256; Index++ )
  {
    Mem32Value = VGA_PERIPH_MEM_mReadMemory(baseaddr+4*Index);
    if ( Mem32Value != (0xDEADBEEF % Index) )
    {
      xil_printf("   - write/read memory failed on address 0x%08x\n\r", baseaddr+4*Index);
      return XST_FAILURE;
    }
  }
  xil_printf("   - write/read memory passed\n\n\r");

  return XST_SUCCESS;
}
