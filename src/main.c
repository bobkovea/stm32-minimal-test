#include "stm32f303xc.h"

// Простая задержка (примерная)
void delay(uint32_t iterations) {
    for(uint32_t i = 0; i < iterations; i++) {
        __asm__("nop");
    }
}

// Инициализация GPIO для PE15 (источник: RM0316, разделы 8.4.1, 8.4.7)
void GPIO_Init(void) {
    // 1. Включаем тактирование порта E
    // RCC->AHBENR bit 21: IOPEEN - IO port E clock enable
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    
    // 2. Настраиваем PE15 как выход (MODER15 = 01)
    // Сбрасываем биты MODER15
    GPIOE->MODER &= ~GPIO_MODER_MODER15_Msk;
    // Устанавливаем MODER15 = 01 (Output mode)
    GPIOE->MODER |= GPIO_MODER_MODER15_0;
    
    // 3. Дополнительные настройки (опционально):
    // - Тип выхода: push-pull (OTYPER15 = 0) - по умолчанию
    // - Скорость: low (OSPEEDR15 = 00) - по умолчанию  
    // - Pull-up/pull-down: none (PUPDR15 = 00) - по умолчанию
}

int main(void) {
       
    GPIO_Init();
    
    // Основной цикл - мигаем PE15
    while(1) {
        // Включаем PE15 (устанавливаем высокий уровень)
        // Используем BSRR для атомарной установки бита
        GPIOE->BSRR = GPIO_BSRR_BS_15;
        delay(500000);
        
        // Выключаем PE15 (устанавливаем низкий уровень)  
        GPIOE->BSRR = GPIO_BSRR_BR_15;
        delay(500000);
    }
}