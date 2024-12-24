/**
 * @file    vcom.h
 * @brief   Simulate USART use GPIO
 * @version 1.0.0
 * @author  Simon Law
 * @date    2023-09-04
 */


#ifndef __VCOM_H
#define __VCOM_H

#include <stdint.h>
#include "esp_attr.h"

/**
 * @brief available frame format
 */
#define VCOM_FRAME_8N1 0   // 8 bits data, none parity, 1 bit stop
#define VCOM_FRAME_8O1 1   // 8 bits data, odd parity, 1 bit stop
#define VCOM_FRAME_8E1 2   // 8 bits data, even parity, 1 bit stop

/**
 * @brief available frame parity type
 */
#define VCOM_PAYLOAD_CHECKSUM_TYPE_ODD 0
#define VCOM_PAYLOAD_CHECKSUM_TYPE_EVEN 1

/**
 * @brief tx state
 */
#define VCOM_TX_STATE_IDLE 0
#define VCOM_TX_STATE_WAIT_START 1
#define VCOM_TX_STATE_TRIGGERED 2

/**
 * @brief rx state
 */
#define VCOM_RX_STATE_IDLE 0
#define VCOM_RX_STATE_WAIT_START 1
#define VCOM_RX_STATE_TRIGGERED 2

/**
 * @brief rx frame error type
 */
#define VCOM_FRAME_ERROR_NONE 0
#define VCOM_FRAME_ERROR_CHECKSUM 1
#define VCOM_FRAME_ERROR_START 2
#define VCOM_FRAME_ERROR_STOP 3
#define VCOM_FRAME_ERROR_OVERFLOW 4

/**
 * @brief vcom handle structure definition
 *
 */
typedef struct vcom_handle_s
{
    uint8_t (*irq_timer_init)(void);
    uint8_t (*irq_timer_start)(void);
    uint8_t (*irq_timer_stop)(void);
    uint8_t (*gpio_init)(void);
    uint8_t (*gpio_deinit)(void);
    uint8_t (*tx_gpio_write)(uint8_t level);
    uint8_t (*rx_gpio_read)(void);
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint32_t step_count;
    uint32_t sample_count;
    uint32_t wrap_count;
    uint32_t now_tick;
    uint32_t next_transfer_tick;
    uint32_t next_capture_tick;
    uint32_t tx_size;
    uint32_t rx_size;
    uint32_t tx_buf_index;
    uint32_t rx_buf_index;
    uint8_t tx_state;
    uint8_t rx_state;
    /* TODO: add rx timeout check */
    int8_t tx_bit_shift;
    int8_t rx_bit_shift;
    uint8_t tx_bit_pcnt;
    uint8_t rx_bit_pcnt;
    uint8_t rx_error_flag;
    /* TODO: change to use ringbuffer for async operation */
    uint8_t inited;
} vcom_handle_t;

uint8_t vcom_init(vcom_handle_t * handle);

uint8_t vcom_transmit(vcom_handle_t *handle, uint8_t *buf, uint32_t len);

uint8_t vcom_receive(vcom_handle_t *handle, uint8_t *buf, uint32_t len);

uint8_t vcom_deinit(vcom_handle_t * handle);

void IRAM_ATTR vcom_timer_handler(vcom_handle_t *handle);

#endif /* __VCOM_H */
