/*
 * modbus_protocol.c
 *
 *  Created on: 28/02/2023
 *      Author: patri
 */

// The interrupt functions defined below implement the logic found on pp. 22-23 of:
// https://www.renesas.com/us/en/document/apn/rl78g14-modbus-asciirtu-r01an5380ej0102-rev102

#include "modbus_protocol.h"

volatile bool g_char_interval_exceeded = false;
volatile bool g_modbus_msg_rcvd = false;

volatile uint8_t buffer[DATA_LENGTH]={RESET_VALUE};

volatile uint16_t g_index = 0;
volatile uint16_t g_length_msg = 0;

volatile uint8_t modbus_rx_buffer[DATA_LENGTH]= {RESET_VALUE};
volatile uint8_t modbus_tx_buffer[DATA_LENGTH] = {RESET_VALUE};

volatile uint16_t holding_registers[2]={RESET_VALUE};
volatile uint16_t input_registers[3]={RESET_VALUE};


// This function is called when the modbus timer sends an interrupt
void modbus_timer_cb(timer_callback_args_t* p_args)
{
    // Check if the timer event is a cycle end event ie. that the period has ended
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        // DEBUG ONLY
        // DELETE ONCE DEBUG COMPLETE
        R_IOPORT_PinWrite(&g_ioport_ctrl, DI_DOSING_EN, BSP_IO_LEVEL_HIGH); // Turn on IO pin


        // If the character interval has been exceeded
        // (indicating that 1000us + 750us = 1750us have already elapsed)
        // and that the FRAME_DELAY (1750us as per Modbus standards) has been exceeded
        if (g_char_interval_exceeded)
        {
            // Reset the flag to false in preparation for receiving the first character of next message
            g_char_interval_exceeded = false;

            // Copy the contents of the uart receive buffer into the modbus receive buffer for message processing
            for(int i = 0; i<=g_index; i++)
            {
                modbus_rx_buffer[i]=buffer[i];
            }

            // Clear the uart receive buffer by setting all elements to RESET_VALUE (0x00)
            memset((uint8_t*)buffer, RESET_VALUE, DATA_LENGTH);

            // Copy the index so that the length of the current message is known and can
            // be used when the current message is processed by the handle_modbus_message function
            g_length_msg = g_index;

            // Reset the uart receive buffer index to zero in preparation for next message
            g_index = 0;


            // Set the modbus message received flag to true so that the main program
            // knows to process the message on the next pass
            g_modbus_msg_rcvd = true;

            // DEBUG ONLY
            // DELETE ONCE DEBUG COMPLETE
            R_IOPORT_PinWrite(&g_ioport_ctrl, DI_DOSING_EN, BSP_IO_LEVEL_LOW); // Turn off IO pin

        }
        else
        {
            // If the character interval has not been exceeded, set the flag to true
            // This indicates that 750us have elapsed
            g_char_interval_exceeded = true;

            // Reset the timer to FRAME_DELAY (1750us) - CHARACTER_TIMEOUT (750us)
            // This means that if a character is not received before the next time this function
            // is called, a FRAME_DELAY (1750us) will have elapsed and the message should be processed
            reset_timer(FRAME_DELAY - CHARACTER_TIMEOUT);

            // DEBUG ONLY
            // DELETE ONCE DEBUG COMPLETE
            R_IOPORT_PinWrite(&g_ioport_ctrl, DI_DOSING_EN, BSP_IO_LEVEL_LOW); // Turn off IO pin
        }
    }
}

// This function is called when the Modbus UART peripheral triggers an interrupt
void modbus_uart_cb(uart_callback_args_t *p_args)
{

    if (startup_complete)
    {
        // Check if event is a received character
        if(UART_EVENT_RX_CHAR == p_args->event)
        {
            // Set the timer to 750us and restart
            reset_timer(CHARACTER_TIMEOUT);

            // If the character interval has been exceeded, reset the buffer and index
            // This indicates that a character has been received more than 750us but
            // less than 1750us since the last character was received. This means that
            // the character is neither a valid part of the current message or the beginning
            // of the next message. Therefore, the modbus UART receive buffer is reset.
            if (g_char_interval_exceeded)
            {
                // Clear the buffer by setting all elements to RESET_VALUE (0x00)
                memset((uint8_t*)buffer, RESET_VALUE, DATA_LENGTH);

                // Reset the index to start writing next message from the beginning of the buffer
                g_index = 0;

            }
            // If the character interval has not been exceeded, add the character to the buffer.
            // This indicates that less than 750us have elapsed since the last character was received,
            // so the character is a valid part of the current message.
            else
            {
                // Add the character to the buffer
                buffer[g_index] = (uint8_t) p_args->data;

                // Increment the index to start writing from the next available position in the buffer
                g_index++;
            }
        }
    }
}

// This function is called to change the period of the timer and restart it
void reset_timer(uint16_t new_period_us)
{
    // Stop the Modbus timer
    R_AGT_Stop(&g_modbus_timer_ctrl);

    // Calculate the frequency of the timer source in Hz
    uint32_t timer_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) >> g_modbus_timer_cfg.source_div;

    // Calculate the number of timer counts needed for the new period
    uint32_t period_counts = (uint32_t)(((uint64_t)timer_freq_hz * new_period_us) / 1000000);

    // Set the new period for the Modbus timer
    R_AGT_PeriodSet(&g_modbus_timer_ctrl, period_counts);

    // Start the Modbus timer
    R_AGT_Start(&g_modbus_timer_ctrl);
}


void handle_modbus_message()
{
    if(g_length_msg > 2)
    {
       // Calculate the CRC of the received message from the contents of the message excluding
       // the received CRC (final two bytes of the message)
       volatile uint16_t crc = crc16((uint8_t*)modbus_rx_buffer, g_length_msg-2);
       volatile uint16_t debug = modbus_rx_buffer[g_length_msg-1] << 8 | modbus_rx_buffer[g_length_msg-2];

       volatile bool b_debug = crc == debug;
       // If the calculated CRC matches the received CRC
       if (crc == (modbus_rx_buffer[g_length_msg-1] << 8 | modbus_rx_buffer[g_length_msg-2]))
       {
           // If the message is addressed to this device
           if (modbus_rx_buffer[0]==SLAVE_ID)
           {
               // Determine the command and take appropriate action
               switch (modbus_rx_buffer[1])
               {
                   case READ_HOLDING_REGISTERS:
                       read_holding_registers();
                       break;
                   case READ_INPUT_REGISTERS:
                       read_input_registers();
                       break;
                   case WRITE_HOLDING_REGISTERS:
                       write_holding_registers();
                       break;
                   default:
                       // If the command is unsupported, return an exception message to the master device
                       modbus_exception(ILLEGAL_FUNCTION, modbus_rx_buffer[1]);
               }
           }
       }
    }
    // Reset the received message flag
    g_modbus_msg_rcvd = false;
}


void modbus_exception(uint8_t exception_code, uint8_t function_code)
{
    modbus_tx_buffer[0] = SLAVE_ID; // Set slave ID in response buffer
    modbus_tx_buffer[1] = function_code | 0x80; // Set function code and set the MSB high to indicate an exception
    modbus_tx_buffer[2] = exception_code; // Set the exception code in the response buffer
    send_modbus(3); // Send the response message
}

void send_modbus(int length)
{
    uint16_t crc = crc16((uint8_t*)modbus_tx_buffer, length); // Calculate the CRC for the message
    modbus_tx_buffer[length] = (uint8_t) crc&0xff; // Append the low byte of the CRC to the message
    modbus_tx_buffer[length+1] = (uint8_t)(crc>>8)&0xff; // Append the high byte of the CRC to the message

//    // Enable RS-485 transmission
//    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_COMM_RX_EN, BSP_IO_LEVEL_HIGH);
//    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_COMM_TX_EN, BSP_IO_LEVEL_HIGH);


    R_SCI_UART_Write(&g_modbus_uart_ctrl, modbus_tx_buffer, length + 2); // Send the message over UART including the CRC bytes

//    // Disable RS-485 transmission
//    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_COMM_RX_EN, BSP_IO_LEVEL_LOW);
//    R_IOPORT_PinWrite(&g_ioport_ctrl, DO_COMM_TX_EN, BSP_IO_LEVEL_LOW);


    memset((uint8_t*)modbus_tx_buffer, RESET_VALUE, DATA_LENGTH); // Clear transmit buffer of message that has just been sent
    memset((uint8_t*)modbus_rx_buffer, RESET_VALUE, DATA_LENGTH); // Clear modbus receive buffer of message that has just been processed
}


void read_input_registers()
{
    // Extract starting address and number of registers to read from message
    uint16_t start_address = (uint16_t)((modbus_rx_buffer[2]<<8)|modbus_rx_buffer[3]);
    uint16_t num_registers = (uint16_t)((modbus_rx_buffer[4]<<8)|modbus_rx_buffer[5]);

    // Check if number of registers is valid
    if (num_registers<1 || num_registers > 125)
    {
        modbus_exception(ILLEGAL_DATA_VALUE, modbus_rx_buffer[1]); // Send exception response if not valid
    }

    // Calculate ending address of register range to read
    uint16_t end_address = (uint16_t)(start_address + num_registers - 1);
    if (end_address>2)
    {
        modbus_exception(ILLEGAL_DATA_ADDRESS, modbus_rx_buffer[1]); // Send exception response if address range is invalid
    }

    // Construct response message header
    modbus_tx_buffer[0] = SLAVE_ID;
    modbus_tx_buffer[1] = modbus_rx_buffer[1]; // Modbus function code received from master
    modbus_tx_buffer[2] = (uint8_t) num_registers*2; // Data size in bytes

    // Loop over the range of input registers and add register values to response message
    int tx_index = 3;
    for (int i = 0; i<num_registers; i++)
    {
        modbus_tx_buffer[tx_index++]= (uint8_t)(input_registers[start_address]>>8)&0xff; // High byte
        modbus_tx_buffer[tx_index++]= (uint8_t)(input_registers[start_address])&0xff; // Low byte
        start_address++;
    }

    // Send response message
    send_modbus(tx_index);
}


// Function to read holding registers
void read_holding_registers()
{
    // Extract start address and number of registers from the received Modbus message
    uint16_t start_address = (uint16_t)((modbus_rx_buffer[2]<<8)|modbus_rx_buffer[3]);
    uint16_t num_registers = (uint16_t)((modbus_rx_buffer[4]<<8)|modbus_rx_buffer[5]);

    // Check if number of registers requested is within the allowed range (1-125)
    if (num_registers<1 || num_registers > 125)
    {
        // If not, send an exception response with ILLEGAL_DATA_VALUE error code (0x03)
        modbus_exception(ILLEGAL_DATA_VALUE, modbus_rx_buffer[1]);
    }

    // Calculate the end address of the requested range of registers
    uint16_t end_address = (uint16_t)(start_address + num_registers - 1);

    // Check if the end address is within the allowed range (0-1)
    if (end_address>1)
    {
        // If not, send an exception response with ILLEGAL_DATA_ADDRESS error code (0x02)
        modbus_exception(ILLEGAL_DATA_ADDRESS, modbus_rx_buffer[1]);
    }

    // If start and end addresses are valid, create a Modbus response with requested data
    // Set the slave ID in the response buffer
    modbus_tx_buffer[0] = SLAVE_ID;
    // Set the function code in the response buffer
    modbus_tx_buffer[1] = modbus_rx_buffer[1];
    // Set the number of bytes in the response buffer (twice the number of registers)
    modbus_tx_buffer[2] = (uint8_t)num_registers*2;
    // Initialize the index for writing data into the response buffer
    int tx_index = 3;
    // Loop through each requested register and add its value to the response buffer
    for (int i = 0; i<num_registers; i++)
    {
        // Get the value of the current holding register
        uint16_t value = holding_registers[start_address];
        // Add the high byte of the value to the response buffer
        modbus_tx_buffer[tx_index++]= (uint8_t)(value>>8)&0xff;
        // Add the low byte of the value to the response buffer
        modbus_tx_buffer[tx_index++]= (uint8_t)(value)&0xff;
        // Move to the next holding register
        start_address++;
    }
    // Send the Modbus response with requested data
    send_modbus(tx_index);
}

void write_holding_registers()
{
    uint16_t startAddr = ((modbus_rx_buffer[2]<<8)|modbus_rx_buffer[3]);  // Get address of starting register
    uint16_t numRegs = ((modbus_rx_buffer[4]<<8)|modbus_rx_buffer[5]);   // Get number of registers to write
    if ((numRegs<1)||(numRegs>123))  // Check if number of registers is within acceptable range as per Modbus standard
    {
        // If not, send an exception response with ILLEGAL_DATA_VALUE error code (0x03)
        modbus_exception(ILLEGAL_DATA_VALUE, modbus_rx_buffer[1]);
    }

    uint16_t endAddr = startAddr + numRegs - 1;  // Calculate end address of register
    if (endAddr > 4)  // Check if end address is within acceptable range
    {
        // If not, send an exception response with ILLEGAL_DATA_ADDRESS error code (0x02)
        modbus_exception(ILLEGAL_DATA_ADDRESS, modbus_rx_buffer[1]);
    }

    // Store 16-bit data starting from modbus_rx_buffer[6]
    // 16 bit Data = firstByte<<8|secondByte
    // Data is stored in the holding registers database
    int rx_index = 6;  // Keep track of index in modbus_rx_buffer
    for (int i = 0; i < numRegs; i++)
    {
        holding_registers[startAddr++] = (modbus_rx_buffer[rx_index++] << 8) | modbus_rx_buffer[rx_index++];
    }

    // Prepare response to send back to master
    // Format: | SLAVE_ID | FUNCTION_CODE | Start Addr | num of Regs    | CRC     |
    //         | 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES      | 2 BYTES |
    modbus_tx_buffer[0] = SLAVE_ID;    // Slave ID
    modbus_tx_buffer[1] = modbus_rx_buffer[1];   // Function code
    modbus_tx_buffer[2] = modbus_rx_buffer[2];   // Start address high byte
    modbus_tx_buffer[3] = modbus_rx_buffer[3];   // Start address low byte
    modbus_tx_buffer[4] = modbus_rx_buffer[4];   // Number of registers high byte
    modbus_tx_buffer[5] = modbus_rx_buffer[5];   // Number of registers low byte

    send_modbus(6);  // Send response back to master. CRC will be calculated in the send function
}



