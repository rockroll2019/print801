/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-19
  * @brief   ��ֽ����ֽ�����صĳ���.
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
#include	"feedpaper.h"
#include	"blackmark.h"
#include	"heat.h"
#include	"print.h"
#include	"extgvar.h"
#include	"timconfig.h"
#include	"esc.h"
#include	"debug.h"
#include	"power.h"
extern	TypeDef_StructHeatCtrl		g_tHeatCtrl;
extern	TypeDef_StructSystemCtrl	g_tSystemCtrl;
//extern	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

extern	void	CutMotorStandby(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef	FEED_MOTOR_MONITOR_ENABLE
	uint8_t	m_bDriveCycleStepNumber;		//��ͬ����ģʽʱ����оƬѭ��һ�ܵĵ������
#endif

#ifdef	FEED_MOTOR_MONITOR_ENABLE
	uint8_t	g_bFeedMotorStepCount;
#endif

/*
	��ֽ�����λ����
							��λ0		��λ1		��λ2		��λ3
	PHASE_1(A)		1				1				0				0
	PHASE_2(B)		1				0				0				1
	��ʼ��Ϊ��λ0
*/
/* m_bFeedPhaseCount
	��־��ֽ������Ʋ�̬����λ��ȫ������ʱ4��1���У�
	��Χ[0~3]����ʼ��Ϊ0����ʼ�������λ���ƹܽ�ʱ����Ϊ��0״̬��ͬ��
	���ֹͣ���������㡣ֹͣ���ٴο�ʼʱҲ�������㡣
	������ֽʱÿ��һ�������1����4��0.��֮ǰ�ȼ�1���ٸ�����ֵȷ����λ���ƹܽŵ������
	Ϊ0ʱִ��0����λ��Ϊ1ʱִ��1����λ���������ơ�
						m_bFeedPhaseCount	��λA		��λB
	��ʼ��						0						1				1
	��1��							1						1				0
	��2��							2						0				0
	��3��							3						0				1
	��4��							4(0)				1				1
	
	������ֽʱÿ��һ�������1��Ϊ0ʱ����3.������ֵȷ����λ���ƹܽŵ������
	Ϊ0ʱִ��3����λ��Ϊ3ʱִ��2����λ�����εݼ���Ϊ1ʱִ��0����λ��
*/
__IO uint8_t		m_bFeedPhaseCount = 0;		//2016.06.16

uint8_t		g_bMaxRushStep = FEED_RUSH_STEPS;
uint32_t	g_iFeedMotorStartStep;

//�������ʱ���
const uint16_t g_tFeedMotorRushTable[] = 
{
	
	16000,
	16000,8000,	5000,	3500,	2800,	2500,	2300,	2144,	2035,	1941,		//����347��о  
	1859,	1786,	1721,	1663,	1610,	1561,	1517,	1477,	1439,	1404,
	1372,	1342,	1313,	1287,	1261,	1238,	1215,	1194,	1174,	1155,
	1136,	1119,	1102,	1086,	1071,	1056,	1042,	1029,	1016,	1003,
	991, 	979, 	968, 	957,	947, 	936,	927,	917,	908,	899,	
	890,	882,	873,	865,	857,	850,	842,	835,	828,	821,	
	815,	808,	802,	796,	789,	784,  778,	772,	766,	761,	
	756,	750,	745,	740,	735,	731,	726,	721,	717,	712,	//6
	708,	704,	699,	695,	691,	687,	683,	679,	675,	672,	//5
	668,	664,	661,	657,	654,	651,	647,	644,	641,	637,	//5
	634,	631,	628,	625,	622,	619,  616,	614,	611,	608,	//4
	605,	603,	600,	597,	595,	592,	590,	587,	585,	582,	//3
	580,	577,	575,	573,	570,	568
	

// 	16000,
// 	16000,8000,	5000,	3500,	2800,	2500,	2300,	2144,	2035,	1941,
// 	1859,	1786,	1721,	1663,	1610,	1561,	1517,	1477,	1439,	1404,
// 	1372,	1342,	1313,	1287,	1261,	1238,	1215,	1194,	1174,	1155,
// 	1136,	1119,	1102,	1086,	1071,	1056,	1042,	1029,	1016,	1003,
// 	991, 	979, 	968, 	957,	947, 	936,	927,	917,	908,	899,	
// 	890,	882,	873,	865,	857,	850,	842,	835,	828,	820,	//	
// 	813,	806,	799,	792,	785,	778,	771,	764,	757,	750,	//7
// 	744,	738,	732,	726,	720,	714,	708,	702,	696,	690,	//6
// 	685,	680,	675,	670,	665,	660,	655,	650,	645,	640,	//5
// 	635,	630,	625,	620,	615,	610,	605,	600,	595,	590,	//5
// 	586,	582,	578,	574,	570,	566,	562,	558,	554,	550,	//4
// 	547,	544,	541,	538,	535,	532,	529,	526,	523,	520,	//3
// 	522,	519,	516,	513,	510,	508,	506,	504,	502,	500,	//2
// 	498

	#if	0
	//20000,14000,8000, 6000, 4800,
	4100, 4000, 3898, 3700, 3600,
	3516, 3350, 3150, 3100, 3018, 
	2800, 2518, 2204, 1844, 1477,
	1250, 1100, 1020, 945,  884,
	833
	#endif
	#if	0
	20000,20000,18000,16000,14000,
	12000,10000,8000, 7000, 6000,
	5000, 4500, 4100, 3898, 3600,
	3516, 3350, 3150, 3100, 3018, 
	2800, 2518, 2204, 1844, 1477,
	1250, 1100, 1020, 945,  884,
	#endif
	#if	0
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  833,
	833,  833,  833,  833,  828,
	820,	813,	806,	799,  792,
	785,	778,	771,	764,	757,
	750,	744,	738,	732,  726,
	720,	714,	708,	702,	696,
	690,	685,	680,	675,  670,
	665,	660,	655,	650,	645,
	640,	635,	630,	625,  620,
	615,	610,	605,	600,	595,
	590,	586,	582,	578,  574,
	570,	566,	562,	558,	554,
	550,	547,	544,	541,  538,
	535,	532,	529,	526,	523,
	520,	522,	519,	516,  513,
	510,	508,	506,	504,	502,
	500,  498
	#endif
};

const uint16_t	m_tFeedMotorBackRushTable[ ] =         //��ֽ���ٱ�
{
//	20000,4000,3898,3016,2518,2218,2004,2000,2000,2000,2000,   //��о�ֲ���ٱ�
    20000,4050,3840,3518,3020,2520,2210,1850,1480,1252
};

//���������ٻ��߼���ʱ���ĳ���,ע��ǰ���sizeof����Ϊsizeof(FEEDMotorRushTable[])						   
#define FEED_MOTOR_RUSH_STEPS	(sizeof(g_tFeedMotorRushTable) / sizeof(g_tFeedMotorRushTable[0]))

//������ʼ��ס�׶Ρ���>�������׶Ρ���>��ֹͣ�׶Ρ���>������ס�׶�
//������ֽʱ��ɺ��ٽ׶�����ȫ��������Ҫ����С����,
//������ֽʱ�����С����С��FEED_MIN_FULL_STEPS,����õ�����ֽ
#define	FEED_MIN_FULL_STEPS		(FEED_MOTOR_RUSH_STEPS * 2)


/* Private function prototypes -----------------------------------------------*/
#ifdef	BLACKMARK_ADC_USE_PE_ADC
//��ֽ�Ͳ�ڱ깲��ͬһ��������
static void	FindBlackMarkSensorShared(void);
static void	JudgeBlackMarkPosition(void);
#else
//����Ϊ��ֽ�Ͳ�ڱ��໥����
static void	FindBlackMarkSensorIndependent(void);
#endif

//���µĺ���ȫ��Ϊ���غ���,ֻ��FeedPaper()����.
//��ֽ���������Ҫ4����λ��ͷһϵ�е�״̬�л����,��4����̬,��ֽ�����1��,����1���о���
static void FeedMoveMinStep(uint32_t direction, uint32_t step);		//��С������ֽ������ֽ
static void FeedMoveFast(uint32_t direction, uint32_t step);			//���ٽ�ֽ������ֽ

static uint16_t	GetMaxAccelerateStepsOfTemperature(void);
static uint16_t	GetMaxAccelerateStepsOfDymaticHeat(void);
static uint16_t	GetMaxAccelerateStepsOfDrivePrint(void);
static uint16_t	GetMaxAccelerateStepsOfLineSpace(void);
static uint16_t	GetMaxAccelerateStepsOfBitmapPrint(void);
static uint16_t	GetMaxAccelerateStepsOfEmphasizePrint(void);

#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
static uint16_t	GetMaxAccelerateStepsOfDifferentMagnification(void);
#endif

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ����ֽ������ƹܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitFeedPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(FEED_EN_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= FEED_EN_CTRL_GPIO_PIN_1 | FEED_EN_CTRL_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(FEED_EN_CTRL_GPIO_PORT, &GPIO_InitStructure);
	FeedMotorDisable();				//�Ȳ��ϵ�
	
	RCC_APB2PeriphClockCmd(FEED_PHASE_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= FEED_PHASE_CTRL_GPIO_PIN_1 | FEED_PHASE_CTRL_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(FEED_PHASE_CTRL_GPIO_PORT, &GPIO_InitStructure);
	FeedPhaseAllSet();			//��ʼ״̬00
	
	m_bFeedPhaseCount = 0;
	#ifdef	FEED_MOTOR_MONITOR_ENABLE
		g_bFeedMotorStepCount = 0;
	#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����ֽ����,���ݸ���ֵȷ����ֽ����,��ǰ������ת��ʹ��,
*   ������ʼ��ס�׶Ρ���>�������׶Ρ���>���ٽ׶Ρ���>��ֹͣ�׶Ρ���>������ס�׶�,
*   ������ֽʱ�����С����С��FEED_MIN_FULL_STEPS,����õ�����ֽ
*		ע�⣺����ֽ������ƺͼ��֮ǰ�����ȳ�ʼ���ܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void FeedPaper(uint32_t direction, uint32_t step)
{
	if (step == 0)
	{
		return;
	}
	
	if (direction == FEED_BACKWARD)		//���Ϊ��ֽ��Ҫ��������ֽ��
	{
		if (step > FEED_BACK_MAX_STEPS)	//�����ֵֽ���ܳ�������
		{
			return;
		}
	}
	
	if (step > FEED_MIN_FULL_STEPS)		 //��ֽ������һ���޶�
	{
		FeedMoveFast(direction, step);
	}
	else
	{
		FeedMoveMinStep(direction, step);
	}
}

/*******************************************************************************
* Function Name  : FindMarkPoint
* Description    : ��ֽ���ڱ괦,�߹��ڱ�ֹͣ,�����300mm��û���ҵ����Զ�ֹͣ.
	�޸��Һڱ귽��. ��Ϊ��ֽ�ͺڱ괫���������͹����������.
	a)��ֽ�ͺڱ괫��������ʱ, ���ݺڱ괫������־����.
	b)���߹���ʱ, ��Ϊ����ͬһ����ֽ��������ڱ�, �ںڱ�λ�ú�ֻ�н���ʱ, ADC���
	��ͬ, �����޷������ֺڱ�ͽ���. 
	�Һڱ�ʱ:
	1.�����ǰλ��Ϊ��ֽ(�Ǻڱ�λ��), ��ʼ��ֽ������300mm, ֱ���ҵ��ڱ�. �����жϺڱ�
	����. ����Ҳ����ڱ�, ˵�����ǰ�ֽ, ����Ѱ��.
	2.�����ǰλ��Ϊ�ڱ�(��ʵҲ����ֻ�ǽ���), �����жϺڱ괦��.
	�жϺڱ괦��: 
	������ֽ�������ڱ�߶ȳߴ�ֵ. 
	1.����ù����з��ְ�ֽ, ˵��֮ǰλ��Ϊ�ڱ�. �������ֽ, ֱ�����½���ڱ�λ��, ��
	����������ֽ2mmʹ�ڱ�ͣ������λ��.
	2.����ù�����û�з��ְ�ֽ, ˵��֮ǰλ��Ϊ����, ֹͣ��ֽ, ��ȱֽ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	FIND_BLACKMARK_MIN_LEN		(1 / MIN_STEP_UNIT_X)		//Ѱ��ʱ���پ���1mm,�������
#define	FIND_BLACKMARK_MAX_LEN		(300 / MIN_STEP_UNIT_X)	//���Ѱ�ҳ���300mm,�������
#define	FIND_BLACKMARK_HEIGHT_LEN	(10 / MIN_STEP_UNIT_X)		//�ڱ�߶�10mm,�������

void	FindMarkPoint(void)
{
	#ifdef	BLACKMARK_ADC_USE_PE_ADC
		FindBlackMarkSensorShared();			//��ֽ�Ͳ�ڱ깲��ͬһ��������
	#else
		FindBlackMarkSensorIndependent();	//��ֽ�Ͳ�ڱ��໥����  
	#endif
}

#ifdef	BLACKMARK_ADC_USE_PE_ADC
//��ֽ�Ͳ�ڱ깲��ͬһ��������
/*******************************************************************************
* Function Name  : FindBlackMarkSensorShared
* Description    : ��ֽ���ڱ괦,�߹��ڱ�ֹͣ,�����300mm��û���ҵ����Զ�ֹͣ.
	b)���߹���ʱ, ��Ϊ����ͬһ����ֽ��������ڱ�, �ںڱ�λ�ú�ֻ�н���ʱ, ADC���
	��ͬ, �����޷������ֺڱ�ͽ���. ֻ��ȷ���Ǻڱ�(���߽���������ֽ)����ֽ�������.
	�Һڱ�ʱ:
	1.�����ǰλ��Ϊ��ֽ(�Ǻڱ�λ��), ��ʼ��ֽ������300mm, ֱ���ҵ��ڱ�. �����жϺڱ�
	����. ����Ҳ����ڱ�, ˵�����ǰ�ֽ, ����Ѱ��.
	2.�����ǰλ��Ϊ�ڱ�(��ʵҲ����ֻ�ǽ���), �����жϺڱ괦��.
	�жϺڱ괦��: 
	������ֽ�������ڱ�߶ȳߴ�ֵ. 
	1.����ù����з��ְ�ֽ, ˵��֮ǰλ��Ϊ�ڱ�. �������ֽ, ֱ�����½���ڱ�λ��, ��
	����������ֽ2mmʹ�ڱ�ͣ������λ��.
	2.����ù�����û�з��ְ�ֽ, ˵��֮ǰλ��Ϊ����, ֹͣ��ֽ, ��ȱֽ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	FindBlackMarkSensorShared(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//�ȴ�֮ǰ����ֽ���
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)				//2016.06.30
	{
		;
	}
	
	//1.�����ǰλ��Ϊ��ֽ(�Ǻڱ�λ��), ��ʼ��ֽ������300mm, ֱ���ҵ��ڱ�. �����жϺڱ괦��.
	//����Ҳ����ڱ�, ˵�����ǰ�ֽ, ����Ѱ��.
	if (g_tSystemCtrl.MarkPositionFlag == 0)
	{
		while (g_tSystemCtrl.MarkPositionFlag == 0)	//һֱ�ߵ��ڱ괦
		{
			GoDotLine(FIND_BLACKMARK_MIN_LEN);				//��1mm
			PaperLenth++;															//ͳ����ֽ����
			if (PaperLenth > FIND_BLACKMARK_MAX_LEN)	//���������ܳ��Ȼ�û�ҵ��ڱ�ǰ��
			{
				break;
			}
		}
		
		//�ҵ��ڱ�. �����жϺڱ괦��.
		if (g_tSystemCtrl.MarkPositionFlag)
		{
			JudgeBlackMarkPosition();
		}
		//����Ҳ����ڱ�, ˵�����ǰ�ֽ, ����Ѱ��.
		else
		{
			return;
		}
	}
	//2.�����ǰλ��Ϊ�ڱ�(��ʵҲ����ֻ�ǽ�������ȫ�հ�), �����жϺڱ괦��.
	else
	{
		JudgeBlackMarkPosition();
	}
}

/*******************************************************************************
* Function Name  : JudgeBlackMarkPosition
* Description    : 	�жϺڱ괦��: 
	������ֽ�������ڱ�߶ȳߴ�ֵ. 
	1.����ù����з��ְ�ֽ, ˵��֮ǰλ��Ϊ�ڱ�. �������ֽ, ֱ�����½���ڱ�λ��, ��
	����������ֽ2mmʹ�ڱ�ͣ������λ��.
	2.����ù�����û�з��ְ�ֽ, ˵��֮ǰλ��Ϊ����, ֹͣ��ֽ, ��ȱֽ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	JudgeBlackMarkPosition(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//�ȴ�֮ǰ����ֽ���
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)			//2016.06.30
	{
		;
	}
	
	//�ںڱ�λ��(Ҳ�����ǽ�������ȫ�հ�)�¼�����ֽ�������ڱ�߶ȳߴ�ֵ
	g_tFeedButton.KeyDownFlag = 1;			//��ʼ�Һڱ�
	g_tFeedButton.StepCount = FIND_BLACKMARK_MAX_LEN;
	while (g_tSystemCtrl.MarkPositionFlag ==0 )
	{
		if(0 ==g_tFeedButton.StepCount)					//û�ҵ��ڱ�ǰ��
		{
			g_tFeedButton.KeyDownFlag =0;					  //ֹͣ��ֽ
			return;
	 	}
	}	
	//1.����ù����з��ְ�ֽ, ˵��֮ǰλ��Ϊ�ڱ�. 
	//�ټ���������ֽ10mmʹ�ڱ�ͣ������λ��.
	g_tFeedButton.KeyDownFlag =1;						            	//�������±�־
	g_tFeedButton.StepCount = FIND_BLACKMARK_HEIGHT_LEN;	//���úڱ���ֽ���򳤶����Ϊ10mm��
	while(g_tSystemCtrl.MarkPositionFlag !=0) 				    //�߹��ڱ�
	{
		if(0 ==g_tFeedButton.StepCount )				            //�����ڱ��ȣ���ֹͣ��������ȱֽ
			break;
	}
	
	if (g_tFeedButton.StepCount != 0)        
	{
		g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ȱֽ
	}
	//2.����ù�����û�з��ְ�ֽ, ˵��֮ǰλ��Ϊ����, ֹͣ��ֽ, ��ȱֽ.
	else
	{
		g_tError.PaperSensorStatusFlag |= 0x01;	  //û���߹��ڱ�ⱨȱֽ
	}
	g_tFeedButton.KeyDownFlag =0;							  //ֹͣ��ֽ
	g_tFeedButton.StepCount =0;			
}
#else
//����Ϊ��ֽ�Ͳ�ڱ��໥����
/*******************************************************************************
* Function Name  : FindBlackMarkSensorIndependent
* Description    : ��ֽ���ڱ괦,�߹��ڱ�ֹͣ,�����300mm��û���ҵ����Զ�ֹͣ.
	a)��ֽ�ͺڱ괫��������ʱ, ���ݺڱ괫������־����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	FindBlackMarkSensorIndependent(void)
{
	uint32_t	PaperLenth;
	
	PaperLenth = 0;
	
	//�ȴ�֮ǰ����ֽ���
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)			//2016.06.30
	{
		;
	}
	
	//�ںڱ�λ��(Ҳ�����ǽ�������ȫ�հ�)�¼�����ֽ�������ڱ�߶ȳߴ�ֵ
	g_tFeedButton.KeyDownFlag = 1;			//��ʼ�Һڱ�
	g_tFeedButton.StepCount = FIND_BLACKMARK_MAX_LEN;
	while (g_tSystemCtrl.MarkPositionFlag ==0 )
	{
		if(0 ==g_tFeedButton.StepCount)					//û�ҵ��ڱ�ǰ��
		{
			g_tFeedButton.KeyDownFlag =0;					  //ֹͣ��ֽ
			return;
	 	}
	}	
	//1.����ù����з��ְ�ֽ, ˵��֮ǰλ��Ϊ�ڱ�. 
	//�ټ���������ֽ10mmʹ�ڱ�ͣ������λ��.
	g_tFeedButton.KeyDownFlag =1;						            	//�������±�־
	g_tFeedButton.StepCount = FIND_BLACKMARK_HEIGHT_LEN;	//���úڱ���ֽ���򳤶����Ϊ10mm��
	while(g_tSystemCtrl.MarkPositionFlag !=0) 				    //�߹��ڱ�
	{
		if(0 ==g_tFeedButton.StepCount )				            //�����ڱ��ȣ���ֹͣ��������ȱֽ
			break;
	}
	
	if (g_tFeedButton.StepCount != 0)        
	{
		g_tError.PaperSensorStatusFlag &= (uint8_t)(~0x01);	//bit.0 = 0 ��ȱֽ
	}
	//2.����ù�����û�з��ְ�ֽ, ˵��֮ǰλ��Ϊ����, ֹͣ��ֽ, ��ȱֽ.
	else
	{
		g_tError.PaperSensorStatusFlag |= 0x01;	  //û���߹��ڱ�ⱨȱֽ
	}
	g_tFeedButton.KeyDownFlag =0;							  //ֹͣ��ֽ
	g_tFeedButton.StepCount =0;			
}
#endif

void	FeedMotorSetPhase(uint8_t	bPhase)
{
	if (bPhase == 0)
	{
		FeedPhaseAllSet();
	}
	else if (bPhase == 1)
	{
		FeedPhaseASet();
		FeedPhaseBReset();
	}
	else if (bPhase == 2)
	{
		FeedPhaseAllReset();
	}
	else if (bPhase == 3)
	{
		FeedPhaseAReset();
		FeedPhaseBSet();
	}
	else
	{
		FeedPhaseAllSet();
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ�����ǰת��1�����࣬���Ƶ����λ,������λ0-1-2-3-0˳��ѭ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	FeedMotorForward(void)
{
	m_bFeedPhaseCount++;
	m_bFeedPhaseCount %= 4;
	switch (m_bFeedPhaseCount)
	{
		case 0:
			FeedMotorSetPhase(0);
			break;
		case 1:
			FeedMotorSetPhase(1);
			break;
		case 2:
			FeedMotorSetPhase(2);
			break;
		case 3:
			FeedMotorSetPhase(3);
			break;
		default:
			//FeedMotorSetPhase(0);
			//m_bFeedPhaseCount = 0;
			break;
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ������ת��1�����࣬���Ƶ����λ,������λ0-3-2-1-0˳��ѭ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	FeedMotorBackward(void)
{
	switch (m_bFeedPhaseCount)
	{
		case 0:
			FeedMotorSetPhase(3);
			m_bFeedPhaseCount = 3;
			break;
		case 1:
			FeedMotorSetPhase(0);
			m_bFeedPhaseCount--;
			break;
		case 2:
			FeedMotorSetPhase(1);
			m_bFeedPhaseCount--;
			break;
		case 3:
			FeedMotorSetPhase(2);
			m_bFeedPhaseCount--;
			break;
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽn����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void FeedMotorBackStep(uint32_t BackNum)
{
	uint8_t i;
    uint8_t StepID;
	
	if(BackNum == 0)
    {    
		return;
    }
    g_bMotorRunFlag = 1;
    FeedMotorEnable();			//ʹ��оƬ,�������
    
	StepID = 0;
	BackNum += 10; 				//���ݻ�о�ֲ� ��Ҫ����ǰ����10�� 
      
    m_bFeedPhaseCount--;
	while(BackNum)
	{	
		BackNum--; 
        m_bFeedPhaseCount++;
        m_bFeedPhaseCount %= 4;
        switch (m_bFeedPhaseCount)
        {
            case 0:
			FeedMotorSetPhase(2);
			break;
            case 1:
			FeedMotorSetPhase(1);
			break;
            case 2:
			FeedMotorSetPhase(0);
			break;
            case 3:
			FeedMotorSetPhase(3);
			break;
        }   

		DelayUs(m_tFeedMotorBackRushTable[StepID]);	
		if (StepID <  9)	         //���ݻ�о�ֲ�ļ��ٱ� ȡ�����
        {    
			StepID++;	
        }    
	}
	DelayUs(20000);				     //���ݻ�о�ֲ� �ӳ�20ms
	
    m_bFeedPhaseCount--;
	StepID = 0;
	for(i = 0 ; i < 10 ; i++)       //���ݻ�о�ֲ� �ڽ�ֽ10��
	{
        m_bFeedPhaseCount++;
        m_bFeedPhaseCount %= 4;
        switch (m_bFeedPhaseCount)
        {
            case 0:
			FeedMotorSetPhase(0);
			break;
            case 1:
			FeedMotorSetPhase(1);
			break;
            case 2:
			FeedMotorSetPhase(2);
			break;
            case 3:
			FeedMotorSetPhase(3);
			break;
        }    
		DelayUs(m_tFeedMotorBackRushTable[StepID]);	
		if (StepID <  9)	//ȡ�����
        {    
			StepID++;
        }   
	}
	DelayUs(20000);				//�ӳ�20ms
	
	FeedMotorDisable();		    //�رյ��
	g_bMotorRunFlag = 0;
	
}


/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ�����ĳ������ת��1������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	StepFeedMotor(uint32_t direction)
{
	#ifdef	MAINTAIN_ENABLE
		static uint8_t	cMaintainCount = 0;
	#endif

	#ifdef	ERROR_EMERGENCY_PROTECT_ENABLE
		if (ErrorDetect())				//�ж��Ƿ��д��󼰱���
		{
			SetEmergencyProtect();	//����
			g_bFeedTaskFlag = 1;		//����δ��ɱ�־
			while (ErrorDetect());
			return;
		}
	#endif
		
	#ifdef	DBG_DUMMY_FEED

	#else
		FeedMotorEnable();			//ʹ��оƬ,����������˴���ͨ�����Ƶ�����С�ķ�ʽ
		if (direction == FEED_FORWARD)
		{
			FeedMotorForward();		//�������
		}
		else
		{
			FeedMotorBackward();	//�������
		}
		
		#ifdef	MAINTAIN_ENABLE
			cMaintainCount++;
			if (cMaintainCount == DEFAULT_CODE_LINE_HEIGHT * STEP_NUMBERS_PER_DOT_HEIGHT)
			{	
				cMaintainCount = 0;
				R_Counter.LineCounter++;	//�м�������1,����Ϊ��λ
				C_Counter.LineCounter++;
			}
		#endif
		
		#ifdef	FEED_MOTOR_MONITOR_ENABLE
			if (direction == FEED_FORWARD)
			{
				g_bFeedMotorStepCount++;
				if (g_bFeedMotorStepCount >= m_bDriveCycleStepNumber)
				{
					g_bFeedMotorStepCount = 0;
				}
				if (GetFeedMotorDriveCycleHomeStatus() == 0x00)
				{
					if (g_bFeedMotorStepCount)	//�����˲�һ��
					{
						InitFeedPort();
					}
				}
			}
			else
			{
				if (g_bFeedMotorStepCount)
				{
					g_bFeedMotorStepCount--;
				}
				else
				{
					g_bFeedMotorStepCount = (m_bDriveCycleStepNumber - 1);
				}
				if (GetFeedMotorDriveCycleHomeStatus() == 0x00)
				{
					if (g_bFeedMotorStepCount)	//�����˲�һ��
					{
						InitFeedPort();
					}
				}
			}
		#endif	//FEED_MOTOR_MONITOR_ENABLE �������
	#endif	//��ʵ��ֽ�������
	g_iFeedMotorStartStep++;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ�����ʼ�ӵ�,����֮ǰ����λֵ,ά��һ��ʱ��,����û���²���
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	StartFeedMotor(uint32_t direction)	//��ʼ��ס�׶�
{
	#ifdef	DBG_DUMMY_FEED
		
	#else
		FeedMotorEnable();			//ʹ��оƬ,�������
		if (direction == FEED_FORWARD)
		{
			//FeedMotorForward();		//�������
		}
		else
		{
			//FeedMotorBackward();	//�������
		}
	#endif
	
	g_bMotorRunFlag = 1;
	g_StepCount = 0;
	g_iFeedMotorStartStep = 0;
	MotoFeedInc = 1;	
}

void	StopFeedMotor(uint32_t direction)	//������ס�׶�
{
	#ifdef	DBG_DUMMY_FEED
		
	#else
		FeedMotorEnable();			//ʹ��оƬ,�������
		if (direction == FEED_FORWARD)
		{
			//FeedMotorForward();		//�������
		}
		else
		{
			//FeedMotorBackward();	//�������
		}
	#endif
}

void EndFeedMotor(void)	//ȫ����·�׶�
{
	HeatPowerOff();
	FeedMotorDisable();		//��ֹоƬ,H�Ų��������
	g_bMotorRunFlag = 0;
	g_StepCount = 0;
	g_bMaxRushStep = MAX_RUSH_STEPS;
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_QT)	//WH����ģʽ��־
	{
		g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;	//�˳�WH����ģʽ
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ����Ժ�����С������,����ʵ��һ��һ����С��.
* Input          : direction�����ƶ��ķ���,step�����ƶ��Ĳ�����,����ֵ >= 0
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void FeedMoveMinStep(uint32_t direction, uint32_t step)	//��С������ֽ������ֽ
{
	if ((direction == FEED_FORWARD) || (direction == FEED_BACKWARD))
	{
		StartFeedMotor(direction);			//��ʼ��ס�׶�
		DelayUs(FEED_START_US);					//��ʱus
		while (step--)
		{
			StepFeedMotor(direction);			//�ı�һ����λ
			DelayUs(FEED_SINGLE_STEP_SPEED_US);	//��ʱus
		}
		
		StopFeedMotor(direction);				//������ס�׶�
		DelayUs(FEED_STOP_US);					//��ʱus
		EndFeedMotor();									//ȫ����·�׶�
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ֽ����Ը��ٲ���,����ʵ�ֿ��ٽ�ֽ������ֽ
* Input          : direction�����ƶ��ķ���,step�����ƶ��Ĳ�����,����ֵ >= 0
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void FeedMoveFast(uint32_t direction, uint32_t step)
{
	uint32_t	i;
	uc16 * ptime;
	
	if ((direction == FEED_FORWARD) || (direction == FEED_BACKWARD))
	{
		if (step > FEED_MIN_FULL_STEPS)					//ȷ�������㹻��
		{
			StartFeedMotor(direction);						//��ʼ��ס�׶�
			DelayUs(FEED_START_US);								//��ʱus
			
			i = 0;
			ptime = g_tFeedMotorRushTable;				//ָ�����ʱ�����ʼ
			while (i < FEED_MOTOR_RUSH_STEPS)			//���ٽ׶�
			{
				StepFeedMotor(direction); //ת��һ��
				DelayUs(*ptime++);									//����������ʱ
				i++;
			}
			
			i = step - FEED_MIN_FULL_STEPS;				//������ٲ���
			ptime = g_tFeedMotorRushTable;				//ָ�����ʱ�����ʼ
			ptime += (FEED_MOTOR_RUSH_STEPS - 1); //ָ�����ʱ���ĩβ
			while (i)															//���ٽ׶�
			{
				StepFeedMotor(direction);	//ת��һ��
				DelayUs(*ptime);										//������ʱ
				--i;
			}
			
			i = FEED_MOTOR_RUSH_STEPS;						//���ٲ���
			ptime = g_tFeedMotorRushTable;				//ָ�����ʱ�����ʼ
			ptime += (FEED_MOTOR_RUSH_STEPS - 1);	//ָ�����ʱ���ĩβ
			while (i)
			{
				StepFeedMotor(direction); //ת��һ��
				DelayUs(*ptime--);									//������ʱ
				--i;
			}
			
			StopFeedMotor(direction);							//������ס�׶�
			DelayUs(FEED_STOP_US);								//��ʱus
			
			EndFeedMotor();												//ȫ����·�׶�
		}
	}
}


static uint16_t	GetMaxAccelerateStepsOfTemperature(void)
{
	int16_t	temperature;
	
	temperature = GetTemperature();					//�����о�¶�ֵ
	if (temperature < 10)
	{
		return (MAX_RUSH_STEPS_UNDER_10_CENTIGRADE);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}

static uint16_t	GetMaxAccelerateStepsOfDymaticHeat(void)
{
	if (g_tHeatCtrl.HeaterGroupNumberCpy > 1)
	{
		return (MAX_RUSH_STEPS_ON_DYNAMIC_HEAT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}

static uint16_t	GetMaxAccelerateStepsOfLineSpace(void)
{
	uint16_t	LineSpaceDotNumber;
	uint16_t	maxstep;
	uint16_t	tempstep;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//�����м�����
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
		LineSpaceDotNumber = (ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh);
	else
		LineSpaceDotNumber = 0;
	
	//����ʱʹ�ñ�׼1:1��������������ӡ,������ͬ,ֻ�޸��м��,��ѹ24V
	//���в���Ч��������ÿ����������޽��еĽ��,��ֵ��ʾ���ٲ���
	if (LineSpaceDotNumber > 2)
	{	//126(220mm/s)ʱ��ӡ����,�������Ŵ�8����ӡ����˳��,ż������ְ׵�
		maxstep = MAX_RUSH_STEPS;
	}
	else
	{
		maxstep = 90 + LineSpaceDotNumber * 10;
	}
	
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
		tempstep = GetMaxAccelerateStepsOfDifferentMagnification();
		if (tempstep < maxstep)
		{
			maxstep = tempstep;
		}
	#endif
	
	return (maxstep);
}

#if	0
static uint16_t	GetMaxAccelerateStepsOfBitmapPrint(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))	//ͼ�δ�ӡ
	{
		return (MAX_RUSH_STEPS_ON_PRT_BITMAP);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
static uint16_t	GetMaxAccelerateStepsOfEmphasizePrint(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.EmphaSizeMode)		//���ش�ӡ
	{
		return (MAX_RUSH_STEPS_ON_EMPHASIZE_PRT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
//����Ŵ���1,����Ŵ���2,3ʱ,����ѹ��;
//����Ŵ���1,����Ŵ���2ʱ,E,U��ѹ��;
static uint16_t	GetMaxAccelerateStepsOfDifferentMagnification(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//if ((ptLPB->PrtMode.MaxRatioX == 1) && \
	//		((ptLPB->PrtMode.MaxRatioY == 2) || (ptLPB->PrtMode.MaxRatioY == 3)))
	if ((ptLPB->PrtMode.MaxRatioX > 1) || (ptLPB->PrtMode.MaxRatioY > 1))
	{
		return (MAX_RUSH_STEPS_ON_DIFFERENT_MAGNIFICATION_PRT);
	}
	else
	{
		return (MAX_RUSH_STEPS);
	}
}
#endif

static uint16_t	GetMaxAccelerateStepsOfDrivePrint(void)
{
	uint16_t	StepNum;
	
	StepNum = MAX_RUSH_STEPS;
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)
	{
		if (g_tSystemCtrl.PrtSourceIndex == COM)						//����
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_COM;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == USB)				//U��
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_USB;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//E��
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_ETHERNET;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == LPT)				//L��
		{
			StepNum = MAX_RUSH_STEPS_ON_SP_DRV_PRT_LPT;
		}
		else
		{
		}
	}
	else
	{
		if (g_tSystemCtrl.PrtSourceIndex == COM)						//����
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_COM;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == USB)				//U��
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_USB;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//E��
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_ETHERNET;
		}
		else if (g_tSystemCtrl.PrtSourceIndex == LPT)				//L��
		{
			StepNum = MAX_RUSH_STEPS_ON_WH_DRV_PRT_LPT;
		}
		else
		{
		}
	}
	
	return (StepNum);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȷ�������߼��ٲ���,�����¶�,���ȴ���,�Ƿ�������ӡ�ۺ�ȷ��
*	�¶���0~10��ʱ,��ӡʱ����ٶȲ�����1200PPS,�����ٲ�������68
*	��̬����ʱ������1200PPS
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
uint16_t	GetMaxAccelerateSteps(void)
{
	uint16_t	maxstep;
	uint16_t	temp;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	//2016.06.11 ���ݴ�ӡģʽ��ֵ
	
	MAX_RUSH_STEPS = g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode];		//2016.06.11
	maxstep = MAX_RUSH_STEPS;		//Ĭ��ֵ
#if	0
	temp = GetMaxAccelerateStepsOfTemperature();	//����¶�
	if (temp < maxstep)
	{
		maxstep = temp;
	}
	
	temp = GetMaxAccelerateStepsOfDymaticHeat();	//�����ȿ���
	if (temp < maxstep)
	{
		maxstep = temp;
	}
	
	if (g_tPrtCtrlData.PCDriverMode)	//������ӡ,����ģʽʱ��ʹ�ô�ӡ������
	{
		temp = GetMaxAccelerateStepsOfDrivePrint();
		if (temp < maxstep)
		{
			maxstep = temp;
		}
	}
	else
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))	//ͼ�δ�ӡ
		{
			temp = MAX_RUSH_STEPS_ON_PRT_BITMAP;
			if (temp < maxstep)
			{
				maxstep = temp;
			}
		}
		else	//��ͼ�δ�ӡ
		{
			#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
			if (ptLPB->PrtMode.EmphaSizeMode)		//���ش�ӡ
			{
				temp = MAX_RUSH_STEPS_ON_EMPHASIZE_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			#endif
			
			temp = GetMaxAccelerateStepsOfLineSpace();	//����ӡ������
			if (temp < maxstep)
			{
				maxstep = temp;
			}
			if (ptLPB->PrtMode.CharCycle90)			//��ת90�ȴ�ӡ
			{
				temp = MAX_RUSH_STEPS_ON_CYCLE90_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
			if (ptLPB->PrtMode.CharNotFlag)		//���Դ�ӡ
			{
				temp = MAX_RUSH_STEPS_ON_CHAR_NOT_PRT;
				if (temp < maxstep)
				{
					maxstep = temp;
				}
			}
			#endif
		}
	}
#endif	
	return (maxstep);
}

/*******************************************************************************
* Function Name  : void AdjustMotorStep(uint16_t minsteptime)
* Description    : ���ݴ������С����ʱ���뵱ǰ����ʱ�Ƚ��ҳ��µ��ʺϵĲ�̬˳���
* ����ȫ�ֱ���g_StepCount��Ӧ˳��ʱ�䣬���minsteptime<��ǰʱ�䣬���ı䵱ǰ˳��
* ����������˳��ݼ����ٱȽ�ֱ�������������ߵ���¼ͷ��
* ע����Ҫ�жϴӵ�ǰg_StepCount - 4��ʼ����,��Ϊ�����������һ��ҲҪ����minsteptime
* 2016-7-13�޸ģ������Ƿ�������ӡ��������ٶ�.
* Autor			 : 
* Date			 : 2016-7
* Input          : һ��������ʱ��
* Output         : 
* Return         : 
*******************************************************************************/
void	AdjustMotorStep(uint16_t minsteptime)
{
	uint16_t	maxstep;
	
	//�õ������ٲ���,�������δ�ӡ���������������ݵ�����޶��ٶ�
	maxstep = GetMaxAccelerateSteps();
	if (g_StepCount > maxstep)	//�ж�Ŀǰ�ٶ��Ƿ񳬹��޶�����ٶ�
	{
		g_StepCount = maxstep;		//�������,���޶��ٶ�Ϊ��׼���е���
	}
	g_bMaxRushStep = maxstep;
	
	//�ж������ٲ�����ʱ���Ƿ��Ӱ�����,�����Ӱ��ֱ�ӷ���,���������
	//����ٶ�ʱ��Ӱ�����,��Ҫ����,�ҳ����ٱ��м���ʱ�����һ�����ܼ���ʱ��Ĳ���
	while (1)		//2016.06.14 ��֤����߲�ʱ��Ҫ���ڼ���ʱ��
	{
		if(g_tFeedMotorRushTable[g_StepCount] > (minsteptime+20))
		{
			break;
		}			
		g_StepCount--;
	}
}
/*******************************************************************************
* Function Name  : KeyFeedPaper(uint16_t DotLine)
* Description    : ��ֽn���У�������ֽ��ʽ ��ռ�ô�ӡ������
* Input          : None
* CONput         : None
* Return         : None
*******************************************************************************/
void KeyFeedPaper(uint16_t DotLine)
{
  g_tFeedButton.KeyDownFlag = 1;			
	g_tFeedButton.StepCount = DotLine;	
}
/*******************************************************************************
* Function Name  : GoDotLine
* Description    : ��ֽN����
* Input          : DotLineNumber:��ֽ�ĵ�����
* Output         : 
* Return         : 
*******************************************************************************/
void	GoDotLine(uint32_t DotLineNumber)
{
	uint32_t	i;
	for (i = 0; i < DotLineNumber; i++)
	{
			PrtDrvBufFull();										//2016.06.27 �ߵ��з����д�ӡ������
			IncDrvPrtBufAddr(0x00, 0);
	}
}

#define	WAIT_LPB_ILDE_MS		(10)
void	WaitPreviousPrintFinish(void)
{
	while (LPB_STATUS_ILDE != GetPrintStatus());
	
	#if	0
	uint32_t	SleepCounter;    	//��ʱ
	
	SleepCounter = g_tSystemCtrl.SleepCounter;
	while (LPB_STATUS_ILDE != GetPrintStatus())
	{
		if (g_tSystemCtrl.SleepCounter > SleepCounter)
		{
			if ((g_tSystemCtrl.SleepCounter - SleepCounter) > WAIT_LPB_ILDE_MS)
			{
				StartTimer(PRINT_CHECK_TIMER);
				SleepCounter = g_tSystemCtrl.SleepCounter;
			}
		}
		else
		{
			SleepCounter = g_tSystemCtrl.SleepCounter;
		}
	}
	#endif
}

//�ȴ��������
void	WaitHeatFinish(void)
{
	while (g_tHeatCtrl.HeatEnable == HEAT_ENABLE);
}

void	WaitFeedPaperFinish(void)
{
	while (g_tSystemCtrl.StepNumber);		//�ȴ���ֽ���
}

void	WaitHeatAndFeedFinish(void)
{
	//�ȴ�ǰ������ֽ���߼������
	while ((g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || (g_tSystemCtrl.StepNumber != 0));
}

//�ȴ����Ȼ���ֽ���
void	WaitPrintAndFeedPaperFinish(void)
{
	WaitPreviousPrintFinish();
	WaitHeatAndFeedFinish();
}

/*******************************************************************************
* Function Name  : void ErrorDetect(void)
* Description	: ������⡣������ֱ�������ȴ���ǰ�д�ӡ���ؼ���\�����
*					���ȴ�ֱ�����������
* Input		  : None
* Output		 : None
* Return		 : None
*******************************************************************************/
uint32_t	ErrorDetect(void)
{
	uint8_t	ErrorStatus;
	
	ErrorStatus = 0;
	if (g_tError.AR_ErrorFlag || g_tError.R_ErrorFlag || g_tError.UR_ErrorFlag)
	{
		ErrorStatus = 1;
	}
	else
	{
		if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	//ֽ��ֱ��ֹͣ��ӡ
		{
			ErrorStatus = 1;
		}
		else
		{
			if ((g_tError.PaperEndStopSensorSelect & 0x01) == 0x01)	//ѡ����ֽ����������ֹͣ��ӡ
			{
				if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//ֽ����ֹͣ��ӡ
				{	//��ӡ����ӡ�굱ǰ�в���ֽ���������״̬
					if ((LPB_STATUS_ILDE == GetPrintStatus()) && \
							(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && \
							(g_tSystemCtrl.StepNumber == 0))
					{
						ErrorStatus = 1;
					}
				}
			}
		}
	}
	
	return	(ErrorStatus);
}

void	SetEmergencyProtect(void)
{
	TIM_Cmd(FEED_TIMER, DISABLE);					//�ض�ʱ��
	TIM_Cmd(HEAT_TIMER, DISABLE);					//�ض�ʱ��
	TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//�ض�ʱ��
	
	EndHeat();						//�ؼ���
	EndFeedMotor();				//����ֽ���
	#if defined (CUTTER_ENABLE)
		CutMotorStandby();	//���е����
	#endif
	
	g_tRealTimeStatus.PrinterStatus |= 0x08;	//�Ƚ�������״̬
}

void	ReleaseEmergencyProtect(void)
{
	g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//�����ų����,�Զ���������״̬
	DelayMs(1000);
	if (LPB_STATUS_ILDE != GetPrintStatus())
	{
		TIM_Cmd(PRINT_CHECK_TIMER, ENABLE);		//����ʱ��
	}
	
	if ((g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || (g_tSystemCtrl.StepNumber != 0))
	{
		TIM_Cmd(FEED_TIMER, ENABLE);					//����ʱ��
		TIM_Cmd(HEAT_TIMER, ENABLE);					//����ʱ��
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
