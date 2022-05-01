/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 公用驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"			
#include "led.h"   	 
//#include "key.h"	 	 	 	 	 
	 
#include "malloc.h"
#include "string.h"    

/***********************************状态定义********************************************/
#define WIFI_OK                         0
#define WIFI_ERROR                      1

#define WIFI_STA_CONNECTED              1
#define WIFI_STA_NOCONNECTED            0
/***********************************Error code********************************************/
        
				
/***********************************联网参数********************************************/

/* ESP8266 WiFi STA模式连接的参数，外部网络参数 */
#define WIFI_EXAP_SSID                  (const char *)"iPhone"
#define WIFI_EXAP_PASSWD                (const char *)"123456789"
#define WIFI_EXAP_ENCRYPTION            (const char *)"wpawpa2_aes"

/* ESP8266 WiFi AP模式连接的参数，可自己设定 */
#define WIFI_AP_SSID                    (const char *)"ATK-ESP8266"
#define WIFI_AP_PASSWD                  (const char *)"12345678"
#define WIFI_AP_ENCRYPTION              (const char *)"wpawpa2_aes"

/* MQTT 服务端的IP和端口 */
#define MQTT_IP                         (const char *)"116.62.17.105"
#define MQTT_PORT                       (const char *)"1883"



/***********************************结构体定义*****************************************/

/*!
 * Wifi driver internal state machine states definition
 */
typedef enum
{
    WIFI_IDLE = 0,
		WIFI_SLEEP,
		WIFI_STA_AP,		
    WIFI_STA_TCP_CLIENT,
		WIFI_STA_TCP_SERVER,
		WIFI_STA_UDP,
		WIFI_AP_TCP_CLIENT,
		WIFI_AP_TCP_SERVER,
		WIFI_AP_UDP,
		WIFI_AP_STA_TCP_CLIENT,
		WIFI_AP_STA_TCP_SERVER,
		WIFI_AP_STA_UDP,
}WifiState_t;

/*!
 * Wifi transport mode
 */
typedef enum
{
    WIFI_MODE_TRANS = 0,
		WIFI_MODE_NORMAL,

}WifiTransptMode_t;


struct Wifi_s
{

		WifiState_t State;
		WifiTransptMode_t TransptMode;
   
		void    ( *Init )( void );
		
		void    ( *SetTcpClient )( const char* ssid, const char* password, const char* ip, const char* port, u8 isTrans );

		u8      ( *QuitTrans )( void );
		
		u8      ( *SetTrans )( void ); 
		
		u8*     ( *SendCmd )( const char* cmd, const char* ack, unsigned short waitTime );
		
		u8      ( *SendData )( const char* data );
		
		
};
/***************************************************************************************/




void ATK8266_Init(void);
void ATK8266_SetTcpClient( const u8* ssid, const u8* password, const u8* tcpIp, const u8* tcpPort, u8 isTrans );
u8* ATK8266_Check_Cmd(u8* str);
u8* ATK8266_Send_Cmd(u8* cmd, u8* ack, u16 waitTime);
u8 ATK8266_Send_Data(u8 *data);
u8* ATK8266_Send_Cmd_getACK(u8* cmd, u8* targetStr, u16 waitTime);
u8 ATK8266_QuitTransMode(void);
u8 ATK8266_SetTransMode(void);



void atk_8266_at_response(u8 mode);



//用户配置参数




extern struct Wifi_s Wifi;
#endif





