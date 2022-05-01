/**********************************************
 * BSP of 0.96' OLED module(SSD1306)
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/		

#ifndef __OLED_H
#define __OLED_H	
#include "main.h"		  	 
#include "sys.h"	  
			   
/****************************OLED IIC�˿ڶ���**********************************/ 		     
#define OLED_SCL_Pin    GPIO_PIN_6
#define OLED_SDA_Pin    GPIO_PIN_7

/****************************�����ĸ��ֲ�������********************************/
#define OLED_IIC_ADDR                         0x78         /* OLED��ַ�� 0x78 */
																	         
#define OLED_FRAMEBUF_LEN  				            1024         /* OledFrameBuf�ĳ����� 8*128Byte 
                                                              ��Ӧ SSD1306 �� GDDRAM ��С */
											                      				      	                 
#define OLED_CMD                              0x00	       /* д����,��SSD1306 datasheet */
#define OLED_DATA                             0x40         /* д����,��SSD1306 datasheet */
																	         				      
#define FONT_SIZE_8                           8            /* ֧�ֵ������С */
#define FONT_SIZE_16                          16         
																	         				      
#define DISPLAY_REVERSE                       1            /* ��ת��ʾ/������ʾ */
#define DISPLAY_NORMAL                        0          

#define DISPLAY_FILL_ZERO                     1            /* ��Ҫʱ�Ƿ����0 */
#define DISPLAY_NOFILL_ZERO                   0 

/*****************************������������*********************************/
#define OLED_PARAMETER_ERROR                  {}           /* �������� */     

/******************************************************************************/



extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;

void OLED_Init(void);                                      /* ��ʼ��SSD1306	*/
void OLED_Clear(unsigned dat);                             /* �������� */
void OLED_ClearArea(u8 col0, u8 page0, u8 col1, u8 page1); /* ���ָ�������� */
void OLED_Refreash(void);                                  /* OLEDˢ���Դ� */
void OLED_Draw_Dot(u8 x, u8 y);                            /* ��ָ��λ�û�һ���� */
void OLED_Draw_Line(u8 x0,u8 y0,u8 x1,u8 y1);              /* ��ָ��λ�û�һ���߶� */
void OLED_Fill_All(void);                                  /* OLEDȫ��������(8��) */
void OLED_ShowChar(u8 col, u8 page, u8 chr, u8 size, u8 isReverse);                          /* ��ָ��λ����ʾһ���ַ� */
void OLED_ShowString(u8 col, u8 page, u8 *str, const u32 strLen, u8 size, u8 isReverse);     /* ��ָ��λ����ʾ�ַ��� */
void OLED_ShowNum(u8 col, u8 page, s32 num, u8 numLen, u8 size, u8 isReverse, u8 isFillZero);/* ��ָ��λ����ʾ���������� */
void OLED_ShowCHinese(u8 col, u8 page, u8 index); /* ��ʾ���� */
void OLED_DrawBMP(u8 col0, u8 page0, u8 col1, u8 page1, u8 BMP[]);                          /* ��ʾͼƬ */


void OLED_Display_On(void);                                /* ����OLED��ʾ */
void OLED_Display_Off(void);                               /* �ر�OLED��ʾ */

u32 oled_pow(u8 m, u8 n); /* m^n���� */

#endif
