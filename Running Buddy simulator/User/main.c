#include "sys.h"
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "gpio.h"
#include "Picture.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "adxl345.h"
#include "max30102.h"
#include "usart1.h"
#include "usart3.h"
#include "GPS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define   ColorNum    13      //ʱ�ӱ�����ɫ������

#define FLASH_SAVE_ADDR  ((u32)0x0800FF00)  				//����FLASH �����ַ(����Ϊż��)

#define STM32_RX1_BUF       Usart1RecBuf 					//����1���ջ�������
#define STM32_Rx1Counter    RxCounter             //����1�����ֽڼ���
#define STM32_RX1BUFF_SIZE  USART1_RXBUFF_SIZE    //����1�����ֽڴ�С

#define STM32_RX3_BUF       Usart3RecBuf          //����3���ջ�������
#define STM32_Rx3Counter    Rx3Counter            //����3�����ֽڼ���
#define STM32_RX3BUFF_SIZE  USART3_RXBUFF_SIZE    //����3�����ֽڴ�С

#define GPS_STR_LEN 48
GPS_INFO   GPS;      //GPS��Ϣ�ṹ��

extern unsigned char rev_start;  //���տ�ʼ��־
extern unsigned char rev_stop;   //����ֹͣ��־
extern unsigned char gps_flag;   //GPS�����־
u8 GPS_rx_flag = 0;   //����GPS���ݱ�־

u8 key_num = 0;      //��ȡ����ֵ�ı���
u8 selectColor = 0;  //ʱ�䱳����ɫ

bool set_flag = 0;  //���ñ�־
bool InitDispalyFlag = 0;//��ʼ��ʾ��־
bool init_display_time=0;
bool shuaxin = 1;  //ˢ�±�־
u8   mode = 0;    //��ʾģʽ
bool p_r = 0;	   //ƽ��/����  =0��ʾƽ�꣬=1��ʾ����
char display[16]; 

u16  step_num = 0;       //����
u16  remind_step = 1000; //���Ѳ���

unsigned short temperature; //�¶�
u16 temp_max = 375;         //�¶�����
u16 temp_min = 200;         //�¶�����
u16 heart_rate_max = 120;   //��������
u16 heart_rate_min = 55;    //��������
u16 spo2_min = 80;         //Ѫ������

float gps_lon,gps_lat;

u8 tixing=0;            //���ѱ�־
u8 StepRemindTime=0;    //������꣬���������ѵ�ʱ��

float adx,ady,adz;   //ADXL345�������
float acc;

long exerciseTime = 0;   //�˶�ʱ��
bool update_eTime = 1;  //�����˶�ʱ��
bool update_gps = 1;    //gps��ʾ
bool start_flag = 0;   //��ʼ�˶���ʱ��־
bool SendFlag = 0;     //�������ݱ�־
bool AppConnectState = 0; //����״̬

u16 timeBackground = 0;
u16 Color[ColorNum] = {
	      0xC0B0, //�Ϻ�ɫ
	      0xFA20,  //��ɫ
				0xE000,  //��ɫ
	      0xA145,  //��ɫ
	      0xFDE0,  //��ɫ   
	      0x0400,  //��ɫ
	      0xFB56,  //��ɫ
	      0x8010,  //��ɫ
	      0x001F,  //��ɫ
	      0x4208,  //��ɫ
	      0x0000,  //��ɫ
	      0x49C0,  //����ɫ
	      0x50B,  //��ɫ
};

long  t_eTime=0xFFFFFF;

void drawClockFace(void); //���Ʊ��̺���
void display_exercise_time(void);

//STM32 Flashд�����ݺ���
void STM32_FlashWriteData(void)  
{
		u16 temp_buf[9];
	  u8 add = 0;
	  
	  /* �����õĲ����Լ���������ֵ�洢�ڵ�Ƭ��FLASH */
	
	  temp_buf[add ++] = heart_rate_max;  /* �洢�������� */
	  temp_buf[add ++] = heart_rate_min;  /* �洢�������� */
		temp_buf[add ++] = spo2_min;        /* �洢Ѫ������ */
	  temp_buf[add ++] = temp_max;        /* �洢�¶����� */
	  temp_buf[add ++] = temp_min;        /* �洢�¶����� */
	  temp_buf[add ++] = remind_step;     /* �洢Ŀ�경�� */ 
	  temp_buf[add ++] = step_num;        /* �洢���ߵĲ��� */
	  temp_buf[add ++] = selectColor;     /* �洢��ɫѡ��ı�־ */ 
	
	  STMFLASH_Write(FLASH_SAVE_ADDR + 0xC0,temp_buf,8); //��������
		delay_ms(10);
}

//STM32 Flash�������ݺ���
void STM32_FlashReadData(void)  
{
		u16 temp_buf[9];
	  u8 add = 0;
	  
	  /* ���洢�����ݶ��� */
	  STM32F10x_Read(FLASH_SAVE_ADDR + 0xC0,temp_buf,8); //��������
	  
	  heart_rate_max = temp_buf[add ++];    /* ������������ */
	  heart_rate_min = temp_buf[add ++];    /* ������������ */
		spo2_min = temp_buf[add ++];          /* ����Ѫ������ */
	  temp_max = temp_buf[add ++];          /* �����¶����� */
	  temp_min = temp_buf[add ++];          /* �����¶����� */
	  remind_step = temp_buf[add ++];       /* ����Ŀ�경�� */ 
	  step_num = temp_buf[add ++];          /* �������ߵĲ��� */
	  selectColor = temp_buf[add ++];       /* ������ɫѡ��ı�־ */
	
		delay_ms(10);
}

// ����Ƿ����µĵ�Ƭ�����ǵĻ���Ҫ�洢��ʼֵ����Ļ�ֱ�Ӷ����洢��ֵ
void STM32_FlashCheck(void)  
{
	  u8 comper_str[6];
		
	  STM32F10x_Read(FLASH_SAVE_ADDR + 0xB0,(u16*)comper_str,5);
	  comper_str[5] = '\0';
	  if(strstr((char *)comper_str,"FDYDZ") == NULL)  //�µĵ�Ƭ��
		{
			 STMFLASH_Write(FLASH_SAVE_ADDR + 0xB0,(u16*)"FDYDZ",5); //д�롰FDYDZ���������´�У��
			 delay_ms(50);
			 STM32_FlashWriteData(); //д���ʼ����
	  }
		STM32_FlashReadData();     //ÿ�ο��������洢������
		timeBackground = Color[selectColor];//��ʼ��ɫ
		delay_ms(100);
}

//��ʾʱ�亯��
void display_time(u8 y,u8 pos,u16 color1,u16 color2,u16 color3,u16 color4)
{
	   if(init_display_time) //����ÿ��ֻ��Ҫ��ʾһ��
		 {
				 LCD_ShowChar(52,y,':',color1,color2,48);
				 Gui_DrawFont_GBK16(12,y+48, color1,color2,(u8 *)"    /  /  ");
				 //Gui_DrawFont_GBK16(104,y+48,color1,color2,(u8 *)"����"); 
			   init_display_time = 0;
		 }
	
		/* ��ʾʱ�䣬�����ݷ����仯ʱ��ˢ����ʾ */
	  if(SysDate.update_hour != SysDate.hour)  //��Сʱ�����仯��ʱ������ʾСʱ
		{
			  SysDate.update_hour = SysDate.hour;
			  if(pos == 1)   //������״̬�£�ѡ��Сʱ������ɫ��Ҫ�仯
				{
						LCD_ShowChar(6,y,SysDate.hour/10+'0',color3,color4,48);
						LCD_ShowChar(30,y,SysDate.hour%10+'0',color3,color4,48);
				}
				else
				{
						LCD_ShowChar(6,y,SysDate.hour/10+'0',color1,color2,48);
						LCD_ShowChar(30,y,SysDate.hour%10+'0',color1,color2,48);
				}
		}
	
	  if(SysDate.update_min != SysDate.min)  //�����ӷ����仯��ʱ������ʾ����
		{
			  SysDate.update_min = SysDate.min;
			  if(pos == 2)    //������״̬�£�ѡ�з���������ɫ��Ҫ�仯
				{
					  LCD_ShowChar(74,y,SysDate.min/10+'0',color3,color4,48);
						LCD_ShowChar(98,y,SysDate.min%10+'0',color3,color4,48);
				}
				else
				{
						LCD_ShowChar(74,y,SysDate.min/10+'0',color1,color2,48);
						LCD_ShowChar(98,y,SysDate.min%10+'0',color1,color2,48);
				}
		}
	
	  if(SysDate.update_sec != SysDate.sec)  //�����ӷ����仯��ʱ������ʾ����
		{
			  SysDate.update_sec = SysDate.sec;
			  if(pos == 3)        //������״̬�£�ѡ����������ɫ��Ҫ�仯
				{
						LCD_ShowChar(128,y+18,SysDate.sec/10+'0',color3,color4,24);
						LCD_ShowChar(140,y+18,SysDate.sec%10+'0',color3,color4,24);
				}
				else
				{
						LCD_ShowChar(128,y+18,SysDate.sec/10+'0',color1,color2,24);
						LCD_ShowChar(140,y+18,SysDate.sec%10+'0',color1,color2,24);
				}
		}
	  
		if(SysDate.update_year != SysDate.year)  //����ݷ����仯��ʱ������ʾ���
		{
			  SysDate.update_year = SysDate.year;
			  sprintf(display,"20%02d",SysDate.year);
			  if(pos == 4)       //������״̬�£�ѡ����������ɫ��Ҫ�仯
				{
						Gui_DrawFont_GBK16(12,y+48,color3,color4,(u8 *)display);
				}
				else
				{
						Gui_DrawFont_GBK16(12,y+48,color1,color2,(u8 *)display);
				}
		}
		
		if(SysDate.update_mon != SysDate.mon)  //���·ݷ����仯��ʱ������ʾ�·�
		{
			  SysDate.update_mon = SysDate.mon;
				sprintf(display,"%02d",SysDate.mon);
			  if(pos == 5)          //������״̬�£�ѡ����������ɫ��Ҫ�仯
				{
						Gui_DrawFont_GBK16(52,y+48,color3,color4,(u8 *)display);
				}
				else
				{
						Gui_DrawFont_GBK16(52,y+48,color1,color2,(u8 *)display);
				}
		}
		
		if(SysDate.update_day != SysDate.day)  //�����ڷ����仯��ʱ������ʾ����
		{
			  SysDate.update_day = SysDate.day;
				sprintf(display,"%02d",SysDate.day);
			  if(pos == 6)        //������״̬�£�ѡ������������ɫ��Ҫ�仯
				{
						Gui_DrawFont_GBK16(76,y+48,color3,color4,(u8 *)display);
				}
				else
				{
					  Gui_DrawFont_GBK16(76,y+48,color1,color2,(u8 *)display);
				}
		}

		SysDate.week=Conver_week(SysDate.year,SysDate.mon,SysDate.day); //������������
		
		if(SysDate.update_week != SysDate.week)  //�����ڷ����仯��ʱ������ʾ����
		{
			  SysDate.update_week = SysDate.week;
			
				switch(SysDate.week)
				{
						case 1:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Mon");break;
						case 2:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Tue");break;
						case 3:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Wed");break;
						case 4:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Thu");break;
						case 5:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Fri");break;
						case 6:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Sat");break;
						case 0:Gui_DrawFont_GBK16(120,y+47,color1,color2,(u8 *)"Sun");break;
				}
	  }
}

//���ʱ��ˢ�±�־��׼����һ�ε�ˢ����ʾ
void clear_time_data(void) 
{
		SysDate.update_hour = 0xff;
	  SysDate.update_min  = 0xff;
	  SysDate.update_sec  = 0xff;
	  SysDate.update_day  = 0xff;
	  SysDate.update_mon  = 0xff;
	  SysDate.update_year = 0xff;
	  SysDate.update_week = 0xff;
}

//��ȡDS1302ʱ�亯��
void Get_Ds1302_Time(void)
{
		DS1302_DateRead(&SysDate);//��ʱ��
}

//��ʾ��ʼ���溯��������ʱ�ӣ�
void DisplayInitInter1(void) 
{
	   Lcd_Clear(BLACK);  //��Ļ���Ϊ��ɫ
		 drawClockFace();   //���Ʊ��̶̿�
		 
		 LCD_ShowPicture(5,100,15,15,gImage_step);
	   LCD_ShowPicture(135,100,14,14,gImage_temp);
	
		 init_display_time = 1;
}

//��ʾ��ʼ���溯������ʾ���ݽ��棩
void DisplayInitInter2(void)
{
	   Lcd_Clear(WHITE);  //��Ļ���Ϊ��ɫ
	  
     t_eTime=0xFFFFFF;
	
	   LCD_ShowPicture(0,16,34,34,gImage_heart_2);      //��ʾ����ͼ��
	   LCD_ShowPicture(42,16,34,34,gImage_spo2_2);      //��ʾѪ��ͼ��
	   LCD_ShowPicture(83,16,34,34,gImage_temp_2);      //��ʾ�¶�ͼ��
	   LCD_ShowPicture(125,16,33,33,gImage_step_2);     //��ʾ����ͼ��
	  
	   LCD_ShowString(7,5,(u8 *)"BPM",GRAY2,WHITE,12);
     LCD_ShowString(49,5,(u8 *)"SPO2",GRAY2,WHITE,12);
     LCD_ShowString(90,5,(u8 *)"TEMP",GRAY2,WHITE,12);
	   LCD_ShowString(128,5,(u8 *)"STEPS",GRAY2,WHITE,12);
	
	   LCD_Fill(0,67,160,129,timeBackground);   //������䱳��ɫ
	   Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");
}

//��ʾ��ʼ���溯��������ʱ�ӣ�
void DisplayInitInter3(void) 
{
	   Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	   
	   clear_time_data();
	
	   LCD_Fill(0,0,160,64,timeBackground); //���ʱ�ӱ�����ɫ
	
	   Gui_DrawFont_GBK12(2, 75,    GRAY2, WHITE,(u8 *) "Lon:");
	   Gui_DrawFont_GBK12(2, 108,   GRAY2, WHITE,(u8 *) "Lat:");
		 init_display_time = 1;
}

//��������ʾ����
void beep_remind(u16 ms) 
{
		BEEP = 1;
		delay_ms(ms);
		BEEP = 0;
}

//�����ֻ������õĲ���
bool ReceiveSetValue_1(char *uart_buf,char *str,u16 *data) 
{
		u16  setValue=0;
	  char *buf = str;
	  char *str1=0,i;
	  char setvalue[6]={0};
	
		if(strstr(uart_buf,buf) == NULL) //û�н��յ�����
		{
				return  1;
		}
	  else
		{
				str1 = strstr(uart_buf,buf);  //���յ�����
				
				while(*str1 < '0' || *str1 > '9')    //�ж��ǲ���0��9��Ч����
				{
						str1 = str1 + 1;
						delay_ms(10);
				}
				i = 0;
				while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
				{
						setvalue[i] = *str1;
						i ++; str1 ++;
						if(*str1 == ',')break;            //���з���ֱ���˳�whileѭ��
						delay_ms(10);
				}
				setvalue[i] = '\0';            //���Ͻ�β��
				setValue = atoi(setvalue);
				*data = setValue;
		}	
		return  0;
}

//�����ֻ������õĲ���
bool ReceiveSetValue_2(char *uart_buf,char *str,float *data) 
{
	  char *buf = str;
	  char *str1=0,i;
	  char setvalue[6]={0};
	
		if(strstr(uart_buf,buf) == NULL) //û�н��յ�����
		{
				return  1;
		}
	  else
		{
				str1 = strstr(uart_buf,buf);  //���յ�����
				
				while(*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
				{
						str1 = str1 + 1;
						delay_ms(10);
				}
				i = 0;
				while((*str1 >= '0' && *str1 <= '9') || (*str1 == '.'))        //�ж��ǲ���0��9��Ч����
				{
					  if(*str1 == '.')
						{
							 setvalue[i] = '.';
						}
					  else 
						{
							 setvalue[i] = *str1;
						}
						i = i + 1; 
					  str1 = str1 + 1;
						delay_ms(10);
				}
				setvalue[i] = '\0';                //���Ͻ�β��
				*data = Str_To_Float(setvalue);    //���ַ���ת��Ϊ������
		}	
		return  0;
}

extern bool usart_rx_stop;
void UsartSendReceiveData(void) //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
{
		char *str1=0,i;
	  u16   int_value=0;
	  float float_value=0.0;
	  int  setValue=0;
	  char setvalue[3]={0};
	  char SEND_BUF[180];
	
	  while(usart_rx_stop == 1)  // ���յ�APP�·�������
		{
			  if(strstr(STM32_RX1_BUF,"SUCCESS")!=NULL) //�����Ͽ���
				{
						AppConnectState = 0; //���ӱ�־��0
				}
			  /*   У׼ʱ���ָ��    */
			  /*   �ֻ��·���ָ����year:0000/00/00   */
			  if(strstr(STM32_RX1_BUF,"year:")!=NULL)  //���յ����������յ�ָ��
				{
					  str1 = strstr(STM32_RX1_BUF,"year:");
					  
					  while(*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
						{
								str1 = str1 + 1;             //ָ����һ���ַ�
								delay_ms(1);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;                  //��0~9���ַ���������
								i ++; str1 ++;  
                if(*str1 == '/')break; 							
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��
						setValue = atoi(setvalue);    //���ַ���ת��Ϊ��������
						if((setValue%100)>=0 && (setValue%100)<=99)
						{
								SysDate.year = setValue%100;    //ʱ�����
						}
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;
								i ++; str1 ++;    
                if(*str1 == '/')break; 								
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��

						setValue = atoi(setvalue);
						if(setValue>=1 && setValue<=12)
						{
								SysDate.mon = setValue;   //ʱ�����
						}
						
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == ',')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��

						setValue = atoi(setvalue);
						if(setValue>=1 && setValue<=31)
						{
								SysDate.day = setValue;         //������
                DS1302_DateSet(&SysDate);		    //����ʱ��	
                if(set_flag==1)InitDispalyFlag = 1;							
						}
				}
			
			  /*   �ֻ��·���ָ����hour:00:00:00   */
				if(strstr(STM32_RX1_BUF,"hour:")!=NULL)  //���յ�����ʱ�����ָ��
				{
					  str1 = strstr(STM32_RX1_BUF,"hour:");
					  
					  while(*str1 < '0' || *str1 > '9')        //�ж��ǲ���0��9��Ч����
						{
								str1 = str1 + 1;                  //ָ����һ���ַ�
								delay_ms(1);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == ':')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=23)
						{
								SysDate.hour = setValue;   //ʱ���Сʱ
						}
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == ':')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��

						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=59)
						{
								SysDate.min = setValue;   //ʱ��ķ���
						}
						
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //�ж��ǲ���0��9��Ч����
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == '\r')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //���Ͻ�β��

						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=59)
						{
								SysDate.sec = setValue;     //ʱ�����
							  DS1302_DateSet(&SysDate);   //����ʱ��
							  if(set_flag==1)InitDispalyFlag = 1;
						}
						beep_remind(90);  //��������ʾ��
				}
				/*   �����������޵�ָ��   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"hear_min:",&int_value))==0)
				{
					  beep_remind(90);  //��������ʾ��
					  if(int_value <= 200)
						{
								heart_rate_min = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   �����������޵�ָ��   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"hear_max:",&int_value))==0)
				{
					  if(int_value < 200)
						{
								heart_rate_max = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   ����Ѫ�����޵�ָ��   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"spo_min:",&int_value))==0)
				{
					  if(int_value < 200)
						{
								spo2_min = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   ����Ŀ�경����ָ��   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"set_step:",&int_value))==0)
				{
					  if(int_value < 60000)
						{
								remind_step = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   �����¶����޵�ָ��   */
			  if((ReceiveSetValue_2(STM32_RX1_BUF,"temp_min:",&float_value))==0)
				{
					  if(float_value < 99.9)
						{
								temp_min = (u16)(float_value*10);
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   �����¶����޵�ָ��   */
			  if((ReceiveSetValue_2(STM32_RX1_BUF,"temp_max:",&float_value))==0)
				{
					  if(float_value < 99.9)
						{
								temp_max = (u16)(float_value*10);
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   �л���ɫָ��   */
				if(strstr(STM32_RX1_BUF,"SwitchColor")!=NULL)
				{
					  beep_remind(90);  //��������ʾ��
						selectColor ++;
						if(selectColor >= ColorNum)selectColor = 0;
						timeBackground = Color[selectColor]; //ѡ�񱳾���ɫ
						if(set_flag==0)
						{
							  if(mode == 0)drawClockFace();   //���Ʊ��̶̿�
							  if(mode == 1)
								{
										LCD_Fill(0,67,160,129,timeBackground);   //������䱳��ɫ
									  LCD_Fill(0,67,160,129,timeBackground);   //������䱳��ɫ
									  Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");

										t_eTime=0xFFFFFF;
								}
								if(mode == 2)
								{
										clear_time_data();
										LCD_Fill(0,0,160,64,timeBackground); //���ʱ�ӱ�����ɫ
										init_display_time = 1;
								}
						}
						else
						{
								InitDispalyFlag = 1;
						}
						STM32_FlashWriteData(); // �洢����
				}	
				/*   �л���ʾ�Ľ���ָ��   */
				if(strstr(STM32_RX1_BUF,"SwitchDisplay")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //��������ʾ��
								mode ++;
								if(mode > 2)mode=0;
								if(mode == 0)DisplayInitInter1(); // ʱ�ӱ��̽���
								if(mode == 1)DisplayInitInter2(); // ������ʾ����
								if(mode == 2)DisplayInitInter3(); // ����ʱ�Ӽ���γ�Ƚ���
						}
				}
				/*   ��ʼ����ͣ��ָ��   */
				if(strstr(STM32_RX1_BUF,"start")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //��������ʾ��
								start_flag = !start_flag;
						}
				}
				/*   ���������ָ��   */
				if(strstr(STM32_RX1_BUF,"CleanStep")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //��������ʾ��
								step_num = 0;  //��������
								STM32_FlashWriteData(); // �洢����
								StepRemindTime=0;
						}
				}
				/*   �˶�ʱ�������ָ��   */
				if(strstr(STM32_RX1_BUF,"CleanTime")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //��������ʾ��
								start_flag = 0;
								exerciseTime = 0;

								if(mode==1){
									display_exercise_time();// �˶�ʱ���������Ҫ������ʾ���˶�ʱ��
								}
						}
				}
				usart_rx_stop = 0; // ������ɱ�־���㡣׼����һ�εĽ���
				/* �ڴ����жϽ�������֮ǰ�ر��˶�ʱ���жϣ��������ݽ�����ɺ󣬶�ʱ�������������� */
				TIM_Cmd(TIM3,ENABLE);
				TIM_ITConfig( TIM3,TIM_IT_Update ,ENABLE );
		}
		if(AppConnectState == 1)
		{
				if(SendFlag == 1)    //1�����ϴ�һ������
				{
						SendFlag = 0;		
						 
						UsartSendBufClear(sizeof(SEND_BUF),SEND_BUF,BLUETOOTHADDR);  //������ͻ���
					  sprintf(SEND_BUF+strlen("SEND"),"hear:%d#,spo2:%d#,temp:%.1f#,step:%d#,time:%02d:%02d:%02d#,lon:%.6f#,lat:%.6f#",hrAvg,spo2Avg,(float)temperature/10,step_num,(int)(exerciseTime/3600),(int)(exerciseTime%3600/60),(int)(exerciseTime%3600%60),gps_lon,gps_lat);/*��������װ��*/
						
						if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))strcat(SEND_BUF,"hear_warn,"); // ���ʲ��ڷ�Χ�����ͳ���ָ��ֻ��˽��յ��Ժ��������ʾ��ɫ
            if((spo2Avg!=0)&&(spo2Avg<=spo2_min))strcat(SEND_BUF,"spo2_warn,"); // Ѫ�����ڷ�Χ�����ͳ���ָ��ֻ��˽��յ��Ժ��������ʾ��ɫ
						if(temperature<=temp_min||temperature>=temp_max)strcat(SEND_BUF,"temp_warn,"); // �¶Ȳ��ڷ�Χ�����ͳ���ָ��ֻ��˽��յ��Ժ��������ʾ��ɫ
						if(step_num >= remind_step)strcat(SEND_BUF,"step_warn,"); // ������꣬�ֻ��˽��յ��Ժ��������ʾ��ɫ       
						
						strcat(SEND_BUF,"\r\n");
						uart1_send((u8 *)SEND_BUF, 0xFF);  //�������ϴ���APP��
				}
		}
		else
		{
				SendFlag = 0;		
		}
}

//��ȡ����ֵ����
u8 keyscan(void)   
{
	  u8 key = 0;
	
		if(KEY1 == 0)   //��һ���������·��ؼ�ֵ1
		{
		  	delay_ms(1); 
			  if(KEY1 == 0)
				{
					  while(KEY1 == 0);
						key = 1;
				}
		}
		if(KEY2 == 0)  //�ڶ����������·��ؼ�ֵ2
		{
		  	delay_ms(80); 
			  if(KEY2 == 0)
				{
						key = 2;
				}
		}
		if(KEY3 == 0) //�������������·��ؼ�ֵ3
		{
		  	delay_ms(80); 
			  if(KEY3 == 0)
				{
						key = 3;
				}
		}
		if(KEY4 == 0) //���ĸ��������·��ؼ�ֵ4
		{
		  	delay_ms(1);
			  if(KEY4 == 0)
				{
					  //while(KEY4 == 0);
						key = 4;
				}
		}
		if(KEY5 == 0) //������������·��ؼ�ֵ5
		{
		  	delay_ms(1);
			  if(KEY5 == 0)
				{
					  while(KEY5 == 0);
						key = 5;
				}
		}
		return key;
}

/**************************************************************************************************************
					                      ����ΪҺ���˵�������ʾ����
***************************************************************************************************************/

#define x            2    //���õĵ�һ����ʼλ��x
#define y            21   //���õĵ�һ����ʼλ��y
#define f_size       16   //�����С
#define interval     2    //ÿ�м��������

int display_menu5(void)//��ʾ���ò�������
{
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(28, 2, WHITE, RED, (u8 *)"Set Step Goal");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
				if(key_num == 2)    //�ڶ����������£����·�
				{
						if(remind_step<60000)remind_step++;
						InitDispalyFlag = 1;
				}
				if(key_num == 3)   //�������������£����Ϸ�
				{
					  if(remind_step>0)remind_step--;
					  InitDispalyFlag = 1;
				}
				if(key_num == 4)  //������һ��
				{
						Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(InitDispalyFlag)
				{
					  sprintf((char *)display,"%05d",remind_step);
						LCD_ShowString(40,60,(u8 *)display,BLACK,WHITE,24); /*  ��ʾ��������ֵ  */
					  InitDispalyFlag = 0;
				}
		}
}

int display_menu4(void)//��ʾ�����¶Ƚ���
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //�·���־
	  bool set_flag=0;     //���ñ�־
	  unsigned char  row = 2; //ÿҳ�����ʾ������
	  u16 color[2][2];//��ɫ����
	  char dat1[5]={'0','0','.','0','\0'};
    char dat2[5]={'0','0','.','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:"},    /*  ��ʾ���ַ����� */
				{"2.Up:"},    /*  ��ʾ���ַ����� */
};
	
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(20, 2, WHITE, RED, (u8 *)"Set Temperature");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
			  if(key_num == 1)   //��һ���������¡������·�
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //�ڶ����������¡��ǼӼ�
				{
					  if(index==1)
						{
							  if(temp_max-temp_min>1)temp_min++;
							  set_flag = 1;
						}
						if(index==2)
						{
							  if(temp_max<999)temp_max++;
							  set_flag = 1;
						}
				}
				if(key_num == 3)   //�������������¡��Ǽ���
				{
					  if(index==1) 
						{
							  if(temp_min>0)temp_min--;
							  set_flag=1;
						}
						if(index==2)
						{
							  if(temp_max-temp_min>1)temp_max--;
							  set_flag = 1;
						}
				}
				if(key_num == 4)  //������һ��
				{
						Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  ��ʼ��������ɫ�����屳����ɫ  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  �������л������еİ���ʱ������ɫ�����屳����ɫ��Ҫ�����仯  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* ѡ��ѡ�����ɫ��δѡ��ѡ��ѡ�����ɫ���� */
								/*  δѡ�е���ɫ����  */
								/*  ÿ���·�ʱ����һ����Ҫ��ʾ��ɫ  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //��ʾѡ������ɫ
					  }
						
						/* �¶�����ֵװ�� */
						dat1[0] = temp_min/100+'0';
						dat1[1] = temp_min%100/10+'0';
						dat1[3] = temp_min%10+'0';
						/* �¶�����ֵװ�� */
						dat2[0] = temp_max/100+'0';
						dat2[1] = temp_max%100/10+'0';
						dat2[3] = temp_max%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								/* ��ʾ����Ϣ */
							  for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //��ʾ��һҳ����Ϣ
								}	
						}
						else
						{
								/* ��ÿ���Ϸ������·�ʱ������Ҫˢ��ÿ�е���ʾ���������׵���ˢ�±����� ֻ��Ҫ��ʾˢ�������������ַ�����*/
								if(down_flag == 1)
								{
									  if(index == 1)  
										{   
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval), color[index-1][0], color[index-1][1], (u8 *)display_[index-1].str);
												Gui_DrawFont_GBK16(x, y + (row-1)*(f_size + interval),   color[row-1][0]  , color[row-1][1]  , (u8 *)display_[row-1].str);
										}
										else           
										{
												Gui_DrawFont_GBK16(x, y + (index-1-1)*(f_size + interval), color[index-1-1][0], color[index-1-1][1], (u8 *)display_[index-1-1].str);
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval),   color[index-1][0]  , color[index-1][1]  , (u8 *)display_[index-1].str);
										}
								}
						}
						if(set_flag)
						{
								/* ������ʾ���õ�ֵ */
								if(index==1)
							  {
									  Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
										Gui_DrawFont_GBK12(105,y + 0*(f_size + interval)+1, color[0][0], color[0][1], (u8 *)"��");
								}
								if(index==2)
								{
										Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);	
									  Gui_DrawFont_GBK12(105,y + 1*(f_size + interval)+1, color[1][0], color[1][1], (u8 *)"��");
								}
						}
						else
						{
								Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
							  Gui_DrawFont_GBK12(105,y + 0*(f_size + interval)+1, color[0][0], color[0][1], (u8 *)"��");
								Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);
							  Gui_DrawFont_GBK12(105,y + 1*(f_size + interval)+1, color[1][0], color[1][1], (u8 *)"��");
						}
						InitDispalyFlag = 0;
						set_flag = 0;
						down_flag= 0;
				}
		}
}

int display_menu3(void)//��ʾ����Ѫ������
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //�·���־
	  bool set_flag=0;     //���ñ�־
	  unsigned char  row = 1; //ÿҳ�����ʾ������
	  u16 color[1][2];//��ɫ����
	  char dat1[3]={'0','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:      %"},    /*  ��ʾ���ַ����� */
};
	
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(20, 2, WHITE, RED, (u8 *)"Set Blood Oxygen");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
			  if(key_num == 1)   //��һ���������¡������·�
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //�ڶ����������¡��ǼӼ�
				{
					  if(index==1)
						{
							  if(spo2_min<200)spo2_min++;
							  set_flag = 1;
						}
				}
				if(key_num == 3)   //�������������¡��Ǽ���
				{
					  if(index==1) 
						{
							  if(spo2_min>0)spo2_min--;
							  set_flag=1;
						}
				}
				if(key_num == 4)  //������һ��
				{
						Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  ��ʼ��������ɫ�����屳����ɫ  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  �������л������еİ���ʱ������ɫ�����屳����ɫ��Ҫ�����仯  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* ѡ��ѡ�����ɫ��δѡ��ѡ��ѡ�����ɫ���� */
								/*  δѡ�е���ɫ����  */
								/*  ÿ���·�ʱ����һ����Ҫ��ʾ��ɫ  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //��ʾѡ������ɫ
					  }
						
						 /* Ѫ������ֵװ�� */
						 dat1[0] = spo2_min/100+'0';
						 dat1[1] = spo2_min%100/10+'0';
						 dat1[2] = spo2_min%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								Gui_DrawFont_GBK16(x, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)display_[0].str);
						}
						
						/* ������ʾ���õ�ֵ */
						Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
						
						InitDispalyFlag = 0;
						set_flag = 0;
						down_flag= 0;
				}
		}
}

int display_menu2(void)//��ʾ�������ʽ���
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //�·���־
	  bool set_flag=0;     //���ñ�־
	  unsigned char  row = 2; //ÿҳ�����ʾ������
	  u16 color[2][2];//��ɫ����
	  char dat1[4]={'0','0','0','\0'};
    char dat2[4]={'0','0','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:"},    /*  ��ʾ���ַ����� */
				{"2.Up:"},    /*  ��ʾ���ַ����� */
};
	
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(28, 2, WHITE, RED, (u8 *)"Set Heart Rate");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
			  if(key_num == 1)   //��һ���������¡������·�
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //�ڶ����������¡��ǼӼ�
				{
					  if(index==1)
						{
							  if(heart_rate_max-heart_rate_min>1)heart_rate_min++;
							  set_flag = 1;
						}
						if(index==2)
						{
							  if(heart_rate_max<200)heart_rate_max++;
							  set_flag = 1;
						}
				}
				if(key_num == 3)   //�������������¡��Ǽ���
				{
					  if(index==1) 
						{
							  if(heart_rate_min>0)heart_rate_min--;
							  set_flag=1;
						}
						if(index==2)
						{
							  if(heart_rate_max-heart_rate_min>1)heart_rate_max--;
							  set_flag = 1;
						}
				}
				if(key_num == 4)  //������һ��
				{
						Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  ��ʼ��������ɫ�����屳����ɫ  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  �������л������еİ���ʱ������ɫ�����屳����ɫ��Ҫ�����仯  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* ѡ��ѡ�����ɫ��δѡ��ѡ��ѡ�����ɫ���� */
								/*  δѡ�е���ɫ����  */
								/*  ÿ���·�ʱ����һ����Ҫ��ʾ��ɫ  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //��ʾѡ������ɫ
					  }
						
						/* ��������ֵװ�� */
						dat1[0] = heart_rate_min/100+'0';
						dat1[1] = heart_rate_min%100/10+'0';
						dat1[2] = heart_rate_min%10+'0';
						/* ��������ֵװ�� */
						dat2[0] = heart_rate_max/100+'0';
						dat2[1] = heart_rate_max%100/10+'0';
						dat2[2] = heart_rate_max%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								/* ��ʾ����Ϣ */
							  for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //��ʾ��һҳ����Ϣ
								}	
						}
						else
						{
								/* ��ÿ���Ϸ������·�ʱ������Ҫˢ��ÿ�е���ʾ���������׵���ˢ�±����� ֻ��Ҫ��ʾˢ�������������ַ�����*/
								if(down_flag == 1)
								{
									  if(index == 1)  
										{   
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval), color[index-1][0], color[index-1][1], (u8 *)display_[index-1].str);
												Gui_DrawFont_GBK16(x, y + (row-1)*(f_size + interval),   color[row-1][0]  , color[row-1][1]  , (u8 *)display_[row-1].str);
										}
										else           
										{
												Gui_DrawFont_GBK16(x, y + (index-1-1)*(f_size + interval), color[index-1-1][0], color[index-1-1][1], (u8 *)display_[index-1-1].str);
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval),   color[index-1][0]  , color[index-1][1]  , (u8 *)display_[index-1].str);
										}
								}
						}
						if(set_flag)
						{
								/* ������ʾ���õ�ֵ */
								if(index==1)Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
								if(index==2)Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);	
						}
						else
						{
								Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
								Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);
						}
						InitDispalyFlag = 0;
						set_flag = 0;
						down_flag= 0;
				}
		}
}

int display_menu1(void)//��ʾʱ�����ý���
{
	  unsigned char index=1;
    u16 nian_temp;
	
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Set Time");
	  InitDispalyFlag = 1;
	  clear_time_data();
	  init_display_time = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
			  if(key_num == 1)   //��һ���������¡������·�
				{
						index ++;
					  if(index>6)index=1;
					  nian_temp=2000+SysDate.year;
						if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //�ж��Ƿ�Ϊ����
								p_r=1;
						else
								p_r=0;
					  clear_time_data();
					  InitDispalyFlag = 1;
				}
				if(key_num == 2)   //�ڶ����������¡��ǼӼ�
				{
						 if(index == 1)//����ʱ
						 {
								 SysDate.hour++;
								 if(SysDate.hour == 24)//��0��
								 SysDate.hour = 0;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else  if(index == 2)//���÷�
						 {
								 SysDate.min++;
								 if(SysDate.min == 60)//60����
								 SysDate.min = 0;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else  if(index == 3)//������
						 {
								 SysDate.sec++;
								 if(SysDate.sec == 60)//60��
								 SysDate.sec = 0;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else if(index == 4)//������
						 {
								 SysDate.year++;
								 if(SysDate.year == 100)SysDate.year = 0;//���99
							 
								 DS1302_DateSet(&SysDate);//����ʱ��
							 
								 nian_temp=2000+SysDate.year;
								if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //�ж��Ƿ�Ϊ����
										p_r=1;
								else
										p_r=0;
						 }
						 else if(index == 5)//������
						 {
								SysDate.mon++;
								if(SysDate.mon==13)//���12����
								SysDate.mon=1;
								if((SysDate.mon==4)||(SysDate.mon==6)||(SysDate.mon==9)||(SysDate.mon==11))//4��6��9��11��ֻ��30��
								{
										if(SysDate.day>30)
												SysDate.day=1;
								}
								else
								{
										if(SysDate.mon==2)//2��
										{
												if(p_r==1)//����2����29��
												{
														if(SysDate.day>29)
																SysDate.day=1;
												}
												else//ƽ��2��ֻ��28��
												{
														if(SysDate.day>28)
																SysDate.day=1;
												}
										}
								}
								DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else if(index == 6)//������
						 {
								 SysDate.day++;
									if((SysDate.mon==1)||(SysDate.mon==3)||(SysDate.mon==5)||(SysDate.mon==7)||(SysDate.mon==8)||(SysDate.mon==10)||(SysDate.mon==12))//��31����·�
									{
											if(SysDate.day==32)
													SysDate.day=1;
									}
									else
									{
											if(SysDate.mon==2)
											{
													if(p_r==1)
													{
															if(SysDate.day==30)//����2����29��
																	SysDate.day=1;
													}
													else
													{
															if(SysDate.day==29)//ƽ��2��ֻ��28��
																	SysDate.day=1;
													}
											}
											else
											{
													if(SysDate.day==31)//���30 
															SysDate.day=1;
											}
									}
									DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 InitDispalyFlag = 1;
				}
				if(key_num == 3)   //�������������¡��Ǽ���
				{
					   if(index == 1)//����ʱ
						 {
								 if(SysDate.hour == 0)SysDate.hour = 24;
								 SysDate.hour--;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else  if(index == 2)//���÷�
						 {
								 if(SysDate.min == 0)SysDate.min = 60;
								 SysDate.min--;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else  if(index == 3)//������
						 {
								 if(SysDate.sec == 0)SysDate.sec = 60;
								 SysDate.sec--;
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else  if(index == 4)//������
						 {
								 if(SysDate.year == 0)SysDate.year = 100;
								 SysDate.year--;
								 DS1302_DateSet(&SysDate);//����ʱ��
								 
								 nian_temp=2000+SysDate.year;
								if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //�ж��Ƿ�Ϊ����
										p_r=1;
								else
										p_r=0;
						 }
						 else if(index == 5)//������
						 {
								SysDate.mon--;
								if(SysDate.mon==0)SysDate.mon=12;
								if((SysDate.mon==4)||(SysDate.mon==6)||(SysDate.mon==9)||(SysDate.mon==11))
								{
										if(SysDate.day>30)SysDate.day=1;
								}
								else
								{
										if(SysDate.mon==2)
										{
												if(p_r==1)
												{
														if(SysDate.day>29)SysDate.day=1;
												}
												else
												{
														if(SysDate.day>28)SysDate.day=1;
												}
										}
								}
								DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 else if(index == 6)//������
						 {
								 SysDate.day--;
									if((SysDate.mon==1)||(SysDate.mon==3)||(SysDate.mon==5)||(SysDate.mon==7)||(SysDate.mon==8)||(SysDate.mon==10)||(SysDate.mon==12))
									{
											if(SysDate.day==0)SysDate.day=31;
									}
									else
									{
											if(SysDate.mon==2)
											{
													if(p_r==1)
													{
															if(SysDate.day==0)SysDate.day=29;
													}
													else
													{
															if(SysDate.day==0)SysDate.day=28;
													}
											}
											else
											{
													if(SysDate.day==0)
															SysDate.day=30;
											}
									}
								 DS1302_DateSet(&SysDate);//����ʱ��
						 }
						 InitDispalyFlag = 1;
				}
				if(key_num == 4)  //������һ��
				{
						Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(InitDispalyFlag)
				{
					  display_time(40,index,BLACK,WHITE,WHITE,timeBackground); //��ʾʱ��
						InitDispalyFlag = 0;
				}
		}
}

int display_main_menu(void) //��ʾ���˵�����
{	  
	  unsigned char index=1,i;
	  bool  up_flag  = 0;  //�Ϸ���־
	  bool down_flag = 0;  //�·���־
	  unsigned char  row = 5;   //ÿҳ�����ʾ������
	  u16 color[5][2];//��ɫ����
	
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Set Time"},    /*  ��ʾ���ַ����� */
				{"2.Set Heart Rate"},
				{"3.Set Blood Oxygen"},
				{"4.Set Temperature"},
				{"5.Set Step Goal"},
};

	  set_flag = 1;
		Lcd_Clear(WHITE);   //��������Ļ���Ϊ��ɫ
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
				key_num = keyscan();  //��ȡ����ֵ
			  if(key_num == 1)     //��һ���������£���ȷ��
				{
						if(index==1)display_menu1();  //ѡ�����ʱ�����ý���
					  if(index==2)display_menu2();  //ѡ������������ʽ���
					  if(index==3)display_menu3();  //ѡ���������Ѫ������
					  if(index==4)display_menu4();  //ѡ����������¶Ƚ���
					  if(index==5)display_menu5();  //ѡ��������ò�������
				}
				if(key_num == 2)    //�ڶ����������£����·�
				{
						index ++;
					  down_flag = 1;
					  if(index>row)
						{
								index=1;
						}
				}
				if(key_num == 3)   //�������������£����Ϸ�
				{
					  index --;
					  up_flag  = 1;
					  if(index==0)
					  {
							  index=row;
						}
				}
				if(key_num == 4) //�˳�����
				{
					  if(mode == 0)DisplayInitInter1(); //ʱ�ӱ��̽���
						if(mode == 1)DisplayInitInter2(); //������ʾ����
						if(mode == 2)DisplayInitInter3(); //����ʱ�Ӽ���γ�Ƚ���
					
						STM32_FlashWriteData(); //�˳������Ժ��ȴ洢�����õ�����
					  set_flag = 0; //���ñ�־����
					  return 0;
				}
				if(down_flag || up_flag || InitDispalyFlag)
				{
						/*  ��ʼ��������ɫ�����屳����ɫ  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
					  
						/*  �������л������еİ���ʱ������ɫ�����屳����ɫ��Ҫ�����仯  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						/* ѡ��ѡ�����ɫ��δѡ��ѡ��ѡ�����ɫ���� */
						/*  δѡ�е���ɫ����  */
						/*  ÿ���·�ʱ����һ����Ҫ��ʾ��ɫ  */
						if(down_flag==1)
						{
								if(index%row==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
								else 
								{		
								  	LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);	
								}
						}
						/*  ÿ���Ϸ�ʱ����һ����Ҫ��ʾ��ɫ  */
						if(up_flag==1)
						{
							  if(index==row)
								{
									LCD_Fill(0,y + (index-row)*(f_size + interval),X_MAX_PIXEL,(y + (index-row)*(f_size + interval))+f_size,WHITE); 
								}
							  else
							  {
								 LCD_Fill(0,y + (index)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index)*f_size)+(index)*interval,WHITE);	 
							  }
						}
						
						/*  ѡ�е���ɫ����  */
						LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //��ʾѡ������ɫ
						
						if(InitDispalyFlag)
						{
								/* ��ʾ����Ϣ */
								for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //��ʾ��һҳ����Ϣ
								}
						}
						else
						{
								/* ��ÿ���Ϸ������·�ʱ������Ҫˢ��ÿ�е���ʾ���������׵���ˢ�±����� ֻ��Ҫ��ʾˢ�������������ַ�����*/
								if(down_flag == 1)   //���·�ʱ��ֻ��Ҫˢ�µ�ǰѡ�е���һ�к���һ��
								{
										if(index%row==1)  //��һ��
										{
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval), color[index-1][0], color[index-1][1], (u8 *)display_[index-1].str);
											  Gui_DrawFont_GBK16(x, y + (row-1)*(f_size + interval), color[row-1][0], color[row-1][1], (u8 *)display_[row-1].str);
										}
										else
										{
												Gui_DrawFont_GBK16(x, y + (index-1-1)*(f_size + interval), color[index-1-1][0], color[index-1-1][1], (u8 *)display_[index-1-1].str);
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval),   color[index-1][0]  , color[index-1][1]  , (u8 *)display_[index-1].str);
										}
								}
								if(up_flag==1) //���Ϸ�ʱ��ֻ��Ҫˢ�µ�ǰѡ�е���һ�к���һ��
								{
										if(index==row)  //�����һ��
										{
											  Gui_DrawFont_GBK16(x, y + (index-row)*(f_size + interval), color[index-row][0], color[index-row][1], (u8 *)display_[index-row].str);
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval), color[index-1][0], color[index-1][1], (u8 *)display_[index-1].str);
										}
										else
										{
												Gui_DrawFont_GBK16(x, y + (index-1)*(f_size + interval), color[index-1][0], color[index-1][1], (u8 *)display_[index-1].str);
												Gui_DrawFont_GBK16(x, y + index*(f_size + interval), color[index][0], color[index][1], (u8 *)display_[index].str);
										}
								}
					  }
						down_flag = 0;  //�·���־����
						up_flag   = 0;  //�Ϸ���־����
						InitDispalyFlag = 0;
				}
		}
}

/*****************************************************************************************************************************
					                                              ����
******************************************************************************************************************************/

// ��ȡ��������
void Get_Steps(void)
{
	  static u16 temp=0;
	  static bool flag=0;
	
	  adxl345_read_average(&adx,&ady,&adz,10);// ��ȡADXL345���ٶ�����
		acc=ady;  // ��ȡy����ٶ�ֵ�����ڲ�������
	
		if(acc>0)// ������
		{
			if(acc/10>=4&&flag==1)// ���ٶ�ֵ��������ֵ�Ƿ����4������flagΪ1������Ϊһ��������ɣ�������1
			{
				flag=0;	
				if(step_num<60000)step_num++;	// ������1
				if(temp!=step_num)// �����������仯��ȥ�洢����
				{
						temp=step_num;
					  STM32_FlashWriteData(); // �洢����
				}
			}
		}
		if(acc<0)//�ڸ���
		{
			acc=-acc;
			if(acc/10>=4)// ���ٶ�ֵ���ڸ����Ƿ�С��-4
			{
					flag=1;// falg��1
			}
		}
}

//��ʾ��ʽ����
void display_format(u16 x0,u16 y0,u16 data,u16 size,u16 Color1,u16 Color2)
{
	  u16 x1=x0,y1=y0;
	
	  if(data<=9&&mode==1){x1=x0+2;}

		sprintf(display,"  %01d  ",data);
		if(data>9)sprintf(display,"  %02d ",data);
		if(data>99)sprintf(display," %03d ",data);
		if(data>999)sprintf(display,"%04d ",data);
		if(data>9999)sprintf(display,"%05d",data);
		
	  if(size==16)
		{
				 Gui_DrawFont_GBK16(x1, y1, Color1, Color2,(u8 *)display);//��ʾֵ
		}
		else
		{
				LCD_ShowString(x1,y1,(u8 *)display,Color1,Color2,size);   //��ʾֵ
		}
}

//��ʾ�˶���ʱ�亯��
void display_exercise_time(void)  
{
	  if(t_eTime!=(exerciseTime%3600/60)) /*�˶�ʱ���Сʱ�ͷ�����ʾ����Ƚϴ�ֻ�е����ݱ仯��ʱ������ʾ*/
		{
	      t_eTime = (exerciseTime%3600/60);
				sprintf(display,"%02d:%02d",(int)(exerciseTime/3600),(int)(exerciseTime%3600/60));
				LCD_ShowString(6,84,(u8 *)display,WHITE,timeBackground,48);  /*  ��ʾ�˶�ʱ���Сʱ�ͷ���  */
		}
	  sprintf(display,"%02d",(int)(exerciseTime%3600%60));
	  LCD_ShowString(128,103,(u8 *)display,WHITE,timeBackground,24); /*  ��ʾ�˶�ʱ������  */
}

// ��ȡGPS���ݺ���
void Get_GPS(void)
{
	  static u8 timeCount=0;
	
		if(rev_stop == 1 && timeCount++ >= 20)   //���������һ��
		{
				if (GPS_RMC_Parse(STM32_RX3_BUF, &GPS)) //����GPRMC
				{
						gps_flag = 0;
						rev_stop  = 0;
					  gps_lon = (float)GPS.longitude_Degree;  // ����
					  gps_lat = (float)GPS.latitude_Degree;   // γ��
				}
				else
				{
						gps_flag = 0;
						rev_stop  = 0;
				}
				timeCount=0;
		}
}

#define CENTER_X  (X_MAX_PIXEL/2)  // ��������x
#define CENTER_Y  (Y_MAX_PIXEL/2)  // ��������y
#define RADIUS 62 // Բ�İ뾶

struct info {
    char *str; 
};
struct info scale[12] = {
				{"12"},{"1"},{"2"},{"3"},{"4"},{"5"},{"6"},{"7"},{"8"},{"9"},{"10"},{"11"},     
};

// ���Ʊ��̶̿Ⱥ���
void drawClockFace(void) 
{
	  int i=0,x1,x2,y1,y2;
	  int len;
	  u16 temp_color = 0;
	
	  if(timeBackground==BLACK)temp_color=GRAY0;else temp_color=timeBackground;
	
	  /* ��Բ */
    Gui_Circle(CENTER_X, CENTER_Y, RADIUS, temp_color);
	  Gui_Circle(CENTER_X, CENTER_Y, RADIUS-1, temp_color);
    Gui_Circle(CENTER_X, CENTER_Y, RADIUS-2, GRAY1);
    
    // ���ƿ̶�
    for(i = 0; i < 60; i ++)
  	{
        float angle = i * 6 * 3.14 / 180;   // ÿ6��Ϊһ�̶�
        len = (i%5 == 0) ? 6 : 1;           // ÿ��5��Ϊ���̶ȣ������Ķ��Ƕ̶̿�
			
			  /* �����������Ǻ���sin��= y/r��cos��= x/r������̶����� */
        x1 = CENTER_X + (RADIUS-5)*sin(angle);
        y1 = CENTER_Y - (RADIUS-5)*cos(angle);
        x2 = CENTER_X + (RADIUS-5-len)*sin(angle);
        y2 = CENTER_Y - (RADIUS-5-len)*cos(angle);
			
			  if(i%15 == 0)LCD_DrawLine(x1, y1, x2, y2, RED);else LCD_DrawLine(x1, y1, x2, y2, WHITE); // ���ߺ��������̶�
			
			  if(i%5 == 0)// ÿ��5�Ŀ̶�
				{
						switch(i)
						{  
  							/* �������̶ֿ���ʾ */
							  case(0):  Gui_DrawFont_GBK12(x2-6,y2-1,temp_color,BLACK,(u8 *)scale[i/5].str); break;
							  case(5):  LCD_ShowString(x2-6,y2,(u8 *)scale[i/5].str,BLUE2,BLACK,12);         break;
							  case(10): LCD_ShowString(x2-8,y2-3,(u8 *)scale[i/5].str,BLUE2,BLACK,12);       break;
							  case(15): Gui_DrawFont_GBK12(x2-8,y2-7,temp_color,BLACK,(u8 *)scale[i/5].str); break;
								case(20): LCD_ShowString(x2-9,y2-7,(u8 *)scale[i/5].str,BLUE2,BLACK,12);       break;
							  case(25): LCD_ShowString(x2-6,y2-10,(u8 *)scale[i/5].str,BLUE2,BLACK,12);      break;
							  case(30): Gui_DrawFont_GBK12(x2-4,y2-15,temp_color,BLACK,(u8 *)scale[i/5].str);break;
							  case(35): LCD_ShowString(x2-3,y2-10,(u8 *)scale[i/5].str,BLUE2,BLACK,12);      break;
							  case(40): LCD_ShowString(x2,y2-6,(u8 *)scale[i/5].str,BLUE2,BLACK,12);         break;
							  case(45): Gui_DrawFont_GBK12(x2+1,y2-7,temp_color,BLACK,(u8 *)scale[i/5].str); break;
							  case(50): LCD_ShowString(x2-1,y2-4,(u8 *)scale[i/5].str,BLUE2,BLACK,12);       break;
							  case(55): LCD_ShowString(x2-4,y2-1,(u8 *)scale[i/5].str,BLUE2,BLACK,12);       break;
						}
				}
    }
}

// ���Ʊ���ָ�뺯��
void drawHand(uint16_t x0, uint16_t y0, uint16_t length, float angle, uint16_t color) 
{
	  uint16_t x1,y1;
	  /*
			�������Ǻ���sin��= y/r,�����ָ���ǻ��ȣ�����=�Ƕ�*��/180������sin(30��)�ڳ�����Ӧ��д��sin(30*3.14/180)
	    �������Ǻ���cos��= x/r,�����ָ���ǻ��ȣ�����=�Ƕ�*��/180������cos(30��)�ڳ�����Ӧ��д��cos(30*3.14/180)
	    rΪԲ�İ뾶���ڱ�����Ҳ����ʱ������ĳ���
	  */
    x1 = x0 + length * sin((angle)*3.14/180.0);  
    y1 = y0 - length * cos((angle)*3.14/180.0);
	  
    LCD_DrawLine(x0, y0, x1, y1, color);  // ��ԭ������ͽ������껭��
}

// ���±���ʱ�Ӻ���
void updateClock(uint8_t h, uint8_t m, uint8_t s) 
{
	  static u16 hourHand_angle=0;
	  static u16 minuteHand_angle=0;
	  static u16 secondHand_angle=0;
	
    // �����ָ��
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, BLACK);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle, BLACK);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle, BLACK);

    // �����½Ƕ�
    hourHand_angle = h * 30 + m * 0.5;    // ÿСʱ30�ȣ�ÿ����0.5��
    minuteHand_angle = m * 6;             // ÿ����6��
    secondHand_angle = s * 6;             // ÿ��6��
	
	  // ������ָ��
	  drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, WHITE);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle,VIOLETRED);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle,GREEN);
		
		// ��ʾѪ��
	  LCD_ShowPicture(44,54,13,13,gImage_spo2);
		sprintf(display,"%03d",spo2Avg);
		if((spo2Avg!=0)&&(spo2Avg<=spo2_min))
		{
			  LCD_ShowString(42,68,(u8 *)display,RED,BLACK,12);   // Ѫ�����ڷ�Χ��������ʾ��ɫ
		}
		else
		{
	    	LCD_ShowString(42,68,(u8 *)display,GRAY0,BLACK,12);  // ��ʾѪ��
		}
		
		// ��ʾ����
	  LCD_ShowPicture(100,54,14,14,gImage_heart);
		sprintf(display,"%03d",hrAvg);
		if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))
		{
				LCD_ShowString(98,68,(u8 *)display,RED,BLACK,12);    // ���ʲ��ڷ�Χ��������ʾ��ɫ
		}
	  else
		{
				LCD_ShowString(98,68,(u8 *)display,GRAY0,BLACK,12);  // ��ʾ����
		}
		
    // ������ָ��
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, WHITE);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle,VIOLETRED);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle,GREEN);
}

int main(void)
{	
	  bool keyLongPress=0;
		unsigned int keyPressTime=0;// �������¼�������
	
		delay_init();	          // ��ʱ������ʼ��	  
	  NVIC_Configuration();
	  GPS_rx_flag = 0;
		delay_ms(500);       // �ϵ�˲�����һ����ʱ�ڳ�ʼ��
	  KEY_GPIO_Init();     // ������ʼ��
	  Lcd_Init();          // LCDҺ����ʼ��
	  STM32_FlashCheck();
	  KEY_GPIO_Init();    // �������ų�ʼ��
	  DS18B20_Init();     // �¶�DS18B20��ʼ��
	  adxl345_init();     // ADXL345��ʼ��
	  Lcd_Clear(timeBackground);  //��Ļ���Ϊ��ɫ
		uart1_Init(9600);
	  USART3_Init(9600);
	  max30102_init_fifo();  // MAX30102��ʼ��
	  DS1302_Init(&SysDate); // DS1302ʱ�ӳ�ʼ��
	  DisplayInitInter1();   // ��ʾ��ʼ����
	  GPS_rx_flag = 1;
		TIM3_Init(199,7199);     // ��ʱ����ʼ������ʱ20ms
		//Tout = ((arr+1)*(psc+1))/Tclk ; 
		//Tclk:��ʱ������Ƶ��(��λMHZ)
		//Tout:��ʱ�����ʱ��(��λus)
		memset(STM32_RX1_BUF,0,STM32_RX1BUFF_SIZE);  //��ջ���
		STM32_Rx1Counter = 0;
		while(1)
		{  
			  key_num = keyscan();     // ����ɨ��
			
				if(key_num == 2)   // ��������״̬�£����ڶ��������ǲ�������
				{
					  selectColor ++;
						if(selectColor >= ColorNum)selectColor = 0;
						timeBackground = Color[selectColor]; //ѡ�񱳾���ɫ
						if(set_flag==0)
						{
							  if(mode == 0)drawClockFace();   //���Ʊ��̶̿�
							  if(mode == 1)
								{
										LCD_Fill(0,67,160,129,timeBackground);   //������䱳��ɫ
									  Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");

										t_eTime=0xFFFFFF;
								}
								if(mode == 2)
								{
										clear_time_data();
										LCD_Fill(0,0,160,64,timeBackground); //���ʱ�ӱ�����ɫ
										init_display_time = 1;
								}
						}
						else
						{
								InitDispalyFlag = 1;
						}
						STM32_FlashWriteData(); // �洢����
				}
				if(key_num == 3)   // ��������״̬�£����������������������
				{
						step_num = 0;
						STM32_FlashWriteData(); // �洢����
					  StepRemindTime=0;
					  beep_remind(90);       // ����������
				}
				if(key_num == 4)   // ��������״̬�£������ĸ������ǿ����˶���ʱ�͹ر��˶���ʱ
				{
						while(KEY4 == 0)
						{
								if(keyPressTime<60000)keyPressTime++;
								if(keyPressTime >= 500 && !keyLongPress)   // �������˶�ʱ������
								{
										start_flag = 0;
										exerciseTime = 0;

									  keyLongPress = 1;
									
									  if(mode==1){
											display_exercise_time();// �˶�ʱ���������Ҫ������ʾ���˶�ʱ��
										}
								  	beep_remind(800);  // ����������
								}
								delay_ms(1);
						}
						if(keyPressTime < 500) // �̰�����ͣ/�����˶�ʱ��
						{
								start_flag = !start_flag;
							  beep_remind(90);  // ����������
						}
						keyPressTime = 0;
						keyLongPress = 0;
				}
				if(key_num == 5) // ��������״̬�£���������������л���ʾ����
				{
					  mode ++;
					  if(mode > 2)mode=0;
					  if(mode == 0)DisplayInitInter1(); // ʱ�ӱ��̽���
						if(mode == 1)DisplayInitInter2(); // ������ʾ����
						if(mode == 2)DisplayInitInter3(); // ����ʱ�Ӽ���γ�Ƚ���
				}

				if(key_num == 1)
				{
						display_main_menu();  // ��ʾ���˵�
				}
				Get_Ds1302_Time();  // ��ȡDS1302ʱ��
				Get_Steps();        // ��ȡ����
				Get_GPS();          // ��ȡ��γ��
				if(shuaxin == 1)    // ��ʱһ��ʱ���ȡ�¶Ⱥ�����Ѫ��
				{
					  shuaxin = 0;
					  temperature = ReadTemperature()*10;           // ��ȡ�¶�
					  max30102_read_data();                        // ��ȡ����Ѫ��ֵ
				}
        /*********************************************************************************/
				/*                                �ڱ���ʾ����                                   */
				/********************************************************************************/
        if(mode == 0)
				{
            if(SysDate.update_sec != SysDate.sec)  // ���뷢���仯ʱ������ʱ�ӽ���
						{
							  SysDate.week=Conver_week(SysDate.year,SysDate.mon,SysDate.day); // ��ȡ��������

								updateClock(SysDate.hour, SysDate.min, SysDate.sec);   // ���±���ʱ��
                
								switch(SysDate.week)  //��ʾ����
								{
										case 1:LCD_ShowString(134,3,(u8 *)"Mon",YELLOWGREEN,BLACK,12);break;
										case 2:LCD_ShowString(134,3,(u8 *)"Tue",YELLOWGREEN,BLACK,12);break;
										case 3:LCD_ShowString(134,3,(u8 *)"Wed",YELLOWGREEN,BLACK,12);break;
										case 4:LCD_ShowString(134,3,(u8 *)"Thu",YELLOWGREEN,BLACK,12);break;
										case 5:LCD_ShowString(134,3,(u8 *)"Fri",YELLOWGREEN,BLACK,12);break;
										case 6:LCD_ShowString(134,3,(u8 *)"Sat",YELLOWGREEN,BLACK,12);break;
										case 0:LCD_ShowString(134,3,(u8 *)"Sun",YELLOWGREEN,BLACK,12);break;
								}
								
								// ��ʾ����
								sprintf(display,"%02d/%02d",SysDate.mon,SysDate.day);
								LCD_ShowString(1,3,(u8 *)display,YELLOWGREEN,BLACK,12);    
							
							  if(step_num >= remind_step)
									display_format(0,118,step_num,12,RED,BLACK);           /*  ������꣬������ʾ��ɫ  */
								else
								  display_format(0,118,step_num,12,YELLOWGREEN,BLACK);   /*  ��ʾ����  */
								
								
								sprintf(display,"%.1f",(float)temperature/10);
								if(temperature<=temp_min||temperature>=temp_max)
								{
										LCD_ShowString(120,118,(u8 *)display,RED,BLACK,12);  /*  �¶Ȳ��ڷ�Χ��������ʾ��ɫ  */
										Gui_DrawFont_GBK12(148,116,RED,BLACK,(u8 *)"��");
								}
								else
								{
										LCD_ShowString(120,118,(u8 *)display,YELLOWGREEN,BLACK,12);  /*  ��ʾ�¶�  */
										Gui_DrawFont_GBK12(148,116,YELLOWGREEN,BLACK,(u8 *)"��");
								}
								
							  SysDate.update_sec = SysDate.sec;
						}
				}
				/*********************************************************************************/
				/*                                ��������ʾ����                                 */
				/*********************************************************************************/
				if(mode == 1)  
				{
					  if(step_num >= remind_step)
							display_format(121,49,step_num,16,RED,WHITE);    /*  ������꣬������ʾ��ɫ  */
						else
							display_format(121,49,step_num,16,GREE2,WHITE);   /*  ��ʾ����  */
						
					  if(update_eTime==1) // һ�������ʾ��������
						{
							  sprintf(display,"%03d",hrAvg);
							  if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))
								{
									  Gui_DrawFont_GBK16(5, 49, RED, WHITE,(u8 *)display); 	/*  ���ʲ��ڷ�Χ��������ʾ��ɫ  */
								}
								else
								{
										Gui_DrawFont_GBK16(5, 49, GREE2, WHITE,(u8 *)display); // ��ʾ����ֵ
                }
								sprintf(display,"%03d",spo2Avg);
								if((spo2Avg!=0)&&(spo2Avg<=spo2_min))
								{
										Gui_DrawFont_GBK16(48, 49, RED, WHITE,(u8 *)display);  /*  Ѫ�����ڷ�Χ��������ʾ��ɫ  */
								}
								else
								{
										Gui_DrawFont_GBK16(48, 49, GREE2, WHITE,(u8 *)display); // ��ʾѪ��ֵ
								}
								sprintf(display,"%.1f",(float)temperature/10);
								if(temperature<=temp_min||temperature>=temp_max)
								{
										Gui_DrawFont_GBK16(85, 49, RED, WHITE,(u8 *)display);   /*  �¶Ȳ��ڷ�Χ��������ʾ��ɫ  */
								}
								else
								{
										Gui_DrawFont_GBK16(85, 49, GREE2, WHITE,(u8 *)display);  // ��ʾ�¶�ֵ
								}
							
								display_exercise_time();  // ��ʾ�˶�ʱ��
							
							  update_eTime = 0;
						}
				}
				/*********************************************************************************/
				/*                            ������ʱ�Ӽ���γ����ʾ����                         */
				/*********************************************************************************/
				if(mode == 2) 
				{
						display_time(0,0,WHITE,timeBackground,WHITE,timeBackground); // ��ʾ����ʱ�ӣ�����������ʱ����
					
					  if(update_gps==1) // һ�������ʾ��γ����Ϣ
						{
								sprintf((char *)display,"%.6f",gps_lon);
							  LCD_ShowString(38,69,(u8 *)display,BLUE,WHITE,24);     /* ��ʾ���� */
							
								sprintf((char *)display,"%.6f",gps_lat);
							  LCD_ShowString(38,101,(u8 *)display,BLUE,WHITE,24);    /* ��ʾγ�� */
							
							  update_gps = 0;
						}
				}
				UsartSendReceiveData();			 //���ڷ��ͺͽ������ݣ����ں��ֻ�APPͨ��
		}	
}

void TIM3_IRQHandler(void)// ��ʱ��3�жϷ���������ڼ�¼ʱ��
{ 
	  static u16 timeCount1 = 0;
	  static u16 timeCount2 = 0;
	  static u16 timeCount3 = 0;
	  static bool flag1=0;
	  static bool alarm_flag1=0;
	
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���ָ����TIM�жϷ������:TIM �ж�Դ 
		{ 
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ  
				timeCount1 ++;
				if(timeCount1 >= 25) //500ms
				{
						timeCount1 = 0;
					  shuaxin = 1;
				}
				timeCount2 ++;
				if(timeCount2>=50)
				{
						timeCount2 = 0;
					  /* ����ʱ��99Сʱ��ת��Ϊ��������99x60x60=356400 */
					  if(exerciseTime<356400 && start_flag) exerciseTime ++;
					  if(StepRemindTime>0)StepRemindTime--;
					  update_eTime=1;
					  update_gps = 1;
						SendFlag = 1;
				}
				/*********************************************************************************/
				/*                            ����Ϊ��������ʾ����                               */
				/*********************************************************************************/
			  if(step_num >= remind_step)  // �������
				{
					  if(flag1==0)
						{
								tixing=1;
							  StepRemindTime=10;  // ������꣬����ʱ��Ϊ10��
						}
						flag1 = 1;
				}
				else
				{
						flag1 = 0;
				}
				
				if(((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))||((spo2Avg!=0)&&(spo2Avg<=spo2_min))||(temperature<=temp_min||temperature>=temp_max))//����Ѫ���¶Ȳ��ڷ�Χ
				{
						tixing = 2;
					  alarm_flag1=1;
				}
				else
				{
						if(alarm_flag1)
						{
							  tixing = 0;
								BEEP=0;
						}
						alarm_flag1=0;
				}
				if(tixing == 1)  // ����������ʾ��
				{
						timeCount3++;
					  if(timeCount3==1)BEEP=1;
					  if(timeCount3==5)BEEP=0;
					  if(timeCount3==9)BEEP=1;
					  if(timeCount3==14)BEEP=0;
					  if(timeCount3==18)BEEP=1;
					  if(timeCount3==23)BEEP=0;
					  if(timeCount3>=50)
						{
								timeCount3 = 0;
							  BEEP=0;
						}
						if(StepRemindTime==0){tixing=0;BEEP=0;}
				}
				if(tixing == 2) // ������Ѫ����Щ�������ڷ�Χ����ʾ��
				{
						timeCount3++;
					  if(timeCount3>=5)
						{
								BEEP = ~BEEP;
							  timeCount3 = 0;
						}
				}
	  }
}

