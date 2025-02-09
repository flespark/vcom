#include "driver_vcom.h"

vcom_handle_t vcom_handle = {0};

/**
 * @brief initialize the vcom driver
 * @param baudrate: UART baudrate
 * @param timer_irq_freq: signal transmit/capture periodic timer interrupt frequency
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_init(uint32_t baudrate, uint32_t timer_irq_freq)
{
  vcom_handle.irq_timer_init = vcom_interface_timer_init;
  vcom_handle.irq_timer_start = vcom_interface_timer_start;
  vcom_handle.irq_timer_stop = vcom_interface_timer_stop;
  vcom_handle.gpio_init = vcom_interface_gpio_init;
  vcom_handle.gpio_deinit = vcom_interface_gpio_deinit;
  vcom_handle.tx_gpio_write = vcom_interface_tx_gpio_write;
  vcom_handle.rx_gpio_read = vcom_interface_rx_gpio_read;
  return vcom_init(&vcom_handle, baudrate, timer_irq_freq);
}

/**
 * @brief send data over vcom blocking until transmit complete
 * @param data pointer to the data to send
 * @param size size of the data to send
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_send_poll(uint8_t *data, uint32_t size)
{
    vcom_transmit(&vcom_handle, data, size);
    while (vcom_handle.tx_state != VCOM_TX_STATE_IDLE);

    return 0;
}

/**
 * @brief receive data over vcom blocking until expected size or complete
 * @param data pointer to the data to receive
 * @param size size of the data to receive
 * @return 0 if successful, otherwise an error code
 */
uint8_t driver_vcom_receive_poll(uint8_t *data, uint32_t size)
{
    vcom_receive(&vcom_handle, data, size);
    while (vcom_handle.rx_state != VCOM_RX_STATE_IDLE) {
        /* add neccessary timeout check, manual reset rx state required if timeout */
    };
    return 0;
}
