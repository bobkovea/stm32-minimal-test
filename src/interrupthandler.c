#include "stm32f303xc.h"

void EXTI0_IRQHandler(void)
{
	EXTI->PR |= EXTI_PR_PR0;
}
