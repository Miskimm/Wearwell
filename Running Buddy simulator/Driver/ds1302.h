#ifndef _DS_1302_H
#define _DS_1302_H

#include "sys.h"

#define RST_CLR  GPIO_ResetBits(GPIOB, GPIO_Pin_9)//CEΪ�͵�ƽ
#define RST_SET  GPIO_SetBits(GPIOB, GPIO_Pin_9)//CEΪ�ߵ�ƽ

#define DAT_CLR  GPIO_ResetBits(GPIOB, GPIO_Pin_8)//IOΪ�͵�ƽ
#define DAT_SET  GPIO_SetBits(GPIOB, GPIO_Pin_8)//IOΪ�ߵ�ƽ
#define DAT_READ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)//��ȡIO��ƽ

#define SCK_CLR  GPIO_ResetBits(GPIOB, GPIO_Pin_7)//SCLKΪ�͵�ƽ
#define SCK_SET  GPIO_SetBits(GPIOB, GPIO_Pin_7)//SCLKΪ�ߵ�ƽ

#define DataToBcd(x) ((x/10)*16+(x%10))//ʮ����תBCD��
#define BcdToData(x) ((x/16)*10+(x%16))  //the BCD code: the least 4bits can't excceed 9

#define WRITE_SEC_REG	        0x80
#define WRITE_MIN_REG           0x82
#define WRITE_HOUR_REG          0x84
#define WRITE_DAY_REG           0x86
#define WRITE_MON_REG           0x88
#define WRITE_WEEK_REG          0x8a
#define WRITE_YEAR_REG          0x8c
#define READ_SEC_REG            0x81
#define READ_MIN_REG            0x83
#define READ_HOUR_REG           0x85
#define READ_DAY_REG            0x87
#define READ_MON_REG            0x89
#define READ_WEEK_REG           0x8b
#define READ_YEAR_REG           0x8d
#define WRITE_CONTROL_REG       0x8e
#define READ_CONTROL_REG        0x8f
#define WRITE_CHARGE_REG        0x90
#define READ_CHARGE_REG         0x91
#define WRITE_RAM_REG           0xc0
#define READ_RAM_REG            0xc1

typedef struct
{
	volatile uint8_t sec;//��
	volatile uint8_t min;//��
	volatile uint8_t hour;//ʱ
	volatile uint8_t day;//��
	volatile uint8_t mon;//��
	volatile uint8_t week;//����
	volatile uint16_t year;//��
	uint8_t update_hour;
	uint8_t update_min;
	uint8_t update_sec;
	uint8_t update_day;
	uint8_t update_mon;
	uint8_t update_year;
	uint8_t update_week;
}DATE;

int DS1302_Init(const DATE * const date);
int DS1302_DateSet(const DATE * const date);
int DS1302_DateRead(DATE * const date);
int Conver_week(u8 year,u8 month,u8 day);
extern DATE SysDate;

#endif

