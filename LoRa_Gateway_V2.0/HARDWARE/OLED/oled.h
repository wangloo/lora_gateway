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
			   
/****************************OLED IIC端口定义**********************************/ 		     
#define OLED_SCL_Pin    GPIO_PIN_6
#define OLED_SDA_Pin    GPIO_PIN_7

/****************************函数的各种参数定义********************************/
#define OLED_IIC_ADDR                         0x78         /* OLED地址是 0x78 */
																	         
#define OLED_FRAMEBUF_LEN  				            1024         /* OledFrameBuf的长度是 8*128Byte 
                                                              对应 SSD1306 的 GDDRAM 大小 */
											                      				      	                 
#define OLED_CMD                              0x00	       /* 写命令,见SSD1306 datasheet */
#define OLED_DATA                             0x40         /* 写数据,见SSD1306 datasheet */
																	         				      
#define FONT_SIZE_8                           8            /* 支持的字体大小 */
#define FONT_SIZE_16                          16         
																	         				      
#define DISPLAY_REVERSE                       1            /* 反转显示/正常显示 */
#define DISPLAY_NORMAL                        0          

#define DISPLAY_FILL_ZERO                     1            /* 必要时是否填充0 */
#define DISPLAY_NOFILL_ZERO                   0 

/*****************************函数错误处理函数*********************************/
#define OLED_PARAMETER_ERROR                  {}           /* 参数错误 */     

/******************************************************************************/



extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;

void OLED_Init(void);                                      /* 初始化SSD1306	*/
void OLED_Clear(unsigned dat);                             /* 清屏函数 */
void OLED_ClearArea(u8 col0, u8 page0, u8 col1, u8 page1); /* 清空指定的区域 */
void OLED_Refreash(void);                                  /* OLED刷新显存 */
void OLED_Draw_Dot(u8 x, u8 y);                            /* 在指定位置画一个点 */
void OLED_Draw_Line(u8 x0,u8 y0,u8 x1,u8 y1);              /* 在指定位置画一条线段 */
void OLED_Fill_All(void);                                  /* OLED全部填充横线(8条) */
void OLED_ShowChar(u8 col, u8 page, u8 chr, u8 size, u8 isReverse);                          /* 在指定位置显示一个字符 */
void OLED_ShowString(u8 col, u8 page, u8 *str, const u32 strLen, u8 size, u8 isReverse);     /* 在指定位置显示字符串 */
void OLED_ShowNum(u8 col, u8 page, s32 num, u8 numLen, u8 size, u8 isReverse, u8 isFillZero);/* 在指定位置显示阿拉伯数字 */
void OLED_ShowCHinese(u8 col, u8 page, u8 index); /* 显示汉字 */
void OLED_DrawBMP(u8 col0, u8 page0, u8 col1, u8 page1, u8 BMP[]);                          /* 显示图片 */


void OLED_Display_On(void);                                /* 开启OLED显示 */
void OLED_Display_Off(void);                               /* 关闭OLED显示 */

u32 oled_pow(u8 m, u8 n); /* m^n函数 */

#endif
