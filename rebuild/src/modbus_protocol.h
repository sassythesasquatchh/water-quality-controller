/*
 * modbus_protocol.h
 *
 *  Created on: 4/03/2023
 *      Author: patri
 */

#ifndef MODBUS_PROTOCOL_H_
#define MODBUS_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "application_data.h"

// https://modbus.org/docs/PI_MBUS_300.pdf (page 106 for formula and page 9 for description of framing)
// https://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf (page 13 for interval lengths for high baud rates)
// Silent interval of at least 3.5 character times (t_3.5) must occur between messages (FRAME_DELAY)
// Silent interval between characters in the same message cannot exceed 1.5 character times (t_1.5) (CHARACTER_TIMEOUT)
// Formula for character time in microseconds is 1000000*(bits per character)/(baud rate)
// For baud rates higher than 19200 Bps, t_3.5 == 1750us and t_1.5 == 750ms
#define CHARACTER_TIMEOUT 750
#define FRAME_DELAY 1750

#define RESET_VALUE 0x00
#define DATA_LENGTH 32
#define SLAVE_ID 0x07

// Modbus error codes as per standard
// https://modbus.org/docs/PI_MBUS_300.pdf (page 22)
#define ILLEGAL_FUNCTION 0x01
#define ILLEGAL_DATA_ADDRESS 0x02
#define ILLEGAL_DATA_VALUE 0x03

// Modbus function codes as per standard
// https://modbus.org/docs/PI_MBUS_300.pdf (page 96)
#define READ_HOLDING_REGISTERS 0x03
#define READ_INPUT_REGISTERS 0x04
#define WRITE_HOLDING_REGISTERS 0x10



void read_input_registers();
void read_holding_registers();
void write_holding_registers ();
void send_modbus(int);
void modbus_exception (uint8_t, uint8_t);
void handle_modbus_message();
void reset_timer(uint16_t);
void modbus_uart_cb(uart_callback_args_t *);
void modbus_timer_cb(timer_callback_args_t*);



extern volatile bool g_modbus_msg_rcvd;
extern volatile bool g_new_configurations_rcvd;

#endif
