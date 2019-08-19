#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "iic.h"
#include "24C02.h"
#include "stdlib.h"

 extern u8 lcd_X[80];
 extern float KP;
 extern float KI;
 extern float KD;
 extern float SV;
 extern float GL;
 u8 datatemp[200];
 u8 MODE=1;       //显示模式
/*******************************************************************/
//按键初始化函数
/*******************************************************************/
void KEY_Init(void) //IO初始化
{ 
 	  GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //使能F0~3端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);   
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //使能F4~7端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);

}
/*******************************************************************/
//按键扫描函数
/*******************************************************************/
u8 Key_Scan(void)
{
	  extern u8 MODE;
    u8 keyValue=0;//按键

    
    GPIO_Write(GPIOF,GPIOF->ODR | 0x000f);//让PF0~3输出高电平
    
        if((GPIOF->IDR & 0x00f0)==0x0000)//若PF4~7全为0，则没有按键按下
        return 0;
    else
    {
        delay_ms(5);//延时消抖
        
        if((GPIOF->IDR & 0x00f0)==0x0000)//若PF4~7全为0，则刚刚是抖动产生
        return 0;  
    
    }
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0001);//让PA8-11输出0001，检测第四行
      switch(GPIOF->IDR & 0x00f0)
        {                               //改变数值并写入
            case 0x0010 : SV+=1;sprintf((char*)lcd_X,"SV:%7.3f",SV);AT24CXX_Write(0,(u8*)lcd_X,20);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=1;break;
            case 0x0020 : SV-=1;sprintf((char*)lcd_X,"SV:%7.3f",SV);AT24CXX_Write(0,(u8*)lcd_X,20);LCD_ShowString(225,80,200,16,16,lcd_X); keyValue=5;break;				
            case 0x0040 : SV+=10;sprintf((char*)lcd_X,"SV:%7.3f",SV);AT24CXX_Write(0,(u8*)lcd_X,20);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=9;break;
            case 0x0080 : SV-=10;sprintf((char*)lcd_X,"SV:%7.3f",SV);AT24CXX_Write(0,(u8*)lcd_X,20);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=13;break;    
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);  //等待按键释放
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0002);//让PA8-11输出0010，检测第三行
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KD+=0.1;sprintf((char*)lcd_X,"KD:%7.3f",KD);AT24CXX_Write(21,(u8*)lcd_X,20); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=2;break;					                                                                                                  
            case 0x0020 : KD-=0.1;sprintf((char*)lcd_X,"KD:%7.3f",KD);AT24CXX_Write(21,(u8*)lcd_X,20); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=6;break;
            case 0x0040 : GL+=10.0;sprintf((char*)lcd_X,"GL:%7.3f",GL);AT24CXX_Write(81,(u8*)lcd_X,20); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=10;break;
            case 0x0080 : GL-=10.0;sprintf((char*)lcd_X,"GL:%7.3f",GL);AT24CXX_Write(81,(u8*)lcd_X,20); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=14;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0004);//让PA8-11输出0100，检测第二行
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KI+=0.1;sprintf((char*)lcd_X,"KI:%7.3f",KI);AT24CXX_Write(41,(u8*)lcd_X,20);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=3;break;
            case 0x0020 : KI-=0.1;sprintf((char*)lcd_X,"KP:%7.3f",KP);AT24CXX_Write(41,(u8*)lcd_X,20);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=7;break;
            case 0x0040 : LCD_Fill(0,0,340,240,WHITE);MODE=0;keyValue=11;break;
            case 0x0080 : GL-=1.0;sprintf((char*)lcd_X,"GL:%7.3f",GL);AT24CXX_Write(81,(u8*)lcd_X,20); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=15;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0008);//让PA8-11输出1000，检测第一行
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KP+=0.1;sprintf((char*)lcd_X,"KP:%7.3f",KP);AT24CXX_Write(61,(u8*)lcd_X,20);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=4;break;
            case 0x0020 : KP-=0.1;sprintf((char*)lcd_X,"KP:%7.3f",KP);AT24CXX_Write(61,(u8*)lcd_X,20);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=8;break;
            case 0x0040 : LCD_Fill(0,0,340,240,WHITE);MODE=1;keyValue=12;break;
            case 0x0080 : LCD_Fill(210,20,340,120,WHITE); 			
	                        AT24CXX_Read(0,datatemp,40); SV=Translatdata();LCD_ShowString(225,80,200,16,16,datatemp);     //读取改变量
		                      AT24CXX_Read(21,datatemp,40);KD=Translatdata();LCD_ShowString(225,60,200,16,16,datatemp);
					                AT24CXX_Read(41,datatemp,40);KI=Translatdata();LCD_ShowString(225,40,200,16,16,datatemp);
					                AT24CXX_Read(61,datatemp,40);KP=Translatdata();LCD_ShowString(225,20,200,16,16,datatemp);
					                AT24CXX_Read(81,datatemp,40);GL=Translatdata();LCD_ShowString(225,100,200,16,16,datatemp);
 
					
					  keyValue=16;break; 

        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
				if(SV<=0) SV=0;
        else if(SV>=200) SV=200;
				if(GL<=0) GL=0.0;
    
    return keyValue;

}
/*******************************************************************/
//读取字符串中所保存的数组并转化为数值
/********************************************************************/
float Translatdata(void)
{ 
	  float x;
	  u8 *p=&datatemp[3];
	  x=atof(p);
	  LED1=!LED1;	
	return x;
}








