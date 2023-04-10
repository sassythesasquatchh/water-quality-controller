/*
 * serial_communication.c
 *
 *  Created on: 24/02/2023
 *      Author: patri
 */

#include "serial_communication.h"
//#include <stdbool.h>
//#include <stdint.h>


static volatile uint8_t g_uart_event = 0;  // declare global variable g_uart_event and initialize it to 0
static volatile bool send_measurement_update = false;  // declare global variable send_measurement_update and initialize it to false
static volatile bool send_diagnostics_update = false;  // declare global variable send_diagnostics_update and initialize it to false
static volatile uint8_t conductivity_arr[3];  // declare global array variable conductivity_arr with 3 elements of type uint8_t

void handle_uart(volatile bool err_arr[], uint16_t conductivity, uint16_t ph)
{
    if (send_measurement_update)  // check if send_measurement_update is true
    {
        toCharArray(conductivity); // Convert three digit integer to array of characters
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) &ph, 1); // write ph reading (1 byte) to UART
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) conductivity_arr, 3); // write conductivity_arr (3 bytes) to UART
        send_measurement_update = false;  // set send_measurement_update back to false
    }

    if (send_diagnostics_update)  // check if send_diagnostics_update is true
    {
        bool no_errors = true;  // declare and initialize boolean variable no_errors to true
        volatile uint8_t temp_err = ALL_CLEAR;  // declare and initialize variable temp_err to ALL_CLEAR

        for (uint8_t err_code = 1; err_code<=NUM_ERR_CODES; err_code++)  // loop through all error codes
        {
            if (err_arr[err_code])  // if there is an error with the current error code
            {
                R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &err_code, 1);  // write the error code to UART
                no_errors = false;  // set no_errors to false
            }
        }

        if (no_errors)  // if no errors were found
        {
            R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);  // write ALL_CLEAR to UART
        }

        temp_err = PENULTIMATE;  // set temp_err to PENULTIMATE
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);  // write PENULTIMATE to UART
        temp_err = END_OF_MESSAGE;  // set temp_err to END_OF_MESSAGE
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);  // write END_OF_MESSAGE to UART
        send_diagnostics_update = false;  // set send_diagnostics_update back to false
    }
}

void uart_cb(uart_callback_args_t *p_args)
{
    // Store the event in a volatile variable
    g_uart_event = (uint8_t) p_args->event;

    // If the received event is a character being received over UART
    if(UART_EVENT_RX_CHAR == p_args->event)
    {
        // Check the value of the received data byte
        switch (p_args->data)
        {
            // If the received data byte indicates a measurement update
            case MEASUREMENT_UPDATE:
                // Set a flag to indicate that a measurement update should be sent over UART
                send_measurement_update = true;
                break;

            // If the received data byte indicates a diagnostics update
            case DIAGNOSTICS_UPDATE:
                // Set a flag to indicate that a diagnostics update should be sent over UART
                send_diagnostics_update = true;
                break;
        }
    }
}

// A function to convert a three-digit integer to an array of characters
void toCharArray(uint16_t number)
{
    // Loop through the digits of the number, starting from the ones place and moving left
    for (int i = 2; i >= 0; i--, number /= 10)
    {
        // Extract the current digit and add the ASCII code for '0' to convert to the corresponding character
        conductivity_arr[i] = (uint8_t) (number % 10) + '0';
    }
}
