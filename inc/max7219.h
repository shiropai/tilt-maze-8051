/* max7219.h — 兩組 MAX7219：點矩陣（2 片串接）與七段顯示器（1 片）
 * 改寫自課堂 matrix / seven_segment / IR_sensor 實驗，三者共用 CLK、DIN，各自有 CS。 */
#ifndef MAX7219_H
#define MAX7219_H

#include "stdutils.h"

#define MAX7219_NOOP         0x00
#define MAX7219_DECODE_MODE  0x09
#define MAX7219_INTENSITY    0x0A
#define MAX7219_SCAN_LIMIT   0x0B
#define MAX7219_SHUT_DOWN    0x0C
#define MAX7219_DISPLAY_TEST 0x0F

/* 七段 Code-B 解碼的特殊字元 */
#define SEG_DASH   0x0A
#define SEG_BLANK  0x0F
#define SEG_DP     0x80     /* OR 上去會點亮小數點 */

void max7219_init(void);

/* 點矩陣畫面緩衝區（16 bytes）：matrix_fb[y] 是左片第 y 行、matrix_fb[8 + y] 是右片第 y 行，
 * bit7 = 該片最左邊那一欄。畫好之後呼叫 matrix_flush() 一次送出。
 * （不用指標當參數：SDCC 的通用指標一個要 3 bytes RAM） */
extern uint8_t matrix_fb[16];
void matrix_clear_fb(void);
void matrix_set_pixel(uint8_t x, uint8_t y);    /* x = 0..15, y = 0..7 */
void matrix_flush(void);
void matrix_set_intensity(uint8_t level);   /* 0..15 */

/* 七段：digit = 0..7（0 = 最右邊），value = 0..9 或 SEG_DASH / SEG_BLANK，可 OR SEG_DP */
void seg_write_digit(uint8_t digit, uint8_t value);
void seg_show_uint(uint16_t value, uint8_t first_digit, uint8_t width);
void seg_clear(void);

#endif
