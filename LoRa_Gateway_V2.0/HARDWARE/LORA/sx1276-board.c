/**********************************************
 * BSP of AiThinker LoRa module(ra-01)
 * Test hardware: STM32F103RCT6; 72M
 * 
 * Create by Wanglu in QingDao University 
 * Used for the graduation project		
 *  
 * Last update time: 2021/07/02
 *********************************************/		
 
#include "radio.h"
#include "sx1276/sx1276.h"
#include "sx1276-board.h"

//#include "delay.h" /* 使用HAL_Delay() 代替*/
#include "stdio.h"
#include "spi.h"


u16 LoRaRxPayloadLen = LORA_BUFFER_SIZE;	 /* Length of LoRa Rx payload */
u16 LoRaTxPayloadLen = LORA_BUFFER_SIZE;	 /* Length of LoRa Tx payload */

u8  LoRaRxPayload[LORA_BUFFER_SIZE];       /* LoRa Rx payload */
u8  LoRaTxPayload[LORA_BUFFER_SIZE];       /* LoRa Tx payload */

/* freeRTOS 提供的软件定时器 */
extern osTimerId RxTimeoutTimerHandle;
extern osTimerId TxTimeoutTimerHandle;
extern osTimerId SyncWordTimeoutTimerHandle;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby, 
    SX1276SetRx,
    SX1276StartCad,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetMaxPayloadLength
};

/*!
 * Antenna switch GPIO pins objects
 */
/* 我们的RA-01/02模组的天线切换开关由硬件自动控制，不需要软件控制
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;*/



//实现ms级延时
//delayMs：延时的ms数
void SX1276DelayMs(uint32_t delayMs){
	HAL_Delay(delayMs);
}

/* 定时器初始化(RX,TX,SyncWord定时完成后都需要调用 SX1276OnTimeoutIrq() 函数) */
void SX1276TimerInit(void){
	/* 本项目使用FreeRTOS配置的软件定时器
	   故在 freertos.c 中初始化 */
}

/* 开启 tx 超时定时器，定时timeoutMs 毫秒 */
void SX1276TxTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(TxTimeoutTimerHandle, timeoutMs);
}

/* 关闭 tx 超时定时器 */
void SX1276TxTimeoutTimerStop(void){
	osTimerStop(TxTimeoutTimerHandle);
}

/* 开启 RX 超时定时器，定时timeoutMs 毫秒 */
void SX1276RxTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(RxTimeoutTimerHandle, timeoutMs);
}

/* 关闭 RX 超时定时器 */
void SX1276RxTimeoutTimerStop(void){
	osTimerStop(RxTimeoutTimerHandle);
}

/* 开启 SyncWord 超时定时器，定时timeoutMs 毫秒 */
void SX1276SyncWordTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(SyncWordTimeoutTimerHandle, timeoutMs);
}

/* 关闭 SyncWord 超时定时器 */
void SX1276SyncWordTimeoutTimerStop(void){
	osTimerStop(SyncWordTimeoutTimerHandle);
}


/* 初始化SPI(cs低使能，极性SPI_CPOL_High，相位SPI_CPHA_2Edge，SPI_FirstBit_MSB)和Rst脚 */
void SX1276IoInit( void )
{
	/* CS引脚的初始化在 MX_GPIO_Init() 中完成 */
	/* SPI的初始化在 MX_SPI1_Init() 中完成 */
}

DioIrqHandler **g_irqHandlers;	/* 中断服务函数数组 */

/* 初始化DIO(上升沿)中断，将 DIO0~5 的GPIO中断函数调用 irqHandlers[0]~irqHandlers[5] */
void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
	/* Irq引脚的初始化在 MX_GPIO_Init() 中已实现 */
	/* 目前代码仅使用DIO0~DIO1 硬件支持DIO0~DIO3 */
	g_irqHandlers=irqHandlers;
}


//将SPI、DIO0~5和Rst脚的中断去初始化
void SX1276IoDeInit( void )
{
	/* 这里省略去初始化的功能了 */
}

/*判断频率是否合法
  如果不限制频率可以直接return true */
bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

/**
  * 硬复位
  * 将rst引脚拉低1ms，然后将复位脚设置为无上下拉的输入模式
  */
void SX1276Reset( void )
{
	/* rst引脚的初始化在 MX_GPIO_Init() 中完成 */

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_RESET);  
	HAL_Delay(1);

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_SET); 
	HAL_Delay(6);
}

/**
  * 设置SPI的片选脚的电平(低电平使能，高电平取消)
  * lev:true的时候高电平,false的时候低电平
  */
void Sx1276SetNSS(bool lev )
{
	if (lev) {
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_SET);	//输出高电平
	} else {
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_RESET);	//输出低电平
	}
}

/**
  * SPI读写函数,data是发送的数据，返回值是接收到的数据
  */
uint8_t Sx1276SpiInOut(uint16_t data )
{
	return SPI1_ReadWriteByte(data);
}
