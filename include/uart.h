#ifndef UART_MY_H
#define UART_MY_H

#include "stm32f303xc.h"

// Отправка данных через DMA
void UART1_DMA_Send(uint8_t* data, uint16_t length)
{
	while (DMA1_Channel4->CNDTR != 0 && (DMA1_Channel4->CCR & DMA_CCR_EN));
	
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CMAR = (uint32_t)data;
    DMA1_Channel4->CNDTR = length;  // Количество данных
    DMA1_Channel4->CCR |= DMA_CCR_EN;  // Запуск DMA
}

// Отправка строки через DMA
void UART1_DMA_SendString(char* str) {
    uint16_t len = 0;
    while (str[len] != '\0') len++;
    UART1_DMA_Send((uint8_t*)str, len);
}

uint32_t UART1_TransferIsComplete(void)
{
	return (DMA1_Channel4->CNDTR == 0U);
}

#endif // UART_MY_H