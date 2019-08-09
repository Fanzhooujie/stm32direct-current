#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"




#define KEYOUT1  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)
#define KEYOUT2  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)
#define KEYOUT3  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)
#define KEYOUT4  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)
#define KEYIN1  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_4)
#define KEYIN2  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_5)
#define KEYIN3  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)
#define KEYIN4  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7)



float Translatdata(void);
void KEY_Init(void);//IO初始化
u8 Key_Scan(void);  	//按键扫描函数	
#endif

