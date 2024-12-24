#ifndef __VCOM_CONF_H
#define __VCOM_CONF_H

#include "vcom.h"

/**
 * @brief usart baudrate
 */
#define VCOM_BAUDRATE 19200

/**
 * @brief hardware timer interrupt frequency for usart transfer and capture
 */
#define VCOM_TIMER_IRQ_FREQ 100000

/**
 * @brief frame format setting
 *
 */
#define VCOM_FRAME_FORMAT VCOM_FRAME_8N1

#endif /* __VCOM_CONF_H */
