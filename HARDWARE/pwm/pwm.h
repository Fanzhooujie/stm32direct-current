#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM5_Cap_Init(u16 arr,u16 psc);
u32 TIM5CH1_CAPTURE_HIGHTIME(void);
#endif
