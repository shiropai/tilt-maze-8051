/* oled_i2c.h — SSD1306 128x64 OLED（I2C），精簡自課堂實驗（ExploreEmbedded，BSD license） */
#ifndef _OLED_I2C_H_
#define _OLED_I2C_H_

#include "stdutils.h"

#define OLED_SlaveAddress      0x78    /* 0x3C << 1 */
#define SSD1306_COMMAND        0x00
#define SSD1306_DATA_CONTINUE  0x40

/* 畫面：8 行（line 0..7）× 128 點；每個字 6 點寬 → 一行約 21 字 */
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t lineNumber, uint8_t cursorPosition);
void OLED_DisplayChar(uint8_t ch);              /* 只支援 font.h 有的字；小寫會轉大寫 */
void OLED_DisplayString(const char *ptr);
void OLED_DisplayUInt(uint16_t value, uint8_t digits);   /* 固定位數、補 0 */

void oledSendCommand(uint8_t cmd);

#endif
