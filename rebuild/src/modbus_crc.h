/*
 * modbus_crc.h
 *
 *  Created on: 7/03/2023
 *      Author: patri
 */

#ifndef MODBUS_CRC_H_
#define MODBUS_CRC_H_
#include "stdint.h"

uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);


#endif /* MODBUS_CRC_H_ */
