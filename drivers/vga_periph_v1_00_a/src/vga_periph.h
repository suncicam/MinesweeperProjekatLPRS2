/*****************************************************************************
* Filename:          D:\work\lprs2\2013_2014\Resene_vezbe\lab56\basic_system/drivers/vga_periph_v1_00_a/src/vga_periph.h
* Version:           1.00.a
* Description:       vga_periph Driver Header File
* Date:              Tue Mar 04 15:31:41 2014 (by Create and Import Peripheral Wizard)
*****************************************************************************/

#ifndef VGA_PERIPH_H
#define VGA_PERIPH_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xil_io.h"

/************************** Constant Definitions ***************************/


/**
 * User Logic Slave Space Offsets
 * -- SLV_REG0 : user logic slave module register 0
 */
#define VGA_PERIPH_USER_SLV_SPACE_OFFSET (0x00000000)
#define VGA_PERIPH_SLV_REG0_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000000)
#define VGA_PERIPH_SLV_REG1_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000004)
#define VGA_PERIPH_SLV_REG2_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000008)
#define VGA_PERIPH_SLV_REG3_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x0000000C)
#define VGA_PERIPH_SLV_REG4_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000010)
#define VGA_PERIPH_SLV_REG5_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000014)
#define VGA_PERIPH_SLV_REG6_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000018)
#define VGA_PERIPH_SLV_REG7_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x0000001C)
#define VGA_PERIPH_SLV_REG8_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000020)
#define VGA_PERIPH_SLV_REG9_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000024)
#define VGA_PERIPH_SLV_REG10_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000028)
#define VGA_PERIPH_SLV_REG11_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x0000002C)
#define VGA_PERIPH_SLV_REG12_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000030)
#define VGA_PERIPH_SLV_REG13_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000034)
#define VGA_PERIPH_SLV_REG14_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x00000038)
#define VGA_PERIPH_SLV_REG15_OFFSET (VGA_PERIPH_USER_SLV_SPACE_OFFSET + 0x0000003C)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a VGA_PERIPH register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the VGA_PERIPH device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void VGA_PERIPH_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define VGA_PERIPH_mWriteReg(BaseAddress, RegOffset, Data) \
 	Xil_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))

/**
 *
 * Read a value from a VGA_PERIPH register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the VGA_PERIPH device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 VGA_PERIPH_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define VGA_PERIPH_mReadReg(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (RegOffset))


/**
 *
 * Write/Read 32 bit value to/from VGA_PERIPH user logic slave registers.
 *
 * @param   BaseAddress is the base address of the VGA_PERIPH device.
 * @param   RegOffset is the offset from the slave register to write to or read from.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	void VGA_PERIPH_mWriteSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Value)
 * 	Xuint32 VGA_PERIPH_mReadSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define VGA_PERIPH_mWriteSlaveReg0(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG0_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg1(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG1_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg2(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG2_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg3(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG3_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg4(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG4_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg5(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG5_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg6(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG6_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg7(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG7_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg8(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG8_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg9(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG9_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg10(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG10_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg11(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG11_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg12(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG12_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg13(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG13_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg14(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG14_OFFSET) + (RegOffset), (Xuint32)(Value))
#define VGA_PERIPH_mWriteSlaveReg15(BaseAddress, RegOffset, Value) \
 	Xil_Out32((BaseAddress) + (VGA_PERIPH_SLV_REG15_OFFSET) + (RegOffset), (Xuint32)(Value))

#define VGA_PERIPH_mReadSlaveReg0(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG0_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg1(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG1_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg2(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG2_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg3(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG3_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg4(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG4_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg5(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG5_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg6(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG6_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg7(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG7_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg8(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG8_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg9(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG9_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg10(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG10_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg11(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG11_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg12(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG12_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg13(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG13_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg14(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG14_OFFSET) + (RegOffset))
#define VGA_PERIPH_mReadSlaveReg15(BaseAddress, RegOffset) \
 	Xil_In32((BaseAddress) + (VGA_PERIPH_SLV_REG15_OFFSET) + (RegOffset))

/************************** Function Prototypes ****************************/


/**
 *
 * Run a self-VGA_PERIPH on the driver/device. Note this may be a destructive VGA_PERIPH if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the VGA_PERIPH instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-VGA_PERIPH code passed
 *    - XST_FAILURE   if any self-VGA_PERIPH code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self VGA_PERIPH may fail if data memory and device are not on the same bus.
 *
 */
XStatus VGA_PERIPH_SelfVGA_PERIPH(void * baseaddr_p);
/**
*  Defines the number of registers available for read and write*/
#define VGA_PERIPH_AXI_LITE_USER_NUM_REG 16


#endif /** VGA_PERIPH_H */
