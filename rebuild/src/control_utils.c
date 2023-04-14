/*
 * control_utils.c
 *
 *  Created on: 10/02/2023
 *      Author: patri
 */
#include "control_utils.h"

#define MAX_CONDUCTIVITY 1000 // Maximum reading of conductivity sensor in micro siemens
#define MAX_PH 140 //Maximum reading of pH sensor (multiplied by 10 to avoid decimals)
#define MAX_ADC 4096 // 2^12 for 12 bit resolution, change this value if using ADC with different resolution
#define V_REF 3300 // Reference voltage level in mV

// This function converts an ADC value to a pH value
uint16_t convert_adc_to_ph(uint16_t adc_value)
{
    // Calculate the voltage in mV
    float temp = (float) adc_value*V_REF / (float) (MAX_ADC - 1);
    // Subtract the offset voltage of 400mV
    temp -= (float) 400.0;
    // Multiply by the maximum pH value
    temp *= MAX_PH;
    // Divide by the range of the 4-20 sensor (2000mV - 400mV)
    temp /= (float) 1600.0;
    // Round value
    temp += (float) 0.5;
    return (uint16_t) temp;
}

// This function converts an ADC value to a conductivity value
uint16_t convert_adc_to_cond(uint16_t adc_value)
{
    // Calculate the voltage in mV
    float temp = (float) adc_value*V_REF / (float) (MAX_ADC - 1);
    // Subtract the offset voltage of 400mV
    temp -= (float) 400.;
    // Multiply by the maximum conductivity value
    temp *= MAX_CONDUCTIVITY;
    // Divide by the range of the 4-20 sensor (2000mV - 400mV)
    temp /= (float) 1600.;
    // Round value
    temp += (float) 0.5;
    return (uint16_t) temp;
}

// This function converts a pH value to an ADC value
uint16_t convert_ph_to_adc(uint16_t ph_value)
{
    // Multiply by the range of the 4-20 sensor (2000mV - 400mV)
    float temp = ph_value * 1600;
    // Divide by the maximum pH value
    temp /= (float) MAX_PH;
    // Add the offset voltage of 400mV
    temp += (float) 400.;
    // Multiply by the maximum ADC value minus one
    temp *= (MAX_ADC -1);
    // Divide by the reference voltage
    temp /= (float) V_REF;
    // Round value
    temp += (float) 0.5;
    return (uint16_t) temp;
}

// This function converts a conductivity value to an ADC value
uint16_t convert_cond_to_adc(uint16_t cond_value)
{
    // Multiply by the range of the 4-20 sensor (2000mV - 400mV)
    float temp = cond_value * 1600;
    // Divide by the maximum conductivity value
    temp /= (float) MAX_CONDUCTIVITY;
    // Add the offset voltage of 400mV
    temp += (float) 400.;
    // Multiply by the maximum ADC value minus one
    temp *= (MAX_ADC -1);
    // Divide by the reference voltage
    temp /= (float) V_REF;
    // Round value
    temp += (float) 0.5;
    return (uint16_t) temp;
}



