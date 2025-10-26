#include "lcd.h"
#include "delay.h"
#include "font.h"

//Һ��IO��ʼ������
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
//��SPI���ߴ���һ��8λ����
void  SPI_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
			LCD_SDA_SET; //�������
    else
			LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(u8 Index)
{
   //SPI д����ʱ��ʼ
   LCD_CS_CLR;
   LCD_RS_CLR;
	 SPI_WriteData(Index);
   LCD_CS_SET;
}

//��Һ����дһ��8λ����
void Lcd_WriteData(u8 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
   SPI_WriteData(Data);
   LCD_CS_SET; 
}
//��Һ����дһ��16λ����
void LCD_WriteData_16Bit(u16 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;
	 SPI_WriteData(Data>>8); 	//д���8λ����
	 SPI_WriteData(Data); 			//д���8λ����
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
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
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
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}    

/*****************************************
 �������ܣ���TFTĳһ�����ɫ                          
 ���ڲ�����color  ����ɫֵ                                 
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//���������ֽ�
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

//��ST7735����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
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
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
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
��������LCD_ShowPicture
���ܣ���ʾͼƬ����
��ڲ�����x y     ��ʾ���������
          width   ͼƬ�Ŀ��  
          length  ͼƬ�ĸ߶� 
          *p      Ҫ��ʾ��ͼƬ
����ֵ����
*************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 width,u16 length,const unsigned char *p)
{
	u16 i; 
	unsigned char picH,picL;

	 Lcd_SetRegion(x+2,y,x+width-1,y+length-1);		//��������
	 for(i=0;i<width*length;i++)
	 {
				picL=*(p+i*2);	//���ݵ�λ��ǰ
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  	
	 }
}

/******************************************************************************
����˵������ָ�����������ɫ
������ݣ�xsta,ysta   ��ʼ����
					xend,yend   ��ֹ����
					color       Ҫ������ɫ
����ֵ��  ��
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	Lcd_SetRegion(xsta,ysta,xend-1,yend-1);//������ʾ��Χ
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

//// ��������ƽ�������ƣ�����С��ֵ��
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

//// ���Բ�Ǿ���
//void TFT_FillRoundRect(int x, int y, int w, int h, int r, u16 color) {
//	
//	  u16 offset;
//	  int dy;
//	
//    // ����У��
//    if (r < 0) r = 0;
//    if (r > w/2) r = w/2;
//    if (r > h/2) r = h/2;
//    
//    // 1. ��������������
//    TFT_FillRect(x, y + r, w, h - 2*r, color);
//    
//    // 2. ��䶥���͵ײ���������
//    TFT_FillRect(x + r, y, w - 2*r, r, color);
//    TFT_FillRect(x + r, y + h - r, w - 2*r, r, color);
//    
//    // 3. ����ĸ�Բ�ǣ�ʹ��ɨ���߷�����
//    for (dy = 1; dy <= r; dy++) {
//        // ���㵱ǰ�е�ˮƽƫ��
//         offset = fast_sqrt(r*r - dy*dy);
//        
//        // ���Ͻ�
//        TFT_FillRect(x + r - offset, y + r - dy, offset, 1, color);
//        // ���Ͻ�
//        TFT_FillRect(x + w - r, y + r - dy, offset, 1, color);
//        // ���½�
//        TFT_FillRect(x + r - offset, y + h - r + dy - 1, offset, 1, color);
//        // ���½�
//        TFT_FillRect(x + w - r, y + h - r + dy - 1, offset, 1, color);
//    }
//}

/******************************************************************************
����˵������Բ����
������ݣ�X,Y   ��ʼ����
					R     Բ�İ뾶
					fc    �ߵ���ɫ
����ֵ��  ��
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
      ����˵��������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
                color   �ߵ���ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		Gui_DrawPoint(uRow,uCol,color);//����
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
����˵���������κ���
������ݣ�x,y    �������
					w,h    ��ȣ�����
					bc     �ߵ���ɫ
����ֵ��  ��
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
��������: ����Ļ��ʾһ͹��İ�ť��
��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
��    ��: ��
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
��������: ����Ļ��ʾһ���µİ�ť��
��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
��    ��: ��
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
����˵������ʾ�����ַ�
������ݣ�x,y��ʾ����
					num Ҫ��ʾ���ַ�
					fc �ֵ���ɫ
					bc �ֵı���ɫ
					sizey �ֺ�
����ֵ��  ��
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey)
{
	u8 temp,sizex,t,m=0,str=num;
	u16 i,TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //�õ�ƫ�ƺ��ֵ
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
	Lcd_SetRegion(x+2,y,x+sizex-1,y+sizey-1);//���ù��λ�� 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //����6x12����
//		else if(sizey==16)temp=ascii_1608[num][i];		 //����8x16����
	  else if(sizey==24)temp=ascii_2412[num][i];		 //����12x24����
		else if(sizey==32)temp=ascii_3216[num][i];		 //����16x32����
		else if(sizey==48)temp=ascii_4824[num][i];		 //����24x48����
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
����˵������ʾ�ַ���
������ݣ�x,y��ʾ����
					*p Ҫ��ʾ���ַ���
					fc �ֵ���ɫ
					bc �ֵı���ɫ
					sizey �ֺ�
����ֵ��  ��
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
����˵������ʾ12x12���ֺ���
������ݣ�x,y    �������
					fc     ������ɫ
					bc     ���屳������ɫ
					s      Ҫ��ʾ�ĺ���
����ֵ��  ��
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
����˵������ʾ16x16���ֺ���
������ݣ�x,y    �������
					fc     ������ɫ
					bc     ���屳������ɫ
					s      Ҫ��ʾ�ĺ���
����ֵ��  ��
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
����˵������ʾ24x24���ֺ���
������ݣ�x,y    �������
					fc     ������ɫ
					bc     ���屳������ɫ
					s      Ҫ��ʾ�ĺ���
����ֵ��  ��
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
����˵������ʾ��������庯��
������ݣ�x,y    �������
					fc     ������ɫ
					bc     ���屳������ɫ
					num    Ҫ��ʾ������
����ֵ��  ��
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



