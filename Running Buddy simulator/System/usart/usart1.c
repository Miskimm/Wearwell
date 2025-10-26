#include <stm32f10x.h>
#include <stdio.h>
#include "usart1.h"
#include "usart3.h"
#include <string.h>
#include <stdbool.h>

char Usart1RecBuf[USART1_RXBUFF_SIZE];//����1�������ݻ���
unsigned int RxCounter = 0;   //����1�յ����ݱ�־λ

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



/****************************************************************************

������Usart1 ��ʼ�� 
   PA9�� USART1_TX
   PA10: USART1_RX

������none

����: none

****************************************************************************/

void uart1_Init(u32 bound)
{
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	
	USART_DeInit(USART1);  
 
     //USART1_TX   PA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		

  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure);
   

  USART_Init(USART1, &USART_InitStructure);     //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������1�����ж�
  USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//ʹ�ܴ���1���߿����ж�/
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}

u8 buf[7]={0};
u8 nums=0;
u8 uart_rev_start = 0;     //���տ�ʼ��־
extern bool AppConnectState;
bool usart_rx_start=0;
bool usart_rx_stop=0;
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	  char ch;
	
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����жϣ�������չ������
		{
        ch = USART_ReceiveData(USART1);//����ģ�������
			  if ((ch == '&') && (usart_rx_start == 0))  //����յ��ַ�'&'���㿪ʼ����
				{
						usart_rx_start = 1;
					  /* Ϊ�˲�Ӱ�촮���жϽ��գ���ʱ�ȰѶ�ʱ���жϹر� */
					  TIM_Cmd(TIM3,DISABLE);
						TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE );
						TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ  
						RxCounter = 0;
				}
			
			  if(usart_rx_start == 1)
			  {
						Usart1RecBuf[RxCounter++] = ch;
						if (ch == '\n') 	//������յ�����
						{
								Usart1RecBuf[RxCounter] = '\n';
								usart_rx_stop  = 1;
							  usart_rx_start = 0;
							  RxCounter = 0;
						}
				}

			  if ((ch == 'C') && (uart_rev_start == 0))  //����յ��ַ�'C'���㿪ʼ����
				{
					uart_rev_start = 1;
					nums = 0;
				}
				if(uart_rev_start == 1)
				{
					  buf[nums++] = ch;  //�ַ��浽������
					  if(nums >= 7)
						{
								nums = 0;
							  uart_rev_start = 0;
							  /* У���Ƿ����ӳɹ� */
							  if((buf[0]=='C')&&(buf[1]=='O')&&(buf[2]=='N')&&(buf[3]=='N')&&(buf[4]=='E')&&(buf[5]=='C')&&(buf[6]=='T'))
								{
										AppConnectState = 1; //���ӳɹ���־��1
								}
						}
			  }
    } 
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
} 

void uart1_SendStr(char*SendBuf)//����1��ӡ����
{
	while(*SendBuf)
	{
	  while((USART1->SR&0X40)==0);//�ȴ�������� 
    USART1->DR = (u8) *SendBuf; 
		SendBuf++;
	}
}

void UsartSendBufClear(u16 len,char *buf,char *temp)//������ͻ���
{
	  u8 i,k=0;
	  unsigned char dat = 0,dat1 = 0;
	
	  for(i = 0 ; i < len ;i++)
		{
				buf[i] = 0;
		}
	
		for(i = 0 ; i < strlen(temp) ;i++)
		{
			if(i%2==0)
			{
				if ((temp[i]>='0') && (temp[i]<='9'))
						dat = temp[i]-'0' ;
				 if ((temp[i]>='A') && (temp[i]<='F'))
						dat =  temp[i]+10-'A' ;
				 if ((temp[i]>='a') && (temp[i]<='f'))
						dat =  temp[i]+10-'a' ;
			}
				
			if(i%2==1)
			{
				if ((temp[i]>='0') && (temp[i]<='9'))
						dat1 = temp[i]-'0' ;
				 if ((temp[i]>='A') && (temp[i]<='F'))
						dat1 =  temp[i]+10-'A' ;
				 if ((temp[i]>='a') && (temp[i]<='f'))
						dat1 =  temp[i]+10-'a' ;

					buf[k] = dat*16 + dat1;
					k ++;
			}	
		}
}

void uart1_send(unsigned char *bufs,unsigned char len)
{
	  if(len != 0xFF)
		{
				while (len--)
				{
						while((USART1->SR&0X40)==0);//�ȴ�������� 
						USART1->DR = (u8) *bufs; 
						bufs ++;
				}
		}
		else//����ֳ����ڻ򳬹�255�������û�д���ֳ�����
    {
        for (; *bufs != 0;	bufs++) 	//���ַ�������ͳ�ȥ
			  {
						while((USART1->SR&0X40)==0);//�ȴ�������� 
						USART1->DR = (u8) *bufs; 
				}
    }
}


