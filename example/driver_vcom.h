#ifndef __DRIVER_VCOM_H
#define __DRIVER_VCOM_H

#include <stdint.h>
#include "vcom.h"
#include "driver_vcom_interface.h"

#ifdef __cplusplus
extern "C"{
#endif
    
/**
 * @brief initialize the vcom driver
 * @param baudrate: UART baudrate
 * @param timer_irq_freq: signal transmit/capture periodic timer interrupt frequency
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_init(uint32_t baudrate, uint32_t timer_irq_freq);

/**
 * @brief send data over vcom using polling
 * @param data pointer to the data to send
 * @param size size of the data to send
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_send_poll(uint8_t *data, uint32_t size);

/**
 * @brief receive data over vcom using polling
 * @param data pointer to the data to receive
 * @param size size of the data to receive
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_receive_poll(uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_VCOM_H */