#ifndef WAKEUP_H
#define WAKEUP_H

#include "boardsetuputils.h"

void Wakeup_EnterStopMode(void)
{
    // Очищаем флаг Wakeup
    PWR->CR |= PWR_CR_CWUF;
    
    // Устанавливаем режим Stop
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	
    PWR->CR &= ~PWR_CR_PDDS;
    PWR->CR |= PWR_CR_LPDS; // Low-power Stop
    
    // Для отладки - разрешаем Debug в Stop режиме
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;
    
	SetPeripheralClock(0);
    
    // Входим в Stop режим
    __WFI();
	
	SetPeripheralClock(1);
}

#endif // WAKEUP_H