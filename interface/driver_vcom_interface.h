#ifndef __DRIVER_VCOM_INTERFACE_H
#define __DRIVER_VCOM_INTERFACE_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

/**
 * @brief initialize the timer for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 timer init fail
 */
uint8_t vcom_interface_timer_init(void);

/**
 * @brief start the periodical interrupt timer for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 timer start fail
 */
uint8_t vcom_interface_timer_start(void);

/**
 * @brief stop the periodical interrupt timer for tx/rx action of vcom
 * @note should be called when vcom is not in use, otherwise overwhelming timer
 *       interrupt may leads system abnormal
 * @return status code
 *          - 0 success
 *          - 1 timer stop fail
 */
uint8_t vcom_interface_timer_stop(void);

/**
 * @brief initialize the gpio for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 gpio init fail
 */
uint8_t vcom_interface_gpio_init(void);

/**
 * @brief deinitialize the gpio for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 gpio deinit fail
 */
uint8_t vcom_interface_gpio_deinit(void);

/**
 * @brief write the tx gpio for vcom
 * @param level: 0 or 1
 * @return status code
 *          - 0 success
 *          - 1 tx gpio write fail
 */
uint8_t vcom_interface_tx_gpio_write(uint8_t level);

/**
 * @brief read the rx gpio for vcom
 * @return status code
 *          - 0 success
 *          - 1 rx gpio read fail
 */
uint8_t vcom_interface_rx_gpio_read(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_VCOM_INTERFACE_H */