#include <stdint.h>
#include <stdio.h>

#include "stm32f303xc.h"
#include "boardsetuputils.h"
#include "uart.h"
#include "utils.h"
#include "gyro.h"
#include "magnet.h"
#include "wakeup.h"
#include "adc.h"

char buffer[64];

int main(void){
	
    ClockInit();
	InterruptsInit();
	RtcInit();
	WakeupInit();
    GpioInit();
	AdcInit();
	UsartInit();
	SpiInit();
	I2cInit();
	//TimersInit();
	L3GD20_Init();
	
	//LSM303AGR_Init();
	LSM303AGR_InitTemperature(); // Включить датчик температуры
	
	while(1)
	{
		uint16_t adcCode;
		
		GPIOA->BSRR |= GPIO_BSRR_BS_3;	
		
		delay_ms(100);
		
		adcCode = AdcRead();      
		snprintf(buffer, sizeof(buffer), "@ 1 = %d\n", adcCode);

		UART1_DMA_SendString(buffer);
			
		//
		
		GPIOA->BSRR |= GPIO_BSRR_BR_3;	
		
		delay_ms(100);
		
		adcCode = AdcRead();      
		snprintf(buffer, sizeof(buffer), "@ 0 = %d\n", adcCode);

		UART1_DMA_SendString(buffer);
		
		//Wakeup_StartRtcWakeupTimer();
		//Wakeup_EnterStopMode();
		
	}
}
