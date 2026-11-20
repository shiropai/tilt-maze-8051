/* oled_i2c.c — 沿用課堂實驗的初始化與游標設定；字元查表改成支援 A~Z */
#include "oled_i2c.h"
#include "i2c.h"
#include "font.h"
#include "sys.h"            /* pow10_tbl */

/* 初始化指令表（和課堂版本相同的指令與順序，只是改成查表 + 迴圈，省 flash） */
__code static const uint8_t oled_init_cmds[] = {
    0xA8, 0x3F,     /* MUX ratio */
    0xD3, 0x00,     /* display offset */
    0x40,           /* start line */
    0xA1,           /* segment re-map */
    0xC8,           /* COM scan direction */
    0xDA, 0x12,
    0x81, 0x7F,     /* contrast */
    0xA4,
    0xA6,           /* normal display */
    0xD5, 0x80,
    0x8D, 0x14,     /* charge pump */
    0xAF,           /* display ON */
};

void OLED_Init(void)
{
    uint8_t i;
    for (i = 0; i < sizeof(oled_init_cmds); i++)
        oledSendCommand(oled_init_cmds[i]);
    OLED_Clear();
}

/* 字元 → font.h 的索引（每字 5 bytes） */
static uint8_t font_index(uint8_t ch)
{
    if (ch >= 'a' && ch <= 'z') ch -= 32;              /* 小寫轉大寫 */
    if (ch >= '0' && ch <= '9') return (uint8_t)(ch - '0' + 8);
    if (ch >= 'A' && ch <= 'Z') return (uint8_t)(ch - 'A' + 18);
    switch (ch) {
        case '-': return 1;
        case ':': return 2;
        case '.': return 3;
        case '!': return 4;
        case '>': return 5;
        case '<': return 6;
        case '%': return 7;
        default:  return 0;                             /* 其他字元顯示空白 */
    }
}

/* 連續寫入（burst）：一次 Start + 位址 + 0x40，後面可以接任意多個資料 byte。
 * 課堂版本每個 byte 都重開一次 I2C 交易，一個字要 6 次；改成 1 次，快約 3 倍。 */
static void oled_data_begin(void)
{
    I2C_Start();
    I2C_SendByte(OLED_SlaveAddress);
    I2C_SendByte(SSD1306_DATA_CONTINUE);
}

static void oled_data_end(void)
{
    I2C_Stop();
}

void OLED_DisplayChar(uint8_t ch)
{
    uint8_t i;
    uint16_t index;

    if (ch == '\n') return;
    index = (uint16_t)font_index(ch) * 5;
    oled_data_begin();
    for (i = 0; i < 5; i++)
        I2C_SendByte(OledFontTable[index + i]);
    I2C_SendByte(0x00);                                 /* 字距 */
    oled_data_end();
}

/* __code 指標：只接受字串常數，比通用指標小又快（不用 _gptrget） */
void OLED_DisplayString(__code const char *ptr)
{
    while (*ptr)
        OLED_DisplayChar((uint8_t)*ptr++);
}

/* 和 seg_show_uint 一樣用 pow10_tbl 一直減，不用除法 */
void OLED_DisplayUInt(uint16_t value, uint8_t digits)
{
    uint8_t k = 5, d;
    while (k--) {
        d = 0;
        while (value >= pow10_tbl[k]) { value -= pow10_tbl[k]; d++; }
        if (k < digits)
            OLED_DisplayChar((uint8_t)('0' + d));
    }
}

void OLED_Clear(void)
{
    uint8_t col, page;
    for (page = 0; page < 8; page++) {
        OLED_SetCursor(page, 0);
        oled_data_begin();
        for (col = 0; col < 128; col++)
            I2C_SendByte(0);
        oled_data_end();
    }
}

/* 課堂版本會把 column + 2（模組可能是 SH1106 的 132 欄），保留原樣 */
void OLED_SetCursor(uint8_t lineNumber, uint8_t cursorPosition)
{
    cursorPosition = cursorPosition + 2;
    oledSendCommand(0x0F & cursorPosition);
    oledSendCommand(0x10 | (cursorPosition >> 4));
    oledSendCommand(0xB0 | lineNumber);
}

void oledSendCommand(uint8_t cmd)
{
    Single_WriteI2C(OLED_SlaveAddress, SSD1306_COMMAND, cmd);
}
