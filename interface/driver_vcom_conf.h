#ifndef __DRIVER_VCOM_CONF_H
#define __DRIVER_VCOM_CONF_H

#include "driver_vcom_interface.h"

/**
 * @brief vcom baudrate
 * @note should set relative low value test if timer interrupt block first
 */
#define VCOM_BAUDRATE 19200

/**
 * @brief hardware timer interrupt frequency(interrupt timers per second) for
 *        vcom transfer and capture
 * @note suggest be 3 times of vcom baudrate at least
 */
#define VCOM_TIMER_IRQ_FREQ 100000

/**
 * @brief frame format setting
 *
 */
#define VCOM_FRAME_FORMAT VCOM_FRAME_8N1

#endif /* __DRIVER_VCOM_CONF_H */