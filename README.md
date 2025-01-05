[English](README.md) | [中文](README_cn.md)

# _VCOM_

A simple software UART driver, data is asynchronous send/receive in hardware timer interrupt, easy to port to different MCU, and support basic baudrate and parity config.

### TODO

- [ ] Check the logical validity of configs.
- [ ] Add async tx/rx finish callback interface.

### Instruction

/src include VCOM source files.

/interface includes VCOM config and platform interface template.

/test includes VCOM test code that check feasibility of config and test basic function.

/example include VCOM sample code.

/project includes common MCU development board sample code. All projects are based VSCode IDE, integrated by CMake, and could build with GCC.

### Install

Reference /interface platform independent template and add your platform GPIO and timer driver.

Add the /src directory, interface driver for your platform, and your own drivers to your project. If you want to use the default example functions, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. Take care of these 2 config:

- UART baudrate (defined by `VCOM_BAUDRATE`) : The VCOM will transmit/capture UART signal at timing defined by baudrate as possible, but limit by timer interrupt frequency. Interrupt frequency should be 5 times of baudrate at least, even multiple scale is better.
- timer interrupt frequency (defined by `VCOM_TIMER_IRQ_FREQ`): This macro should refered in timer interface driver by yourself. Although higher interrupt frequency usually brings higher signal precision, but too higher will cause tasks could not get enough CPU time , even lead queue the interrupt itself. Suggest frequency belows 1/20 CPU clock.

Because the difference of MCU in GPIO toggle capability, CPU speed and instruction efficiency, recommend to run test cases in /test directory to confirm feasible of config before apply VCOM.

Notice the `vcom_timer_handler` function in /src should called in corresponding interrupt entry. And config a high timer interrupt priority in `vcom_interface_timer_init` that ensure handler will not blocked by other interrupt in the same system. Avoiding indirect callback of handler and put it into RAM if possible for getting higher baudrate.

### Contributing  

Bug issue/PR are welcome. VCOM is build for universal applicable and easy usage, more complesxity is unacceptable, so please fork for adding feature.
