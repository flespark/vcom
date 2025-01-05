#!/bin/sh

# 1. download the binary file to the stm32f103 by stm32_programmer_cli
STM32_Programmer_CLI.exe -c port=SWD -w build/vcom-stm32f1.elf 0x08000000 -rst