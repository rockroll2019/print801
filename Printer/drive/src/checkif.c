/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   ͨ�Žӿڼ����صĳ���.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
	* ͨ���������˿ڵ�״̬�жϽӿ��ͺţ�Ϊ��ʱΪ����+U���ͺţ�Ϊ��ʱΪ�����ͺš�
  */

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"checkif.h"
#include	"debug.h"
#include	"usart.h"
#include	"usb_main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define		INTERFACE_TYPE_A_VALUE					(0x00)    
#define		INTERFACE_TYPE_B_VALUE					(0x03)
#define		INTERFACE_TYPE_C_VALUE					(0x02)

#define		INTERFACE_TYPE_A								(0x00)		//���� 
#define		INTERFACE_TYPE_B								(0x01)		//USB_BT
#define		INTERFACE_TYPE_C								(0x02)		//����
#define		INTERFACE_TYPE_D								(0x03)		//����
#define		INTERFACE_TYPE_UNKOWN						(0xFF)		//NONE

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ���ӿڼ��˿ڣ�һ������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitCheckInterfacePort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;											//���ո�Ϊ�������� �жϽӿ�״̬
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��⺸�ӵ�ͨ�Žӿ�״̬
* Input          : ��
* Output         : ����������ӿ�����
* Return         : ���ز�����ͨ�Žӿ�
*******************************************************************************/
uint8_t	GetInterfaceType(void)
{
		uint8_t	i;
		uint8_t	GetData0=0,GetData1=0;
	
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
		GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN|INTERFACE_CHECK_IN1_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;											
		GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
		GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN0_GPIO_PIN))
		{
				GetData0|=0x01;
		}	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN1_GPIO_PIN))
		{
				GetData1|=0x01;
		}	

		RCC_APB2PeriphClockCmd(INTERFACE_CHECK_IN_RCC_APB2Periph_GPIO, ENABLE);
		GPIO_InitStructure.GPIO_Pin			= INTERFACE_CHECK_IN0_GPIO_PIN|INTERFACE_CHECK_IN1_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;											//���ո�Ϊ�������� �жϽӿ�״̬
		GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
		GPIO_Init(INTERFACE_CHECK_IN_GPIO_PORT, &GPIO_InitStructure);
	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN0_GPIO_PIN))
		{
				GetData0|=0x02;
		}	
		if(GPIO_ReadInputDataBit(INTERFACE_CHECK_IN_GPIO_PORT, INTERFACE_CHECK_IN1_GPIO_PIN))
		{
				GetData1|=0x02;
		}	
	
		
// 	GetData0 = INTERFACE_TYPE_B_VALUE;   //2016.06.12
		if (GetData0 == INTERFACE_TYPE_A_VALUE)		
		{
				i	= INTERFACE_TYPE_A;//P 0                //����
		}
		else if (GetData0 == INTERFACE_TYPE_B_VALUE)
		{
				i	= INTERFACE_TYPE_B;//S+U 1              //USB BT 
		}
		else if (GetData0 == INTERFACE_TYPE_C_VALUE)
		{
				if (GetData1 == INTERFACE_TYPE_A_VALUE)	
				{
						i	= INTERFACE_TYPE_C;                    //NET
				}
				else if (GetData1 == INTERFACE_TYPE_B_VALUE)
				{
						i	= INTERFACE_TYPE_D;                    //����
				}
				else 
				{
						i	= INTERFACE_TYPE_UNKOWN;
				}	
		}
		else
		{
				i	= INTERFACE_TYPE_UNKOWN;
		}
	
		return	(i);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��⺸�ӵ�ͨ�Žӿ�����,���ݽӿ����ำֵ�ӿ�ȫ�ֱ�����ֵ
* Input          : ��
* Output         : ����������ӿ�����
* Return         : ���ز�����ͨ�Žӿ�
*******************************************************************************/
void	CheckInterface(void)
{
	uint8_t	type;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART_BUSY_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	 //æ���жϽӿ�����
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(USART_BUSY_PORT,USART_BUSY_PIN);
	GPIO_InitStructure.GPIO_Pin		= USART_BUSY_PIN;	   	 //æ���жϽӿ�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //��������
  GPIO_Init(USART_BUSY_PORT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(USART_BUSY_PORT,USART_BUSY_PIN))  //����
	{
		g_tInterface.COM_Type = 0;	
	}
	else
	{
		g_tInterface.COM_Type = 1;	
	}	
	
	RCC_APB2PeriphClockCmd(USB_RCC_APB2Periph_PORT, ENABLE);	 //2018.02.28 
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;		   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(USB_DISCONNECT,USB_DISCONNECT_PIN);
	GPIO_InitStructure.GPIO_Pin		= USB_DISCONNECT_PIN;	   	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //��������
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(USB_DISCONNECT,USB_DISCONNECT_PIN))  //usb
	{
 		g_tInterface.USB_Type = 0;
	}
	else
	{
	  g_tInterface.USB_Type = 1;		//2016.07.21
	}	
	
	RCC_APB2PeriphClockCmd(CHECK_NET_RCC_APB2Periph_GPIO, ENABLE);  //2018.02.28  PA2�ӵ�����MDIO������ �жϴ˽�
	GPIO_InitStructure.GPIO_Pin		= CHECK_NET_GPIO_PIN;	   	 
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
  GPIO_Init(CHECK_NET_GPIO_PORT, &GPIO_InitStructure);
	DelayUs(50);
  type = GPIO_ReadInputDataBit(CHECK_NET_GPIO_PORT,CHECK_NET_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin		= CHECK_NET_GPIO_PIN;	   	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   		 //��������
  GPIO_Init(CHECK_NET_GPIO_PORT, &GPIO_InitStructure);
  DelayUs(50);
	if(type != GPIO_ReadInputDataBit(CHECK_NET_GPIO_PORT,CHECK_NET_GPIO_PIN))  //�����򲻴�������
	{
		g_tInterface.ETH_Type = 0;
	}
	else
	{
 	  g_tInterface.ETH_Type = 1;
	}	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
