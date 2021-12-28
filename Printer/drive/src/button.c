/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   ��ֽ�����Լ���̫����λ���������صĳ���.
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
#include	"button.h"
#include	"timconfig.h"
#include	"feedpaper.h"
#include	"esc.h"
#include	"extgvar.h"

extern uint32_t	GetPrintBusyStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�������ֽ�����Ķ˿�����,����Ϊ��������
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
void	InitFeedButtonPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#ifdef	FEED_BUTTON_CHECK_BY_INT
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(FEED_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= FEED_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;				//2016.06.09   
	GPIO_Init(FEED_BUTTON_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(FEED_BUTTON_GPIO_PORT_SOURCE, FEED_BUTTON_GPIO_PIN_SOURCE);	//�ж�
  EXTI_InitStructure.EXTI_Line = FEED_BUTTON_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//�ⲿ�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//�½�����Ч
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#else
	RCC_APB2PeriphClockCmd(FEED_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= FEED_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(FEED_BUTTON_GPIO_PORT, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����ֽ����״̬
* Input          : ��
* Output         : �������,�����߹�
* Return         : ���ز���,0/1
*******************************************************************************/
uint8_t	ReadFeedButtonStatus(void)
{	//����ܽ�ֵ
	if (FEED_BUTTON_VALUE_CLOSE == GPIO_ReadInputDataBit(FEED_BUTTON_GPIO_PORT, FEED_BUTTON_GPIO_PIN))
	{
		return	(FEED_BUTTON_STATUS_CLOSE);
	}
	else
	{
		return	(FEED_BUTTON_STATUS_OPEN);
	}
}

/*******************************************************************************
* Function Name  : CheckFeedButton
* Description    : Feed���Ĵ���,����������ʱ,����ֽģʽ��ס��ֽ,�ɿ�ֹͣ.
*				   �ڱ�ֽģʽʱ,�߹��ڱ괦,�����300mm��û���ҵ��ڱ�,��ֹͣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	CheckFeedButton(void)
{
	uint32_t Temp;
	uint32_t Feednum;
  if(g_tSysConfig.BlackMarkEnable)	
	{	
		if((FeedToGapFlag)&&((g_tError.PaperSensorStatusFlag & 0x01) == 0) && ((g_tError.AR_ErrorFlag & 0x02) == 0)
							&& (g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0  )    //  2016.01 ��ֽ������ֽ��־����λ
		{
				DelayUs(400);
				FindMarkPoint();                //���ںϸǺ��Һڱ� 2016.07.02
				FeedToGapFlag = 0;
		}
  }
	
	if (g_tFeedButton.KeyEnable != 0)	//��������
	{
		return;
	}	//��������ʱ��鰴���������
	
	if (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)	// ������ӡ ���жϰ��� 2016.06.16
	{
		return;
	}	
	
	if (g_tFeedButton.KeyStatus == 0)	//û�а�������
	{
		return;
	}	//û�а������´������
	
	//����Ϊ���������а������´���
	if (g_tFeedButton.KeyCount)	//����������Ϊ0
	{
		if (g_tSystemCtrl.SelfTestMode)	//�Լ�ģʽʱ����ֽ
		{
			g_tFeedButton.KeyCount--;
			if (g_tFeedButton.KeyCount == 0)
			{
				g_tFeedButton.KeyStatus = 0;
			}
			return;
		}	//�Լ�ģʽ�������
		else if (g_tSystemCtrl.HexMode)	//16����ʱ��������
		{
			return;
		}
		else	//����ģʽ����ʼ
		{
			if (g_tMacro.RunFlag == 0)	//�������ڴ���궨��
			{
				//����Ƿ������ֽ, ���������˳�����				
				if (PRINTER_STATUS_BUSY == GetPrintBusyStatus())
				{
					return;
				}
				
				//���������ֽ
				g_tRealTimeStatus.PrinterStatus |= 0x08;	//�Ƚ�������״̬
				if (PAPER_TYPE_CONTINUOUS == g_tSystemCtrl.PaperType)	//����ֽ
				{
					if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//����������
					{	//�ɿ�������ֹͣ��ֽ
						Feednum=0;
						while (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
						{
							if(Feednum<10)
							{	
								g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 �������±�־ ��λ
								g_tFeedButton.StepCount = 200;							
								DelayMs(50);
								Feednum++;	
							}		
						}
					
						g_tFeedButton.KeyStatus = 0;	//������Ч
						g_tFeedButton.KeyCount = 0;		//���֮ǰ���еļ���
					}
					else	//�����Ѿ��ɿ�,ֻ�����ѱ���İ���������ֽ
					{
						while (g_tFeedButton.KeyCount != 0)
						{
// 							GoDotLine(DEFAULT_CODE_LINE_HEIGHT);	//��ֽ1��
// 							g_tFeedButton.KeyCount--;
							g_tFeedButton.KeyCount = 0;   //2020.09.28  �ɿ����������� ������ֽ
						}
						g_tFeedButton.KeyStatus = 0;	//������Ч
					}
				}			//����ֽ�������
				else	//�ڱ�ֽ����ʼ
				{
					FindMarkPoint();
					g_tFeedButton.KeyStatus = 0;	//������Ч
					g_tFeedButton.KeyCount = 0;		//���֮ǰ���еļ���
				}
				
				g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//��ֽ���,�Զ���������״̬
			}	//��������ִ�к괦�����
		}	//����ģʽ�������
	}	//����������Ϊ�㴦�����
	else
	{
		g_tFeedButton.KeyStatus = 0;	//�ް�������
	}	//��������Ϊ�㴦�����
	
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�������̫����λ�����Ķ˿�����,����Ϊ��������
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
void	InitResetEthernetButtonPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
#ifdef	RESET_ETHERNET_BUTTON_CHECK_BY_INT
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= RESET_ETHERNET_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(RESET_ETHERNET_BUTTON_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(RESET_ETHERNET_BUTTON_GPIO_PORT_SOURCE, RESET_ETHERNET_BUTTON_GPIO_PIN_SOURCE);	//�ж�
  EXTI_InitStructure.EXTI_Line = RESET_ETHERNET_BUTTON_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//�ⲿ�ж�	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//��������Ч
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#else
	RCC_APB2PeriphClockCmd(RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= RESET_ETHERNET_BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(RESET_ETHERNET_BUTTON_GPIO_PORT, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ����ӡ�����Ƹ�λ��̫���ܽ�
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
void	InitPrtCtrlResetEthernetPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= PRT_CTRL_RESET_ETH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����̫����λ����״̬
* Input          : ��
* Output         : �������,�����߹�
* Return         : ���ز���,0/1
*******************************************************************************/
uint8_t	ReadResetEthernetButtonStatus(void)
{	//����ܽ�ֵ
	if (RESET_ETHERNET_BUTTON_VALUE_CLOSE == 
			GPIO_ReadInputDataBit(RESET_ETHERNET_BUTTON_GPIO_PORT, RESET_ETHERNET_BUTTON_GPIO_PIN))
	{
		return	(RESET_ETHERNET_BUTTON_STATUS_CLOSE);
	}
	else
	{
		return	(RESET_ETHERNET_BUTTON_STATUS_OPEN);
	}
}

void	ResetEthernet(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= PRT_CTRL_RESET_ETH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(PRT_CTRL_RESET_ETH_GPIO_PORT, PRT_CTRL_RESET_ETH_GPIO_PIN);
	DelayMs(RESET_ETHERNET_MS);
	GPIO_SetBits(PRT_CTRL_RESET_ETH_GPIO_PORT, PRT_CTRL_RESET_ETH_GPIO_PIN);
	
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPD;
	GPIO_Init(PRT_CTRL_RESET_ETH_GPIO_PORT, &GPIO_InitStructure);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
