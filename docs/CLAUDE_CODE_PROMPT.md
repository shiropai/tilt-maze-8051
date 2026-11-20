# 給 Claude Code 的提示詞

在這個資料夾裡打開終端機、執行 `claude`，然後貼上下面的提示詞。

---

## 第一次開工（貼這段）

```
你好，這是我的微處理機期末專題：用 8051 做「體感平衡迷宮」，傾斜板子讓點矩陣上的球走迷宮。
評分看創意性和硬體使用的完整性，所以每種硬體都要用得有意義。

這個資料夾是另一個 Claude（claude.ai 上的聊天端）幫我建的，它負責規劃和給建議，你負責寫程式、編譯、管理 git 並 push 到 GitHub。你們透過 docs/ 裡的檔案溝通：
- CLAUDE.md：你的工作規則（每次都會自動讀）
- docs/PROJECT_CONTEXT.md：專題規格、腳位、硬體限制、里程碑
- docs/HANDOFF.md：聊天端 Claude 留給你的待辦，優先處理
- docs/DEVLOG.md：你的開發日誌，每次做完都要在最上面新增一筆，聊天端 Claude 會讀它來給我建議

請先做這些事，一步一步來：
1. 讀完 CLAUDE.md 和 docs/ 裡的四個檔案，用幾句話告訴我你理解的專題內容和目前進度
2. 確認我的電腦能用 make 編譯（如果沒有 SDCC，告訴我怎麼安裝）
3. 問我 PROJECT_CONTEXT.md 第 5 節的「待確認事項」，一次問完，我回答後更新 inc/board.h 和文件
4. 問我 GitHub repo 要叫什麼名字、public 還是 private，然後建立 repo 並 push 第一版
5. 給我 M1 硬體測試的步驟，我燒錄測試後會把結果告訴你

每完成一個工作單元，都要更新 DEVLOG、勾選 HANDOFF、commit 並 push。
你沒辦法自己在實機上測試，所以改完程式要告訴我怎麼測。請用繁體中文和我溝通。
```

---

## 之後每次開工（貼這段）

```
繼續體感平衡迷宮專題。照 CLAUDE.md 的「每次開工」流程：讀 PROJECT_CONTEXT、HANDOFF、DEVLOG 最後幾筆，
告訴我目前進度和你打算做什麼，HANDOFF 裡有新的待辦就優先處理。做完記得寫 DEVLOG、commit、push。
```

---

## 把實機測試結果告訴 Claude Code（範例）

```
M1 測試結果：
1. OLED 正常
2. UART 有收到 READY
3. 球會動，但往右傾時往左跑
4. 點矩陣左右顛倒
5. 七段只有 4 位
請修正並記進 DEVLOG。
```
