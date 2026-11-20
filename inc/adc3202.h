/* adc3202.h — MCP3202 12-bit ADC（SPI），改寫自課堂 IR_sensor 實驗 */
#ifndef ADC3202_H
#define ADC3202_H

#include "stdutils.h"

/* channel：ADC_CH_POT（可變電阻）或 ADC_CH_IR（IR 感測器），回傳 0..4095 */
uint16_t adc3202_read(uint8_t channel);

#endif
