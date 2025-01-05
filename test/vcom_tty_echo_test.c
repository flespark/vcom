#include "vcom.h"
#include "driver_vcom_interface.h"

#define VCOM_TTY_ECHO_TEST_COUNT 20

vcom_handle_t vcom_handle;

static uint8_t rx_buf[1];

/**
 * @brief used for test if vcom communication speed as expected. before run this
 *        test, connect tx and rx pin of vcom to serial port of computer. open a
 *        echo serial terminal software with corresponding baudrate and parity,
 *        open the port. run this test, then type something, you should see the
 *        same thing in terminal.
 * @return void
 */
void vcom_timer_irq_feasible_test(void)
{
    printf("vcom_timer_irq_feasible_test start\n");
    
    vcom_handle.irq_timer_init = vcom_interface_timer_init;
    vcom_handle.irq_timer_start = vcom_interface_timer_start;
    vcom_handle.irq_timer_stop = vcom_interface_timer_stop;
    vcom_handle.gpio_init = vcom_interface_gpio_init;
    vcom_handle.gpio_deinit = vcom_interface_gpio_deinit;
    vcom_handle.tx_gpio_write = vcom_interface_tx_gpio_write;
    vcom_handle.rx_gpio_read = vcom_interface_rx_gpio_read;
    if (vcom_init(&vcom_handle) != 0) {
      printf("vcom_init failed\n");
      goto exit;
    }

    vcom_transmit(&vcom_handle, (uint8_t *)"Hello, World!\n", 14);
    
    for (uint32_t i = 0; i < VCOM_TTY_ECHO_TEST_COUNT; i++) {
        while (vcom_handle.tx_state != VCOM_TX_STATE_IDLE);
        vcom_receive(&vcom_handle, rx_buf, 1);
        while (vcom_handle.rx_state != VCOM_RX_STATE_IDLE);
        if (vcom_handle.rx_error_flag != 0) {
            printf("rx error flag is not 0, vcom_tty_echo_test failed\n");
            goto exit;
        }
        if (vcom_handle.rx_size > 0) {
            vcom_transmit(&vcom_handle, rx_buf, 1);
        }
    }

    printf("vcom_tty_echo_test passed\n");

exit:
    vcom_deinit(&vcom_handle);
}