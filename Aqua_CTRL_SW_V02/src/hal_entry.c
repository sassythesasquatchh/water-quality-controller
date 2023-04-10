#include <modbus_protocol.h>
#include "hmi_control.h"
#include "control_utils.h"
#include "hal_data.h"
#include "application_data.h"
#include "dosing_control.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

// Define a volatile 64-bit unsigned integer variable to hold microsecond values
volatile uint64_t micros = 0;

// Define a function to increment the microsecond value when a timer event occurs
void count_micros (timer_callback_args_t *p_args)
{
    // Check if the event is a timer cycle end
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        // If so, increment the microsecond value
        micros++;
    }
}

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    // Define a struct to hold configuration settings
    struct AppConfig configs;
    // Call a function to set up the configuration settings
    configure_app(&configs);

    // Define an array of volatile boolean variables with length 16 and initialize all values to 0
    volatile bool err_arr[16] = {0};

    // Define a volatile boolean variable and initialize it to true
    volatile bool firstPass = true;

    // Define a volatile boolean variable and initialize it to false
    volatile bool hmi_display_cond = false; // 0 for pH, 1 for conductivity

    // Define volatile 64-bit unsigned integer variables and initialize them to 0
    volatile uint64_t ADCTimer=0;
    volatile uint64_t displayToggleTimer = 0;
    volatile uint64_t hmiTimer = 0;

    /* Initializes the ADC module. */
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);

    /* Initializes the IOPORT module. */
    R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);

    /* Initializes the GPT (General Purpose Timer) module. */
    (void) R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);

    /* Initializes the UART module. */
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);

    /* Enable channels for scanning. */
    // Set up the ADC scan configuration
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);

    // Start the timer
    (void) R_GPT_Start(&g_timer0_ctrl);

    // Enter the main loop
    while (true){

        // If it's time to read the sensors or if this is the first pass through the loop
        if ((micros - ADCTimer >= adc_scan_interval) || firstPass){
            // Call a function to read the sensors and update the configuration settings
            read_sensors(&configs, (bool*)err_arr);

            // Reset the first pass flag
            firstPass = false;
            // Set the timer for the next sensor reading
            ADCTimer = micros;
        }

        // Call a function to control the pumps based on the sensor readings
        handle_pumps(&configs);

        // If it's time to toggle the sensor display
        if (micros - displayToggleTimer > sensor_display_toggle_interval)
        {
            // Switch the flag between pH and conductivity display
            hmi_display_cond = !hmi_display_cond;
            // Set the timer for the next display toggle
            displayToggleTimer = micros;
            // Turn on/off the corresponding sensor display pins based on the flag value
            R_IOPORT_PinWrite(&g_ioport_ctrl, DO_CONDUCTIV_SENSOR_DISPLAY_ON, hmi_display_cond);
            R_IOPORT_PinWrite(&g_ioport_ctrl, DO_PH_SENSOR_DISPLAY_ON, !hmi_display_cond);
        }

        if (micros - hmiTimer > hmi_interval){ // check if HMI update interval has elapsed
            if (hmi_display_cond) { // if flag is set to true
                handle_hmi(input_registers[CONDUCTIVITY_READING_INDEX]); // handle the HMI with conductivity reading
            } else {
                handle_hmi(input_registers[PH_READING_INDEX]); // handle the HMI with pH reading
            }
            hmiTimer = micros; // update HMI timer
        }

        if (g_modbus_msg_rcvd) { // check if Modbus message has been received
            handle_modbus_message(); // handle the Modbus message
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
