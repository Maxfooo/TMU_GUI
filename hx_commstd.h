/*
 * File:   HX_CommStd.h
 * Author: maxr
 *
 * Created on July 12, 2017, 2:30 PM
 */

#ifndef HX_COMMSTD_H
#define	HX_COMMSTD_H

/********** FROM PC interface cmds **********/
#define HX_PC_DATA_WRITE            0x00
#define HX_PC_STATUS_REQUEST        0x01
#define HX_PC_INITIATE_MAIN_CODE    0x02
#define HX_PC_ACK_COMPLETE          0x03
#define HX_PC_FLUSH_INPUT_BUFFER    0x04
#define HX_PC_FLUSH_OUTPUT_BUFFER   0x05
#define HX_PC_REQUEST_CHIP_ID       0x06
#define HX_PC_REQUEST_INPUT_BUFFER  0x07
#define HX_PC_REQUEST_DEBUG_VAR     0x08
#define HX_PC_REQUEST_PREVIOUS_PKT  0x09

// -- Packet commands
#define HX_PKT_SPP_H        0x00
#define HX_PKT_SPP_C        0x01
#define HX_PKT_SPP_GENERAL  0x02
#define HX_PKT_I2C          0x10
#define HX_PKT_SPI          0x20
#define HX_PKT_ONE_WIRE     0x30
#define HX_PKT_ASL          0x31
#define HX_PKT_SPP_C_TEST   0xF0

// -- DUT Status Returns
#define HX_DUT_NACK 0x00
#define HX_DUT_ACK 0x01
#define HX_DUT_COMPLETE 0x10
#define HX_DUT_BUSY 0x11
#define HX_DUT_UNDEFINED 0xFF

////////////////////
// HX_I2C PACKET STRUCTURE
// <Mode>
// <Slave Address>
// <# of Tx Bytes>
// <# of Rx Bytes>
// <Byte0, Byte1, Byte2, ...>
// <Checksum>
////////////////////

// I2C Modes
#define HX_I2C_WRITE_MODE 0x00
#define HX_I2C_WR_RD_MODE 0x10
#define HX_I2C_READ_MODE  0x20
#define HX_I2C_DEBUG_MODE0 0x30
#define HX_I2C_DEBUG_MODE1 0x31
#define HX_I2C_DEBUG_MODE2 0x32

// -- SPP Command Masks
//MS Mode Masks
#define SPP_MODE_MASK               0xF0
#define SPP_WRITE_TO_REG            0x00
#define SPP_READ_FROM_REG           0x10
#define SPP_WRITE_TO_OTP            0x20
#define SPP_READ_FROM_OTP           0x30
#define SPP_READ_CHIP_ID            0x40
#define SPP_SELECT_ACT              0x50
#define SPP_CHECK_STATE             0x60
//LS Mode Masks
#define SPP_NULL                    0x00
#define SPP_MEM0                    0x00
#define SPP_MEM1                    0x01
#define SPP_MEM2                    0x02
#define SPP_MEM3                    0x03
#define SPP_MEM4                    0x04
#define SPP_MEM5                    0x05
#define SPP_ACT_OTP                 0x00
#define SPP_ACT_REG                 0x01
/********** TO PC interface cmds **********/
// --- General Returns
#define HX_UC_NACK 0x00
#define HX_UC_ACK 0x01
#define HX_UC_CMD_UNRECOGNIZED 0xFF

// --- Status returns
#define HX_UC_COMPLETE 0x10
#define HX_UC_BUSY 0x11

#define HX_UC_ERROR_PC_CHECKSUM 0x20
#define HX_UC_ERROR_PC_COMM 0x21
#define HX_UC_ERROR_PC_TIMEOUT 0x22

#define HX_PC_ERROR_UC_CHECKSUM 0x30
#define HX_PC_ERROR_UC_COMM 0x31
#define HX_PC_ERROR_UC_TIMEOUT 0x32

#define HX_UC_ERROR_INTERNAL_PKT 0x40
#define HX_UC_ERROR_DUT_COMM 0x41
#define HX_UC_ERROR_MAIN_TIMEOUT 0x42
#define HX_UC_ERROR_UNKNOWN 0x43

// --- Status Masks
#define HX_STATUS_MASK 0xF0
#define HX_PC_TO_UC_ERROR 0x20
#define HX_UC_TO_PC_ERROR 0x30
#define HX_UC_TO_DUT_ERROR 0x40


#endif	/* HX_COMMSTD_H */

