/*
 * application_data.c
 *
 *  Created on: 25/03/2023
 *      Author: patri
 */

#include "application_data.h"


const uint16_t analog_sensor_high_threshold = 2484; // 2000 mV converted to 12 bit ADC reading
const uint16_t analog_sensor_low_threshold = 496; // 400 mV converted to 12 bit ADC reading
const uint16_t analog_mains_high_threshold = 3644; // Represents mains voltage rising above 26V
const uint16_t analog_mains_low_threshold = 1281; // Represents mains voltage dropping below 12V

volatile uint16_t holding_registers[16]={RESET_VALUE};
volatile uint16_t input_registers[3]={RESET_VALUE};

//uint16_t ph_setpoint = 70; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
//uint16_t ph_low_warning = 68; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
//uint16_t ph_high_warning = 81; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
//uint16_t ph_low_cutoff = 67; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
//uint16_t ph_high_cutoff = 83; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
//uint16_t ph_hysteresis = 1; // In pH*10 to avoid decimals (eg. write 1 for 0.1 pH)
//
//uint16_t conductivity_setpoint = 700; // In micro-siemens
//uint16_t conductivity_low_warning = 300; // In micro-siemens
//uint16_t conductivity_high_warning = 900; // In micro-siemens
//uint16_t conductivity_low_cutoff = 200; // In micro-siemens
//uint16_t conductivity_high_cutoff = 900; // In micro-siemens
//uint16_t conductivity_hysteresis = 5; // In micro-siemens



uint8_t ph_sensor_voltage_adc_channel = ADC_CHANNEL_17;
uint8_t conductivity_sensor_voltage_adc_channel = ADC_CHANNEL_20;
uint8_t supply_voltage_adc_channel = ADC_CHANNEL_18;
uint8_t conductivity_sensor_current_adc_channel = ADC_CHANNEL_19;
uint8_t ph_sensor_current_adc_channel = ADC_CHANNEL_16;

//uint32_t dosing_period = 240*K; //Enter dosing period in seconds
//uint8_t ph_pump_duty_cycle = 25; //Enter duty cycle for pH pump as percentage of dosing period
//uint8_t cond_pump_duty_cycle = 50; //Enter duty cycle for conductivity pump as percentage of dosing period



uint32_t sensor_display_toggle_interval = 5*K; // Time period between automatic toggling from pH to conductivity for HMI display
uint32_t hmi_interval = 5; // In milliseconds
uint32_t uart_interval = 10; // In milliseconds
uint32_t adc_scan_interval = 10*K; // In seconds
uint32_t start_up_interval = 10*K; // In seconds

uint32_t sensor_response_time = 5*K; // Upper bound of settling time for the pH sensor, in seconds

void configure_app(struct AppConfig *configs)
{
    // Calculate the 'on interval' for the pH pump by multiplying the dosing period with the duty cycle as a percentage
    configs->pump_on_interval_ph = (uint32_t) holding_registers[PH_PUMP_DUTY_CYCLE_INDEX]*holding_registers[PH_PUMP_DOSING_PERIOD_INDEX]*K/100;
    // Calculate the 'off interval' for the pH pump by multiplying the dosing period with the (100% - duty cycle as a percentage)
    configs->pump_off_interval_ph = (uint32_t) (100-holding_registers[PH_PUMP_DUTY_CYCLE_INDEX])*holding_registers[PH_PUMP_DOSING_PERIOD_INDEX]*K/100;
    // Calculate the 'on interval' for the conductivity pump by multiplying the dosing period with the duty cycle as a percentage
    configs->pump_on_interval_cond = (uint32_t) holding_registers[CONDUCTIVITY_PUMP_DUTY_CYCLE_INDEX]*holding_registers[CONDUCTIVITY_PUMP_DOSING_PERIOD_INDEX]*K/100;
    // Calculate the 'off interval' for the conductivity pump by multiplying the dosing period with the (100% - duty cycle as a percentage)
    configs->pump_off_interval_cond = (uint32_t) (100-holding_registers[CONDUCTIVITY_PUMP_DUTY_CYCLE_INDEX])*holding_registers[CONDUCTIVITY_PUMP_DOSING_PERIOD_INDEX]*K/100;
    // Convert the pH setpoint to raw ADC value
    configs->ph_setpoint_raw = convert_ph_to_adc(holding_registers[PH_SETPOINT_INDEX]-holding_registers[PH_HYSTERESIS_INDEX]);
    // Convert the pH low warning threshold to raw ADC value
    configs->ph_low_warning_raw = convert_ph_to_adc(holding_registers[PH_LOW_WARNING_INDEX]-holding_registers[PH_HYSTERESIS_INDEX]);
    // Convert the pH high warning threshold to raw ADC value
    configs->ph_high_warning_raw = convert_ph_to_adc(holding_registers[PH_HIGH_WARNING_INDEX]-holding_registers[PH_HYSTERESIS_INDEX]);
    // Convert the pH low cutoff threshold to raw ADC value
    configs->ph_low_cutoff_raw = convert_ph_to_adc(holding_registers[PH_LOW_CUTOFF_INDEX]-holding_registers[PH_HYSTERESIS_INDEX]);
    // Convert the pH high cutoff threshold to raw ADC value
    configs->ph_high_cutoff_raw = convert_ph_to_adc(holding_registers[PH_HIGH_CUTOFF_INDEX]-holding_registers[PH_HYSTERESIS_INDEX]);
    // Convert the conductivity setpoint to raw ADC value
    configs->conductivity_setpoint_raw = convert_cond_to_adc(holding_registers[CONDUCTIVITY_SETPOINT_INDEX]-holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]);
    // Convert the conductivity low warning threshold to raw ADC value
    configs->conductivity_low_warning_raw = convert_cond_to_adc(holding_registers[CONDUCTIVITY_LOW_WARNING_INDEX]-holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]);
    // Convert the conductivity high warning threshold to raw ADC value
    configs->conductivity_high_warning_raw = convert_cond_to_adc(holding_registers[CONDUCTIVITY_HIGH_WARNING_INDEX]-holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]);
    // Convert the conductivity low cutoff threshold to raw ADC value
    configs->conductivity_low_cutoff_raw = convert_cond_to_adc(holding_registers[CONDUCTIVITY_LOW_CUTOFF_INDEX]-holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]);
    // Convert the conductivity high cutoff threshold to raw ADC value
    configs->conductivity_high_cutoff_raw = convert_cond_to_adc(holding_registers[CONDUCTIVITY_HIGH_CUTOFF_INDEX]-holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]);
}

void initialise_holding_registers()
{
    holding_registers[PH_SETPOINT_INDEX]=70; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
    holding_registers[PH_LOW_WARNING_INDEX]=68; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
    holding_registers[PH_HIGH_WARNING_INDEX]=81; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
    holding_registers[PH_LOW_CUTOFF_INDEX]=67; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
    holding_registers[PH_HIGH_CUTOFF_INDEX]=83; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)
    holding_registers[PH_HYSTERESIS_INDEX]=1; // In pH*10 to avoid decimals (eg. write 70 for 7.0 pH)

    holding_registers[CONDUCTIVITY_SETPOINT_INDEX]=700; // In micro-siemens
    holding_registers[CONDUCTIVITY_LOW_WARNING_INDEX]=300; // In micro-siemens
    holding_registers[CONDUCTIVITY_HIGH_WARNING_INDEX]=900; // In micro-siemens
    holding_registers[CONDUCTIVITY_LOW_CUTOFF_INDEX]=200; // In micro-siemens
    holding_registers[CONDUCTIVITY_HIGH_CUTOFF_INDEX]=900; // In micro-siemens
    holding_registers[CONDUCTIVITY_HYSTERESIS_INDEX]=5; // In micro-siemens

    holding_registers[PH_PUMP_DUTY_CYCLE_INDEX]=25; //Enter duty cycle for pH pump as percentage of dosing period
    holding_registers[PH_PUMP_DOSING_PERIOD_INDEX]=240; //Enter dosing period in seconds
    holding_registers[CONDUCTIVITY_PUMP_DUTY_CYCLE_INDEX]=900; // In micro-siemens
    holding_registers[CONDUCTIVITY_PUMP_DOSING_PERIOD_INDEX]=50; //Enter duty cycle for conductivity pump as percentage of dosing period

}


