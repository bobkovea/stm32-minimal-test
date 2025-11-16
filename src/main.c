#include <stdint.h>
#include <stdio.h>

#include "stm32f303xc.h"
#include "boardsetuputils.h"
#include "uart.h"
#include "utils.h"
#include "gyro.h"


int main(void) {
	
    ClockInit();
    GpioInit();
	UsartInit();
	SpiInit();
	
	L3GD20_Init();
	
		
    // Основной цикл - мигаем PE15
    while(1) {
		
		char buffer[64];
		
        GPIOE->BSRR |= GPIO_BSRR_BS_15;		
        delay(100000);
        // Выключаем PE15 (устанавливаем низкий уровень)  
        GPIOE->BSRR |= GPIO_BSRR_BR_15;
		
		GyroData_t gyroData;
		L3GD20_ReadGyroData(&gyroData);
		
		//float gyroResult;
		//GyroData_To_DPS(&gyroData, &gyroResult);
		
		snprintf(buffer, sizeof(buffer), "x = %d\ty = %d\tz = %d\r\n", 
         gyroData.x, gyroData.y, gyroData.z);
		 
		UART1_DMA_SendString(buffer);
        delay(1000000);
    }
}