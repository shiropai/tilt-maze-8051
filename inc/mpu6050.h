/* mpu6050.h — 精簡自課堂 OLED_MPU6050 實驗（只保留用到的暫存器） */
#ifndef _MPU6050_H
#define _MPU6050_H

#include "stdutils.h"
#include "i2c.h"

#define SMPLRT_DIV        0x19
#define CONFIG            0x1A
#define GYRO_CONFIG       0x1B
#define ACCEL_CONFIG      0x1C
#define ACCEL_CONFIG2     0x1D   /* MPU6500/9250 才有；MPU6050 上寫入無影響 */
#define INT_PIN_CFG       0x37
#define INT_ENABLE        0x38
#define ACCEL_XOUT_H      0x3B
#define ACCEL_XOUT_L      0x3C
#define ACCEL_YOUT_H      0x3D
#define ACCEL_YOUT_L      0x3E
#define ACCEL_ZOUT_H      0x3F
#define ACCEL_ZOUT_L      0x40
#define GYRO_XOUT_H       0x43
#define GYRO_XOUT_L       0x44
#define GYRO_YOUT_H       0x45
#define GYRO_YOUT_L       0x46
#define GYRO_ZOUT_H       0x47
#define GYRO_ZOUT_L       0x48
#define SIGNAL_PATH_RESET 0x68
#define PWR_MGMT_1        0x6B
#define WHO_AM_I_MPU6050  0x75   /* MPU6050 應回傳 0x68 */

/* AD0 接 GND → 7-bit 位址 0x68，左移 1 位成 8-bit 位址 */
#define MPU6050_ADDRESS   0xD0          /* 0x68 << 1；直接寫常數避免 SDCC 溢位警告 */

enum Ascale { AFS_2G = 0, AFS_4G, AFS_8G, AFS_16G };          /* ±2g: 1g = 16384 */
enum Gscale { GFS_250DPS = 0, GFS_500DPS, GFS_1000DPS, GFS_2000DPS };

/* 用巨集直接呼叫 I2C，省下一層函式的參數 RAM */
#define writeByte(addr, reg, dat)  Single_WriteI2C((addr), (reg), (dat))
#define readByte(addr, reg)        Single_ReadI2C((addr), (reg))

void    MPU6050_INIT(void);
/* 遊戲只用 readByte(MPU6050_ADDRESS, ACCEL_XOUT_H / ACCEL_YOUT_H) 讀高位元組。
 * 4 KB flash 放不下沒用到的函式（SDCC 不會自動移除），所以課堂的
 * readMPU6050_AccelData / GyroData / resetMPU6050 已刪除，需要時再加回來。 */

#endif
