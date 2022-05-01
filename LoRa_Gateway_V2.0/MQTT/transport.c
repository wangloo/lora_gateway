/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/
 
 
/*******************************************************************************
 * 暂时只支持一次订阅一个Topic,
 * 2021/05/14
 *******************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "transport.h"  	/* MQTT */
#include "common.h"       /* WiFi-STA TCP */

//#include "sys.h"
#include "usart.h"		
#include "led.h"	 
#include "malloc.h"
#include "string.h"    
#include "MQTTPacket.h"
#include "tim.h"	/* TIM_Set() */

u8  MqttRxPayload[MQTT_MAX_RECV_LEN];   /* MQTT Rx 报文中的数据负载 */
u16 MqttRxPayloadLen = 0;               /* MQTT Rx 报文中的数据负载长度 */
u8  MqttTxPayload[MQTT_MAX_SEND_LEN];   /* MQTT Tx 报文中的数据负载 */
u16 MqttTxPayloadLen = 0;               /* MQTT Tx 报文中的数据负载长度 */
u8  MQTT_RX_BUF[MQTT_MAX_RECV_LEN];     /* MQTT发送和接收的数据报缓冲区 */

u8 cTxPingCount = 0;	/* 心跳发送计数 */

/* MQTT state, init value = MQTT_NOCONN */
MqttState_t MqttState = MQTT_NOCONN;

/* 事件标志组句柄, definition in freertos.c */
extern EventGroupHandle_t EventGroupHandler; 

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;        
	u16 j;
	
	// for循环发送来避免遇到0x00丢失之后数据的问题
	for(j=0; j < buflen; j++)                     //循环发送数据
	{
		while((USART2->SR&0X40)==0);     //循环发送,直到发送完毕   
		USART2->DR=buf[j];  
//			printf(" %d",buf[j]);	/* for debug */
	}
	while((USART2->SR&0X40)==0);	

	return rc;
}


/**********************************************
 * @brief  copy some bytes from USART2_RX_BUF to buf.
           called by MQTTPacket_read().
 * @param  buf      : buffer to store wanted data.
           count    : number of bytes wanted.
 * @retval -1       : USART2 is receiving data.
           else     : same as count.
 *********************************************/
int transport_getdata(unsigned char* buf, int count) {

	if(USART2_RX_STA&0x8000)		//接收到一次数据了
	{		
		memcpy(buf, (void*)&USART2_RX_BUF[read_buf_len], count);
		read_buf_len += count;	/* read_buf_len is an important variable,
															 it must be set 0 when finishing reading 
															 a whole msg or handling a wrong msg format. */
		return count;
	}
	else return -1;
}



/**********************************************
 * @brief  Connect to MQTT serve within waitTime.
           
 * @param  waitTime  : limit-time of connect action(unit 10ms).
 * @retval MQTT_OK            : connection successful.
           MQTT_ERROR_TIMEOUT : connection failed as timeout.
 *********************************************/
u8 MQTT_Connect(u8* clientID, u8 isCleanSession, u8* username, u8* password, \
                u8 keepAliveInterval, u8 waitTime) {
	u8* tpMsgBuf;
	u8  tcMsgBufLen;
	u8  cRtnCode;
	
	/* MQTT连接报文参数 */
	MQTTPacket_connectData xMqttConnMsg = MQTTPacket_connectData_initializer; 
	
	if (Wifi.State != WIFI_STA_TCP_CLIENT) {
		printf("TCP connection is necessary before MQTT connection\r\n");
		return MQTT_ERROR_STATE;
	}
	if (Wifi.TransptMode !=  WIFI_MODE_TRANS) {
		printf("Transparent-Transport mode need to be set\r\n");
		return MQTT_ERROR_TRANSMODE;
	}
	
  tpMsgBuf = mymalloc(200);       /* Length of CONNECT message is less than 200 Bytes */
	tcMsgBufLen = 200;              /* init tcSrilzdConnMsgLen */
	memset(tpMsgBuf,0,tcMsgBufLen);	/* init tpSrilzdConnMsg */
	
	
	/* some paramters in CONNECT Message */
	/* 暂不支持遗嘱相关设置 */
	xMqttConnMsg.clientID.cstring = clientID;    /* client ID */
	xMqttConnMsg.cleansession = isCleanSession;     
//	xMqttConnMsg.willFlag = willFlag;           /*  think about it in future */
//	xMqttConnMsg.will = will;
	xMqttConnMsg.username.cstring = username;     /* username (control by Mqtt-Server) */
	xMqttConnMsg.password.cstring = password;     /* password (control by Mqtt-Server) */
	xMqttConnMsg.keepAliveInterval = keepAliveInterval;         /* within 120s, the client must sent data or ping message 
	                                                 OR the Mqtt server will close the connection */				
	
	/* form MQTT CONNECT massage */
	tcMsgBufLen = MQTTSerialize_connect(tpMsgBuf, tcMsgBufLen, &xMqttConnMsg);
	/* USART2 send CONNECT massage */
	transport_sendPacketBuffer(0, tpMsgBuf, tcMsgBufLen);	
	
	u8 sessionPresent, connackRes;
	while (--waitTime) {	
		if (USART2_RX_STA&0x8000) {
			if (MQTTPacket_read(tpMsgBuf, tcMsgBufLen, transport_getdata) == CONNACK) {
				if (MQTTDeserialize_connack(&sessionPresent, &connackRes, tpMsgBuf, tcMsgBufLen)==1 && connackRes== 0) {
					printf("Connect MQTT successfully!\r\n\r\n");
					break;
				}
			}
			USART2_RX_STA = 0;
		}
		HAL_Delay(10);
	}
	
	/* recieve CONNACK timeout */
	if (waitTime == 0) {
		printf("connect MQTT timeout\r\n");
		cRtnCode = MQTT_ERROR_TIMEOUT;
	} else {
		MqttState = MQTT_IDLE;
		xEventGroupSetBits(EventGroupHandler, EVENTBIT_MQTT_CONNECTED);	/* 修改事件标志位 */
		TIM_Set(&htim3, 1);	      /* 开启TIM3，用于定时发送心跳包 */
		USART2_RX_STA = 0;
		cRtnCode = MQTT_OK;
	}
	
	myfree(tpMsgBuf);
	return cRtnCode;
}
/**********************************************
 * @brief  Disconnect to MQTT serve.
           
 * @param  null
 * @retval null
 *********************************************/
void MQTT_Disconnect(void) {
	u8* tpMsgBuf;
	u8  tcMsgBufLen;
	
  tpMsgBuf = mymalloc(200);       /* Length of SUBSCRIBE message is less than 200 Bytes */
	tcMsgBufLen = 200;              /* init tcSrilzdConnMsgLen */
	memset(tpMsgBuf,0,tcMsgBufLen);	/* init tpSrilzdConnMsg */
	
	tpMsgBuf[0] = 0xE0;
	tpMsgBuf[1] = 0x00;
	tcMsgBufLen = 2;
	
	/* USART2 send CONNECT massage */
	transport_sendPacketBuffer(0, tpMsgBuf, tcMsgBufLen);	
	
	/* no ack message to wait */
	
	MqttState = MQTT_NOCONN;	/* update MqttState to NOCONNECT */
}

/**********************************************
 * @brief  Subscribe a topic.
           
 * @param  topic     : topic name of the subscription.
           reqQos    : requested QoS: Max acceptable QoS.
           waitTime  : limit-time of subscribe action(unit 10ms).
 * @retval MQTT_OK            : connection successful.
           MQTT_ERROR_STATE   : wrong MQTT state(see MqttState_t).
					 MQTT_ERROR_TIMEOUT : wait SUBACK msg timeout.
					 MQTT_ERROR_PARAM   : paramter error.
 *********************************************/
u8 MQTT_SubSCribeTopic(u8* topic, u8 reqQoS, u8 waitTime) {
	u8* tpMsgBuf;
	u8  tcMsgBufLen;
	u8  cRtnCode = MQTT_OK;  /* MQTT_SubSCribe() return code */
	
	u8  dup = 0;            /* dup Flag: SUBSCRIBE msg dont use, set 0 */
	u8  msgID = 1;          /* SUBSCRIBE msg ID: must same as SUBACK */
	u8  topicCount = 1;     /* topic mount This SUBSCRIBE msg will subscribe */
	
	
	/* topic struct, store strings in two ways(cstring and lenstring). */
	MQTTString topicString = MQTTString_initializer;
	
	if (MqttState == MQTT_NOCONN) {
		return MQTT_ERROR_STATE;
	}
	
	topicString.cstring = MQTT_SUB_TOPIC;
	
  tpMsgBuf = mymalloc(200);       /* Length of SUBSCRIBE message is less than 200 Bytes */
	tcMsgBufLen = 200;              /* init tcSrilzdConnMsgLen */
	memset(tpMsgBuf,0,tcMsgBufLen);	/* init tpSrilzdConnMsg */
	
	/* form MQTT SUBSCRIBE massage */
	tcMsgBufLen = MQTTSerialize_subscribe(tpMsgBuf, tcMsgBufLen, dup, msgID, topicCount, &topicString, &reqQoS);
	/* USART2 send CONNECT massage */
	transport_sendPacketBuffer(0, tpMsgBuf, tcMsgBufLen);//ESP8266透传模式下发送数据到串口2
	
	
	//等待服务器答复
	unsigned short ackMsgID; /* SUBACK msg ID should be same as SUBSCRIBE msg */
	int ackCount;            /* topic count in SUBACK msg */
	int grtdQoS = -1;        /* granted QoS: max granted QoS.  init value:-1 */
	
	while (--waitTime) {	/* 未达到超时时间 */
	
		if (USART2_RX_STA&0x8000) { /* USART2 接收了一帧数据 */
		
			if (MQTTPacket_read(tpMsgBuf, tcMsgBufLen, transport_getdata) == SUBACK) {
			
				if (MQTTDeserialize_suback(&ackMsgID, 1, &ackCount, &grtdQoS, tpMsgBuf, tcMsgBufLen)==1) {
				
					if (grtdQoS != reqQoS || ackMsgID != msgID || ackCount != 1) {
						printf("illegal paramter in SUBACK msg !\r\n");
						cRtnCode = MQTT_ERROR_PARAM;
					} else {
						printf("成功订阅 Topic = %s\r\n\r\n",topicString.cstring);
						MqttState = MQTT_LISTEN;
						read_buf_len = 0;
						USART2_RX_STA = 0;
						break;
					}
				}
			}
			USART2_RX_STA = 0;	/* must clear the flag after handling  */
			read_buf_len = 0;
		}
		HAL_Delay(10);
	}
	
	/* wait SUBACK timeout */
	if (cRtnCode == MQTT_OK && waitTime == 0) {
		printf("subscribe topic timeout\r\n");
		cRtnCode = MQTT_ERROR_TIMEOUT;
	} 
	
	myfree(tpMsgBuf);
	return cRtnCode;
}
/**********************************************
 * @brief  Publish a message.
           
 * @param  topic     : which topic this msg belongs to.
           msg       : content of the message.
					 msgLen    : length of the message.
           waitTime  : limit-time of publish action(unit 10ms).
 * @retval MQTT_OK            : connection successful.
           MQTT_ERROR_STATE   : wrong MQTT state(see MqttState_t).
 *********************************************/
u8 MQTT_PublishMsg(u8* topic, u8* msg, u8 msgLen, u8 waitTime) {
	u8* tpMsgBuf;            /* temp buf */
	u8  tcMsgBufLen;         /* length of temp buf */
	u8  cRtnCode = MQTT_OK;  /* MQTT_PublishMsg() return code */

	u8  dup = 0;            /* dup Flag: duplicate send set 1, first send set 0 */
	int qos = 0;            /* qos of massage to be sent */
	u8  retained = 0;       /* no restore and no replace any retained msg */
	u8  msgID = 1;          /* SUBSCRIBE msg ID: must same as SUBACK */	
	
	MQTTString topicString = MQTTString_initializer;	/* MQTT发布/订阅的Topic结构体 */
	
	if (MqttState == MQTT_NOCONN) {
		return MQTT_ERROR_STATE;
	}
	
	topicString.cstring = topic;
	
  tpMsgBuf = mymalloc(200);       /* Length of PUBLISH message is less than 200 Bytes */
	tcMsgBufLen = 200;              /* init tcSrilzdConnMsgLen */
	memset(tpMsgBuf,0,tcMsgBufLen);	/* init tpSrilzdConnMsg */
	
	/* form MQTT PUBLISH massage */
	tcMsgBufLen = MQTTSerialize_publish(tpMsgBuf, tcMsgBufLen, dup, qos, retained, msgID, topicString, msg, msgLen);
	transport_sendPacketBuffer(0, tpMsgBuf, tcMsgBufLen);


	/* when qos=0, no need to wait for a PUBACK */
	printf("publish a msg: %s \r\nin topic: %s\r\n", msg, topic);
	
	myfree(tpMsgBuf);
	return cRtnCode;
}