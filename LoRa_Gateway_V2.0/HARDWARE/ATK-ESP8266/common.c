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

#include "common.h"
#include "transport.h"

struct Wifi_s Wifi =
{
		WIFI_IDLE,             /* wifi state : init idle mode*/
		WIFI_MODE_NORMAL,      /* wifi transport mode: init normal mode */
		
    ATK8266_Init,          /* wifi init function */
		ATK8266_SetTcpClient,  /* set wifi to tcp client mode function */
		ATK8266_QuitTransMode,
		ATK8266_SetTransMode,
		ATK8266_Send_Cmd,
		ATK8266_Send_Data,

};

/* 事件标志组句柄, definition in freertos.c */
extern EventGroupHandle_t EventGroupHandler; 

void ATK8266_Init(void);
u8* ATK8266_Check_Cmd(u8* str);
u8* ATK8266_Send_Cmd(u8* cmd, u8* ack, u16 waitTime);
u8 ATK8266_Send_Data(u8 *data);
void ATK8266_SetTcpClient(const u8* ssid, const u8* password, const u8* tcpIp, const u8* tcpPort, u8 isTrans);

//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART2_RX_BUF);	//发送到串口
		if(mode)USART2_RX_STA=0;
	} 
}

/***************************************************************
 * \brief  发送命令后,检测接收到的应答
 *         
 * \param  [IN] str      :  期待的应答结果
 *
 * \retval      0        :  收到期望的ack
 *              1        :  检查ack失败
 ***************************************************************/
u8* ATK8266_Check_Cmd(u8* str) {
	u8* res = NULL;
	
	if(USART2_RX_STA&0X8000)		/* 串口接收到一次数据 */
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF] = 0; /* 添加结束符 */
		res = strstr((const char*)USART2_RX_BUF,(const char*)str);

	} 
	return res;
}

/***************************************************************
 * \brief  通过串口向ATK-ESP8266发送AT指令
 *         返回ack的索引
 * \param  [IN] cmd      :  发送的命令字符串
 * \param  [IN] ack      :  期待的应答结果,如果为空,则表示不需要等待应答
 * \param  [IN] waitTime :  等待时间(单位:10ms)
 *
 * \retval NULL          :  发送失败
 *         (u8 *)        :  收到目标ack，返回索引
 ***************************************************************/
u8* ATK8266_Send_Cmd(u8* cmd, u8* ack, u16 waitTime)
{
	u8* res = NULL; 
	
	if (Wifi.TransptMode == WIFI_MODE_TRANS) {
		/* 透传模式不能发送命令！ */
		return NULL;
	}
	
	USART2_RX_STA=0;
	u2_printf("%s\r\n",cmd);	/* 发送命令(AT指令必须加换行) */
	
	if (ack && waitTime)		//需要等待应答
	{
		while (--waitTime) {
			HAL_Delay(10);
			if (USART2_RX_STA&0X8000) { /* 串口接收完成标记 */
				res = ATK8266_Check_Cmd(ack);
				if (res != NULL) {
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART2_RX_STA=0;
			} 
		}
		if (waitTime == 0) { /* 超时 */
			printf("send cmd:\"%s\" timeout!\r\n", cmd);
			res = NULL;
		}			
	}
	return res;
} 

/***************************************************************
 * \brief  通过串口向ATK-ESP8266发送数据
 *         (本项目没有使用, 选用transport_sendPacketBuffer())
 * \param  data       :  发送的数据
 *
 * \retval WIFI_OK    :  发送成功
 *         WIFI_ERROR :  发送失败
 ***************************************************************/
u8 ATK8266_Send_Data(u8 *data)
{
	u8 res = WIFI_OK; 
	
	if (Wifi.TransptMode == WIFI_MODE_NORMAL) {
		/* 只有透传模式才能发数据！ */
		return WIFI_ERROR;
	}
	
	USART2_RX_STA=0;
	u2_printf("%s",data);	/* 发送命令(数据不需要添加换行) */
	
	/* 本项目没有使用故没有实现更多，
	   如果需要可自行实现如检查ack、超时等功能 */
	/* ... */
	return res;
}

/***************************************************************
 * \brief  ATK-ESP8266退出透传模式
 *
 * \retval WIFI_OK       :  退出成功
 *         WIFI_ERROR    :  退出失败
 ***************************************************************/
u8 ATK8266_QuitTransMode(void) {

	if (Wifi.TransptMode == WIFI_MODE_TRANS) {
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(15);				       	/* 大于串口组帧时间(10ms) */
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(15);					      /* 大于串口组帧时间(10ms) */
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(500);					      /* 等待500ms */

		if (ATK8266_Send_Cmd("AT", "OK", 20) == NULL) { /* 退出透传判断 */
			return WIFI_ERROR;
		}
		Wifi.TransptMode = WIFI_MODE_NORMAL;
		return WIFI_OK;
		
	} else if (Wifi.TransptMode == WIFI_MODE_NORMAL){
		/* 原本不在透传模式, 不需要退出 */
		return WIFI_OK;
	}

}

/***************************************************************
 * \brief  设置为ATK-ESP8266透传模式
 *
 * \retval WIFI_OK       :  退出成功
 *         WIFI_ERROR    :  退出失败
 ***************************************************************/
u8 ATK8266_SetTransMode(void) {

	if (Wifi.TransptMode == WIFI_MODE_NORMAL) {
	  /* set transparent transport mode and start transparent transport. */
		if ((ATK8266_Send_Cmd("AT+CIPMODE=1", "OK", 200) == NULL) || \
		    (ATK8266_Send_Cmd("AT+CIPSEND","OK",20) == NULL)) {       
			return WIFI_ERROR;
		}
		
		Wifi.TransptMode = WIFI_MODE_TRANS;
		return WIFI_OK;
		
	} else if (Wifi.TransptMode == WIFI_MODE_TRANS){
		/* 已处于透传模式 */
		return WIFI_OK;
	}
}



void ATK8266_Init(void)
{

  while(Wifi.SendCmd("AT","OK",20) == NULL)//检查WIFI模块是否在线
	{
		printf("尝试连接ATK-ESP8266...\r\n");
		
		/* 考虑当前处于透传模式的情况 */
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(15);				       	/* 大于串口组帧时间(10ms) */
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(15);					      /* 大于串口组帧时间(10ms) */
		while((USART2->SR&0X40)==0);	/* 等待发送空 */
		USART2->DR='+';      
		HAL_Delay(500);					      /* 等待500ms */
		ATK8266_Send_Cmd("AT+CIPMODE=0", "OK", 20);	/* 关闭透传模式	*/

		HAL_Delay(300);

	} 
	printf("\r\nATK-ESP8266连接成功！\r\n\r\n");
	
	while(Wifi.SendCmd("ATE0", "OK", 20) == NULL);	/* 关闭回显 */

}

void ATK8266_SetTcpClient(const u8* ssid, const u8* password, const u8* tcpIp, const u8* tcpPort, u8 isTrans) {
	u8* tpAPCmd;	     /* 临时存放 Client 连接外部AP的指令 */
	u8* tpTcpCmd;      /* 临时存放 Client 连接TCP服务端的指令 */
	
	tpAPCmd = mymalloc(32);
	tpTcpCmd = mymalloc(32);
	
	Wifi.SendCmd("AT+CWMODE=1","OK",50);	 /* set STA mode */
	Wifi.SendCmd("AT+RST","OK",50);		   /* close DHCP(only AP mode) */
	
	HAL_Delay(4000);                           /* delay 4s for reset */   
	
	sprintf((char *)tpAPCmd,"AT+CWJAP=\"%s\",\"%s\"",ssid, password);	/* 设置无线参数:ssid,密码 */
	while(Wifi.SendCmd(tpAPCmd,"OK",300) == NULL);					  /* connect to external AP */
	/* 判断AP是否连接成功 */
	Wifi.SendCmd("AT+CIPSTA?",":",50);	    /* check status of AP connection */
	if(ATK8266_Check_Cmd("+CIPSTA:") == NULL) {
		printf("连接AP失败! 请检查\r\n");
		return;
	}
	printf("\r\n连接无线AP成功, ssid = %s, password = %s\r\n", ssid, password);
  xEventGroupSetBits(EventGroupHandler, EVENTBIT_WIFI_CONNECTED);	/* 修改事件标志位 */
	
	/* 0: 单连接, 1:多连接 */
	Wifi.SendCmd("AT+CIPMUX=0", "OK", 20);             
	
	sprintf((char *)tpTcpCmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", tcpIp, tcpPort);    /* 设置目标TCP服务器 */
	while(Wifi.SendCmd(tpTcpCmd, "OK", 200) == NULL);
	/* 判断TCP是否连接成功 */
	Wifi.SendCmd("AT+CIPSTATUS",":",50);	/* check status of TCP connection */
	if(ATK8266_Check_Cmd("STATUS:3") == NULL) {
		printf("建立TCP失败! 请检查\r\n");
		return;
	} 
	Wifi.State = WIFI_STA_TCP_CLIENT;
	printf("\r\n建立TCP连接成功, host = %s, port = %s\r\n",tcpIp, tcpPort);	


	/* need change transport mode? */
	if (isTrans == WIFI_MODE_TRANS) {

		Wifi.SetTrans();
	}
	
	/* free temp variable */
	myfree(tpTcpCmd);
	myfree(tpAPCmd);
}











































