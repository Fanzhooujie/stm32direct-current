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
 u8 MODE=1;       //��ʾģʽ
/*******************************************************************/
//������ʼ������
/*******************************************************************/
void KEY_Init(void) //IO��ʼ��
{ 
 	  GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //ʹ��F0~3�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);   
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);    //ʹ��F4~7�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
    GPIO_Init(GPIOF, &GPIO_InitStructure);

}
/*******************************************************************/
//����ɨ�躯��
/*******************************************************************/
u8 Key_Scan(void)
{
	  extern u8 MODE;
    u8 keyValue=0;//����

    
    GPIO_Write(GPIOF,GPIOF->ODR | 0x000f);//��PF0~3����ߵ�ƽ
    
        if((GPIOF->IDR & 0x00f0)==0x0000)//��PF4~7ȫΪ0����û�а�������
        return 0;
    else
    {
        delay_ms(5);//��ʱ����
        
        if((GPIOF->IDR & 0x00f0)==0x0000)//��PF4~7ȫΪ0����ո��Ƕ�������
        return 0;  
    
    }
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0001);//��PA8-11���0001����������
      switch(GPIOF->IDR & 0x00f0)
        {                               //�ı���ֵ��д��
            case 0x0010 : V+=1;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=1;break;
            case 0x0020 : V-=1;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X); keyValue=5;break;				
            case 0x0040 : V+=10;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=9;break;
            case 0x0080 : V-=10;sprintf((char*)lcd_X,"V :%0.1f",V);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);keyValue=13;break;    
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);  //�ȴ������ͷ�
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0002);//��PA8-11���0010����������
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KD+=0.1;sprintf((char*)lcd_X,"KD:%0.2f",KD);AT24CXX_Write(21,(u8*)lcd_X,10); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=2;break;					                                                                                                  
            case 0x0020 : KD-=0.1;sprintf((char*)lcd_X,"KD:%0.2f",KD);AT24CXX_Write(21,(u8*)lcd_X,10); LCD_ShowString(225,60,200,16,16,lcd_X);keyValue=6;break;
            case 0x0040 : GL+=10.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=10;break;
            case 0x0080 : GL-=10.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=14;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0004);//��PA8-11���0100�����ڶ���
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KI+=0.1;sprintf((char*)lcd_X,"KI:%0.2f",KI);AT24CXX_Write(41,(u8*)lcd_X,10);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=3;break;
            case 0x0020 : KI-=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(41,(u8*)lcd_X,10);LCD_ShowString(225,40,200,16,16,lcd_X); keyValue=7;break;
            case 0x0040 : LCD_Fill(0,0,340,240,WHITE);MODE=0;keyValue=11;break;
            case 0x0080 : GL-=1.0;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);keyValue=15;break; 
        }
				 while((GPIOF->IDR & 0x00f0)	> 0);
    GPIO_Write(GPIOF,(GPIOF->ODR & 0xfff0 )| 0x0008);//��PA8-11���1000������һ��
      switch(GPIOF->IDR & 0x00f0)
        {
            case 0x0010 : KP+=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(61,(u8*)lcd_X,10);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=4;break;
            case 0x0020 : KP-=0.1;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(61,(u8*)lcd_X,10);LCD_ShowString(225,20,200,16,16,lcd_X);keyValue=8;break;
            case 0x0040 :LCD_Fill(0,0,340,240,WHITE);MODE=1;keyValue=12;break;
            case 0x0080 : LCD_Fill(210,20,340,120,WHITE); 			
	                        AT24CXX_Read(0,datatemp,40);V=Translatdata();LCD_ShowString(225,80,200,16,16,datatemp);     //��ȡ�ı���
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
//��ȡ�ַ���������������鲢ת��Ϊ���
/********************************************************************/
float Translatdata(void)
{ 
	  float x;
	  char *p=&datatemp[3];
	  x=atof(p);
	  LED1=!LED1;	
	return x;
}








