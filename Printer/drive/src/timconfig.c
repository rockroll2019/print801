/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-6-4
  * @brief   ʱ��������صĳ���.
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
#include	"timconfig.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : 
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	IncreaseDelayMs(void)
{
	volatile static uint32_t DelayMsCount = 0;
	
	DelayMsCount++;
}

void	DelayUs(uint32_t	delaytime)
{
	uint32_t	i;
	volatile uint8_t	j;
	
	if (delaytime > 0)
	{
		for (i = 0; i < delaytime; i++)
		{
			for (j = 0; j < 9; j++);	//��Ҫ��ȷ���� J ����ֵ,72M��j=9; 48M��j=6
		}
	}
}

void	DelayMs(uint32_t DelayNumber)
{
	volatile uint32_t	i;		//2016.06.04
	volatile uint8_t	j;
	
	if (DelayNumber > 0)
	{
		for (i = 0; i < DelayNumber * 1000; i++)
		{
			for (j = 0; j < 9; j++);	//��Ҫ��ȷ���� J ����ֵ,72M��j=9; 48M��j=6
		}
	}
#if	0
	if (DelayNumber > 0)
	{
		DelayMsCount = 0;
		while (DelayMsCount < DelayNumber)
		{
			;
		}
	}
#endif
}

/*******************************************************************************
* Function Name  : void TimerConfigPrescaler(TIM_TypeDef* TIMx, uint32_t Prescaler)
* Description    : ��ʼ������ʱ����Ԥ��Ƶ����
* Input          : TIMx:��ʱ���ţ�Prescaler:Ԥ��Ƶ����,PCLK1�϶�ʱ��.
* Output         : None
* Return         : None
*******************************************************************************/
void TimerConfigPrescaler(TIM_TypeDef* TIMx, uint32_t Prescaler)
{
	TIM_TimeBaseInitTypeDef		TimeBaseInitStruct;
	RCC_ClocksTypeDef					RCC_ClocksStatus;
	uint32_t	TimerClockHz;
	
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	/*----STEP1:��λʹ�õ�TIMERx�豸----*/
	TIM_DeInit(TIMx);
	
	//ʹ��ʱ��
	if (TIMx == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if (TIMx == TIM3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else if (TIMx == TIM4)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else if (TIMx == TIM5)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	else if (TIMx == TIM6)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	else if (TIMx == TIM7)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	else if (TIMx == TIM12)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	else if (TIMx == TIM13)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	else if (TIMx == TIM14)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	if (TIMx == TIM1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	else if (TIMx == TIM8)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	else if (TIMx == TIM9)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
	else if (TIMx == TIM10)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	else if (TIMx == TIM11)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
	else if (TIMx == TIM15)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	else if (TIMx == TIM16)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	else if (TIMx == TIM17)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	
	/*----STEP2:����ʹ�õ�TIMERx�豸----*/
	RCC_GetClocksFreq(&RCC_ClocksStatus);			//��ȡϵͳʱ��
	
	if ((TIMx == TIM2) || (TIMx == TIM3)  || (TIMx == TIM4) || \
			(TIMx == TIM5) || (TIMx == TIM6)  || (TIMx == TIM7) || \
			(TIMx == TIM12) || (TIMx == TIM13)  || (TIMx == TIM14))
	{
		TimerClockHz = RCC_ClocksStatus.PCLK1_Frequency;
	}
	else if ((TIMx == TIM1) || (TIMx == TIM8) || \
			(TIMx == TIM9) || (TIMx == TIM10)  || (TIMx == TIM11) || \
			(TIMx == TIM15) || (TIMx == TIM16)  || (TIMx == TIM17))
	{
		TimerClockHz = RCC_ClocksStatus.PCLK2_Frequency;
	}
	
	if (RCC_ClocksStatus.HCLK_Frequency == TimerClockHz)
	{	 //Ԥ��Ƶֵ,	
		if (Prescaler < TimerClockHz)
		{
			TimeBaseInitStruct.TIM_Prescaler = TimerClockHz / Prescaler - 1;
		}	
	}
	else
	{	//Ԥ��Ƶֵ
		if (Prescaler < (TimerClockHz * 2))
		{
			TimeBaseInitStruct.TIM_Prescaler = (TimerClockHz * 2) / Prescaler - 1;
		}
	}
	
	TimeBaseInitStruct.TIM_ClockDivision	= TIM_CKD_DIV1;					//ʱ�ӷ�Ƶ����
	if ((TIMx != TIM6) || (TIMx != TIM7))
	{
		TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Down;	//����ģʽ����
	}
	else
	{
		TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up;		//����ģʽ����
	}
	TimeBaseInitStruct.TIM_Period					= 1;										//��װ��ֵ,AutoReload
	TimeBaseInitStruct.TIM_RepetitionCounter = 0; 								//T1��T8������
	
	TIM_TimeBaseInit(TIMx, &TimeBaseInitStruct);
	
	/*----STEP3:���ʹ�õ�TIMERx��־----*/
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	
	/*----STEP4:����ʹ�õ�TIMERx�ж�----*/
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	
	TIM_UpdateDisableConfig(TIMx, DISABLE);
	
	/*----STEP5:����ʹ�õ�TIMERx�豸----*/
	TIM_Cmd(TIMx, DISABLE);
}

/*******************************************************************************
* Function Name  : SetTimerCounterWithAutoReload(TIM_TypeDef* TIMx, uint16_t Counter)
* Description    : ���ö�ʱ���ļ���ֵ
* Input          : TIMx:��ʱ���ţ�Counter:����ֵ����װ��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void  SetTimerCounterWithAutoReload(TIM_TypeDef* TIMx, uint16_t Counter)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIMx->ARR = Counter;
	TIMx->CNT = Counter;
}

/*******************************************************************************
* Function Name  : StartTimer(TIM_TypeDef* TIMx)
* Description    : ������ʱ��
* Input          : TIMx:��ʱ����
* Output         : None
* Return         : None
*******************************************************************************/
void  StartTimer(TIM_TypeDef* TIMx)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/*******************************************************************************
* Function Name  : StopTimer(TIM_TypeDef* TIMx)
* Description    : �رն�ʱ��
* Input          : TIMx:��ʱ����
* Output         : None
* Return         : None
*******************************************************************************/
void  StopTimer(TIM_TypeDef* TIMx)
{
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, DISABLE);
}

#ifdef	DBG_PROCESS_TIME
volatile uint32_t	g_iDbgProcessTimeUs;
void	InitSystick(void)
{
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(72000000 / 1000))
	{ 
		/* Capture error */
		while (1);
	}
	SysTick_CounterCmd(SysTick_Counter_Disable);	//��ֹʹ��
	//����systickʱ��Դ,72/8=9MHZ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//������װ��ֵ,��ʱus, =9,1us; =90,10us; =900,100us; =9000,1ms;
	SysTick_SetReload(90);
	SysTick_CounterCmd(SysTick_Counter_Clear);		//���ö�ʱ����ֵ
	SysTick_ITConfig(ENABLE);
	g_iDbgProcessTimeUs = 0;
}

void	StopProcessTimeCounter(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable);	//��ֹʹ��
	SysTick_CounterCmd(SysTick_Counter_Clear);		//���ö�ʱ����ֵ
}
#endif

uint32_t	GetTimerEnableStatus(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
	
	if (TIM_CR1_CEN == (TIMx->CR1 & TIM_CR1_CEN))
		return (ENABLE);		//����
	else
		return (DISABLE);		//�ر�
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
