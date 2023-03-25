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

uint16_t convert_adc_to_ph(uint16_t adc_value)
{
    float temp = (float) adc_value*V_REF / (float) (MAX_ADC - 1);
    temp -= (float) 400.0;
    temp *= MAX_PH;
    temp /= (float) 1600.0;
    temp += (float) 0.5;
    return (uint16_t) temp;
}

uint16_t convert_adc_to_cond(uint16_t adc_value)
{
    float temp = (float) adc_value*V_REF / (float) (MAX_ADC - 1);
    temp -= (float) 400.;
    temp *= MAX_CONDUCTIVITY;
    temp /= (float) 1600.;
    temp += (float) 0.5;
    return (uint16_t) temp;
}

uint16_t convert_ph_to_adc(uint16_t ph_value)
{
    float temp = ph_value * 1600;
    temp /= (float) MAX_PH;
    temp += (float) 400.;
    temp *= (MAX_ADC -1);
    temp /= (float) V_REF;
    temp += (float) 0.5;
    return (uint16_t) temp;
}

uint16_t convert_cond_to_adc(uint16_t cond_value)
{
    float temp = cond_value * 1600;
    temp /= (float) MAX_CONDUCTIVITY;
    temp += (float) 400.;
    temp *= (MAX_ADC -1);
    temp /= (float) V_REF;
    temp += (float) 0.5;
    return (uint16_t) temp;
}


