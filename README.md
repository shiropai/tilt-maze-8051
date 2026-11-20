# 體感平衡迷宮 Tilt Maze — 8051 期末專題

把整塊板子當成托盤，傾斜它來控制 16 x 8 點矩陣上的小球走迷宮。
使用 MPU6050、MAX7219 點矩陣與七段顯示器、OLED、UART、外部中斷、軟體 PWM、蜂鳴器、IR 感測器與可變電阻（MCP3202 ADC）。

## 資料夾結構

```
tilt-maze-8051/
├── CLAUDE.md                 Claude Code 的工作規則（Claude Code 會自動讀）
├── README.md                 本檔案
├── makefile                  SDCC 編譯
├── docs/
│   ├── PROJECT_CONTEXT.md    專題規格、腳位、硬體限制、里程碑
│   ├── HANDOFF.md            聊天端 Claude → Claude Code 的待辦
│   ├── DEVLOG.md             Claude Code 的開發日誌（聊天端 Claude 讀）
│   ├── SCHOOL_TEST.md        帶去學校的編譯／硬體測試單
│   └── CLAUDE_CODE_PROMPT.md 貼給 Claude Code 的提示詞
├── inc/                      標頭檔（board.h = 所有腳位）
├── src/                      程式碼
└── tools/                    電腦端工具（例如 UART 排行榜，之後加入）
```

## 編譯與燒錄

```bash
make          # 產生 main.hex
make size     # 查看 RAM 使用量
make clean
```
用課堂的燒錄器把 `main.hex` 燒進 8051。編譯與燒錄都在學校電腦上進行，步驟見 `docs/SCHOOL_TEST.md`。

## 兩個 Claude 的協作流程

```
 ┌──────────────────────────┐   PROJECT_CONTEXT.md / HANDOFF.md   ┌──────────────────────┐
 │ 聊天端 Claude（claude.ai）│ ──────────────────────────────────▶ │     Claude Code      │
 │   規劃、規格、給建議      │                                      │ 寫程式、編譯、git push│
 │                          │ ◀────────────────────────────────── │                      │
 └──────────────────────────┘          DEVLOG.md                  └──────────────────────┘
               ▲                                                             │
               └──────────── 讀取方式：電腦資料夾連線 或 GitHub repo ◀───────┘
```

1. 在這個資料夾開 Claude Code，貼上 `docs/CLAUDE_CODE_PROMPT.md` 的提示詞
2. Claude Code 寫程式 → 更新 `DEVLOG.md` → commit → push
3. 回到 claude.ai 的「微處理機」Project，請聊天端 Claude 讀最新的 DEVLOG 給建議
4. 聊天端 Claude 把新的待辦寫進 `HANDOFF.md`，Claude Code 下次開工時處理
