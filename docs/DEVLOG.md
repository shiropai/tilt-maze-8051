# DEVLOG：開發日誌

> **Claude Code 寫，聊天端 Claude 讀。** 新的紀錄加在最上面，舊紀錄不要改。
> 聊天端 Claude 會根據這裡的內容給建議，所以請寫清楚「為什麼」和「卡在哪」。

## 日誌格式（複製這段）

```markdown
### YYYY-MM-DD HH:MM　[M?] 一句話標題
- **做了什麼**：
- **改了哪些檔案**：
- **編譯結果**：成功／失敗；程式碼 ____ bytes；Stack starts at 0x__
- **實機測試**：未測／已測（使用者回報：……）
- **遇到的問題**：
- **需要使用者或聊天端 Claude 決定的事**：
- **下一步**：
- **commit**：
```

---

### 2026-11-20 23:30　[M1] 縮減 flash：4619 → 3585 bytes，可以放進 AT89S51
- **做了什麼**：使用者確認晶片是 **AT89S51（4 KB flash、128 B RAM）**，原本的程式放不下，所以依照使用者（聊天端 Claude）的清單逐項縮減。每一步都重新 `make` 並各自 commit：

  | 步驟 | ROM（bytes） | 減少 | commit |
  |---|---:|---:|---|
  | 起點（SDCC 4.6.0；聊天端用 4.2.0 量到 4653） | 4619 | | |
  | 1. 刪除沒用到的函式：`readMPU6050_AccelData`、`readMPU6050_GyroData`、`resetMPU6050`、`read16`、`matrix_set_intensity`、`uart_put_int`、`uart_put_uint`、`oledSendData`（都先用 grep 確認沒有被呼叫） | 4076 | −543 | d30483f |
  | 2. `seg_show_uint` 的 `%`、`/` 改成「10 的次方表一直減」，七段和 OLED 共用 `pow10_tbl`（`sys.c`），不再連結 `_divuint`、`_moduint` | 3933 | −143 | e53daf6 |
  | 3. `uart_puts`、`OLED_DisplayString` 參數改成 `__code const char *`，不再連結 `_gptrget` | 3840 | −93 | 114c63d |
  | 4. `OLED_Init`、`MPU6050_INIT` 改成 `__code` 指令表 + 迴圈（送出的指令、順序、延遲都和原本相同） | 3721 | −119 | 326499c |
  | 5. 字型只保留 M1 用到的 31 個字（44 → 31 字），`font_index` 改成在 `FONT_CHARS` 字串裡找位置；字型 ×5 改成 `(i<<2)+i` 的 8 位元運算 | 3585 | −136 | 0d9c72b |
  | 6. makefile 加 `--code-size 4096 --iram-size 128`：超過容量時 `make` 會失敗（實際試過 `--code-size 3000`，連結器回報 Insufficient ROM、exit 1） | 3585 | 0 | 7d8dea4 |

  - 第 1 步中，`readMPU6050_*Data` 的 `int *buffer` 指標參數是 `_gptrput` 的來源，刪掉之後它也跟著消失
  - 「`font_index*5` 會連結 `_mulint`」這點查證後**不成立**：SDCC 已經自動用硬體 `mul ab`，沒有連結 `_mulint`。仍然改成 shift 寫法，因為字型表縮小後索引可以用 8 位元
  - 現在連結的函式庫只剩 SDCC 啟動碼（crt*，93 bytes），沒有任何算術或指標函式庫
  - `PROJECT_CONTEXT.md` 第 7 節新增 **Flash 預算**：驅動合計 2030 bytes，遊戲程式可用 2066 bytes，依里程碑分配並保留 236 bytes 餘裕；也列出避免連結函式庫的寫法
  - 為了能逐步量測，在 Mac 上用 Homebrew 安裝了 **SDCC 4.6.0**（燒錄仍在學校）
- **改了哪些檔案**：`src/mpu6050.c`、`src/max7219.c`、`src/sys.c`、`src/oled_i2c.c`、`inc/mpu6050.h`、`inc/max7219.h`、`inc/sys.h`、`inc/oled_i2c.h`、`inc/font.h`、`inc/board.h`（只改註解）、`makefile`、`docs/PROJECT_CONTEXT.md`（第 5、6、7、8 節）、`docs/SCHOOL_TEST.md`、`docs/HANDOFF.md`
- **編譯結果**：成功、0 警告；**ROM 3585 / 4096 bytes**；Stack starts at **0x51**（堆疊剩 47 bytes）
- **實機測試**：未測。**所有修改都只編譯過**，下列行為需要實機確認：
  - OLED 初始化（改成查表）、MPU6050 初始化（改成查表）
  - OLED 字型：`TILT MAZE`、`M1 HW TEST`、`AX:`、`AY:`、`POT:`、`IR:`、`HOLD FLAT...` 都要正常顯示
  - 七段的數字（改成減法取位數）：右 4 位秒數、左 4 位旋鈕值
- **行為上的小改變**：
  - OLED 不再自動把小寫轉大寫，也不再忽略 `\n`。不在 `FONT_CHARS` 裡的字（含小寫）都顯示成空白
  - `uart_puts`、`OLED_DisplayString` 只能傳字串常數，不能傳 RAM 陣列（M7 從 UART 收到的資料若要顯示，要另外寫逐字輸出）
- **遇到的問題**：無
- **需要使用者或聊天端 Claude 決定的事**：
  - 遊戲只剩約 2 KB，M2 起每個里程碑請依照第 7 節的預算規劃；超過時可以先做預算表最後列出的再縮減項目
  - `uart_put_uint` 已刪除，M6 要再加回來（已計入預算）
  - 新增 OLED 文字時記得在 `font.h` 補字（每字 5 bytes）
- **下一步**：使用者燒錄 M1，照 `docs/SCHOOL_TEST.md` 第 2 步測試
- **commit**：d30483f、e53daf6、114c63d、326499c、0d9c72b、7d8dea4；文件更新見本次 commit

---

### 2026-11-20 22:15　[M0] 新增學校測試單 SCHOOL_TEST.md
- **做了什麼**：
  - 使用者說明：**編譯、燒錄、實機測試全部在學校電腦上做**，Mac 不裝 SDCC。所以 Claude Code 之後**無法自己編譯**，每次改程式都只能「靜態檢查」，`make` / `make size` 的結果要靠使用者從學校回報
  - 新增 `docs/SCHOOL_TEST.md`：第 0 步編譯並記錄 Stack starts at、程式大小；第 1 步硬體待確認事項（晶片、晶振、七段位數、蜂鳴器……附判斷方法）；第 2 步 M1 測試 15 項，每項附預期結果
  - 核對 `main.c` 後修正 HANDOFF 測試清單的一處描述：`buzzer_beep(half_period, ms)` 第一個參數是**音高**（1 = 2 kHz、2 = 1 kHz、4 = 500 Hz），不是響幾聲。所以開機是響一聲中音，暫停是一聲低音，INT1 是一聲高音
  - 補充說明：如果七段只有 4 位，左 4 位的 POT 讀值不會顯示
  - `PROJECT_CONTEXT.md` 第 5 節加註開發環境；第 8 節更新 M0 狀態；README 加上測試單連結
- **改了哪些檔案**：`docs/SCHOOL_TEST.md`（新）、`docs/PROJECT_CONTEXT.md`、`docs/HANDOFF.md`、`README.md`
- **編譯結果**：未編譯（本機沒有 SDCC；程式碼沒有改動）
- **實機測試**：未測，等使用者到學校
- **遇到的問題**：CLAUDE.md 規定收工前 `make` 要成功，但本機無法編譯。**建議聊天端 Claude 考慮**：要不要調整規則，或請使用者在 Mac 安裝 SDCC 只用來檢查編譯和 RAM（燒錄仍在學校做），這樣 M2 之後改程式比較不會帶著編譯錯誤去學校
- **需要使用者或聊天端 Claude 決定的事**：同上；以及等學校回報第 1 步（晶片型號最重要）
- **下一步**：使用者在學校填完 `SCHOOL_TEST.md` → Claude Code 依結果改 `board.h`、修 M1
- **commit**：見本次 commit

---

### 2026-11-20 22:00　[M0] git 初始化、建立 GitHub repo
- **做了什麼**：
  - 讀完 CLAUDE.md 與 docs/，向使用者確認進度
  - `git init`（分支 `main`），第一次 commit `[M0] 專案骨架`，內容就是聊天端 Claude 交付的版本，沒有修改
  - 用 `gh repo create` 建立 **public** repo：https://github.com/shiropai/tilt-maze-8051 ，並 push
- **改了哪些檔案**：只有 `docs/DEVLOG.md`、`docs/HANDOFF.md`
- **編譯結果**：**未編譯**。使用者的 Mac 還沒有 SDCC（已請使用者執行 `brew install sdcc`）
- **實機測試**：未測
- **遇到的問題**：無
- **需要使用者或聊天端 Claude 決定的事**：
  - repo 名稱使用者沒有指定，暫用資料夾名稱 `tilt-maze-8051`
  - `PROJECT_CONTEXT.md` 第 5 節的待確認事項都已經問過使用者，但**還沒有回覆**：晶片型號（最重要，4 KB flash 會放不下）、晶振、點矩陣 CS 與左右順序、七段位數、蜂鳴器有源或無源、按鍵腳位、燒錄方式。`board.h` 維持原本的假設值
- **下一步**：等 SDCC 裝好後執行 `make` 和 `make size` 確認可以編譯 → 收到使用者回覆後更新 `board.h` → M1 實機測試
- **commit**：8f0ce03（骨架），本筆日誌見下一個 commit

---

### 2026-11-20 21:40　[M0] 專案骨架建立（由聊天端 Claude 建立）
- **做了什麼**：
  - 從課堂實驗整理出驅動程式：I2C、MPU6050、OLED（擴充 A~Z 字型、改成 burst 寫入）、MAX7219（點矩陣 ×2 + 七段，共用匯流排）、MCP3202 ADC、組語 delay
  - 新增 `sys.c`：Timer0 250 us 時基、LED 軟體 PWM、蜂鳴器音調、INT0/INT1 去彈跳、UART 收發
  - 寫好 M1 硬體整合測試版 `main.c`（球隨傾斜滾動，所有硬體同時運作）
  - 所有腳位集中在 `inc/board.h`
- **改了哪些檔案**：全部新建
- **編譯結果**：SDCC 4.2.0 編譯成功、無警告；程式碼 4653 bytes；Stack starts at 0x52（128 bytes RAM 時堆疊剩 46 bytes）
- **實機測試**：未測
- **遇到的問題**：第一版 RAM 超出 37 bytes。解法：拿掉指標參數、陣列改索引、MPU6050 讀取不用暫存陣列、UART RX 緩衝改 8 bytes
- **需要使用者決定的事**：見 `PROJECT_CONTEXT.md` 第 5 節（晶片型號、晶振、點矩陣 CS 腳位……）
- **下一步**：Claude Code 接手 `HANDOFF.md` 的第一批待辦
- **commit**：尚未 commit（資料夾交給 Claude Code 後由它 git init 並做第一次 commit）
