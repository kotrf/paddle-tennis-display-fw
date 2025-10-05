#include "stm32f4xx_it.h"
#include "main.h"
#include "defs.h"
#include "rgbdisplay.h"
#include "stm32f4xx_ll_dma.h"

static constexpr uint32_t TimerMainChanHighIndex = TIM_GET_CHANNEL_INDEX(TIMER_MAIN_CHAN_HIGH);
static constexpr uint32_t TimerMainChanLowIndex = TIM_GET_CHANNEL_INDEX(TIMER_MAIN_CHAN_LOW);

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
	while (1) {
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief  This function handles TIM2 interrupt.
 * @param  None
 * @retval None
 */
void TIMER_MAIN_IRQHandler(void) {
	LL_TIM_DisableCounter(TIMER_MAIN);	// Disable Counter

	// LL_TIM_OC_SetMode of channel "HIGH" to LL_TIM_OCMODE_FORCED_INACTIVE
	__IO uint32_t *pRegHigh = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMER_MAIN->CCMR1) + OFFSET_TAB_CCMRx[TimerMainChanHighIndex]));
	MODIFY_REG(*pRegHigh, ((TIM_CCMR1_OC1M  | TIM_CCMR1_CC1S) << SHIFT_TAB_OCxx[TimerMainChanHighIndex]), LL_TIM_OCMODE_FORCED_INACTIVE << SHIFT_TAB_OCxx[TimerMainChanHighIndex]);

	// LL_TIM_OC_SetMode of channel "LOW" to LL_TIM_OCMODE_FORCED_INACTIVE
	__IO uint32_t *pRegLow = (__IO uint32_t *)((uint32_t)((uint32_t)(&TIMER_MAIN->CCMR1) + OFFSET_TAB_CCMRx[TimerMainChanLowIndex]));
	MODIFY_REG(*pRegLow, ((TIM_CCMR1_OC1M  | TIM_CCMR1_CC1S) << SHIFT_TAB_OCxx[TimerMainChanLowIndex]), LL_TIM_OCMODE_FORCED_INACTIVE << SHIFT_TAB_OCxx[TimerMainChanLowIndex]);

	TIMER_MAIN->SR = ~(TIM_SR_CC1IF|TIM_SR_CC2IF);			// Clear flags CC1 and CC2 by writing 0
	TIMER_MAIN->DIER &= ~(TIM_DIER_CC1IE|TIM_DIER_CC2IE);	// Disable IT for CC1 and CC2

	disp.onTxFinish();
}

/**
 * @brief  This function handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Stream4_IRQHandler(void) {
	if (LL_DMA_IsActiveFlag_TC4(DMA1) == 1) {
//		LL_DMA_ClearFlag_TC4(DMA1);
		DMA1->HIFCR = DMA_HIFCR_CTCIF4; // Clear Flag TC4 for DMA1
		LL_TIM_EnableIT_CC1(TIMER_MAIN);
	}
	else if (LL_DMA_IsActiveFlag_TE4(DMA1) == 1) {
		disp.onTxError();
	}
}

/**
 * @brief  This function handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Stream5_IRQHandler(void) {
	if (LL_DMA_IsActiveFlag_TC5(DMA1) == 1) {
		DMA1->HIFCR = DMA_HIFCR_CTCIF5; // Clear Flag TC5 for DMA1
		LL_TIM_EnableIT_CC2(TIMER_MAIN);
	}
	else if (LL_DMA_IsActiveFlag_TE5(DMA1) == 1) {
		disp.onTxError();
	}
}
