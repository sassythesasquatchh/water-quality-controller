#include <modbus_protocol.h>
#include "hmi_control.h"
#include "control_utils.h"
#include "hal_data.h"
#include "serial_communication.h"
#include "application_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

static volatile uint64_t  micros = 0;

void count_micros (timer_callback_args_t *p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        micros++;
    }
}
/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    uint16_t ph_reading_adc = 0;
    uint16_t conductivity_reading_adc = 0;
    uint16_t ph_reading_converted = 0;
    uint16_t conductivity_reading_converted = 0;
    uint16_t supply_voltage_diagnostic_reading = 0;

    volatile bool takingActionPH = false;
    volatile bool takingActionCond = false;

    volatile bool takeActionPH = false;
    volatile bool takeActionCond = false;

    volatile bool pumpOnCond = false;
    volatile bool pumpOnPH = false;

    volatile bool err_arr[24] = {0};

    volatile bool firstPass = true;

    volatile bool hmi_display_cond = false; // 0 for pH, 1 for conductivity

    volatile uint64_t offTimerPH=0;
    volatile uint64_t onTimerPH=0;
    volatile uint64_t offTimerCond=0;
    volatile uint64_t onTimerCond=0;
    volatile uint64_t ADCTimer=0;
    volatile uint64_t displayToggleTimer = 0;
    volatile uint64_t hmiTimer = 0;
    volatile uint64_t uartTimer = 0;

    bsp_io_port_pin_t ph_pump_pin = BSP_IO_PORT_01_PIN_11;
    bsp_io_port_pin_t conductivity_pump_pin = BSP_IO_PORT_01_PIN_10;
    bsp_io_port_pin_t conductivity_pump_on_led_pin = BSP_IO_PORT_07_PIN_00;
    bsp_io_port_pin_t ph_pump_on_led_pin = BSP_IO_PORT_07_PIN_02;

    /* Initializes the module. */
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    (void) R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);

    /* Enable channels. */
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);

    (void) R_GPT_Start(&g_timer0_ctrl);

    while (true){

        if ((micros - ADCTimer >= adc_scan_interval) || firstPass){
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

            err_arr[PH_READING_HIGH] = ph_reading_adc >= ph_high_warning_raw;
            err_arr[PH_READING_LOW] = ph_reading_adc <= ph_low_warning_raw;
            err_arr[PH_SENSOR_VOLTAGE_HIGH] = ph_reading_adc >= analog_sensor_high_threshold;
            err_arr[PH_SENSOR_VOLTAGE_LOW] = ph_reading_adc <= analog_sensor_low_threshold;
            err_arr[CONDUCTIVITY_READING_HIGH] = conductivity_reading_adc >= conductivity_high_warning_raw;
            err_arr[CONDUCTIVITY_READING_LOW] = conductivity_reading_adc <= conductivity_low_warning_raw;
            err_arr[CONDUCTIVITY_SENSOR_VOLTAGE_HIGH] = conductivity_reading_adc >= analog_sensor_high_threshold;
            err_arr[CONDUCTIVITY_SENSOR_VOLTAGE_LOW] = conductivity_reading_adc <= analog_sensor_low_threshold;
            err_arr[SUPPLY_VOLTAGE_HIGH] = supply_voltage_diagnostic_reading >= analog_mains_high_threshold;
            err_arr[SUPPLY_VOLTAGE_LOW] = supply_voltage_diagnostic_reading <= analog_mains_low_threshold;

            takeActionPH = ph_reading_adc <= ph_setpoint_raw;
            takeActionCond = conductivity_reading_adc <= conductivity_setpoint_raw;

            ph_reading_converted = convert_adc_to_ph(ph_reading_adc);
            conductivity_reading_converted = convert_adc_to_cond(conductivity_reading_adc);

            firstPass = false;
            ADCTimer = micros;

        }

        if (!pumpOnPH && takeActionPH && !takingActionPH){
            pumpOnPH = true;
            onTimerPH = micros;
            takingActionPH = true;
            R_IOPORT_PinWrite(&g_ioport_ctrl, ph_pump_pin, BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, ph_pump_on_led_pin, BSP_IO_LEVEL_HIGH);
        }

        if (pumpOnPH && (micros - onTimerPH > pump_on_interval_ph)){
            pumpOnPH = false;
            offTimerPH = micros;
            R_IOPORT_PinWrite(&g_ioport_ctrl, ph_pump_pin, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, ph_pump_on_led_pin, BSP_IO_LEVEL_LOW);
        }

        if (!pumpOnPH && (micros - offTimerPH > pump_off_interval_ph)){
            takingActionPH = false;
        }

        if (!pumpOnCond && takeActionCond && !takingActionCond){
            pumpOnCond = true;
            onTimerCond = micros;
            takingActionCond = true;
            R_IOPORT_PinWrite(&g_ioport_ctrl, conductivity_pump_pin, BSP_IO_LEVEL_HIGH);
            R_IOPORT_PinWrite(&g_ioport_ctrl, conductivity_pump_on_led_pin, BSP_IO_LEVEL_HIGH);
        }

        if (pumpOnCond && (micros - onTimerCond > pump_on_interval_cond)){
            pumpOnCond = false;
            offTimerCond = micros;
            R_IOPORT_PinWrite(&g_ioport_ctrl, conductivity_pump_pin, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite(&g_ioport_ctrl, conductivity_pump_on_led_pin, BSP_IO_LEVEL_LOW);
        }

        if (!pumpOnCond && (micros - offTimerCond > pump_off_interval_cond)){
            takingActionCond = false;
        }

        if (micros - displayToggleTimer > sensor_display_toggle_interval)
        {
            hmi_display_cond = !hmi_display_cond;
            displayToggleTimer = micros;
            R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIV_SENSOR_DISPLAY_ON, hmi_display_cond);
            R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_SENSOR_DISPLAY_ON, !hmi_display_cond);
        }

        if (micros - hmiTimer > hmi_interval){
           if (hmi_display_cond)
           {
               handle_hmi(conductivity_reading_converted);
           }else
           {
               handle_hmi(ph_reading_converted);
           }
           hmiTimer = micros;
        }

        if (g_modbus_msg_rcvd)
        {
            handle_modbus_message();
        }

    }


#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
