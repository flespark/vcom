#include "vcom.h"
#include "driver_vcom_conf.h"

#define VCOM_PAYLOAD_FRAME_BITS 8

#if (VCOM_FRAME_FORMAT == VCOM_FRAME_8O1)
#define VCOM_PAYLOAD_CHECKSUM VCOM_PAYLOAD_CHECKSUM_TYPE_ODD
#elif (VCOM_FRAME_FORMAT == VCOM_FRAME_8E1)
#define VCOM_PAYLOAD_CHECKSUM VCOM_PAYLOAD_CHECKSUM_TYPE_EVEN
#endif

#ifdef VCOM_PAYLOAD_CHECKSUM
#define VCOM_MTU_FRAME_BITS (VCOM_PAYLOAD_FRAME_BITS + 1)
#else
#define VCOM_MTU_FRAME_BITS VCOM_PAYLOAD_FRAME_BITS
#endif

#define VCOM_MAC_FRAME_BITS (VCOM_MTU_FRAME_BITS + 2)

/**
 * @brief calculate the least common multiple of two numbers
 *
 * @param x
 * @param y
 * @return uint32_t
 */
static inline uint32_t lcm(uint32_t x, uint32_t y)
{
    uint32_t temp;
    uint32_t a = x;
    uint32_t b = y;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return (x / a) * y;
}

/**
 * @brief tx pin bit transfer, called by timer interrupt
 *
 * @param handle: vcom handle
 */
static inline void vcom_bit_transfer(vcom_handle_t *handle)
{
    static uint8_t tx_bit;

    if (handle->tx_state == VCOM_TX_STATE_IDLE) {
        return;
    }

    if (handle->tx_bit_shift == -1) {
        handle->tx_gpio_write(0);
        handle->tx_bit_pcnt = 0;
        handle->tx_bit_shift++;
    } else if (handle->tx_bit_shift < VCOM_PAYLOAD_FRAME_BITS) {
        tx_bit = (handle->tx_buf[handle->tx_buf_index] >> handle->tx_bit_shift) & 0x01;
        handle->tx_gpio_write(tx_bit);
        if (tx_bit) {
            handle->tx_bit_pcnt++;
        }
        handle->tx_bit_shift++;
#ifdef VCOM_PAYLOAD_CHECKSUM
    } else if (handle->tx_bit_shift < VCOM_MTU_FRAME_BITS) {
        tx_bit = handle->tx_bit_pcnt & 0x01;
#if (VCOM_PAYLOAD_CHECKSUM == VCOM_PAYLOAD_CHECKSUM_TYPE_ODD)
        handle->tx_gpio_write(!tx_bit);
#elif (VCOM_PAYLOAD_CHECKSUM == VCOM_PAYLOAD_CHECKSUM_TYPE_EVEN)
        handle->tx_gpio_write(tx_bit);
#endif
        handle->tx_bit_shift++;
#endif
    } else {
        handle->tx_gpio_write(1);
        handle->tx_bit_shift = -1;
        handle->tx_buf_index++;
        if (handle->tx_buf_index >= handle->tx_size) {
            /* next sample after stop bit tx signal must keep high */
            handle->next_transfer_tick += handle->sample_count;
            handle->tx_state = VCOM_TX_STATE_IDLE;
            return;
        }
    }

    if (handle->tx_state == VCOM_TX_STATE_WAIT_START) {
        handle->next_transfer_tick = handle->now_tick + handle->sample_count;
        handle->tx_state = VCOM_TX_STATE_TRIGGERED;
    } else {
        handle->next_transfer_tick += handle->sample_count;
    }
    if (handle->next_transfer_tick >= handle->wrap_count) {
        handle->next_transfer_tick -= handle->wrap_count;
    }
}

/**
 * @brief rx pin bit capture, called by timer interrupt
 *
 * @param handle: vcom handle
 */
static inline void vcom_bit_capture(vcom_handle_t *handle)
{
    static uint8_t rx_bit;
    static uint8_t rx_byte;

    if (handle->rx_state == VCOM_RX_STATE_IDLE) {
        return;
    }

    if (handle->rx_bit_shift == -1) {
        rx_bit = handle->rx_gpio_read();
        if (rx_bit != 0) {
            if (handle->rx_buf_index != 0) {
                /* being triggered and subsequent bit is 1, stop rx */
                goto stop_rx;
            } else {
                /* wait start bit */
                handle->next_capture_tick = (handle->now_tick + handle->step_count);
                if (handle->next_capture_tick >= handle->wrap_count) {
                    handle->next_capture_tick = 0;
                }
                return;
            }
        }
        /* else bit is 0, rx trigger */
        rx_byte = 0;
        handle->rx_bit_pcnt = 0;
        handle->rx_bit_shift++;
    } else if (handle->rx_bit_shift < VCOM_PAYLOAD_FRAME_BITS) {
        rx_bit = handle->rx_gpio_read();
        rx_byte |= (rx_bit << handle->rx_bit_shift);
        handle->rx_bit_pcnt += rx_bit;
        handle->rx_bit_shift++;
#ifdef VCOM_PAYLOAD_CHECKSUM
    } else if (handle->rx_bit_shift < VCOM_MTU_FRAME_BITS) {
        rx_bit = handle->rx_gpio_read();
        handle->rx_bit_pcnt = handle->rx_bit_pcnt & 0x01;
#if (VCOM_PAYLOAD_CHECKSUM == VCOM_PAYLOAD_CHECKSUM_TYPE_ODD)
        handle->rx_bit_pcnt = !handle->rx_bit_pcnt;
#endif
        if (rx_bit != handle->rx_bit_pcnt) {
            handle->rx_error_flag |= VCOM_FRAME_ERROR_CHECKSUM;
        }
        handle->rx_bit_shift++;
#endif
    } else {
        rx_bit = handle->rx_gpio_read();
        if (rx_bit == 0) {
            handle->rx_error_flag |= VCOM_FRAME_ERROR_STOP;
        }
        if (handle->rx_buf_index < handle->rx_size) {
            handle->rx_buf[handle->rx_buf_index] = rx_byte;
        } else {
            handle->rx_error_flag |= VCOM_FRAME_ERROR_OVERFLOW;
            goto stop_rx;
        }

        handle->rx_bit_shift = -1;
        handle->rx_buf_index++;
    }

    if (handle->rx_state == VCOM_RX_STATE_WAIT_START) {
        handle->next_capture_tick =
            handle->now_tick + handle->sample_count + handle->sample_count / 2;
        handle->rx_state = VCOM_RX_STATE_TRIGGERED;
    } else {
        handle->next_capture_tick += handle->sample_count;
    }
    if (handle->next_capture_tick >= handle->wrap_count) {
        handle->next_capture_tick -= handle->wrap_count;
    }
    return;

stop_rx:
    handle->rx_size = handle->rx_buf_index;
    /* disable capture check, boost irq handler efficiency */
    handle->next_capture_tick = handle->wrap_count;
    handle->rx_state = VCOM_RX_STATE_IDLE;
    return;
}

/**
 * @brief initialize vcom instance
 *
 * @param handle: vcom handle
 * @return status code
 *         - 0: success
 *         - 1: already initialized
 *         - 2: irq timer init not set
 *         - 3: irq timer start not set
 *         - 4: irq timer stop not set
 *         - 5: gpio init not set
 *         - 6: gpio deinit not set
 *         - 7: tx gpio write function not set
 *         - 8: rx gpio read function not set
 *         - 9: irq timer init failed
 *         - 10: gpio init failed
 *         - 11: irq timer start failed
 */
uint8_t vcom_init(vcom_handle_t *handle)
{
    if (handle->inited) {
        return 1;
    }
    if (handle->irq_timer_init == NULL) {
        return 2;
    }
    if (handle->irq_timer_start == NULL) {
        return 3;
    }
    if (handle->irq_timer_stop == NULL) {
        return 4;
    }
    if (handle->gpio_init == NULL) {
        return 5;
    }
    if (handle->gpio_deinit == NULL) {
        return 6;
    }
    if (handle->tx_gpio_write == NULL) {
        return 7;
    }
    if (handle->rx_gpio_read == NULL) {
        return 8;
    }

    if (handle->irq_timer_init() != 0) {
        return 9;
    }
    if (handle->gpio_init() != 0) {
        return 10;
    }

    /* TODO: timer irq freq and baudrate validate */
    handle->step_count = VCOM_BAUDRATE;
    handle->sample_count = VCOM_TIMER_IRQ_FREQ;
    /* multiple frame bits for tx delay */
    handle->wrap_count = lcm(VCOM_TIMER_IRQ_FREQ, VCOM_BAUDRATE) * VCOM_MAC_FRAME_BITS;
    handle->tx_size = 0;
    handle->rx_bit_shift = -1;
    handle->tx_state = VCOM_TX_STATE_IDLE;
    handle->rx_state = VCOM_RX_STATE_IDLE;
    handle->now_tick = 0;
    /* next few samples after init tx signal must keep high */
    handle->next_transfer_tick = handle->sample_count * VCOM_MAC_FRAME_BITS;
    /* on-demand start rx signal check, decrease irq handler time */
    handle->next_capture_tick = handle->wrap_count;

    if (handle->irq_timer_start() != 0) {
        return 11;
    }

    handle->tx_gpio_write(1);
    /* TODO: check if the interrupt handler take too long to block next interrupt */
    handle->inited = 1;

    return 0;
}

/**
 * @brief transmit data by vcom instance, not wait for completion
 *
 * @param handle: vcom handle
 * @param buf: data head pointer
 * @param len: data length
 * @return status code
 *         - 0: success
 *         - 1: not initialized
 *         - 2: invalid data head pointer or length
 *         - 3: task busy
 */
uint8_t vcom_transmit(vcom_handle_t *handle, uint8_t *buf, uint32_t len)
{
    if (handle->inited == 0) {
        return 1;
    }

    if (buf == NULL || len == 0) {
        return 2;
    }

    if (handle->tx_state != VCOM_TX_STATE_IDLE) {
        return 3;
    }

    handle->tx_buf = buf;
    handle->tx_size = len;
    handle->tx_buf_index = 0;
    handle->tx_bit_shift = -1;
    /* tx signal must keep high until next sample */
    handle->next_transfer_tick =
        handle->next_transfer_tick - handle->now_tick < handle->sample_count
            ? handle->next_transfer_tick % handle->wrap_count
            : handle->now_tick;
    handle->tx_state = VCOM_TX_STATE_WAIT_START;
    return 0;
}

/**
 * @brief receive data by vcom instance, not wait for completion
 *
 * @param handle: vcom handle
 * @param buf: data head pointer
 * @param len: data length
 * @return status code
 *         - 0: success
 *         - 1: not initialized
 *         - 2: invalid data head pointer or length
 */
uint8_t vcom_receive(vcom_handle_t *handle, uint8_t *buf, uint32_t len)
{
    if (handle->inited == 0) {
        return 1;
    }

    if (buf == NULL || len == 0) {
        return 2;
    }

    /* reset rx state, to end previous rx */
    handle->rx_state = VCOM_RX_STATE_IDLE;

    handle->rx_buf = buf;
    handle->rx_size = len;
    handle->rx_buf_index = 0;
    handle->rx_bit_shift = -1;
    handle->rx_error_flag = 0;
    handle->next_capture_tick = handle->now_tick % handle->wrap_count;
    handle->rx_state = VCOM_RX_STATE_WAIT_START;

    return 0;
}

/**
 * @brief deinitialize vcom instance
 *
 * @param handle: vcom handle
 * @return status code
 *         - 0: success
 *         - 1: not initialized
 *         - 2: irq timer stop failed
 *         - 3: gpio deinit failed
 */
uint8_t vcom_deinit(vcom_handle_t *handle)
{
    if (handle->inited == 0) {
        return 1;
    }
    if (handle->irq_timer_stop() != 0) {
        return 2;
    }
    if (handle->gpio_deinit() != 0) {
        return 3;
    }

    handle->inited = 0;
    return 0;
}

/**
 * @brief timer interrupt handler
 *
 * @note put this function in timer interrupt handler
 * @param handle: vcom handle
 */
void vcom_timer_handler(vcom_handle_t *handle)
{
    handle->now_tick += handle->step_count;

    if (handle->now_tick - handle->next_transfer_tick <= handle->step_count) {
        vcom_bit_transfer(handle);
    }
    if (handle->now_tick - handle->next_capture_tick <= handle->step_count) {
        vcom_bit_capture(handle);
    }
    if (handle->now_tick >= handle->wrap_count) {
        handle->now_tick = 0;
    }
}