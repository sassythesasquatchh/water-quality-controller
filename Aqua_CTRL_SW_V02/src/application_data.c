/*
 * application_data.c
 *
 *  Created on: 25/03/2023
 *      Author: patri
 */

#include "application_data.h"

void configure_app(struct AppConfig *configs)
{
    configs->pump_on_interval_ph = (uint32_t) ph_pump_duty_cycle*dosing_period/100;
    configs->pump_off_interval_ph = (uint32_t) (100-ph_pump_duty_cycle)*dosing_period/100;
    configs->pump_on_interval_cond = (uint32_t) cond_pump_duty_cycle*dosing_period/100;
    configs->pump_off_interval_cond = (uint32_t) (100-cond_pump_duty_cycle)*dosing_period/100;
    configs->ph_setpoint_raw = convert_ph_to_adc(ph_setpoint-ph_hysteresis);
    configs->ph_low_warning_raw = convert_ph_to_adc(ph_low_warning-ph_hysteresis);
    configs->ph_high_warning_raw = convert_ph_to_adc(ph_high_warning-ph_hysteresis);
    configs->conductivity_setpoint_raw = convert_cond_to_adc(conductivity_setpoint-conductivity_hysteresis);
    configs->conductivity_low_warning_raw = convert_ph_to_adc(conductivity_low_warning-conductivity_hysteresis);
    configs->conductivity_high_warning_raw = convert_ph_to_adc(conductivity_high_warning-conductivity_hysteresis);
}


