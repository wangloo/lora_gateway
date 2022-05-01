/**********************************************
 * Json format processing
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/	
 
#ifndef __MYJSON_H
#define __MYJSON_H
#include "sys.h"

/**********************************Error Code*******************************************/

#define JSON_OK                                  0
#define JSON_ERROR_FORMAT                        1
#define JSON_ERROR_LOAD                          2   
#define JSON_ERROR_DATAFLAG                      3

/***************************************************************************************/


/* �Լ�ʵ�ֵ� int ����ת char* */
u8* myitoa(int IntNum, u8* Str, u8 radix);
/* ����json�ַ��� */
u8 DecomposeJsonStr(const u8* jsonStr, u16 jsonStrLen, u8* data, u16* dataLen);
/* �ϳ�json�ַ��� */
u8 ComposeJsonStr(u8* jsonStr, u16* jsonStrLen, u8* data, u16 dataLen);

/* test jasson lib */
void convert_ascii2json_test(char *ReceiveBuffer);
#endif

