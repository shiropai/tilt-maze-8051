# HANDOFF：聊天端 Claude → Claude Code

> 這裡是聊天端 Claude（claude.ai「微處理機」Project）留給 Claude Code 的待辦與建議。
> **Claude Code**：每次開工先讀這份，完成的項目打勾 `[x]` 並在後面註明 commit（例如 `→ a1b2c3d`）。
> 不要刪除項目；不做的項目改成 `[-]` 並寫原因。
> **聊天端 Claude**：新的指示加在最上面，標日期。

---

## 2026-11-20　第一批：專案啟動（M0 → M1）

### 先做（M0）
- [-] 確認 `make` 在使用者的電腦上能編譯（macOS 需要 `brew install sdcc`）→ 使用者決定編譯／燒錄都在學校電腦做，Mac 不裝 SDCC；改由使用者在學校跑 `make`，結果填 `docs/SCHOOL_TEST.md` 第 0 步
- [x] `git init`、做第一次 commit（訊息：`[M0] 專案骨架`）→ 8f0ce03
- [x] 建立 GitHub repo 並 push 第一版（先問使用者 repo 名稱與 public / private）→ 8f0ce03，public：https://github.com/shiropai/tilt-maze-8051
- [ ] 問使用者 `PROJECT_CONTEXT.md` 第 5 節的「待確認事項」，答案更新到 `inc/board.h` 和 `PROJECT_CONTEXT.md`（已問；整理成 `docs/SCHOOL_TEST.md` 第 1 步，等學校回報）
  - 最重要的是**晶片型號**：目前程式 4.6 KB，如果是 4 KB flash 的 AT89S51 就要大幅縮減
- [ ] 問使用者燒錄器怎麼用，若可以用指令燒錄就加 `make flash`

### 再做（M1 實機驗證）
- [ ] 請使用者燒錄目前的 M1 測試版，照下面清單逐項測試，結果記進 DEVLOG：（清單已整理成 `docs/SCHOOL_TEST.md` 第 2 步；注意 `buzzer_beep` 第一個參數是音高不是聲數，第 1 項應為「響一聲」）
  1. 開機 OLED 顯示 `TILT MAZE` / `M1 HW TEST`，蜂鳴器響一聲
  2. 電腦開序列埠（9600 baud）收到 `TILT MAZE M1 READY`
  3. 傾斜板子，點矩陣上的點會滾動；碰到邊界有短促聲音
  4. 向右傾時點往右、向前傾時點往下？（不對就改 `TILT_X_SIGN` / `TILT_Y_SIGN`）
  5. 兩片點矩陣左右順序對嗎？（不對就改 `MATRIX_NEAR_IS_LEFT`）
  6. OLED 的 AX、AY 隨傾斜改變；POT 隨旋鈕改變；IR 隨手靠近改變
  7. 七段顯示器：右 4 位每秒加 1，左 4 位是旋鈕讀值
  8. 按 INT0（P3_2）暫停，LED 從呼吸燈變全亮；再按一次繼續
  9. 按 INT1（P3_3）點回到中央；電腦送 `r` 也會回中央
- [ ] 依測試結果修正，直到 M1 全部通過

### 給 Claude Code 的提醒
- 目前 `src/main.c` 只是硬體測試，正式遊戲（M2 之後）建議把遊戲邏輯拆到 `src/game.c`、迷宮資料放 `src/levels.c`
- 速度優化的方向（M2 之後如果覺得卡再做）：
  - `mpu6050.c` 改成 burst read（一次 I2C 交易讀連續暫存器）
  - `i2c.c` 的 `Delay()` 可以試著縮短，但要實機確認 OLED 和 MPU6050 都還正常
- 如果使用者的晶片其實是 8052（256 bytes RAM），大陣列可以加 `__idata` 放到上半部 RAM，堆疊會比較寬鬆
