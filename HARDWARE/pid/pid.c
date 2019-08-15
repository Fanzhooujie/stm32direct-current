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
  float SetSpeed; //�����趨ֵ
  float OUTPWM; //���PWM����ֵ
  float err; //����ƫ��ֵ
  float err_next; //������һ��ƫ��ֵ
  float err_last; //��������ǰ��ƫ��ֵ
  float Kp,Ki,Kd;
 }pid; //������������֡�΢��ϵ��

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

float PID_realize(float Nowspeed){              //Ŀǰ �ٶ�/�Ƕ� �Ĳ���pid
	float INPWM=0.0;                             //PWMƫ��ֵ
	pid.Kp=KP;
  pid.Ki=KI;
  pid.Kd=KD;
	if(MODE==1)         //�ٶ�PIDģʽ
	pid.SetSpeed=SV;
	else if(MODE==0)    //�Ƕ�PIDģʽ
	pid.SetSpeed=GL;

  pid.err=pid.SetSpeed-Nowspeed;  //ʵ��ƫ��ֵ
  INPWM=pid.Kp*(pid.err-pid.err_next)+pid.Ki*pid.err+pid.Kd*(pid.err-2*pid.err_next+pid.err_last);
  pid.OUTPWM+=INPWM;
	if(pid.OUTPWM>1000)
		pid.OUTPWM=1000;
	else if(pid.OUTPWM<0)
		pid.OUTPWM=0;
	pid.err_last=pid.err_next;
  pid.err_next=pid.err;
  return pid.OUTPWM;    //�������PWM����ֵ
}






