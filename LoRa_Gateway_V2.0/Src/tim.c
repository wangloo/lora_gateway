/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

#include "led.h"
#include "key.h"
#include "usart.h"
#include "transport.h"
#include "MQTTPacket.h"
#include "event_groups.h"

extern I2C_HandleTypeDef hi2c1;	
extern EventGroupHandle_t EventGroupHandler; /* 事件标志组句柄, definition in freertos.c */

HAL_I2C_StateTypeDef I2cCurState;	/* IIC 的状态，用于刷新OLED */

u8 closeCount = 0;	/* test MQTT_Disconnect(); */


/* USER CODE END 0 */

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

/* TIM3 init function */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 4999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}
/* TIM6 init function */
void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 3599;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}
/* TIM7 init function */
void MX_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 8999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* TIM6 clock enable */
    __HAL_RCC_TIM6_CLK_ENABLE();

    /* TIM6 interrupt Init */
    HAL_NVIC_SetPriority(TIM6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspInit 0 */

  /* USER CODE END TIM7_MspInit 0 */
    /* TIM7 clock enable */
    __HAL_RCC_TIM7_CLK_ENABLE();

    /* TIM7 interrupt Init */
    HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  /* USER CODE BEGIN TIM7_MspInit 1 */

  /* USER CODE END TIM7_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspDeInit 0 */

  /* USER CODE END TIM6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM6_CLK_DISABLE();

    /* TIM6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM6_IRQn);
  /* USER CODE BEGIN TIM6_MspDeInit 1 */

  /* USER CODE END TIM6_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspDeInit 0 */

  /* USER CODE END TIM7_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM7_CLK_DISABLE();

    /* TIM7 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  /* USER CODE BEGIN TIM7_MspDeInit 1 */

  /* USER CODE END TIM7_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&htim3))	/* 7200*5000/72M=500ms 产生一次中断 */
	{
	  __HAL_TIM_CLEAR_FLAG(&htim3,TIM_EVENTSOURCE_UPDATE );       //清除TIM3更新中断标志  
		
		if (cTxPingCount++ == 40) {	/* 20s 发送一次心跳 */
		
			cTxPingCount = 0;
			/* 使用 xEventGroupSetBitsFromISR() 需要在 CubeMX 中 勾选:
			   xTimerPendFunctionCallFromISR() and xEventGroupSetBitsFromISR() */
			xEventGroupSetBitsFromISR(EventGroupHandler, EVENTBIT_MQTT_TXPING, pdFALSE);
		}

	}
	else if(htim==(&htim7)) /* 125ms标记串口一次接收完成 */
	{
		USART2_RX_STA |= 1<<15;	//标记接收完成
		read_buf_len=0;//transport_getdata()中用到，需要在特定的位置清零，他人历程是在定时器里
		__HAL_TIM_CLEAR_FLAG(&htim7,TIM_EVENTSOURCE_UPDATE );       //清除TIM7更新中断标志  
		TIM7->CR1&=~(1<<0);     			//关闭定时器7    
		
		if (MqttState == MQTT_LISTEN) {
			int qc;
			memset(MQTT_RX_BUF, 0, MQTT_MAX_RECV_LEN);
			qc = MQTTPacket_read(MQTT_RX_BUF, MQTT_MAX_RECV_LEN, transport_getdata);
				if (qc == PUBLISH) {
				xEventGroupSetBitsFromISR(EventGroupHandler, EVENTBIT_MQTT_RXMSG, pdFALSE);

				} else if (qc == PINGRESP) {
					printf("收到 PINGRESP message\r\n");
					USART2_RX_STA = 0;//串口2接收缓冲清空
					memset(USART2_RX_BUF,0,USART2_MAX_RECV_LEN);
					read_buf_len = 0;//这里必须加
				}

	 }
		
	} 
	else if (htim == (&htim6)) { /* 50ms 产生一次中断 */
		
		if (KEY0 == 0) { /* KEY0 消抖，输入捕获中断服务函数中开启TIM6中断 */
			/* 发布测试消息 */
			//MQTT_PublishMsg(MQTT_PUB_TOPIC, MQTT_MSG_UPLOAD, strlen(MQTT_MSG_UPLOAD), 200);
			
			MqttTxPayloadLen = strlen(MQTT_MSG_UPLOAD);
			memcpy(MqttTxPayload, MQTT_MSG_UPLOAD, MqttTxPayloadLen);
//			xEventGroupSetBitsFromISR(EventGroupHandler, EVENTBIT_MQTT_TXMSG|EVENTBIT_OLED_UPLOAD, pdFALSE);
			xEventGroupSetBitsFromISR(EventGroupHandler, EVENTBIT_MQTT_TXMSG, pdFALSE);
			LED1 =! LED1;
		}

		TIM_Set(htim, 0);    			//关闭定时器6  
		__HAL_TIM_CLEAR_FLAG(&htim6,TIM_EVENTSOURCE_UPDATE );       //清除TIM6更新中断标志  
		
		
	}
}

//设置TIM的开关
//sta:0，关闭;1,开启;
void TIM_Set(TIM_HandleTypeDef *htim, uint8_t sta)
{
	if(sta)
	{
		__HAL_TIM_SET_COUNTER(htim, 0);//计数器清空
		HAL_TIM_Base_Start_IT(htim); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE  //使能TIMx	
	}else HAL_TIM_Base_Stop_IT(htim);//关闭定时器3	   
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
