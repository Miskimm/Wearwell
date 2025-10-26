#ifndef __MAX30102_H
#define __MAX30102_H
#include "sys.h"
   	   		   
//IO��������
#define SDA_IN()  {GPIOA->CRH&=0X0FFFFFFF;GPIOA->CRH|=0X40000000;}	
#define SDA_OUT() {GPIOA->CRH&=0X0FFFFFFF;GPIOA->CRH|=0X70000000;}

//IO��������	 
#define IIC_SCL    PBout(3) //SCL
#define IIC_SDA    PAout(15) //SDA	 
#define READ_SDA   PAin(15)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(void);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//void IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

//void IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
//void IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);
#endif
















