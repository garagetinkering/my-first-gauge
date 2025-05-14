#include "I2C_Driver.h"

void I2C_Init(void) {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);                
}

bool I2C_Read(uint8_t driver_addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length) {
  Wire.beginTransmission(driver_addr);
  Wire.write(reg_addr); 
  
  if ( Wire.endTransmission(true)){
    printf("The I2C transmission fails. - I2C Read\r\n");
    return I2C_FAIL;
  }

  if (Wire.requestFrom(driver_addr, length) != length) {
    printf("I2C read length mismatch\r\n");
    return I2C_FAIL;
  }

  for (int i = 0; i < length; i++) {
    *reg_data++ = Wire.read();
  }

  return I2C_OK;
}

bool I2C_Write(uint8_t driver_addr, uint8_t reg_addr, const uint8_t *reg_data, uint32_t length) {
  Wire.beginTransmission(driver_addr);
  Wire.write(reg_addr);

  for (int i = 0; i < length; i++) {
    Wire.write(*reg_data++);
  }

  if ( Wire.endTransmission(true)) {
    printf("The I2C transmission fails. - I2C Write\r\n");
    return I2C_FAIL;
  }
  return I2C_OK;
}