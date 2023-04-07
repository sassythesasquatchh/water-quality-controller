/*
 * modbus_protocol.c
 *
 *  Created on: 28/02/2023
 *      Author: patri
 */

#include "modbus_protocol.h"
#include "hal_data.h"
#include <stdbool.h>
#include "modbus_crc.h"

volatile bool g_char_interval_exceeded = false;
volatile bool g_modbus_msg_rcvd = false;

volatile uint8_t buffer[DATA_LENGTH]={RESET_VALUE};

volatile uint16_t g_index = 0;

volatile uint8_t modbus_rx_buffer[DATA_LENGTH]= {RESET_VALUE};
volatile uint8_t modbus_tx_buffer[DATA_LENGTH] = {RESET_VALUE};

volatile uint16_t holding_registers[2]={RESET_VALUE};
volatile uint16_t input_registers[3]={RESET_VALUE};


void modbus_timer_cb(timer_callback_args_t* p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        if (g_char_interval_exceeded)
        {
            g_char_interval_exceeded = false;

            for(int i = 0; i<=g_index; i++)
            {
                modbus_rx_buffer[i]=buffer[i];
            }
            memset((uint8_t*)buffer, RESET_VALUE, DATA_LENGTH); //Clear receive buffer ready to immediately receive next message

            g_index = 0;
            g_modbus_msg_rcvd = true;

        }
        else{
            g_char_interval_exceeded = true;
            reset_timer(FRAME_DELAY - CHARACTER_TIMEOUT);
        }
    }
}

void modbus_uart_cb(uart_callback_args_t *p_args)
{
    if(UART_EVENT_RX_CHAR == p_args->event)
    {
        reset_timer(CHARACTER_TIMEOUT);

        if (g_char_interval_exceeded)
        {
            memset((uint8_t*)buffer, RESET_VALUE, DATA_LENGTH);
            g_index = 0;

        } else
        {
            buffer[g_index] = (uint8_t) p_args->data;
            g_index++;
        }
    }
}

void reset_timer(uint16_t new_period)
{
    R_AGT_Stop(&g_modbus_timer_ctrl);
    uint32_t timer_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) >>
            g_modbus_timer_cfg.source_div;
    uint32_t period_counts =
            (uint32_t) (((uint64_t) timer_freq_hz * new_period) / 1000000);
    R_AGT_PeriodSet(&g_modbus_timer_ctrl, period_counts);
    R_AGT_Start(&g_modbus_timer_ctrl);
}

void handle_modbus_message()
{
    uint16_t crc = crc16((uint8_t*)modbus_rx_buffer, g_index-2);
    if (crc == (modbus_rx_buffer[g_index] << 8 | modbus_rx_buffer[g_index-1])) //If CRC matches
    {
        if (modbus_rx_buffer[0]==SLAVE_ID) //If message is addressed to this device
        {
            switch (modbus_rx_buffer[1])
            {
                case 0x03:
                    read_holding_registers();
                    break;
                case 0x04:
                    read_input_registers();
                    break;
                case 0x10:
                    //write_multiple_registers();
                    break;
                default:
                    modbus_exception(ILLEGAL_FUNCTION, modbus_rx_buffer[1]);
            }
        }
    }

    g_modbus_msg_rcvd = false;
}

void modbus_exception (uint8_t exception_code, uint8_t function_code)
{
    modbus_tx_buffer[0] = SLAVE_ID;
    modbus_tx_buffer[1] = function_code | 0x80;
    modbus_tx_buffer[2] = exception_code;
    send_modbus(3);
}

void send_modbus(int length)
{
    uint16_t crc = crc16((uint8_t*)modbus_tx_buffer, length);
    modbus_tx_buffer[length] = (uint8_t) crc&0xff;
    modbus_tx_buffer[length+1] = (uint8_t)(crc>>8)&0xff;
    R_SCI_UART_Write(&modbus_uart_ctrl, modbus_tx_buffer, length + 2);
    memset((uint8_t*)modbus_tx_buffer, RESET_VALUE, DATA_LENGTH); // Clear transmit buffer of sent message
    memset((uint8_t*)modbus_rx_buffer, RESET_VALUE, DATA_LENGTH); // Clear modbus receive buffer of processed message
}

void read_input_registers()
{
    uint16_t start_address = (uint16_t)((modbus_rx_buffer[2]<<8)|modbus_rx_buffer[3]);
    uint16_t num_registers = (uint16_t)((modbus_rx_buffer[4]<<8)|modbus_rx_buffer[5]);
    if (num_registers<1 || num_registers > 125)
    {
        modbus_exception(ILLEGAL_DATA_VALUE, modbus_rx_buffer[1]);
    }

    uint16_t end_address = (uint16_t)(start_address + num_registers - 1);
    if (end_address>2)
    {
        modbus_exception(ILLEGAL_DATA_ADDRESS, modbus_rx_buffer[1]);
    }

    modbus_tx_buffer[0] = SLAVE_ID;
    modbus_tx_buffer[1] = modbus_rx_buffer[1];
    modbus_tx_buffer[2] = (uint8_t) num_registers*2;
    int tx_index = 3;
    for (int i = 0; i<num_registers; i++)
    {
        modbus_tx_buffer[tx_index++]= (uint8_t)(input_registers[start_address]>>8)&0xff;
        modbus_tx_buffer[tx_index++]= (uint8_t)(input_registers[start_address])&0xff;
        start_address++;
    }
    send_modbus(tx_index);
}

void read_holding_registers()
{
    uint16_t start_address = (uint16_t)((modbus_rx_buffer[2]<<8)|modbus_rx_buffer[3]);
    uint16_t num_registers = (uint16_t)((modbus_rx_buffer[4]<<8)|modbus_rx_buffer[5]);
    if (num_registers<1 || num_registers > 125)
    {
        modbus_exception(ILLEGAL_DATA_VALUE, modbus_rx_buffer[1]);
    }

    uint16_t end_address = (uint16_t)(start_address + num_registers - 1);
    if (end_address>1)
    {
        modbus_exception(ILLEGAL_DATA_ADDRESS, modbus_rx_buffer[1]);
    }

    modbus_tx_buffer[0] = SLAVE_ID;
    modbus_tx_buffer[1] = modbus_rx_buffer[1];
    modbus_tx_buffer[2] = (uint8_t)num_registers*2;
    int tx_index = 3;
    for (int i = 0; i<num_registers; i++)
    {
        modbus_tx_buffer[tx_index++]= (uint8_t)(holding_registers[start_address]>>8)&0xff;
        modbus_tx_buffer[tx_index++]= (uint8_t)(holding_registers[start_address])&0xff;
        start_address++;
    }
    send_modbus(tx_index);
}




