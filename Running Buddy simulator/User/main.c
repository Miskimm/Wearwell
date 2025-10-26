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

#define   ColorNum    13      //时钟背景颜色的数量

#define FLASH_SAVE_ADDR  ((u32)0x0800FF00)  				//设置FLASH 保存地址(必须为偶数)

#define STM32_RX1_BUF       Usart1RecBuf 					//串口1接收缓存数组
#define STM32_Rx1Counter    RxCounter             //串口1接收字节计数
#define STM32_RX1BUFF_SIZE  USART1_RXBUFF_SIZE    //串口1接收字节大小

#define STM32_RX3_BUF       Usart3RecBuf          //串口3接收缓存数组
#define STM32_Rx3Counter    Rx3Counter            //串口3接收字节计数
#define STM32_RX3BUFF_SIZE  USART3_RXBUFF_SIZE    //串口3接收字节大小

#define GPS_STR_LEN 48
GPS_INFO   GPS;      //GPS信息结构体

extern unsigned char rev_start;  //接收开始标志
extern unsigned char rev_stop;   //接收停止标志
extern unsigned char gps_flag;   //GPS处理标志
u8 GPS_rx_flag = 0;   //接收GPS数据标志

u8 key_num = 0;      //获取按键值的变量
u8 selectColor = 0;  //时间背景颜色

bool set_flag = 0;  //设置标志
bool InitDispalyFlag = 0;//初始显示标志
bool init_display_time=0;
bool shuaxin = 1;  //刷新标志
u8   mode = 0;    //显示模式
bool p_r = 0;	   //平年/润年  =0表示平年，=1表示润年
char display[16]; 

u16  step_num = 0;       //步数
u16  remind_step = 1000; //提醒步数

unsigned short temperature; //温度
u16 temp_max = 375;         //温度上限
u16 temp_min = 200;         //温度下限
u16 heart_rate_max = 120;   //心率上限
u16 heart_rate_min = 55;    //心率下限
u16 spo2_min = 80;         //血氧下限

float gps_lon,gps_lat;

u8 tixing=0;            //提醒标志
u8 StepRemindTime=0;    //步数达标，蜂鸣器提醒的时间

float adx,ady,adz;   //ADXL345三轴变量
float acc;

long exerciseTime = 0;   //运动时间
bool update_eTime = 1;  //更新运动时间
bool update_gps = 1;    //gps显示
bool start_flag = 0;   //开始运动计时标志
bool SendFlag = 0;     //发送数据标志
bool AppConnectState = 0; //连接状态

u16 timeBackground = 0;
u16 Color[ColorNum] = {
	      0xC0B0, //紫红色
	      0xFA20,  //橙色
				0xE000,  //红色
	      0xA145,  //褐色
	      0xFDE0,  //黄色   
	      0x0400,  //绿色
	      0xFB56,  //粉色
	      0x8010,  //紫色
	      0x001F,  //蓝色
	      0x4208,  //灰色
	      0x0000,  //黑色
	      0x49C0,  //咖啡色
	      0x50B,  //青色
};

long  t_eTime=0xFFFFFF;

void drawClockFace(void); //绘制表盘函数
void display_exercise_time(void);

//STM32 Flash写入数据函数
void STM32_FlashWriteData(void)  
{
		u16 temp_buf[9];
	  u8 add = 0;
	  
	  /* 将设置的参数以及上限下限值存储在单片机FLASH */
	
	  temp_buf[add ++] = heart_rate_max;  /* 存储心率上限 */
	  temp_buf[add ++] = heart_rate_min;  /* 存储心率下限 */
		temp_buf[add ++] = spo2_min;        /* 存储血氧下限 */
	  temp_buf[add ++] = temp_max;        /* 存储温度上限 */
	  temp_buf[add ++] = temp_min;        /* 存储温度下限 */
	  temp_buf[add ++] = remind_step;     /* 存储目标步数 */ 
	  temp_buf[add ++] = step_num;        /* 存储行走的步数 */
	  temp_buf[add ++] = selectColor;     /* 存储颜色选择的标志 */ 
	
	  STMFLASH_Write(FLASH_SAVE_ADDR + 0xC0,temp_buf,8); //存入数据
		delay_ms(10);
}

//STM32 Flash读出数据函数
void STM32_FlashReadData(void)  
{
		u16 temp_buf[9];
	  u8 add = 0;
	  
	  /* 将存储的数据读出 */
	  STM32F10x_Read(FLASH_SAVE_ADDR + 0xC0,temp_buf,8); //读出数据
	  
	  heart_rate_max = temp_buf[add ++];    /* 读出心率上限 */
	  heart_rate_min = temp_buf[add ++];    /* 读出心率下限 */
		spo2_min = temp_buf[add ++];          /* 读出血氧下限 */
	  temp_max = temp_buf[add ++];          /* 读出温度上限 */
	  temp_min = temp_buf[add ++];          /* 读出温度下限 */
	  remind_step = temp_buf[add ++];       /* 读出目标步数 */ 
	  step_num = temp_buf[add ++];          /* 读出行走的步数 */
	  selectColor = temp_buf[add ++];       /* 读出颜色选择的标志 */
	
		delay_ms(10);
}

// 检查是否是新的单片机，是的话需要存储初始值，否的话直接读出存储的值
void STM32_FlashCheck(void)  
{
	  u8 comper_str[6];
		
	  STM32F10x_Read(FLASH_SAVE_ADDR + 0xB0,(u16*)comper_str,5);
	  comper_str[5] = '\0';
	  if(strstr((char *)comper_str,"FDYDZ") == NULL)  //新的单片机
		{
			 STMFLASH_Write(FLASH_SAVE_ADDR + 0xB0,(u16*)"FDYDZ",5); //写入“FDYDZ”，方便下次校验
			 delay_ms(50);
			 STM32_FlashWriteData(); //写入初始数据
	  }
		STM32_FlashReadData();     //每次开机读出存储的数据
		timeBackground = Color[selectColor];//初始颜色
		delay_ms(100);
}

//显示时间函数
void display_time(u8 y,u8 pos,u16 color1,u16 color2,u16 color3,u16 color4)
{
	   if(init_display_time) //这里每次只需要显示一次
		 {
				 LCD_ShowChar(52,y,':',color1,color2,48);
				 Gui_DrawFont_GBK16(12,y+48, color1,color2,(u8 *)"    /  /  ");
				 //Gui_DrawFont_GBK16(104,y+48,color1,color2,(u8 *)"星期"); 
			   init_display_time = 0;
		 }
	
		/* 显示时间，当数据发生变化时才刷新显示 */
	  if(SysDate.update_hour != SysDate.hour)  //当小时发生变化的时候再显示小时
		{
			  SysDate.update_hour = SysDate.hour;
			  if(pos == 1)   //在设置状态下，选中小时字体颜色需要变化
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
	
	  if(SysDate.update_min != SysDate.min)  //当分钟发生变化的时候再显示分钟
		{
			  SysDate.update_min = SysDate.min;
			  if(pos == 2)    //在设置状态下，选中分钟字体颜色需要变化
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
	
	  if(SysDate.update_sec != SysDate.sec)  //当秒钟发生变化的时候再显示秒钟
		{
			  SysDate.update_sec = SysDate.sec;
			  if(pos == 3)        //在设置状态下，选中秒字体颜色需要变化
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
	  
		if(SysDate.update_year != SysDate.year)  //当年份发生变化的时候再显示年份
		{
			  SysDate.update_year = SysDate.year;
			  sprintf(display,"20%02d",SysDate.year);
			  if(pos == 4)       //在设置状态下，选中年字体颜色需要变化
				{
						Gui_DrawFont_GBK16(12,y+48,color3,color4,(u8 *)display);
				}
				else
				{
						Gui_DrawFont_GBK16(12,y+48,color1,color2,(u8 *)display);
				}
		}
		
		if(SysDate.update_mon != SysDate.mon)  //当月份发生变化的时候再显示月份
		{
			  SysDate.update_mon = SysDate.mon;
				sprintf(display,"%02d",SysDate.mon);
			  if(pos == 5)          //在设置状态下，选中月字体颜色需要变化
				{
						Gui_DrawFont_GBK16(52,y+48,color3,color4,(u8 *)display);
				}
				else
				{
						Gui_DrawFont_GBK16(52,y+48,color1,color2,(u8 *)display);
				}
		}
		
		if(SysDate.update_day != SysDate.day)  //当日期发生变化的时候再显示日期
		{
			  SysDate.update_day = SysDate.day;
				sprintf(display,"%02d",SysDate.day);
			  if(pos == 6)        //在设置状态下，选中日期字体颜色需要变化
				{
						Gui_DrawFont_GBK16(76,y+48,color3,color4,(u8 *)display);
				}
				else
				{
					  Gui_DrawFont_GBK16(76,y+48,color1,color2,(u8 *)display);
				}
		}

		SysDate.week=Conver_week(SysDate.year,SysDate.mon,SysDate.day); //读出星期数据
		
		if(SysDate.update_week != SysDate.week)  //当星期发生变化的时候再显示星期
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

//清除时间刷新标志，准备下一次的刷新显示
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

//获取DS1302时间函数
void Get_Ds1302_Time(void)
{
		DS1302_DateRead(&SysDate);//读时间
}

//显示初始界面函数（表盘时钟）
void DisplayInitInter1(void) 
{
	   Lcd_Clear(BLACK);  //屏幕填充为黑色
		 drawClockFace();   //绘制表盘刻度
		 
		 LCD_ShowPicture(5,100,15,15,gImage_step);
	   LCD_ShowPicture(135,100,14,14,gImage_temp);
	
		 init_display_time = 1;
}

//显示初始界面函数（显示数据界面）
void DisplayInitInter2(void)
{
	   Lcd_Clear(WHITE);  //屏幕填充为白色
	  
     t_eTime=0xFFFFFF;
	
	   LCD_ShowPicture(0,16,34,34,gImage_heart_2);      //显示心率图标
	   LCD_ShowPicture(42,16,34,34,gImage_spo2_2);      //显示血氧图标
	   LCD_ShowPicture(83,16,34,34,gImage_temp_2);      //显示温度图标
	   LCD_ShowPicture(125,16,33,33,gImage_step_2);     //显示步数图标
	  
	   LCD_ShowString(7,5,(u8 *)"BPM",GRAY2,WHITE,12);
     LCD_ShowString(49,5,(u8 *)"SPO2",GRAY2,WHITE,12);
     LCD_ShowString(90,5,(u8 *)"TEMP",GRAY2,WHITE,12);
	   LCD_ShowString(128,5,(u8 *)"STEPS",GRAY2,WHITE,12);
	
	   LCD_Fill(0,67,160,129,timeBackground);   //区域填充背景色
	   Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");
}

//显示初始界面函数（数字时钟）
void DisplayInitInter3(void) 
{
	   Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	   
	   clear_time_data();
	
	   LCD_Fill(0,0,160,64,timeBackground); //填充时钟背景颜色
	
	   Gui_DrawFont_GBK12(2, 75,    GRAY2, WHITE,(u8 *) "Lon:");
	   Gui_DrawFont_GBK12(2, 108,   GRAY2, WHITE,(u8 *) "Lat:");
		 init_display_time = 1;
}

//蜂鸣器提示函数
void beep_remind(u16 ms) 
{
		BEEP = 1;
		delay_ms(ms);
		BEEP = 0;
}

//接收手机端设置的参数
bool ReceiveSetValue_1(char *uart_buf,char *str,u16 *data) 
{
		u16  setValue=0;
	  char *buf = str;
	  char *str1=0,i;
	  char setvalue[6]={0};
	
		if(strstr(uart_buf,buf) == NULL) //没有接收到数据
		{
				return  1;
		}
	  else
		{
				str1 = strstr(uart_buf,buf);  //接收到数据
				
				while(*str1 < '0' || *str1 > '9')    //判断是不是0到9有效数字
				{
						str1 = str1 + 1;
						delay_ms(10);
				}
				i = 0;
				while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
				{
						setvalue[i] = *str1;
						i ++; str1 ++;
						if(*str1 == ',')break;            //换行符，直接退出while循环
						delay_ms(10);
				}
				setvalue[i] = '\0';            //加上结尾符
				setValue = atoi(setvalue);
				*data = setValue;
		}	
		return  0;
}

//接收手机端设置的参数
bool ReceiveSetValue_2(char *uart_buf,char *str,float *data) 
{
	  char *buf = str;
	  char *str1=0,i;
	  char setvalue[6]={0};
	
		if(strstr(uart_buf,buf) == NULL) //没有接收到数据
		{
				return  1;
		}
	  else
		{
				str1 = strstr(uart_buf,buf);  //接收到数据
				
				while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
				{
						str1 = str1 + 1;
						delay_ms(10);
				}
				i = 0;
				while((*str1 >= '0' && *str1 <= '9') || (*str1 == '.'))        //判断是不是0到9有效数字
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
				setvalue[i] = '\0';                //加上结尾符
				*data = Str_To_Float(setvalue);    //将字符串转换为浮点数
		}	
		return  0;
}

extern bool usart_rx_stop;
void UsartSendReceiveData(void) //串口发送和接收数据，用于和手机APP通信
{
		char *str1=0,i;
	  u16   int_value=0;
	  float float_value=0.0;
	  int  setValue=0;
	  char setvalue[3]={0};
	  char SEND_BUF[180];
	
	  while(usart_rx_stop == 1)  // 接收到APP下发的数据
		{
			  if(strstr(STM32_RX1_BUF,"SUCCESS")!=NULL) //蓝牙断开了
				{
						AppConnectState = 0; //连接标志置0
				}
			  /*   校准时间的指令    */
			  /*   手机下发的指令是year:0000/00/00   */
			  if(strstr(STM32_RX1_BUF,"year:")!=NULL)  //接收到设置年月日的指令
				{
					  str1 = strstr(STM32_RX1_BUF,"year:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;             //指向下一个字符
								delay_ms(1);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;                  //将0~9的字符存入数组
								i ++; str1 ++;  
                if(*str1 == '/')break; 							
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);    //将字符串转换为整形数据
						if((setValue%100)>=0 && (setValue%100)<=99)
						{
								SysDate.year = setValue%100;    //时间的年
						}
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;    
                if(*str1 == '/')break; 								
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符

						setValue = atoi(setvalue);
						if(setValue>=1 && setValue<=12)
						{
								SysDate.mon = setValue;   //时间的月
						}
						
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == ',')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符

						setValue = atoi(setvalue);
						if(setValue>=1 && setValue<=31)
						{
								SysDate.day = setValue;         //设置日
                DS1302_DateSet(&SysDate);		    //设置时间	
                if(set_flag==1)InitDispalyFlag = 1;							
						}
				}
			
			  /*   手机下发的指令是hour:00:00:00   */
				if(strstr(STM32_RX1_BUF,"hour:")!=NULL)  //接收到设置时分秒的指令
				{
					  str1 = strstr(STM32_RX1_BUF,"hour:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;                  //指向下一个字符
								delay_ms(1);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == ':')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=23)
						{
								SysDate.hour = setValue;   //时间的小时
						}
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == ':')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符

						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=59)
						{
								SysDate.min = setValue;   //时间的分钟
						}
						
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
							  if(*str1 == '\r')break;         
								delay_ms(1);
						}
						setvalue[i] = '\0';            //加上结尾符

						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=59)
						{
								SysDate.sec = setValue;     //时间的秒
							  DS1302_DateSet(&SysDate);   //设置时间
							  if(set_flag==1)InitDispalyFlag = 1;
						}
						beep_remind(90);  //蜂鸣器提示音
				}
				/*   设置心率下限的指令   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"hear_min:",&int_value))==0)
				{
					  beep_remind(90);  //蜂鸣器提示音
					  if(int_value <= 200)
						{
								heart_rate_min = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   设置心率上限的指令   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"hear_max:",&int_value))==0)
				{
					  if(int_value < 200)
						{
								heart_rate_max = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   设置血氧下限的指令   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"spo_min:",&int_value))==0)
				{
					  if(int_value < 200)
						{
								spo2_min = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   设置目标步数的指令   */
			  if((ReceiveSetValue_1(STM32_RX1_BUF,"set_step:",&int_value))==0)
				{
					  if(int_value < 60000)
						{
								remind_step = int_value;
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   设置温度下限的指令   */
			  if((ReceiveSetValue_2(STM32_RX1_BUF,"temp_min:",&float_value))==0)
				{
					  if(float_value < 99.9)
						{
								temp_min = (u16)(float_value*10);
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   设置温度上限的指令   */
			  if((ReceiveSetValue_2(STM32_RX1_BUF,"temp_max:",&float_value))==0)
				{
					  if(float_value < 99.9)
						{
								temp_max = (u16)(float_value*10);
							  if(set_flag==1)InitDispalyFlag = 1;
						}
				}	
				/*   切换颜色指令   */
				if(strstr(STM32_RX1_BUF,"SwitchColor")!=NULL)
				{
					  beep_remind(90);  //蜂鸣器提示音
						selectColor ++;
						if(selectColor >= ColorNum)selectColor = 0;
						timeBackground = Color[selectColor]; //选择背景颜色
						if(set_flag==0)
						{
							  if(mode == 0)drawClockFace();   //绘制表盘刻度
							  if(mode == 1)
								{
										LCD_Fill(0,67,160,129,timeBackground);   //区域填充背景色
									  LCD_Fill(0,67,160,129,timeBackground);   //区域填充背景色
									  Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");

										t_eTime=0xFFFFFF;
								}
								if(mode == 2)
								{
										clear_time_data();
										LCD_Fill(0,0,160,64,timeBackground); //填充时钟背景颜色
										init_display_time = 1;
								}
						}
						else
						{
								InitDispalyFlag = 1;
						}
						STM32_FlashWriteData(); // 存储数据
				}	
				/*   切换显示的界面指令   */
				if(strstr(STM32_RX1_BUF,"SwitchDisplay")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //蜂鸣器提示音
								mode ++;
								if(mode > 2)mode=0;
								if(mode == 0)DisplayInitInter1(); // 时钟表盘界面
								if(mode == 1)DisplayInitInter2(); // 数据显示界面
								if(mode == 2)DisplayInitInter3(); // 数字时钟及经纬度界面
						}
				}
				/*   开始和暂停的指令   */
				if(strstr(STM32_RX1_BUF,"start")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //蜂鸣器提示音
								start_flag = !start_flag;
						}
				}
				/*   步数清零的指令   */
				if(strstr(STM32_RX1_BUF,"CleanStep")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //蜂鸣器提示音
								step_num = 0;  //步数清零
								STM32_FlashWriteData(); // 存储数据
								StepRemindTime=0;
						}
				}
				/*   运动时间清零的指令   */
				if(strstr(STM32_RX1_BUF,"CleanTime")!=NULL)
				{
						if(set_flag==0)
						{
							  beep_remind(90);  //蜂鸣器提示音
								start_flag = 0;
								exerciseTime = 0;

								if(mode==1){
									display_exercise_time();// 运动时间清零后需要更新显示下运动时间
								}
						}
				}
				usart_rx_stop = 0; // 接收完成标志清零。准备下一次的接收
				/* 在串口中断接收数据之前关闭了定时器中断，串口数据接收完成后，定时器继续开启工作 */
				TIM_Cmd(TIM3,ENABLE);
				TIM_ITConfig( TIM3,TIM_IT_Update ,ENABLE );
		}
		if(AppConnectState == 1)
		{
				if(SendFlag == 1)    //1秒钟上传一次数据
				{
						SendFlag = 0;		
						 
						UsartSendBufClear(sizeof(SEND_BUF),SEND_BUF,BLUETOOTHADDR);  //清除发送缓存
					  sprintf(SEND_BUF+strlen("SEND"),"hear:%d#,spo2:%d#,temp:%.1f#,step:%d#,time:%02d:%02d:%02d#,lon:%.6f#,lat:%.6f#",hrAvg,spo2Avg,(float)temperature/10,step_num,(int)(exerciseTime/3600),(int)(exerciseTime%3600/60),(int)(exerciseTime%3600%60),gps_lon,gps_lat);/*发送数据装载*/
						
						if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))strcat(SEND_BUF,"hear_warn,"); // 心率不在范围，发送超标指令，手机端接收到以后字体会显示红色
            if((spo2Avg!=0)&&(spo2Avg<=spo2_min))strcat(SEND_BUF,"spo2_warn,"); // 血氧不在范围，发送超标指令，手机端接收到以后字体会显示红色
						if(temperature<=temp_min||temperature>=temp_max)strcat(SEND_BUF,"temp_warn,"); // 温度不在范围，发送超标指令，手机端接收到以后字体会显示红色
						if(step_num >= remind_step)strcat(SEND_BUF,"step_warn,"); // 步数达标，手机端接收到以后字体会显示红色       
						
						strcat(SEND_BUF,"\r\n");
						uart1_send((u8 *)SEND_BUF, 0xFF);  //将数据上传至APP端
				}
		}
		else
		{
				SendFlag = 0;		
		}
}

//获取按键值函数
u8 keyscan(void)   
{
	  u8 key = 0;
	
		if(KEY1 == 0)   //第一个按键按下返回键值1
		{
		  	delay_ms(1); 
			  if(KEY1 == 0)
				{
					  while(KEY1 == 0);
						key = 1;
				}
		}
		if(KEY2 == 0)  //第二个按键按下返回键值2
		{
		  	delay_ms(80); 
			  if(KEY2 == 0)
				{
						key = 2;
				}
		}
		if(KEY3 == 0) //第三个按键按下返回键值3
		{
		  	delay_ms(80); 
			  if(KEY3 == 0)
				{
						key = 3;
				}
		}
		if(KEY4 == 0) //第四个按键按下返回键值4
		{
		  	delay_ms(1);
			  if(KEY4 == 0)
				{
					  //while(KEY4 == 0);
						key = 4;
				}
		}
		if(KEY5 == 0) //第五个按键按下返回键值5
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
					                      以下为液晶菜单界面显示部分
***************************************************************************************************************/

#define x            2    //设置的第一行起始位置x
#define y            21   //设置的第一行起始位置y
#define f_size       16   //字体大小
#define interval     2    //每行间隔的像素

int display_menu5(void)//显示设置步数界面
{
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(28, 2, WHITE, RED, (u8 *)"Set Step Goal");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
				if(key_num == 2)    //第二个按键按下，是下翻
				{
						if(remind_step<60000)remind_step++;
						InitDispalyFlag = 1;
				}
				if(key_num == 3)   //第三个按键按下，是上翻
				{
					  if(remind_step>0)remind_step--;
					  InitDispalyFlag = 1;
				}
				if(key_num == 4)  //返回上一级
				{
						Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(InitDispalyFlag)
				{
					  sprintf((char *)display,"%05d",remind_step);
						LCD_ShowString(40,60,(u8 *)display,BLACK,WHITE,24); /*  显示步数设置值  */
					  InitDispalyFlag = 0;
				}
		}
}

int display_menu4(void)//显示设置温度界面
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //下翻标志
	  bool set_flag=0;     //设置标志
	  unsigned char  row = 2; //每页最多显示的行数
	  u16 color[2][2];//颜色数组
	  char dat1[5]={'0','0','.','0','\0'};
    char dat2[5]={'0','0','.','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:"},    /*  显示的字符界面 */
				{"2.Up:"},    /*  显示的字符界面 */
};
	
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(20, 2, WHITE, RED, (u8 *)"Set Temperature");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
			  if(key_num == 1)   //第一个按键按下。是往下翻
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //第二个按键按下。是加键
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
				if(key_num == 3)   //第三个按键按下。是减键
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
				if(key_num == 4)  //返回上一级
				{
						Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  初始化字体颜色和字体背景颜色  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  当按上行或者下行的按键时字体颜色和字体背景颜色需要发生变化  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* 选中选项的颜色和未选择选中选项的颜色处理 */
								/*  未选中的颜色处理  */
								/*  每次下翻时，上一行需要显示白色  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //显示选中行颜色
					  }
						
						/* 温度下限值装载 */
						dat1[0] = temp_min/100+'0';
						dat1[1] = temp_min%100/10+'0';
						dat1[3] = temp_min%10+'0';
						/* 温度上限值装载 */
						dat2[0] = temp_max/100+'0';
						dat2[1] = temp_max%100/10+'0';
						dat2[3] = temp_max%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								/* 显示主信息 */
							  for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //显示第一页的信息
								}	
						}
						else
						{
								/* 当每次上翻或者下翻时，不需要刷新每行的显示，这样容易导致刷新变慢。 只需要显示刷新相连的两行字符即可*/
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
								/* 单独显示设置的值 */
								if(index==1)
							  {
									  Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
										Gui_DrawFont_GBK12(105,y + 0*(f_size + interval)+1, color[0][0], color[0][1], (u8 *)"℃");
								}
								if(index==2)
								{
										Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);	
									  Gui_DrawFont_GBK12(105,y + 1*(f_size + interval)+1, color[1][0], color[1][1], (u8 *)"℃");
								}
						}
						else
						{
								Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
							  Gui_DrawFont_GBK12(105,y + 0*(f_size + interval)+1, color[0][0], color[0][1], (u8 *)"℃");
								Gui_DrawFont_GBK16(72, y + 1*(f_size + interval), color[1][0], color[1][1], (u8 *)dat2);
							  Gui_DrawFont_GBK12(105,y + 1*(f_size + interval)+1, color[1][0], color[1][1], (u8 *)"℃");
						}
						InitDispalyFlag = 0;
						set_flag = 0;
						down_flag= 0;
				}
		}
}

int display_menu3(void)//显示设置血氧界面
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //下翻标志
	  bool set_flag=0;     //设置标志
	  unsigned char  row = 1; //每页最多显示的行数
	  u16 color[1][2];//颜色数组
	  char dat1[3]={'0','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:      %"},    /*  显示的字符界面 */
};
	
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(20, 2, WHITE, RED, (u8 *)"Set Blood Oxygen");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
			  if(key_num == 1)   //第一个按键按下。是往下翻
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //第二个按键按下。是加键
				{
					  if(index==1)
						{
							  if(spo2_min<200)spo2_min++;
							  set_flag = 1;
						}
				}
				if(key_num == 3)   //第三个按键按下。是减键
				{
					  if(index==1) 
						{
							  if(spo2_min>0)spo2_min--;
							  set_flag=1;
						}
				}
				if(key_num == 4)  //返回上一级
				{
						Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  初始化字体颜色和字体背景颜色  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  当按上行或者下行的按键时字体颜色和字体背景颜色需要发生变化  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* 选中选项的颜色和未选择选中选项的颜色处理 */
								/*  未选中的颜色处理  */
								/*  每次下翻时，上一行需要显示白色  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //显示选中行颜色
					  }
						
						 /* 血氧下限值装载 */
						 dat1[0] = spo2_min/100+'0';
						 dat1[1] = spo2_min%100/10+'0';
						 dat1[2] = spo2_min%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								Gui_DrawFont_GBK16(x, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)display_[0].str);
						}
						
						/* 单独显示设置的值 */
						Gui_DrawFont_GBK16(72, y + 0*(f_size + interval), color[0][0], color[0][1], (u8 *)dat1);
						
						InitDispalyFlag = 0;
						set_flag = 0;
						down_flag= 0;
				}
		}
}

int display_menu2(void)//显示设置心率界面
{
	  unsigned char index=1,i;
	  bool down_flag = 0;  //下翻标志
	  bool set_flag=0;     //设置标志
	  unsigned char  row = 2; //每页最多显示的行数
	  u16 color[2][2];//颜色数组
	  char dat1[4]={'0','0','0','\0'};
    char dat2[4]={'0','0','0','\0'};
	  
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Low:"},    /*  显示的字符界面 */
				{"2.Up:"},    /*  显示的字符界面 */
};
	
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(28, 2, WHITE, RED, (u8 *)"Set Heart Rate");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
			  if(key_num == 1)   //第一个按键按下。是往下翻
				{
						index ++;
					  if(index>row)index=1;
					  down_flag = 1;
				}
				if(key_num == 2)   //第二个按键按下。是加键
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
				if(key_num == 3)   //第三个按键按下。是减键
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
				if(key_num == 4)  //返回上一级
				{
						Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(down_flag || set_flag || InitDispalyFlag)
				{
						/*  初始化字体颜色和字体背景颜色  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
						/*  当按上行或者下行的按键时字体颜色和字体背景颜色需要发生变化  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						if(set_flag==0)
						{
								/* 选中选项的颜色和未选择选中选项的颜色处理 */
								/*  未选中的颜色处理  */
								/*  每次下翻时，上一行需要显示白色  */
								if(down_flag==1)
								{
										if(index==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
										else        LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);
								}
								LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //显示选中行颜色
					  }
						
						/* 心率下限值装载 */
						dat1[0] = heart_rate_min/100+'0';
						dat1[1] = heart_rate_min%100/10+'0';
						dat1[2] = heart_rate_min%10+'0';
						/* 心率上限值装载 */
						dat2[0] = heart_rate_max/100+'0';
						dat2[1] = heart_rate_max%100/10+'0';
						dat2[2] = heart_rate_max%10+'0';
						
						if(InitDispalyFlag == 1) 
						{
								/* 显示主信息 */
							  for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //显示第一页的信息
								}	
						}
						else
						{
								/* 当每次上翻或者下翻时，不需要刷新每行的显示，这样容易导致刷新变慢。 只需要显示刷新相连的两行字符即可*/
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
								/* 单独显示设置的值 */
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

int display_menu1(void)//显示时间设置界面
{
	  unsigned char index=1;
    u16 nian_temp;
	
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Set Time");
	  InitDispalyFlag = 1;
	  clear_time_data();
	  init_display_time = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
			  if(key_num == 1)   //第一个按键按下。是往下翻
				{
						index ++;
					  if(index>6)index=1;
					  nian_temp=2000+SysDate.year;
						if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //判断是否为闰年
								p_r=1;
						else
								p_r=0;
					  clear_time_data();
					  InitDispalyFlag = 1;
				}
				if(key_num == 2)   //第二个按键按下。是加键
				{
						 if(index == 1)//设置时
						 {
								 SysDate.hour++;
								 if(SysDate.hour == 24)//到0点
								 SysDate.hour = 0;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else  if(index == 2)//设置分
						 {
								 SysDate.min++;
								 if(SysDate.min == 60)//60分钟
								 SysDate.min = 0;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else  if(index == 3)//设置秒
						 {
								 SysDate.sec++;
								 if(SysDate.sec == 60)//60秒
								 SysDate.sec = 0;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else if(index == 4)//设置年
						 {
								 SysDate.year++;
								 if(SysDate.year == 100)SysDate.year = 0;//最大到99
							 
								 DS1302_DateSet(&SysDate);//设置时间
							 
								 nian_temp=2000+SysDate.year;
								if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //判断是否为闰年
										p_r=1;
								else
										p_r=0;
						 }
						 else if(index == 5)//设置月
						 {
								SysDate.mon++;
								if(SysDate.mon==13)//最大12个月
								SysDate.mon=1;
								if((SysDate.mon==4)||(SysDate.mon==6)||(SysDate.mon==9)||(SysDate.mon==11))//4，6，9，11月只有30天
								{
										if(SysDate.day>30)
												SysDate.day=1;
								}
								else
								{
										if(SysDate.mon==2)//2月
										{
												if(p_r==1)//闰年2月有29天
												{
														if(SysDate.day>29)
																SysDate.day=1;
												}
												else//平年2月只有28天
												{
														if(SysDate.day>28)
																SysDate.day=1;
												}
										}
								}
								DS1302_DateSet(&SysDate);//设置时间
						 }
						 else if(index == 6)//设置日
						 {
								 SysDate.day++;
									if((SysDate.mon==1)||(SysDate.mon==3)||(SysDate.mon==5)||(SysDate.mon==7)||(SysDate.mon==8)||(SysDate.mon==10)||(SysDate.mon==12))//有31天的月份
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
															if(SysDate.day==30)//闰年2月有29天
																	SysDate.day=1;
													}
													else
													{
															if(SysDate.day==29)//平年2月只有28天
																	SysDate.day=1;
													}
											}
											else
											{
													if(SysDate.day==31)//最大到30 
															SysDate.day=1;
											}
									}
									DS1302_DateSet(&SysDate);//设置时间
						 }
						 InitDispalyFlag = 1;
				}
				if(key_num == 3)   //第三个按键按下。是减键
				{
					   if(index == 1)//设置时
						 {
								 if(SysDate.hour == 0)SysDate.hour = 24;
								 SysDate.hour--;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else  if(index == 2)//设置分
						 {
								 if(SysDate.min == 0)SysDate.min = 60;
								 SysDate.min--;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else  if(index == 3)//设置秒
						 {
								 if(SysDate.sec == 0)SysDate.sec = 60;
								 SysDate.sec--;
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 else  if(index == 4)//设置年
						 {
								 if(SysDate.year == 0)SysDate.year = 100;
								 SysDate.year--;
								 DS1302_DateSet(&SysDate);//设置时间
								 
								 nian_temp=2000+SysDate.year;
								if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //判断是否为闰年
										p_r=1;
								else
										p_r=0;
						 }
						 else if(index == 5)//设置月
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
								DS1302_DateSet(&SysDate);//设置时间
						 }
						 else if(index == 6)//设置日
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
								 DS1302_DateSet(&SysDate);//设置时间
						 }
						 InitDispalyFlag = 1;
				}
				if(key_num == 4)  //返回上一级
				{
						Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
						LCD_Fill(0,0,160,18,RED);
						Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
						InitDispalyFlag = 1;
						key_num = 0;
						return 0;
				}
				if(InitDispalyFlag)
				{
					  display_time(40,index,BLACK,WHITE,WHITE,timeBackground); //显示时间
						InitDispalyFlag = 0;
				}
		}
}

int display_main_menu(void) //显示主菜单界面
{	  
	  unsigned char index=1,i;
	  bool  up_flag  = 0;  //上翻标志
	  bool down_flag = 0;  //下翻标志
	  unsigned char  row = 5;   //每页最多显示的行数
	  u16 color[5][2];//颜色数组
	
struct info {
    char *str; 
};
struct  info  display_[] = {
				{"1.Set Time"},    /*  显示的字符界面 */
				{"2.Set Heart Rate"},
				{"3.Set Blood Oxygen"},
				{"4.Set Temperature"},
				{"5.Set Step Goal"},
};

	  set_flag = 1;
		Lcd_Clear(WHITE);   //清屏，屏幕填充为白色
	  LCD_Fill(0,0,160,18,RED);
	  Gui_DrawFont_GBK16(48, 2, WHITE, RED,(u8 *)"Settings");
	  InitDispalyFlag = 1;
		while(1)
		{
			  UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
				key_num = keyscan();  //获取按键值
			  if(key_num == 1)     //第一个按键按下，是确定
				{
						if(index==1)display_menu1();  //选择进入时间设置界面
					  if(index==2)display_menu2();  //选择进入设置心率界面
					  if(index==3)display_menu3();  //选择进入设置血氧界面
					  if(index==4)display_menu4();  //选择进入设置温度界面
					  if(index==5)display_menu5();  //选择进入设置步数界面
				}
				if(key_num == 2)    //第二个按键按下，是下翻
				{
						index ++;
					  down_flag = 1;
					  if(index>row)
						{
								index=1;
						}
				}
				if(key_num == 3)   //第三个按键按下，是上翻
				{
					  index --;
					  up_flag  = 1;
					  if(index==0)
					  {
							  index=row;
						}
				}
				if(key_num == 4) //退出设置
				{
					  if(mode == 0)DisplayInitInter1(); //时钟表盘界面
						if(mode == 1)DisplayInitInter2(); //数据显示界面
						if(mode == 2)DisplayInitInter3(); //数字时钟及经纬度界面
					
						STM32_FlashWriteData(); //退出设置以后，先存储下设置的数据
					  set_flag = 0; //设置标志清零
					  return 0;
				}
				if(down_flag || up_flag || InitDispalyFlag)
				{
						/*  初始化字体颜色和字体背景颜色  */
					  for(i = 0;i < row; i ++)
					  {
								color[i][0]=BLACK;
							  color[i][1]=WHITE;
						}
					  
						/*  当按上行或者下行的按键时字体颜色和字体背景颜色需要发生变化  */
						color[index-1][0]=WHITE;color[index-1][1]=BLUE;
						
						/* 选中选项的颜色和未选择选中选项的颜色处理 */
						/*  未选中的颜色处理  */
						/*  每次下翻时，上一行需要显示白色  */
						if(down_flag==1)
						{
								if(index%row==1)LCD_Fill(0,y + (row-1)*(f_size + interval),X_MAX_PIXEL,(y + (row-1)*(f_size + interval))+f_size,WHITE); 
								else 
								{		
								  	LCD_Fill(0,y + (index-1-1)*(f_size + interval),X_MAX_PIXEL,((y + f_size) + (index-1-1)*f_size)+(index-1)*interval,WHITE);	
								}
						}
						/*  每次上翻时，下一行需要显示白色  */
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
						
						/*  选中的颜色处理  */
						LCD_Fill(0,y + (index-1)*(f_size + interval),X_MAX_PIXEL,((y+f_size) + (index-1)*f_size)+(index-1)*interval,BLUE); //显示选中行颜色
						
						if(InitDispalyFlag)
						{
								/* 显示主信息 */
								for(i = 0;i < row; i ++) 
								{
										Gui_DrawFont_GBK16(x, y + i*(f_size + interval), color[i][0], color[i][1], (u8 *)display_[i].str);    //显示第一页的信息
								}
						}
						else
						{
								/* 当每次上翻或者下翻时，不需要刷新每行的显示，这样容易导致刷新变慢。 只需要显示刷新相连的两行字符即可*/
								if(down_flag == 1)   //当下翻时，只需要刷新当前选中的这一行和上一行
								{
										if(index%row==1)  //第一行
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
								if(up_flag==1) //当上翻时，只需要刷新当前选中的这一行和下一行
								{
										if(index==row)  //第最后一行
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
						down_flag = 0;  //下翻标志清零
						up_flag   = 0;  //上翻标志清零
						InitDispalyFlag = 0;
				}
		}
}

/*****************************************************************************************************************************
					                                              结束
******************************************************************************************************************************/

// 获取步数函数
void Get_Steps(void)
{
	  static u16 temp=0;
	  static bool flag=0;
	
	  adxl345_read_average(&adx,&ady,&adz,10);// 获取ADXL345加速度数据
		acc=ady;  // 获取y轴加速度值，用于步数计算
	
		if(acc>0)// 在正轴
		{
			if(acc/10>=4&&flag==1)// 加速度值，在正轴值是否大于4，并且flag为1，则视为一个周期完成，步数加1
			{
				flag=0;	
				if(step_num<60000)step_num++;	// 步数加1
				if(temp!=step_num)// 当步数发生变化才去存储步数
				{
						temp=step_num;
					  STM32_FlashWriteData(); // 存储数据
				}
			}
		}
		if(acc<0)//在负轴
		{
			acc=-acc;
			if(acc/10>=4)// 加速度值，在负轴是否小于-4
			{
					flag=1;// falg置1
			}
		}
}

//显示格式函数
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
				 Gui_DrawFont_GBK16(x1, y1, Color1, Color2,(u8 *)display);//显示值
		}
		else
		{
				LCD_ShowString(x1,y1,(u8 *)display,Color1,Color2,size);   //显示值
		}
}

//显示运动的时间函数
void display_exercise_time(void)  
{
	  if(t_eTime!=(exerciseTime%3600/60)) /*运动时间的小时和分钟显示字体比较大，只有当数据变化的时候再显示*/
		{
	      t_eTime = (exerciseTime%3600/60);
				sprintf(display,"%02d:%02d",(int)(exerciseTime/3600),(int)(exerciseTime%3600/60));
				LCD_ShowString(6,84,(u8 *)display,WHITE,timeBackground,48);  /*  显示运动时间的小时和分钟  */
		}
	  sprintf(display,"%02d",(int)(exerciseTime%3600%60));
	  LCD_ShowString(128,103,(u8 *)display,WHITE,timeBackground,24); /*  显示运动时间秒钟  */
}

// 获取GPS数据函数
void Get_GPS(void)
{
	  static u8 timeCount=0;
	
		if(rev_stop == 1 && timeCount++ >= 20)   //如果接收完一行
		{
				if (GPS_RMC_Parse(STM32_RX3_BUF, &GPS)) //解析GPRMC
				{
						gps_flag = 0;
						rev_stop  = 0;
					  gps_lon = (float)GPS.longitude_Degree;  // 经度
					  gps_lat = (float)GPS.latitude_Degree;   // 纬度
				}
				else
				{
						gps_flag = 0;
						rev_stop  = 0;
				}
				timeCount=0;
		}
}

#define CENTER_X  (X_MAX_PIXEL/2)  // 表盘中心x
#define CENTER_Y  (Y_MAX_PIXEL/2)  // 表盘中心y
#define RADIUS 62 // 圆的半径

struct info {
    char *str; 
};
struct info scale[12] = {
				{"12"},{"1"},{"2"},{"3"},{"4"},{"5"},{"6"},{"7"},{"8"},{"9"},{"10"},{"11"},     
};

// 绘制表盘刻度函数
void drawClockFace(void) 
{
	  int i=0,x1,x2,y1,y2;
	  int len;
	  u16 temp_color = 0;
	
	  if(timeBackground==BLACK)temp_color=GRAY0;else temp_color=timeBackground;
	
	  /* 画圆 */
    Gui_Circle(CENTER_X, CENTER_Y, RADIUS, temp_color);
	  Gui_Circle(CENTER_X, CENTER_Y, RADIUS-1, temp_color);
    Gui_Circle(CENTER_X, CENTER_Y, RADIUS-2, GRAY1);
    
    // 绘制刻度
    for(i = 0; i < 60; i ++)
  	{
        float angle = i * 6 * 3.14 / 180;   // 每6°为一刻度
        len = (i%5 == 0) ? 6 : 1;           // 每逢5格为长刻度，其他的都是短刻度
			
			  /* 根据任意三角函数sinθ= y/r，cosθ= x/r，计算刻度坐标 */
        x1 = CENTER_X + (RADIUS-5)*sin(angle);
        y1 = CENTER_Y - (RADIUS-5)*cos(angle);
        x2 = CENTER_X + (RADIUS-5-len)*sin(angle);
        y2 = CENTER_Y - (RADIUS-5-len)*cos(angle);
			
			  if(i%15 == 0)LCD_DrawLine(x1, y1, x2, y2, RED);else LCD_DrawLine(x1, y1, x2, y2, WHITE); // 画线函数画出刻度
			
			  if(i%5 == 0)// 每逢5的刻度
				{
						switch(i)
						{  
  							/* 表盘数字刻度显示 */
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

// 绘制表盘指针函数
void drawHand(uint16_t x0, uint16_t y0, uint16_t length, float angle, uint16_t color) 
{
	  uint16_t x1,y1;
	  /*
			任意三角函数sinθ= y/r,这里θ指的是弧度，弧度=角度*π/180，例如sin(30°)在程序中应该写成sin(30*3.14/180)
	    任意三角函数cosθ= x/r,这里θ指的是弧度，弧度=角度*π/180，例如cos(30°)在程序中应该写成cos(30*3.14/180)
	    r为圆的半径，在表盘里也就是时分秒针的长度
	  */
    x1 = x0 + length * sin((angle)*3.14/180.0);  
    y1 = y0 - length * cos((angle)*3.14/180.0);
	  
    LCD_DrawLine(x0, y0, x1, y1, color);  // 将原点坐标和交点坐标画线
}

// 更新表盘时钟函数
void updateClock(uint8_t h, uint8_t m, uint8_t s) 
{
	  static u16 hourHand_angle=0;
	  static u16 minuteHand_angle=0;
	  static u16 secondHand_angle=0;
	
    // 清除旧指针
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, BLACK);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle, BLACK);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle, BLACK);

    // 计算新角度
    hourHand_angle = h * 30 + m * 0.5;    // 每小时30度，每分钟0.5度
    minuteHand_angle = m * 6;             // 每分钟6度
    secondHand_angle = s * 6;             // 每秒6度
	
	  // 绘制新指针
	  drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, WHITE);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle,VIOLETRED);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle,GREEN);
		
		// 显示血氧
	  LCD_ShowPicture(44,54,13,13,gImage_spo2);
		sprintf(display,"%03d",spo2Avg);
		if((spo2Avg!=0)&&(spo2Avg<=spo2_min))
		{
			  LCD_ShowString(42,68,(u8 *)display,RED,BLACK,12);   // 血氧不在范围，字体显示红色
		}
		else
		{
	    	LCD_ShowString(42,68,(u8 *)display,GRAY0,BLACK,12);  // 显示血氧
		}
		
		// 显示心率
	  LCD_ShowPicture(100,54,14,14,gImage_heart);
		sprintf(display,"%03d",hrAvg);
		if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))
		{
				LCD_ShowString(98,68,(u8 *)display,RED,BLACK,12);    // 心率不在范围，字体显示红色
		}
	  else
		{
				LCD_ShowString(98,68,(u8 *)display,GRAY0,BLACK,12);  // 显示心率
		}
		
    // 绘制新指针
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 20, hourHand_angle, WHITE);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 30, minuteHand_angle,VIOLETRED);
    drawHand(X_MAX_PIXEL/2, Y_MAX_PIXEL/2, 38, secondHand_angle,GREEN);
}

int main(void)
{	
	  bool keyLongPress=0;
		unsigned int keyPressTime=0;// 按键按下计数变量
	
		delay_init();	          // 延时函数初始化	  
	  NVIC_Configuration();
	  GPS_rx_flag = 0;
		delay_ms(500);       // 上电瞬间加入一定延时在初始化
	  KEY_GPIO_Init();     // 按键初始化
	  Lcd_Init();          // LCD液晶初始化
	  STM32_FlashCheck();
	  KEY_GPIO_Init();    // 按键引脚初始化
	  DS18B20_Init();     // 温度DS18B20初始化
	  adxl345_init();     // ADXL345初始化
	  Lcd_Clear(timeBackground);  //屏幕填充为白色
		uart1_Init(9600);
	  USART3_Init(9600);
	  max30102_init_fifo();  // MAX30102初始化
	  DS1302_Init(&SysDate); // DS1302时钟初始化
	  DisplayInitInter1();   // 显示初始界面
	  GPS_rx_flag = 1;
		TIM3_Init(199,7199);     // 定时器初始化，定时20ms
		//Tout = ((arr+1)*(psc+1))/Tclk ; 
		//Tclk:定时器输入频率(单位MHZ)
		//Tout:定时器溢出时间(单位us)
		memset(STM32_RX1_BUF,0,STM32_RX1BUFF_SIZE);  //清空缓存
		STM32_Rx1Counter = 0;
		while(1)
		{  
			  key_num = keyscan();     // 按键扫描
			
				if(key_num == 2)   // 不在设置状态下，按第二个按键是步数清零
				{
					  selectColor ++;
						if(selectColor >= ColorNum)selectColor = 0;
						timeBackground = Color[selectColor]; //选择背景颜色
						if(set_flag==0)
						{
							  if(mode == 0)drawClockFace();   //绘制表盘刻度
							  if(mode == 1)
								{
										LCD_Fill(0,67,160,129,timeBackground);   //区域填充背景色
									  Gui_DrawFont_GBK16(30, 68, timeBackground, WHITE,(u8 *)"Exercise Time");

										t_eTime=0xFFFFFF;
								}
								if(mode == 2)
								{
										clear_time_data();
										LCD_Fill(0,0,160,64,timeBackground); //填充时钟背景颜色
										init_display_time = 1;
								}
						}
						else
						{
								InitDispalyFlag = 1;
						}
						STM32_FlashWriteData(); // 存储数据
				}
				if(key_num == 3)   // 不在设置状态下，按第三个按键是里程清零
				{
						step_num = 0;
						STM32_FlashWriteData(); // 存储数据
					  StepRemindTime=0;
					  beep_remind(90);       // 蜂鸣器提醒
				}
				if(key_num == 4)   // 不在设置状态下，按第四个按键是开启运动计时和关闭运动计时
				{
						while(KEY4 == 0)
						{
								if(keyPressTime<60000)keyPressTime++;
								if(keyPressTime >= 500 && !keyLongPress)   // 长按，运动时间清零
								{
										start_flag = 0;
										exerciseTime = 0;

									  keyLongPress = 1;
									
									  if(mode==1){
											display_exercise_time();// 运动时间清零后需要更新显示下运动时间
										}
								  	beep_remind(800);  // 蜂鸣器提醒
								}
								delay_ms(1);
						}
						if(keyPressTime < 500) // 短按，暂停/开启运动时间
						{
								start_flag = !start_flag;
							  beep_remind(90);  // 蜂鸣器提醒
						}
						keyPressTime = 0;
						keyLongPress = 0;
				}
				if(key_num == 5) // 不在设置状态下，按第五个按键是切换显示界面
				{
					  mode ++;
					  if(mode > 2)mode=0;
					  if(mode == 0)DisplayInitInter1(); // 时钟表盘界面
						if(mode == 1)DisplayInitInter2(); // 数据显示界面
						if(mode == 2)DisplayInitInter3(); // 数字时钟及经纬度界面
				}

				if(key_num == 1)
				{
						display_main_menu();  // 显示主菜单
				}
				Get_Ds1302_Time();  // 读取DS1302时间
				Get_Steps();        // 获取步数
				Get_GPS();          // 获取经纬度
				if(shuaxin == 1)    // 延时一段时间读取温度和心率血氧
				{
					  shuaxin = 0;
					  temperature = ReadTemperature()*10;           // 读取温度
					  max30102_read_data();                        // 读取心率血氧值
				}
        /*********************************************************************************/
				/*                                在表盘示界面                                   */
				/********************************************************************************/
        if(mode == 0)
				{
            if(SysDate.update_sec != SysDate.sec)  // 当秒发生变化时，更新时钟界面
						{
							  SysDate.week=Conver_week(SysDate.year,SysDate.mon,SysDate.day); // 读取星期数据

								updateClock(SysDate.hour, SysDate.min, SysDate.sec);   // 更新表盘时钟
                
								switch(SysDate.week)  //显示星期
								{
										case 1:LCD_ShowString(134,3,(u8 *)"Mon",YELLOWGREEN,BLACK,12);break;
										case 2:LCD_ShowString(134,3,(u8 *)"Tue",YELLOWGREEN,BLACK,12);break;
										case 3:LCD_ShowString(134,3,(u8 *)"Wed",YELLOWGREEN,BLACK,12);break;
										case 4:LCD_ShowString(134,3,(u8 *)"Thu",YELLOWGREEN,BLACK,12);break;
										case 5:LCD_ShowString(134,3,(u8 *)"Fri",YELLOWGREEN,BLACK,12);break;
										case 6:LCD_ShowString(134,3,(u8 *)"Sat",YELLOWGREEN,BLACK,12);break;
										case 0:LCD_ShowString(134,3,(u8 *)"Sun",YELLOWGREEN,BLACK,12);break;
								}
								
								// 显示月日
								sprintf(display,"%02d/%02d",SysDate.mon,SysDate.day);
								LCD_ShowString(1,3,(u8 *)display,YELLOWGREEN,BLACK,12);    
							
							  if(step_num >= remind_step)
									display_format(0,118,step_num,12,RED,BLACK);           /*  步数达标，字体显示红色  */
								else
								  display_format(0,118,step_num,12,YELLOWGREEN,BLACK);   /*  显示步数  */
								
								
								sprintf(display,"%.1f",(float)temperature/10);
								if(temperature<=temp_min||temperature>=temp_max)
								{
										LCD_ShowString(120,118,(u8 *)display,RED,BLACK,12);  /*  温度不在范围，字体显示红色  */
										Gui_DrawFont_GBK12(148,116,RED,BLACK,(u8 *)"℃");
								}
								else
								{
										LCD_ShowString(120,118,(u8 *)display,YELLOWGREEN,BLACK,12);  /*  显示温度  */
										Gui_DrawFont_GBK12(148,116,YELLOWGREEN,BLACK,(u8 *)"℃");
								}
								
							  SysDate.update_sec = SysDate.sec;
						}
				}
				/*********************************************************************************/
				/*                                在数据显示界面                                 */
				/*********************************************************************************/
				if(mode == 1)  
				{
					  if(step_num >= remind_step)
							display_format(121,49,step_num,16,RED,WHITE);    /*  步数达标，字体显示红色  */
						else
							display_format(121,49,step_num,16,GREE2,WHITE);   /*  显示步数  */
						
					  if(update_eTime==1) // 一秒更新显示以下数据
						{
							  sprintf(display,"%03d",hrAvg);
							  if((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))
								{
									  Gui_DrawFont_GBK16(5, 49, RED, WHITE,(u8 *)display); 	/*  心率不在范围，字体显示红色  */
								}
								else
								{
										Gui_DrawFont_GBK16(5, 49, GREE2, WHITE,(u8 *)display); // 显示心率值
                }
								sprintf(display,"%03d",spo2Avg);
								if((spo2Avg!=0)&&(spo2Avg<=spo2_min))
								{
										Gui_DrawFont_GBK16(48, 49, RED, WHITE,(u8 *)display);  /*  血氧不在范围，字体显示红色  */
								}
								else
								{
										Gui_DrawFont_GBK16(48, 49, GREE2, WHITE,(u8 *)display); // 显示血氧值
								}
								sprintf(display,"%.1f",(float)temperature/10);
								if(temperature<=temp_min||temperature>=temp_max)
								{
										Gui_DrawFont_GBK16(85, 49, RED, WHITE,(u8 *)display);   /*  温度不在范围，字体显示红色  */
								}
								else
								{
										Gui_DrawFont_GBK16(85, 49, GREE2, WHITE,(u8 *)display);  // 显示温度值
								}
							
								display_exercise_time();  // 显示运动时间
							
							  update_eTime = 0;
						}
				}
				/*********************************************************************************/
				/*                            在数字时钟及经纬度显示界面                         */
				/*********************************************************************************/
				if(mode == 2) 
				{
						display_time(0,0,WHITE,timeBackground,WHITE,timeBackground); // 显示数字时钟，年月日星期时分秒
					
					  if(update_gps==1) // 一秒更新显示经纬度信息
						{
								sprintf((char *)display,"%.6f",gps_lon);
							  LCD_ShowString(38,69,(u8 *)display,BLUE,WHITE,24);     /* 显示经度 */
							
								sprintf((char *)display,"%.6f",gps_lat);
							  LCD_ShowString(38,101,(u8 *)display,BLUE,WHITE,24);    /* 显示纬度 */
							
							  update_gps = 0;
						}
				}
				UsartSendReceiveData();			 //串口发送和接收数据，用于和手机APP通信
		}	
}

void TIM3_IRQHandler(void)// 定时器3中断服务程序，用于记录时间
{ 
	  static u16 timeCount1 = 0;
	  static u16 timeCount2 = 0;
	  static u16 timeCount3 = 0;
	  static bool flag1=0;
	  static bool alarm_flag1=0;
	
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 检查指定的TIM中断发生与否:TIM 中断源 
		{ 
				TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位  
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
					  /* 最大计时到99小时，转换为总秒数：99x60x60=356400 */
					  if(exerciseTime<356400 && start_flag) exerciseTime ++;
					  if(StepRemindTime>0)StepRemindTime--;
					  update_eTime=1;
					  update_gps = 1;
						SendFlag = 1;
				}
				/*********************************************************************************/
				/*                            以下为蜂鸣器提示部分                               */
				/*********************************************************************************/
			  if(step_num >= remind_step)  // 步数达标
				{
					  if(flag1==0)
						{
								tixing=1;
							  StepRemindTime=10;  // 步数达标，提醒时间为10秒
						}
						flag1 = 1;
				}
				else
				{
						flag1 = 0;
				}
				
				if(((hrAvg!=0)&&(hrAvg>=heart_rate_max||hrAvg<=heart_rate_min))||((spo2Avg!=0)&&(spo2Avg<=spo2_min))||(temperature<=temp_min||temperature>=temp_max))//心率血氧温度不在范围
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
				if(tixing == 1)  // 步数达标的提示音
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
				if(tixing == 2) // 当心率血氧这些参数不在范围的提示音
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

