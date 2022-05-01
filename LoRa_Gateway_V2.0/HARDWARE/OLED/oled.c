/**********************************************
 * BSP of 0.96' OLED module(SSD1306)
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/			

#include "oled.h"
#include "oledfont.h"  
#include "stdlib.h"
#include "i2c.h"
#include "myjson.h" /* myiota()*/

u8 OledFrameBuf[8][128] = {0};         /* 对应SSD1306中128*64 bit的GDDRAM */



	
void OLED_Refreash(void); 
 	  

/**********************************************
 * @brief  初始化SSD1306	
	         于初始化GPIO之后调用
 * @param  None							 
 * @retval None
 *********************************************/			    
void OLED_Init(void) { 	

	/* 	GPIO的初始化在MX_GPIO_Init()中进行 */
	
	/* 初始化命令数组必须定义为 全局变量 或 局部静态变量，
	   若定义为局部变量，则可能 OLED_Init 执行结束，DMA没有传输完成 */
	static u8 OledInitCmd[29] = 
				   {0xAE,0x00,0x10,0x40,0xB0,0x81,0xFF,0xA1,0xA6,0xA8,\
					  0x3F,0xC8,0xD3,0x00,0xD5,0x80,0xD8,0x05,0xD9,0xF1,\
					  0xDA,0x12,0xDB,0x30,0x8D,0x14,0xAF,0x20,0x00
	};
	u16 OledInitCmdLength = sizeof(OledInitCmd); /* Length of OledInitCmd[] */
	
	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_CMD, I2C_MEMADD_SIZE_8BIT, OledInitCmd, OledInitCmdLength);
	
	HAL_Delay(20);
	
	OLED_Clear(0);
	OLED_Refreash();
}  
/**********************************************
 * @brief  清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
 * @param  None							 
 * @retval None
 *********************************************/	
void OLED_Clear(unsigned dat)  {  
	u8 i,n;		  
	for (i = 0; i < 8; i++) {  
		for (n = 0; n < 128; n++) {
			OledFrameBuf[i][n] = 0;
		}
	}
}

/**********************************************
 * @brief  清空指定的区域
 * @param  col0  : 列地址-起 [0, 127]
	         page0 : 页地址-起 [0, 7]
           col1  : 列地址-终 [0, 127]
	         page1 : 页地址-终 [0, 7]
 * @retval None
 *********************************************/	
void OLED_ClearArea(u8 col0, u8 page0, u8 col1, u8 page1)
{
	u8 i, j;
	for (i = page0; i <= page1; i++) {
		for (j = col0; j <= col1; j++) {
			OledFrameBuf[i][j] = (0 << 0);	
		}
	}	
}

/**********************************************
 * @brief  在指定位置画一个点
 * @param  x        : [0, 127]
           y        : [0, 63]
 * @retval None
 *********************************************/
void OLED_Draw_Dot(u8 x, u8 y)
{
	if (x > 127 || y > 63) {
		printf("OLED_Draw_Dot() para overflow! \r\n");
		return;
	}
	
	OledFrameBuf[y/8][x] |= 1<<(y%8);
}

/**********************************************
 * @brief  在指定位置画一条线段（来源：正点原子）
 * @param  x0        : [0, 127]
           y0        : [0, 63]
					 x1        : [0, 127]
           y1        : [0, 63]
 * @retval None
 *********************************************/
void OLED_Draw_Line(u8 x0,u8 y0,u8 x1,u8 y1) { 
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x1-x0; //计算坐标增量 
	delta_y=y1-y0; 
	uRow=x0; 
	uCol=y0; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		OLED_Draw_Dot(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
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

/**********************************************
 * @brief  OLED全部填充横线(8条)
 * @param  None							 
 * @retval None
 *********************************************/			  
void OLED_Fill_All(void)  {  
	u8 i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 128; j++) {
			OledFrameBuf[i][j] = (1 << 0);	/* 每个page仅置 bit0 为 1 */
		}
	}	
}

/**********************************************
 * @brief  在指定位置显示一个字符
 * @param  col      : [0, 128-1]，满足16*8和8*6的字体
           page     : [0, 6]    ，满足16*8和8*6的字体
           chr      : 显示的字符
           size     : 字体大小( FONT_SIZE_8, FONT_SIZE_16 )
					 isReverse: 是否反显( DISPLAY_NORMAL, DISPLAY_REVERSE )
 * @retval None
 *********************************************/
void OLED_ShowChar(u8 col, u8 page, u8 chr, u8 size, u8 isReverse)
{      	
	unsigned char c=0, i=0;	
	
	c=chr-' ';//得到偏移后的值	
	
	if (col > 128-1) {
		col = 0;
		page = page+2;	/* 这里需要改，判断SIZE */
	}
	if (size == FONT_SIZE_16) {	/* FONT_SIZE_16 */
		if (isReverse == DISPLAY_NORMAL){	/* 正常显示 */
			for (i = 0; i < 8; i++){
					OledFrameBuf[page][col+i]   = F8X16[c*16+i];		
					OledFrameBuf[page+1][col+i] = F8X16[c*16+i+8];	
			}				
		} else {	/* 反显 */
				for (i = 0; i < 8; i++){
					OledFrameBuf[page][col+i]   = ~F8X16[c*16+i];		
					OledFrameBuf[page+1][col+i] = ~F8X16[c*16+i+8];	
				}			
		}
	} else if (size == FONT_SIZE_8) {	/* FONT_SIZE_8 */
			if (isReverse == DISPLAY_NORMAL) { /* 正常显示 */
				for (i = 0; i < 6; i++) {
					OledFrameBuf[page][col+i] = F6x8[c][i];
				}
			} else { /* 反显 */
				for( i = 0; i < 6; i++) {
					OledFrameBuf[page][col+i] = ~F6x8[c][i];
				}
			}
		}
}

/**********************************************
 * @brief  在指定位置显示字符串
 * @param  col      : [0, 1023 - strLen], 多余部分会换行显示
           page     : [0, 6]
           str      : 显示的字符串
					 strLen   : 字符串的长度
           size     : 字体大小( FONT_SIZE_8, FONT_SIZE_16 )
					 isReverse: 是否反显( DISPLAY_NORMAL, DISPLAY_REVERSE )
 * @retval None
 *********************************************/
void OLED_ShowString(u8 col, u8 page, u8 *str, const u32 strLen, u8 size, u8 isReverse) {
	  unsigned char i = 0;
		
		if (col > 1023 - strLen) {
			OLED_PARAMETER_ERROR;
			return;
		}
	  while (str[i] != '\0') {
	      OLED_ShowChar(col, page, str[i], size, isReverse);
				if (size == FONT_SIZE_8) {
					col += 6;
					if (col > 122) {
						col = 0;
						page = page + 1;
					}
				} else if (size == FONT_SIZE_16) {
					col += 8;
					if (col > 120) {
						col = 0;
						page = page + 2;
					}
				}
				
		  	i++;
	  }
}

/**********************************************
* @brief  在指定位置显示阿拉伯数字（支持负数）
* @param  col       : [0, 1023 - strLen], 多余部分会换行显示
          page      : [0, 6]
          num       : 显示的数字
			    numLen    : 数字的长度（不含负号）
          size      : 字体大小( FONT_SIZE_8, FONT_SIZE_16 )
				  isReverse : 是否反显( DISPLAY_NORMAL, DISPLAY_REVERSE )
					isFillZero: 是否在 numLen > 真实位数 时使用 0 填充
					            负数不可填充 0 ( 代码未处理, users need to notice!!)
* @retval None
*********************************************/
void OLED_ShowNum(u8 col, u8 page, s32 num, u8 numLen, u8 size, u8 isReverse, u8 isFillZero) {
	u8* tpStr;
	u8  tcStrLen = 0;   /* 转换后的字符串长度 */
	u8  deltaLen = 0;   /* 需要填充的 0 个数 */
	
	tpStr = (u8 *)mymalloc(numLen + 1);	/* 确保负数能存下 */
	
	myitoa(num, tpStr, 10);
	
	tcStrLen = strlen((const char *)tpStr);
	if (isFillZero && (tcStrLen < numLen)) { /* fill 0 */
		deltaLen = numLen - tcStrLen;
		for (int k = 0; k < deltaLen; k++) {
			OLED_ShowChar(col + k*size, page, '0', size, isReverse);
		}
	}
	OLED_ShowString(col + deltaLen*size, page, tpStr, tcStrLen, size, isReverse);
	
	myfree(tpStr);
	
}


//
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;	 
	while (n--)  result *= m;    
	return result;
}				  





/**********************************************
 * @brief  显示汉字
 * @param  col   : [0, 127]  
           page  : [0, 7]
           index : 字库的索引
 * @retval None
 *********************************************/
void OLED_ShowCHinese(u8 col, u8 page, u8 index) {      			    

	/* 暂时不需要 */
}

/**********************************************
 * @brief  显示图片 
 * @param  col0  : 列地址-起 [0, 128]
           page0 : 页地址-起 [0, 8]
		  		 col1  : 列地址-终 [0, 128]
  			 	 page1 : 页地址-终 [0, 8]
           BMP   : 存储图片的数组
 * @retval None
 *********************************************/
void OLED_DrawBMP(u8 col0, u8 page0, u8 col1, u8 page1, u8 BMP[]) { 	

	u8 i, j;
	u16 index = 0;
	
	if (col0>128 || col1>128 || page0>8 || page1>8) {
		OLED_PARAMETER_ERROR;
		return;
	}
	for (i = page0; i < page1; i++) {
		for (j = col0; j < col1; j++) {
			OledFrameBuf[i][j] = BMP[index++];
		}
	}
} 
	
/**********************************************
 * @brief  OLED刷新显存 
 * @param  None							 
 * @retval None
 * @note   整个项目只需要调用一次，以后在IIC传输完成
 *         中断服务函数中会重复调用，达到自动刷新
 *         后期可以改成 定时器刷新 或 DMA循环发送
 *********************************************/	  
void OLED_Refreash(void) {	
	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_DATA, I2C_MEMADD_SIZE_8BIT, OledFrameBuf[0], OLED_FRAMEBUF_LEN);	
}




/**********************************************
 * @brief  开启OLED显示(没测试) 
 * @param  None							 
 * @retval None
 *********************************************/	  
void OLED_Display_On(void)
{
	/*              (SET DCDC)  (DCDC ON) (DISPLAY ON)   */
//	static u8 OledDisplayOnCmd[] = {0X8D, 0X14, 0XAF};
//	const u16 OledDisplayOnCmdLength = strlen((const char *)OledDisplayOnCmd);
//	
//	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_CMD, I2C_MEMADD_SIZE_8BIT, OledDisplayOnCmd, OledDisplayOnCmdLength);
}  
/**********************************************
 * @brief  关闭OLED显示(没测试) 
 * @param  None							 
 * @retval None
 *********************************************/	
void OLED_Display_Off(void) {
//	static u8 OledDisplayOffCmd[] = {0X8D, 0X10, 0XAE};
//	const u16 OledDisplayOffCmdLength = strlen((const char *)OledDisplayOffCmd);
//	
//	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_CMD, I2C_MEMADD_SIZE_8BIT, OledDisplayOffCmd, OledDisplayOffCmdLength);
}		


















