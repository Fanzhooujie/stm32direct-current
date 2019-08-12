#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
#include "pid.h"
#include "lcd.h"
#include "iic.h"
#include "24C02.h"

extern u8 sum;
extern u8 MODE;
extern float GL;
 int main(void)
 {
  float changeDat=0.0; //���PWM����ֵ
  float	Rad=0.0;	 	    //���ת��
 	u32 temp=0;
  float number=0.0;	 
	delay_init();	    	 //��ʱ������ʼ��		 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	PID_init();          //PID��ʼ��
  LCD_Init();
	POINT_COLOR=BLACK;     //������ɫΪ��ɫ������Ϊ��ɫ 
	BACK_COLOR=WHITE;	 //LCD��ʼ��    
  KEY_Init();        //������ʼ��
	AT24CXX_Init();    //IIC��ʼ��
 	TIM3_PWM_Init(1000,72-1);	 //����Ƶ��PWMƵ��=72000000/900=80Khz	 
 	TIM5_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ���	 
	LCD_dateInit();//lcd��ֵ��ʼ����ʾ
	LED1=0;
   	while(1)
	{
    Key_Scan();
		temp=TIM5CH1_CAPTURE_HIGHTIME();
		Rad=1000000/temp/668.0;
		if(MODE==1)                            //�ٶȽ�����ʾ
	  {changeDat=PID_realize(Rad);  	       //ռ�ձ�����ֵ
		TIM_SetCompare2(TIM3,changeDat);	
		LCD_Coor(Rad,changeDat);
		Rad=0.00;
		}
		else if(MODE==0)                       //�ǶȽ�����ʾ
		{	 number=GL/360*688/2.0;		
		   LCD_Angle();
			 if(sum<number)
				 TIM_SetCompare2(TIM3,500);
			 else if(sum>=number)
				 TIM_SetCompare2(TIM3,0);
		}
	}
}

