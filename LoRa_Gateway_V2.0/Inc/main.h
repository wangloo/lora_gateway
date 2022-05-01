/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LoRa_DIO0_Pin GPIO_PIN_0
#define LoRa_DIO0_GPIO_Port GPIOC
#define LoRa_DIO0_EXTI_IRQn EXTI0_IRQn
#define LoRa_DIO1_Pin GPIO_PIN_1
#define LoRa_DIO1_GPIO_Port GPIOC
#define LoRa_DIO1_EXTI_IRQn EXTI1_IRQn
#define LoRa_DIO2_Pin GPIO_PIN_2
#define LoRa_DIO2_GPIO_Port GPIOC
#define LoRa_DIO2_EXTI_IRQn EXTI2_IRQn
#define LoRa_DIO3_Pin GPIO_PIN_3
#define LoRa_DIO3_GPIO_Port GPIOC
#define LoRa_DIO3_EXTI_IRQn EXTI3_IRQn
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define LoRa_RST_Pin GPIO_PIN_4
#define LoRa_RST_GPIO_Port GPIOC
#define KEY_0_Pin GPIO_PIN_5
#define KEY_0_GPIO_Port GPIOC
#define KEY_0_EXTI_IRQn EXTI9_5_IRQn
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* gateway ID */
#define GATEWAY_ID                              0x01


/* 事件标志位 */                                              
#define EVENTBIT_WIFI_CONNECTED                 (1 << 0)          /* WiFi连接成功 */        
#define EVENTBIT_MQTT_CONNECTED                 (1 << 1)          /* MQTT连接成功 */
#define EVENTBIT_MQTT_RXMSG                     (1 << 2)          /* MQTT收到消息 */ 
#define EVENTBIT_MQTT_TXMSG                     (1 << 3)          /* MQTT需要发送消息 */
#define EVENTBIT_MQTT_TXPING                    (1 << 4)          /* MQTT需要发送心跳 */ 
#define EVENTBIT_LORA_RXMSG                     (1 << 5)          /* LoRa收到消息 */
#define EVENTBIT_LORA_TXMSG                     (1 << 6)          /* LoRa需要发送消息 */
#define EVENTBIT_OLED_UPLOAD                    (1 << 7)          /* OLED 显示上行传输 */
#define EVENTBIT_OLED_DOWNLOAD                  (1 << 8)          /* OLED 显示下行传输 */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
