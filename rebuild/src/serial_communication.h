/*
 * serial_communication.h
 *
 *  Created on: 25/02/2023
 *      Author: patri
 */

#ifndef SERIAL_COMMUNICATION_H_
#define SERIAL_COMMUNICATION_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"

//Error codes to send
#define ALL_CLEAR 0x00
#define PH_READING_HIGH 0x01
#define PH_READING_LOW 0x02
#define PH_SENSOR_VOLTAGE_HIGH 0x03
#define PH_SENSOR_VOLTAGE_LOW 0x04
#define CONDUCTIVITY_READING_HIGH 0x05
#define CONDUCTIVITY_READING_LOW 0x06
#define CONDUCTIVITY_SENSOR_VOLTAGE_HIGH 0x07
#define CONDUCTIVITY_SENSOR_VOLTAGE_LOW 0x08
#define SUPPLY_VOLTAGE_HIGH 0x09
#define SUPPLY_VOLTAGE_LOW 0x0A
#define END_OF_MESSAGE 0xFF
#define PENULTIMATE 0xFE
#define NUM_ERR_CODES 10

//Command codes from ECU
#define MEASUREMENT_UPDATE 0x01
#define DIAGNOSTICS_UPDATE 0x02

//Function declarations
void handle_uart(volatile bool[], uint16_t, uint16_t);
void uart_cb(uart_callback_args_t*);
void toCharArray(uint16_t);

#endif /* SERIAL_COMMUNICATION_H_ */
