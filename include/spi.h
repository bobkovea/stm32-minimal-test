#ifndef SPI_MY_H
#define SPI_MY_H

#include "stm32f303xc.h"

// Управление Chip Select на PE3
void SPI1_CS_Low(void) {
    GPIOE->BSRR = GPIO_BSRR_BR_3;  // PE3 low
}

void SPI1_CS_High(void) {
    GPIOE->BSRR = GPIO_BSRR_BS_3;  // PE3 high
}

// Передача/прием байта по SPI
uint8_t SPI_Transfer(uint8_t data) {
    // Ждем готовности TX буфера
    while (!(SPI1->SR & SPI_SR_TXE));
    // Записываем данные в DR
    *((volatile uint8_t*)&SPI1->DR) = data;
    // Ждем приема данных
    while (!(SPI1->SR & SPI_SR_RXNE));
    // Читаем принятые данные
    return *((volatile uint8_t*)&SPI1->DR);
}



#endif // SPI_MY_H