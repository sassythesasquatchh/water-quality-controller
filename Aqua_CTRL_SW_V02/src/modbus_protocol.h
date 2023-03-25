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

#define CHARACTER_TIMEOUT 750
#define FRAME_DELAY 1750
#define RESET_VALUE 0x00
#define DATA_LENGTH 32
#define SLAVE_ID 0x07
#define ILLEGAL_FUNCTION 0x01
#define ILLEGAL_DATA_ADDRESS 0x02
#define ILLEGAL_DATA_VALUE 0x03

void read_input_registers();
void read_holding_registers();
void send_modbus(int);
void modbus_exception (uint8_t, uint8_t);
void handle_modbus_message();
void reset_timer(uint16_t);
void modbus_uart_cb(uart_callback_args_t *);
void modbus_timer_cb(timer_callback_args_t*);

extern volatile uint16_t holding_registers[2];
extern volatile uint16_t input_registers[2];

extern volatile bool g_modbus_msg_rcvd;

#endif
