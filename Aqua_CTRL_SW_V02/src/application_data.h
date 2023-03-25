/*
 * application_data.h
 *
 *  Created on: 24/03/2023
 *      Author: patri
 */

#ifndef APPLICATION_DATA_H_
#define APPLICATION_DATA_H_

#include <stdint.h>
#include "hal_data.h"
#include "control_utils.h"

#define M 1000000
#define K 1000

#define PH_READING_ADC 0x00
#define CONDUCTIVITY_READING_ADC 0x01
#define PH_READING_CONVERTED 0x02
#define CONDUCTIVITY_READING_CONVERTED 0x03
#define SUPPLY_VOLTAGE_DIAGNOSTIC_READING 0x04

extern volatile uint64_t micros;

struct AppConfig
{
    uint32_t pump_on_interval_ph;
    uint32_t pump_off_interval_ph;
    uint32_t pump_on_interval_cond;
    uint32_t pump_off_interval_cond;
    uint16_t ph_setpoint_raw;
    uint16_t ph_low_warning_raw;
    uint16_t ph_high_warning_raw;
    uint16_t conductivity_setpoint_raw;
    uint16_t conductivity_low_warning_raw;
    uint16_t conductivity_high_warning_raw;

};

void configure_app(struct AppConfig*);

extern const uint16_t analog_sensor_high_threshold;
extern const uint16_t analog_sensor_low_threshold;
extern const uint16_t analog_mains_high_threshold;
extern const uint16_t analog_mains_low_threshold;

extern uint16_t ph_setpoint;
extern uint16_t ph_low_warning;
extern uint16_t ph_high_warning;
extern uint16_t ph_hysteresis;

extern uint16_t conductivity_setpoint;
extern uint16_t conductivity_low_warning;
extern uint16_t conductivity_high_warning;
extern uint16_t conductivity_hysteresis;

extern uint8_t ph_sensor_voltage_adc_channel;
extern uint8_t conductivity_sensor_voltage_adc_channel;
extern uint8_t supply_voltage_adc_channel;
extern uint8_t conductivity_sensor_current_adc_channel;
extern uint8_t ph_sensor_current_adc_channel;

extern uint32_t dosing_period;
extern uint8_t ph_pump_duty_cycle;
extern uint8_t cond_pump_duty_cycle;

extern uint32_t sensor_display_toggle_interval;
extern uint32_t hmi_interval;
extern uint32_t uart_interval;
extern uint32_t adc_scan_interval;

#endif /* APPLICATION_DATA_H_ */
