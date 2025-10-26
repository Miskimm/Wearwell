#include "lcd.h"
#include "delay.h"
#include "font.h"

//液晶IO初始化配置
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
      
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//向SPI总线传输一个8位数据
void  SPI_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
			LCD_SDA_SET; //输出数据
    else
			LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

//向液晶屏写一个8位指令
void Lcd_WriteIndex(u8 Index)
{
   //SPI 写命令时序开始
   LCD_CS_CLR;
   LCD_RS_CLR;
	 SPI_WriteData(Index);
   LCD_CS_SET;
}

//向液晶屏写一个8位数据
void Lcd_WriteData(u8 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
   SPI_WriteData(Data);
   LCD_CS_SET; 
}
//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(u16 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
	 SPI_WriteData(Data>>8); 	//写入高8位数据
	 SPI_WriteData(Data); 			//写入低8位数据
   LCD_CS_SET; 
}

void Lcd_WriteReg(u8 Index,u8 Data)
{
		Lcd_WriteIndex(Index);
		Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(100);
}

//LCD Init
void Lcd_Init(void)
{	
	LCD_GPIO_Init();
	Lcd_Reset(); //Reset before LCD Init.

	Lcd_WriteIndex(0x11);//Sleep exit 
	delay_ms (120);
		
	//ST7735R Frame Rate
	Lcd_WriteIndex(0xB1); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB2); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteIndex(0xB3); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	
	Lcd_WriteIndex(0xB4); //Column inversion 
	Lcd_WriteData(0x07); 
	
	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0); 
	Lcd_WriteData(0xA2); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x84); 
	Lcd_WriteIndex(0xC1); 
	Lcd_WriteData(0xC5); 

	Lcd_WriteIndex(0xC2); 
	Lcd_WriteData(0x0A); 
	Lcd_WriteData(0x00); 

	Lcd_WriteIndex(0xC3); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0x2A); 
	Lcd_WriteIndex(0xC4); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0xEE); 
	
	Lcd_WriteIndex(0xC5); //VCOM 
	Lcd_WriteData(0x0E); 
	
	Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
	if(USE_HORIZONTAL==0)Lcd_WriteData(0x00);
	else if(USE_HORIZONTAL==1)Lcd_WriteData(0xC0);
	else if(USE_HORIZONTAL==2)Lcd_WriteData(0x70);
	else Lcd_WriteData(0xA0); 
	
	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1a); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x18); 
	Lcd_WriteData(0x2f); 
	Lcd_WriteData(0x28); 
	Lcd_WriteData(0x20); 
	Lcd_WriteData(0x22); 
	Lcd_WriteData(0x1f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x23); 
	Lcd_WriteData(0x37); 
	Lcd_WriteData(0x60); 	
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x10); 

	Lcd_WriteIndex(0xe1); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x17); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x2c); 
	Lcd_WriteData(0x29); 
	Lcd_WriteData(0x2e); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x39); 
	Lcd_WriteData(0x3f); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x03); 
	Lcd_WriteData(0x10);  
	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x7f);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x9f);
	
	Lcd_WriteIndex(0xF0); //Enable test command  
	Lcd_WriteData(0x01); 
	Lcd_WriteIndex(0xF6); //Disable ram power save mode 
	Lcd_WriteData(0x00); 
	
	Lcd_WriteIndex(0x3A); //65k mode 
	Lcd_WriteData(0x05); 
	
	
	Lcd_WriteIndex(0x29);//Display on	 
}


/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start);//Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+0);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+1);
	
	Lcd_WriteIndex(0x2c);

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

//从ST7735读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}

/*************************************************
函数名：LCD_ShowPicture
功能：显示图片函数
入口参数：x y     显示的起点坐标
          width   图片的宽度  
          length  图片的高度 
          *p      要显示的图片
返回值：无
*************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 width,u16 length,const unsigned char *p)
{
	u16 i; 
	unsigned char picH,picL;

	 Lcd_SetRegion(x+2,y,x+width-1,y+length-1);		//坐标设置
	 for(i=0;i<width*length;i++)
	 {
				picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  	
	 }
}

/******************************************************************************
函数说明：在指定区域填充颜色
入口数据：xsta,ysta   起始坐标
					xend,yend   终止坐标
					color       要填充的颜色
返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Lcd_SetRegion(xsta,ysta,xend-1,yend-1);//设置显示范围
	for(i=ysta;i<yend+1;i++)
	{													   	 	
		for(j=xsta;j<xend+2;j++)
		{
			LCD_WriteData_16Bit(color); 
		}
	} 					  	    
}

//void TFT_FillRect(int x, int y, int width, int height, u16 color) {
//	 u16 row;
//	 u16 col;
//	
//    for ( row = y; row < y + height; row++) {
//        for ( col = x; col < x + width; col++) {
//            Gui_DrawPoint(col, row, color);
//        }
//    }
//}

//// 快速整数平方根近似（用于小数值）
//uint16_t fast_sqrt(uint32_t x) {
//	  int i;
//	  uint32_t y; 
//	
//    if (x == 0) return 0;
//    
//    y	= x;
//    for (i = 0; i < 10; i++) {
//        y = (y + x / y) >> 1;
//    }
//    return (uint16_t)y;
//}

//// 填充圆角矩形
//void TFT_FillRoundRect(int x, int y, int w, int h, int r, u16 color) {
//	
//	  u16 offset;
//	  int dy;
//	
//    // 参数校验
//    if (r < 0) r = 0;
//    if (r > w/2) r = w/2;
//    if (r > h/2) r = h/2;
//    
//    // 1. 填充中央矩形区域
//    TFT_FillRect(x, y + r, w, h - 2*r, color);
//    
//    // 2. 填充顶部和底部矩形条带
//    TFT_FillRect(x + r, y, w - 2*r, r, color);
//    TFT_FillRect(x + r, y + h - r, w - 2*r, r, color);
//    
//    // 3. 填充四个圆角（使用扫描线方法）
//    for (dy = 1; dy <= r; dy++) {
//        // 计算当前行的水平偏移
//         offset = fast_sqrt(r*r - dy*dy);
//        
//        // 左上角
//        TFT_FillRect(x + r - offset, y + r - dy, offset, 1, color);
//        // 右上角
//        TFT_FillRect(x + w - r, y + r - dy, offset, 1, color);
//        // 左下角
//        TFT_FillRect(x + r - offset, y + h - r + dy - 1, offset, 1, color);
//        // 右下角
//        TFT_FillRect(x + w - r, y + h - r + dy - 1, offset, 1, color);
//    }
//}

/******************************************************************************
函数说明：画圆函数
入口数据：X,Y   起始坐标
					R     圆的半径
					fc    线的颜色
返回值：  无
******************************************************************************/
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
} 

/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		Gui_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/******************************************************************************
函数说明：画方形函数
入口数据：x,y    起点坐标
					w,h    宽度，长度
					bc     线的颜色
返回值：  无
******************************************************************************/
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	LCD_DrawLine(x,y,x+w,y,0xEF7D);
	LCD_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	LCD_DrawLine(x,y+h,x+w,y+h,0x2965);
	LCD_DrawLine(x,y,x,y+h,0xEF7D);
    LCD_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		LCD_DrawLine(x,y,x+w,y,0xEF7D);
		LCD_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		LCD_DrawLine(x,y+h,x+w,y+h,0x2965);
		LCD_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		LCD_DrawLine(x,y,x+w,y,0x2965);
		LCD_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		LCD_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		LCD_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		LCD_DrawLine(x,y,x+w,y,0xffff);
		LCD_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		LCD_DrawLine(x,y+h,x+w,y+h,0xffff);
		LCD_DrawLine(x,y,x,y+h,0xffff);
	}
}

/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	LCD_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	LCD_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	LCD_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	LCD_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	LCD_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	LCD_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	LCD_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	LCD_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	LCD_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    LCD_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}

/******************************************************************************
函数说明：显示单个字符
入口数据：x,y显示坐标
					num 要显示的字符
					fc 字的颜色
					bc 字的背景色
					sizey 字号
返回值：  无
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey)
{
	u8 temp,sizex,t,m=0,str=num;
	u16 i,TypefaceNum;//一个字符所占字节大小
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //得到偏移后的值
	if(sizey==48)
	{
		num=num-16;
	}
	if(sizey==24 && str!=' ')
	{
		num=num-13;
	}
//	if(sizey==12 && num>=27)
//	{
//		num=num-38;
//	}
	Lcd_SetRegion(x+2,y,x+sizex-1,y+sizey-1);//设置光标位置 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
//		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
	  else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
		else if(sizey==48)temp=ascii_4824[num][i];		 //调用24x48字体
		else return;
		for(t=0;t<8;t++) 
		{
				if(temp&(0x01<<t))LCD_WriteData_16Bit(fc);
				else LCD_WriteData_16Bit(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
		}
	}   	 	  
}

/******************************************************************************
函数说明：显示字符串
入口数据：x,y显示坐标
					*p 要显示的字符串
					fc 字的颜色
					bc 字的背景色
					sizey 字号
返回值：  无
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey);
		x+=sizey/2;
		p++;
	}  
}

/******************************************************************************
函数说明：显示12x12汉字函数
入口数据：x,y    起点坐标
					fc     字体颜色
					bc     字体背景的颜色
					s      要显示的汉字
返回值：  无
******************************************************************************/
void Gui_DrawFont_GBK12(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else 
		{
			for (k=0;k<hz12_num;k++)
			{
			  if ((hz12[k].Index[0]==*(s))&&(hz12[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<12;i++)
				    {
						  for(j=0;j<8;j++) 
							{
						    	if(hz12[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
					   	for(j=0;j<4;j++) 
							{
						    	if(hz12[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=12;
		} 
	}
}

/******************************************************************************
函数说明：显示16x16汉字函数
入口数据：x,y    起点坐标
					fc     字体颜色
					bc     字体背景的颜色
					s      要显示的汉字
返回值：  无
******************************************************************************/
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else 
		{
		

			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						  for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
					   	for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

/******************************************************************************
函数说明：显示24x24汉字函数
入口数据：x,y    起点坐标
					fc     字体颜色
					bc     字体背景的颜色
					s      要显示的汉字
返回值：  无
******************************************************************************/
//void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
//{
//	unsigned char i,j;
//	unsigned short k;

//	while(*s) 
//	{
//		if( *s < 0x80 ) 
//		{
//			k=*s;
//			if (k>32) k-=32; else k=0;

//		    for(i=0;i<16;i++)
//			  for(j=0;j<8;j++) 
//				{
//			    	if(asc16[k*16+i]&(0x80>>j))	
//							Gui_DrawPoint(x+j,y+i,fc);
//						else 
//						{
//							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
//						}
//				}
//			s++;x+=8;
//		}
//		else 
//		{

//			for (k=0;k<hz24_num;k++) 
//			{
//			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
//			  { 
//				    for(i=0;i<24;i++)
//				    {
//						for(j=0;j<8;j++) 
//							{
//						    	if(hz24[k].Msk[i*3]&(0x80>>j))
//								Gui_DrawPoint(x+j,y+i,fc);
//								else 
//								{
//									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
//								}
//							}
//						for(j=0;j<8;j++) 
//							{
//						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
//								else {
//									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
//								}
//							}
//						for(j=0;j<8;j++) 
//							{
//						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
//								Gui_DrawPoint(x+j+16,y+i,fc);
//								else 
//								{
//									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
//								}
//							}
//				    }
//			  }
//			}
//			s+=2;x+=24;
//		}
//	}
//}

/******************************************************************************
函数说明：显示数码管字体函数
入口数据：x,y    起点坐标
					fc     字体颜色
					bc     字体背景的颜色
					num    要显示的数字
返回值：  无
******************************************************************************/
//void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
//{
//	unsigned char i,j,k,c;
//	//lcd_text_any(x+94+i*42,y+34,32,32,0x7E8,0x0,sz32,knum[i]);
////	w=w/8;

//    for(i=0;i<32;i++)
//	{
//		for(j=0;j<4;j++) 
//		{
//			c=*(sz32+num*32*4+i*4+j);
//			for (k=0;k<8;k++)	
//			{
//	
//		    	if(c&(0x80>>k))	Gui_DrawPoint(x+j*8+k,y+i,fc);
//				else {
//					if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
//				}
//			}
//		}
//	}
//}



