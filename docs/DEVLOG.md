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
