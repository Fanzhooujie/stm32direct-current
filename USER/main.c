#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "input.h"
#include "pwm.h"
#include "pid.h"
#include "lcd.h"
#include "iic.h"
#include "24C02.h"
#include "my-printf.h"


extern u8 sum;
extern u8 MODE;
extern float GL;
 int main(void)
 {
  float changeDat=0.0; //输出PWM重载值
  float	Rad=0.0;	 	    //电机转速
 	u32 temp=0;
  float number=0.0;
	float angle;	 
	delay_init();	    	 //延时函数初始化		 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	PID_init();          //PID初始化
  LCD_Init();
	POINT_COLOR=BLACK;     //画笔颜色为黑色，背景为白色 
	BACK_COLOR=WHITE;	 //LCD初始化    
  KEY_Init();        //按键初始化
	AT24CXX_Init();    //IIC初始化
 	TIM3_PWM_Init(1000,72-1);	 //不分频。PWM频率=72000000/900=80Khz	 
 	TIM5_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数	 
	LCD_dateInit();//lcd数值初始化显示
	LED1=0;
   	while(1)
	{
    Key_Scan();
		temp=TIM5CH1_CAPTURE_HIGHTIME();//一周期脉冲高电平时间
		Rad=1000000/temp/668.0;         //目前转速
    if(sum>=334) sum=0; 
  	angle=sum*360/668*2.0;         //目前角度       
    if(angle>GL){angle=0;sum=0;}
	  else if(angle<=0) angle=0;
		if(MODE==1)                         //速度界面显示
	  {
			changeDat=PID_realize(Rad);  	       //占空比重载值
		  TIM_SetCompare2(TIM3,changeDat);	
		  LCD_Coor(Rad,changeDat);
		}
		else if(MODE==0)                    //角度界面显示
		{	number=GL/360*668/2.0;		
			if(sum<number)
				changeDat=PID_realize(angle);    //占空比重载值
			else if(sum>=number)
				TIM_SetCompare2(TIM3,0);       //pid控制占空比
			  LCD_Angle(angle,changeDat);		
		}
	}
}

