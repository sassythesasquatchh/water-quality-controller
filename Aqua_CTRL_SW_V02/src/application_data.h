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


const uint16_t analog_sensor_high_threshold = 2484; // 2000 mV converted to 12 bit ADC reading
const uint16_t analog_sensor_low_threshold = 496; // 400 mV converted to 12 bit ADC reading
const uint16_t analog_mains_high_threshold = 3644; // Represents mains voltage rising above 26V
const uint16_t analog_mains_low_threshold = 1281; // Represents mains voltage dropping below 12V

uint16_t ph_setpoint = 70; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
uint16_t ph_low_warning = 68; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
uint16_t ph_high_warning = 81; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
uint16_t ph_hysteresis = 1; // In pH*10 to avoid decimals (eg. write 1 for 0.1 pH)

uint16_t conductivity_setpoint = 700; // In micro-siemens
uint16_t conductivity_low_warning = 300; // In micro-siemens
uint16_t conductivity_high_warning = 900; // In micro-siemens
uint16_t conductivity_hysteresis = 5; // In micro-siemens

uint8_t ph_sensor_voltage_adc_channel = ADC_CHANNEL_17;
uint8_t conductivity_sensor_voltage_adc_channel = ADC_CHANNEL_20;
uint8_t supply_voltage_adc_channel = ADC_CHANNEL_18;
uint8_t conductivity_sensor_current_adc_channel = ADC_CHANNEL_19;
uint8_t ph_sensor_current_adc_channel = ADC_CHANNEL_16;

uint32_t dosing_period = 240*K; //Enter dosing period in seconds
uint8_t ph_pump_duty_cycle = 25; //Enter duty cycle for pH pump as percentage of dosing period
uint8_t cond_pump_duty_cycle = 50; //Enter duty cycle for conductivity pump as percentage of dosing period


uint32_t sensor_display_toggle_interval = 5*M; // Time period between automatic toggling from pH to conductivity for HMI display
uint32_t hmi_interval = 5*K; // In milliseconds
uint32_t uart_interval = 10*K; // In milliseconds
uint32_t adc_scan_interval = 10*M; // In seconds





#endif /* APPLICATION_DATA_H_ */
