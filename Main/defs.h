#ifndef __DEFS_H_
#define __DEFS_H_

#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_tim.h>

#define LED_INT_Pin 		LL_GPIO_PIN_13
#define LED_INT_Port 		GPIOC
#define KEY_MAIN_Pin		LL_GPIO_PIN_0
#define KEY_MAIN_Port		GPIOA
#define RGB_DO_High_Pin		LL_GPIO_PIN_7
#define RGB_DO_Low_Pin		LL_GPIO_PIN_6
#define RGB_DO_Port			GPIOA

#define TIMER_PWM				TIM3
#define TIMER_PWM_CHAN_HIGH		LL_TIM_CHANNEL_CH2
#define TIMER_PWM_CHAN_LOW		LL_TIM_CHANNEL_CH1
#define TIMER_PWM_IRQHandler	TIM3_IRQHandler
#define TIMER_PWM_IRQn			TIM3_IRQn
#define TIMER_PWM_PERIOD		125
#define TIMER_PWM_WIDTH_1		90
#define TIMER_PWM_WIDTH_0		35

#define TIMER_MAIN				TIM4
#define TIMER_MAIN_IRQHandler	TIM4_IRQHandler
#define TIMER_MAIN_IRQn			TIM4_IRQn
#define TIMER_MAIN_PERIOD		5000

#endif // __DEFS_H_
