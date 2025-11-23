#include <stdint.h>
#include <stdio.h>

#include "stm32f303xc.h"
#include "boardsetuputils.h"
#include "uart.h"
#include "utils.h"
#include "gyro.h"
#include "magnet.h"
#include "wakeup.h"


char buffer[64];

int main(void){
	
    ClockInit();
	InterruptsInit();
	WakeupInit();
    GpioInit();
	UsartInit();
	SpiInit();
	I2cInit();

	L3GD20_Init();

	
	//LSM303AGR_Init();
	LSM303AGR_InitTemperature(); // Включить датчик температуры
	
	uint8_t flag = 0;

	while(1)
	{
		GPIOE->BSRR |= GPIO_BSRR_BS_15;		
		
		GyroData_t gyroData;
		L3GD20_ReadGyroData(&gyroData);
		
		//float gyroResult;
		//GyroData_To_DPS(&gyroData, &gyroResult);
		
		snprintf(buffer, sizeof(buffer), "x = %d\ty = %d\tz = %d\r\n", 
		gyroData.x, gyroData.y, gyroData.z);
		
		UART1_DMA_SendString(buffer);
		
		
		delay(100000);
		// Выключаем PE15 (устанавливаем низкий уровень)  
		GPIOE->BSRR |= GPIO_BSRR_BR_15;
	
		
		while(!LSM303AGR_TemperatureReady());
		
		//const int8_t temp = LSM303AGR_ReadRawTemperature();
		const float temp_c = LSM303AGR_ReadTemperatureCelsius();
		const uint8_t temp_c_int = (uint8_t)temp_c;
		const uint8_t temp_c_man = (uint8_t)((uint16_t)(temp_c * 100.0) % 100);
		
		snprintf(buffer, sizeof(buffer), "Temp: %d.%d\r\n", temp_c_int, temp_c_man);
		UART1_DMA_SendString(buffer);
		
		delay(100000);
		
		if(!flag)
		{
			flag = 1;	
			Wakeup_EnterStopMode();
		}
		
		
	}
}