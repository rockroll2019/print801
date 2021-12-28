/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   usart��س���
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include	"usart.h"
#include	"extgvar.h"
#include	"debug.h"

extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�����ڿ��ƹܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitUsartPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/************* ���ô��ڶ˿� ****************************/
	RCC_APB2PeriphClockCmd(USART_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		=	USART_RXD_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//��������  
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(USART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		=	USART_TXD_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;		//�������
	GPIO_Init(USART_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(USART_BUSY_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	//æ
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
	SetUsartHardBusy();			//����Ϊæ
}

/*******************************************************************************
* Function Name  : void ConfigUsart(void)
* Description    : Configures the USART.��׼ȫ˫��ģʽ���жϽ��ա���ѯ����
	M		PCE		USART֡���
	0		0			1λ��ʼλ + 8λ����λ + ֹͣλ
	0		1			1λ��ʼλ + 7λ����λ + ��żУ��λ + ֹͣλ
	1		0			1λ��ʼλ + 9λ����λ + ֹͣλ
	1		1			1λ��ʼλ + 8λ����λ + ��żУ��λ + ֹͣλ
	���Ե���������Ϊ8λʱ,����������żУ��,Ҳ���Բ���Ҫ��żУ��.
	���ǵ���������Ϊ7λʱ,����������żУ��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	ConfigUsart(void)
{
	USART_InitTypeDef		USART_InitStructure;
	const uint32_t m_tBpsValueTable[] = 
	{
		1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
	};	//�����ʱ�
	
#if	defined	DBG_USART || defined	DBG_ADC	|| defined	DBG_INBUF_RCV	|| defined	DBG_SPI_ZIKU	|| defined	DBG_SPI_ROM
	/*����Ϊ���Թ��ܳ���*/
	RCC_APB1PeriphClockCmd(USART_RCC_APBPeriph, ENABLE);	//ʹ��ʱ��
	
	USART_InitStructure.USART_BaudRate		= 115200;								//���ò�����	
	USART_InitStructure.USART_Parity			= USART_Parity_No; 			//��У��
	USART_InitStructure.USART_WordLength	= USART_WordLength_8b;	//8λ����	
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;			//ֹͣλΪ1λ
	USART_InitStructure.USART_Mode				= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_Init(USART_SOURCE, &USART_InitStructure);			//��ʼ��USART_SOURCE
	
	USART_ITConfig(USART_SOURCE, USART_IT_RXNE, ENABLE);	//��USART�����ж�
	USART_Cmd(USART_SOURCE, ENABLE);										//ʹ��USART_SOURCE�豸
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TC);				//�巢����ɱ�־
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TXE);				//�巢����ɱ�־ 2017.10.18  ����GDоƬ
#else
	/*����Ϊ�������ܳ���*/
	uint8_t Temp;
	
	RCC_APB1PeriphClockCmd(USART_RCC_APBPeriph, ENABLE);	//ʹ��ʱ��
	//���ò�����
	#ifdef	UART_MAX_SPEED_ENABLE
		USART_InitStructure.USART_BaudRate = UART_MAX_BAUD;
		USART_InitStructure.USART_Parity	 = USART_Parity_No;			//��У��
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//7λ
	#else
		USART_InitStructure.USART_BaudRate = m_tBpsValueTable[g_tSysConfig.ComBaudRate & 0x07];
		Temp = g_tSysConfig.ComProtocol & 0x03;
		if (Temp == 0 || Temp == 2)	//��У��
		{
			USART_InitStructure.USART_Parity		 = USART_Parity_No;			//��У��
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ
		}
		else
		{
			if (Temp == 3)						//żУ��
			{
				USART_InitStructure.USART_Parity = USART_Parity_Even;	//żУ��
			}
			else //if( Temp ==1)			//��У��
			{
				USART_InitStructure.USART_Parity = USART_Parity_Odd;	//��У��
			}
			
			if (g_tSysConfig.ComProtocol & 0x08)
			{	
				USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//7λ
			}
			else
			{
				USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//8λ
			}
		}
	#endif
	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					//ֹͣλΪ1λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��ʹ��Ӳ��������
	USART_Init(USART_SOURCE, &USART_InitStructure);			//��ʼ��USART_SOURCE
	
	USART_ITConfig(USART_SOURCE, USART_IT_RXNE, ENABLE);	//��USART�����ж�
	USART_Cmd(USART_SOURCE, ENABLE);										//ʹ��USART_SOURCE�豸
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TC);				//�巢����ɱ�־
	USART_ClearFlag(USART_SOURCE, USART_FLAG_TXE);				//�巢����ɱ�־ 2017.10.18  ����GDоƬ	
#endif
}

/*******************************************************************************
* Function Name  : UsartSendStr(USART_TypeDef * USARTx, * uint8_t Str)
* Description    : ��ָ���Ĵ��ڷ���һ�ַ���
* Input          : USARTX�����ںţ�Srt:�ַ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
void UsartSendStr(USART_TypeDef * USARTx, uint8_t * Str)
{
	while (*Str)
	{
		while ((!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE))  && (USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TC) == RESET));   //2017.10.18  ����GDоƬ
		USART_SendData(USARTx, *Str);
		Str++;
	}
}

/*******************************************************************************
* Function Name  : SetUsartBusy
* Description    : ���ô���æ�ź�
* Input          : ������Ӳ������������־���
* Output         : None
* Return         : None
*******************************************************************************/
void	SetUsartBusy(void)
{
	uint8_t	temp;
	
	if ((g_tSysConfig.ComProtocol & USART_HANDSHAKE_FLAG_VALUE) == USART_HANDSHAKE_HARD_VALUE)
	{
		SetUsartHardBusy();		//Ӳ������Э��
	}
	else
	{	//���ò�������²��ܻط�XON/XOFF�Է�ֹ���ù��߻���
		if (g_tSystemCtrl.SetMode == 0)
		{
			temp = XOFF;
			UpLoadData(&temp, 1);	//�ϴ�����
		}
	}
}

/*******************************************************************************
* Function Name  : ClearUsartBusy
* Description    : �������æ�ź�
* Input          : ������Ӳ������������־���
* Output         : None
* Return         : None
*******************************************************************************/
void	ClearUsartBusy(void)
{
	uint8_t	temp;
	
	if ((g_tSysConfig.ComProtocol & USART_HANDSHAKE_FLAG_VALUE) == USART_HANDSHAKE_HARD_VALUE)
	{
		ClearUsartHardBusy();	//Ӳ������Э��
	}
	else
	{	//���ò�������²��ܻط�XON/XOFF�Է�ֹ���ù��߻���
		if (g_tSystemCtrl.SetMode == 0)
		{
			temp = XON;
			UpLoadData(&temp, 1);	//�ϴ�����
		}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
