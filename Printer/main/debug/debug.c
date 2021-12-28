/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ���Կ����׶ι��ܵĳ���
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
#include	"apptype.h"
#include	"mode.h"
#include	"debug.h"

#ifdef	DBG_RX_TOTAL_THEN_PRT
extern volatile uint8_t	g_bRxFinishFlag;	//��ʾ����ͨ���������Ƿ��Ѿ��������
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#ifdef	VERSION_DEBUG	//������Ը�������
#include	"led.h"
#include	"button.h"
#include	"extgvar.h"
#include	"update.h"

void	LedHint(void)
{
#if defined	(FEED_BUTTON_CHECK_BY_INT) || defined (RESET_ETHERNET_BUTTON_CHECK_BY_INT)
	if (g_tFeedButton.KeyStatus)
	{
		PELedOn();						//LEDָʾ��
		if (g_tFeedButton.KeyCount)
			g_tFeedButton.KeyCount--;
		if (!g_tFeedButton.KeyCount)
			g_tFeedButton.KeyStatus = 0;
	}
	else
	{
		PELedOff();						//LEDָʾ��
	}
#else
	//�Բ�ѯ��ʽ���԰���
	if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
	{
		PELedOn();						//LEDָʾ��
	}
	else
	{
		PELedOff();						//LEDָʾ��
	}
#endif
}

#ifdef	DBG_TIM
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ���Ը�����ʱ������,ͨ��LEDָʾ�ƹ۲춨ʱʱ���Ƿ�׼ȷ,
*	���ж�ʱ����ͨ�����ַ�ʽ����.��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�
*	3.�����ж�
*	4.��ʼ����ʱ��,������ʱ��,��ʱ���ж��в���LED
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	TIM_TypeDef * TIMx;
	uint16_t	DelayUs;
	
	SystemInit();					//1.��ʼ��ʱ��
	PELedPortInit();			//2.��ʼ��LED�˿�
	ErrorLedPortInit();	
	InitNVIC();						//3.�����ж�
	
	//4.��ʼ����ʱ��,������ʱ��,��ʱ���ж��в���LED
	#ifdef	DBG_TIM1
	//Ԥ��Ƶ10us,��ʱ1ms
	TimerConfigPrescaler(TIM1, 100000);
	TIMx = TIM1;
	DelayUs = 100;		//������ʱ�ж�����
	#endif
	
	#ifdef	DBG_TIM2
	//����T2Ϊϵͳʱ��,10uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(TIM2, 100000);
	TIMx = TIM2;
	DelayUs = 100;		//������ʱ�ж�����
	#endif
	
	#ifdef	DBG_TIM3
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(TIM3, 1000000);
	TIMx = TIM3;
	DelayUs = 1000;		//������ʱ�ж�����
	#endif
	
	#ifdef	DBG_TIM4
	TimerConfigPrescaler(TIM4, 1000000);	//����T4Ϊ��ֽ��ʱ��,Ԥ��ƵΪ1000000,1uS,����
	TIMx = TIM4;
	DelayUs = 1000;		//������ʱ�ж�����
	#endif
	
	#ifdef	DBG_TIM5
	//����T5Ϊ����AD��ʱ��,Ԥ��Ƶ��ʱ1uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(TIM5, 1000000);
	TIMx = TIM5;
	DelayUs = 1000;		//������ʱ�ж�����
	#endif
	
	TIM_SetCounter(TIMx, DelayUs);		//������ʱ��������ʱ
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	PELedOn();												//LEDָʾ�Ƴ�ʼ��
	ErrorLedOn();
	
	while (1);
}
#endif	/*���ж�ʱ�����Գ������*/


#ifdef	DBG_BUTTON
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ���԰���,ͨ��LEDָʾ����ʾ�Ƿ��⵽����
*	������ֽ��������̫����λ����.ͨ��EXTI�ж�ʵ��.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,����ȫ�ֱ���
*	3.�����ж�
*	4.��鰴������״ֵ̬,����LED
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	////InitResetEthernetButtonPort();
	
	InitKeyCtrlStructParameter();			//2.�������ݽṹ
	
	InitNVIC();						//3.�����ж�
	
	//4.��鰴������״ֵ̬,����LED		
	while (1)
	{
		LedHint();		//ͨ��LEDָʾ����ʾ,�����ǰ����ж��д���,Ҳ������ͨ����ѯ������ʽ����
	}
}
#endif	/*�������Գ������*/


#ifdef	DBG_POWER_CTRL
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"power.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ���Լ��ȵ�Դ��ѹ�������,ͨ��LEDָʾ����ʾ�Ƿ����,��ʾ�����۲�
*	ͨ����ʱ��2�ж�ʵ��.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,��ʱ��2,�����˿�,��Դ���ƶ˿�,����ȫ�ֱ���
*	3.�����ж�
*	4.��鰴������״ֵ̬,����LED
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	TIM_TypeDef * TIMx;
	uint16_t	DelayUs;
	
	SystemInit();					//1.��ʼ��ʱ��
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��Դ���ƶ˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitKeyCtrlStructParameter();			//2.�������ݽṹ
	
	//��ʼ����ʱ��,������ʱ��,��ʱ���ж��в���LED
	//����T2Ϊʱ��,1uS
	TimerConfigPrescaler(TIM2, 1000000);
	TIMx = TIM2;
	DelayUs = 1000;		//������ʱ�ж�����1s
	
	InitNVIC();						//3.�����ж�
	
	TIM_SetCounter(TIMx, DelayUs);		//������ʱ��������ʱ
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	HeatPowerOff();												//��ʼ����Դ���ƶ˿�
	PELedOff();												//LEDָʾ�Ƴ�ʼ��
	ErrorLedOff();
	
	//4.�ж��в�����Դ�Ƿ����������LED
	while (1);
}
#endif	/*���ȵ�Դ������Ʋ��Գ������*/


#ifdef	DBG_USART
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ���ͨ��
*	ͨ�����ڽ����ж�ʵ��.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,����ȫ�ֱ���,����4,
*	3.�����ж�
*	4.�ж��н��յ�������ԭ�����ͻ�PC��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();	
	InitKeyCtrlStructParameter();		//2.�������ݽṹ
	InitCheckInterfacePort();
	CheckInterface();				//���ͨ�Žӿ����Ͳ���ֵȫ�ֱ���,Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	InitUsartPort();
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	//4.��鰴������״ֵ̬,����LED,�����ж��н��յ�������ԭ�����ͻ�PC��
	while (1)
	{
		LedHint();		//ͨ��LEDָʾ����ʾ,�����ǰ����ж��д���,Ҳ������ͨ����ѯ������ʽ����
	}

}
#endif	/*USART���Գ������*/


#ifdef	DBG_ADC
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ����ADC
*	ͨ�����ڷ���ת�����ʵ��.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,����ȫ�ֱ���,����4,ADC,POWER,PE,PNE,BM,TEMP�ȶ˿�,TIM2
*	3.�����ж�
*	4.�ж��н��յ�������ԭ�����ͻ�PC��,ADCת�����ת������Ҫ��ֵ���жϳ����͵�PC��.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint16_t	DelayUs;
	TIM_TypeDef * TIMx;
	
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();	
	InitKeyCtrlStructParameter();		//2.�������ݽṹ
	InitUsartPort();
	ConfigUsart();				//2.�첽��������
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	//��ʼ����ʱ��,������ʱ��,��ʱ���ж��в���LED
	//����T5Ϊʱ��,1uS
	TIMx = TIM5;
	TimerConfigPrescaler(TIMx, 1000000);
	DelayUs = 50000;		//������ʱ�ж�����50ms
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	TIM_SetCounter(TIMx, DelayUs);		//������ʱ��������ʱ
	TIM_SetAutoreload(TIMx, DelayUs);
	StartTimer(TIMx);
	
	//4.��鰴������״ֵ̬,����LED,�����ж��н��յ�������ԭ�����ͻ�PC��
	while (1)
	{
		//LedHint();		//ͨ��LEDָʾ����ʾ,�����ǰ����ж��д���,Ҳ������ͨ����ѯ������ʽ����
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			StopTimer(TIMx);
		}
	}
}
#endif	/*ADC���Գ������*/


#ifdef	DBG_FEED
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ������ֽ�������
*	�����ֽ����,���º���ֽ����.ͨ��DelayUs()ʵ����ʱ.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,����4,ADC,Feed�˿�
*	3.�����ж�
*	4.�����ֽ����,���º����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitFeedPort();
	InitPlatenPort();
	InitCheckInterfacePort();	
	InitKeyCtrlStructParameter();		//2.�������ݽṹ,
	InitErrorStructParameter();			//�������ݽṹ
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	InitUsartPort();
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	
	//4.��鰴������״ֵ̬,����LED,��ֽ,ע����һ����ֽδ����֮ǰ��Ҫ�ٰ�����ֽ��!
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LEDָʾ��
			if ((ReadCoverStatus() == PLATEN_STATUS_CLOSE))
			{
				//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);			//��ֽ����,��Ϊ��������
				FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS * 20);	//��ֽ����,��Ϊ����
				DelayUs(1000*1000);
			}
		}
		else
		{
			PELedOff();						//LEDָʾ��
		}		
	}
	
}
#endif	/* ��ֽFEED���Գ������,ʹ����ʱ��ʽ��ֽ */


#ifdef	DBG_CUT
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#define		CUT_NUMBER		(1)

/*******************************************************************************
* Function Name  : ������
* Description    : �����е��������
*	�����ֽ����,���º���ֽ����,Ȼ����ֽ.ͨ��DelayUs()ʵ����ʱ.
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,����4,ADC,Feed�˿�,�е��˿�
*	3.�����ж�
*	4.�����ֽ����,���º����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iCutResult;
	uint32_t	iCount;
	
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitKeyCtrlStructParameter();		//2.�������ݽṹ
	InitErrorStructParameter();			//�������ݽṹ
	InitUsartPort();
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	InitCutPaper();
	//CutPaper(0);
	
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	//FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	
	//4.��鰴������״ֵ̬,����LED,��ֽ,ע����һ����ֽδ����֮ǰ��Ҫ�ٰ�����ֽ��!
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)	//���°���
		{
			for (iCount = 0; iCount < CUT_NUMBER; iCount++)		//ѭ�����ɴ�
			{
				//if (ReadCoverStatus() == PLATEN_STATUS_CLOSE)
				{
					//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS / 2);			//��ֽ����,��Ϊ��������
					//FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS * 2);	//��ֽ����,��Ϊ����
					FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);	//��ֽ����,��Ϊ����
					iCutResult = CutPaper(1);
					FeedPaper(FEED_FORWARD, FEED_ONE_TIME_STEPS);	//��ֽ����,��Ϊ����
					iCutResult = CutPaper(0);
					if (iCutResult)
					{
						ErrorLedOn();						//LEDָʾ�Ƹ澯
						//break;
					}
					else
					{
						ErrorLedOff();
					}
				}
			}
			DelayUs(1000*1000);
		}
	}
	
}
#endif	/* �е�CUT���Գ������,ʹ����ʱ��ʽ��ֽ */


#ifdef	DBG_INBUF_RCV
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

void	ProcessComInBuf(void)
{
	uint8_t	ReadChar;
	
	if (g_tInBuf.BytesNumber != 0)
	{
		ReadChar = ReadInBuffer();	//��ȡ���ջ�����
		UpLoadData(&ReadChar, 1);
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����USART���ռ�INBUF��������,INBUF���ݲ�ѯ��ʽ���͵�PC
*	�����жϽ������ݷ���INBUF,�����򽫶�ȡ���Ľ�����ͻ�PC��
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����,��ȡ���ǲ�����ָ��,ֱ�ӽ���ȡ����ش�PC
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iCutResult;
	
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������	
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	ConfigUsart();				//2.�첽��������
		
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	
	while (1)
	{	//���м�鰴���ȹ���
		ProcessComInBuf();			//������ջ�����		
	}
}
#endif	/* INBUF���ռ����Ͳ��Գ������ */


#ifdef	DBG_SPI_ZIKU
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ����SPI����,�ֿ��ѯ��ʽ��ȡ
*	����ֿ��ȡ����,���յ�PC���͵��ַ����ݺ�,�����ַ�ָ��,������Ӧ���򴦶�ȡ�ֿ�,
*	����ȡ���Ľ�����ͻ�PC��
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����,����ָ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{	
	uint32_t	iAddress;	
	uint32_t	iCount;
	uint8_t	bBuf[BYTES_OF_ASCII_FONT_A];
	
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������	
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	CheckInterface();
	
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	#if	0
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	#endif
	//iAddress = ASCII_12_24_BASE_ADDR;
	iAddress = SYS_PARA_BASE_ADDR;
	while (1)
	{	//���м�鰴���ȹ���
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		#if	1
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LEDָʾ��
			sFLASH_ReadBuffer(bBuf, iAddress, BYTES_OF_ASCII_FONT_A);
			iAddress += BYTES_OF_ASCII_FONT_A;
			#if	0
			for (iCount = 0; iCount < BYTES_OF_ASCII_FONT_A; iCount++)
			{	
				DelayUs(1000);
				UpLoadData(&bBuf[iCount], 1);
				DelayUs(1000);
			}
			#else
			UpLoadData(bBuf, BYTES_OF_ASCII_FONT_A);
			#endif
		}
		else
		{
			PELedOff();						//LEDָʾ��
		}
		#endif
	}
}
#endif	/* �ֿ��ѯ��ʽ��ȡ���Գ������ */


#ifdef	DBG_SPI_ROM
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ����SPI�����ֿ�,��ѯ��ʽ����,д��,��ȡ
*	����ָ����ַ�Ŀ�,��������������ϴ�;д��ָ������,�ٶ���,������ȡ����ϴ���PC��
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iAddress;
	uint32_t	iCount;
	uint8_t	cGetData;
	uint8_t	bBuf[BYTES_OF_ASCII_FONT_A * 2];
	uint8_t *	cStr = "kill jiangzemin,kill hujintao,kill Japanese,kill American!";
	
	SystemInit();					//1.��ʼ��ʱ��
	
	GPIO_AFIODeInit();
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������	
	//InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	CheckInterface();
	
	ConfigUsart();				//2.�첽��������
		
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	CutPaper(0);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
		
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;
	for (iCount = 0; iCount < (strlen(cStr) + 1); iCount++)
	{
		sFLASH_ReadBuffer(&cGetData, iAddress, 1);	//��ȡ����ǰ�������ϴ�
		UpLoadData(&cGetData, 1);
		iAddress++;
	}
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
	
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;
	sFLASH_EraseSector(iAddress);		//��������4K
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
	
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;	
	sFLASH_ReadBuffer(bBuf, iAddress, (strlen(cStr) + 1));	//��ȡ������������ϴ�
	UpLoadData(bBuf, (strlen(cStr) + 1));
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
	
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;
	sFLASH_WriteBuffer(cStr, iAddress, (strlen(cStr) + 1));				//д��ָ������
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
	
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;
	for (iCount = 0; iCount < (strlen(cStr) + 1); iCount++)
	{
		sFLASH_ReadBuffer(&cGetData, iAddress, 1);	//��ȡд��ָ���������ϴ�
		UpLoadData(&cGetData, 1);
		iAddress++;
	}
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
	
	PELedOn();						//LEDָʾ��
	iAddress = SYS_PARA_BASE_ADDR;	
	sFLASH_ReadBuffer(bBuf, iAddress, (strlen(cStr) + 1));	//��ȡд��ָ���������ϴ�
	UpLoadData(bBuf, (strlen(cStr) + 1));
	DelayUs(1000*1000);
	PELedOff();						//LEDָʾ��
}
#endif	/* �ֿ��ѯ��ʽ����,д��,�������Գ������ */


#ifdef	DBG_SPI_HEAT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"

extern	uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];	//���Ȼ�оһ�������еļ��ȼĴ�����Ԫ��

/*******************************************************************************
* Function Name  : ������
* Description    : ���Ի�оSPIʹ��DMA��ʽ���о����һ���д�ӡ���ݵĳ���
*	�趨�û�оSPI����,��оSPI��DMA,׼����һ��������,���°�����,����LED,
*	ͬʱ����DMA����.��DMA���ͽ������ж��йر�LED,��ͨ����������λ������һ���д�ӡ����
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,��оSPI����,��оSPI_DMA����
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����,׼��һ��������,��ⰴ��������DMA����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	SystemInit();					//1.��ʼ��ʱ��	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	
	InitHeatElementBuf();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	//InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	sHeat_DMAInit();				//���û�оSPI��������MDA
	sHeat_Init();						//��ʼ����оSPI
	
	ConfigUsart();				//2.�첽��������
		
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	
	PELedOff();						//LEDָʾ��
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			PELedOn();						//LEDָʾ��
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);
			DelayUs(1000*1000);
		}
	}
}
#endif	/* ��оSPIʹ��DMA����һ���д�ӡ���ݲ��Գ������ */

#ifdef	DBG_HEAT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ�ӡ
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint16_t	HeatUs;
	uint8_t	cTemp[sizeof(uint16_t)];
	static uint8_t m_Count = 0;
	
	SystemInit();					//1.��ʼ��ʱ��	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_DMAInit();			//���û�оSPI��������MDA
	sHeat_Init();					//��ʼ����оSPI
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
		
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			HexMode();
			break;
		default:
			break;
	}
	
	DelayUs(1000*1000);		//�ȴ�ADC�����ȶ�
	while (1)
	{
		//ListenEthPort();						//��������������˿�
		//���м�鰴���ȹ���
		//ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			m_Count++;
			if ((m_Count % 2) == 0)
			{
				PELedOn();						//LEDָʾ��
				HeatUs = GetHeatTime();
				cTemp[0] = (HeatUs & 0xFF00) >> 8;
				cTemp[1] = (uint8_t)(HeatUs & 0x00FF);
				UpLoadData(cTemp, sizeof(uint16_t));			//�ϴ�����
				
				//StartHeatAll();
				StartHeat(HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN);
				DelayUs(1000*1000);
			}
			else
			{
				PELedOff();						//LEDָʾ��
				//StopHeatAll();
				EndHeat();
				DelayUs(1000*1000);
			}
		}
	}
}
#endif	/* ���Ȳ��Գ������ */


#ifdef	DBG_SPI_HEAT_DATA_SET
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];

/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ�ӡ
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;	
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.��ʼ��ʱ��
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_DMAInit();			//���û�оSPI��������MDA
	sHeat_Init();					//��ʼ����оSPI
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			HexMode();
			break;
		default:
			break;
	}
	
	//GoDotLine(2400);
	iCount = 0;
	cData = 0;
	while (1)
	{
		//ListenEthPort();						//��������������˿�
		//���м�鰴���ȹ���
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		#if	0
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)
			{
				g_tbHeatElementBuf[i] = cData;
			}
			cData++;
			
			for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)
			{
				cTemp = SPI_HeatSendByte(g_tbHeatElementBuf[i]);
			}
			while ((SPI_HEAT_SPI->SR & SPI_I2S_FLAG_TXE) == 0);		//ȷ���������
			
			DelayUs(1000*1000);
		}
		#endif
		#if	1
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			iCount++;
			if (iCount % 2)
			{
				PELedOn();						//LEDָʾ��
				StartHeat(m_tHeatCtrlPinTable[0]);
				//StartHeatPartOne();
				DelayUs(1000*1000);
				
				StopHeatPartOne();
				PELedOff();						//LEDָʾ��
				DelayUs(1000*1000);
			}
			else
			{
				PELedOn();						//LEDָʾ��
				StartHeat(m_tHeatCtrlPinTable[1]);
				//StartHeatPartTwo();
				DelayUs(1000*1000);
				
				StopHeatPartTwo();
				PELedOff();						//LEDָʾ��
				DelayUs(1000*1000);
			}
		}
		else
		{
			PELedOff();						//LEDָʾ��
		}
		#endif
	}
}
#endif	/* ��оSPI3ͨ�Ų��Գ������ */


#ifdef	DBG_LATCH
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];

/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ�ӡ���ƹܽ�
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;	
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.��ʼ��ʱ��	
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_DMAInit();			//���û�оSPI��������MDA
	sHeat_Init();					//��ʼ����оSPI
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	#if	0
	DriveCutPaper(CUT_TYPE_DEFAULT);	
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			HexMode();
			break;
		default:
			break;
	}
	
	iCount = 0;	
	while (1)
	{
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
			iCount++;
			if (iCount % 2)
			{
				PELedOn();						//LEDָʾ��
				GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
				DelayUs(1000*1000);
			}
			else
			{
				PELedOff();						//LEDָʾ��
				GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
				DelayUs(1000*1000);
			}
		}
	}
}
#endif	/* ��ӡ���ƹܽŲ��Գ������ */


#ifdef	DBG_PRT
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ�ӡ
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
	uint32_t	iCount;
	uint32_t	i;
	uint8_t	cData;
	uint16_t	cTemp;
	
	SystemInit();					//1.��ʼ��ʱ��
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_Init();					//��ʼ����оSPI
	sHeat_DMAInit();			//���û�оSPI��������MDA
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
	
	#ifdef	DBG_USB
	if (g_tInterface.USB_Type)	//��ʼ��USB�ӿ�
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();
		//USB_Disconnect_Config();	//��SetUSBPort()������ͬ
	}
	#endif
	
	InitNVIC();						//3.�����ж�
	ClearUsartBusy();			//��ʼ��ʱ����Ϊæ
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//����ΪԤ��Ƶ100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//����ΪԤ��Ƶ10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//����Ԥ��ƵΪ1ms
	#endif
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
	g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
	#endif
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	//FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	//FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			HexMode();
			break;
		default:
			break;
	}
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//GoDotLine(24);
		//FeedPaper(FEED_FORWARD, 24);	//��ֽ����,��Ϊ��������
		//DelayUs(1000*1000);
		
		//ListenEthPort();						//��������������˿�
		//���м�鰴���ȹ���
		if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
		{
		#ifdef	DBG_MOTOR_HOLD			
			if ((iCount % 3) == 0)
			{
				StartFeedMotor(FEED_FORWARD);	//�ظ����֮ǰ����λ,���ǲ��ı���λ����
				PELedOn();						//LEDָʾ��
			}
			else if ((iCount % 3) == 1)
			{
				StopFeedMotor(FEED_FORWARD);
				PELedOn();						//LEDָʾ��
			}
			else if ((iCount % 3) == 2)
			{
				EndFeedMotor();				//�رյ��
				PELedOff();						//LEDָʾ��
			}
			
			iCount++;
			DelayUs(1000*1000);
		#else
			GoDotLine(FEED_STEPS_AFTER_CLOSE_SHELL);
		#endif
		}
		
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		
		#if	0
		//ǰ���м�����ֽ���
		if ((g_tInBuf.BytesNumber == 0) && 
				(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && 
				(g_tSystemCtrl.StepNumber == 0))
		{
			EndHeat();										//�ؼ��ȿ���
			EndFeedMotor();								//�رյ��
			g_StepCount = 0;
			g_bMotorRunFlag = 0;
		}
		#endif
	}
}
#endif	/* ��ӡ���Գ������ */


#ifdef	DBG_USB
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : ������
* Description    : ����USB
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
		
	SystemInit();					//1.��ʼ��ʱ��
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_Init();					//��ʼ����оSPI
	sHeat_DMAInit();			//���û�оSPI��������MDA
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
	
	InitNVIC();						//3.�����ж�
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//����ΪԤ��Ƶ100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//����ΪԤ��Ƶ10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//����Ԥ��ƵΪ1ms
	#endif
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_USB
	if (g_tInterface.USB_Type)	//��ʼ��USB�ӿ�
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();
		//USB_Disconnect_Config();	//��SetUSBPort()������ͬ
	}	
	#endif
	
	ClearComInBufBusy();
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
	g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
	#endif
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			DelayUs(1000*500);
			HexMode();
			break;
		default:
			break;
	}
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//��������������˿�
		CheckFeedButton();								//���м�鰴���ȹ���
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		
		#if	0
		//ǰ���м�����ֽ���
		if ((g_tInBuf.BytesNumber == 0) && 
				(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && 
				(g_tSystemCtrl.StepNumber == 0))
		{
			EndHeat();										//�ؼ��ȿ���
			EndFeedMotor();								//�رյ��
			g_StepCount = 0;
			g_bMotorRunFlag = 0;
		}
		#endif
	}
}
#endif	/* USB���Գ������ */


#ifdef	DBG_ETH_USART
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : ������
* Description    : ����USB
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
		
	SystemInit();					//1.��ʼ��ʱ��
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();
	InitUsartPort();	
	InitHeatPort();
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_Init();					//��ʼ����оSPI
	sHeat_DMAInit();			//���û�оSPI��������MDA
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	ConfigUsart();				//2.�첽��������
	if (g_tInterface.USB_Type)	//��ʼ��USB�ӿ�
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();		
	}
	
	ClearComInBufBusy();
	
	InitNVIC();						//3.�����ж�
	
#ifdef WITHBOOT
//	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif
	
	#if defined DBG_DEAL_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 10000);	//����ΪԤ��Ƶ100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
	TimerConfigPrescaler(DBG_TIMER, 100000);	//����ΪԤ��Ƶ10us
	#elif	defined	DBG_PRT_SPEED
	TimerConfigPrescaler(DBG_TIMER, 1000);	//����Ԥ��ƵΪ1ms
	#endif
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
	g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
	#endif
		
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			DelayUs(1000*500);
			HexMode();
			break;
		default:
			break;
	}
	
	#if	0
	//DriveCutPaper(CUT_TYPE_DEFAULT);
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	#endif
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//��������������˿�
		//CheckFeedButton();								//���м�鰴���ȹ���
		//ProcessCharMaskReadBuf();			//�����ַ������ȡ������
		SetComInBufBusy();
	}
}
#endif	/* ETH_USART���Գ������ */


#ifdef	DBG_UPDATE
#include	<string.h>
#include	"timconfig.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"button.h"
#include	"usart.h"
#include	"ad.h"
#include	"feedpaper.h"
#include	"platen.h"

#include	"cutter.h"
#include	"spiflash.h"
#include	"checkif.h"
#include	"cominbuf.h"
#include	"charmaskbuf.h"
#include	"print.h"
#include	"heat.h"
#include	"usb_hwconfig.h"
#include	"usb_init.h"
#include	"usb_prop.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;

/*******************************************************************************
* Function Name  : ������
* Description    : ����USB
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
int main(void)
{
#ifdef	DBG_MOTOR_HOLD
	uint32_t	iCount;
#endif
	
	SystemInit();					//1.��ʼ��ʱ��
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	
	PELedPortInit();			//2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,CUT�˿�
	ErrorLedPortInit();
	InitFeedButtonPort();
	//InitResetEthernetButtonPort();
	InitPowerCtrlPort();
	InitFeedPort();
	InitPlatenPort();
	InitCutPort();
	InitCheckInterfacePort();	
	InitHeatPort();
	
#ifdef WITHBOOT
//	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif
	
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	InitHeatElementBuf();
	InitHeatCtrlStruct();
	
	sFLASH_Init();				//ϵͳ��������洢���ⲿ�ֿ�,������ȳ�ʼ��,�ٳ�ʼ������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	sHeat_Init();					//��ʼ����оSPI
	sHeat_DMAInit();			//���û�оSPI��������MDA
	
	CheckInterface();			//Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	
	if (g_tInterface.COM_Type)	//��ʼ�����ڽӿ�
	{
		InitUsartPort();
		ConfigUsart();				//2.�첽��������
	}
	
	if (g_tInterface.USB_Type)	//��ʼ��USB�ӿ�
	{
		SetUSBPort();
	 	Set_USBClock();
		USB_Init();		
	}
	
	InitNVIC();						//3.�����ж�
	
	#if defined DBG_DEAL_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 10000);	//����ΪԤ��Ƶ100us
	#elif	defined DBG_COPY_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 100000);	//����ΪԤ��Ƶ10us
	#elif	defined	DBG_PRT_SPEED
		TimerConfigPrescaler(DBG_TIMER, 1000);	//����Ԥ��ƵΪ1ms
	#elif	defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		TimerConfigPrescaler(DBG_TIMER, 100000);	//����ΪԤ��Ƶ10us
	#endif
	
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//����T4Ϊϵͳʱ��,1uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ�������д�ӡ������
	TimerConfigPrescaler(PRINT_CHECK_TIMER, 1000000);
	TimerConfigPrescaler(BACKUP_TIMER, 1000000);	//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
	
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	StartTimer(ROUTINE_TIMER);
	
	#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
		TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//����1uS,ͨ�Ŷ�ʱ
		g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
	#endif
	
	switch (GetModeType())				//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					//����ģʽ
			break;
		case	MODE_SELFTEST_PRT:				//�Լ�ģʽ
			DelayUs(1000*500);
			SystemSelfTest();
			break;
		case	MODE_RESET_ETHERNET:	//��λ��̫��ģʽ
			//ResetEthernet();
			break;
		case	MODE_HEX_PRT:							//16����ģʽ
			DelayUs(1000*500);
			ClearComInBufBusy();
			HexMode();
			break;
		case	MODE_NORMAL_UPDATE:
			ClearComInBufBusy();
			Update();
			break;
		case	MODE_RELEASE_JTAG:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);	//SWJ,JTAGȫ������
			break;
		default:
			break;
	}
	
	ClearComInBufBusy();
	
	#if	1
	FeedPaper(FEED_BACKWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_ANTI_BACKLASH);	//��ֽ����,��Ϊ��������
	FeedPaper(FEED_FORWARD, FEED_STEPS_AFTER_CLOSE_SHELL);	//��ֽ
	DriveCutPaper(CUT_TYPE_DEFAULT);
	#endif
	
	#ifdef	DBG_MOTOR_HOLD
	iCount = 0;
	#endif
	while (1)
	{
		//��������������˿�
		CheckFeedButton();								//���м�鰴���ȹ���
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
	}
}
#endif	/* DBG_UPDATE���Գ������ */



#ifdef	DBG_EXT_SRAM

#include	"init.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);

extern	void LongToStr(uint32_t num, uint8_t *Str);

/*******************************************************************************
* Function Name  : ������
* Description    : �����ⲿSRAM
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
int main(void)
{
	#ifdef	EXT_SRAM_ENABLE
		uint8_t		bBuf[16];
		uint32_t	Result;
		uint32_t	iLen;
	#endif
	
	Init();		//��ʼ��ʱ��,GPIO�˿�,ȫ�ֱ���,�ⲿ�豸
	
#ifdef WITHBOOT
//	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//����1uS,ͨ�Ŷ�ʱ
	g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	
	#ifdef	EXT_SRAM_ENABLE
		while (1)
		{
			if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
			{
				DelayMs(100);
				
				Result = TestReadWriteExtSRAM();
				if (Result == 0)
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is OK!\n");
				}
				else if (Result == 1)
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is Error!\n");
				}
				else
				{
					UsartSendStr(USART1, "Read and Write extern SRAM is Unknow Error!\n");
				}
				
				if (Result == 0)
				{
					iLen = GetExtSRAMLen();
					LongToStr(iLen, bBuf);
					UsartSendStr(USART1, "The extern SRAM length is bytes of ");
					UsartSendStr(USART1, bBuf);
					UsartSendStr(USART1, "\n");
				}
				
				DelayMs(100);
			}
		}
	#endif
	
	while (1)
	{
		CheckFeedButton();				//���м�鰴���ȹ���
		#ifdef	MAINTAIN_ENABLE
			CheckMaintainRoutin();	//���������ά�������洢
		#endif
		ProcessCharMaskReadBuf();	//�����ַ������ȡ������
	}
}

#endif	/* �ⲿSRAM���Գ������ */

#ifdef	DBG_CPCL_TEXT

#include	"init.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);

/*******************************************************************************
* Function Name  : ������
* Description    : ������ʽ�����汾
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
int main(void)
{
	Init();		//��ʼ��ʱ��,GPIO�˿�,ȫ�ֱ���,�ⲿ�豸
	
#ifdef BOOTLOAD
//	ConfigOptionFlash();	//����BOOTLOAD����Ϸ���
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT		//�����ٴ�
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//����1uS,ͨ�Ŷ�ʱ
	g_bRxFinishFlag = RX_FINISHED;		//ͨ�Ź����Ѿ����
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	EnableReceiveInterrupt();
	
	while (1)
	{
		if (CMD_SYS_ESCPOS == GetCommandSystem())		//����ESCָ�
		{
			CheckFeedButton();				//���м�鰴���ȹ���
			#ifdef	MAINTAIN_ENABLE
				CheckMaintainRoutin();	//���������ά�������洢
			#endif
			ProcessCharMaskReadBuf();	//�����ַ������ȡ������
		}
		#ifdef	CPCL_CMD_ENABLE
			else if (CMD_SYS_CPCL == GetCommandSystem())		//����CPCLָ�
			{
				//���м�鰴���ȹ���
				CPCL_Command();		//����CPCLָ��
			}
		#endif
		else	//��������ָ�
		{
		}
	}	//��ѭ������
	
}

#endif	/* DBG_CPCL_TEXT ���Գ������ */

#ifdef	DBG_ESC_RELEASE

#include	"init.h"
#include "speaker.h"

extern	uc16	m_tHeatCtrlPinTable[];
extern	uint8_t	g_tbHeatElementBuf[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	void	ProcessModeType(void);
extern uint8_t Eth_ClearBusy;

/*******************************************************************************
* Function Name  : ������
* Description    : ������ʽ�����汾
*	
*	��Ҫ�Ĳ�������:
*	1.��ʼ��ʱ��,����USBʱ��
*	2.��ʼ��LED�˿�,�����˿�,��ֽ���ƶ˿�,USB�˿�,����ȫ�ֱ���,���ƻ���ȫ�ֱ���,
*		����4,ADC,Feed�˿�,�е��˿�,�ֿ�SPI�˿�,�ֿ�SPI����,���ȶ˿�,����ȫ�ֱ���,
*	3.�����ж�
*	4.�����жϽ��յ����ݷŵ����ջ�����
*	5.����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
int main(void)
{
	Sys_Init();								//��ʼ��ʱ��,GPIO�˿�,ȫ�ֱ���,�ⲿ�豸
	
#ifdef WITHBOOT
	ConfigOptionByte();   //2017.10.18  оƬ��д����
 	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT										//�����ٴ�
	TimerConfigPrescaler(RX_FLAG_TIM, 1000000);		//����1uS,ͨ�Ŷ�ʱ
	g_bRxFinishFlag = RX_FINISHED;								//ͨ�Ź����Ѿ����
#endif
	
	ProcessModeType();
	Eth_ClearBusy =1;	
	
	ClearComInBufBusy();
	Eth_ClearBusy =0;		
	while (1)
	{
		CheckFeedButton();				//���м�鰴���ȹ���
		#ifdef	MAINTAIN_ENABLE
// 	CheckMaintainRoutin();	  //���������ά�������洢
		#endif
		ProcessCharMaskReadBuf();	//�����ַ������ȡ������
	}
}

#endif	/* DBG_ESC_RELEASE ���Գ������ */



#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
