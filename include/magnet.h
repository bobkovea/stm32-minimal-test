#ifndef LSM303AGR_H
#define LSM303AGR_H

#include "i2c.h"

#define LSM303AGR_ACC_ADDR   (0x32 >> 1)  // 0x19
#define LSM303AGR_MAG_ADDR   (0x3C >> 1)  // 0x1E

// Регистры акселерометра LSM303AGR
#define LSM303AGR_CTRL_REG1_A  0x20
#define LSM303AGR_CTRL_REG4_A  0x23
#define LSM303AGR_OUT_X_L_A    0x28
#define LSM303AGR_STATUS_REG_A 0x27

// Регистры магнитометра LSM303AGR
#define LSM303AGR_CFG_REG_A_M  0x60
#define LSM303AGR_CFG_REG_B_M  0x61
#define LSM303AGR_CFG_REG_C_M  0x62
#define LSM303AGR_STATUS_REG_M 0x67
#define LSM303AGR_OUTX_L_REG_M 0x68

// Регистры температуры LSM303AGR
#define LSM303AGR_TEMP_CFG_REG_A 0x1F
#define LSM303AGR_STATUS_REG_AUX_A  0x07
#define LSM303AGR_OUT_TEMP_L_A   0x0C
#define LSM303AGR_OUT_TEMP_H_A   0x0D

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} SensorData;

// LSM303AGR функции
void LSM303AGR_Init(void);
void LSM303AGR_InitTemperature(void);
uint8_t LSM303AGR_CheckConnection(void);
void LSM303AGR_ReadAccelerometer(SensorData* accel);
void LSM303AGR_ReadMagnetometer(SensorData* mag);
int8_t LSM303AGR_ReadRawTemperature();
float LSM303AGR_ReadTemperatureCelsius(void);
uint8_t LSM303AGR_TemperatureReady(void);


// LSM303AGR функции
void LSM303AGR_Init(void) {
    // Инициализация акселерометра
    I2C_Write(LSM303AGR_ACC_ADDR, LSM303AGR_CTRL_REG1_A, 0x57); // 100Hz, все оси, включен
    I2C_Write(LSM303AGR_ACC_ADDR, LSM303AGR_CTRL_REG4_A, 0x08); // High resolution
    
    // Инициализация магнитометра
    I2C_Write(LSM303AGR_MAG_ADDR, LSM303AGR_CFG_REG_A_M, 0x43); // 50Hz, непрерывный режим
    I2C_Write(LSM303AGR_MAG_ADDR, LSM303AGR_CFG_REG_B_M, 0x00); // Gain по умолчанию
    I2C_Write(LSM303AGR_MAG_ADDR, LSM303AGR_CFG_REG_C_M, 0x00); // Continuous mode
}



uint8_t LSM303AGR_CheckConnection(void) {
    return I2C_CheckDevice(LSM303AGR_ACC_ADDR) && I2C_CheckDevice(LSM303AGR_MAG_ADDR);
}

void LSM303AGR_ReadAccelerometer(SensorData* accel) {
    uint8_t buffer[6];
    I2C_ReadMultiple(LSM303AGR_ACC_ADDR, LSM303AGR_OUT_X_L_A, buffer, 6);
    
    accel->x = (int16_t)((buffer[1] << 8) | buffer[0]);
    accel->y = (int16_t)((buffer[3] << 8) | buffer[2]);
    accel->z = (int16_t)((buffer[5] << 8) | buffer[4]);
}

void LSM303AGR_ReadMagnetometer(SensorData* mag) {
    uint8_t buffer[6];
    I2C_ReadMultiple(LSM303AGR_MAG_ADDR, LSM303AGR_OUTX_L_REG_M, buffer, 6);
    
    mag->x = (int16_t)((buffer[1] << 8) | buffer[0]);
    mag->y = (int16_t)((buffer[3] << 8) | buffer[2]);
    mag->z = (int16_t)((buffer[5] << 8) | buffer[4]);
}


void LSM303AGR_InitTemperature(void) {
    // Включение датчика температуры в акселерометре
    // TEMP_CFG_REG_A: биты 6-7 = 11 (включить датчик температуры)
    I2C_Write(LSM303AGR_ACC_ADDR, LSM303AGR_TEMP_CFG_REG_A, 0xC0);
	I2C_Write(LSM303AGR_ACC_ADDR, LSM303AGR_CTRL_REG4_A, 0x80); // BDU enable
	
	// Включение акселерометра: ODR = 100Hz, все оси включены
    I2C_Write(LSM303AGR_ACC_ADDR, LSM303AGR_CTRL_REG1_A, 0x57);
}

int8_t LSM303AGR_ReadRawTemperature() {
	
	I2C_Read(LSM303AGR_ACC_ADDR, LSM303AGR_OUT_TEMP_L_A);
	const int8_t temp_raw = (int8_t)I2C_Read(LSM303AGR_ACC_ADDR, LSM303AGR_OUT_TEMP_H_A);
	return temp_raw;
}

uint8_t LSM303AGR_TemperatureReady(void) {
    uint8_t status = I2C_Read(LSM303AGR_ACC_ADDR, LSM303AGR_STATUS_REG_AUX_A);
    return (status & 0x04); // TDA = 1 (Temperature new data available)
}


float LSM303AGR_ReadTemperatureCelsius(void) {

    const int8_t temp_raw = LSM303AGR_ReadRawTemperature();

    // Температура (°C) = (RAW / 8) + 25
    return (float)temp_raw / 8.0f + 25.0f;
}



#endif // LSM303AGR_H