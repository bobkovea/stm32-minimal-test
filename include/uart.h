#ifndef UART_MY_H
#define UART_MY_H

#include <stdbool.h>
#include "stm32f303xc.h"

bool UART1_TransferIsComplete(void)
{
	return (USART1->ISR & USART_ISR_TC);
}

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
	while(!UART1_TransferIsComplete());
}

#endif // UART_MY_H