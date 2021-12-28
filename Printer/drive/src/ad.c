/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.2
  * @date    2012-5-29
  * @brief   ADת����صĳ���.
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
#include	<string.h>
#include	"ad.h"
#include	"temperature.h"
#include	"blackmark.h"
#include	"pe.h"
#include	"pne.h"
#include	"power.h"
#include	"ptake.h"
#include	"extgvar.h"
#include	"debug.h"
#include	"led.h"
#include	"maintain.h"

extern	uint8_t	g_bWriteFlashFlag;					//SPIFLASH��־
extern	volatile uint8_t	g_bRoutinFlag;		//T2�жϱ�־

extern	void	DelayUs(uint32_t	delaytime);
extern	void	SetEmergencyProtect(void);
extern	uint32_t	GetFeedPaperStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address 	(ADC1_BASE + 0x4C)

#ifdef	DBG_ADC_POWER
#define	POWER_ADC_COUNT		(1)	//����ʱ��Ҫ֪���任����
#else
#define	POWER_ADC_COUNT		(8)	//����㹻�������ȡƽ��
#endif

#define	ADC_COUNT					(8)	//����㹻�������ȡƽ��

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TypeDef_StructAD	g_tADC;			//����ADת�������Ľṹ

/* Private function prototypes -----------------------------------------------*/
static void UpdateErrorStatusByADCResult(void);
static void	UpdatePaperStatusByADCResult(void);
static void	UpdateBlackMarkStatusByADCResult(void);
static void	UpdatePaperNearEndStatusByADCResult(void);
static void	UpdateTemperatureStatusByADCResult(void);
static void	UpdatePowerStatusByADCResult(void);
static void	UpdatePaperAndBlackMarkStatusByADCResult(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : �������е�ADC�˿ڵĳ�ʼ��
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
void	InitADCPort(void)
{
	//InitBlackMarkPort();
	InitPaperEndPort();
	//InitPaperNearEndPort();
	InitPowerInPort();
	InitTemperaturePort();
	InitPaperTakeAwayPort();
}

/*************************************************
����: void	OpenADC(void)
����: ��DMA�ж��йص�ADCת���������������
����: ��
����: ��
**************************************************/
#define	ADC_INTERVAL_MS		(1)
void	OpenADC(void)
{
	ADC_Cmd(ADC_SOURCE, ENABLE);		//��Ҫ���β��ܿ�ʼADת��
	ADC_Cmd(ADC_SOURCE, ENABLE);		//��Ҫ���β��ܿ�ʼADת��
}

/*************************************************
����: void	ADC_Config(void)
����: ADC��������
����: ��
����: ��
**************************************************/
void	ADC_Config(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	
	/* ADCCLK = PCLK2/6 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;				//��������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;							//ɨ�跽ʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;				//����ת��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�ⲿ������ֹ
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		//�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUMBER;	//����ת����ͨ����
  ADC_Init(ADC_SOURCE, &ADC_InitStructure);
	
	#define		ADC_SAMPLE_TIME		ADC_SampleTime_28Cycles5

	/* ADC_SOURCE regular channels configuration [����ģʽͨ������]*/ 
  ADC_RegularChannelConfig(ADC_SOURCE, POWER_ADC_CHANNEL, 3, ADC_SAMPLE_TIME); 				// V24V_IN  POWER_ADC_CHANNEL  2016.04.04
	ADC_RegularChannelConfig(ADC_SOURCE, TEMPERATURE_ADC_CHANNEL, 1, ADC_SAMPLE_TIME);	//�¶�
  ADC_RegularChannelConfig(ADC_SOURCE, PE_ADC_CHANNEL, 2, ADC_SAMPLE_TIME); 				  //ȱֽ  PE_ADC_CHANNEL  Ҳ���ںڱ�
	ADC_RegularChannelConfig(ADC_SOURCE, PTAKE_ADC_CHANNEL, 4, ADC_SAMPLE_TIME);  			// ֽ���Ƿ�ȡ��
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
	ADC_RegularChannelConfig(ADC_SOURCE, PE_ADC_CHANNEL, 5, ADC_SAMPLE_TIME); 					//�ڱ� �ڱ��ȡֽ���ô�����ʱ
	//#else
	//ADC_RegularChannelConfig(ADC_SOURCE, BLACKMARK_ADC_CHANNEL, 5, ADC_SAMPLE_TIME); 		//�ڱ� 
	#endif
	//ADC_RegularChannelConfig(ADC_SOURCE, PNE_ADC_CHANNEL, 6, ADC_SAMPLE_TIME); 					//ֽ���� PNE_ADC_CHANNEL 
	
  /* Enable ADC_SOURCE DMA [ʹ��ADC_SOURCE DMA]*/
  ADC_DMACmd(ADC_SOURCE, ENABLE);
  
  /* Enable ADC_SOURCE [ʹ��ADC_SOURCE]*/
  ADC_Cmd(ADC_SOURCE, ENABLE);  

  /* Enable ADC_SOURCE reset calibaration register */   
  ADC_ResetCalibration(ADC_SOURCE);
  /* Check the end of ADC_SOURCE reset calibration register */
  while (ADC_GetResetCalibrationStatus(ADC_SOURCE));

  /* Start ADC_SOURCE calibaration */
  ADC_StartCalibration(ADC_SOURCE);
  /* Check the end of ADC_SOURCE calibration */
	while (ADC_GetCalibrationStatus(ADC_SOURCE));
	
	/* Start ADC_SOURCE Software Conversion */
	ADC_SoftwareStartConvCmd(ADC_SOURCE, ENABLE);
}

/*************************************************
����: void	ADC_DMA_Configuration(void)
����: DMA��������
����: ��
����: ��
**************************************************/
void	ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;	//����DMA��ʼ���ṹ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	#ifdef	STM32F10X_CL
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	#endif
	
	/* DMA1 channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			//�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_tADC.ADBuffer;		//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//����ΪԴ
  DMA_InitStructure.DMA_BufferSize = ADC_BUF_LEN;		//��������С,����DMA�ڴ���ʱ�������ĳ���word
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����DMA���������ģʽ��һ������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				//����DMA���ڴ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//���������ֳ�
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//�ڴ������ֳ�
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;			//����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//����DMA�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;				//����DMA��2��memory�еı����������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
  /* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);			//������ɲ����ж�
}

/*******************************************************************************
* Function Name  : void	DMA1_Channel1_ADFunc
* Description    : ����ADC��DMA�жϹ��ܣ����ڼ���ADCת��ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	DMA1_Channel1_ADFunc(void)
{
	static uint8_t ADCount = 0;		//ADC����ͳ��
	
	#ifdef	BLACKMARK_ADC_USE_PE_ADC	
	static uint16_t m_tu16Buf[ADC_CHANNEL_NUMBER] = {0, 0, 0, 0, 0};
	#else	
  static uint16_t m_tu16Buf[ADC_CHANNEL_NUMBER] = {0, 0, 0, 0};
  #endif
	uint16_t	tu16TempBuf[ADC_CHANNEL_NUMBER];
	uint32_t	i;
	uint32_t	j;
	
	volatile uint16_t	* ptu16Buf = NULL;
	uint16_t * ptu16TempBuf = NULL;
	
	DMA_ClearITPendingBit(DMA1_IT_TC1);		//���DMA��������ж�λ
	ADC_Cmd(ADC_SOURCE, DISABLE); 				//��ADģ��
	ADCount++;								//�����������ɴ�ƽ��ֵ
	g_bRoutinFlag = 0;				//T2�жϱ�־������ʱʹ��
	
	//��������ʱ��������
	ptu16TempBuf = tu16TempBuf;				//��ʱ�Ӻ�
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
		*ptu16TempBuf = 0x00;
		ptu16TempBuf++;
	}
	//�ټ��㱾�β�����ֵ
	ptu16Buf = g_tADC.ADBuffer;	//ADCת�����
	for (i = 0; i < ADC_CONVERT_NUMBER; i++)
	{
		ptu16TempBuf = tu16TempBuf;			//��ʱ�Ӻ�
		for (j = 0; j < ADC_CHANNEL_NUMBER; j++)
		{
			*ptu16TempBuf += *ptu16Buf;
			ptu16TempBuf++;
			ptu16Buf++;
		}
	}
	//Ȼ�������ֵ����ȡ������ȡƽ��,��������ƽ����Ľ����ŵ���̬�洢����
	ptu16Buf = m_tu16Buf;							//ADCת�������̬�洢��
	ptu16TempBuf = tu16TempBuf;				//��ʱ�Ӻ�
	for (i = 0; i < ADC_CHANNEL_NUMBER; i++)
	{
		*ptu16TempBuf >>= ADC_CONVERT_SHIFT_NUMBER;	//ȡƽ��
		*ptu16Buf += *ptu16TempBuf;	//�½����Ӵ洢����̬�洢����
		ptu16Buf++;
		ptu16TempBuf++;
	}
	
	if (ADCount >= ADC_COUNT)			//�ܴ�����,��ƽ��ֵ
	{
		ptu16Buf = m_tu16Buf;				//ADCת�������̬�洢��
		for (i = 0; i < ADC_CHANNEL_NUMBER; i++)	//ȡƽ��
		{
			*ptu16Buf /= ADCount;
			ptu16Buf++;
		}
		ptu16Buf = m_tu16Buf;				//ADCת�������̬�洢��,����ADC����
		g_tADC.Temperature_ADC_Value		= *ptu16Buf++;
		g_tADC.PaperEnd_ADC_Value				= *ptu16Buf++;
		g_tADC.Power_ADC_Value				  = *ptu16Buf++;				//2016.04  ���ݵ�·���� 
		g_tADC.PaperTakeAway_ADC_Value	= *ptu16Buf++; 
		#ifdef	BLACKMARK_ADC_USE_PE_ADC	
		g_tADC.BlackMark_ADC_Value			= *ptu16Buf++;
		#endif
		//g_tADC.PaperNearEnd_ADC_Value 	= *ptu16Buf++;
		
		ptu16Buf = m_tu16Buf;				//ADCת�������̬�洢��
		for (i = 0; i < ADC_CHANNEL_NUMBER; i++)	//��������
		{
			*ptu16Buf = 0x00;
			ptu16Buf++;
		}
		ADCount	= 0;	//ת����������
		
		#ifdef	ERROR_STATUS_ADC_UPDATE_ENABLE
			UpdateErrorStatusByADCResult();	//����ADC�������ֵ������������״̬�������
		#endif
	}
	
	ADC_DMA_Config();		//��������һ��
}

/* ����Ϊ��ADC�ķ�װ,���ظ���������ֵ */
uint16_t	GetADCValueOfPaperEnd(void)
{
	return	(g_tADC.PaperEnd_ADC_Value);
}

uint16_t	GetADCValueOfTemperature(void)
{
	return	(g_tADC.Temperature_ADC_Value);
}

uint16_t	GetADCValueOfPower(void)
{
	return	(g_tADC.Power_ADC_Value);
}

uint16_t	GetADCValueOfPaperNearEnd(void)
{
	return	(g_tADC.PaperNearEnd_ADC_Value);
}

uint16_t	GetADCValueOfBlackMark(void)
{
	return	(g_tADC.BlackMark_ADC_Value);
}

uint16_t	GetADCValueOfPaperTakeAway(void)
{
	return	(g_tADC.PaperTakeAway_ADC_Value);
}

const uint16_t	g_tPaperendADValueTable[] =
{
	(uint16_t)PE_ADC_VALUE_LOW, 
	(uint16_t)PE_ADC_VALUE_MIDDLE, 
	(uint16_t)PE_ADC_VALUE_HIGH
};	//��ֽ������������ֵ

/* ����Ϊ���غ��� */
static void	UpdatePaperStatusByADCResult(void)
{
	//�жϴ�ӡͷ�Ƿ�ȱֽ��ڱ�λ�ã���ֽ���ڱ꣩ʱ��ѹ�ߣ���ֽʱ��ѹ��
	if (g_tADC.PaperEnd_ADC_Value > g_tPaperendADValueTable[g_tSysConfig.PaperSensorSensitive])
	{		
		if (g_tSysConfig.BlackMarkEnable)	//�ڱ�����ʱ�ż��
		{
			g_tSystemCtrl.MarkPositionFlag = 0x01;	//�ڱ�λ��
		}	
		else
		{
			g_tError.PaperSensorStatusFlag |= 0x01;	  //bit.0 = 1 ȱֽ  �Ǻڱ�ģʽ�²Ÿ�ֵȱֽ��־
		}	
	}
	else
	{
		if (g_tSysConfig.BlackMarkEnable)	//�ڱ�����ʱ�ż��
		{
			g_tSystemCtrl.MarkPositionFlag = 0x00;	  //�ڱ�λ�����
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ȱֽ  2016.11.17  �ڱ�ģʽ�� ��ֽ����ȱֽ��־
		}	
		else
		{	
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ȱֽ
		}
	}
}

static void	UpdateBlackMarkStatusByADCResult(void)
{
	const uint16_t	g_tBlackmarkADValueTable[] =
	{
		(uint16_t)BLACKMARK_ADC_VALUE_LOW, 
		(uint16_t)BLACKMARK_ADC_VALUE_MIDDLE, 
		(uint16_t)BLACKMARK_ADC_VALUE_HIGH
	};	//��ڱ괫����������ֵ
	
	//�жϴ�ӡͷ�Ƿ�ڱ�λ�ã��ڱ�ʱ��ѹ�ߣ��Ǻڱ�ʱ��ѹ��
	if (g_tADC.BlackMark_ADC_Value > g_tBlackmarkADValueTable[g_tSysConfig.BlackMarkSensitive])
	{
		g_tSystemCtrl.MarkPositionFlag = 0x01;	//�ڱ�λ��
	}
	else
	{
		g_tSystemCtrl.MarkPositionFlag = 0x00;	//�Ǻڱ�λ��
	}
}

/*******************************************************************************
* Function Name  : void	UpdatePaperStatusByADCResult()
* Description    : ����ADC�������ֵ����ȱֽ����״̬�������. 
	����ȱֽ�������ͺڱ괫�����Ƿ������Ϊ�������.
	����ʱ���Բ���ȱֽ�ͺڱ�, ����ʱ�����Ƿ�ڱ�ģʽ����ȱֽ�ͺڱ���. ��ʱ�ڱ���
	�޷��ж��Ƿ�ȱֽ, ֻ����������ʽȷ���Ƿ�ȱֽ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	UpdatePaperAndBlackMarkStatusByADCResult(void)
{
	//��ֽ�Ͳ�ڱ깲��ͬһ��������
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
		if (g_tSysConfig.BlackMarkEnable)	//�ڱ�����ʱֻ���ڱ�, ������ȱֽ, Ҳ�޴��ж��Ƿ�ȱֽ
		{
			UpdateBlackMarkStatusByADCResult();
			//�жϴ�ӡͷ�Ƿ�ȱֽ��ڱ�λ�ã���ֽ���ڱ꣩ʱ��ѹ�ߣ���ֽʱ��ѹ��
			if (g_tADC.PaperEnd_ADC_Value > g_tPaperendADValueTable[g_tSysConfig.PaperSensorSensitive])
			{
			}
			else
			{
				g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ȱֽ
			}
		}
		else	//�Ǻڱ�ģʽʱֻ��ֽ, ������ڱ�
		{
			UpdatePaperStatusByADCResult();
		}
	//����Ϊ��ֽ�Ͳ�ڱ��໥����
	#else
		UpdatePaperStatusByADCResult();		//�жϴ�ӡͷ�Ƿ�ȱֽ����ֽʱ��ѹ�ߣ���ֽʱ��ѹ��
// 		if (g_tSysConfig.BlackMarkEnable)	//�ڱ�����ʱ�ż�� T801û�е����ͺڱ괫�������Ͳ��Դ���������;
// 		{
// 			UpdateBlackMarkStatusByADCResult();
// 		}
	#endif
}

static void	UpdatePaperNearEndStatusByADCResult(void)
{
    static uint32_t PNEDelayNum = 0;       //��ʱ�ɼ�ֽ�����ź� 
	#ifdef	PNE_ENABLE
		const uint16_t	g_tPneADValueTable[] =
		{
			(uint16_t)PNE_ADC_VALUE_LOW, 
			(uint16_t)PNE_ADC_VALUE_MIDDLE, 
			(uint16_t)PNE_ADC_VALUE_HIGH
		};	//ֽ����������������ֵ
	#else
		const uint16_t	g_tPneADValueTable[] =
		{
			(uint16_t)PE_ADC_VALUE_LOW, 
			(uint16_t)PE_ADC_VALUE_MIDDLE, 
			(uint16_t)PE_ADC_VALUE_HIGH
		};	//��ֽ������������ֵ
	#endif
	
	#ifdef	PNE_ENABLE
		//ֻ�е���ӡ����ֹʱ�Ÿ���ֽ�����ź�,���򲻴���
		if (PRINTER_STATUS_ILDE == GetFeedPaperStatus())
		{
            
			if ((g_tADC.PaperNearEnd_ADC_Value > g_tPneADValueTable[g_tSysConfig.PaperSensorSensitive]) && (g_tSysConfig.PaperNearEndSensorEnable ==1) ) // ����ֽ�������� �� ����ֽ��������
			{
				PNEDelayNum++;
				if(PNEDelayNum > 200)   //2s�����б��ֵ  ʱ���д�����
				{
						g_tError.PaperSensorStatusFlag |= 0x02;		//bit.1 = 1 ֽ����
						PNEDelayNum = 200;
				}   
				else
				{
						g_tError.PaperSensorStatusFlag &= ~0x02;    //���־
				}    
			}
			else
			{
                PNEDelayNum = 0;
				g_tError.PaperSensorStatusFlag &= ~0x02;    //���־
			}
		}
	#endif
}

static void	UpdateTemperatureStatusByADCResult(void)
{
	//----ͨ�����µ�·���жϴ�ӡͷ�Ƿ�������,�¶ȼ���������ֵ,�¶ȼ���������ֵ
	if ((g_tADC.Temperature_ADC_Value > TEMPERATURE_ERROR_ADC_LOW) || \
			(g_tADC.Temperature_ADC_Value < TEMPERATURE_ERROR_ADC_HIGH))
	{
		g_tError.UR_ErrorFlag |= 0x80;	   		//bit7 �¶ȼ���·����
	}
	else
	{
		g_tError.UR_ErrorFlag &= (uint8_t)(~0x80);
	}
	
	//----�жϴ�ӡͷ�Ƿ����----//
	//��ӡͷ������ѹԽ�ͣ���ӡͷ�¶�Խ��
	if (g_tADC.Temperature_ADC_Value < TEMPERATURE_WARN_ADC_HIGH)
	{
		g_tError.AR_ErrorFlag |= 0x01;	   		//bit.0 = 1 ��ӡͷ����
	}
	else if (g_tADC.Temperature_ADC_Value > TEMPERATURE_WARN_ADC_LOW)
	{
		g_tError.AR_ErrorFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ӡͷ����
	}
}

static void	UpdatePowerStatusByADCResult(void)
{
	//----�ж������Դ��ѹ�Ƿ�����������ѹ�����ѹ
	if (g_tADC.Power_ADC_Value < POWER_VALUE_LOW)	//Ƿ��ѹ
	{	//��ֹ����дʱ���ظ�д
// 		if (((g_tError.UR_ErrorFlag & 0x04) == 0) && (WriteFlashFlag == 0))  //2017.04.24
// 		{
// 			#ifdef	MAINTAIN_CMD_ENABLE
// 			  WriteFlashParameter(1);							//дCPU_FLASHά��������
// 			#endif
// 			g_tError.UR_ErrorFlag |= 0x04;			//bit3Ƿ��ѹ
// 		}
	}
	else
	{
		if (((g_tError.UR_ErrorFlag & 0x04) == 0x04) && (WriteFlashFlag == 0))	//��ֹ����дʱ���ظ�д
		{
			#ifdef	MAINTAIN_CMD_ENABLE
				ReadFlashParameter();								//��ά������������ֹд��ʱ����ҳ
			#endif
			g_tError.UR_ErrorFlag &= (uint8_t)(~0x04);
		}
	}
	
	if (g_tADC.Power_ADC_Value > POWER_VALUE_HIGH)	//����ѹ
	{
		g_tError.UR_ErrorFlag |= 0x02;	   		//bit2 ����ѹ
	}
	else
	{
		g_tError.UR_ErrorFlag &= (uint8_t)(~0x02);
	}
	
	if (g_tADC.Power_ADC_Value > POWER_VALUE_CLOSE)	//�رմ�ӡ��ѹ
	{
		SetEmergencyProtect();
	}
}

static void UpdatePaperTakeAwayStatusByADCResult(void)
{
	const uint16_t	m_tPaperTakeAwayADValueTable[] =
	{
		(uint16_t)PTAKE_ADC_VALUE_LOW, 
		(uint16_t)PTAKE_ADC_VALUE_MIDDLE, 
		(uint16_t)PTAKE_ADC_VALUE_HIGH
	};	//ֽ��ȡ�ߴ�����������ֵ
	
	//�ж�ֽ���Ƿ��Ѿ�ȡ��,�ߵ�ƽʱΪȱֽ����ʾֽ�ű�ȡ�ߣ��͵�ƽΪ��ֽ����ʾֽ��δȡ��
	if (g_tADC.PaperTakeAway_ADC_Value > m_tPaperTakeAwayADValueTable[g_tSysConfig.PaperTakeAwaySensorSensitive])
	{
		if(g_tSysConfig.PaperTakeAwaySensorEnable ==1)         //���Ӵ�ȡֽ���ܿ���
		{	
			g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x04);	 //bit.2 = 0 ֽ�ű�ȡ��
		}	
	}
	else 
	{
		if(g_tSysConfig.PaperTakeAwaySensorEnable ==1)         //���Ӵ�ȡֽ���ܿ���
		{	
			g_tError.PaperSensorStatusFlag |= 0x04;							 //bit.2 = 1 ֽ��δ��ȡ��
		}	
	}
}

/*******************************************************************************
* Function Name  : void	UpdateErrorStatusByADCResult()
* Description    : ����ADC�������ֵ������������״̬�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void UpdateErrorStatusByADCResult(void)
{
  if(g_tError.DoorOpen == 0)                    //�ϸǴ� ������ֽ״̬
  {    
		UpdatePaperAndBlackMarkStatusByADCResult();
		UpdatePaperTakeAwayStatusByADCResult();
  }    
	UpdatePaperNearEndStatusByADCResult();
	UpdateTemperatureStatusByADCResult();
	UpdatePowerStatusByADCResult();
    
	AutoGoPaper();      //�Զ���ֽ����״̬
	
}
/*******************************************************************************
* Function Name  : void	AutoGoPaper()
* Descripti on   : �Զ���ֽ����״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void AutoGoPaper(void)
{
	static uint16_t CounterNum = 0;		 
 				  		
	if((g_tError.PaperSensorStatusFlag & 0x01) != 0)    //ȱֽ״̬��
	{
		AutoFeedPaper = 1;                              //ȱֽ״̬Ϊ1  ȱֽ����ֽΪ2  ��ֽ��ɺ�ֵΪ0
    CounterNum = 0;
	}	
  else
  {    
        if(AutoFeedPaper == 1)
        {
            if((g_tError.PaperSensorStatusFlag & 0x01) == 0) //��ֽ״̬��
            {
                CounterNum++;
                if(CounterNum > 10)                            //����
                {
                    CounterNum = 0;
                    AutoFeedPaper = 2;					 
                }
            } 			
        }	
    }   
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
