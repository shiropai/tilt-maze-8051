/* sys.c — 見 sys.h 的資源分配說明 */
#include "board.h"
#include "sys.h"

#define DEBOUNCE_MS   150
#define UART_RX_SIZE  8               /* 必須是 2 的次方 */

/* ---- Timer0 時基 ---- */
static volatile uint16_t ms_count;
static volatile uint8_t  sub_ms;      /* 0..3，4 × 250 us = 1 ms */

/* ---- LED PWM ---- */
static volatile uint8_t  pwm_phase;   /* 0..15 */
static volatile uint8_t  led_mask;
static volatile uint8_t  led_duty;    /* 0..16 */

/* ---- 蜂鳴器 ---- */
static volatile uint16_t bz_ms;
static volatile uint8_t  bz_half;
static volatile uint8_t  bz_div;

/* ---- 按鍵 ---- */
volatile __bit evt_pause;
volatile __bit evt_select;
static volatile uint8_t  last_int0_ms;  /* 只存低 8 位：去彈跳 150 ms < 256 ms，差值仍正確 */
static volatile uint8_t  last_int1_ms;

/* ---- UART ---- */
static volatile __bit   tx_ready;
static volatile uint8_t rx_buf[UART_RX_SIZE];
static volatile uint8_t rx_head, rx_tail;

void sys_init(void)
{
    BUZZER    = 0;
    LED_PORT  = 0xFF;                 /* LED 全滅（低電位點亮） */
    BTN_PAUSE = 1;                    /* 準雙向 I/O 當輸入前先寫 1 */
    BTN_SELECT = 1;

    TMOD = 0x22;                      /* T1 mode 2（baud）、T0 mode 2（250 us 時基） */
    TH0  = T0_RELOAD;
    TL0  = T0_RELOAD;
    TH1  = UART_TH1_9600;
    TL1  = UART_TH1_9600;
    SCON = 0x50;                      /* UART mode 1，允許接收 */
    tx_ready = 1;

    IT0 = 1;                          /* INT0 下降緣觸發 */
    IT1 = 1;                          /* INT1 下降緣觸發 */
    PT0 = 1;                          /* Timer0 高優先權，PWM 與聲音比較穩 */

    ET0 = 1;
    EX0 = 1;
    EX1 = 1;
    ES  = 1;
    TR0 = 1;
    TR1 = 1;
    EA  = 1;
}

/* 10 的次方表：七段和 OLED 顯示數字共用（用減法取位數，避免連結除法函式庫） */
__code const uint16_t pow10_tbl[5] = {1, 10, 100, 1000, 10000};

uint16_t sys_ms(void)
{
    uint16_t t;
    ET0 = 0;                          /* 16-bit 變數要原子讀取 */
    t = ms_count;
    ET0 = 1;
    return t;
}

/* ===================== 中斷服務程式 ===================== */

void timer0_isr(void) __interrupt(1)
{
    /* 軟體 PWM：16 階，週期 4 ms（250 Hz），肉眼不會閃 */
    pwm_phase = (pwm_phase + 1) & 0x0F;
    LED_PORT = (pwm_phase < led_duty) ? (uint8_t)~led_mask : 0xFF;

    /* 蜂鳴器方波 */
    if (bz_ms) {
        if (--bz_div == 0) {
            bz_div = bz_half;
            BUZZER = !BUZZER;
        }
    } else {
        BUZZER = 0;
    }

    /* 1 ms 計數 */
    if (++sub_ms >= 4) {
        sub_ms = 0;
        ms_count++;
        if (bz_ms) bz_ms--;
    }
}

void int0_isr(void) __interrupt(0)
{
    if ((uint8_t)((uint8_t)ms_count - last_int0_ms) > DEBOUNCE_MS) {
        last_int0_ms = (uint8_t)ms_count;
        evt_pause = 1;
    }
}

void int1_isr(void) __interrupt(2)
{
    if ((uint8_t)((uint8_t)ms_count - last_int1_ms) > DEBOUNCE_MS) {
        last_int1_ms = (uint8_t)ms_count;
        evt_select = 1;
    }
}

void uart_isr(void) __interrupt(4)
{
    if (RI) {
        uint8_t next = (rx_head + 1) & (UART_RX_SIZE - 1);
        if (next != rx_tail) {        /* 滿了就丟掉 */
            rx_buf[rx_head] = SBUF;
            rx_head = next;
        }
        RI = 0;
    }
    if (TI) {
        TI = 0;
        tx_ready = 1;
    }
}

/* ===================== LED / 蜂鳴器 ===================== */

void led_set(uint8_t mask, uint8_t duty)
{
    led_mask = mask;
    led_duty = (duty > 16) ? 16 : duty;
}

void buzzer_beep(uint8_t half_period, uint16_t duration_ms)
{
    if (half_period == 0) half_period = 1;
    ET0 = 0;
    bz_half = half_period;
    bz_div  = half_period;
    bz_ms   = duration_ms;
    ET0 = 1;
}

/* ===================== UART ===================== */

void uart_putc(char c)
{
    while (!tx_ready);
    tx_ready = 0;
    SBUF = c;
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

uint8_t uart_available(void)
{
    return (uint8_t)((rx_head - rx_tail) & (UART_RX_SIZE - 1));
}

char uart_getc(void)
{
    char c;
    if (rx_head == rx_tail) return 0;
    c = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) & (UART_RX_SIZE - 1);
    return c;
}
