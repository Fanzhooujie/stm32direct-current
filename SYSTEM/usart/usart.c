#include "sys.h"
#include "usart.h"	  
#include "lcd.h"
#include "iic.h"
#include "24C02.h"
#include "led.h"
#include "delay.h"
#include "string.h"


#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}
u8 len;
extern unsigned char lcd_X[40];
extern u8 datatemp[200];
extern	float KP;
extern float KI;
extern float KD;
extern float SV;
extern float GL;

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else{ USART_RX_STA|=0x8000;	//接收完成了
        					
				if(USART_RX_STA&0x8000)
				{
					len=USART_RX_STA&0x3fff;//此次接收到的长度
					USART_Change(USART_RX_BUF);//引用函数
					USART_RX_STA=0;//标记为位清零
							LED1!=LED1;
				}
			  }
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	

void USART_Change(u8 *str)
{
  float X,Y;
	u8 a=0,b=0,c=0,d=0,e=0;
	while(len--)
	{ 
		a++;  //小数点后面位数a
		if(USART_RX_BUF[len-1]=='.')
	  {
     if     (a==1){X=(USART_RX_BUF[len]-'0')*0.1;}//小数点后为一位时的数值
		 else if(a==2){X=(USART_RX_BUF[len]-'0')*0.1+(USART_RX_BUF[len+1]-'0')*0.01;}//小数点后为两位时的数值
		 len=USART_RX_STA&0x3fff;//此次接收到的长度
		 break;}
	}
	while(len--)
	{
		b++; //等号后面的位数b
		if(USART_RX_BUF[len-1]=='=')		
		{	
			c=USART_RX_BUF[len-2];//数组的第二个字符
			d=USART_RX_BUF[len-3];//数组的第一个字符 
			e=b-a;//等号与小数点之间的位数	
			if(e==2)  {Y=USART_RX_BUF[len]-'0';}  //只有各位时
			else if(e==3) {(Y=USART_RX_BUF[len]-'0')*10+USART_RX_BUF[len+1]-'0';} //有两位数
			else if(e==4) {(Y=USART_RX_BUF[len]-'0')*100+(Y=USART_RX_BUF[len+1]-'0')*10+USART_RX_BUF[len+2]-'0';}	//有三位数		
			len=USART_RX_STA&0x3fff;//此次接收到的长度
			break;				
		}
	}
	printf("      %s",USART_RX_BUF);
	printf("      %c",c);
	printf("      %c",d);
	
	
		 if(d=='K'&&c=='P')
		 {
			 KP=X+Y;sprintf((char*)lcd_X,"KP:%0.2f",KP);AT24CXX_Write(61,(u8*)lcd_X,10);LCD_ShowString(225,20,200,16,16,lcd_X);
		 }
		 if(d=='K'&&c=='I')
		 {
			 KI=X+Y;sprintf((char*)lcd_X,"KI:%0.2f",KI);AT24CXX_Write(41,(u8*)lcd_X,10);LCD_ShowString(225,40,200,16,16,lcd_X);
		 }
		 if(d=='K'&&c=='D')
		 {
			 KD=X+Y;sprintf((char*)lcd_X,"KD:%0.2f",KD);AT24CXX_Write(21,(u8*)lcd_X,10); LCD_ShowString(225,60,200,16,16,lcd_X);
		 }
		 if(d=='S'&&c=='V')
		 {
			 SV=X+Y;sprintf((char*)lcd_X,"SV:%0.1f",SV);AT24CXX_Write(0,(u8*)lcd_X,10);LCD_ShowString(225,80,200,16,16,lcd_X);
		 }
		 if(d=='G'&&c=='L')
		 {
			 GL=X+Y;sprintf((char*)lcd_X,"GL:%0.2f",GL);AT24CXX_Write(81,(u8*)lcd_X,10); LCD_ShowString(225,100,200,16,16,lcd_X);
		 }
		 delay_ms(1000);
		 memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));//接收数组清零
		 len=0;//长度清零	
}




