/*
 * dosing_control.c
 *
 *  Created on: 25/03/2023
 *      Author: patri
 */

#include "dosing_control.h"

// Declare boolean variables to keep track of whether corrective action is taking place
// This is different to checking whether the pump is on because the corrective action
// includes both an on-period and an off-period to account for hysteresis
// ie. when action is taken, the pump will stay on for a period, then will
// remain off for a period before it can begin taking action again
static volatile bool takingActionPH = false;
static volatile bool takingActionCond = false;

// Declare boolean variables to indicate whether corrective action is needed
static volatile bool takeActionPH = false;
static volatile bool takeActionCond = false;

// Declare boolean variables to indicate whether each pump is on
static volatile bool pumpOnCond = false;
static volatile bool pumpOnPH = false;
// Declare uint64_t variables to keep track of the timing of events
static volatile uint64_t offTimerPH = 0; // Times the 'off-interval' for the pH pump
static volatile uint64_t onTimerPH = 0; // Times the 'on-interval' for the pH pump
static volatile uint64_t offTimerCond = 0; // Times the 'off-interval' for the conductivity pump
static volatile uint64_t onTimerCond = 0; // Times the 'off-interval' for the conductivity pump

volatile bool sensors_on = false;

void read_sensors(struct AppConfig *configs, bool err_arr[])
{
    uint16_t ph_reading_adc = 0; // Initialize variable for storing pH sensor reading from ADC
    uint16_t conductivity_reading_adc = 0; // Initialize variable for storing conductivity sensor reading from ADC
    uint16_t supply_voltage_diagnostic_reading = 0; // Initialize variable for storing supply voltage reading from ADC

    // Start ADC scan
    (void) R_ADC_ScanStart(&g_adc0_ctrl);

    /* Wait for conversion to complete. */
    adc_status_t status;
    status.state = ADC_STATE_SCAN_IN_PROGRESS; // Set the initial state of the ADC scan to 'in progress'
    while (ADC_STATE_SCAN_IN_PROGRESS == status.state) // Wait for ADC scan to complete
    {
        (void) R_ADC_StatusGet(&g_adc0_ctrl, &status); // Get the current status of the ADC scan
    }

    /* Read digitized data from the ADC. */

    // Read the digitized pH sensor reading from ADC
    R_ADC_Read(&g_adc0_ctrl, ph_sensor_voltage_adc_channel, &ph_reading_adc);
    // Read the digitized conductivity sensor reading from ADC
    R_ADC_Read(&g_adc0_ctrl, conductivity_sensor_voltage_adc_channel, &conductivity_reading_adc);
    // Read the digitized supply voltage diagnostic reading from ADC
    R_ADC_Read(&g_adc0_ctrl, supply_voltage_adc_channel, &supply_voltage_diagnostic_reading);

    // Turn off the power supply to the pH and conductivity sensors
    turn_sensors_off();

    // Check if pH reading is higher than the high warning threshold and set corresponding error flag
    err_arr[PH_HIGH_WARNING] = ph_reading_adc >= configs->ph_high_warning_raw;
    // Check if pH reading is lower than the low warning threshold and set corresponding error flag
    err_arr[PH_LOW_WARNING] = ph_reading_adc <= configs->ph_low_warning_raw;
    // Check if conductivity reading is higher than the high warning threshold and set corresponding error flag
    err_arr[CONDUCTIVITY_HIGH_WARNING] = conductivity_reading_adc >= configs->conductivity_high_warning_raw;
    // Check if conductivity reading is lower than the low warning threshold and set corresponding error flag
    err_arr[CONDUCTIVITY_LOW_WARNING] = conductivity_reading_adc <= configs->conductivity_low_warning_raw;
    // Check if supply voltage diagnostic reading is higher than the high threshold and set corresponding error flag
    err_arr[SUPPLY_VOLTAGE_HIGH] = supply_voltage_diagnostic_reading >= analog_mains_high_threshold;
    // Check if supply voltage diagnostic reading is lower than the low threshold and set corresponding error flag
    err_arr[SUPPLY_VOLTAGE_LOW] = supply_voltage_diagnostic_reading <= analog_mains_low_threshold;

    // Check if pH reading is lower than or equal to the pH setpoint and set corresponding take action flag
    takeActionPH = ph_reading_adc <= configs->ph_setpoint_raw;
    // Check if conductivity reading is lower than or equal to the conductivity setpoint and set corresponding take action flag
    takeActionCond = conductivity_reading_adc <= configs->conductivity_setpoint_raw;

    // Convert pH sensor reading from ADC to pH value and store in input_registers
    input_registers[PH_READING_INDEX] = convert_adc_to_ph(ph_reading_adc);
    // Convert conductivity sensor reading from ADC to conductivity value and store in input_registers
    input_registers[CONDUCTIVITY_READING_INDEX] = convert_adc_to_cond(conductivity_reading_adc);
}

void handle_pumps(struct AppConfig *configs)
{
    // Check if pH pump needs to be turned on
    if (!pumpOnPH && takeActionPH && !takingActionPH){
        pumpOnPH = true;
        onTimerPH = micros; // Start timer to keep track of how long the pump has been on
        takingActionPH = true; // Set flag to indicate that the pump is currently running
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP, BSP_IO_LEVEL_HIGH); // Turn on the pH pump
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP_ON_LED, BSP_IO_LEVEL_HIGH); // Turn on the LED indicating that the pH pump is on
    }

    // Check if pH pump needs to be turned off
    if (pumpOnPH && (micros - onTimerPH > configs->pump_on_interval_ph)){
        pumpOnPH = false;
        offTimerPH = micros; // Start timer to keep track of how long the pump has been off
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP, BSP_IO_LEVEL_LOW); // Turn off the pH pump
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_PUMP_ON_LED, BSP_IO_LEVEL_LOW); // Turn off the LED indicating that the pH pump is off
    }

    // Check if it's time to stop taking action with the pH pump
    if (!pumpOnPH && (micros - offTimerPH > configs->pump_off_interval_ph)){
        takingActionPH = false; // Set flag to indicate that it's time to stop taking action with the pH pump
    }

    // Check if conductivity pump needs to be turned on
    if (!pumpOnCond && takeActionCond && !takingActionCond){
        pumpOnCond = true;
        onTimerCond = micros; // Start timer to keep track of how long the pump has been on
        takingActionCond = true; // Set flag to indicate that the pump is currently running
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP, BSP_IO_LEVEL_HIGH); // Turn on the conductivity pump
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP_ON_LED, BSP_IO_LEVEL_HIGH); // Turn on the LED indicating that the conductivity pump is on
    }

    // Check if conductivity pump needs to be turned off
    if (pumpOnCond && (micros - onTimerCond > configs->pump_on_interval_cond)){
        pumpOnCond = false;
        offTimerCond = micros; // Start timer to keep track of how long the pump has been off
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP, BSP_IO_LEVEL_LOW); // Turn off the conductivity pump
        R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_PUMP_ON_LED, BSP_IO_LEVEL_LOW); // Turn off the LED indicating that the conductivity pump is off
    }

    // Check if it's time to stop taking action with the conductivity pump
    if (!pumpOnCond && (micros - offTimerCond > configs->pump_off_interval_cond)){
        takingActionCond = false; // Set flag to indicate that it's time to stop taking action with the conductivity pump
    }
}

void turn_sensors_on()
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_SENSOR_ON, BSP_IO_LEVEL_HIGH); // Turn on the pH sensor
    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_SENSOR_ON, BSP_IO_LEVEL_HIGH); // Turn on the conductivity sensor

    sensors_on = true;
}

void turn_sensors_off()
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_SENSOR_ON, BSP_IO_LEVEL_LOW); // Turn off the pH sensor
    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIVITY_SENSOR_ON, BSP_IO_LEVEL_LOW); // Turn off the conductivity sensor

    sensors_on = false;
}
