/*
 * dosing_control.c
 *
 *  Created on: 25/03/2023
 *      Author: patri
 */

#include "dosing_control.h"
#include "application_data.h"

static volatile bool takingActionPH = false;
static volatile bool takingActionCond = false;

static volatile bool takeActionPH = false;
static volatile bool takeActionCond = false;

static volatile bool pumpOnCond = false;
static volatile bool pumpOnPH = false;

static volatile uint64_t offTimerPH = 0;
static volatile uint64_t onTimerPH = 0;
static volatile uint64_t offTimerCond = 0;
static volatile uint64_t onTimerCond = 0;

void read_sensors(struct AppConfig *configs, bool err_arr[], uint16_t readings[])
{

    uint16_t ph_reading_adc = 0;
    uint16_t conductivity_reading_adc = 0;
    uint16_t supply_voltage_diagnostic_reading = 0;


    (void) R_ADC_ScanStart(&g_adc0_ctrl);
    /* Wait for conversion to complete. */
    adc_status_t status;
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
    {
        (void) R_ADC_StatusGet(&g_adc0_ctrl, &status);
    }
    /* Read converted data. */

    R_ADC_Read(&g_adc0_ctrl, ph_sensor_voltage_adc_channel, &ph_reading_adc);
    R_ADC_Read(&g_adc0_ctrl, conductivity_sensor_voltage_adc_channel, &conductivity_reading_adc);
//            R_ADC_Read(&g_adc0_ctrl, AI_V_PH_SENSOR, &ph_sensor_diagnostic_reading);
//            R_ADC_Read(&g_adc0_ctrl, AI_V_CONDUCTIV_SENSOR, &conductivity_sensor_diagnostic_reading);
    R_ADC_Read(&g_adc0_ctrl, supply_voltage_adc_channel, &supply_voltage_diagnostic_reading);

    err_arr[PH_HIGH_WARNING] = ph_reading_adc >= configs->ph_high_warning_raw;
    err_arr[PH_LOW_WARNING] = ph_reading_adc <= configs->ph_low_warning_raw;
    //err_arr[PH_SENSOR_VOLTAGE_HIGH] = ph_reading_adc >= analog_sensor_high_threshold;
    //err_arr[PH_SENSOR_VOLTAGE_LOW] = ph_reading_adc <= analog_sensor_low_threshold;
    err_arr[CONDUCTIVITY_HIGH_WARNING] = conductivity_reading_adc >= configs->conductivity_high_warning_raw;
    err_arr[CONDUCTIVITY_LOW_WARNING] = conductivity_reading_adc <= configs->conductivity_low_warning_raw;
    //err_arr[CONDUCTIVITY_SENSOR_VOLTAGE_HIGH] = conductivity_reading_adc >= analog_sensor_high_threshold;
    //err_arr[CONDUCTIVITY_SENSOR_VOLTAGE_LOW] = conductivity_reading_adc <= analog_sensor_low_threshold;
    err_arr[SUPPLY_VOLTAGE_HIGH] = supply_voltage_diagnostic_reading >= analog_mains_high_threshold;
    err_arr[SUPPLY_VOLTAGE_LOW] = supply_voltage_diagnostic_reading <= analog_mains_low_threshold;

    takeActionPH = ph_reading_adc <= configs->ph_setpoint_raw;
    takeActionCond = conductivity_reading_adc <= configs->conductivity_setpoint_raw;

    readings[PH_READING_CONVERTED] = convert_adc_to_ph(ph_reading_adc);
    readings[CONDUCTIVITY_READING_CONVERTED] = convert_adc_to_cond(conductivity_reading_adc);

}

void handle_pumps(struct AppConfig *configs)
{
    if (!pumpOnPH && takeActionPH && !takingActionPH){
        pumpOnPH = true;
        onTimerPH = micros;
        takingActionPH = true;
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP, BSP_IO_LEVEL_HIGH);
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP_ON_LED, BSP_IO_LEVEL_HIGH);
    }

    if (pumpOnPH && (micros - onTimerPH > configs->pump_on_interval_ph)){
        pumpOnPH = false;
        offTimerPH = micros;
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP, BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP_ON_LED, BSP_IO_LEVEL_LOW);
    }

    if (!pumpOnPH && (micros - offTimerPH > configs->pump_off_interval_ph)){
        takingActionPH = false;
    }

    if (!pumpOnCond && takeActionCond && !takingActionCond){
        pumpOnCond = true;
        onTimerCond = micros;
        takingActionCond = true;
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP, BSP_IO_LEVEL_HIGH);
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP_ON_LED, BSP_IO_LEVEL_HIGH);
    }

    if (pumpOnCond && (micros - onTimerCond > configs->pump_on_interval_cond)){
        pumpOnCond = false;
        offTimerCond = micros;
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP, BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP_ON_LED, BSP_IO_LEVEL_LOW);
    }

    if (!pumpOnCond && (micros - offTimerCond > configs->pump_off_interval_cond)){
        takingActionCond = false;
    }

}
