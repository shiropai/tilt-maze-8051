/* max7219.c
 *
 * 注意：MAX7219 不管 CS 是高是低都會把 CLK 上的資料移進移位暫存器，
 * 只有在 CS（LOAD）上升時才鎖存。所以和 MCP3202 共用 CLK/DIN 沒關係：
 * 下一次寫 MAX7219 時會先把舊資料完整推出去，再拉高 CS 鎖存。 */
#include "board.h"
#include "max7219.h"
#include "sys.h"            /* pow10_tbl */

static void bus_send_byte(uint8_t b)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        BUS_CLK = 0;
        BUS_DIN = (b & 0x80) ? 1 : 0;
        b <<= 1;
        BUS_CLK = 1;
    }
    BUS_CLK = 0;
}

/* 點矩陣兩片串接：先送「遠端」那片，再送「近端」那片，最後拉高 CS 一起鎖存 */
static void matrix_write_both(uint8_t addr, uint8_t far_data, uint8_t near_data)
{
    CS_MATRIX = 0;
    bus_send_byte(addr);
    bus_send_byte(far_data);
    bus_send_byte(addr);
    bus_send_byte(near_data);
    CS_MATRIX = 1;
}

static void seg_write_reg(uint8_t addr, uint8_t dat)
{
    CS_SEG = 0;
    bus_send_byte(addr);
    bus_send_byte(dat);
    CS_SEG = 1;
}

void max7219_init(void)
{
    uint8_t i;

    CS_MATRIX = 1;
    CS_SEG    = 1;
    CS_ADC    = 1;
    BUS_CLK   = 0;

    /* 點矩陣：不解碼、掃 8 行、亮度低一點（比較不刺眼，也比較省電） */
    matrix_write_both(MAX7219_SHUT_DOWN,    0x01, 0x01);
    matrix_write_both(MAX7219_DISPLAY_TEST, 0x00, 0x00);
    matrix_write_both(MAX7219_DECODE_MODE,  0x00, 0x00);
    matrix_write_both(MAX7219_SCAN_LIMIT,   0x07, 0x07);
    matrix_write_both(MAX7219_INTENSITY,    0x02, 0x02);
    for (i = 1; i <= 8; i++)
        matrix_write_both(i, 0x00, 0x00);

    /* 七段：Code-B 解碼，直接送 0~9 */
    seg_write_reg(MAX7219_SHUT_DOWN,    0x01);
    seg_write_reg(MAX7219_DISPLAY_TEST, 0x00);
    seg_write_reg(MAX7219_DECODE_MODE,  0xFF);
    seg_write_reg(MAX7219_SCAN_LIMIT,   SEG_DIGITS - 1);
    seg_write_reg(MAX7219_INTENSITY,    0x04);
    seg_clear();
}

uint8_t matrix_fb[16];

void matrix_clear_fb(void)
{
    uint8_t i;
    for (i = 0; i < 16; i++)
        matrix_fb[i] = 0;
}

void matrix_set_pixel(uint8_t x, uint8_t y)
{
    if (x < 8) matrix_fb[y]     |= (uint8_t)(0x80 >> x);
    else       matrix_fb[8 + y] |= (uint8_t)(0x80 >> (x - 8));
}

void matrix_flush(void)
{
    uint8_t r;
    for (r = 0; r < 8; r++) {
#if MATRIX_NEAR_IS_LEFT
        matrix_write_both(r + 1, matrix_fb[8 + r], matrix_fb[r]);
#else
        matrix_write_both(r + 1, matrix_fb[r], matrix_fb[8 + r]);
#endif
    }
}

void seg_write_digit(uint8_t digit, uint8_t value)
{
    seg_write_reg(digit + 1, value);
}

/* 不用 / 和 %（會連結函式庫 _divuint / _moduint，約 150 bytes）：
 * 從最高位開始，用 10 的次方「一直減」算出每一位。超過 width 位的高位會被丟掉。 */
void seg_show_uint(uint16_t value, uint8_t first_digit, uint8_t width)
{
    uint8_t k = 5, d;
    while (k--) {
        d = 0;
        while (value >= pow10_tbl[k]) { value -= pow10_tbl[k]; d++; }
        if (k < width)
            seg_write_digit(first_digit + k, d);
    }
}

void seg_clear(void)
{
    uint8_t i;
    for (i = 0; i < SEG_DIGITS; i++)
        seg_write_digit(i, SEG_BLANK);
}
