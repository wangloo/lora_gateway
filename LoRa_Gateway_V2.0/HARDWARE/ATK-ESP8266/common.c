/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� ������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#include "common.h"
#include "transport.h"

struct Wifi_s Wifi =
{
		WIFI_IDLE,             /* wifi state : init idle mode*/
		WIFI_MODE_NORMAL,      /* wifi transport mode: init normal mode */
		
    ATK8266_Init,          /* wifi init function */
		ATK8266_SetTcpClient,  /* set wifi to tcp client mode function */
		ATK8266_QuitTransMode,
		ATK8266_SetTransMode,
		ATK8266_Send_Cmd,
		ATK8266_Send_Data,

};

/* �¼���־����, definition in freertos.c */
extern EventGroupHandle_t EventGroupHandler; 

void ATK8266_Init(void);
u8* ATK8266_Check_Cmd(u8* str);
u8* ATK8266_Send_Cmd(u8* cmd, u8* ack, u16 waitTime);
u8 ATK8266_Send_Data(u8 *data);
void ATK8266_SetTcpClient(const u8* ssid, const u8* password, const u8* tcpIp, const u8* tcpPort, u8 isTrans);

//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s",USART2_RX_BUF);	//���͵�����
		if(mode)USART2_RX_STA=0;
	} 
}

/***************************************************************
 * \brief  ���������,�����յ���Ӧ��
 *         
 * \param  [IN] str      :  �ڴ���Ӧ����
 *
 * \retval      0        :  �յ�������ack
 *              1        :  ���ackʧ��
 ***************************************************************/
u8* ATK8266_Check_Cmd(u8* str) {
	u8* res = NULL;
	
	if(USART2_RX_STA&0X8000)		/* ���ڽ��յ�һ������ */
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF] = 0; /* ��ӽ����� */
		res = strstr((const char*)USART2_RX_BUF,(const char*)str);

	} 
	return res;
}

/***************************************************************
 * \brief  ͨ��������ATK-ESP8266����ATָ��
 *         ����ack������
 * \param  [IN] cmd      :  ���͵������ַ���
 * \param  [IN] ack      :  �ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
 * \param  [IN] waitTime :  �ȴ�ʱ��(��λ:10ms)
 *
 * \retval NULL          :  ����ʧ��
 *         (u8 *)        :  �յ�Ŀ��ack����������
 ***************************************************************/
u8* ATK8266_Send_Cmd(u8* cmd, u8* ack, u16 waitTime)
{
	u8* res = NULL; 
	
	if (Wifi.TransptMode == WIFI_MODE_TRANS) {
		/* ͸��ģʽ���ܷ������ */
		return NULL;
	}
	
	USART2_RX_STA=0;
	u2_printf("%s\r\n",cmd);	/* ��������(ATָ�����ӻ���) */
	
	if (ack && waitTime)		//��Ҫ�ȴ�Ӧ��
	{
		while (--waitTime) {
			HAL_Delay(10);
			if (USART2_RX_STA&0X8000) { /* ���ڽ�����ɱ�� */
				res = ATK8266_Check_Cmd(ack);
				if (res != NULL) {
					printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART2_RX_STA=0;
			} 
		}
		if (waitTime == 0) { /* ��ʱ */
			printf("send cmd:\"%s\" timeout!\r\n", cmd);
			res = NULL;
		}			
	}
	return res;
} 

/***************************************************************
 * \brief  ͨ��������ATK-ESP8266��������
 *         (����Ŀû��ʹ��, ѡ��transport_sendPacketBuffer())
 * \param  data       :  ���͵�����
 *
 * \retval WIFI_OK    :  ���ͳɹ�
 *         WIFI_ERROR :  ����ʧ��
 ***************************************************************/
u8 ATK8266_Send_Data(u8 *data)
{
	u8 res = WIFI_OK; 
	
	if (Wifi.TransptMode == WIFI_MODE_NORMAL) {
		/* ֻ��͸��ģʽ���ܷ����ݣ� */
		return WIFI_ERROR;
	}
	
	USART2_RX_STA=0;
	u2_printf("%s",data);	/* ��������(���ݲ���Ҫ��ӻ���) */
	
	/* ����Ŀû��ʹ�ù�û��ʵ�ָ��࣬
	   �����Ҫ������ʵ������ack����ʱ�ȹ��� */
	/* ... */
	return res;
}

/***************************************************************
 * \brief  ATK-ESP8266�˳�͸��ģʽ
 *
 * \retval WIFI_OK       :  �˳��ɹ�
 *         WIFI_ERROR    :  �˳�ʧ��
 ***************************************************************/
u8 ATK8266_QuitTransMode(void) {

	if (Wifi.TransptMode == WIFI_MODE_TRANS) {
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(15);				       	/* ���ڴ�����֡ʱ��(10ms) */
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(15);					      /* ���ڴ�����֡ʱ��(10ms) */
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(500);					      /* �ȴ�500ms */

		if (ATK8266_Send_Cmd("AT", "OK", 20) == NULL) { /* �˳�͸���ж� */
			return WIFI_ERROR;
		}
		Wifi.TransptMode = WIFI_MODE_NORMAL;
		return WIFI_OK;
		
	} else if (Wifi.TransptMode == WIFI_MODE_NORMAL){
		/* ԭ������͸��ģʽ, ����Ҫ�˳� */
		return WIFI_OK;
	}

}

/***************************************************************
 * \brief  ����ΪATK-ESP8266͸��ģʽ
 *
 * \retval WIFI_OK       :  �˳��ɹ�
 *         WIFI_ERROR    :  �˳�ʧ��
 ***************************************************************/
u8 ATK8266_SetTransMode(void) {

	if (Wifi.TransptMode == WIFI_MODE_NORMAL) {
	  /* set transparent transport mode and start transparent transport. */
		if ((ATK8266_Send_Cmd("AT+CIPMODE=1", "OK", 200) == NULL) || \
		    (ATK8266_Send_Cmd("AT+CIPSEND","OK",20) == NULL)) {       
			return WIFI_ERROR;
		}
		
		Wifi.TransptMode = WIFI_MODE_TRANS;
		return WIFI_OK;
		
	} else if (Wifi.TransptMode == WIFI_MODE_TRANS){
		/* �Ѵ���͸��ģʽ */
		return WIFI_OK;
	}
}



void ATK8266_Init(void)
{

  while(Wifi.SendCmd("AT","OK",20) == NULL)//���WIFIģ���Ƿ�����
	{
		printf("��������ATK-ESP8266...\r\n");
		
		/* ���ǵ�ǰ����͸��ģʽ����� */
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(15);				       	/* ���ڴ�����֡ʱ��(10ms) */
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(15);					      /* ���ڴ�����֡ʱ��(10ms) */
		while((USART2->SR&0X40)==0);	/* �ȴ����Ϳ� */
		USART2->DR='+';      
		HAL_Delay(500);					      /* �ȴ�500ms */
		ATK8266_Send_Cmd("AT+CIPMODE=0", "OK", 20);	/* �ر�͸��ģʽ	*/

		HAL_Delay(300);

	} 
	printf("\r\nATK-ESP8266���ӳɹ���\r\n\r\n");
	
	while(Wifi.SendCmd("ATE0", "OK", 20) == NULL);	/* �رջ��� */

}

void ATK8266_SetTcpClient(const u8* ssid, const u8* password, const u8* tcpIp, const u8* tcpPort, u8 isTrans) {
	u8* tpAPCmd;	     /* ��ʱ��� Client �����ⲿAP��ָ�� */
	u8* tpTcpCmd;      /* ��ʱ��� Client ����TCP����˵�ָ�� */
	
	tpAPCmd = mymalloc(32);
	tpTcpCmd = mymalloc(32);
	
	Wifi.SendCmd("AT+CWMODE=1","OK",50);	 /* set STA mode */
	Wifi.SendCmd("AT+RST","OK",50);		   /* close DHCP(only AP mode) */
	
	HAL_Delay(4000);                           /* delay 4s for reset */   
	
	sprintf((char *)tpAPCmd,"AT+CWJAP=\"%s\",\"%s\"",ssid, password);	/* �������߲���:ssid,���� */
	while(Wifi.SendCmd(tpAPCmd,"OK",300) == NULL);					  /* connect to external AP */
	/* �ж�AP�Ƿ����ӳɹ� */
	Wifi.SendCmd("AT+CIPSTA?",":",50);	    /* check status of AP connection */
	if(ATK8266_Check_Cmd("+CIPSTA:") == NULL) {
		printf("����APʧ��! ����\r\n");
		return;
	}
	printf("\r\n��������AP�ɹ�, ssid = %s, password = %s\r\n", ssid, password);
  xEventGroupSetBits(EventGroupHandler, EVENTBIT_WIFI_CONNECTED);	/* �޸��¼���־λ */
	
	/* 0: ������, 1:������ */
	Wifi.SendCmd("AT+CIPMUX=0", "OK", 20);             
	
	sprintf((char *)tpTcpCmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", tcpIp, tcpPort);    /* ����Ŀ��TCP������ */
	while(Wifi.SendCmd(tpTcpCmd, "OK", 200) == NULL);
	/* �ж�TCP�Ƿ����ӳɹ� */
	Wifi.SendCmd("AT+CIPSTATUS",":",50);	/* check status of TCP connection */
	if(ATK8266_Check_Cmd("STATUS:3") == NULL) {
		printf("����TCPʧ��! ����\r\n");
		return;
	} 
	Wifi.State = WIFI_STA_TCP_CLIENT;
	printf("\r\n����TCP���ӳɹ�, host = %s, port = %s\r\n",tcpIp, tcpPort);	


	/* need change transport mode? */
	if (isTrans == WIFI_MODE_TRANS) {

		Wifi.SetTrans();
	}
	
	/* free temp variable */
	myfree(tpTcpCmd);
	myfree(tpAPCmd);
}











































