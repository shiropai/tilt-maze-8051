/* adc3202.c — 邏輯沿用 IR_sensor 實驗的 read_adc3202()，腳位改用 board.h 的共用匯流排 */
#include "board.h"
#include "adc3202.h"

static void spi_delay(void)
{
    uint8_t i;
    for (i = 0; i < 2; i++);
}

static uint8_t spi_transfer(uint8_t data_in)
{
    uint8_t data_out = 0;
    uint8_t i;
    for (i = 0; i < 8; i++) {
        data_out <<= 1;
        BUS_CLK = 0;
        spi_delay();
        BUS_DIN = (data_in & 0x80) ? 1 : 0;
        spi_delay();
        BUS_CLK = 1;
        spi_delay();
        if (BUS_MISO)
            data_out |= 0x01;
        data_in <<= 1;
    }
    return data_out;
}

uint16_t adc3202_read(uint8_t channel)
{
    uint16_t value;
    uint8_t b;

    BUS_MISO = 1;               /* 8051 準雙向 I/O：先寫 1 才能當輸入 */
    CS_ADC = 0;
    spi_transfer(0x01);                             /* start bit */
    b = spi_transfer(channel ? 0xE0 : 0xA0);        /* single-ended, MSBF + 高 4 位 */
    value = (uint16_t)(b & 0x0F) << 8;
    b = spi_transfer(0x00);                         /* 低 8 位 */
    value |= b;
    CS_ADC = 1;
    BUS_CLK = 0;
    BUS_DIN = 0;
    return value;
}
