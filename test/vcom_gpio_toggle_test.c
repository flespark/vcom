#include "vcom.h"
#include "driver_vcom_interface.h"

#define VCOM_TEST_TOGGLE_TIMES 1000000

vcom_handle_t vcom_handle;

/**
 * @brief used for test if vcom gpio read/write is working. before run this test,
 *        connect tx pin to rx pin.
 * @return void
 */
void vcom_gpio_toggle_test(void)
{
    printf("vcom_gpio_toggle_test start\n");

    vcom_handle.irq_timer_init = vcom_interface_timer_init;
    vcom_handle.irq_timer_start = vcom_interface_timer_start;
    vcom_handle.irq_timer_stop = vcom_interface_timer_stop;
    vcom_handle.gpio_init = vcom_interface_gpio_init;
    vcom_handle.gpio_deinit = vcom_interface_gpio_deinit;
    vcom_handle.tx_gpio_write = vcom_interface_tx_gpio_write;
    vcom_handle.rx_gpio_read = vcom_interface_rx_gpio_read;
    if (vcom_init(&vcom_handle) != 0) {
      printf("vcom_init failed\n");
      return;
    }
    
    for (uint32_t i = 0; i < VCOM_TEST_TOGGLE_TIMES; i++) {
        vcom_handle.tx_gpio_write(i & 0x1);
        /* NOTE: if failed, add delay to exclude the suspect of gpio toggle lag
         */
        if (vcom_handle.rx_gpio_read() != (i & 0x1)) {
            printf("vcom_gpio_toggle_test failed, process: %u\n", i);
            goto exit;
        }
    }

    printf("vcom_gpio_toggle_test passed\n");
    
exit:
    vcom_deinit(&vcom_handle);
}