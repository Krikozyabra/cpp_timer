# cpp_timer
Devices:
1) MTU - stm32f103c8t6
2) 7 segment 4 digit display
3) 2 buttons
4) 1 led

Was using KEIL IDE \
Compiler control settings:

C++17 and C/99

-xc -std=c99 --target=arm-arm-none-eabi -mcpu=cortex-m3 -c
-fno-rtti -funsigned-char -fshort-enums -fshort-wchar
-D__EVAL -gdwarf-4 -O3 -ffunction-sections -Wall -Wextra -Wno-packed -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier
-I./RTE/Device/STM32F103C8
-I./RTE/_Target_1
-ID:/Arm/Packs/ARM/CMSIS/6.2.0/CMSIS/Core/Include
-ID:/Arm/Packs/Keil/STM32F1xx_DFP/2.4.1/Device/Include
-D__UVISION_VERSION="543" -DSTM32F10X_MD -D_RTE_
-o ./Objects/*.o -MMD
