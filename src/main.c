/******************************************************************************
 *  體感平衡迷宮 Tilt Maze — 8051 期末專題
 *
 *  目前版本：M1 硬體整合測試（Hardware bring-up）
 *  ⚠ 已用 SDCC 編譯通過，但還沒有在實機上跑過。
 *
 *  這一版的目的：一次確認所有硬體都能同時運作，還沒有迷宮。
 *    - MPU6050 傾斜 → 點矩陣（16 x 8）上的球滾動，碰到邊界會反彈並發出聲音
 *    - OLED  ：顯示 AX、AY、可變電阻（POT）、IR 感測器的讀值
 *    - 七段  ：右 4 位 = 經過秒數，左 4 位 = 可變電阻讀值
 *    - INT0  ：暫停／繼續          INT1：球回到中央
 *    - LED   ：PWM 呼吸燈（暫停時全亮）
 *    - UART  ：開機送出 READY，事件會回報；從電腦送 'r' 也能讓球回中央
 *  完整遊戲規劃見 docs/PROJECT_CONTEXT.md。
 ******************************************************************************/
#include "board.h"
#include "sys.h"              /* 中斷原型，一定要 include 在 main.c */
#include "delay.h"
#include "i2c.h"
#include "mpu6050.h"
#include "oled_i2c.h"
#include "max7219.h"
#include "adc3202.h"

#define FRAME_MS      20      /* 遊戲更新週期：50 Hz */
#define POS_MAX_X     (((MATRIX_W - 1) << 8) + 255)   /* Q8.8 定點數 */
#define POS_MAX_Y     (((MATRIX_H - 1) << 8) + 255)
#define V_MAX         48      /* 每個 frame 最多移動 48/256 格 */

/* RAM 很緊（標準 8051 只有 128 bytes，堆疊也在裡面），所以：
 *   - 全域變數盡量用 8-bit；- 不用指標當參數（SDCC 通用指標 3 bytes）；
 *   - x / y 兩軸用陣列 + 索引，同一個函式處理。 */
#define AX_X 0
#define AX_Y 1
static int8_t  off[2];        /* 校正零點（水平時的讀值） */
static int16_t pos[2];        /* 球的位置，Q8.8：高 8 位是格子座標 */
static int16_t vel[2];        /* 速度，Q8.8 格 / frame */
static int8_t  tilt[2];       /* 扣掉零點、修正方向後的傾斜量（1g ≈ 64） */
static __bit   paused;

/* 只讀加速度的高位元組：±2g 時 1g = 64，對遊戲夠用，而且 I2C 只要讀 2 次 */
static int8_t read_tilt_x(void) { return (int8_t)readByte(MPU6050_ADDRESS, ACCEL_XOUT_H); }
static int8_t read_tilt_y(void) { return (int8_t)readByte(MPU6050_ADDRESS, ACCEL_YOUT_H); }

static void calibrate(void)
{
    static int16_t sx, sy;
    uint8_t i;
    sx = 0;
    sy = 0;
    for (i = 0; i < 16; i++) {
        sx += read_tilt_x();
        sy += read_tilt_y();
        delay_ms(5);
    }
    off[AX_X] = (int8_t)(sx >> 4);
    off[AX_Y] = (int8_t)(sy >> 4);
}

static void read_tilt(void)
{
    tilt[AX_X] = (int8_t)(TILT_X_SIGN * (read_tilt_x() - off[AX_X]));
    tilt[AX_Y] = (int8_t)(TILT_Y_SIGN * (read_tilt_y() - off[AX_Y]));
}

static void ball_center(void)
{
    pos[AX_X] = (int16_t)((MATRIX_W / 2) << 8);
    pos[AX_Y] = (int16_t)((MATRIX_H / 2) << 8);
    vel[AX_X] = 0;
    vel[AX_Y] = 0;
}

/* 一個軸的物理：傾斜 → 速度（含摩擦）→ 位置，碰邊界反彈。回傳 1 表示撞牆 */
static __bit axis_step(uint8_t a)
{
    int16_t v = vel[a];
    int16_t p;
    int16_t pmax = (a == AX_X) ? POS_MAX_X : POS_MAX_Y;
    __bit hit = 0;

    v += tilt[a] >> 2;        /* 傾斜越大加速越快；係數之後依手感調整 */
    v -= v >> 4;              /* 摩擦 */
    if (v >  V_MAX) v =  V_MAX;
    if (v < -V_MAX) v = -V_MAX;
    p = pos[a] + v;
    if (p < 0)    { p = 0;    v = -(v >> 1); hit = 1; }
    if (p > pmax) { p = pmax; v = -(v >> 1); hit = 1; }
    pos[a] = p;
    vel[a] = v;
    return hit;
}

static void render_ball(void)
{
    matrix_clear_fb();
    matrix_set_pixel((uint8_t)(pos[AX_X] >> 8), (uint8_t)(pos[AX_Y] >> 8));
    matrix_flush();
}

static void oled_signed(uint8_t line, uint8_t col, int16_t v)
{
    OLED_SetCursor(line, col);
    if (v < 0) { OLED_DisplayChar('-'); v = -v; }
    else       { OLED_DisplayChar(' '); }
    OLED_DisplayUInt((uint16_t)v, 4);
}

static void handle_uart(void)
{
    char c;
    while (uart_available()) {
        c = uart_getc();
        if (c == 'r' || c == 'R') {
            ball_center();
            uart_puts("RESET\r\n");
        }
    }
}

void main(void)
{
    uint16_t last, seconds = 0;
    uint8_t  frame = 0, breath = 0;
    __bit    breath_up = 1;

    sys_init();
    SDA = 1;
    SCL = 1;
    max7219_init();
    OLED_Init();
    MPU6050_INIT();

    OLED_SetCursor(0, 0);  OLED_DisplayString("TILT MAZE");
    OLED_SetCursor(1, 0);  OLED_DisplayString("M1 HW TEST");
    OLED_SetCursor(3, 0);  OLED_DisplayString("AX:");
    OLED_SetCursor(4, 0);  OLED_DisplayString("AY:");
    OLED_SetCursor(5, 0);  OLED_DisplayString("POT:");
    OLED_SetCursor(6, 0);  OLED_DisplayString("IR:");

    OLED_SetCursor(7, 0);  OLED_DisplayString("HOLD FLAT...");
    calibrate();
    OLED_SetCursor(7, 0);  OLED_DisplayString("            ");

    ball_center();
    buzzer_beep(2, 150);
    uart_puts("TILT MAZE M1 READY\r\n");

    last = sys_ms();
    while (1) {
        handle_uart();

        if ((uint16_t)(sys_ms() - last) < FRAME_MS)
            continue;
        last += FRAME_MS;

        /* ---- 按鍵事件（外部中斷） ---- */
        if (evt_pause) {
            evt_pause = 0;
            paused = !paused;
            buzzer_beep(paused ? 4 : 2, 80);
            uart_puts(paused ? "PAUSE\r\n" : "RESUME\r\n");
        }
        if (evt_select) {
            evt_select = 0;
            ball_center();
            buzzer_beep(1, 60);
            uart_puts("CENTER\r\n");
        }

        /* ---- 讀感測器、更新物理 ---- */
        read_tilt();
        if (!paused) {
            __bit hit = axis_step(AX_X);
            if (axis_step(AX_Y)) hit = 1;
            if (hit) buzzer_beep(1, 15);
        }
        render_ball();

        /* ---- LED 呼吸燈（PWM） ---- */
        if (paused) {
            led_set(0xFF, 16);
        } else {
            if (breath_up) { if (++breath >= 16) breath_up = 0; }
            else           { if (--breath == 0)  breath_up = 1; }
            led_set(0xFF, breath);
        }

        /* ---- 慢速更新：OLED 很慢，所以每個 frame 只更新一個欄位 ---- */
        switch (frame) {
            case 1:  oled_signed(3, 30, tilt[AX_X]);   break;
            case 2:  oled_signed(4, 30, tilt[AX_Y]);   break;
            case 3:  OLED_SetCursor(5, 30);
                     OLED_DisplayUInt(adc3202_read(ADC_CH_POT), 4);   break;
            case 4:  OLED_SetCursor(6, 30);
                     OLED_DisplayUInt(adc3202_read(ADC_CH_IR), 4);    break;
            case 5:  seg_show_uint(adc3202_read(ADC_CH_POT), 4, 4);   break;
            default: break;
        }
        if (++frame >= 50) {          /* 50 × 20 ms = 1 s */
            frame = 0;
            if (!paused) seconds++;
            seg_show_uint(seconds, 0, 4);
        }
    }
}
