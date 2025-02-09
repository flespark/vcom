#include "vcom_interface.h"
#include "tim.h"
#include "main.h"

uint8_t vcom_interface_timer_init(uint32_t hz)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    uint32_t apb1_freq = HAL_RCC_GetPCLK1Freq();
    if (apb1_freq < hz || apb1_freq % hz != 0) {
        return 1;
    }
    uint32_t max_count = apb1_freq / hz;
    if (max_count > 0xFFFF) {
        return 2;
    }

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = max_count;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
        return 3;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
        return 4;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
        return 5;
    }
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
