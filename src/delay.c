/* delay.c — 沿用課堂實驗的組語延遲（參數經 DPL/DPH 傳入） */
#include "delay.h"

void delay_ms(unsigned int input_ms)
{
    input_ms;               /* 避免 unreferenced 警告 */
    __asm
    DELAY1: MOV R4,dph      ; R4 = input_ms[15:8]
    DELAY3: INC R4          ; avoid dph == 0
    DELAY4: MOV R5,dpl      ; R5 = input_ms[7:0]
    DL1:    MOV R6,#3
    DL2:    MOV R7,#170
    DL3:    DJNZ R7,DL3
            DJNZ R6,DL2
            DJNZ R5,DL1
            DJNZ R4,DELAY4
    __endasm;
}
