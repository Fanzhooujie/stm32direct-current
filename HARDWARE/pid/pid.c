#include "pid.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "pwm.h"
#include "lcd.h"
#include "iic.h"
#include "24C02.h"

extern u8 MODE;
extern float GL;
float KP=0.10;
float KI=0.50;
float KD=0.60;
float SV=25.0;

unsigned char lcd_X[80];
unsigned char lcd_DATA[10];
struct _pid{
  float SetSpeed; //定义设定值
  float OUTPWM; //输出PWM重载值
  float err; //定义偏差值
  float err_next; //定义上一个偏差值
  float err_last; //定义最上前的偏差值
  float Kp,Ki,Kd;
 }pid; //定义比例、积分、微分系数

void PID_init(void){
  pid.SetSpeed=SV;
  pid.OUTPWM=0.0;
  pid.err=0.0;
  pid.err_last=0.0;
  pid.err_next=0.0;
  pid.Kp=KP;
  pid.Ki=KI;
  pid.Kd=KD;
}

float PID_realize(float Nowspeed){              //目前 速度/角度 的差速pid
	float INPWM=0.0;                             //PWM偏差值
	pid.Kp=KP;
  pid.Ki=KI;
  pid.Kd=KD;
	if(MODE==1)         //速度PID模式
	pid.SetSpeed=SV;
	else if(MODE==0)    //角度PID模式
	pid.SetSpeed=GL;

  pid.err=pid.SetSpeed-Nowspeed;  //实际偏差值
  INPWM=pid.Kp*(pid.err-pid.err_next)+pid.Ki*pid.err+pid.Kd*(pid.err-2*pid.err_next+pid.err_last);
  pid.OUTPWM+=INPWM;
	if(pid.OUTPWM>1000)
		pid.OUTPWM=1000;
	else if(pid.OUTPWM<0)
		pid.OUTPWM=0;
	pid.err_last=pid.err_next;
  pid.err_next=pid.err;
  return pid.OUTPWM;    //返回输出PWM重载值
}






