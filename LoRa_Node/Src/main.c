/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "radio.h"
#include "sx1276Regs-Fsk.h"
#include "sx1276Regs-LoRa.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define GATEWAY_ID                                0x01
#define NODE_ID                                   0x00
#define FLAG                                      0x01
#define TEMPERATURE	                              0x14
#define HUMIDITY                                  0x50
#define BATTERY                                   0x63


#define RX_TIMEOUT_VALUE                            1000
#define LORA_BUFFER_SIZE                            64 // Define the payload size here


#define USE_BAND_433	//选择一个频率
#define USE_MODEM_LORA	//选择lora模式

#if defined( USE_BAND_433 )

#define RF_FREQUENCY                                434000000 // Hz

#elif defined( USE_BAND_780 )

#define RF_FREQUENCY                                780000000 // Hz

#elif defined( USE_BAND_868 )

#define RF_FREQUENCY                                868000000 // Hz

#elif defined( USE_BAND_915 )

#define RF_FREQUENCY                                915000000 // Hz

#else
    #error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             20        // dBm

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         /* 带宽       [0: 125 kHz,
                                                               *             1: 250 kHz,
                                                               *             2: 500 kHz,
                                                               *             3: Reserved]  */
																																 			    
#define LORA_SPREADING_FACTOR                       7         /* 扩频因子   [SF7..SF12]    */
																																 			    
#define LORA_CODINGRATE                             1         /* 编码率     [1: 4/5, 
                                                               *             2: 4/6,
                                                               *             3: 4/7,
                                                               *             4: 4/8]       */
																															 
#define LORA_PREAMBLE_LENGTH                        8         /* 前导码长度 Tx 和 Rx 相同  */
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define LORA_CRC_ON                                 true
#define LORA_CRC_OFF                                false

#elif defined( USE_MODEM_FSK )

#define FSK_FDEV                                    25e3      // Hz
#define FSK_DATARATE                                50e3      // bps
#define FSK_BANDWIDTH                               50e3      // Hz
#define FSK_AFC_BANDWIDTH                           83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#else
    #error "Please define a modem in the compiler options."
#endif

#define MSG                            "{\"GateWayID\":1,\"NodeID\":0,\"Flag\":1,\"Temp\":20,\"Humi\":70,\"Battery\":100}"
#define MSG_LONG                       "1234567890abcdefghijk"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static RadioEvents_t RadioEvents;

u16 LoRaRxBufferSize = LORA_BUFFER_SIZE;
u8  LoRaRxBuffer[LORA_BUFFER_SIZE];

u8  LoRaTxBufferSize = LORA_BUFFER_SIZE;
u8  LoRaTxBuffer[LORA_BUFFER_SIZE];


s8 LoRa_Rx_RssiValue = 0; /* 接收的信号强度 */
s8 LoRa_Rx_SnrValue = 0;  /* 信噪比 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void OnTxDone( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	u16 regPaConfigInitVal = 0;
	
	
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	/* Radio initialization */
	/* 注册相关的回调函数 */
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;

	/* sx1276-board.c 中定义的全局变量 */
  Radio.Init( &RadioEvents );
	Radio.SetChannel( RF_FREQUENCY );
	
#if defined( USE_MODEM_LORA )

	/*
	 * LoRa模式
	 * 发射功率 20 dBm
	 * (LoRa无)
	 * 带宽
	 * 扩频因子
	 * 纠错编码率
	 * 前导码长度 8
	 * 显式报头
	 * CRC开启
	 * 跳频 关闭
	 * 跳频周期 0
	 * 
	 *
	 * 发送超时时间 3000
	 */
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
	/*
	 * LoRa模式
	 * 带宽
	 * 扩频因子 7
	 * 纠错编码率
	 * (LoRa无)
	 * 前导码长度 8
	 * 接收超时时间 5
	 * 显式报头
	 * 负载长度 ***
	 * CRC关闭
	 * 跳频 关闭
	 * 跳频周期 0
	 *
	 * 持续/单一接收模式
	 */ 
  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, LORA_CRC_OFF, 0, 0, LORA_IQ_INVERSION_ON, true );
#endif	


	regPaConfigInitVal = SX1276Read( 0x42 );
	
	memset(LoRaTxBuffer,0,LoRaTxBufferSize);

	LoRaTxBuffer[0] = GATEWAY_ID;
	LoRaTxBuffer[1] = NODE_ID;
	LoRaTxBuffer[2] = FLAG;
	LoRaTxBuffer[3] = TEMPERATURE;
	LoRaTxBuffer[4] = HUMIDITY;
	LoRaTxBuffer[5] = BATTERY;	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	u8 TxFlag = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if (regPaConfigInitVal == 0x12) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}
		
		if(TxFlag >= 6) {
			TxFlag = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
			Radio.Send( (uint8_t *)LoRaTxBuffer, 6 );
		}
		TxFlag++;
		HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void OnTxDone( void )
{
	Radio.Sleep( );
	printf("TxDone\r\n");
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	static bool ledStatus=false;
	Radio.Sleep( );
	memset(LoRaRxBuffer,0,LoRaRxBufferSize);
	LoRaRxBufferSize = size;
	memcpy( LoRaRxBuffer, payload, LoRaRxBufferSize );
	LoRa_Rx_RssiValue = rssi;                    /* 接收的信号强度 */
	LoRa_Rx_SnrValue = snr;                      /* 信噪比 */
	printf("\r\n RxDone\r\n rssi:%d\r\n snr:%d\r\n size:%d\r\n payload:%s\r\n",rssi,snr,size,payload);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	
	if ( strncmp( ( const char* )LoRaRxBuffer, ( const char* )"led", 3 ) == 0 ) {
//		LED1 =! LED1;
	}
	
	Radio.Rx( RX_TIMEOUT_VALUE );
}

void OnTxTimeout( void )
{
	Radio.Sleep( );
	printf("TxTIMEOUT\r\n");
}

void OnRxTimeout( void )
{
	Radio.Sleep( );
	printf("RxTIMEOUT retry recive\r\n");
//	Radio.Rx( RX_TIMEOUT_VALUE );
}

void OnRxError( void )
{
	Radio.Sleep( );
	printf("RxError  retry recive\r\n");
//	Radio.Rx( RX_TIMEOUT_VALUE );
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
