##############################################################################
## Filename:          D:\work\lprs2\2013_2014\Resene_vezbe\lab56\basic_system/drivers/vga_periph_mem_v1_00_a/data/vga_periph_mem_v2_1_0.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Wed Mar 05 10:25:21 2014 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "vga_periph_mem" "NUM_INSTANCES" "DEVICE_ID" "C_S_AXI_MEM0_BASEADDR" "C_S_AXI_MEM0_HIGHADDR" 
}
