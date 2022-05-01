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

//#include "delay.h" /* ʹ��HAL_Delay() ����*/
#include "stdio.h"
#include "spi.h"


u16 LoRaRxPayloadLen = LORA_BUFFER_SIZE;	 /* Length of LoRa Rx payload */
u16 LoRaTxPayloadLen = LORA_BUFFER_SIZE;	 /* Length of LoRa Tx payload */

u8  LoRaRxPayload[LORA_BUFFER_SIZE];       /* LoRa Rx payload */
u8  LoRaTxPayload[LORA_BUFFER_SIZE];       /* LoRa Tx payload */

/* freeRTOS �ṩ�������ʱ�� */
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
/* ���ǵ�RA-01/02ģ��������л�������Ӳ���Զ����ƣ�����Ҫ�������
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;*/



//ʵ��ms����ʱ
//delayMs����ʱ��ms��
void SX1276DelayMs(uint32_t delayMs){
	HAL_Delay(delayMs);
}

/* ��ʱ����ʼ��(RX,TX,SyncWord��ʱ��ɺ���Ҫ���� SX1276OnTimeoutIrq() ����) */
void SX1276TimerInit(void){
	/* ����Ŀʹ��FreeRTOS���õ������ʱ��
	   ���� freertos.c �г�ʼ�� */
}

/* ���� tx ��ʱ��ʱ������ʱtimeoutMs ���� */
void SX1276TxTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(TxTimeoutTimerHandle, timeoutMs);
}

/* �ر� tx ��ʱ��ʱ�� */
void SX1276TxTimeoutTimerStop(void){
	osTimerStop(TxTimeoutTimerHandle);
}

/* ���� RX ��ʱ��ʱ������ʱtimeoutMs ���� */
void SX1276RxTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(RxTimeoutTimerHandle, timeoutMs);
}

/* �ر� RX ��ʱ��ʱ�� */
void SX1276RxTimeoutTimerStop(void){
	osTimerStop(RxTimeoutTimerHandle);
}

/* ���� SyncWord ��ʱ��ʱ������ʱtimeoutMs ���� */
void SX1276SyncWordTimeoutTimerStart( uint32_t timeoutMs ){
	osTimerStart(SyncWordTimeoutTimerHandle, timeoutMs);
}

/* �ر� SyncWord ��ʱ��ʱ�� */
void SX1276SyncWordTimeoutTimerStop(void){
	osTimerStop(SyncWordTimeoutTimerHandle);
}


/* ��ʼ��SPI(cs��ʹ�ܣ�����SPI_CPOL_High����λSPI_CPHA_2Edge��SPI_FirstBit_MSB)��Rst�� */
void SX1276IoInit( void )
{
	/* CS���ŵĳ�ʼ���� MX_GPIO_Init() ����� */
	/* SPI�ĳ�ʼ���� MX_SPI1_Init() ����� */
}

DioIrqHandler **g_irqHandlers;	/* �жϷ��������� */

/* ��ʼ��DIO(������)�жϣ��� DIO0~5 ��GPIO�жϺ������� irqHandlers[0]~irqHandlers[5] */
void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
	/* Irq���ŵĳ�ʼ���� MX_GPIO_Init() ����ʵ�� */
	/* Ŀǰ�����ʹ��DIO0~DIO1 Ӳ��֧��DIO0~DIO3 */
	g_irqHandlers=irqHandlers;
}


//��SPI��DIO0~5��Rst�ŵ��ж�ȥ��ʼ��
void SX1276IoDeInit( void )
{
	/* ����ʡ��ȥ��ʼ���Ĺ����� */
}

/*�ж�Ƶ���Ƿ�Ϸ�
  ���������Ƶ�ʿ���ֱ��return true */
bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

/**
  * Ӳ��λ
  * ��rst��������1ms��Ȼ�󽫸�λ������Ϊ��������������ģʽ
  */
void SX1276Reset( void )
{
	/* rst���ŵĳ�ʼ���� MX_GPIO_Init() ����� */

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_RESET);  
	HAL_Delay(1);

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_SET); 
	HAL_Delay(6);
}

/**
  * ����SPI��Ƭѡ�ŵĵ�ƽ(�͵�ƽʹ�ܣ��ߵ�ƽȡ��)
  * lev:true��ʱ��ߵ�ƽ,false��ʱ��͵�ƽ
  */
void Sx1276SetNSS(bool lev )
{
	if (lev) {
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_SET);	//����ߵ�ƽ
	} else {
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_RESET);	//����͵�ƽ
	}
}

/**
  * SPI��д����,data�Ƿ��͵����ݣ�����ֵ�ǽ��յ�������
  */
uint8_t Sx1276SpiInOut(uint16_t data )
{
	return SPI1_ReadWriteByte(data);
}
