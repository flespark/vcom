#include "vcom_interface.h"
#include "tim.h"
#include "main.h"

uint8_t vcom_interface_timer_init(void)
{
    MX_TIM4_Init();
    return 0;
}

uint8_t vcom_interface_timer_start(void)
{
    HAL_TIM_Base_Start_IT(&htim4);
    return 0;
}

uint8_t vcom_interface_timer_stop(void)
{
    HAL_TIM_Base_Stop_IT(&htim4);
    return 0;
}

uint8_t vcom_interface_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(VCOM_TX_GPIO_Port, VCOM_TX_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin : VCOM_TX_Pin */
    GPIO_InitStruct.Pin   = VCOM_TX_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(VCOM_TX_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : VCOM_RX_Pin */
    GPIO_InitStruct.Pin  = VCOM_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(VCOM_RX_GPIO_Port, &GPIO_InitStruct);

    return 0;
}

uint8_t vcom_interface_gpio_deinit(void)
{
    HAL_GPIO_DeInit(VCOM_TX_GPIO_Port, VCOM_TX_Pin);
    HAL_GPIO_DeInit(VCOM_RX_GPIO_Port, VCOM_RX_Pin);
    return 0;
}

uint8_t vcom_interface_tx_gpio_write(uint8_t level)
{
    if (level == 0) {
        HAL_GPIO_WritePin(VCOM_TX_GPIO_Port, VCOM_TX_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(VCOM_TX_GPIO_Port, VCOM_TX_Pin, GPIO_PIN_SET);
    }
    return 0;
}

uint8_t vcom_interface_rx_gpio_read(void)
{
    return HAL_GPIO_ReadPin(VCOM_RX_GPIO_Port, VCOM_RX_Pin);
}
