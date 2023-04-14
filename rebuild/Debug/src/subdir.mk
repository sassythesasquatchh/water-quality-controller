################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/application_data.c \
../src/control_utils.c \
../src/dosing_control.c \
../src/hal_entry.c \
../src/hmi_control.c \
../src/modbus_crc.c \
../src/modbus_protocol.c \
../src/serial_communication.c 

C_DEPS += \
./src/application_data.d \
./src/control_utils.d \
./src/dosing_control.d \
./src/hal_entry.d \
./src/hmi_control.d \
./src/modbus_crc.d \
./src/modbus_protocol.d \
./src/serial_communication.d 

OBJS += \
./src/application_data.o \
./src/control_utils.o \
./src/dosing_control.o \
./src/hal_entry.o \
./src/hmi_control.o \
./src/modbus_crc.o \
./src/modbus_protocol.o \
./src/serial_communication.o 

SREC += \
rebuild.srec 

MAP += \
rebuild.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/src" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra/fsp/inc" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra/fsp/inc/api" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra/fsp/inc/instances" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra_gen" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra_cfg/fsp_cfg/bsp" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/rebuild/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"
