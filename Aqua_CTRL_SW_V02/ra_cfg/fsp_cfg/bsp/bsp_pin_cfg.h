/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define DO_PH_SENSOR_ON (BSP_IO_PORT_01_PIN_00)
#define DO_CONDUCTIV_SENSOR_ON (BSP_IO_PORT_01_PIN_01)
#define DO_BCD_A (BSP_IO_PORT_01_PIN_02)
#define DO_BCD_B (BSP_IO_PORT_01_PIN_03)
#define DO_BCD_C (BSP_IO_PORT_01_PIN_04)
#define DO_BCD_D (BSP_IO_PORT_01_PIN_05)
#define DO_V_CONDUCTIV_PUMP_ON (BSP_IO_PORT_01_PIN_10)
#define DO_V_PH_PUMP_ON (BSP_IO_PORT_01_PIN_11)
#define DI_UART_RX_ESP32 (BSP_IO_PORT_03_PIN_01)
#define DO_UART_TX_ESP32 (BSP_IO_PORT_03_PIN_02)
#define DO_ESP32_EN (BSP_IO_PORT_03_PIN_03)
#define DO_7XSEGMENT_3_EN (BSP_IO_PORT_04_PIN_00)
#define DO_7XSEGMENT_2_EN (BSP_IO_PORT_04_PIN_01)
#define DO_7XSEGMENT_1_EN (BSP_IO_PORT_04_PIN_02)
#define DO_CTRL_YELLOW_STATUS_LED (BSP_IO_PORT_04_PIN_03)
#define DO_CTRL_Green_STATUS_LED (BSP_IO_PORT_04_PIN_04)
#define DO_CTRL_RED_STATUS_LED (BSP_IO_PORT_04_PIN_05)
#define DO_CONDUCTIV_SENSOR_DISPLAY_ON (BSP_IO_PORT_04_PIN_06) /* switches LED on, Indicating that the Conductivity sensor is switched on. */
#define DI_USART_RX (BSP_IO_PORT_04_PIN_08)
#define DO_USART_TX (BSP_IO_PORT_04_PIN_09)
#define DO_COMM_TX_EN (BSP_IO_PORT_04_PIN_10)
#define DO_COMM_RX_EN (BSP_IO_PORT_04_PIN_11) /* Active Low */
#define DI_DOSING_EN (BSP_IO_PORT_04_PIN_12) /* Digital Input */
#define AI_I_SENSE_PH_SENSOR (BSP_IO_PORT_05_PIN_00)
#define AI_V_PH_SENSOR (BSP_IO_PORT_05_PIN_01)
#define AI_24V0 (BSP_IO_PORT_05_PIN_02)
#define AI_I_SENSE_CONDUTIV_SENSOR (BSP_IO_PORT_05_PIN_03)
#define AI_V_CONDUCTIV_SENSOR (BSP_IO_PORT_05_PIN_04)
#define DO_CONDUCTIV_PUMP_ON_LED (BSP_IO_PORT_07_PIN_00) /* switches on LED, indicating Conductiv. Pump is on */
#define DO_PH_SENSOR_DISPLAY_ON (BSP_IO_PORT_07_PIN_01) /* switches on LED, indicating PH Sensor is switched On */
#define DO_PH_PUMP_ON_LED (BSP_IO_PORT_07_PIN_02) /* switches on LED, indicating PH Pump is on */
extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA6M5BG3CFB.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif /* BSP_PIN_CFG_H_ */
