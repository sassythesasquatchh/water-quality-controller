/*
 * modbus_slave.h
 *
 *  Created on: 7/03/2023
 *      Author: patri
 */

#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

#include "modbus_crc.h"

#define SLAVE_ID 7

#define ILLEGAL_FUNCTION       0x01
#define ILLEGAL_DATA_ADDRESS   0x02
#define ILLEGAL_DATA_VALUE     0x03


static uint16_t holding_registers_database[2]={0};


uint8_t readHoldingRegs (void);
void modbusException (uint8_t);
void sendData (uint8_t *, int);


#endif /* MODBUS_SLAVE_H_ */
