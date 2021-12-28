/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ���ȿ�����صĳ���.
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
#include <string.h>
#include	"heat.h"
#include	"print.h"
#include	"ad.h"
#include	"power.h"
#include	"temperature.h"
#include	"extgvar.h"
#include	"debug.h"

extern void	DelayUs(uint32_t	delaytime);

extern	uint8_t	g_tbHeatElementBuf[];	//���Ȼ�оһ�������еļ��ȼĴ�����Ԫ��
extern	TypeDef_StructAD	g_tADC;			//����ADת�������Ľṹ
extern uint32_t	g_iFeedMotorStartStep;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
TypeDef_StructHeatCtrl	g_tHeatCtrl;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if	MAX_HEAT_SUM == 6
#elif	MAX_HEAT_SUM == 3
#elif	MAX_HEAT_SUM == 2
uc16	m_tHeatCtrlPinTable[] = {HEAT_PDST1_GPIO_PIN, HEAT_PDST2_GPIO_PIN};
#elif	MAX_HEAT_SUM == 1
uc16	m_tHeatCtrlPinTable[] = {HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN};
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�����ȿ��ƹܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitHeatPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(HEAT_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= HEAT_PDST1_GPIO_PIN | \
																		HEAT_PDST2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;			//�ܽž�Ϊ���
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(HEAT_CTRL_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(HEAT_PLATCH_RCC_APB2Periph_GPIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin			= HEAT_PLATCH_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;			//�ܽž�Ϊ���
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(HEAT_PLATCH_GPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	StopHeatAll();
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����о����,��������������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	LatchHeatData(void)
{
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_ResetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
	GPIO_SetBits(HEAT_PLATCH_GPIO_PORT, HEAT_PLATCH_GPIO_PIN);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�����ȿ��ƽṹ
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitHeatCtrlStruct(void)
{
	g_tHeatCtrl.HeatEnable						= HEAT_DISABLE;
	g_tHeatCtrl.HeaterGroupNumber			= 0;
	g_tHeatCtrl.HeaterGroupNumberCpy	= 0;
	memset(g_tHeatCtrl.HeaterGroupElement, 0x00, MAX_HEAT_SUM);
	memset(g_tHeatCtrl.HeaterGroupElementCpy, 0x00, MAX_HEAT_SUM);
	//memset(g_tHeatCtrl.HeaterGroupElement, 0x00, sizeof(g_tHeatCtrl.HeaterGroupElement) / sizeof(g_tHeatCtrl.HeaterGroupElement[0]));
	//memset(g_tHeatCtrl.HeaterGroupElementCpy, 0x00, sizeof(g_tHeatCtrl.HeaterGroupElementCpy) / sizeof(g_tHeatCtrl.HeaterGroupElementCpy[0]));
}

/*******************************************************************************
* Function Name  : 
* Description    : ��ʼ����
* Autor					 : 
* Date					 : 2012-8-2
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	StartHeat(volatile uint16_t	HeatPins)		//2016.06.11
{
	#ifdef	DBG_DUMMY_HEAT
		
	#else
		HeatPowerOn();
		GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HeatPins);		//��Ӧ�ķ�����ȿ���
		
		#ifdef	MAINTAIN_ENABLE
			R_Counter.HeatCounter++;			//���ȼ�������1
			C_Counter.HeatCounter++;
		#endif
	#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : ȫ��ֹͣ����
* Autor					 : 
* Date					 : 2012-8-2
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	EndHeat(void)
{
	#ifdef	DBG_DUMMY_HEAT
		
	#else
		HeatPowerOff();
		StopHeatAll();
	#endif
}

/*******************************************************************************
* Function Name  : int16_t	GetTemperature(void)
* Description    : ���㵱ǰ��о��⵽���¶�,��λ��
* Autor					 : 
* Date					 : 2012-6-11
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
int16_t		GetTemperature(void)
{
	uint16_t i;
	int16_t	Temperature;
	//��о�ڲ��¶�ת����
	const int16_t	g_tTemperatureValueTable[] = 
	{
		(uint16_t)(ADC_MAX_VALUE * RTH_20 / (RH + RTH_20)),
		(uint16_t)(ADC_MAX_VALUE * RTH_15 / (RH + RTH_15)),
		(uint16_t)(ADC_MAX_VALUE * RTH_10 / (RH + RTH_10)),
		(uint16_t)(ADC_MAX_VALUE * RTH_5 / (RH + RTH_5)),
		(uint16_t)(ADC_MAX_VALUE * RTH0 / (RH + RTH0)),
		(uint16_t)(ADC_MAX_VALUE * RTH5 / (RH + RTH5)),
		(uint16_t)(ADC_MAX_VALUE * RTH10 / (RH + RTH10)),
		(uint16_t)(ADC_MAX_VALUE * RTH15 / (RH + RTH15)),
		(uint16_t)(ADC_MAX_VALUE * RTH20 / (RH + RTH20)),
		(uint16_t)(ADC_MAX_VALUE * RTH25 / (RH + RTH25)),
		(uint16_t)(ADC_MAX_VALUE * RTH30 / (RH + RTH30)),
		(uint16_t)(ADC_MAX_VALUE * RTH35 / (RH + RTH35)),
		(uint16_t)(ADC_MAX_VALUE * RTH40 / (RH + RTH40)),
		(uint16_t)(ADC_MAX_VALUE * RTH45 / (RH + RTH45)),
		(uint16_t)(ADC_MAX_VALUE * RTH50 / (RH + RTH50)),
		(uint16_t)(ADC_MAX_VALUE * RTH55 / (RH + RTH55)),
		(uint16_t)(ADC_MAX_VALUE * RTH60 / (RH + RTH60)),
		(uint16_t)(ADC_MAX_VALUE * RTH65 / (RH + RTH65)),
		(uint16_t)(ADC_MAX_VALUE * RTH70 / (RH + RTH70)),
		(uint16_t)(ADC_MAX_VALUE * RTH75 / (RH + RTH75)),
		(uint16_t)(ADC_MAX_VALUE * RTH80 / (RH + RTH80)),
		(uint16_t)(ADC_MAX_VALUE * RTH85 / (RH + RTH85)),
		(uint16_t)(ADC_MAX_VALUE * RTH90 / (RH + RTH90)),
		(uint16_t)(ADC_MAX_VALUE * RTH95 / (RH + RTH95)),
		(uint16_t)(ADC_MAX_VALUE * RTH100 / (RH + RTH100)),
	};
	
	for (i = 0; i < TEMPERATURE_TABLE_VALUE_TOTAL_NUMBER; i++)
	{
		if (g_tADC.Temperature_ADC_Value > g_tTemperatureValueTable[i])
		{
			break;
		}
	}
	Temperature = (TEMPERATURE_TABLE_VALUE_LOW + 5 * i);	//���㵱ǰ���¶�ֵ����-20������ÿ��5��һ������

	return (Temperature);
}

/*******************************************************************************
* Function Name  : 
* Description    : ����ȫ�ֱ���g_tbHeatElementBuf��1�ĸ���ͳ��,�Ƚ�������������
*	�������ĵ���,ȷ���Ƿ�һ�μ���.�ֱ�ͳ������ĵ���,����������֮�ͳ���1�μ���
*	���ֵ,������Ϊ���μ���,��������Ϊ1�μ���.
* Autor					 : 
* Date					 : 2016-3
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void	SetHeatNumber(void)
{
	volatile uint8_t	* p = NULL;
	uint32_t	i;
	uint32_t	Count;
	
#ifdef	HEAT_ONE_TIME_ENABLE		//������,�������ݶ���ͳͳһ�μ���
	g_tHeatCtrl.HeaterGroupNumber = 1;		//һ�м��ȴ���
	#if	MAX_HEAT_SUM == 2
		g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//���ȷ������
	#elif	MAX_HEAT_SUM == 1
		g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//���ȷ������
	#endif
#else		//����Ϊ��ʽ����
	p = g_tbHeatElementBuf;
	p += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;			//����пհ��ֽڲ��ܴ�ӡ
	Count = 0;
	
	for (i = 0; i < DST1_HEAT_BYTE; i++)		//�����������ֽ���
	{
		if (*p)		//�����ֵΪ0�Ͳ���ͳ��ֱ������
		{
			Count += GetHighBitNumberInByte( *p );	//ͳ��ÿ���ֽ��е�1�ĸ���
		}
		p++;
	}
	
	if (Count == 0)		//�������Ϊ0,������ͳ���Ҳ���
	{
		g_tHeatCtrl.HeaterGroupNumber = 1;		//һ�м��ȴ���
		#if	MAX_HEAT_SUM == 2
			g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//���ȷ������
		#elif	MAX_HEAT_SUM == 1
			g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//���ȷ������
		#endif
	}
	else		//�������ͳ���Ҳ�
	{
		if (Count == MAX_HEAT_DOT_ONE_TIME)	//�ﵽ�����ȵ�����ֱ���������μ���
		{
			g_tHeatCtrl.HeaterGroupNumber = 2;									//һ�м��ȴ���
			#if	MAX_HEAT_SUM == 2
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//���ȷ������
				g_tHeatCtrl.HeaterGroupElement[1] = m_tHeatCtrlPinTable[1];	//���ȷ������
			#elif	MAX_HEAT_SUM == 1
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//���ȷ������
			#endif
		}
		else		//�������ͳ����һ��������ʣ�µĵ���
		{
			for (i = 0; i < DST2_HEAT_BYTE; i++)	//�Ҳ���������ֽ���
			{
				if (*p)		//�����ֵΪ0�Ͳ���ͳ��ֱ������
				{
					Count += GetHighBitNumberInByte( *p );	//ͳ��ÿ���ֽ��е�1�ĸ���
				}
				p++;
			}
			
			if (Count <= MAX_HEAT_DOT_ONE_TIME)			//��������Ͳ��������һ�μ��ȵ���,һ�μ���
			{
				g_tHeatCtrl.HeaterGroupNumber = 1;		//һ�м��ȴ���
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0] | m_tHeatCtrlPinTable[1];	//���ȷ������
			}
			else
			{
				g_tHeatCtrl.HeaterGroupNumber = 2;									//һ�м��ȴ���
				g_tHeatCtrl.HeaterGroupElement[0] = m_tHeatCtrlPinTable[0];	//���ȷ������
				g_tHeatCtrl.HeaterGroupElement[1] = m_tHeatCtrlPinTable[1];	//���ȷ������
			}
		}		//ʣ�������ͳ�����
	}		//����ͳ�ƴ������
#endif
}

/*******************************************************************************
* Function Name  : uint16_t	GetHeatTime(void)
* Description    : ȷ������ʱ�䣬����������������������ߴ�ӡģʽ�Ĳ�ͬ��
*				   ����ǰ��о����¶ȵĲ�ͬ���м���ʱ�����
* Autor			 : 
* Date			 : 2016-3
* Input          : 
* Output         : 
* Return         : ȷ������һ���е�һ�μ���ʱ��,us
*******************************************************************************/
uint16_t	GetHeatTime(void)
{
	uint16_t	heattime;
	uint16_t	adjusttime;
	int32_t	Temperature;
	uint8_t	temp;
	
	const uint16_t	cHeatTimeTable[] = 
	{
		HEAT_TIME_LOW,			//��Ũ��
		HEAT_TIME_MIDDLE,		//�е�Ũ��
		HEAT_TIME_HIGH,			//��Ũ��
	};
#define	PAPER_KIND	(sizeof(cHeatTimeTable) / sizeof(cHeatTimeTable[0]))
	
#define	HEAT_ADJUST_TIME_US_LEVEL_1			(500)	//500
#define	HEAT_ADJUST_TIME_US_LEVEL_2			(400)	//400
#define	HEAT_ADJUST_TIME_US_LEVEL_3			(300)	//300
#define	HEAT_ADJUST_TIME_US_LEVEL_4			(200)	//200
#define	HEAT_ADJUST_TIME_US_LEVEL_5			(100)	//100
#define	HEAT_ADJUST_TIME_US_LEVEL_6			(50)		//50
#define	HEAT_ADJUST_TIME_US_LEVEL_7			(30)		//30
#define	HEAT_ADJUST_TIME_US_LEVEL_8			(10)		//10
#define	HATUL_1													(HEAT_ADJUST_TIME_US_LEVEL_1)
#define	HATUL_2													(HEAT_ADJUST_TIME_US_LEVEL_2)
#define	HATUL_3													(HEAT_ADJUST_TIME_US_LEVEL_3)
#define	HATUL_4													(HEAT_ADJUST_TIME_US_LEVEL_4)
#define	HATUL_5													(HEAT_ADJUST_TIME_US_LEVEL_5)
#define	HATUL_6													(HEAT_ADJUST_TIME_US_LEVEL_6)
#define	HATUL_7													(HEAT_ADJUST_TIME_US_LEVEL_7)
#define	HATUL_8													(HEAT_ADJUST_TIME_US_LEVEL_8)
	const uint16_t	cStartHeatTimeCompensateTable[] = 
	{
		HATUL_1,  HATUL_1,	HATUL_1,	HATUL_1,	HATUL_1,
		HATUL_2,  HATUL_2,	HATUL_2,	HATUL_2,	HATUL_2,
		HATUL_3,  HATUL_3,	HATUL_3,	HATUL_3,	HATUL_3,
		HATUL_4,  HATUL_4,	HATUL_4,	HATUL_4,	HATUL_4,
		HATUL_5,  HATUL_5,	HATUL_5,	HATUL_5,	HATUL_5,
		HATUL_6,  HATUL_6,	HATUL_6,	HATUL_6,	HATUL_6,
		HATUL_7,  HATUL_7,	HATUL_7,	HATUL_7,	HATUL_7,
		HATUL_8,  HATUL_8,	HATUL_8,	HATUL_8,	HATUL_8,
	};

#define	START_STEP_COMPENSATE_TABLE_LEN	\
	(sizeof(cStartHeatTimeCompensateTable) / sizeof(cStartHeatTimeCompensateTable[0]))

	temp = g_tSysConfig.Density;		//��ӡŨ�ȵ�������
	if (temp >= PAPER_KIND)					//����Ԥ��ֵʱ�����±�Խ��
		temp = 1;
	heattime = cHeatTimeTable[temp];	//��׼����ʱ��
	
	#ifdef	HEAT_TIME_BY_POWER_ADJUST_ENABLE
		heattime = GetAdjustHeatTimeOfPowerVoltage(heattime);	//�õ����ݵ�Դ��ѹ�����Ļ�׼����ʱ��
	#endif
	
	#ifdef	HEAT_TIME_START_STEP_ADJUST_ENABLE
		//��ӳ�ʼ���׶����Ӽ���ʱ��  2016.06.20  �����𲽵ļ���ʱ�� ���ڳ����Ƿ����
		if (g_iFeedMotorStartStep < START_STEP_COMPENSATE_TABLE_LEN)
		{	//heattime = GetAdjustHeatTimeOfFirstLineHeat(heattime);
			heattime += cStartHeatTimeCompensateTable[g_iFeedMotorStartStep];
		}
	#endif
	
	#ifdef	HEAT_TIME_BY_TEMPERATURE_ADJUST_ENABLE
		Temperature = GetTemperature();	//���㵱ǰ���¶�ֵ
		adjusttime = GetAdjustHeatTimeOfTemperature(Temperature);	//�õ������¶Ȳ����ĸ��Ӽ���ʱ��
		if (Temperature > ROOM_TEMPERATURE)
		{
			if (heattime > adjusttime)
			{
				heattime -= adjusttime;			//2017.03.16  ������280�ļ���ʱ��
				if(heattime < 280)
				{
					heattime = 280;
				}	
			}
		}
		else
		{
			heattime += adjusttime;
		}
	#endif
	
	if (heattime < MIN_HEAT_TIME)
	{	
		heattime = MIN_HEAT_TIME;
	}
	if (heattime > MAX_HEAT_TIME)
	{
		heattime = MAX_HEAT_TIME;
	}
	
	heattime /= STEP_NUMBERS_PER_DOT_HEIGHT;
	
	return (heattime);
}


/*******************************************************************************
* Function Name  : uint8_t GetHighBitNumberInByte(uint8_t DataIn)
* Description    : ���1�ֽ������� bit =1 �ĸ���
* Input          : �ֽ�
* Output         : 
* Return         : �ֽ��� 1 �ĸ���
*******************************************************************************/
static uint8_t	GetHighBitNumberInByte(uint8_t DataIn)
{
	uint32_t	i;
	uint32_t	Result;
	
	Result = 0;	
	for (i = 0; i < 8; i++)
	{
		if ((DataIn & 0x01) != 0)
		{
			Result++;
		}
		DataIn >>= 1;
	}
	
	return (Result);
}

#if	0
/*******************************************************************************
* Function Name  : uint16_t	GetPowerVoltage(void)
* Description    : ���㵱ǰ��⵽�Ĺ����ѹֵ,��λV
* Autor					 : 
* Date					 : 2013-3-21
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
static double	GetPowerVoltage(void)		//�����Դ��ѹֵ
{
	double	voltage;
	
	//�õ���ǰ�⵽��ADC�����Ĺ����ѹֵ��ע���ѹ��ѹ������ӷ�ѹ�������,��λV
	voltage = VREF * g_tADC.Power_ADC_Value / POWER_DIVIDE_RATIO / ADC_MAX_VALUE;
	
	return (voltage);
}
#endif

/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime)
* Description    : ���㲹��,���ݵ�ǰ��о�����ѹ�Ĳ�ͬ���м���ʱ�����.
* �Ա�׼��ѹΪ��׼,���յ�ѹ��ֵƽ������
*	Ti = Tstd * (Ustd / Ui)^2
* Autor			 : 
* Date			 : 2013-3-19
* Input          : ��
* Output         : ������׼����
* Return         : ���������һ���еĸ��ݹ����ѹ������һ�μ���ʱ�����,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime)
{
	double	heattime;
	uint32_t	StdPowerVoltageADC;
	uint32_t	CurrentPowerVoltageADC;
	volatile uint32_t Power_ADC_Value;
	
	StdPowerVoltageADC = ((uint32_t)POWER_VALUE_STD) * ((uint32_t)POWER_VALUE_STD);
	//CurrentPowerVoltageADC = ((uint32_t)g_tADC.Power_ADC_Value) * ((uint32_t)g_tADC.Power_ADC_Value);
	Power_ADC_Value = (uint32_t)g_tADC.Power_ADC_Value;
	CurrentPowerVoltageADC = Power_ADC_Value;
	CurrentPowerVoltageADC *= Power_ADC_Value;
	if (CurrentPowerVoltageADC)			//��ֹ����Ϊ0
		heattime = (uint16_t)(((double)(StdPowerVoltageADC * htime)) / ((double)(CurrentPowerVoltageADC)));
	else
		heattime = htime;
	
	return ((uint16_t)heattime);
}

//#ifdef	HEAT_TIME_START_STEP_ADJUST_ENABLE
#if	0
/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime)
* Description    : ���㲹��,���ڴӾ�ֹ��ʼ��ӡ�Ĳ��ֵ��н��м���ʱ�����.
* Autor			 : 
* Date			 : 2013-3-21
* Input          : 
* Output         : ������׼����
* Return         : ���������һ���еĸ��ݼ��ٲ������в�����һ�μ���ʱ�����,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime)
{
	uint16_t	CurrentStep;
	
	CurrentStep = g_iFeedMotorStartStep;
	htime += cStartHeatTimeCompensateTable[CurrentStep];
	
	return (htime);
}
#endif

/*******************************************************************************
* Function Name  : static uint16_t	GetAdjustHeatTimeOfTemperature(int16_t	Temperature)
* Description    : �����¶Ȳ���,���ݵ�ǰ��о����¶ȵĲ�ͬ���м���ʱ�����.
* ��25��Ϊ��׼,�¶�ÿ����1��,����ʱ���������us;�¶�ÿ����1��,
* ����ʱ����������us.�����¶�ֵ����Ϊ��������,ÿ�����ε��¶Ȳ���ֵ��ͬ.				   
* Autor			 : 
* Date			 : 2012-1-11
* Input          : ��о�¶�
* Output         : ���ӵ�������ʱ��
* Return         : ���������һ���еĸ����¶Ȳ����ĸ���һ�μ���ʱ�����,us
*******************************************************************************/
static uint16_t	GetAdjustHeatTimeOfTemperature(int32_t	Temperature)
{
	uint16_t	adjusttime;
	
	adjusttime = 0;	
	if (Temperature < -10)
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 5);
	}
	else if ( (Temperature >= -10) && (Temperature < 0) )
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 4);
	}
	else if ( (Temperature >= 0) && (Temperature < (int16_t)25) )
	{
		adjusttime = (uint16_t)((ROOM_TEMPERATURE - Temperature) * 5);    //2016.07.02 3 ��Ϊ5
	}
	else if ( (Temperature >= 25) && (Temperature < 60) )
	{
		adjusttime = (uint16_t)((Temperature - ROOM_TEMPERATURE) * 9);		//5,6,7,4���͹���  2016.07.02 2��Ϊ6 2017.03.16 7��Ϊ9
	}
	else
	{
		adjusttime = (uint16_t)((Temperature - ROOM_TEMPERATURE) * 7);		//5,6,7,4���͹���  2016.07.02 3��Ϊ7
	}
	
	return (adjusttime);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
