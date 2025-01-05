#include "driver_vcom_interface.h"
/* add neccessary include */

/**
 * @brief initialize the periodical interrupt timer for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 vcom init fail
 */
uint8_t vcom_interface_timer_init(void)
{
    /* add neccessary code */
    return 0;
}

/**
 * @brief start the periodical interrupt timer for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 timer start fail
 */
uint8_t vcom_interface_timer_start(void)
{
    /* add neccessary code */
    return 0;
}

/**
 * @brief stop the periodical interrupt timer for tx/rx action of vcom
 * @note should be called when vcom is not in use, otherwise overwhelming timer
 *       interrupt may leads system abnormal
 * @return status code
 *          - 0 success
 *          - 1 timer stop fail
 */
uint8_t vcom_interface_timer_stop(void)
{
    /* add neccessary code */
    return 0;
}

/**
 * @brief initialize the gpio for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 gpio init fail
 */
uint8_t vcom_interface_gpio_init(void)
{
    /* add neccessary code */
    return 0;
}

/**
 * @brief deinitialize the gpio for tx/rx action of vcom
 * @return status code
 *          - 0 success
 *          - 1 gpio deinit fail
 */
uint8_t vcom_interface_gpio_deinit(void)
{
    return 0;
}

/**
 * @brief write the tx gpio for vcom
 * @param level: 0 or 1
 * @return status code
 *          - 0 success
 *          - 1 tx gpio write fail
 */
uint8_t vcom_interface_tx_gpio_write(uint8_t level)
{
    /* add neccessary code */
    return 0;
}

/**
 * @brief read the rx gpio for vcom
 * @return status code
 *          - 0 success
 *          - 1 rx gpio read fail
 */
uint8_t vcom_interface_rx_gpio_read(void)
{
    /* add neccessary code */
    return 0;
}
