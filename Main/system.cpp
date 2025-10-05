#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_utils.h"
#include "system.h"
#include "defs.h"

System::System() {
	// MCU Configuration

	// Reset of all peripherals, Initializes the Flash interface and the Systick
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	// System interrupt init
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	// SysTick_IRQn interrupt configuration
	NVIC_SetPriority(SysTick_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void System::initClock() {
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3) {
	}

	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while (LL_RCC_HSE_IsReady() != 1) {
	}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 200,
	LL_RCC_PLLP_DIV_2);
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1) {
	}

	while (LL_PWR_IsActiveFlag_VOS() == 0) {
	}

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
	}

	LL_Init1msTick(SystecmClockHz);
	LL_SetSystemCoreClock(SystecmClockHz);
	LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);

	// GPIO Ports Clock Enable
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);

	// Peripheral clock enable
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
}

/**
 * @brief  This function  configures the DMA transfer,
 *         configures the NVIC for DMA and enables the DMA.
 * @param  None
 * @retval None
 */
void System::initDma(uint32_t *timerCcData, uint32_t timerCcDataLen) {
	NVIC_SetPriority(DMA1_Stream4_IRQn, 0);
	NVIC_SetPriority(DMA1_Stream5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	LL_DMA_ConfigTransfer(DMA1, LL_DMA_STREAM_4,
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
			LL_DMA_PRIORITY_HIGH |
			LL_DMA_MODE_NORMAL |
			LL_DMA_PERIPH_NOINCREMENT |
			LL_DMA_MEMORY_INCREMENT |
			LL_DMA_PDATAALIGN_WORD |
			LL_DMA_MDATAALIGN_WORD);
	LL_DMA_ConfigTransfer(DMA1, LL_DMA_STREAM_5,
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
			LL_DMA_PRIORITY_HIGH |
			LL_DMA_MODE_NORMAL |
			LL_DMA_PERIPH_NOINCREMENT |
			LL_DMA_MEMORY_INCREMENT |
			LL_DMA_PDATAALIGN_WORD |
			LL_DMA_MDATAALIGN_WORD);

	LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_5);
	LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_5, LL_DMA_CHANNEL_5);

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_4, (uint32_t)timerCcData,
			(uint32_t) &TIMER_MAIN->CCR1,
			LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_4));
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_5, (uint32_t)timerCcData,
			(uint32_t) &TIMER_MAIN->CCR2,
			LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_5));

	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, timerCcDataLen);
	LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_5, timerCcDataLen);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_4);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_5);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_STREAM_4);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_STREAM_5);

	/***************************/
	/* Enable the DMA transfer */
	/***************************/
//	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
void System::initTimerMain() {
	LL_TIM_InitTypeDef TIM_InitStruct = { 0 };
	LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = { 0 };

	TIM_InitStruct.Prescaler = 0;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = TIMER_MAIN_PERIOD-1;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_InitStruct.RepetitionCounter = 0;
	LL_TIM_Init(TIMER_MAIN, &TIM_InitStruct);

	LL_TIM_SetCounter(TIMER_MAIN, TIMER_MAIN_PERIOD-1);
	LL_TIM_DisableARRPreload(TIMER_MAIN);
	LL_TIM_SetClockSource(TIMER_MAIN, LL_TIM_CLOCKSOURCE_INTERNAL);

	TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
	TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
	TIM_OC_InitStruct.CompareValue = TIMER_MAIN_WIDTH_0;
	TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	LL_TIM_OC_Init(TIMER_MAIN, TIMER_MAIN_CHAN_HIGH, &TIM_OC_InitStruct);
	LL_TIM_OC_Init(TIMER_MAIN, TIMER_MAIN_CHAN_LOW, &TIM_OC_InitStruct);

	LL_TIM_OC_EnablePreload(TIMER_MAIN, TIMER_MAIN_CHAN_HIGH);
	LL_TIM_OC_EnablePreload(TIMER_MAIN, TIMER_MAIN_CHAN_LOW);
	LL_TIM_OC_DisableFast(TIMER_MAIN, TIMER_MAIN_CHAN_HIGH);
	LL_TIM_OC_DisableFast(TIMER_MAIN, TIMER_MAIN_CHAN_LOW);

	LL_TIM_SetTriggerOutput(TIMER_MAIN, LL_TIM_TRGO_RESET);	// ???
	LL_TIM_DisableMasterSlaveMode(TIMER_MAIN);

	LL_TIM_DisableIT_UPDATE(TIMER_MAIN);
	LL_TIM_DisableIT_CC1(TIMER_MAIN);

	// TIM3 interrupt Init
	NVIC_SetPriority(TIMER_MAIN_IRQn,
			NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(TIMER_MAIN_IRQn);

	// Force update generation
//	LL_TIM_GenerateEvent_CC1(TIMER_MAIN);

//	LL_TIM_EnableDMAReq_CC1(TIMER_MAIN);
}

void System::runTimerMain(bool en) {
	if (en) {
		LL_TIM_SetCounter(TIMER_MAIN, 0);
		LL_TIM_EnableCounter(TIMER_MAIN);
	}
	else {
		LL_TIM_DisableCounter(TIMER_MAIN);
		LL_TIM_ClearFlag_CC1(TIMER_MAIN);
	}
}

void System::updateTimerMainCompare(uint32_t compareValue) {
	LL_TIM_OC_SetCompareCH1(TIMER_MAIN, compareValue);
}

void System::updateTimerMainCompareNoPreload(uint32_t compareValue) {
	LL_TIM_OC_DisablePreload(TIMER_MAIN, TIMER_MAIN_CHAN_LOW);
	LL_TIM_OC_SetCompareCH1(TIMER_MAIN, compareValue);
	LL_TIM_OC_EnablePreload(TIMER_MAIN, TIMER_MAIN_CHAN_LOW);
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
void System::initUsart1() {
	LL_USART_InitTypeDef USART_InitStruct = { 0 };

	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(USART1);
	LL_USART_Enable(USART1);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void System::initGpio() {
	LL_EXTI_InitTypeDef EXTI_InitStruct = { 0 };
	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// LED "Red"
	LL_GPIO_ResetOutputPin(LED_RED_Port, LED_RED_Pin);

	GPIO_InitStruct.Pin = LED_RED_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(LED_RED_Port, &GPIO_InitStruct);

	// Debug signal
	LL_GPIO_ResetOutputPin(DBG_Port, DBG_Pin);

	GPIO_InitStruct.Pin = DBG_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(DBG_Port, &GPIO_InitStruct);

	// TIMER_MAIN Output GPIO Config
	LL_GPIO_ResetOutputPin(RGB_DO_Port, RGB_DO_High_Pin);
	LL_GPIO_ResetOutputPin(RGB_DO_Port, RGB_DO_Low_Pin);

	GPIO_InitStruct.Pin = RGB_DO_High_Pin | RGB_DO_Low_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
	LL_GPIO_Init(RGB_DO_Port, &GPIO_InitStruct);

	/**USART1 GPIO Configuration
	 PA9   ------> USART1_TX
	 PA10   ------> USART1_RX
	 */
/*
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
*/

	// KEY GPIO Config
	LL_GPIO_SetPinPull(KEY_MAIN_Port, KEY_MAIN_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(KEY_MAIN_Port, KEY_MAIN_Pin, LL_GPIO_MODE_INPUT);

	/**/
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);

	/**/
	EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_0;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
	LL_EXTI_Init(&EXTI_InitStruct);
}

void System::delay(uint32_t delayMs) {
	LL_mDelay(delayMs);
}
