#include "vcom_interface.h"
#include "vcom.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/dedic_gpio.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_clk_tree.h"

static gptimer_handle_t vcom_timer = NULL;
static dedic_gpio_bundle_handle_t gpio_bundle = NULL;
static const char *TAG = "VCOM";

#define VCOM_TX_GPIO            2        // Using GPIO2 for output
#define VCOM_RX_GPIO            3        // Using GPIO3 for input

static IRAM_ATTR bool vcom_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    extern vcom_handle_t vcom_handle;
    vcom_timer_handler(&vcom_handle);
    return false; // return false to keep the timer running
}

uint8_t vcom_interface_timer_init(uint32_t hz)
{
    esp_err_t ret;
    uint32_t xtal_freq;
    ret = esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL,  ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &xtal_freq);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get XTAL frequency");
        return 1;
    }
    
    if (hz > xtal_freq / 2 || xtal_freq % hz != 0) {
        ESP_LOGE(TAG, "Invalid timer frequency");
        return 2;
    }

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_XTAL,  // using XTAL as clock source for better accuracy
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = hz,
    };
    
    ret = gptimer_new_timer(&timer_config, &vcom_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create timer");
        return 3;
    }

    gptimer_event_callbacks_t cbs = {
        .on_alarm = vcom_timer_callback,
    };
    
    ret = gptimer_register_event_callbacks(vcom_timer, &cbs, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register timer callback");
        return 4;
    }

    return 0;
}

uint8_t vcom_interface_timer_start(void)
{
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1, // Alarm period equals GPTIMER counter resolution
        .reload_count = 0, // Reset to 0 after alarm
        .flags.auto_reload_on_alarm = true,
    };
    
    esp_err_t ret = gptimer_set_alarm_action(vcom_timer, &alarm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set timer alarm");
        return 1;
    }
    
    ret = gptimer_enable(vcom_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable timer");
        return 1;
    }
    
    ret = gptimer_start(vcom_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start timer");
        return 1;
    }
    
    return 0;
}

uint8_t vcom_interface_timer_stop(void)
{
    esp_err_t ret = gptimer_stop(vcom_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop timer");
        return 1;
    }
    
    ret = gptimer_disable(vcom_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disable timer");
        return 1;
    }
    
    return 0;
}

uint8_t vcom_interface_gpio_init(void)
{
    gpio_config_t io_conf = {};
    
    // configure tx GPIO
    io_conf.pin_bit_mask = (1ULL << VCOM_TX_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        return 1;
    }

    // configure rx GPIO
    io_conf.pin_bit_mask = (1ULL << VCOM_RX_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        return 1;
    }

    // create dedicated GPIO bundle for high-speed operation
    const int gpio_array[] = {VCOM_TX_GPIO};
    dedic_gpio_bundle_config_t bundle_config = {
        .gpio_array = gpio_array,
        .array_size = sizeof(gpio_array) / sizeof(gpio_array[0]),
        .flags = {
            .out_en = 1,  // enable output
        },
    };

    ret = dedic_gpio_new_bundle(&bundle_config, &gpio_bundle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create GPIO bundle");
        return 1;
    }

    return 0;
}

uint8_t vcom_interface_gpio_deinit(void)
{
    if (gpio_bundle) {
        dedic_gpio_del_bundle(gpio_bundle);
    }
    
    esp_err_t ret;
    ret = gpio_reset_pin(VCOM_TX_GPIO);
    if (ret != ESP_OK) return 1;
    
    ret = gpio_reset_pin(VCOM_RX_GPIO);
    if (ret != ESP_OK) return 1;
    
    return 0;
}

uint8_t vcom_interface_tx_gpio_write(uint8_t level)
{
    // use dedicated GPIO for fastest possible output
    dedic_gpio_bundle_write(gpio_bundle, 0x01, level ? 0x01 : 0x00);
    return 0;
}

uint8_t vcom_interface_rx_gpio_read(void)
{
    return gpio_get_level(VCOM_RX_GPIO);
}
