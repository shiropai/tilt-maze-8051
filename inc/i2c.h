/* i2c.h — 軟體 I2C（bit-bang），沿用課堂 OLED_MPU6050 實驗；腳位在 board.h */
#ifndef I2C_H
#define I2C_H

#include "board.h"
#include "stdutils.h"

void    I2C_Start(void);
void    I2C_Stop(void);
void    I2C_SendACK(__bit ack);
__bit   I2C_RecvACK(void);
void    I2C_SendByte(uint8_t dat);
uint8_t I2C_RecvByte(void);
void    Single_WriteI2C(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t REG_data);
uint8_t Single_ReadI2C(uint8_t SlaveAddress, uint8_t REG_Address);
void    Delay(void);

#endif
