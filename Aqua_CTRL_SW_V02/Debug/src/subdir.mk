################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/control_utils.c \
../src/hal_entry.c \
../src/hmi_control.c \
../src/modbus_crc.c \
../src/modbus_protocol.c \
../src/modbus_slave.c \
../src/serial_communication.c 

C_DEPS += \
./src/control_utils.d \
./src/hal_entry.d \
./src/hmi_control.d \
./src/modbus_crc.d \
./src/modbus_protocol.d \
./src/modbus_slave.d \
./src/serial_communication.d 

OBJS += \
./src/control_utils.o \
./src/hal_entry.o \
./src/hmi_control.o \
./src/modbus_crc.o \
./src/modbus_protocol.o \
./src/modbus_slave.o \
./src/serial_communication.o 

SREC += \
Aqua_CTRL_SW_V02.srec 

MAP += \
Aqua_CTRL_SW_V02.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/src" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra/fsp/inc" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra/fsp/inc/api" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra/fsp/inc/instances" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra_gen" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra_cfg/fsp_cfg/bsp" -I"C:/Users/patri/Downloads/amjad_dosing _controller_hmi (3)/Aqua_CTRL_SW_V02/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

