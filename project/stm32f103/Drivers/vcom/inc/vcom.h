#ifndef __VCOM_H
#define __VCOM_H

#include <stdint.h>
#include "vcom_conf.h"

/**
 * @brief available frame format
 */
#define VCOM_FRAME_8N1 0 /* 8 bits data, none parity, 1 bit stop */
#define VCOM_FRAME_8O1 1 /* 8 bits data, odd parity, 1 bit stop */
#define VCOM_FRAME_8E1 2 /* 8 bits data, even parity, 1 bit stop */

/**
 * @brief available frame parity type
 */
#define VCOM_PAYLOAD_CHECKSUM_TYPE_ODD  0
#define VCOM_PAYLOAD_CHECKSUM_TYPE_EVEN 1

/**
 * @brief tx state
 */
#define VCOM_TX_STATE_IDLE       0
#define VCOM_TX_STATE_WAIT_START 1
#define VCOM_TX_STATE_TRIGGERED  2

/**
 * @brief rx state
 */
#define VCOM_RX_STATE_IDLE       0
#define VCOM_RX_STATE_WAIT_START 1
#define VCOM_RX_STATE_TRIGGERED  2

/**
 * @brief rx frame error type
 */
#define VCOM_FRAME_ERROR_NONE     0
#define VCOM_FRAME_ERROR_CHECKSUM 1
#define VCOM_FRAME_ERROR_START    2
#define VCOM_FRAME_ERROR_STOP     3
#define VCOM_FRAME_ERROR_OVERFLOW 4

/**
 * @brief vcom handle structure definition
 *
 */
typedef struct vcom_handle_s
{
    uint8_t (*irq_timer_init)(void);         /* periodic interrupt timer init interface */
    uint8_t (*irq_timer_start)(void);        /* periodic interrupt timer start interface */
    uint8_t (*irq_timer_stop)(void);         /* periodic interrupt timer stop interface */
    uint8_t (*gpio_init)(void);              /* tx and rx gpio init interface */
    uint8_t (*gpio_deinit)(void);            /* tx and rx gpio deinit interface */
    uint8_t (*tx_gpio_write)(uint8_t level); /* tx gpio write interface */
    uint8_t (*rx_gpio_read)(void);           /* rx gpio read interface */
    uint8_t *tx_buf;                         /* tx data buffer */
    uint8_t *rx_buf;                         /* rx data buffer */
    uint32_t step_count;                     /* tick number of interrupt timer period */
    uint32_t sample_count;                   /* tick number of tx/rx bit delay */
    uint32_t wrap_count;                     /* tick number of wrap around */
    uint32_t now_tick;                       /* now tick */
    uint32_t next_transfer_tick;             /* next tx gpio transfer tick */
    uint32_t next_capture_tick;              /* next rx gpio capture tick */
    uint32_t tx_size;                        /* tx data size */
    uint32_t rx_size;                        /* rx data size */
    uint32_t tx_buf_index;                   /* tx data index */
    uint32_t rx_buf_index;                   /* rx data index */
    uint8_t  tx_state;                       /* tx state */
    uint8_t  rx_state;                       /* rx state */
    int8_t   tx_bit_shift;                   /* tx bit shift */
    int8_t   rx_bit_shift;                   /* rx bit shift */
    uint8_t  tx_bit_pcnt;                    /* tx bit checksum */
    uint8_t  rx_bit_pcnt;                    /* rx bit checksum */
    uint8_t  rx_error_flag;                  /* rx error flag */
    uint8_t  inited;                         /* init flag */
} vcom_handle_t;

/**
 * @brief init vcom instance
 *
 * @param handle: vcom handle
 * @return 0: success
 */
uint8_t vcom_init(vcom_handle_t * handle);

/**
 * @brief transmit data by vcom instance, not wait for completion
 *
 * @param handle: vcom handle
 * @param buf: data head pointer
 * @param len: data length
 * @return 0: success, 1: not initialized, 2: invalid data head pointer or length, 3: task busy
 */
uint8_t vcom_transmit(vcom_handle_t *handle, uint8_t *buf, uint32_t len);

/**
 * @brief receive data by vcom instance, not wait for completion
 *
 * @param handle: vcom handle
 * @param buf: data head pointer
 * @param len: data length
 * @return 0: success, 1: not initialized, 2: invalid data head pointer or length, 3: task busy
 */
uint8_t vcom_receive(vcom_handle_t *handle, uint8_t *buf, uint32_t len);

/**
 * @brief deinit vcom instance
 *
 * @param handle: vcom handle
 * @return 0: success, 1: not initialized, 2: interrupt timer stop failed, 3: gpio deinit failed
 */
uint8_t vcom_deinit(vcom_handle_t * handle);

/**
 * @brief vcom periodic interrupt timer handler
 *
 * @param handle: vcom handle
 */
void vcom_timer_handler(vcom_handle_t *handle);

#endif
