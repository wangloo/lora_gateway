/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "led.h"
#include "oled.h"
//#include "oledfont.h"
#include "bmp.h"
#include "myjson.h"
#include "usart.h"
#include "tim.h"

#include "common.h"      /* WiFi-STA TCP */
#include "transport.h"   /* MQTT */
#include "MQTTPacket.h" /* struct defination MQTT_String 
                           dont't include this file in transport.h 
													 遵循一次修改编译最少源文件的原理 */

#include "sx1276-board.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


EventGroupHandle_t EventGroupHandler; /* 事件标志组句柄 */

/* USER CODE END Variables */
osThreadId LedTaskHandle;
osThreadId MqttTaskHandle;
osThreadId DisplayTaskHandle;
osThreadId LoRaTaskHandle;
osTimerId RxTimeoutTimerHandle;
osTimerId TxTimeoutTimerHandle;
osTimerId SyncWordTimeoutTimerHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* LoRa event callback */
void OnLoRaTxDone( void );
void OnLoRaRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnLoRaTxTimeout( void );
void OnLoRaRxTimeout( void );
void OnLoRaRxError( void );
void OnLoRaCadDone( bool channelActivityDetected );

/* USER CODE END FunctionPrototypes */

void StartLedTask(void const * argument);
void StartMqttTask(void const * argument);
void StartDisplayTask(void const * argument);
void StartLoRaTask(void const * argument);
void RxTimeoutTimerCallback(void const * argument);
void TxTimeoutTimerCallback(void const * argument);
void SyncWordTimeoutTimerCallback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	EventGroupHandler=xEventGroupCreate(); /* 创建事件标志组(24bit) */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of RxTimeoutTimer */
  osTimerDef(RxTimeoutTimer, RxTimeoutTimerCallback);
  RxTimeoutTimerHandle = osTimerCreate(osTimer(RxTimeoutTimer), osTimerOnce, NULL);

  /* definition and creation of TxTimeoutTimer */
  osTimerDef(TxTimeoutTimer, TxTimeoutTimerCallback);
  TxTimeoutTimerHandle = osTimerCreate(osTimer(TxTimeoutTimer), osTimerOnce, NULL);

  /* definition and creation of SyncWordTimeoutTimer */
  osTimerDef(SyncWordTimeoutTimer, SyncWordTimeoutTimerCallback);
  SyncWordTimeoutTimerHandle = osTimerCreate(osTimer(SyncWordTimeoutTimer), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of LedTask */
  osThreadDef(LedTask, StartLedTask, osPriorityIdle, 0, 128);
  LedTaskHandle = osThreadCreate(osThread(LedTask), NULL);

  /* definition and creation of MqttTask */
  osThreadDef(MqttTask, StartMqttTask, osPriorityIdle, 0, 256);
  MqttTaskHandle = osThreadCreate(osThread(MqttTask), NULL);

  /* definition and creation of DisplayTask */
  osThreadDef(DisplayTask, StartDisplayTask, osPriorityHigh, 0, 128);
  DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* definition and creation of LoRaTask */
  osThreadDef(LoRaTask, StartLoRaTask, osPriorityIdle, 0, 256);
  LoRaTaskHandle = osThreadCreate(osThread(LoRaTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartLedTask */
/**
  * @brief  Function implementing the LedTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartLedTask */
void StartLedTask(void const * argument)
{

  /* USER CODE BEGIN StartLedTask */
//	osTimerStart(RxTimeoutTimerHandle, 5000);
  /* Infinite loop */
  for(;;)
  {
		LED0=!LED0;
		//printf("led task running...\r\n");
		
    osDelay(500);
		//HAL_Delay(500);
  }
  /* USER CODE END StartLedTask */
}

/* USER CODE BEGIN Header_StartMqttTask */
/**
* @brief Function implementing the MqttTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMqttTask */
void StartMqttTask(void const * argument)
{
  /* USER CODE BEGIN StartMqttTask */
 
 	u8 qc=0;
	u8* tpMsgBuf;
	u8  tcMsgBufLen;
//	u8 MemOccupancyRate=mallco_dev.perused();
	
	tpMsgBuf = pvPortMalloc(50);	/*  50 * size_t(32 b) = 200 Bytes */
	tcMsgBufLen = 200;
	memset(tpMsgBuf, 0, tcMsgBufLen);
	
	Wifi.SetTcpClient(WIFI_EXAP_SSID, WIFI_EXAP_PASSWD, \
	                  MQTT_IP, MQTT_PORT, WIFI_MODE_TRANS);
	
	                     /* Client ID ,  清理会话,    用户名,     密码,         保持连接时间 */ 
	MQTT_Connect(MQTT_CONN_CLINETID, 1, MQTT_CONN_USERNAME, MQTT_CONN_PASSWORD, MQTT_CONN_KEEPALIVE, 200);									
	
	MQTT_SubSCribeTopic(MQTT_SUB_TOPIC, 0, 200); /* 订阅 "gateway/1/cmd" topic */
	
  /* Infinite loop */
  for(;;)
  {
		/* time to send PING msg (timer3) */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_MQTT_TXPING) == EVENTBIT_MQTT_TXPING) {
			xEventGroupClearBits(EventGroupHandler, EVENTBIT_MQTT_TXPING);
			tcMsgBufLen = MQTTSerialize_pingreq(tpMsgBuf, tcMsgBufLen);//心跳
			transport_sendPacketBuffer(0, tpMsgBuf, tcMsgBufLen);
		}
		/* receive new message */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_MQTT_RXMSG) == EVENTBIT_MQTT_RXMSG) {
			xEventGroupClearBits(EventGroupHandler, EVENTBIT_MQTT_RXMSG);	/* 清除接收标志位 */
			
			/* 一些消息数据包携带的参数 */
			u8 dup;           
			int qos;
			u8 retained;
			u16 msgID;             /* 消息ID */
			int payloadLen = 0;    /* 数据负载长度(内部调用,最终会赋值给) */
			u8* payload;
			int rc;
			MQTTString receivedTopic;	/* 接收消息的主题 */

			
			cTxPingCount = 0;	/* reset the time to send PING msg */
			rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgID, &receivedTopic,
					                         &payload, &payloadLen, MQTT_RX_BUF, MQTT_MAX_RECV_LEN);

			/* 以下的字符串末尾都没加 '\0', 故要使用 %.*s 打印 */
			printf("\r\n!!!Mqtt Rx a msg, topic: %.*s\r\n", receivedTopic.lenstring.len, receivedTopic.lenstring.data);// 不知道为什么显式不了

			
			if(payloadLen > 0)	/* payloadlen_in > 0 代表有效的字符串 */
			{
				memset(MqttRxPayload, 0, MQTT_MAX_RECV_LEN);     /* clear the global MQTTRxPayload */
				MqttRxPayloadLen = payloadLen;                   /* set the global MQTTRxPayloadLen */
				memcpy(MqttRxPayload, payload, payloadLen);      /* set the global MQTTRxPayload */
				xEventGroupSetBits(EventGroupHandler, EVENTBIT_LORA_TXMSG);	/* do LoRa tx */						
			}
			printf("可以开始接收新的消息了\r\n");
			
			USART2_RX_STA = 0;//串口2接收缓冲清空
			memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
			read_buf_len = 0;//这里必须加
			
		}
		
		/* Mqtt Tx */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_MQTT_TXMSG) == EVENTBIT_MQTT_TXMSG) {
			xEventGroupClearBits(EventGroupHandler, EVENTBIT_MQTT_TXMSG);
			
			if (MqttState != MQTT_NOCONN) {	/* MQTT 连接才能发送 */
				/* 发布消息 */
				MQTT_PublishMsg(MQTT_PUB_TOPIC, MqttTxPayload, MqttTxPayloadLen, 200);
				
				/* 这里其实是不合理的，理想情况应该是发送前将 EVENTBIT_OLED_UPLOAD 置位
					 发送完成后清除，但是目前 MQTT_PublishMsg() 当QoS=0 时无需等待 PUBACK
					 所以无法判断是否发送完成，暂且这样简单实现 */
				xEventGroupSetBits(EventGroupHandler, EVENTBIT_OLED_UPLOAD);
			} else {
				printf("Mqtt tx error: MQTT_NOCONN\r\n");
			}

		}
	  osDelay(500);

  }

  /* USER CODE END StartMqttTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void const * argument)
{
  /* USER CODE BEGIN StartDisplayTask */
	u8 k = 0;	                 /* temp variable */
	EventBits_t nowEvent = 0;	 /* 当前事件标志 */
	u8 gatewayID[10] = {0};	   /* OLED 显示的网关ID字段 */
	u8 runTime_Sec  = 0;       /* run time —— second bit */
	u8 runTime_Min  = 0;       /* run time —— minute bit */
	u8 runTime_Hour = 0;       /* run time —— hour bit */
	
	sprintf((u8 *)gatewayID, "GwID: %d", GATEWAY_ID);	
	
	/* bar area */
	OLED_ShowString(0,0,gatewayID, strlen(gatewayID), FONT_SIZE_16, DISPLAY_NORMAL);
	OLED_DrawBMP(80,0,96,2,BMP_WIFI_NOCONN_16);
	OLED_DrawBMP(112,0,128,2,BMP_MQTT_NOCONN_16);
	
	OLED_Draw_Line(0,20,127,20); /* split line */
	
	/* run-time area */
	OLED_ShowString(0,4,"RunTime:", 8, FONT_SIZE_16, DISPLAY_NORMAL);
	OLED_ShowChar(80, 4, ':', FONT_SIZE_16, DISPLAY_REVERSE);
	OLED_ShowChar(96, 4, ':', FONT_SIZE_16, DISPLAY_REVERSE);
	
	/* state area */
	OLED_ShowString(0,6,"State:", 6, FONT_SIZE_16, DISPLAY_NORMAL);
//	OLED_DrawBMP(64,6,80,8,BMP_UPLOAD_16);
//	OLED_DrawBMP(96,6,112,8,BMP_DOWNLOAD_16);

  /* Infinite loop */
	
  for(;;)
  {
	  LED1 = ! LED1;

		
		if (k++ == 2) {	/* about 1 second */
			k = 0;
			if (runTime_Sec++ == 60) { /* second bit overflow */
				runTime_Sec = 0;
				if (runTime_Min++ == 60) {	/* minute bit overflow */
					runTime_Min = 0;
					if (runTime_Hour++ == 100) {	/* hour bit overflow */
						runTime_Hour = 0;
					}
				}
			}
		}
		
		/* run-time area update */
		OLED_ShowNum(64,4,runTime_Hour,2,FONT_SIZE_16,DISPLAY_REVERSE, DISPLAY_FILL_ZERO);
		OLED_ShowNum(88,4,runTime_Min,2,FONT_SIZE_16,DISPLAY_REVERSE, DISPLAY_FILL_ZERO);
		OLED_ShowNum(112,4,runTime_Sec,2,FONT_SIZE_16,DISPLAY_REVERSE, DISPLAY_FILL_ZERO);
		
		//nowEvent = xEventGroupGetBits(EventGroupHandler);
		
		/* WiFi连接成功 */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_WIFI_CONNECTED) == EVENTBIT_WIFI_CONNECTED) {
//			xEventGroupClearBits(EventGroupHandler, EVENTBIT_WIFI_CONNECTED);
			OLED_DrawBMP(80,0,96,2,BMP_WIFI_16);
		}
		/* Mqtt连接成功 */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_MQTT_CONNECTED) == EVENTBIT_MQTT_CONNECTED) {
//			xEventGroupClearBits(EventGroupHandler, EVENTBIT_MQTT_CONNECTED);
			OLED_DrawBMP(112,0,128,2,BMP_MQTT_16);
		}


		/* Idle */
		if ((xEventGroupGetBits(EventGroupHandler) & (EVENTBIT_OLED_DOWNLOAD|EVENTBIT_OLED_UPLOAD)) == 0) {
			OLED_ShowString(64,6,"Ready!", 6, FONT_SIZE_16, DISPLAY_NORMAL);
		} 
		else {
			OLED_ClearArea(64,6,112,8);
			/* LoRa Txing */
			if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_OLED_DOWNLOAD) == EVENTBIT_OLED_DOWNLOAD) {
				xEventGroupClearBits(EventGroupHandler, EVENTBIT_OLED_DOWNLOAD);
				OLED_DrawBMP(96,6,112,8,BMP_DOWNLOAD_16);
			} 
			/* Mqtt Txing */
			if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_OLED_UPLOAD) == EVENTBIT_OLED_UPLOAD) {
				xEventGroupClearBits(EventGroupHandler, EVENTBIT_OLED_UPLOAD);
				printf("MQTT Txing... \r\n");
				OLED_DrawBMP(64,6,80,8,BMP_UPLOAD_16);
			}
			
		}
		
		osDelay(500);
  }
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartLoRaTask */
/**
* @brief Function implementing the LoRaTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLoRaTask */
void StartLoRaTask(void const * argument)
{
  /* USER CODE BEGIN StartLoRaTask */
	
	static RadioEvents_t RadioEvents; /*  Radio events function pointer */
	u8 cJsonRstCode = 0;
	u8 i, j;
	const TickType_t xTicksToWait = 20000 / portTICK_PERIOD_MS; /* 20000 ms */
	
	/* register LoRa event callback*/
	RadioEvents.TxDone    = OnLoRaTxDone;
  RadioEvents.RxDone    = OnLoRaRxDone;
  RadioEvents.TxTimeout = OnLoRaTxTimeout;
  RadioEvents.RxTimeout = OnLoRaRxTimeout;
  RadioEvents.RxError   = OnLoRaRxError;
	RadioEvents.CadDone   = OnLoRaCadDone;
	
	/* Radio 是 sx1276-board.c 中定义的全局变量 */
  Radio.Init( &RadioEvents );
	Radio.SetChannel( RF_FREQUENCY );
	
#if defined( USE_MODEM_LORA )

	/*
	 * LoRa模式           
	 * 发射功率        : 20 dBm
	 * 频率偏差        : 0 (LoRa无)
	 * 带宽            : 0 (125 kHz)
	 * 扩频因子        : 7
	 * 纠错编码率      : 1 (4/5)
	 * 前导码长度      : 8
	 * 报头模式        : 可变报头(显式报头)
	 * CRC校验         : 关闭
	 * 跳频            : 关闭
	 * 跳频周期        : 0
	 *
	 * 发送超时时间    : 3000
	 */
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_OFF,
                                   LORA_CRC_OFF, LORA_FREQ_HOP_OFF, LORA_HOP_PERIOD, LORA_IQ_INVERSION_OFF, 3000 );
	/*
	 * LoRa模式
	 * 带宽            : 0 (125 kHz)
	 * 扩频因子        : 7
	 * 纠错编码率      : 1 (4/5)
	 * AFC 带宽        : 0(LoRa无)
	 * 前导码长度      : 8
	 * 单次接收超时时间: 5
	 * 报头模式        : 可变报头(显式报头)
	 * 负载长度        : 0 (使用可变报头)
	 * CRC校验         : 关闭
	 * 跳频            : 关闭
	 * 跳频周期        : 0
	 *
	 * 接收模式        : 持续接收
	 */ 
  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_OFF,
                                   LORA_PAYLOAD_LENGTH, LORA_CRC_OFF, LORA_FREQ_HOP_OFF, LORA_HOP_PERIOD, LORA_IQ_INVERSION_OFF, LORA_RX_CONTINUOUS );
#endif	
	
	//xEventGroupWaitBits(EventGroupHandler, EVENTBIT_MQTT_CONNECTED, pdFALSE, pdFALSE, xTicksToWait);

	Radio.Rx( RX_TIMEOUT_VALUE );
  /* Infinite loop */
  for(;;)
  {
		//printf("RegModemStat = 0x%2x\r\n",SX1276Read( 0x18 ));	/* debug */
		
		/* LoRa需要发送消息 */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_LORA_TXMSG) == EVENTBIT_LORA_TXMSG) {
			xEventGroupClearBits(EventGroupHandler, EVENTBIT_LORA_TXMSG);

			/* 解析收到的json字符串 */
			cJsonRstCode = DecomposeJsonStr(MqttRxPayload, MqttRxPayloadLen, LoRaTxPayload, &LoRaTxPayloadLen);
			if (cJsonRstCode == JSON_OK) {
				
				/* 貌似是不及时清除标志位，也就是重复的SetBits，会导致MqttRxPayload数组被莫名修改
				   暂时不清除其中的原因 */
				xEventGroupSetBits(EventGroupHandler, EVENTBIT_OLED_DOWNLOAD); /* clear in OnLoRaTxDone() */
			  printf("sending message to LoRa node...\r\n");

			}

		}
		
		/* LoRa收到消息 */
		if ((xEventGroupGetBits(EventGroupHandler) & EVENTBIT_LORA_RXMSG) == EVENTBIT_LORA_RXMSG) {
			xEventGroupClearBits(EventGroupHandler, EVENTBIT_LORA_RXMSG);
			
			ComposeJsonStr(MqttTxPayload, &MqttTxPayloadLen, LoRaRxPayload, LoRaRxPayloadLen);
			
			/* for debug */
			printf("组合后的json数据: ");
			for (i = 0; i < MqttTxPayloadLen; i++) {
				printf("%c",MqttTxPayload[i]);
			}
			printf("\r\n");
			
			/* Mqtt 发送 */
			xEventGroupSetBits(EventGroupHandler, EVENTBIT_MQTT_TXMSG);	
			
			Radio.Rx( RX_TIMEOUT_VALUE );
		}
		
		
    osDelay(500);
  }

  /* USER CODE END StartLoRaTask */
}

/* RxTimeoutTimerCallback function */
void RxTimeoutTimerCallback(void const * argument)
{
  /* USER CODE BEGIN RxTimeoutTimerCallback */
	
	/* 发生中断后需要调用 SX1276OnTimeoutIrq 函数 */
	SX1276OnTimeoutIrq();	
	printf("\r\ntimer irq\r\n");
	
  /* USER CODE END RxTimeoutTimerCallback */
}

/* TxTimeoutTimerCallback function */
void TxTimeoutTimerCallback(void const * argument)
{
  /* USER CODE BEGIN TxTimeoutTimerCallback */
	
	/* 发生中断后需要调用 SX1276OnTimeoutIrq 函数 */
	SX1276OnTimeoutIrq();	
	printf("\r\ntimer irq\r\n");
	  
  /* USER CODE END TxTimeoutTimerCallback */
}

/* SyncWordTimeoutTimerCallback function */
void SyncWordTimeoutTimerCallback(void const * argument)
{
  /* USER CODE BEGIN SyncWordTimeoutTimerCallback */
	
	/* 发生中断后需要调用 SX1276OnTimeoutIrq 函数 */
	SX1276OnTimeoutIrq();	
	printf("\r\ntimer irq\r\n");
	  
  /* USER CODE END SyncWordTimeoutTimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void OnLoRaRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	Radio.Sleep( );
	int i;
	memset(LoRaRxPayload,0,LORA_BUFFER_SIZE);
	
	LoRaRxPayloadLen = size;
	memcpy( LoRaRxPayload, payload, size );

	printf("\r\n !!!LoRa Rx Done\r\n");
	printf("rssi:%d\r\n snr:%d\r\n size:%d\r\n", rssi, snr, size);
	
	
	for (i = 0; i < size; i++) {	/* for debug */
		if (i == 0) {
			printf("payload: ");
		}	
		printf("0x%02x ",*(payload+i));
		if (i == size -1) {
			printf("\r\n\r\n");
		}
	}
	
	/* 遵循中断服务函数中不进行复杂处理的原则，设置事件标志位
     在 LoRaTxRxTask 任务中对数据进行解析	*/
	xEventGroupSetBitsFromISR(EventGroupHandler, EVENTBIT_LORA_RXMSG, pdFALSE);	/* FromISR? */
	
}
void OnLoRaTxDone( void )
{
	Radio.Sleep( );
	xEventGroupClearBitsFromISR(EventGroupHandler, EVENTBIT_OLED_DOWNLOAD);
	printf("TxDone\r\n");
}
void OnLoRaTxTimeout( void )
{
	Radio.Sleep( );
	printf("TxTIMEOUT\r\n");
}

void OnLoRaRxTimeout( void )
{
	Radio.Sleep( );
	printf("RxTIMEOUT retry recive\r\n");
	
	/* 这里选择接收超时后再次重启接收 */
	Radio.Rx( RX_TIMEOUT_VALUE );	
}

void OnLoRaRxError( void )
{
	Radio.Sleep( );
	printf("RxError  retry recive\r\n");
//	Radio.Rx( RX_TIMEOUT_VALUE );
}
void OnLoRaCadDone( bool channelActivityDetected )
{
//	Radio.Sleep( );
	printf("CadDone\r\n");
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
