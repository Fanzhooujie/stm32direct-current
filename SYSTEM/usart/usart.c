#include "sys.h"
#include "usart.h"	  
#include "lcd.h"
#include "iic.h"
#include "24C02.h"
#include "led.h"
#include "delay.h"
#include "string.h"


#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
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
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}
u8 len;
extern unsigned char lcd_X[40];
extern u8 datatemp[200];
extern	float KP;
extern float KI;
extern float KD;
extern float SV;
extern float GL;

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else{ USART_RX_STA|=0x8000;	//���������
        					
				if(USART_RX_STA&0x8000)
				{
					len=USART_RX_STA&0x3fff;//�˴ν��յ��ĳ���
					USART_Change(USART_RX_BUF);//���ú���
					USART_RX_STA=0;//���Ϊλ����
							LED1!=LED1;
				}
			  }
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
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
		a++;  //С�������λ��a
		if(USART_RX_BUF[len-1]=='.')
	  {
     if     (a==1){X=(USART_RX_BUF[len]-'0')*0.1;}//С�����Ϊһλʱ����ֵ
		 else if(a==2){X=(USART_RX_BUF[len]-'0')*0.1+(USART_RX_BUF[len+1]-'0')*0.01;}//С�����Ϊ��λʱ����ֵ
		 len=USART_RX_STA&0x3fff;//�˴ν��յ��ĳ���
		 break;}
	}
	while(len--)
	{
		b++; //�Ⱥź����λ��b
		if(USART_RX_BUF[len-1]=='=')		
		{	
			c=USART_RX_BUF[len-2];//����ĵڶ����ַ�
			d=USART_RX_BUF[len-3];//����ĵ�һ���ַ� 
			e=b-a;//�Ⱥ���С����֮���λ��	
			if(e==2)  {Y=USART_RX_BUF[len]-'0';}  //ֻ�и�λʱ
			else if(e==3) {(Y=USART_RX_BUF[len]-'0')*10+USART_RX_BUF[len+1]-'0';} //����λ��
			else if(e==4) {(Y=USART_RX_BUF[len]-'0')*100+(Y=USART_RX_BUF[len+1]-'0')*10+USART_RX_BUF[len+2]-'0';}	//����λ��		
			len=USART_RX_STA&0x3fff;//�˴ν��յ��ĳ���
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
		 memset(USART_RX_BUF,0,sizeof(USART_RX_BUF));//������������
		 len=0;//��������	
}




