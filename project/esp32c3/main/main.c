#include "vcom.h"
#include "vcom_interface.h"
#include "esp_log.h"
#include "esp_task_wdt.h"

#define RX_DATA_BUFFER_SIZE 80

static const char *TAG = "VCOM_MAIN";
vcom_handle_t vcom_handle = {0};

void app_main(void)
{
    static uint8_t rx_buf[RX_DATA_BUFFER_SIZE];
    
    // initialize VCOM handle
    vcom_handle.irq_timer_init = vcom_interface_timer_init;
    vcom_handle.irq_timer_start = vcom_interface_timer_start;
    vcom_handle.irq_timer_stop = vcom_interface_timer_stop;
    vcom_handle.gpio_init = vcom_interface_gpio_init;
    vcom_handle.gpio_deinit = vcom_interface_gpio_deinit;
    vcom_handle.tx_gpio_write = vcom_interface_tx_gpio_write;
    vcom_handle.rx_gpio_read = vcom_interface_rx_gpio_read;

    uint8_t ret = vcom_init(&vcom_handle);
    if (ret != 0) {
        ESP_LOGE(TAG, "VCOM init failed with error: %d", ret);
        return;
    }
    
    vcom_transmit(&vcom_handle, (uint8_t *)"Hello World!\n", 13);

    // main loop
    while (1) {
        while (vcom_handle.tx_state != VCOM_TX_STATE_IDLE) {
            // delay for feed watchdog
            vTaskDelay(1);
        }
        vcom_receive(&vcom_handle, rx_buf, RX_DATA_BUFFER_SIZE);
        
        while (vcom_handle.rx_state != VCOM_RX_STATE_IDLE) {
            vTaskDelay(1);
        }
        if (vcom_handle.rx_size > 0) {
            vcom_transmit(&vcom_handle, rx_buf, vcom_handle.rx_size);
        }
    }
}