/**********************************************
 * Json format processing
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/	
 
#include "myjson.h"
#include <jansson.h>
#include "string.h"

/* 各命令参数的名字 */
u8 KeyName[10][32] = {"GateWayID", "NodeID", "Flag", "Temp", "Humi", "Battery", NULL};

/* 自己实现的 int 数字转 char* */
u8* myitoa(int IntNum, u8* Str, u8 radix);
/* 解析json字符串 */
u8 DecomposeJsonStr(const u8* jsonStr, u16 jsonStrLen, u8* data, u16* dataLen);
/* 合成json字符串 */
u8 ComposeJsonStr(u8* jsonStr, u16* jsonStrLen, u8* data, u16 dataLen);

/**********************************************
 * @brief  解析Json格式，分离出LoRa Payload
           需要解析的Json str 一定是命令(下行链路)
 * @param  jsonStr       : string of json format.
           jsonStrLen    : length of jsonStr.
					 data          : [out] LoRa payload.
					 dataLen       : [out] length of LoRa payload.
 * @retval JSON_OK       : form LoRa payload successfully.
           JSON_ERROR_x  : error code, definition in "myjson.h".
 *********************************************/
u8 DecomposeJsonStr(const u8* jsonStr, u16 jsonStrLen, u8* data, u16* dataLen)
{
	json_t *root;
	json_t *DatagramFlag;	/* 数据报类型(json_t) */
	json_error_t j_error;
	
	u8 GateWayID,NodeID;
	u8 Temp,Humi,Battery;
	u8 Led,Pump;
	const u8 *Lcd;
	u8 LcdLen = 0;
	u8 iDatagramFlag = 0; /* 数据报类型(int) */
	
	u8 cRc = JSON_OK;	/* return value */
	u8 i;

	
	if(jsonStr[0]!='{')//不是标准的json格式
	{
		printf("不是标准的json字符串\r\n");
		return JSON_ERROR_FORMAT;
	}
	
	root=json_loadb((const char*)jsonStr, jsonStrLen, JSON_ALLOW_NUL, &j_error);//将字符串转换成json，都是ASCII
	if(!root)	{
		printf("解析json数据发生错误，");
		printf("错误代码 :%s\r\n",j_error.text);
		json_decref(root);
		return JSON_ERROR_LOAD;
	}
	
	// 先判断数据报的类型是下发还是上传
	DatagramFlag=json_object_get(root,"Flag");
	if(!json_is_integer(DatagramFlag))	{
		printf("分离数据报类型出现错误\r\n");
		json_decref(root);
		return JSON_ERROR_DATAFLAG;
	}
	
	iDatagramFlag = (int)json_integer_value(DatagramFlag);
	printf("datagramflag:%d\r\n", iDatagramFlag);
	
	
	switch(iDatagramFlag)
	{
		case 1:     /* 上传数据(用不到) */
		
			/* 这种方法局限性大 */
			//json_unpack(root, "{s:i,s:i,s:i,s:i,s:i}","GateWayID",&GateWayID,"NodeID", &NodeID,"Temp", &Temp,"Humi", &Humi,"Battery", &Battery);

      //将他们单独分离(省略了检错)
      GateWayID = json_integer_value(json_object_get(root,"GateWayID"));
			NodeID = json_integer_value(json_object_get(root,"NodeID"));
			Temp = json_integer_value(json_object_get(root,"Temp"));
			Humi = json_integer_value(json_object_get(root,"Humi"));
			Battery = json_integer_value(json_object_get(root,"Battery"));

			printf("\r\nGateWayID:%d, NodeID:%d\r\n",(int)GateWayID,(int)NodeID);   
			printf("Temp:%d, Humi:%d, Battery:%d \r\n",(int)Temp,(int)Humi,(int)Battery);  
			
			/* 合成 LoRa数据报payload */
			data[0] = GateWayID;
			data[1] = NodeID;
			data[2] = Temp;
			data[3] = Humi;
			data[4] = Battery;
			
			*dataLen = 5;
			break;
			
		case 2:     /* 下发命令 */
      GateWayID = json_integer_value(json_object_get(root,"GateWayID"));
			NodeID = json_integer_value(json_object_get(root,"NodeID"));
      Led = json_integer_value(json_object_get(root,"Led"));
			Pump = json_integer_value(json_object_get(root,"Pump"));
			Lcd = json_string_value(json_object_get(root,"Lcd"));
			LcdLen = strlen(Lcd);
			
			printf("\r\nGateWayID:%d, NodeID:%d\r\n",(int)GateWayID,(int)NodeID);   
			printf("Led:%d, Pump:%d, Lcd:%s \r\n",(int)Led,(int)Pump,Lcd); 

			/* 合成 LoRa数据报payload */
			data[0] = GateWayID;
			data[1] = NodeID;
			data[2] = Led;
			data[3] = Pump;
			data[4] = LcdLen;
			for (i = 0; i < LcdLen; i++) {
				data[i+5] = Lcd[i];
			}
			*dataLen = i + 5;
			break;
			
		default:
			printf("illegal datagram flag! \r\n");
			cRc = JSON_ERROR_DATAFLAG;
			break;
	}
	
	json_decref(root);	/* 不要用free，用free()会导致Heap溢出 */
	return cRc;
}


/*
{"GateWayID":1,"NodeID":0,"Flag":1,"Temp":20,"Humi":70,"Battery":100}
*/
/**********************************************
 * @brief  根据收到的 LoRa payload 生成Json字符串
           用于LoRa结点向上传输传感数据时(上行链路)
 * @param  jsonStr       : [out]string of json format.
           jsonStrLen    : [out]length of jsonStr.
					 data          : LoRa payload.
					 dataLen       : length of LoRa payload.
 * @retval JSON_OK       : form LoRa payload successfully.
           JSON_ERROR_x  : error code, definition in "myjson.h".
 *********************************************/
u8 ComposeJsonStr(u8* jsonStr, u16* jsonStrLen, u8* data, u16 dataLen) {	
	u8 index = 0;
	u8 j = 0;
	u8 cRc = JSON_OK; /* return value */

	/* pvPortMalloc() 不能用于 ISR 中 */
	u8* tDataStr = (u8 *)mymalloc(4);	/* 4 Byte */
	
	jsonStr[index++] = '{';
	for (j = 0; j < dataLen; j++) {
		if (KeyName[j] == NULL) { /* 判断两个数组长度不同 */
			printf("error\n");
			cRc = JSON_ERROR_FORMAT;
		}
		
		memset(tDataStr, 0x00, sizeof(tDataStr));
		
		jsonStr[index++] = '\"';
		memcpy(jsonStr + index, (const char *)KeyName[j], strlen((const char *)KeyName[j]));
		index += strlen((const char *)KeyName[j]);
		jsonStr[index++] = '\"';
		jsonStr[index++] = ':';
		myitoa(data[j], tDataStr, 10);
		memcpy(jsonStr + index, tDataStr, strlen(tDataStr));
		index += strlen((const char *)tDataStr);
		
		if (j != dataLen - 1) {
			jsonStr[index++] = ',';
		}
	}
	
	jsonStr[index++] = '}';
	*jsonStrLen = index;
	
	myfree(tDataStr);	/* free temp variable */
	
	return cRc;

}



//	u8 str[]="{\"cx\":20, \"cy\":33}";
//	convert_ascii2json_test(str);	
/**
 * CJSON测试函数
 * @ReceiveBuffer 要转换成JSON的字符串
 * @return NULL
 * @note  
 */
u8 *jsonstr="{\"cx\":20, \"cy\":33}";
json_error_t j_error;
void convert_ascii2json_test(char *ReceiveBuffer)
{
 json_t *root;
 int data1,data2;
 printf("receive_buffer:%s\r\n", ReceiveBuffer);
 if(ReceiveBuffer[0]=='{')
 {  
  root=json_loads(ReceiveBuffer,0,&j_error);//将字符串转换成json，都是ASCII
	if(!root)	{
		printf("error in build json_t\r\n");
		printf("error code :%s\r\n",j_error.text);
	}
	
  json_unpack(root, "{s:i, s:i}","cx", &data1,"cy", &data2);

  printf("data1:%d, data2:%d\r\n",(int)data1,(int)data2);    
  json_delete(root);//不要用free，用malloc()&free()会导致Heap溢出
  //free(root);
 }
 else
 {
  printf("no json str\r\n");
 }
}

/**********************************************
* @brief  int 数字转 String 数字
          (like .toString in java)
* @param  IntNum     : int 类型的数字
          Str        : 转换后的 String 数字
          radix      : 进制(目前仅支持 10 进制)
* @retval u8*        : 转换后的 String 数字 指针(同 Str)
*********************************************/
u8* myitoa(int IntNum, u8* Str, u8 radix) {
	u8* ptr = Str;
	int num = IntNum;
	u8 *left = NULL, *right = NULL;
	if (radix != 10) {
		return ptr;
	}
	if (num == 0) {
		*(ptr++) = '0';
		*ptr = '\0';
		return Str;
	}

	if (num < 0) {
		*(ptr++) = '-';
		num *= -1;
	}
	left = ptr; /* ptr point to the first abc */

	while (num != 0) {
		*(ptr++) = (u8)((num % 10) + '0');
		num /= 10;
	}
	right = --ptr; /* ptr point to the last abc */

	/* reverse */
	while (left != right) {
		u8 temp;

		temp = *left;
		*left = *right;
		*right = temp;

		left++;
		if (left == right) {
			break;
		} else {
			right--;
		}
	}

	*(++ptr) = '\0';

	return Str;
}

