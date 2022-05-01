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

//复位脚----PC4
#define SX1278_RST_PIN_INDEX	GPIO_PIN_11
#define SX1278_RST_PIN_GROUP	GPIOB
//SPI片选使用的PA4
#define SX1278_CS_PIN_INDEX	GPIO_PIN_12
#define SX1278_CS_PIN_GROUP	GPIOB

//DIO0引脚(这里修改了需要手动修改中断初始化和中断函数)
#define SX1278_DIO0_PIN_INDEX	GPIO_PIN_10
#define SX1278_DIO0_PIN_GROUP	GPIOB
//DIO1引脚(这里修改了需要手动修改中断初始化和中断函数)
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
/* 我们的RA-01/02模组的天线切换开关由硬件自动控制，不需要软件控制
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;*/

/*!
 * Tx and Rx timers
 */
/*TimerEvent_t TxTimeoutTimer;
TimerEvent_t RxTimeoutTimer;
TimerEvent_t RxTimeoutSyncWord;*/

//实现ms级延时
//delayMs：延时的ms数
void SX1276DelayMs(uint32_t delayMs){
	HAL_Delay(delayMs);
}

//定时器初始化(RX,TX,SyncWord定时完成后都需要调用 SX1276OnTimeoutIrq() 函数)
void SX1276TimerInit(void){
}

void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
//  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

//	TIM3->CR1 |= TIM_CR1_URS;	//如果不设置这个会导致定时器启动的时候立即进入中断
//	
//	//定时器TIM3初始化
//	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
// 
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
//	
//	//中断优先级NVIC设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
//	
//	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

//	TIM_Cmd(TIM3, ENABLE);  //启动TIM					 
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志
//		TIM_Cmd(TIM3, DISABLE);  //失能TIMx
//		SX1276OnTimeoutIrq();	//发生中断后需要调用 SX1276OnTimeoutIrq 函数
//		//printf("\r\ntimer irq\r\n");
//	}
}
//开启 tx 超时定时器，定时timeoutMs 毫秒
void SX1276TxTimeoutTimerStart( uint32_t timeoutMs ){
	//这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
//	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz的计数频率，计数到10次为1ms 
}

//关闭 tx 超时定时器
void SX1276TxTimeoutTimerStop(void){
	///这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
//	TIM_Cmd(TIM3, DISABLE);  //失能TIMx
}

//开启 RX 超时定时器，定时timeoutMs 毫秒
void SX1276RxTimeoutTimerStart( uint32_t timeoutMs ){
	//这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz的计数频率，计数到10次为1ms 
}

//关闭 RX 超时定时器
void SX1276RxTimeoutTimerStop(void){
	///这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
//	TIM_Cmd(TIM3, DISABLE);  //失能TIMx	
}

//开启 SyncWord 超时定时器，定时timeoutMs 毫秒
void SX1276SyncWordTimeoutTimerStart( uint32_t timeoutMs ){
	//这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
	TIM3_Int_Init((timeoutMs * 10) -1,7199);//10Khz的计数频率，计数到10次为1ms 
}

//关闭 SyncWord 超时定时器
void SX1276SyncWordTimeoutTimerStop(void){
	///这里简化使用了一个定时器，实际工程中最好使用独立的3个定时器，或者使用软件定时器
//	TIM_Cmd(TIM3, DISABLE);  //失能TIMx	
}

//初始化SPI(cs低使能，极性SPI_CPOL_High，相位SPI_CPHA_2Edge，SPI_FirstBit_MSB)和Rst脚
void SX1276IoInit( void )
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	SPI_InitTypeDef  SPI_InitStructure;
//	
	//初始化一个gpio用来控制spi1的cs
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Pin = SX1278_CS_PIN_INDEX;
//	HAL_GPIO_Init(SX1278_CS_PIN_GROUP, &GPIO_InitStruct);

	//SPI的初始化
	//MX_SPI2_Init();
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//SPI时钟使能
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	//片上外设模式
//	GPIO_InitStructure.GPIO_Pin = SX1278_SCK_PIN_INDEX|SX1278_MISO_PIN_INDEX|SX1278_MOSI_PIN_INDEX;
//	GPIO_Init(SX1278_SCK_PIN_GROUP, &GPIO_InitStructure);	//初始化GPIO

//	//配置SPI
//	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
//	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
//	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//设置时钟极性(串行同步时钟的空闲状态为高电平还是低电平)
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//设置相位(串行同步时钟的第几个跳变沿（上升或下降）数据被采样)
//	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:设置为软件控制(SSI控制)
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//定义波特率预分频的值:波特率预分频值为256(256是最低,可以设置完成之后调整速度,如果速度过快导致通信失败再调小)
//	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式(CRC校验相关)
//	SPI_Init(SPI1,&SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
//	
//	SPI_Cmd(SPI1,ENABLE); //使能SPI外设
}

DioIrqHandler **g_irqHandlers;

//初始化DIO(上升沿)中断，将 DIO0~5 的GPIO中断函数调用 irqHandlers[0]~irqHandlers[5]
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

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能RCC_APB2Periph_AFIO口时钟
//	
//	//初始化DIO0
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	    //使能指定端口时钟（这里是为了方便ABC全开了）
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz，这里不用传参，直接写死用最大速度50MHZ
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//下拉输入
//	GPIO_InitStructure.GPIO_Pin = SX1278_DIO0_PIN_INDEX;
//	GPIO_Init(SX1278_DIO0_PIN_GROUP, &GPIO_InitStructure);	//初始化GPIO
//	
//	//配置中断
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);//设置映射线关系(这里的 GPIO_PortSourceGPIOX 刚好是0-5，所以直接用pinName/16表示，GPIO_PinSourceXX刚好是0-15，所以用pinName%16表示了)
//	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//中断线u8_pinNum
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//外部中断
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升降沿触发
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能中断
//	EXTI_Init(&EXTI_InitStructure);//配置中断

//	//配置中断优先级
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	//外部中断XX
//	//stm32f103c8例子中这个优先级对应抢占优先级2子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;	//抢占优先级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;   //子优先级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断
//	NVIC_Init(&NVIC_InitStructure);   //配置优先级
//	
//	EXTI_ClearITPendingBit(EXTI_Line0);//清除中断标志位
//	
//	//DIO1中断
//	GPIO_InitStructure.GPIO_Pin = SX1278_DIO1_PIN_INDEX;
//	GPIO_Init(SX1278_DIO1_PIN_GROUP, &GPIO_InitStructure);	//初始化GPIO
//	
//	//配置中断
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);//设置映射线关系(这里的 GPIO_PortSourceGPIOX 刚好是0-5，所以直接用pinName/16表示，GPIO_PinSourceXX刚好是0-15，所以用pinName%16表示了)
//	EXTI_InitStructure.EXTI_Line=EXTI_Line1;	//中断线u8_pinNum
//	EXTI_Init(&EXTI_InitStructure);//配置中断

//	//配置中断优先级
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;	//外部中断XX
//	NVIC_Init(&NVIC_InitStructure);   //配置优先级
//	
//	EXTI_ClearITPendingBit(EXTI_Line1);//清除中断标志位
	
	//DIO2~5省略了
}

////外部中断线0中断处理函数，这里需要回调对应的函数
//void EXTI0_IRQHandler(void){
//	g_irqHandlers[0]();
////	EXTI_ClearITPendingBit(EXTI_Line0);//清除中断标志位
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
////外部中断线1中断处理函数，这里需要回调对应的函数
//void EXTI1_IRQHandler(void){
//	g_irqHandlers[1]();
////	EXTI_ClearITPendingBit(EXTI_Line1);//清除中断标志位
//}

//将SPI、DIO0~5和Rst脚的中断去初始化
void SX1276IoDeInit( void )
{
	//这里省略去初始化的功能了
}

//判断频率是否合法
//如果不限制频率可以直接return true
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
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	
	//__HAL_RCC_GPIOB_CLK_ENABLE();
	

	
	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_RESET);  //输出低电平
	HAL_Delay(1);
	
//	HAL_GPIO_DeInit(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX);
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Pin = SX1278_RST_PIN_INDEX;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	HAL_GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SX1278_RST_PIN_GROUP, SX1278_RST_PIN_INDEX, GPIO_PIN_SET);  //输出低电平
	HAL_Delay(6);
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	    //使能指定端口时钟（这里是为了方便ABC全开了）
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz，这里不用传参，直接写死用最大速度50MHZ
//	GPIO_InitStructure.GPIO_Pin = SX1278_RST_PIN_INDEX;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	//设置为推挽输出
//	GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStructure);	//初始化GPIO

//	GPIO_ResetBits(SX1278_RST_PIN_GROUP,SX1278_RST_PIN_INDEX);	//输出低电平
//	delay_ms(1);
//	
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
//	GPIO_Init(SX1278_RST_PIN_GROUP, &GPIO_InitStructure);	//初始化GPIO
//	delay_ms(6);
}

/**
  * 设置SPI的片选脚的电平(低电平使能，高电平取消)
  * lev:true的时候高电平,false的时候低电平
  */
void Sx1276SetNSS(bool lev )
{
	if(lev){
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_SET);	//输出高电平
	}else{
		HAL_GPIO_WritePin(SX1278_CS_PIN_GROUP, SX1278_CS_PIN_INDEX, GPIO_PIN_RESET);	//输出低电平
	}
}

/**
  * SPI读写函数,data是发送的数据，返回值是接收到的数据
  */
uint8_t Sx1276SpiInOut(uint16_t data )
{
	return SPI1_ReadWriteByte(data);
//	//完成SPI的读写功能
//	uint16_t retry=0;				 	
//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){//检查指定的SPI标志位设置与否:发送缓存空标志位(缓冲区空了就可以拷贝数据了)
//		if((retry++)>2000){
//			//超时了
//			return 0xff;
//		}
//	}
//	SPI_I2S_SendData(SPI1, data); //通过外设SPIx发送一个数据
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){ //检查指定的SPI标志位设置与否:接受缓存非空标志位（非空了就表示接收数据完成了）
//		if((retry++)>2000){
//			//超时了
//			return 0xff;
//		}
//	}
//	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}
