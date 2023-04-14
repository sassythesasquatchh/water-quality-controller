################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_agt/r_agt.c 

C_DEPS += \
./ra/fsp/src/r_agt/r_agt.d 

OBJS += \
./ra/fsp/src/r_agt/r_agt.o 

SREC += \
rebuild.srec 

MAP += \
rebuild.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_agt/%.o: ../ra/fsp/src/r_agt/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -gdwarf-4 -D_RA_CORE=CM33 -D_RENESAS_RA_ -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/src" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra/fsp/inc" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra/fsp/inc/api" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra/fsp/inc/instances" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra/arm/CMSIS_5/CMSIS/Core/Include" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra_gen" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra_cfg/fsp_cfg/bsp" -I"C:/Users/patri/OneDrive/Patricks OneDrive Share/Documents/water-quality-controller/water-quality-controller/rebuild/ra_cfg/fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

