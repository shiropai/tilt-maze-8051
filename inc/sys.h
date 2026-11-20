/* sys.h — 系統服務：Timer0 時基、軟體 PWM、蜂鳴器、外部中斷按鍵、UART
 *
 * 資源分配（標準 8051 只有 Timer0 / Timer1）：
 *   Timer0  mode 2，每 250 us 中斷一次 → 1 ms 計數、LED 軟體 PWM、蜂鳴器方波
 *   Timer1  mode 2，UART 9600 baud（不能另作他用）
 *   INT0    暫停鍵（下降緣）    INT1  選關鍵（下降緣）
 *   Serial  RX 進環形緩衝區；TX 用旗標等待
 *
 * SDCC 規定：中斷函式的原型必須出現在含 main() 的檔案裡，
 * 所以 main.c 一定要 #include "sys.h"。 */
#ifndef SYS_H
#define SYS_H

#include "stdutils.h"

/* ---- 中斷函式原型（不要刪） ---- */
void int0_isr(void)   __interrupt(0);
void timer0_isr(void) __interrupt(1);
void int1_isr(void)   __interrupt(2);
void uart_isr(void)   __interrupt(4);

void     sys_init(void);
uint16_t sys_ms(void);                 /* 開機後的毫秒數（每 65.5 s 溢位一次，用差值比較） */

/* ---- 按鍵事件：中斷裡設為 1（已去彈跳），主程式讀完自己清 0 ---- */
extern volatile __bit evt_pause;
extern volatile __bit evt_select;

/* ---- LED 軟體 PWM：mask 決定哪幾顆亮，duty = 0..16 決定亮度 ---- */
void led_set(uint8_t mask, uint8_t duty);

/* ---- 蜂鳴器：half_period 以 250 us 為單位（1 → 2 kHz、2 → 1 kHz、4 → 500 Hz） ---- */
void buzzer_beep(uint8_t half_period, uint16_t duration_ms);

/* ---- UART 9600 8N1 ---- */
void    uart_putc(char c);
void    uart_puts(const char *s);
/* uart_put_uint / uart_put_int 目前沒用到，為了 4 KB flash 先刪除；M6 傳成績時再加回 */
uint8_t uart_available(void);
char    uart_getc(void);              /* 沒資料時回傳 0 */

#endif
