/******************************************************************************
 *  board.h  —  體感平衡迷宮：所有腳位與硬體參數集中在這裡
 *
 *  晶片：AT89S51 — 4 KB flash、128 B 內部 RAM（2026-11-20 使用者確認）
 *        makefile 用 --code-size 4096 --iram-size 128 檢查，超過會編譯失敗。
 *
 *  ⚠ 標記「待確認」的項目還沒在實機上驗證。接線若和這裡不同，
 *    只改這個檔案，不要在各個驅動裡寫死腳位。
 ******************************************************************************/
#ifndef BOARD_H
#define BOARD_H

#include <8051.h>

/* ---------------- 系統時脈 ------------------------------------------------
 * 晶振 11.0592 MHz（2026-11-20 使用者確認）。TH1 = 0xFD 剛好 9600 baud。
 * 若換成 12 MHz 的板子，把 FOSC_11_0592MHZ 改成 0。 */
#define FOSC_11_0592MHZ     1

#if FOSC_11_0592MHZ
#define T0_RELOAD           26      /* 256 - 230 → 230 × 1.085 us ≈ 250 us   */
#define UART_TH1_9600       0xFD
#else
#define T0_RELOAD           6       /* 256 - 250 → 250 us @ 12 MHz          */
#define UART_TH1_9600       0xFD    /* 12 MHz 下約 8929 baud，誤差偏大       */
#endif

/* ---------------- I2C 匯流排：MPU6050 + OLED 共用 --------------------------
 * 兩個實驗（OLED_MPU6050）都用 P0_0 / P0_1，位址不衝突：
 *   MPU6050 = 0xD0 (0x68 << 1)，OLED SSD1306 = 0x78 (0x3C << 1)
 * 注意：七段實驗的 3x3 鍵盤也接在 P0，本專題不使用鍵盤。 */
#define SCL                 P0_0
#define SDA                 P0_1

/* ---------------- 序列匯流排：MAX7219 ×2 組 + MCP3202 ADC 共用 --------------
 * 參考 IR_sensor 實驗：CLK / DIN 共用，各元件用自己的 CS。
 *   BUS_CLK  → MAX7219 CLK、MCP3202 CLK
 *   BUS_DIN  → MAX7219 DIN、MCP3202 DIN
 *   BUS_MISO ← MCP3202 DOUT
 * 待確認：點矩陣模組（外接杜邦線）的 CS 要接到 CS_MATRIX。 */
#define BUS_CLK             P2_0
#define BUS_DIN             P2_1
#define BUS_MISO            P2_2
#define CS_ADC              P2_3    /* MCP3202 CS（IR 實驗的 LOAD）          */
#define CS_SEG              P2_4    /* 七段顯示器 MAX7219（IR 實驗的 LOAD_MAX）*/
#define CS_MATRIX           P2_5    /* 點矩陣 MAX7219 ×2 串接（待確認）       */

/* MCP3202 通道：IR 實驗中 ch1 = IR 感測器、ch0 = 可變電阻 */
#define ADC_CH_POT          0
#define ADC_CH_IR           1

/* ---------------- 點矩陣排列（待確認） ------------------------------------
 * 兩片 8x8 左右並排成 16 x 8 的場地。x = 0..15（左→右），y = 0..7（上→下）。
 * 串接時，先送出的資料會跑到「遠端」那片。
 * MATRIX_NEAR_IS_LEFT = 1：靠近 8051（DIN 直接接的）那片在左邊。 */
#define MATRIX_NEAR_IS_LEFT 1
#define MATRIX_W            16
#define MATRIX_H            8

/* ---------------- 七段顯示器 ---------------------------------------------- */
#define SEG_DIGITS          8       /* 8 位（2026-11-20 使用者確認）         */

/* ---------------- Port 3 ---------------------------------------------------
 * P3_0 RXD / P3_1 TXD : UART（Timer1 產生 9600 baud）
 * P3_2 INT0           : 暫停／開始鍵（下降緣觸發）
 * P3_3 INT1           : 選關／重來鍵（下降緣觸發）
 * P3_7                : 蜂鳴器（Hourglass 實驗的接法）
 *                       待確認：有源或無源。程式送方波，無源才會有不同音高 */
#define BTN_PAUSE           P3_2
#define BTN_SELECT          P3_3
#define BUZZER              P3_7

/* ---------------- LED（P1，低電位點亮） ----------------------------------- */
#define LED_PORT            P1

/* ---------------- MPU6050 軸向（待確認，裝好板子後實測調整） --------------- */
#define TILT_X_SIGN         (+1)    /* 往右傾斜時，球要往 x 增加的方向       */
#define TILT_Y_SIGN         (-1)    /* 往前傾斜時，球要往 y 增加的方向       */

#endif
