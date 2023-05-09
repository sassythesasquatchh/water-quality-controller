/*
 * application_data.h
 *
 *  Created on: 24/03/2023
 *      Author: patri
 */

#ifndef APPLICATION_DATA_H_
#define APPLICATION_DATA_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "control_utils.h"
#include "modbus_protocol.h"


#define M 1000000
#define K 1000

// Indices for the input registers
#define DOSING_STATUS_REGISTER 0x00
#define PH_READING_INDEX 0x01
#define CONDUCTIVITY_READING_INDEX 0x02


// Indices for the holding registers
#define PH_SETPOINT_INDEX 0x00
#define CONDUCTIVITY_SETPOINT_INDEX 0x01

#define PH_LOW_WARNING_INDEX 0x02
#define PH_HIGH_WARNING_INDEX 0x03
#define PH_LOW_CUTOFF_INDEX 0x04
#define PH_HIGH_CUTOFF_INDEX 0x05

#define CONDUCTIVITY_LOW_WARNING_INDEX 0x06
#define CONDUCTIVITY_HIGH_WARNING_INDEX 0x07
#define CONDUCTIVITY_LOW_CUTOFF_INDEX 0x08
#define CONDUCTIVITY_HIGH_CUTOFF_INDEX 0x09

#define PH_HYSTERESIS_INDEX 0x0A
#define CONDUCTIVITY_HYSTERESIS_INDEX 0x0B

#define PH_PUMP_DUTY_CYCLE_INDEX 0x0C
#define PH_PUMP_DOSING_PERIOD_INDEX 0x0D
#define CONDUCTIVITY_PUMP_DUTY_CYCLE_INDEX 0x0E
#define CONDUCTIVITY_PUMP_DOSING_PERIOD_INDEX 0x0F

#define PH_READING_ADC 0x00
#define CONDUCTIVITY_READING_ADC 0x01
#define PH_READING_CONVERTED 0x02
#define CONDUCTIVITY_READING_CONVERTED 0x03
#define SUPPLY_VOLTAGE_DIAGNOSTIC_READING 0x04

extern volatile uint64_t millis;
extern volatile bool startup_complete;

extern volatile uint16_t holding_registers[16];
extern volatile uint16_t input_registers[3];

struct AppConfig
{
    uint32_t pump_on_interval_ph;
    uint32_t pump_off_interval_ph;
    uint32_t pump_on_interval_cond;
    uint32_t pump_off_interval_cond;
    uint16_t ph_setpoint_raw;
    uint16_t ph_low_warning_raw;
    uint16_t ph_high_warning_raw;
    uint16_t ph_low_cutoff_raw;
    uint16_t ph_high_cutoff_raw;
    uint16_t conductivity_setpoint_raw;
    uint16_t conductivity_low_warning_raw;
    uint16_t conductivity_high_warning_raw;
    uint16_t conductivity_low_cutoff_raw;
    uint16_t conductivity_high_cutoff_raw;

};

void configure_app(struct AppConfig*);
void initialise_holding_registers(void);

extern const uint16_t analog_sensor_high_threshold;
extern const uint16_t analog_sensor_low_threshold;
extern const uint16_t analog_mains_high_threshold;
extern const uint16_t analog_mains_low_threshold;

extern uint16_t ph_setpoint;
extern uint16_t ph_low_warning;
extern uint16_t ph_high_warning;
extern uint16_t ph_low_cutoff;
extern uint16_t ph_high_cutoff;
extern uint16_t ph_hysteresis;

extern uint16_t conductivity_setpoint;
extern uint16_t conductivity_low_warning;
extern uint16_t conductivity_high_warning;
extern uint16_t conductivity_low_cutoff;
extern uint16_t conductivity_high_cutoff;
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
extern uint32_t start_up_interval;

extern uint32_t sensor_response_time;

#endif /* APPLICATION_DATA_H_ */
