#ifndef I2C_MY_H
#define I2C_MY_H

#include "stm32f303xc.h"


// I2C функции (остаются такими же)
uint8_t I2C_CheckDevice(uint8_t addr) {
    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    
    volatile uint32_t timeout = 100000;
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if(timeout == 0) return 0;
    
    I2C1->CR2 = (addr << 1) | (0 << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND | I2C_CR2_START;
    
    timeout = 100000;
    while(!(I2C1->ISR & (I2C_ISR_STOPF | I2C_ISR_NACKF)) && timeout--);
    
    uint8_t result = (I2C1->ISR & I2C_ISR_STOPF) ? 1 : 0;
    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    
    return result;
}

void I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    
    volatile uint32_t timeout = 100000;
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if(timeout == 0) return;
    
    // ПРАВИЛЬНО: 2 байта данных - адрес регистра и данные
    I2C1->CR2 = (dev_addr << 1) |           // Адрес устройства
                (2 << I2C_CR2_NBYTES_Pos) | // 2 байта: адрес регистра + данные
                I2C_CR2_AUTOEND |           // Автоматический STOP
                I2C_CR2_START;              // Начать передачу
    
    // Сначала отправляем адрес регистра
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_TXIS) && timeout--);
    if(timeout == 0) return;
    I2C1->TXDR = reg_addr;
    
    // Затем отправляем данные
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_TXIS) && timeout--);
    if(timeout == 0) return;
    I2C1->TXDR = data;
    
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_STOPF) && timeout--);
    I2C1->ICR = I2C_ICR_STOPCF;
}



uint8_t I2C_Read(uint8_t dev_addr, uint8_t reg_addr) {
    uint8_t data;
    
    // Фаза 1: Записываем адрес регистра
    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    
    volatile uint32_t timeout = 100000;
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if(timeout == 0) return 0xFF;
    
    I2C1->CR2 = (dev_addr << 1) |           // Адрес устройства
                (1 << I2C_CR2_NBYTES_Pos) | // 1 байт: адрес регистра
                I2C_CR2_AUTOEND |           // Автоматический STOP
                I2C_CR2_START;              // Начать передачу
    
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_TXIS) && timeout--);
    if(timeout == 0) return 0xFF;
    I2C1->TXDR = reg_addr;
    
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_STOPF) && timeout--);
    I2C1->ICR = I2C_ICR_STOPCF;
    
    // Фаза 2: Читаем данные
    I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
    
    timeout = 100000;
    while((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if(timeout == 0) return 0xFF;
    
    I2C1->CR2 = (dev_addr << 1) |           // Адрес устройства
                (1 << I2C_CR2_NBYTES_Pos) | // 1 байт данных
                I2C_CR2_AUTOEND |           // Автоматический STOP
                I2C_CR2_START |             // Начать передачу
                I2C_CR2_RD_WRN;             // Режим чтения
    
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_RXNE) && timeout--);
    if(timeout == 0) return 0xFF;
    
    data = I2C1->RXDR;
    
    timeout = 100000;
    while(!(I2C1->ISR & I2C_ISR_STOPF) && timeout--);
    I2C1->ICR = I2C_ICR_STOPCF;
    
    return data;
}


void I2C_ReadMultiple(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t count)
{
    uint32_t timeout;

    // Ожидаем освобождение шины
    timeout = 100000;
    while ((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
    if (timeout == 0) return;

    // -----------------------------
    // ФАЗА 1: запись адреса регистра
    // -----------------------------
    I2C1->CR2 = 0;                                  // сброс конфигурации CR2
    I2C1->CR2 = (dev_addr << 1) |                  // адрес устройства
                 (1 << I2C_CR2_NBYTES_Pos) |       // 1 байт для записи
                 0;                                // RD_WRN = 0 (write)

    I2C1->CR2 |= I2C_CR2_START;                    // START

    // ждем TXIS
    timeout = 100000;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && timeout--);
    if (timeout == 0) return;

    I2C1->TXDR = reg_addr;                         // отправляем адрес регистра

    // ждем окончания передачи (TC)
    timeout = 100000;
    while (!(I2C1->ISR & I2C_ISR_TC) && timeout--);
    if (timeout == 0) {
        I2C1->CR2 |= I2C_CR2_STOP;
        return;
    }

    // -----------------------------
    // ФАЗА 2: чтение байтов
    // -----------------------------
    I2C1->CR2 = 0;                                  // сброс CR2 перед новой фазой!
    I2C1->CR2 = (dev_addr << 1) | 
                (count << I2C_CR2_NBYTES_Pos) |
                I2C_CR2_RD_WRN |                  // чтение
                I2C_CR2_AUTOEND;                  // STOP после завершения

    I2C1->CR2 |= I2C_CR2_START;                    // repeated START

    // читаем count байтов
    for(uint8_t i = 0; i < count; i++) {
        timeout = 100000;
        while(!(I2C1->ISR & I2C_ISR_RXNE) && timeout--);
        if(timeout == 0) {
            I2C1->CR2 |= I2C_CR2_STOP;
            return;
        }
        data[i] = I2C1->RXDR;
    }

    // Ждем STOPF
    timeout = 100000;
    while (!(I2C1->ISR & I2C_ISR_STOPF) && timeout--);
    I2C1->ICR = I2C_ICR_STOPCF;
}

























uint8_t I2C_CheckAddresses(void) {
	
    for(uint8_t addr = 1; addr < 127; addr++) {
        // Сбрасываем все флаги
        I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF | I2C_ICR_BERRCF;
        
        // Ждем когда I2C не занят
        volatile uint32_t timeout = 100000;
        while((I2C1->ISR & I2C_ISR_BUSY) && timeout--);
        if(timeout == 0) continue;
        
        // Настраиваем передачу для проверки адреса
        I2C1->CR2 = (addr << 1) |           // Адрес устройства (бит 0 = 0 - запись)
                    (0 << I2C_CR2_NBYTES_Pos) |    // 0 байт данных
                    I2C_CR2_AUTOEND |              // Автоматический STOP
                    I2C_CR2_START;                 // Начать передачу
        
        // Ждем завершения (STOPF) или ошибки (NACKF)
        timeout = 100000;
        while(!(I2C1->ISR & (I2C_ISR_STOPF | I2C_ISR_NACKF)) && timeout--);
        
        // Проверяем результат
        if(I2C1->ISR & I2C_ISR_STOPF) {
			I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
			return addr;

        }
        
        // Сбрасываем флаги
        I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF;
        
        // Небольшая задержка между проверками
        for(volatile int i = 0; i < 1000; i++);
    }
	
	return 0;
}


#endif // I2C_MY_H