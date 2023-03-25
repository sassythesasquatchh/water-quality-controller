/*
 * control_utils.h
 *
 *  Created on: 10/02/2023
 *      Author: patri
 */

#ifndef CONTROL_UTILS_H_
#define CONTROL_UTILS_H_

#include <stdint.h>

uint16_t convert_adc_to_ph(uint16_t);
uint16_t convert_adc_to_cond(uint16_t);
uint16_t convert_ph_to_adc(uint16_t);
uint16_t convert_cond_to_adc(uint16_t);

#endif /* CONTROL_UTILS_H_ */
