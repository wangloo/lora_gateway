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

u8 OledFrameBuf[8][128] = {0};         /* ��ӦSSD1306��128*64 bit��GDDRAM */



	
void OLED_Refreash(void); 
 	  

/**********************************************
 * @brief  ��ʼ��SSD1306	
	         �ڳ�ʼ��GPIO֮�����
 * @param  None							 
 * @retval None
 *********************************************/			    
void OLED_Init(void) { 	

	/* 	GPIO�ĳ�ʼ����MX_GPIO_Init()�н��� */
	
	/* ��ʼ������������붨��Ϊ ȫ�ֱ��� �� �ֲ���̬������
	   ������Ϊ�ֲ������������ OLED_Init ִ�н�����DMAû�д������ */
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
 * @brief  ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
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
 * @brief  ���ָ��������
 * @param  col0  : �е�ַ-�� [0, 127]
	         page0 : ҳ��ַ-�� [0, 7]
           col1  : �е�ַ-�� [0, 127]
	         page1 : ҳ��ַ-�� [0, 7]
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
 * @brief  ��ָ��λ�û�һ����
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
 * @brief  ��ָ��λ�û�һ���߶Σ���Դ������ԭ�ӣ�
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
	delta_x=x1-x0; //������������ 
	delta_y=y1-y0; 
	uRow=x0; 
	uCol=y0; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		OLED_Draw_Dot(uRow,uCol);//���� 
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
 * @brief  OLEDȫ��������(8��)
 * @param  None							 
 * @retval None
 *********************************************/			  
void OLED_Fill_All(void)  {  
	u8 i, j;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 128; j++) {
			OledFrameBuf[i][j] = (1 << 0);	/* ÿ��page���� bit0 Ϊ 1 */
		}
	}	
}

/**********************************************
 * @brief  ��ָ��λ����ʾһ���ַ�
 * @param  col      : [0, 128-1]������16*8��8*6������
           page     : [0, 6]    ������16*8��8*6������
           chr      : ��ʾ���ַ�
           size     : �����С( FONT_SIZE_8, FONT_SIZE_16 )
					 isReverse: �Ƿ���( DISPLAY_NORMAL, DISPLAY_REVERSE )
 * @retval None
 *********************************************/
void OLED_ShowChar(u8 col, u8 page, u8 chr, u8 size, u8 isReverse)
{      	
	unsigned char c=0, i=0;	
	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ	
	
	if (col > 128-1) {
		col = 0;
		page = page+2;	/* ������Ҫ�ģ��ж�SIZE */
	}
	if (size == FONT_SIZE_16) {	/* FONT_SIZE_16 */
		if (isReverse == DISPLAY_NORMAL){	/* ������ʾ */
			for (i = 0; i < 8; i++){
					OledFrameBuf[page][col+i]   = F8X16[c*16+i];		
					OledFrameBuf[page+1][col+i] = F8X16[c*16+i+8];	
			}				
		} else {	/* ���� */
				for (i = 0; i < 8; i++){
					OledFrameBuf[page][col+i]   = ~F8X16[c*16+i];		
					OledFrameBuf[page+1][col+i] = ~F8X16[c*16+i+8];	
				}			
		}
	} else if (size == FONT_SIZE_8) {	/* FONT_SIZE_8 */
			if (isReverse == DISPLAY_NORMAL) { /* ������ʾ */
				for (i = 0; i < 6; i++) {
					OledFrameBuf[page][col+i] = F6x8[c][i];
				}
			} else { /* ���� */
				for( i = 0; i < 6; i++) {
					OledFrameBuf[page][col+i] = ~F6x8[c][i];
				}
			}
		}
}

/**********************************************
 * @brief  ��ָ��λ����ʾ�ַ���
 * @param  col      : [0, 1023 - strLen], ���ಿ�ֻỻ����ʾ
           page     : [0, 6]
           str      : ��ʾ���ַ���
					 strLen   : �ַ����ĳ���
           size     : �����С( FONT_SIZE_8, FONT_SIZE_16 )
					 isReverse: �Ƿ���( DISPLAY_NORMAL, DISPLAY_REVERSE )
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
* @brief  ��ָ��λ����ʾ���������֣�֧�ָ�����
* @param  col       : [0, 1023 - strLen], ���ಿ�ֻỻ����ʾ
          page      : [0, 6]
          num       : ��ʾ������
			    numLen    : ���ֵĳ��ȣ��������ţ�
          size      : �����С( FONT_SIZE_8, FONT_SIZE_16 )
				  isReverse : �Ƿ���( DISPLAY_NORMAL, DISPLAY_REVERSE )
					isFillZero: �Ƿ��� numLen > ��ʵλ�� ʱʹ�� 0 ���
					            ����������� 0 ( ����δ����, users need to notice!!)
* @retval None
*********************************************/
void OLED_ShowNum(u8 col, u8 page, s32 num, u8 numLen, u8 size, u8 isReverse, u8 isFillZero) {
	u8* tpStr;
	u8  tcStrLen = 0;   /* ת������ַ������� */
	u8  deltaLen = 0;   /* ��Ҫ���� 0 ���� */
	
	tpStr = (u8 *)mymalloc(numLen + 1);	/* ȷ�������ܴ��� */
	
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
 * @brief  ��ʾ����
 * @param  col   : [0, 127]  
           page  : [0, 7]
           index : �ֿ������
 * @retval None
 *********************************************/
void OLED_ShowCHinese(u8 col, u8 page, u8 index) {      			    

	/* ��ʱ����Ҫ */
}

/**********************************************
 * @brief  ��ʾͼƬ 
 * @param  col0  : �е�ַ-�� [0, 128]
           page0 : ҳ��ַ-�� [0, 8]
		  		 col1  : �е�ַ-�� [0, 128]
  			 	 page1 : ҳ��ַ-�� [0, 8]
           BMP   : �洢ͼƬ������
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
 * @brief  OLEDˢ���Դ� 
 * @param  None							 
 * @retval None
 * @note   ������Ŀֻ��Ҫ����һ�Σ��Ժ���IIC�������
 *         �жϷ������л��ظ����ã��ﵽ�Զ�ˢ��
 *         ���ڿ��Ըĳ� ��ʱ��ˢ�� �� DMAѭ������
 *********************************************/	  
void OLED_Refreash(void) {	
	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_DATA, I2C_MEMADD_SIZE_8BIT, OledFrameBuf[0], OLED_FRAMEBUF_LEN);	
}




/**********************************************
 * @brief  ����OLED��ʾ(û����) 
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
 * @brief  �ر�OLED��ʾ(û����) 
 * @param  None							 
 * @retval None
 *********************************************/	
void OLED_Display_Off(void) {
//	static u8 OledDisplayOffCmd[] = {0X8D, 0X10, 0XAE};
//	const u16 OledDisplayOffCmdLength = strlen((const char *)OledDisplayOffCmd);
//	
//	HAL_I2C_Mem_Write_DMA(&hi2c1, OLED_IIC_ADDR, OLED_CMD, I2C_MEMADD_SIZE_8BIT, OledDisplayOffCmd, OledDisplayOffCmdLength);
}		


















