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

uint16_t analog_sensor_high_threshold = 2484; // 2000 mV converted to 12 bit ADC reading
uint16_t analog_sensor_low_threshold = 496; // 400 mV converted to 12 bit ADC reading
uint16_t analog_mains_high_threshold = 3644; // Represents mains voltage rising above 26V
uint16_t analog_mains_low_threshold = 1281; // Represents mains voltage dropping below 12V

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

volatile uint32_t pump_on_interval_ph = (uint32_t) ph_pump_duty_cycle*dosing_period/100;
volatile uint32_t pump_off_interval_ph = (uint32_t) (100-ph_pump_duty_cycle)*dosing_period/100;
volatile uint32_t pump_on_interval_cond = (uint32_t) cond_pump_duty_cycle*dosing_period/100;
volatile uint32_t pump_off_interval_cond = (uint32_t) (100-cond_pump_duty_cycle)*dosing_period/100;
static uint32_t adc_scan_interval = 10*M; // Interval between ADC scans in seconds
static uint32_t sensor_display_toggle_interval = 5*M; // Time period between automatic toggling from pH to conductivity for HMI display
static uint32_t hmi_interval = 5*K; // In milliseconds
static uint32_t uart_interval = 10*K; // In milliseconds

uint16_t ph_setpoint_raw = convert_ph_to_adc(ph_setpoint-ph_hysteresis);
uint16_t ph_low_warning_raw = convert_ph_to_adc(ph_low_warning-ph_hysteresis);
uint16_t ph_high_warning_raw = convert_ph_to_adc(ph_high_warning-ph_hysteresis);

uint16_t conductivity_setpoint_raw = convert_cond_to_adc(conductivity_setpoint-conductivity_hysteresis);
uint16_t conductivity_low_warning_raw = convert_ph_to_adc(conductivity_low_warning-conductivity_hysteresis);
uint16_t conductivity_high_warning_raw = convert_ph_to_adc(conductivity_high_warning-conductivity_hysteresis);




#endif /* APPLICATION_DATA_H_ */
