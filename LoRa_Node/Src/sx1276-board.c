/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX1276 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "radio.h"
#include "sx1276/sx1276.h"
#include "sx1276-board.h"
#include "stdio.h"
#include "spi.h"

//��λ��----PC4
#define SX1278_RST_PIN_INDEX	GPIO_PIN_11
#define SX1278_RST_PIN_GROUP	GPIOB
//SPIƬѡʹ�õ�PA4
#define SX1278_CS_PIN_INDEX	GPIO_PIN_12
#define SX1278_CS_PIN_GROUP	GPIOB

//DIO0����(�����޸�����Ҫ�ֶ��޸��жϳ�ʼ�����жϺ���)
#define SX1278_DIO0_PIN_INDEX	GPIO_PIN_10
#define SX1278_DIO0_PIN_GROUP	GPIOB
//DIO1����(�����޸�����Ҫ�ֶ��޸��жϳ�ʼ�����жϺ���)
#define SX1278_DIO1_PIN_INDEX	GPIO_PIN_1
#define SX1278_DIO1_PIN_GROUP	GPIOC

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

/*!
 * Tx and Rx timers
 */
/*TimerEvent_t TxTimeoutTimer;
TimerEvent_t RxTimeoutTimer;
TimerEvent_t RxTimeoutSyncWord;*/

//ʵ��ms����ʱ
//delayMs����ʱ��ms��
void SX1276DelayMs(uint32_t delayMs){
	HAL_Delay(delayMs);
}

//��ʱ����ʼ��(RX,TX,SyncWord��ʱ��ɺ���Ҫ���� SX1276OnTimeoutIrq() ����)
void SX1276TimerInit(void){
}

void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
//  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

//	TIM3->CR1 |= TIM_CR1_URS;	//�������������ᵼ�¶�ʱ��������ʱ�����������ж�
//	
//	//��ʱ��TIM3��ʼ��
//	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
// 
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
//	
//	//�ж����ȼ�NVIC����
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
//	
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

//	TIM_Cmd(TIM3, ENABLE);  //����TIM					 
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־
//		TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx
//		SX1276OnTimeoutIrq();	//�����жϺ���Ҫ���� SX1276OnTimeoutIrq ����
//		//printf("\r\ntimer irq\r\n");
//	}
}
//���� tx ��ʱ��ʱ������ʱtimeoutMs ����
void SX1276TxTimeoutTimerStart( uint32_t timeoutMs ){
	//�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
//	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz�ļ���Ƶ�ʣ�������10��Ϊ1ms 
}

//�ر� tx ��ʱ��ʱ��
void SX1276TxTimeoutTimerStop(void){
	///�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
//	TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx
}

//���� RX ��ʱ��ʱ������ʱtimeoutMs ����
void SX1276RxTimeoutTimerStart( uint32_t timeoutMs ){
	//�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz�ļ���Ƶ�ʣ�������10��Ϊ1ms 
}

//�ر� RX ��ʱ��ʱ��
void SX1276RxTimeoutTimerStop(void){
	///�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
//	TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx	
}

//���� SyncWord ��ʱ��ʱ������ʱtimeoutMs ����
void SX1276SyncWordTimeoutTimerStart( uint32_t timeoutMs ){
	//�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz�ļ���Ƶ�ʣ�������10��Ϊ1ms 
}

//�ر� SyncWord ��ʱ��ʱ��
void SX1276SyncWordTimeoutTimerStop(void){
	///�����ʹ����һ����ʱ����ʵ�ʹ��������ʹ�ö�����3����ʱ��������ʹ�������ʱ��
//	TIM_Cmd(TIM3, DISABLE);  //ʧ��TIMx	
}

//��ʼ��SPI(cs��ʹ�ܣ�����SPI_CPOL_High����λSPI_CPHA_2Edge��SPI_FirstBit_MSB)��Rst��
void SX1276IoInit( void )
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	SPI_InitTypeDef  SPI_InitStructure;
//	
	//��ʼ��һ��gpio��������spi1��cs
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Pin = SX1278_CS_PIN_INDEX;
//	HAL_GPIO_Init(SX1278_CS_PIN_GROUP, &GPIO_InitStruct);

	//SPI�ĳ�ʼ��
	//MX_SPI2_Init();
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPIʱ��ʹ��
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	//Ƭ������ģʽ
//	GPIO_InitStructure.GPIO_Pin = SX1278_SCK_PIN_INDEX|SX1278_MISO_PIN_INDEX|SX1278_MOSI_PIN_INDEX;
//	GPIO_Init(SX1278_SCK_PIN_GROUP, &GPIO_InitStructure);	//��ʼ��GPIO

//	//����SPI
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ʱ�Ӽ���(����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ���ǵ͵�ƽ)
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//������λ(����ͬ��ʱ�ӵĵڼ��������أ��������½������ݱ�����)
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:����Ϊ�������(SSI����)
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256(256�����,�����������֮������ٶ�,����ٶȹ��쵼��ͨ��ʧ���ٵ�С)
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ(CRCУ�����)
//	SPI_Init(SPI1,&SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
//	
//	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI����
}

DioIrqHandler **g_irqHandlers;

//��ʼ��DIO(������)�жϣ��� DIO0~5 ��GPIO�жϺ������� irqHandlers[0]~irqHandlers[5]
void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};

	g_irqHandlers=irqHandlers;
	
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//		
//  GPIO_InitStruct.Pin = SX1278_DIO0_PIN_INDEX;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(SX1278_DIO0_PIN_GROUP, &GPIO_InitStruct);
	
	/* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

//	HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//	
//  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

//  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
//	EXTI_InitTypeDef EXTI_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	g_irqHandlers=irqHandlers;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ��RCC_APB2Periph_AFIO��ʱ��
//	
//	//��ʼ��DIO0
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	    //ʹ��ָ���˿�ʱ�ӣ�������Ϊ�˷���ABCȫ���ˣ�
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz�����ﲻ�ô��Σ�ֱ��д��������ٶ�50MHZ
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//��������
//	GPIO_InitStructure.GPIO_Pin = SX1278_DIO0_PIN_INDEX;
//	GPIO_Init(SX1278_DIO0_PIN_GROUP, &GPIO_InitStructure);	//��ʼ��GPIO
//	
//	//�����ж�
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);//����ӳ���߹�ϵ(����� GPIO_PortSourceGPIOX �պ���0-5������ֱ����pinName/16��ʾ��GPIO_PinSourceXX�պ���0-15��������pinName%16��ʾ��)
//	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//�ж���u8_pinNum
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ⲿ�ж�
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�������ش���
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ���ж�
//	EXTI_Init(&EXTI_InitStructure);//�����ж�

//	//�����ж����ȼ�
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	//�ⲿ�ж�XX
//	//stm32f103c8������������ȼ���Ӧ��ռ���ȼ�2�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;	//��ռ���ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;   //�����ȼ�
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ж�
//	NVIC_Init(&NVIC_InitStructure);   //�������ȼ�
//	
//	EXTI_ClearITPendingBit(EXTI_Line0);//����жϱ�־λ
//	
//	//DIO1�ж�
//	GPIO_InitStructure.GPIO_Pin = SX1278_DIO1_PIN_INDEX;
//	GPIO_Init(SX1278_DIO1_PIN_GROUP, &GPIO_InitStructure);	//��ʼ��GPIO
//	
//	//�����ж�
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);//����ӳ���߹�ϵ(����� GPIO_PortSourceGPIOX �պ���0-5������ֱ����pinName/16��ʾ��GPIO_PinSourceXX�պ���0-15��������pinName%16��ʾ��)
//	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//�ж���u8_pinNum
//	EXTI_Init(&EXTI_InitStructure);//�����ж�

//	//�����ж����ȼ�
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;	//�ⲿ�ж�XX
//	NVIC_Init(&NVIC_InitStructure);   //�������ȼ�
//	
//	EXTI_ClearITPendingBit(EXTI_Line1);//����жϱ�־λ
	
	//DIO2~5ʡ����
}

////�ⲿ�ж���0�жϴ�������������Ҫ�ص���Ӧ�ĺ���
//void EXTI0_IRQHandler(void){
//	g_irqHandlers[0]();
////	EXTI_ClearITPendingBit(EXTI_Line0);//����жϱ�־λ
//}
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */
	g_irqHandlers[0]();
  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}
////�ⲿ�ж���1�жϴ�������������Ҫ�ص���Ӧ�ĺ���
//void EXTI1_IRQHandler(void){
//	g_irqHandlers[1]();
////	EXTI_ClearITPendingBit(EXTI_Line1);//����жϱ�־λ
//}

//��SPI��DIO0~5��Rst�ŵ��ж�ȥ��ʼ��
void SX1276IoDeInit( void )
{
	//����ʡ��ȥ��ʼ���Ĺ�����
}

//�ж�Ƶ���Ƿ�Ϸ�
//���������Ƶ�ʿ���ֱ��return true
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
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	

	
	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_RESET);  //����͵�ƽ
	HAL_Delay(1);
	
//	HAL_GPIO_DeInit(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX);
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Pin = SX1278_RST_PIN_INDEX;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	HAL_GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_SET);  //����͵�ƽ
	HAL_Delay(6);
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	    //ʹ��ָ���˿�ʱ�ӣ�������Ϊ�˷���ABCȫ���ˣ�
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO���ٶ�Ϊ50MHz�����ﲻ�ô��Σ�ֱ��д��������ٶ�50MHZ
//	GPIO_InitStructure.GPIO_Pin = SX1278_RST_PIN_INDEX;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	//����Ϊ�������
//	GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStructure);	//��ʼ��GPIO

//	GPIO_ResetBits(SX1278_RST_PIN_GROUP,SX1278_RST_PIN_INDEX);	//����͵�ƽ
//	delay_ms(1);
//	
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
//	GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStructure);	//��ʼ��GPIO
//	delay_ms(6);
}

/**
  * ����SPI��Ƭѡ�ŵĵ�ƽ(�͵�ƽʹ�ܣ��ߵ�ƽȡ��)
  * lev:true��ʱ��ߵ�ƽ,false��ʱ��͵�ƽ
  */
void Sx1276SetNSS(bool lev )
{
	if(lev){
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_SET);	//����ߵ�ƽ
	}else{
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_RESET);	//����͵�ƽ
	}
}

/**
  * SPI��д����,data�Ƿ��͵����ݣ�����ֵ�ǽ��յ�������
  */
uint8_t Sx1276SpiInOut(uint16_t data )
{
	return SPI1_ReadWriteByte(data);
//	//���SPI�Ķ�д����
//	uint16_t retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){//���ָ����SPI��־λ�������:���ͻ���ձ�־λ(���������˾Ϳ��Կ���������)
//		if((retry++)>2000){
//			//��ʱ��
//			return 0xff;
//		}
//	}
//	SPI_I2S_SendData(SPI1, data); //ͨ������SPIx����һ������
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){ //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ���ǿ��˾ͱ�ʾ������������ˣ�
//		if((retry++)>2000){
//			//��ʱ��
//			return 0xff;
//		}
//	}
//	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����
}
