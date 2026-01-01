#ifndef ADC_MY_H
#define ADC_MY_H

#include "stm32f303xc.h"

// Чтение значения ADC
uint16_t AdcRead(void) {
	
// Запуск одиночного преобразования
    ADC1->CR |= ADC_CR_ADSTART;
    
    // Ждем завершения преобразования
    while (!(ADC1->ISR & ADC_ISR_EOC));
    
    // Чтение результата
    uint16_t result = ADC1->DR;
    
    // Сброс флага EOC (опционально, считывание DR сбрасывает флаг)
    ADC1->ISR &= ~ADC_ISR_EOC;
    
    return result;
}

#endif // ADC_MY_H