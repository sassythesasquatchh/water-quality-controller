/*
 * hmi_control.c
 *
 *  Created on: 10/02/2023
 *      Author: patri
 */

#include "hmi_control.h"
#include "hal_data.h"

#define HIGH 1               // Define the value of HIGH as 1
#define LOW 0                // Define the value of LOW as 0

static int bcd_pins[4] = {   // Array of BCD pins
    DO_BCD_D,                // Pin for BCD digit D
    DO_BCD_C,                // Pin for BCD digit C
    DO_BCD_B,                // Pin for BCD digit B
    DO_BCD_A                 // Pin for BCD digit A
};

static int digit_enable_pins[3] = {    // Array of digit enable pins
    DO_7XSEGMENT_1_EN,                 // Pin for 7-segment digit 1
    DO_7XSEGMENT_2_EN,                 // Pin for 7-segment digit 2
    DO_7XSEGMENT_3_EN                  // Pin for 7-segment digit 3
};

static bool bcd_values_arr[3][4]= {    // 2D array to store BCD values
    {LOW, LOW, LOW, LOW},              // Initial values for digit 1
    {LOW, LOW, LOW, LOW},              // Initial values for digit 2
    {LOW, LOW, LOW, LOW}               // Initial values for digit 3
};

static volatile uint16_t cached_reading = 0;    // Cached reading of the analog input
static volatile uint16_t current_digit = 0;     // Current digit being displayed

void determine_bcd(uint16_t num, bool bcd_values[]){

    switch (num){                // Determine the BCD values based on the given number

        case 0: // If num is 0, set the bcd_values array to 0000
            bcd_values[0] = LOW;
            bcd_values[1] = LOW;
            bcd_values[2] = LOW;
            bcd_values[3] = LOW;
            break;

        case 1: // If num is 1, set the bcd_values array to 0001
            bcd_values[0] = LOW;
            bcd_values[1] = LOW;
            bcd_values[2] = LOW;
            bcd_values[3] = HIGH;
            break;

        case 2: // If num is 2, set the bcd_values array to 0010
            bcd_values[0] = LOW;
            bcd_values[1] = LOW;
            bcd_values[2] = HIGH;
            bcd_values[3] = LOW;
            break;

        case 3: // If num is 3, set the bcd_values array to 0011
            bcd_values[0] = LOW;
            bcd_values[1] = LOW;
            bcd_values[2] = HIGH;
            bcd_values[3] = HIGH;
            break;

        case 4: // If num is 4, set the bcd_values array to 0100
            bcd_values[0] = LOW;
            bcd_values[1] = HIGH;
            bcd_values[2] = LOW;
            bcd_values[3] = LOW;
            break;

        case 5: // If num is 5, set the bcd_values array to 0101
            bcd_values[0] = LOW;
            bcd_values[1] = HIGH;
            bcd_values[2] = LOW;
            bcd_values[3] = HIGH;
            break;

        case 6: // If num is 6, set the bcd_values array to 0110
            bcd_values[0] = LOW;
            bcd_values[1] = HIGH;
            bcd_values[2] = HIGH;
            bcd_values[3] = LOW;
            break;

        case 7: // If num is 7, set the bcd_values array to 0111
            bcd_values[0] = LOW;
            bcd_values[1] = HIGH;
            bcd_values[2] = HIGH;
            bcd_values[3] = HIGH;
            break;


        case 8: // If num is 8, set the bcd_values array to 1000
            bcd_values[0] = true;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = false;
            break;

        case 9: // If num is 9, set the bcd_values array to 1001
            bcd_values[0] = true;
            bcd_values[1] = false;
            bcd_values[2] = false;
            bcd_values[3] = true;
            break;
    }
}

void set_bcd_pins()
{
    // Loop through each of the four BCD pins
    for(uint8_t i = 0; i<4; i++){
        // Write the inverse value of the corresponding value in the BCD array to the pin
        // Inverse value is used because the hardware is active low
        R_IOPORT_PinWrite(&g_ioport_ctrl, bcd_pins[i], !bcd_values_arr[current_digit][i]);
    }
}

void handle_hmi(uint16_t reading)
{
    // If the reading has changed since the last time handle_hmi was called
    if (reading != cached_reading)
    {
        // Update the cached reading and convert it to BCD values
        cached_reading = reading;
        num_to_digits(cached_reading);
    }

    // Disable the current digit
    R_IOPORT_PinWrite(&g_ioport_ctrl, digit_enable_pins[current_digit], LOW);

    // Move to the next digit and wrap around to 0 if necessary
    increment_digit();

    // Set the BCD pins for the new digit
    set_bcd_pins();

    // Enable the new digit
    R_IOPORT_PinWrite(&g_ioport_ctrl, digit_enable_pins[current_digit], HIGH);
    return;
}

void increment_digit()
{
    // If the current digit is 2, wrap around to 0
    if (current_digit == 2){
        current_digit = 0;
    } else{
        // Otherwise, move to the next digit
        current_digit++;
    }
}

void num_to_digits(uint16_t num)
{
    // Convert the number to BCD values and store them in the bcd_values_arr array
    for (int i = 2; i >= 0; i--)
    {
        determine_bcd(num%10, bcd_values_arr[i]);
        num = num / 10;
    }
}
