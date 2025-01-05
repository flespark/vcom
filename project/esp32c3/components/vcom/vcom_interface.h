#ifndef __VCOM_INTERFACE_H
#define __VCOM_INTERFACE_H

#ifdef __cplusplus
extern "C"{
#endif

#include "stdint.h"

uint8_t vcom_interface_timer_init(void);
uint8_t vcom_interface_timer_start(void);
uint8_t vcom_interface_timer_stop(void);
uint8_t vcom_interface_gpio_init(void);
uint8_t vcom_interface_gpio_deinit(void);
uint8_t vcom_interface_tx_gpio_write(uint8_t level);
uint8_t vcom_interface_rx_gpio_read(void);

#ifdef __cplusplus
}
#endif

#endif /* __VCOM_INTERFACE_H */
