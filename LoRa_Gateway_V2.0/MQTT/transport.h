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
//#include "main.h"
#include "sys.h"

#define MQTT_MAX_RECV_LEN 256
#define MQTT_MAX_SEND_LEN 256

/**********************************Error Code*******************************************/
#define MQTT_OK                                  0
#define MQTT_ERROR_STATE                         1
#define MQTT_ERROR_TRANSMODE                     2   
#define MQTT_ERROR_TIMEOUT                       3
#define MQTT_ERROR_PARAM                         4

/**********************************MQTT���Ĳ���*****************************************/
/* for connect configuration */
#define MQTT_CONN_CLINETID                       "ATK-8266"
#define MQTT_CONN_USERNAME                       NULL
#define MQTT_CONN_PASSWORD                       NULL
#define MQTT_CONN_KEEPALIVE                      60

/* for subscribe configuration */
#define MQTT_SUB_TOPIC                           "gateway/1/cmd"


/* for publish configuration */
#define MQTT_PUB_TOPIC                           "gateway/1/data"
#define MQTT_MSG_HELLO                           "this is ATK8266"
#define MQTT_MSG_UPLOAD                          "{\"GateWayID\":1,\"NodeID\":0,\"Flag\":1,\"Temp\":20,\"Humi\":70,\"Battery\":100}"
/***************************************************************************************/

/* state of mqtt connection */
typedef enum
{
	MQTT_IDLE = 0,
	MQTT_NOCONN,
	MQTT_LISTEN,
}MqttState_t;


extern u8  MqttRxPayload[MQTT_MAX_RECV_LEN];   /* MQTT Rx �����е����ݸ��� */
extern u16 MqttRxPayloadLen;                   /* MQTT Rx �����е����ݸ��س��� */
extern u8  MqttTxPayload[MQTT_MAX_SEND_LEN];   /* MQTT Tx �����е����ݸ��� */
extern u16 MqttTxPayloadLen;                   /* MQTT Tx �����е����ݸ��س��� */
extern u8  MQTT_RX_BUF[MQTT_MAX_RECV_LEN];     /* MQTT���ͺͽ��յ����ݱ������� */


extern MqttState_t MqttState;	      /* ��ǰMQTT����״̬ */
extern u8 cTxPingCount;             /* �������ͼ��� */



int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);

u8 MQTT_Connect(u8* clientID, u8 isCleanSession, u8* username, u8* password, u8 keepAliveInterval, u8 waitTime);
void MQTT_Disconnect(void);
u8 MQTT_SubSCribeTopic(u8* topic, u8 reqQoS, u8 waitTime);
u8 MQTT_PublishMsg(u8* topic, u8* msg, u8 msgLen, u8 waitTime);


