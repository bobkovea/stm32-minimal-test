#ifndef GYRO_MY_H
#define GYRO_MY_H

#include "spi.h"

// Регистры L3GD20
#define L3GD20_WHO_AM_I      0x0F
#define L3GD20_CTRL_REG1     0x20
#define L3GD20_CTRL_REG2     0x21
#define L3GD20_CTRL_REG3     0x22
#define L3GD20_CTRL_REG4     0x23
#define L3GD20_CTRL_REG5     0x24
#define L3GD20_REFERENCE     0x25
#define L3GD20_OUT_TEMP      0x26
#define L3GD20_STATUS_REG    0x27
#define L3GD20_OUT_X_L       0x28
#define L3GD20_OUT_X_H       0x29
#define L3GD20_OUT_Y_L       0x2A
#define L3GD20_OUT_Y_H       0x2B
#define L3GD20_OUT_Z_L       0x2C
#define L3GD20_OUT_Z_H       0x2D

// Бит для чтения
#define L3GD20_READ_BIT      0x80
#define L3GD20_MULTI_BIT     0x40

// WHO_AM_I значение для L3GD20
#define L3GD20_ID            0xD4

// Структура для данных гироскопа
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} GyroData_t;

// Чтение регистра датчика
uint8_t L3GD20_ReadRegister(uint8_t reg) {
    uint8_t value;
    
    SPI1_CS_Low();
    
    // Отправляем адрес регистра с флагом чтения
    SPI_Transfer(reg | L3GD20_READ_BIT);
    // Читаем значение
    value = SPI_Transfer(0x00);
    
    SPI1_CS_High();
    
    return value;
}

// Запись в регистр датчика
void L3GD20_WriteRegister(uint8_t reg, uint8_t value) {
    SPI1_CS_Low();
    
    // Отправляем адрес регистра
    SPI_Transfer(reg);
    // Отправляем значение
    SPI_Transfer(value);
    
    SPI1_CS_High();
}

// Пакетное чтение данных гироскопа
void L3GD20_ReadGyroData(GyroData_t* gyroData) {
    uint8_t data[6];
    
    SPI1_CS_Low();
    
    // Отправляем команду множественного чтения
    SPI_Transfer(L3GD20_OUT_X_L | L3GD20_READ_BIT | L3GD20_MULTI_BIT);
    
    // Читаем 6 байт данных
    for (int i = 0; i < 6; i++) {
        data[i] = SPI_Transfer(0x00);
    }
    
    SPI1_CS_High();
    
    // Объединяем байты в 16-битные значения
    gyroData->x = (int16_t)((data[1] << 8) | data[0]);
    gyroData->y = (int16_t)((data[3] << 8) | data[2]);
    gyroData->z = (int16_t)((data[5] << 8) | data[4]);
}

// Инициализация датчика
uint8_t L3GD20_Init(void) {
    // Проверка WHO_AM_I
    uint8_t who_am_i = L3GD20_ReadRegister(L3GD20_WHO_AM_I);
    if (who_am_i != L3GD20_ID) {
        return 0; // Ошибка инициализации
    }
    
    // Настройка CTRL_REG1
    L3GD20_WriteRegister(L3GD20_CTRL_REG1, 0x0F); // Все оси, 95Hz
    
    // Настройка CTRL_REG4
    L3GD20_WriteRegister(L3GD20_CTRL_REG4, 0x30); // 2000 dps
    
    return 1; // Успех
}

// Преобразование в dps
void GyroData_To_DPS(GyroData_t* raw, float* dps) {
    const float sensitivity = 70.0f / 1000.0f; // dps/digit для 2000dps
    
    dps[0] = raw->x * sensitivity;
    dps[1] = raw->y * sensitivity;
    dps[2] = raw->z * sensitivity;
}

#endif // GYRO_MY_H