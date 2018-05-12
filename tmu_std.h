#ifndef TMU_STD_H
#define TMU_STD_H

#define SET_INTERNAL_HEATER 1
#define SET_EXTERNAL_HEATER 0

#define SET_NMOS 1
#define SET_NPN 0

#define SET_BIT_16 1
#define SET_BIT_8 0

#define NUM_OF_GAIN_RES 8
#define NUM_OF_RANGE_RES 8
#define NUM_OF_MUX_CHANNELS 8

#define C_TO_K 273.15

// --------------- Packet info ----------------
#define NUM_OF_LATCHES 5
#define SIZE_OF_TX_LATCH_PKT 5
#define SIZE_OF_RX_LATCH_PKT 3

#define CMD_INDEX 0
#define VAL_HI_INDEX 1
#define VAL_LO_INDEX 2
#define TX_LAT_NUM_INDEX 3
#define RX_LAT_NUM_INDEX 1
#define TX_CS_INDEX 4
#define RX_CS_INDEX 2
// --------------------------------------------

// --------------- I2C info ----------------
#define I2C_ADDRESS 0x5A
#define I2C_NUM_OF_WR_RX 3 // when writing data, always get 3 bytes back

// --------------------------------------------

// ------------ Current limit info ------------
#define NUM_INT_HEATING_UNITS 2
#define NUM_INT_HEATER_LEGS 4

#define CURR_LIMIT_DAC_VOLT_MAX 2.9
#define CURR_LIMIT_DAC_CODE_MAX 127
#define CURR_LIMIT_DAC_LSB (CURR_LIMIT_DAC_VOLT_MAX / (CURR_LIMIT_DAC_CODE_MAX+1))
// mV delta between bits
// must use # of steps (128) instead of max value (127)

#define CURR_LIMIT_PER_UNIT 172 // Nominal milli-Amps
#define CURR_LIMIT_CHIP_MAX (NUM_INT_HEATING_UNITS*CURR_LIMIT_PER_UNIT)
#define CURR_LIMIT_NOMINAL_LEG_RES 2 // ohms

#define CURR_LIMIT_V_TO_MA_SLOPE 0.004 // (0.05V / 12.5mA) -> Using table in datasheet

#define EXT_RES_VAL_MIN 1
// --------------------------------------------

// ------------- Temperature info -------------
#define TEMP_DAC_VOLT_MAX 2.9
#define TEMP_DAC_CODE_MAX 4095
#define TEMP_DAC_VOLT_LSB (TEMP_DAC_VOLT_MAX / (TEMP_DAC_CODE_MAX+1))
#define TEMP_CELSIUS_MAX 125
#define TEMP_CELSIUS_MIN 25
#define TEMP_CELSIUS_RANGE (TEMP_CELSIUS_MAX-TEMP_CELSIUS_MIN)
#define TEMP_CELSIUS_LSB (TEMP_CELSIUS_RANGE / (TEMP_DAC_CODE_MAX+1))

// Using parameters from "OCXO ASIC LLDR revE.docx"
#define THERM_NOM_BETA 4250
#define THERM_NOM_RES_REF 100000
#define THERM_NOM_SET_TEMP 91
// Feedback Res = 200Ohms; Coarse Set R = 6kOhms
// --------------------------------------------


// ---------- Temperature Cycle info ----------
#define PIECEWISE_STEP_COUNT 20
#define DAEMON_CHECK_PERIOD 500 // ms (= 0.5 second)
#define TEMP_CYCLE_CHECK_PERIOD (2*DAEMON_CHECK_PERIOD)
#define SAWTOOTH_PER_MIN 30
#define PIECEWISE_TIME_STEP_MIN 1
// --------------------------------------------

// ------------- LATCH ADDRESSES AND NUMBERS --------------
#define ANALOG_0_ADDR  0xF8 // NMOS/NPN select
#define OVEN_CTLR_ADDR 0xF9 // enable, resistors, internal/external, current limit
#define HEAT_ADDR      0xFA // temperature control
#define BIAS_ADDR      0xFD // Reference voltage
#define CMOS_OUT_ADDR  0xE3 // EN polarity, Resistor sheet measure
#define ADC_MUX_ADDR   0xFB

#define ANALOG_0_NUM  0x03
#define OVEN_CTLR_NUM 0x04
#define HEAT_NUM      0x05
#define BIAS_NUM      0x07
#define CMOS_OUT_NUM  0x08
#define ADC_MUX_NUM   0x0A
// --------------------------------------------

// ------------- SFR ADDRESSES AND NUMBERS --------------
#define ASIC_REV_ADDR 0xE7
#define ASIC_REV_NUM 0x10
// --------------------------------------------

// --------------- TMU COMMANDS ---------------
// Boron Initial Programming Document page 16
#define CMD_REPORT_ASIC_ID 0x01
// TX: cmd, checksum
// RX:
#define CMD_WRITE_OTP 0x30
// TX: cmd, addLow, addHi, numBytes, data, checksum
// RX: (cmd or 0xFF), (0x00 or ERR), checksum
#define CMD_READ_OTP 0x20
// TX: cmd, addLo, addHi, numBytes, checksum
// RX: cmd, DATA, checksum
#define CMD_WRITE_LATCH 0x50
// TX: cmd, dataHi, dataLo, address(latch #), checksum
// RX: (cmd or 0xFF), (0x00 or ERR), checksum
#define CMD_READ_LATCH 0x51
// TX: cmd, address(latch#), checksum
// RX: cmd, dataHi, dataLo, address(latch#), checksum
#define CMD_READ_SFR 0x3F
// TX: cmd, Mapped-SFR-Address, checksum
// RX: cmd, DATA, checksum
#define CMD_MUX_RD_TX 0x52
// TX: cmd, Mux location, mux delay, checksum
// RX: (cmd or 0xFF), (0x00 or ERR), checksum
#define CMD_MUX_RD_RX 0x53
// TX: cmd, Mux location, checksum
// RX: cmd, dataLo, dataHi, Mux location, checksum

#define CMD_WR_RAM 0x32
// TX: cmd, addr_lo, addr_hi, num_bytes, DATA, checksum
// RX: cmd, 0x00, checksum
#define CMD_RD_RAM 0x22
// TX: cmd, addr_lo, addr_hi, num_bytes, checksum
// RX: cmd, DATA, checksum

#define CMD_RD_ROM 0x24
// TX: cmd, addr_low, addr_hi, num_bytes, checksum
// RX: cmd, DATA, checksum


#define ERR_BUSY 0x11
#define ERR_OVERFLOW 0x12
#define ERR_CMD 0x13
#define ERR_INCOMPLETE_OP 0x14
#define ERR_CHECKSUM 0x15
#define ERR_BYTE_COUNT 0x16
#define ERR_PARAM_REWRITE 0x21
#define ERR_SW_STOP 0x31

// --------------------------------------------




#endif // TMU_STD_H
