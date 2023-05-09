/*
 * hmi_control.h
 *
 *  Created on: 10/02/2023
 *      Author: patri
 */

#ifndef HMI_CONTROL_H_
#define HMI_CONTROL_H_

#include <stdbool.h>
#include <stdint.h>

void determine_bcd(uint16_t, bool[]);
void set_bcd_pins();
void handle_hmi(uint16_t);
void increment_digit();
void num_to_digits(uint16_t);
//void set_hmi_lights();

#endif /* HMI_CONTROL_H_ */
