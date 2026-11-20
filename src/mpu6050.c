/* mpu6050.c — 改寫自課堂 OLED_MPU6050 實驗
 * 為了省內部 RAM（標準 8051 只有 128 bytes）：拿掉暫存陣列與多餘的變數，
 * readByte / writeByte 改成巨集（見 mpu6050.h），讀取直接組成 16-bit 值。
 * 優化方向（見 docs/HANDOFF.md）：每個位元組都是一次獨立的 I2C 交易，
 * 之後可以改成連續讀（burst read）加快速度。 */
#include "mpu6050.h"
#include "i2c.h"
#include "delay.h"

void MPU6050_INIT(void)
{
    delay_ms(100);
    writeByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x00);              /* 解除睡眠 */
    delay_ms(100);
    writeByte(MPU6050_ADDRESS, SIGNAL_PATH_RESET, 0x07);
    writeByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x01);              /* 時脈：X 軸陀螺 PLL */
    writeByte(MPU6050_ADDRESS, CONFIG, 0x03);                  /* DLPF ≈ 44 Hz，濾掉手抖 */
    writeByte(MPU6050_ADDRESS, SMPLRT_DIV, 0);
    writeByte(MPU6050_ADDRESS, GYRO_CONFIG, GFS_500DPS << 3);
    writeByte(MPU6050_ADDRESS, ACCEL_CONFIG, AFS_2G << 3);     /* ±2g：1g = 16384 */
    writeByte(MPU6050_ADDRESS, ACCEL_CONFIG2, 3);
    writeByte(MPU6050_ADDRESS, INT_PIN_CFG, 0x22);
    writeByte(MPU6050_ADDRESS, INT_ENABLE, 0x00);
}

static int read16(uint8_t reg_h)
{
    uint8_t h = readByte(MPU6050_ADDRESS, reg_h);
    return (int)(((uint16_t)h << 8) | readByte(MPU6050_ADDRESS, reg_h + 1));
}

void readMPU6050_AccelData(int *buffer)
{
    buffer[0] = read16(ACCEL_XOUT_H);
    buffer[1] = read16(ACCEL_YOUT_H);
    buffer[2] = read16(ACCEL_ZOUT_H);
}

void readMPU6050_GyroData(int *buffer)
{
    buffer[0] = read16(GYRO_XOUT_H);
    buffer[1] = read16(GYRO_YOUT_H);
    buffer[2] = read16(GYRO_ZOUT_H);
}

void resetMPU6050(void)
{
    writeByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x80);
    delay_ms(100);
}
