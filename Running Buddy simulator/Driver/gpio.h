#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"

#define KEY1 PBin(12)
#define KEY2 PBin(13)
#define KEY3 PBin(14)
#define KEY4 PBin(15)
#define KEY5 PAin(8)

#define BEEP     PCout(13)

void KEY_GPIO_Init(void);//���ų�ʼ��

#endif
