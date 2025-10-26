#include "sys.h"

#define USE_HORIZONTAL 3  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define X_MAX_PIXEL	        132
#define Y_MAX_PIXEL	        160

#else
#define X_MAX_PIXEL	        160
#define Y_MAX_PIXEL	        132
#endif

#define YELLOWGREEN  0x9E66
#define HotPink 0xFB56
#define  CYAN   0x50B
#define CARMINE 0xE00B
#define COFFEE  0x49C0
#define BERGUNDY 0x4004
#define VIOLETRED 0xc0b0
#define GREE2  	0x320
#define ORANGE  0xfa20
#define RED  	  0xf800
#define GREEN	  0x07e0
#define BLUE 	  0x001f
#define BLUE2 	0x22f
#define WHITE  	0xffff
#define BLACK	  0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	    //��ɫ0 3165 00110 001011 00101
#define GRAY1   0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2   0x4208      	//��ɫ2  1111111111011111

#define LCD_CTRLA   	  	GPIOA		//����TFT���ݶ˿�
#define LCD_CTRLB   	  	GPIOB		//����TFT���ݶ˿�

#define LCD_SCL        	GPIO_Pin_4	//TFT --SCL/SCK
#define LCD_SDA        	GPIO_Pin_5	//TFT --SDA/DIN
#define LCD_RST     	  GPIO_Pin_6	//TFT --RST/RES
#define LCD_RS         	GPIO_Pin_7	//TFT --RS/DC
#define LCD_CS        	GPIO_Pin_0  //TFT --CS/CE

//Һ�����ƿ���1�������궨��
#define	LCD_SCL_SET  	LCD_CTRLA->BSRR=LCD_SCL    
#define	LCD_SDA_SET  	LCD_CTRLA->BSRR=LCD_SDA   
#define	LCD_RST_SET  	LCD_CTRLA->BSRR=LCD_RST 
#define	LCD_RS_SET  	LCD_CTRLA->BSRR=LCD_RS
#define	LCD_CS_SET  	LCD_CTRLB->BSRR=LCD_CS  
 
//Һ�����ƿ���0�������궨��
#define	LCD_SCL_CLR  	LCD_CTRLA->BRR=LCD_SCL  
#define	LCD_SDA_CLR  	LCD_CTRLA->BRR=LCD_SDA
#define	LCD_RST_CLR  	LCD_CTRLA->BRR=LCD_RST
#define	LCD_RS_CLR  	LCD_CTRLA->BRR=LCD_RS 
#define	LCD_CS_CLR  	LCD_CTRLB->BRR=LCD_CS 

#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //�������
#define LCD_DATAIN     LCD_DATA->IDR;   //��������

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void LCD_WriteData_16Bit(u16 Data);
void Lcd_WriteReg(u8 Index,u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end);
void Lcd_SetXY(u16 x,u16 y);
void Gui_DrawPoint(u16 x,u16 y,u16 Data);
unsigned int Lcd_ReadPoint(u16 x,u16 y);
u16 LCD_BGR2RGB(u16 c);
void Lcd_Clear(u16 Color);
void LCD_ShowPicture(u16 x,u16 y,u16 width,u16 length,const unsigned char *p);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) ;
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey);
void Gui_DrawFont_GBK12(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num);
void TFT_FillRoundRect(int x, int y, int w, int h, int r, u16 color);

