/*
 * serial_communication.c
 *
 *  Created on: 24/02/2023
 *      Author: patri
 */

#include "serial_communication.h"
//#include <stdbool.h>
//#include <stdint.h>


static volatile uint8_t g_uart_event = 0;
static volatile bool send_measurement_update = false;
static volatile bool send_diagnostics_update = false;
static volatile uint8_t conductivity_arr[3];

void handle_uart(volatile bool err_arr[], uint16_t conductivity, uint16_t ph)
{
    if (send_measurement_update)
    {
        toCharArray(conductivity); // Convert three digit integer to array of characters
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) &ph, 1); // No need to convert as ph reading is guaranteed to be less than one byte
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) conductivity_arr, 3);
        send_measurement_update = false;
    }

    if (send_diagnostics_update)
    {
        bool no_errors = true;
        volatile uint8_t temp_err = ALL_CLEAR;

        for (uint8_t err_code = 1; err_code<=NUM_ERR_CODES; err_code++)
        {
            if (err_arr[err_code])
            {
                R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &err_code, 1);
                no_errors = false;
            }
        }

        if (no_errors)
        {

            R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);
        }

        temp_err = PENULTIMATE;
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);
        temp_err = END_OF_MESSAGE;
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *) &temp_err, 1);
        send_diagnostics_update = false;
    }
}


void uart_cb(uart_callback_args_t *p_args)
{
    g_uart_event = (uint8_t) p_args->event;

    if(UART_EVENT_RX_CHAR == p_args->event)
    {
        switch (p_args->data)
        {
            case MEASUREMENT_UPDATE:
                send_measurement_update = true;
                break;

            case DIAGNOSTICS_UPDATE:
                send_diagnostics_update = true;
                break;
        }
    }
}

void toCharArray(uint16_t number)
{
    for (int i = 2; i >= 0; i--, number /= 10)
    {
        conductivity_arr[i] = (uint8_t) (number % 10) + '0';
    }
}
