#include "stm32f303xc.h"

void EXTI0_IRQHandler(void)
{
	EXTI->PR |= EXTI_PR_PR0;
}


void RTC_WKUP_IRQHandler(void) {
	
	RTC->ISR &= ~RTC_ISR_WUTF;
	
    EXTI->PR |= EXTI_PR_PR20;
	
    RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	
	RTC->CR &= ~RTC_CR_WUTE;
	while (!(RTC->ISR & RTC_ISR_WUTWF));
	
	RTC->WPR = 0xFF;
}