
C_SOURCES = $(wildcard ./Core/Src/*.c)
C_SOURCES += $(wildcard ./Drivers/STM32F1xx_HAL_Driver/Src/*.c) 

C_INCLUDES =  \
-I./Core/Inc \
-I./Drivers/STM32F1xx_HAL_Driver/Inc \
-I./Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-I./Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-I./Drivers/CMSIS/Include \
-I./Drivers/EEPROM

C_DEFS =  \
-DUSE_FULL_LL_DRIVER \
-DSTM32F103xB

ASM_SOURCES = ./Core/Startup/startup_stm32f103rbtx.s
AS_INCLUDES =
AS_DEFS = 

LDSCRIPT = ./STM32F103RBTX_FLASH.ld