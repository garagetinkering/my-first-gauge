#pragma once
#include <Wire.h> 

#define I2C_SCL_PIN       7
#define I2C_SDA_PIN       15

#define I2C_OK            true
#define I2C_FAIL          false

void I2C_Init(void);

bool I2C_Read(uint8_t driver_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length);
bool I2C_Write(uint8_t driver_addr, uint8_t reg_addr, const uint8_t *reg_data, uint32_t length);