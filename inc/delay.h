#ifndef DELAY_H
#define DELAY_H

/* 忙等延遲，校準於課堂實驗板。會擋住主迴圈，遊戲中盡量改用 sys_ms() 計時。 */
void delay_ms(unsigned int input_ms);

#endif
