/*
 * hmi_control.c
 *
 *  Created on: 10/02/2023
 *      Author: patri
 */

#include "hmi_control.h"
#include "hal_data.h"

#define HIGH 1
#define LOW 0

static int bcd_pins[4] = {DO_BCD_D, DO_BCD_C, DO_BCD_B, DO_BCD_A};

static int digit_enable_pins[3] = {DO_7XSEGMENT_1_EN, DO_7XSEGMENT_2_EN, DO_7XSEGMENT_3_EN};

static bool bcd_values_arr[3][4]= {0};

static volatile uint16_t cached_reading = 0;
static volatile uint16_t current_digit = 0;

void determine_bcd(uint16_t num, bool bcd_values[]){

    switch (num){

        case 0:
            bcd_values[0] = false;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = false;
            break;

        case 1:
            bcd_values[0] = false;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = true;
            break;

        case 2:
            bcd_values[0] = false;
            bcd_values[1] = false;
            bcd_values[2] = true;
            bcd_values[3] = false;
            break;

        case 3:
            bcd_values[0] = false;
            bcd_values[1] = false;
            bcd_values[2] = true;
            bcd_values[3] = true;
            break;

        case 4:
            bcd_values[0] = false;
            bcd_values[1] = true;
            bcd_values[2] = false;
            bcd_values[3] = false;
            break;

        case 5:
            bcd_values[0] = false;
            bcd_values[1] = true;
            bcd_values[2] = false;
            bcd_values[3] = true;
            break;

        case 6:
            bcd_values[0] = false;
            bcd_values[1] = true;
            bcd_values[2] = true;
            bcd_values[3] = false;
            break;

        case 7:
            bcd_values[0] = false;
            bcd_values[1] = true;
            bcd_values[2] = true;
            bcd_values[3] = true;
            break;

        case 8:
            bcd_values[0] = true;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = false;
            break;

        case 9:
            bcd_values[0] = true;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = true;
            break;
    }
}

void set_bcd_pins()
{
    for(uint8_t i = 0; i<4; i++){
        R_IOPORT_PinWrite(&g_ioport_ctrl, bcd_pins[i], !bcd_values_arr[current_digit][i]);
    }
}

void handle_hmi(uint16_t reading)
{
    if (reading != cached_reading)
    {
        cached_reading = reading;
        num_to_digits(cached_reading);
    }

    R_IOPORT_PinWrite(&g_ioport_ctrl, digit_enable_pins[current_digit], LOW);
    increment_digit();
    set_bcd_pins();
    R_IOPORT_PinWrite(&g_ioport_ctrl, digit_enable_pins[current_digit], HIGH);
    return;
}

void increment_digit()
{
    if (current_digit == 2){
        current_digit = 0;
    } else{
        current_digit++;
    }
}

void num_to_digits(uint16_t num)
{
    for (int i = 2; i >= 0; i--)
    {
        determine_bcd(num%10, bcd_values_arr[i]);
        num = num / 10;
    }
}

