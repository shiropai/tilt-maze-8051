# CLAUDE.md — 給 Claude Code 的工作說明

這是一個 **8051 微處理機期末專題：體感平衡迷宮（Tilt Maze）**，用 SDCC 編譯，C 語言為主。
使用者是電機系大二學生，請用**繁體中文**溝通；程式碼註解也用繁體中文。

## 協作方式（重要）

這個專案有兩個 Claude 一起工作，透過這個資料夾裡的檔案溝通：

| 角色 | 負責 | 寫入的檔案 |
|---|---|---|
| **聊天端 Claude**（claude.ai「微處理機」Project） | 規劃、規格、給建議、看日誌找問題 | `docs/PROJECT_CONTEXT.md`、`docs/HANDOFF.md` |
| **你（Claude Code）** | 寫程式、編譯、維護 git、push 到 GitHub | 程式碼、`docs/DEVLOG.md`，並勾選 `docs/HANDOFF.md` 的項目 |

### 每次開工
1. 讀 `docs/PROJECT_CONTEXT.md`（規格、腳位、硬體限制、里程碑）
2. 讀 `docs/HANDOFF.md`（聊天端 Claude 留給你的待辦與建議，**優先處理**）
3. 讀 `docs/DEVLOG.md` 最後 2~3 筆（上次做到哪裡）
4. 跑 `git status`、`git log --oneline -5` 確認狀態

### 每次收工（或完成一個工作單元）
1. `make` 必須成功、沒有新的警告；跑 `make size` 確認堆疊起點不超過 `0x60`
2. 在 `docs/DEVLOG.md` **最上面**新增一筆日誌（格式見該檔案）。**不要刪除或改寫舊日誌**
3. 在 `docs/HANDOFF.md` 把完成的項目打勾，並註明是哪一筆 commit
4. 如果里程碑狀態改變，更新 `docs/PROJECT_CONTEXT.md` 第 8 節的表格
5. `git commit`（訊息格式：`[M2] 加入牆壁碰撞`），然後 `git push`

日誌要寫得讓聊天端 Claude **不看程式碼也能看懂**：做了什麼、為什麼、測試結果、遇到的問題、需要使用者決定的事。

## 建置

```bash
brew install sdcc     # macOS 第一次使用時安裝（Windows/Linux 自行安裝 SDCC）
make                  # 產生 ./main.hex
make size             # 看 RAM 使用量：看「Stack starts at」
make clean
```

燒錄：使用者用課堂的燒錄器把 `main.hex` 燒進晶片。燒錄指令還不知道，問使用者之後可以加一個 `make flash`。

## 你無法自己測試硬體

你只能編譯，**不能**在實機上跑。所以：
- 每完成一個功能，給使用者一份**簡短的實機測試清單**（例如：「傾斜板子向右，球應該往右滾」）
- 使用者回報的結果記進 DEVLOG 的「實機測試」欄位
- 標明哪些程式「只編譯過、沒實測」

## 寫程式的規則（8051 資源非常少）

1. **腳位只能定義在 `inc/board.h`**，其他檔案不准寫死 `P2_1` 這類腳位
2. **RAM 只有 128 bytes**（含堆疊）：
   - 常數表（迷宮、音階、字型）一律 `__code`
   - 變數用最小的型別（`uint8_t` / `int8_t`），**不用** `float`、`long`、`printf`、`sprintf`
   - **不要用指標當函式參數**（SDCC 通用指標要 3 bytes），改用全域陣列 + 索引
   - 每次改完跑 `make size`
3. **I2C 很慢**（軟體 bit-bang）：遊戲迴圈中只讀 MPU6050 的 X、Y 高位元組；OLED 每個 frame 只更新一小塊
4. **Timer1 被 UART 佔用**，所有計時都用 `sys_ms()`（Timer0 的 1 ms 計數），不要再開新的 timer
5. 中斷函式原型在 `inc/sys.h`，**main.c 一定要 include 它**（SDCC 規定）
6. 中斷服務程式要短；不要在 ISR 裡呼叫 I2C、OLED、`delay_ms`
7. 遊戲迴圈中不要用 `delay_ms()`（會卡住），用 `sys_ms()` 的差值計時
8. 課堂的驅動程式（`i2c.c`、`delay.c` 的時序）是在實機上驗證過的，**沒有必要不要改時序**
9. 每個檔案開頭寫一段說明：這個模組做什麼、沿用自哪個課堂實驗、改了什麼

## Git 與 GitHub

- 分支：`main`。小步提交，每個 commit 只做一件事
- 第一次 push 前如果還沒有遠端 repo：先問使用者要 public 還是 private、repo 名稱，再用 `gh repo create` 建立
- 不要 commit `obj/` 和 `main.hex`（已在 `.gitignore`）
- commit 訊息用繁體中文，開頭標里程碑，例如 `[M3] 加入暫停狀態`

## 不要做的事

- 不要刪除或改寫 `docs/DEVLOG.md` 的舊紀錄
- 不要大幅改寫 `docs/PROJECT_CONTEXT.md` 的規格；規格需要改時，先問使用者，並在 DEVLOG 記錄原因
- 不要為了「更好看」重構整個專案；這是要在期末前完成的作業，以能 demo 為優先
