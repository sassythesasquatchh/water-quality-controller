/*
 * dosing_control.h
 *
 *  Created on: 25/03/2023
 *      Author: patri
 */

#ifndef DOSING_CONTROL_H_
#define DOSING_CONTROL_H_

#include <stdbool.h>
#include <stdint.h>
#include "application_data.h"

//Error codes to send
#define PH_SENSOR_FAULT 0x00
#define PH_LOW_WARNING 0x01
#define PH_HIGH_WARNING 0x02
#define PH_LOW_CUTOFF 0x03
#define PH_HIGH_CUTOFF 0x04
#define CONDUCTIVITY_SENSOR_FAULT 0x05
#define CONDUCTIVITY_LOW_WARNING 0x06
#define CONDUCTIVITY_HIGH_WARNING 0x07
#define CONDUCTIVITY_LOW_CUTOFF 0x08
#define CONDUCTIVITY_HIGH_CUTOFF 0x09
#define SUPPLY_VOLTAGE_LOW 0x0A
#define SUPPLY_VOLTAGE_HIGH 0x0B

void read_sensors(struct AppConfig *, bool[], uint16_t[]);
void handle_pumps(struct AppConfig *);

#endif /* DOSING_CONTROL_H_ */