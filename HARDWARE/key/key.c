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

 extern u8 lcd_X[40];
 extern float KP;
 extern float KI;
 extern float KD;
 extern float V;
 extern float GL;
 u8 datatemp[200];
 u8 MODE=1;       //ÏÔÊ¾Ä£Ê½
/*******************************************************************/
//°´¼ü³õÊ¼»¯º¯Êý
/*******************************************************************/
void KEY_Init(void) //IO³õÊ¼»¯
{ 
 	  GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //Ê¹ÄÜF0~3¶Ë¿ÚÊ±ÖÓ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ËÙ¶È50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);   
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //Ê¹ÄÜF4~7¶Ë¿ÚÊ±ÖÓ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//ËÙ¶È50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);

}
/*******************************************************************/
//°´¼üÉ¨Ãèº¯Êý
/*******************************************************************/
u8 Key_Scan(void)
{
	  extern u8 MODE;
    u8 keyValue=0;//°´¼ü

    
    GPIO_Write(GPIOF,GPIOF->ODR | 0x000f);//ÈÃPF0~3Êä³ö¸ßµçÆ½
    
        if((GPIOF->IDR & 0x00f0)==0x0000)//ÈôPF4~7È«Îª0£¬ÔòÃ»ÓÐ°´¼ü°´ÏÂ
        return 0;
    else
    {
        delay_ms(5);//ÑÓÊ±Ïû¶¶
        
        if((GPIOF->IDR & 0x00f0)==0x0000)//ÈôPF4~7È«Îª0£¬Ôò¸Õ¸ÕÊÇ¶¶¶¯²úÉú
        return 0;  
    
    }
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0001);//ÈÃPA8-11Êä³ö0001£¬¼ì²âµÚËÄÐÐ
      switch(GPIOF->IDR & 0x00f0)
        {                               //¸Ä±äÊýÖµ²¢Ð´Èë
            case 0x0010 : V+=1;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=1;break;
            case 0x0020 : V-=1;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X); keyValue=5;break;				
            case 0x0040 : V+=10;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=9;break;
            case 0x0080 : V-=10;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=13;break;    
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);  //µÈ´ý°´¼üÊÍ·Å
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0002);//ÈÃPA8-11Êä³ö0010£¬¼ì²âµÚÈýÐÐ
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KD+=0.1;sprintf((char*)lcd_X,"KD:%0.2f",KD);AT24CXX_Write(21,(u8*)lcd_X,10); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=2;break;					                                                                                                  
            case 0x0020 : KD-=0.1;sprintf((char*)lcd_X,"KD:%0.2f",KD);AT24CXX_Write(21,(u8*)lcd_X,10); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=6;break;
            case 0x0040 : GL+=10.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=10;break;
            case 0x0080 : GL-=10.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=14;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0004);//ÈÃPA8-11Êä³ö0100£¬¼ì²âµÚ¶þÐÐ
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KI+=0.1;sprintf((char*)lcd_X,"KI:%0.2f",KI);AT24CXX_Write(41,(u8*)lcd_X,10);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=3;break;
            case 0x0020 : KI-=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(41,(u8*)lcd_X,10);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=7;break;
            case 0x0040 : LCD_Fill(0,0,340,240,WHITE);MODE=0;keyValue=11;break;
            case 0x0080 : GL-=1.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=15;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0008);//ÈÃPA8-11Êä³ö1000£¬¼ì²âµÚÒ»ÐÐ
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KP+=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(61,(u8*)lcd_X,10);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=4;break;
            case 0x0020 : KP-=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(61,(u8*)lcd_X,10);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=8;break;
            case 0x0040 :LCD_Fill(0,0,340,240,WHITE);MODE=1;keyValue=12;break;
            case 0x0080 : LCD_Fill(210,20,340,120,WHITE); 			
	                        AT24CXX_Read(0,datatemp,40);V=Translatdata();LCD_ShowString(225,80,200,16,16,datatemp);     //¶ÁÈ¡¸Ä±äÁ¿
		                      AT24CXX_Read(21,datatemp,40);KD=Translatdata();LCD_ShowString(225,60,200,16,16,datatemp);
					                AT24CXX_Read(41,datatemp,40);KI=Translatdata();LCD_ShowString(225,40,200,16,16,datatemp);
					                AT24CXX_Read(61,datatemp,40);KP=Translatdata();LCD_ShowString(225,20,200,16,16,datatemp);
					                AT24CXX_Read(81,datatemp,40);GL=Translatdata();LCD_ShowString(225,100,200,16,16,datatemp);
 
					
					  keyValue=16;break; 

        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
				if(V<=0) V=0;
        else if(V>=200) V=200;
				if(GL<=0) GL=0.0;
    
    return keyValue;

}
/*******************************************************************/
//¶ÁÈ¡×Ö·û´®ÖÐËù±£´æµÄÊý×é²¢×ª»¯ÎªÊýÖ
/********************************************************************/
float Translatdata(void)
{ 
	  float x;
	  char *p=&datatemp[3];
	  x=atof(p);
	  LED1=!LED1;	
	return x;
}








