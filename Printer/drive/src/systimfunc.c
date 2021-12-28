/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-7-31
  * @brief   ��صĳ���.
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
#include	"systimfunc.h"
#include	"ad.h"
#include	"timconfig.h"
#include	"extgvar.h"
#include	"monobyte.h"
#include	"print.h"
#include	"heat.h"
#include	"cutter.h"
#include	"feedpaper.h"
#include	"platen.h"
#include	"beep.h"
#include	"led.h"
#include	"maintain.h"
#include	"parallel.h"
#include	"power.h"
#include "speaker.h"

extern  volatile uint8_t	g_bPrtSPIDMAIntForDriveMode;
extern	TypeDef_StructHeatCtrl		g_tHeatCtrl;
//extern	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
extern	uc16	g_tFeedMotorRushTable[];
extern	uint8_t	g_bMaxRushStep;
extern	TypeDef_StructInBuffer volatile g_tInBuf;  //2016.06.08

#ifdef	PAPER_OUT_RE_PRINT_ENABLE
extern	void	PaperOutDetect(void);
#endif
extern uc16	m_tHeatCtrlPinTable[];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	LED_DELAY_PE_MS			  (100)
#define	LED_FAST_PE_MS			  (200)
#define	LED_FAST_FLASH_MS			(400)
#define	LED_SLOW_FLASH_MS			(800)
#define	LED_SLOW_DOOR_MS			(1000)
#define	LED_STATUS_OFF				(0)
#define	LED_STATUS_ON					(1)
#define	LED_PTK_ON_MS			    (200)   //δȡ������
#define	LED_PTK_OFF_MS			  (100)  //
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void	ErrorLedFlash(void);
static void	PELedFlash(void);
static void	PTKLedAndBuzzerFlash(void);
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʱ��������,ϵͳʱ������,��������ʱ1mS�ж�1��,
*	���жϳ����б�����,�ڽ������ߺ�,ÿ0.25���ж�һ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	SystemTimer2Func(void)
{
  static uint16_t	DoorNum = 0;
	static uint16_t	CutLedNum = 0;
  static uint16_t	TempNum = 0;
	static uint16_t DataDealNum = 0;
	#ifdef	MAINTAIN_ENABLE	
		static uint16_t	TimeCntr = 0;
	#endif
	
	g_bRoutinFlag = 0;
	g_tSystemCtrl.SleepCounter++;	 //ϵͳ�������ߵļ�����,��������Դ�ڻ�����
	//2018.12.10  
	NetReloadCounter++;	
	OpenADC(); 							//��AD�ɼ�

	if((g_tSpeaker.SpeakEnable == 1) || (g_tSpeaker.SpeakPEnable == 1))
	{
		g_tSpeaker.SpeakCnt++;
		if(g_tSpeaker.SpeakCnt > 5000)
		{
			g_tSpeaker.SpeakCnt = 0;
			g_tSpeaker.SpeakFlag = 0;
		}
	}
	else
	{
		g_tSpeaker.SpeakCnt = 0;
	}
	
	//û������ ���� д����flash�Ѿ���ʼ
	if((g_tSpeaker.SpeakWriteHandle == WRITE_START) && (g_tInBuf.PutPosition == g_tInBuf.GetPosition))
	{
		if(g_tSpeaker.SpeakWriteTime++ >1000)
		{
			g_tSpeaker.SpeakWriteTime = 0;
			g_tSpeaker.SpeakWriteHandle = WRITE_END;
		}
	}
	
	if(g_tSpeaker.SpeakBufFlg == 1) //������·����
	{
		
			Line_2A_WTN588H_Tim(g_tSpeaker.SpeakBuf);
	}
	
	
	#if	0
	//��ֹ������ر�
	if (LPB_PROCESS_STATUS_UNFINISH == GetLPBProcessStatus())
	{
		StartTimer(PRINT_CHECK_TIMER);
	}
	#endif
	
	#ifdef	REALTIME_STATUS_UPDATE_ENABLE
	
	CheckCoverStatus();
		
  if((AutoFeedPaper == 2) && (g_tError.PaperSensorStatusFlag & 0x01) == 0 )  
	{
	   TempNum++;
		 if(TempNum>500)			
		 {				
		   if( g_tSystemCtrl.PaperType == PAPER_TYPE_CONTINUOUS)		
			 {
 				  //g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 �������±�־ ��λ
 					//g_tFeedButton.StepCount = 800;
			 }
				else if(g_tSystemCtrl.PaperType == PAPER_TYPE_LABLE)  //��ǩֽ		
				{
					FeedToGapFlag = 1;		
					FeedPaperFlag = 1;        
				}	
			 AutoFeedPaper = 0;
			 TempNum=0;
		 }	 
	  }
		else
    {    
			TempNum=0;
    }
        
		if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	//ֽ��ֱ��ֹͣ��ӡ
		{
			g_tRealTimeStatus.PrinterStatus |= 0x08;	//��������״̬
		}
		else	//ֽ�������������ֽ
		{
			g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);		//��ȱֽ��������״̬
			if ((g_tError.PaperEndStopSensorSelect & 0x01) == 0x01)	//ѡ����ֽ����������ֹͣ��ӡ
			{
				if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//ֽ����ֹͣ��ӡ
				{	//��ӡ����ӡ�굱ǰ�в���ֽ���������״̬
					if ((LPB_STATUS_ILDE == GetPrintStatus()) && \
							(g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && \
							(g_tSystemCtrl.StepNumber == 0))
					{
						g_tRealTimeStatus.PrinterStatus |= 0x08;	//��������״̬
					}
				}
				else	//ֽ���������ֽ
				{
					g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//��ȱֽ��������״̬
				}
			}
		}
	#endif     

		
	AutomaticStatusBack();				//�Զ�״̬���ش��� 2016.07.07 ÿ������ �����Զ�����ʱ�ٷ�������

	
	//����Ǯ���
	if(g_tDrawer1.ON_Time)						//Ǯ��1���ƣ�
	{
		if(g_tDrawer1.ON_Time <= g_tDrawer1.OFF_Time)
		{
			DriveDrawer(g_tDrawer1.DrwID, DISABLE);
		}
		g_tDrawer1.ON_Time--;
	}
	if(g_tDrawer2.ON_Time)						//Ǯ��1���ƣ�
	{
		if(g_tDrawer2.ON_Time <= g_tDrawer2.OFF_Time)
		{
			DriveDrawer(g_tDrawer2.DrwID, DISABLE);
		}
		g_tDrawer2.ON_Time--;
	}
	//2016.09.14 ���Ʒ�����
	DrvBuzzerFun();
	#ifdef	PARALLEL_PNE_OUT_ENABLE
		if (g_tInterface.LPT_Type)	//���ڻ������PE�ź�
		{
			if (g_tError.LPTPaperEndSensorSelect)	//PE����PNE֮��������һ����ѡ�������ֽ���ź�
			{	//PE����PNE����֮���κ�һ����⵽ȱֽ�ұ�ʹ�ܼ����ȱֽ
				if ((g_tError.LPTPaperEndSensorSelect & 0x03) == 0x03)	//PE = E, PNE = E,���߾�ѡ��
				{
					if (g_tError.PaperSensorStatusFlag & 0x01)	//�����κ�һ����⵽ȱֽ�ź������ȱֽ
						LPT_PE_Out_PaperEnd();			//�������ȱֽ�ź�
					else
						LPT_PE_Out_PaperFull();			//������ڲ�ȱֽ�ź�,���߶�û�м�⵽ȱֽ�ź�
				}
				else if ((g_tError.LPTPaperEndSensorSelect & 0x02) == 0x02)	//PE = E,ֻѡ��PE
				{
					if ((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)
						LPT_PE_Out_PaperEnd();			//�������ȱֽ�ź�
					else
						LPT_PE_Out_PaperFull();			//������ڲ�ȱֽ�ź�,PEû�м�⵽ȱֽ�ź�
				}
				else	//PNE = E,ֻѡ��PNE
				{
					if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)
						LPT_PE_Out_PaperEnd();			//�������ȱֽ�ź�
					else
						LPT_PE_Out_PaperFull();			//������ڲ�ȱֽ�ź�,PNEû�м�⵽ȱֽ�ź�
				}
			}
			else	//���߾�δ��ѡ���������ֽ���ź�
			{
				LPT_PE_Out_PaperFull();			//������ڲ�ȱֽ�ź�
			}
		}
	#endif	//PARALLEL_PNE_OUT_ENABLE ����
	
	#ifdef	MAINTAIN_ENABLE	
		//����ʱ��ά��������,�Է���Ϊ��λ
		if (g_bSleepModeFlag)
		{
			TimeCntr += ROUTINE_SLEEP_INTERVAL_MS;
		}
		else
		{
			TimeCntr++;
		}
		if (TimeCntr >= 60*1000)		//60��
		{
			TimeCntr = 0;
			R_Counter.TimeCounter++;
			C_Counter.TimeCounter++;
//			if (g_tResetableCounter.TimeCounter > \
//				 (g_tResetableCounterBakeup.TimeCounter + MAINTAIN_WRITE_ROUNTIN_MINUTE))
//			{
//				g_bMaintainRountinWriteFlag = 1;
//			}
		}
	#endif
	//2016.06.16 ������ӡ����������������ʼ��ʱ��5��ӡ
	DataDealNum++;
  if(DataDealNum == 20)		
	{
		DataDealNum = 0;
		
		if( g_bMotorRunFlag == 0 && g_tError.DoorOpen ==0 				//���û������,�ϸ�û�� 
			&& (g_tError.PaperSensorStatusFlag & 0x01)==0	          //û��ȱֽ
			&& (g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0)	//û�д���
		{
			if (U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD || g_tFeedButton.KeyDownFlag ==1)		//2016.07.10 ��ӡ�����������ݻ��߰�������
			{			
				StartFeedMotor(FEED_FORWARD);						//�ظ����֮ǰ����λ,���ǲ��ı���λ����,��Ӵ˾��ӡЧ������
				SetTimerCounterWithAutoReload(DealPrtData_TIMER, FEED_START_US);
				StartTimer(DealPrtData_TIMER);	        //2016.06.15 ������ʱ5 �߲�
				HeatPowerOn();
			}	
		}		
	}			
	
// 	//��ӡ�����������ݣ�Ʊ�ݴ�ӡ��־λ��λ 2016.10.25 ȥ��
// 	if(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)
// 	{	
//      g_tSystemCtrl.BillPrtFlag |= 4;                
// 	}
	
	ErrorLedFlash();	//����ָʾ�ƴ���
	PELedFlash();			//ȱָֽʾ�ƴ���
	//2016.06.30  �е����������δȡ�������������ⱨ�� 0.5sһ��
	if(g_tSystemCtrl.CutLedFlag == 1 )		
	{		
		PTKLedAndBuzzerFlash();
		
	}	
	else
	{
		if(g_tSpeaker.SpeakPEnable == 0)	//��ȱֽʱ�����������������־λ
			g_tSpeaker.SpeakFlag = 0;
  }
	
	
	#ifdef	ERROR_EMERGENCY_PROTECT_ENABLE
		if (!ErrorDetect())
		{
			if (g_bFeedTaskFlag)
			{
				ReleaseEmergencyProtect();
				g_bFeedTaskFlag = 0;
			}
		}
	#endif
	
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
		PaperOutDetect();
	#endif
}

/*******************************************************************************
* Function Name  : ������DrvBuzzerFun
* Description    : ���Ʒ�����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	DrvBuzzerFun(void)
{
	static uint16_t	BuzzerStatus = 0;
	static uint16_t	BuzzerTemp = 0;
	
	if(BuzzerRunFlag == 1)
	{	
		 if(BuzzerNum!=0)
		 {
			 if(BuzzerStatus == 0)
			 {
					BeepOn();
			    BuzzerTemp++;
				  if(BuzzerTemp == BuzzerTime)
					{
						BuzzerTemp = 0;
						BuzzerStatus = 1;
					}	
			 }
       else
       {
					BeepOff();
					BuzzerTemp++;
					if(BuzzerTemp == BuzzerTime)
					{
						BuzzerTemp = 0;
						BuzzerStatus = 0;
						BuzzerNum--;
					}	
			 }				 			  
		 }	 
		 else
		 {
			 BeepOff();    	
			 BuzzerTemp = 0;
			 BuzzerStatus = 0;
			 BuzzerTime = 0;
			 BuzzerRunFlag = 0;
		 }	 	
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʱ��������,������ȿ��ƶ�ʱ
* Input          : �������
* Output         : �������
* Return         : ���ز���
* ���Ⱥ���ֽ�ϲ����д������˵����
* �����ж���ֽ��ʼʱ��g_tHeatCtrl.HeatEnable = 0,g_tSystemCtrl.StepNumber > 0
* 			  ������: g_tSystemCtrl.StepNumber = 0
* ���Ȳ���ֽ��ʼʱ	: g_tHeatCtrl.HeatEnable = 1,g_tSystemCtrl.StepNumber > 0
*			  ������: g_tHeatCtrl.HeatEnable = 0,g_tSystemCtrl.StepNumber = 0	
* ����Ϊ����н��д���,ǰһ�������֮ǰ,�������ò�������һ����,Ҳ���ܸı������ֽ��ȫ�ֱ���
* ����������һ��һ����
* ����һ���з�Ϊ������,ÿ�����εĹ������������һ��,���Σ����6�Σ����ȱ����ε�һ����.
*  ����Ϊÿ��һ����,�ٰ���g_HeatUs�ļ���ʱ�����g_tHeatCtrl.HeaterGroupNumberCpy��.
*  ÿһ�����ܵļ���ʱ��Ϊg_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy
*  ��һ�������ʱ�䰴�ռ��ٱ�g_tFeedMotorRushTable[]��ѡ��,ѡ��ԭ��Ϊ���һ����ʱ�䲻����
*	һ�����ܵļ���ʱ��.
*  ����ļ��ٱ�����־g_StepCount�ڵ�һ�ν����ж�֮ǰ,������һ���м��ȵĳ���ȷ��.
*  ����ʱ��g_HeatUs������һ���м��ȵĳ���ȷ��.
*  �����һ�������ʱ�䳬��һ�����ܵļ���ʱ��,�����������ߵ��������Ϊ����ʱ,Ȼ�����.
*   ����ÿһ�����֮��Ҫ�ж��Ƿ���Ҫ���ÿ���ʱ,�����Ҫ������һ����־����,
*	��ʾ����ʱ�Ѿ����ù���.
*  �ڼ�����һ�����к󽫿���ʱ��־����,׼������һ��������.
�ߵ��ͬʱ���Ȳ��ֵ��жϴ���ʱ�����̣�
	��ʼ��һ��������˺�����ת���У��رռ���
			���õ������ʱ(����ʱ������ֻ�ߵ��������)���رռ���
					��ʼ���ȷ���1��(����ʱ�������������ж�ǰ������Ϊ�Զ��ظ�����)
						 ��ʼ���ȷ���2��
									....
											������������ϣ���ʱ���Ҳ�ߵ�λ���رռ��ȣ���ձ�����ϡ�
*******************************************************************************/
#ifdef	DBG_HEAT_INT_COUNT
volatile uint32_t	iDbgHeatTotalCount	= 0;
volatile uint32_t	iDbgHeatStartCount	= 0;
volatile uint32_t	iDbgHeatEndCount		= 0;
#endif
#define		INT_FOR_MOTOR				(0)
#define		INT_FOR_HEAT				(1)
#define		INT_FOR_OVER				(3)		//2016.06.14
/* ������ߵ�������� */
void	ProcessHeat(void)
{
	static uint8_t	mHeatCount = 0;	//ͳ��һ���м��ȴ���,ÿִ��һ��һ���������ֵ����1,�����������
	static uint8_t	mIntFlag = INT_FOR_MOTOR;	//��־���뱾���ж���Ϊ�˿����ߵ������Ϊ�˿��Ƽ���
	volatile uint16_t	HeatPins;
	uint16_t	Time;
	uint32_t	LPBStatus;
	
	EndHeat();	//�ؼ��ȿ���
	if (mIntFlag == INT_FOR_MOTOR)	//�����ߵ��
	{
		if (g_bMotorRunFlag == 0)	//�����������ھ�ֹ״̬,��׼������
		{
			StartFeedMotor(FEED_FORWARD);	//�ظ����֮ǰ����λ,���ǲ��ı���λ����,��Ӵ˾��ӡЧ������
			TIM_SetCounter(FEED_TIMER, FEED_START_US);
		}	//�����ֹ�������
		else	//��������˶���
		{
			StepFeedMotor(FEED_FORWARD);
			if (g_StepCount > (MAX_RUSH_STEPS - 1))	//�������ͳ���Ѿ��������ٱ�
			{
				Time = g_tFeedMotorRushTable[MAX_RUSH_STEPS - 1];
			}
			else
			{
				Time = g_tFeedMotorRushTable[g_StepCount];
				g_StepCount++;
			}
			TIM_SetCounter(FEED_TIMER, (Time - g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy));	//���ÿ���ʱ
			
			mIntFlag = INT_FOR_HEAT;	//������һ���ж��ֽ����ߵ�����޷��������,�˴�Ϊ1��һ���г���
		}
		
		return;
	}
	else if (mIntFlag == INT_FOR_HEAT)	//�������
	{
		if (mHeatCount < (STEP_NUMBERS_PER_DOT_HEIGHT * g_tHeatCtrl.HeaterGroupNumberCpy))	//����δ���
		{
			HeatPins = g_tHeatCtrl.HeaterGroupElementCpy[mHeatCount % g_tHeatCtrl.HeaterGroupNumberCpy];
			StartHeat(HeatPins);	//���ȶ�ʱ���Զ���װ�ؼ������趨ֵ  2016.06.14
			mHeatCount++;
		}
		else	//���������
		{
			EndHeat();															//�ؼ��ȿ���
			g_tHeatCtrl.HeatEnable = HEAT_DISABLE;	//������ɱ�־
			mHeatCount = 0;
			mIntFlag = INT_FOR_MOTOR;
			
			LPBStatus = GetPrintStatus();
			if ((LPBStatus == LPB_STATUS_SPI_SEND_OVER) || (LPBStatus == LPB_STATUS_SET_GO_LINE_SPACE))
			{
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}
		
		return;
	}
	else	//��ֹ����ʱ
	{
		//EndHeat();															//�ؼ��ȿ���
		g_tHeatCtrl.HeatEnable = HEAT_DISABLE;	//������ɱ�־
		mHeatCount = 0;
		mIntFlag = INT_FOR_MOTOR;
	}
}

#ifdef	DBG_MOTOR_STOP_COUNT
	uint32_t	giStopCount = 0;	//��ӡ�е��ֹͣ������
#endif

#define		FEED_MOTOR_ILDE_NUMBER		(2)  //2016.06.14 2016.06.24 20��Ϊ1
void	ProcessMotor(void)
{
	static	uint8_t	IldeCount = 0;		//�����������������ּ����ж�
	static	uint8_t	StopHoldFlag = 0;	//���������ס����
// 	static	uint32_t	StepCount = 0;		//ͳ���ߵ������
	uint32_t	LPBStatus;
	
	if (FeedStepCount < g_tSystemCtrl.StepNumber)	//��ֽδ���
	{
		IldeCount = 0;				//��ֽ������
		if (g_bMotorRunFlag == 0)		//�����������ھ�ֹ״̬,��׼������
		{	//�ظ����֮ǰ����λ,���ǲ��ı���λ����,��Ӵ˾��ӡЧ������
			StartFeedMotor(FEED_FORWARD);
			SetTimerCounterWithAutoReload(FEED_TIMER, FEED_START_US);	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
		}		//�����ֹ�������
		else	//��������˶���
		{
			StepFeedMotor(FEED_FORWARD);
			FeedStepCount++;
			if (g_StepCount < (MAX_RUSH_STEPS - 1))	//���ٽ׶���ʱʱ��
			{	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
				SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[g_StepCount]);
				g_StepCount++;
			}
			else	//���ٽ׶���ʱʱ��
			{	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
				SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[(MAX_RUSH_STEPS - 1)]);
			}
		}
	}		//��ֽδ��ɴ������
	else	//��ֽ��ɵȴ�һ��ʱ�䴦��ʼ
	{
		if (IldeCount == 0)
		{
			FeedStepCount = 0;
			g_tSystemCtrl.StepNumber = 0;
			
		  SetTimerCounterWithAutoReload(FEED_TIMER, g_tFeedMotorRushTable[g_StepCount]); //2016.06.14
			
			LPBStatus = GetPrintStatus();
			if ((LPBStatus == LPB_STATUS_SPI_SEND_OVER) || (LPBStatus == LPB_STATUS_SET_GO_LINE_SPACE))
			{
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}
		
		IldeCount++;
		if (IldeCount > FEED_MOTOR_ILDE_NUMBER)
		{
			if (StopHoldFlag == 0)		//��ӽ�����ס����
			{
				StopFeedMotor(FEED_FORWARD);
				SetTimerCounterWithAutoReload(FEED_TIMER, FEED_STOP_US);	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
				StopHoldFlag = 1;
			}
			else
			{
				IldeCount = 0;
				StopHoldFlag = 0;
				EndFeedMotor();								//�رյ��
				TIM_Cmd(FEED_TIMER, DISABLE);	//�رյ�����ƶ�ʱ��
				#ifdef	DBG_MOTOR_STOP_COUNT
					giStopCount++;
				#endif
			}
		}
	}	//��ֽ��ɵȴ�һ��ʱ�䴦�����
}


/*******************************************************************************
* Function Name  : ������T3HeatLineFunc  2016.06.16  T3��������
* Description    : ��ʱ��������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	T3HeatLineFunc(void)
{
	u8 static T3Cntr = 0;
	
	T3_over=1;			//���ü��ȱ�־
	T3Cntr++;
	if(T3Cntr ==1)
	{
		//���ü���ʱ��
		SetTimerCounterWithAutoReload(FEED_TIMER, g_HeatUs);	    //���ü���ʱ��
		StartTimer(FEED_TIMER);
		//������һ�μ���
		if(	g_tHeatCtrl.HeaterGroupNumber ==2 )										//��Ҫ���μ��ȣ�
		{
			StartHeatPartOne();
			StopHeatPartTwo();
		}
		else															//��Ҫһ�μ���
		{
			StartHeatAll();
			T3Cntr++;
		}
	}
	else if(T3Cntr ==2)									//�ڶ��μ���
	{
		StartHeatPartTwo();
		StopHeatPartOne();
	}
	else												//��������
	{
		StopHeatAll();
		TIM_Cmd(TIM3, DISABLE);
		T3Cntr =0;
		T3_over=0;		//2016.07.10
		HeatOver =0;
	}
}

//���º���Ϊ��һ�������д����ж���ֽ���ߵ��
void SystemTimer3Func(void)
{	
	  T3HeatLineFunc();
}
/*******************************************************************************
* Function Name  : ������SystemT5FuncDealWithPrtData  2016.06.16  T6��������
* Description    : ��ʱ��������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	SystemT5FuncDealWithPrtData(void)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	i;
	
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint8_t	Width, Number;
	uint8_t	* pSrc = NULL;
  uint16_t DotLine;
	uint16_t Temp1;
	
	if(g_bMotorRunFlag == 0)
	{	
		EndFeedMotor();							            	//�رյ��
		TIM_Cmd(DealPrtData_TIMER, DISABLE);			//�رյ�����ƶ�ʱ��		
		return;
	}
		
	if ((U_PrtBufCtrl.IdxWR == U_PrtBufCtrl.IdxRD && g_tFeedButton.KeyDownFlag == 0 )	         //û��������û��������
		|| (g_tError.PaperSensorStatusFlag & 0x01) 			                                         //����ֽ��ȱֽ
		|| (g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag) )	//�д��� 2016.08.04
	{		 
		 StopFeedMotor(FEED_FORWARD);
		 g_bMotorRunFlag = 0;
		 SetTimerCounterWithAutoReload(DealPrtData_TIMER, FEED_STOP_US);	          //ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
		 StartTimer(DealPrtData_TIMER);
	}
  else	
	{
// 		while(T3_over==1);          //2016.07.10
		StepFeedMotor(FEED_FORWARD);
		if(g_tFeedButton.KeyDownFlag == 1)
		{
			if (g_StepCount < g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode])	//ȡ�����
			{
				g_StepCount++;
			}
		}	
		else
		{	
			//�Ӽ��ٲ�������
			
			Temp1 =U_PrtBufCtrl.IdxWR;	

			if(Temp1 >=U_PrtBufCtrl.IdxRD )
			{	
				DotLine =(Temp1 - U_PrtBufCtrl.IdxRD);
			}	
			else
			{	
				DotLine =(DrvPrtBufDeep - U_PrtBufCtrl.IdxRD + Temp1);
			}
			if(MotoFeedInc==1)								            //���ٴ���	1-����, 2-����, 3-����
			{
				if (g_StepCount > DotLine)	  //����ǰ�������ڵ��ڻ���߶ȣ�תΪ���٣��������	
				{	
					MotoFeedInc = 3;		
				}	
				else
				{	
					g_StepCount++;
				}	
			}
			else if (MotoFeedInc==2)								
			{
				if (g_StepCount < DotLine)	   //����ǰ����С�ڻ���߶ȣ�תΪ���٣��������	
				{	
					MotoFeedInc = 3;
				}	
				else;
				{
					g_StepCount--;
				}	
			}
			else if (MotoFeedInc==3)						//����			
			{
				if (g_StepCount >= DotLine)	//����ǰ�������ڵ��ڻ���߶ȣ�תΪ����		
				{
					MotoFeedInc = 2;
				}
				else if (g_StepCount < DotLine)	//����ǰ����С�ڻ���߶ȣ�ͬʱС������٣���תΪ����
				{
					MotoFeedInc = 1;
				}
			}
			else
			{
				MotoFeedInc = 3;
			}
			if (g_StepCount > g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode])	//ȡ�����
			{
				g_StepCount = g_tSystemCtrl.MaxStepPrtNum[g_tPrtCtrlData.PCDriverMode];
			}
  	}	
	  //ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
		SetTimerCounterWithAutoReload(DealPrtData_TIMER, g_tFeedMotorRushTable[g_StepCount]);
		StartTimer(DealPrtData_TIMER);
    //��������
		if(g_tFeedButton.KeyDownFlag ==1)	  					                              //���Ȱ�����ֽ
		{
			if(g_tFeedButton.StepCount >0)
			{
				g_tFeedButton.StepCount--;
				if(g_tFeedButton.StepCount ==0)
				{	
					g_tFeedButton.KeyDownFlag =0;		
				}	
			}
			return;
		}
		//��ӡ����
		if ((U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].ActiveDots) != 0x00)
		{	
			HeatTime = GetHeatTime();			//�������ʱ��
			//SetHeatNumber();						//���㱾���м��ȴ���,���ü��ȿ��ƹܽ�
			//������ȴ���
			if (U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].ActiveDots <= MAX_HEAT_DOT_ONE_TIME)			//��������Ͳ��������һ�μ��ȵ���,һ�μ���
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
	
			g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//��ֵ���Ʊ���
			g_tHeatCtrl.HeaterGroupNumber = 0;
			for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
			{
				g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
				g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
			}
	
			g_HeatUs = HeatTime;
			totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//һ�����ܼ���ʱ��
  		//AdjustMotorStep(totalheatus);					//��������ʱ,�������ٲ���
			while (1)		//2016.06.14 ��֤����߲�ʱ��Ҫ���ڼ���ʱ��
			{
				if(g_tFeedMotorRushTable[g_StepCount] > (totalheatus+200))
				{
					break;
				}			
				g_StepCount--;
			}			
			SetTimerCounterWithAutoReload(DealPrtData_TIMER, g_tFeedMotorRushTable[g_StepCount]);
		  StartTimer(DealPrtData_TIMER);
			
		  g_bPrtSPIDMAIntForDriveMode = 1;
			sHeat_StartDMATx(U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxRD].Buf);	//������оSPI����
		}
// 		U_PrtBufCtrl.BufHeight--;								  	//�и߼�����-1 2016.07.10
		U_PrtBufCtrl.IdxRD++;										    //�����е�ַ
		if (U_PrtBufCtrl.IdxRD >=DrvPrtBufDeep)			//����ӡ�߶ȵ�����8����24�㣩= 192  ������ramʱ �˴���Ҫ����
		{	
			U_PrtBufCtrl.IdxRD = 0;
		}	
	}
}
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʱ��������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	SystemTimer5Func(void)
{
	
}

/*******************************************************************************
* Function Name  : ������
* Description    : ָʾ�ƴ�������
	ָʾ����˸��ʽ˵���� ������������������ʱ��
	POW��Դ���ϵ糤��
	ERROR����ָʾ�� ��˸Ƶ��
	���ɻָ�����  ������200ms����200msΪON��200msΪOFF
	�ɻָ�����    ������800ms����800msΪON��800msΪOFF
	���Զ��ָ����� ���� 
	���� ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	ErrorLedFlash(void)
{
	static uint16_t	LedTimeCount = 0;	//����ʱ��,�����Ѿ�Ϊ�߻��ߵ͵�ά��ʱ��,ms
	static uint8_t	LedStatus = LED_STATUS_OFF;	//LED�Ƶ�״̬��=0ΪϨ��=1Ϊ����
	
	if (g_tError.UR_ErrorFlag)	//�в��ɻָ�����,ָ�����˸���������죬200msΪ���
	{  
		if (LedStatus == LED_STATUS_OFF)		//�ȴ�����
		{
			ErrorLedOff();
			DriveBuzzer(DISABLE);
			
			LedTimeCount++;			//ͳ�����ʱ��
			if (LedTimeCount >= LED_FAST_FLASH_MS)	//�ﵽ��Ϊ��
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_ON;
			}
		}
		else if (LedStatus == LED_STATUS_ON)	//�ٴ�����
		{
			ErrorLedOn();
			if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
			{
				DriveBuzzer(ENABLE);
			}
			
			LedTimeCount++;			//ͳ������ʱ��
			if (LedTimeCount >= LED_FAST_FLASH_MS)	//�ﵽ��Ϊ��
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_OFF;
			}
		}
		else
		{
		}
	}		//���ɻָ����������
	else if (g_tError.R_ErrorFlag)	      //�пɻָ�������,ָ�����˸���������죬800msΪ���
	{
		if (LedStatus == LED_STATUS_OFF)		//�ȴ�����
		{
			ErrorLedOff();
			DriveBuzzer(DISABLE);
			
			LedTimeCount++;			//ͳ�����ʱ��
			if (LedTimeCount >= LED_SLOW_FLASH_MS)	//�ﵽ��Ϊ��
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_ON;
			}
		}
		else if (LedStatus == LED_STATUS_ON)	    //�ٴ�����
		{
			ErrorLedOn();
			if (g_tSysConfig.BuzzerEnable & 0x01)	  //����ʱ����
			{
				DriveBuzzer(ENABLE);
			}
			
			LedTimeCount++;			//ͳ������ʱ��
			if (LedTimeCount >= LED_SLOW_FLASH_MS)	//�ﵽ��Ϊ��
			{
				LedTimeCount = 0;
				LedStatus = LED_STATUS_OFF;
			}
		}
		else
		{
		}
	}		//�ɻָ�������ʾ�������
	else if (g_tError.DoorOpen)	           //���Զ��ָ�������ʼ,ָʾ�Ƴ��� 2016.08.04
	{
		if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
		{		
 			 	if (LedStatus == LED_STATUS_OFF)		//�ȴ����� 2017.05.22
				{
					ErrorLedOff(); 
					DriveBuzzer(DISABLE);					
					LedTimeCount++;			//ͳ�����ʱ��
					if (LedTimeCount >= LED_SLOW_DOOR_MS)	//�ﵽ��Ϊ��
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_ON;
					}
				}
				else if (LedStatus == LED_STATUS_ON)	//�ٴ�����
				{
					if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
					{
						DriveBuzzer(ENABLE);
					}
					ErrorLedOn();
					LedTimeCount++;			//ͳ������ʱ��
					if (LedTimeCount >= LED_FAST_PE_MS)	//�ﵽ��Ϊ��
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_OFF;
					}
				}
				else
				{
				}
		}	
	}	
	else if (g_tError.AR_ErrorFlag)	        //���Զ��ָ�������ʼ,ָʾ�Ƴ���
	{
		ErrorLedOn();
		if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
		{
 			//DriveBuzzer(ENABLE);							//2016.06.19 ���ָ����� �ϸǴ򿪺͹��� ����������
		}
	}		//���Զ��ָ����������
	else	//û���������ִ�����ʼ,�ش���ָʾ��,�ط�����
	{
		LedTimeCount = 0;		//������ܵļ����м�ֵ
		ErrorLedOff();

		if (g_tError.PaperSensorStatusFlag &0x01 ==0)		//��ȱֽ����
		{
			if (g_tSysConfig.BuzzerEnable & 0x01)	//ȱֽ,����ʱ����
			{
				if(g_tSystemCtrl.CutLedFlag == 0)	//2016.07.19  û��δȡ��ʱ�ر�
				{	
					if(BuzzerRunFlag == 0)	//2016.09.14 ��ֹ�Է�����ָ���Ӱ��
					{	
						DriveBuzzer(DISABLE);
					}	
				}	
			}
		}	//�ط���������
	}		//û���������ִ��������,��������в�����ȱֽ�ź�,ֻ���޴���رշ�����ǰ���ȱֽ
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȱָֽʾ�ƴ�����
	ָʾ����˸��ʽ˵����
	ȱֽ�ƣ�PAPER��״̬
	ȱֽ    ȱֽ�Ƴ���  ����������(���ܿ�����)
	ֽ����  ȱֽ������  ��800ms��
	δȡֽ  ȱֽ�ƿ���  ��200ms�� 
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	PELedFlash(void)
{
	static uint16_t	LedTimeCount = 0;	//����ʱ��,�����Ѿ�Ϊ�߻��ߵ͵�ά��ʱ��,ms
	static uint8_t	LedStatus = LED_STATUS_OFF;	//LED�Ƶ�״̬��=0ΪϨ��=1Ϊ����
	static uint8_t	PEAlarmNum = 0;	    //ȱֽ��������������
	static uint8_t	PEAlarmFlag = 0;
	
	if(g_tError.DoorOpen)
	{
		return;
	}	
	//ȱֽ����,λ����,����Ч,bit.0=ȱֽ;bit.1= ֽ����
	if (g_tMacro.RunFlag == 0)			//��ִ�к궨��
	{
		if (((g_tError.PaperSensorStatusFlag & 0x01) == 0x01)	&& (g_tError.DoorOpen == 0))	//ȱֽ ���Һϸ�
		{
			//ȱֽ�Ƴ���  �������������
			PaperEndLedOn();
			
			if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
			{	
					if(g_tSpeaker.SpeakCtrl == 1)
					{
						if((g_tSpeaker.SpeakFlag == 0) && (g_tSpeaker.SpeakDoorOpen == 0))
						{
							g_tSpeaker.SpeakPEnable = 1;
				//			Line_2A_WTN5_Tim(1);
	            Line_2A_WTN588H_Tim(g_tSysConfig.POutVoiceNumber);
						}
					}
				
				if (LedStatus == LED_STATUS_OFF)		//�ȴ�����
				{
					DriveBuzzer(DISABLE);					
					LedTimeCount++;			//ͳ�����ʱ��
					if (LedTimeCount >= LED_FAST_PE_MS)	//�ﵽ��Ϊ��
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_ON;
					}
				}
				else if (LedStatus == LED_STATUS_ON)	//�ٴ�����
				{
					if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
					{
						if(g_tSpeaker.SpeakCtrl == 0)
						DriveBuzzer(ENABLE);
					}
					
					LedTimeCount++;			//ͳ������ʱ��
					if (LedTimeCount >= LED_FAST_PE_MS)	//�ﵽ��Ϊ��
					{
						LedTimeCount = 0;
						LedStatus = LED_STATUS_OFF;
					}
				}
				else
				{
				}
			}
			else if(g_tSysConfig.BuzzerEnable == 0)	//2016.07.15 ������������ʱ ȱֽҲ��������
			{
				if(PEAlarmNum < 0x03 && PEAlarmFlag == 0)  //��⵽����δȡֽ
	      {			
					if (LedStatus == LED_STATUS_OFF)		     //�ȴ�����
					{
						BeepOn();					
						LedTimeCount++;			//ͳ�����ʱ��
						if (LedTimeCount >= LED_DELAY_PE_MS)	   //�ﵽ��
						{
							LedTimeCount = 0;
							LedStatus = LED_STATUS_ON;
						}
					}
					else if (LedStatus == LED_STATUS_ON)	   
					{
						BeepOff();  	                          
						LedTimeCount++;			                      //ͳ��ʱ��
						if (LedTimeCount >= LED_DELAY_PE_MS)	  //�ﵽ��Ϊ��
						{
							LedTimeCount = 0;
							LedStatus = LED_STATUS_OFF;
							PEAlarmNum++;
						}
					}
				}
				else		//���ݱ�ȡ��
				{
					BeepOff();
					PEAlarmFlag = 1;
					PEAlarmNum = 0;
				}	
			}			
			return;
		}		//ȱֽ�������
		else if ((g_tError.PaperSensorStatusFlag & 0x02) == 0x02)	//ֽ����
		{
			//ֽ����  ȱֽ������  ��800ms��
			if (LedStatus == LED_STATUS_OFF)		//�ȴ�����
			{
				PaperEndLedOff();
				DriveBuzzer(DISABLE);
				
				LedTimeCount++;			//ͳ�����ʱ��
				if (LedTimeCount >= LED_SLOW_FLASH_MS)	//�ﵽ��Ϊ��
				{
					LedTimeCount = 0;
					LedStatus = LED_STATUS_ON;
				}
			}
			else if (LedStatus == LED_STATUS_ON)	//�ٴ�����
			{
				PaperEndLedOn();
				if (g_tSysConfig.BuzzerEnable & 0x01)	//����ʱ����
				{
					DriveBuzzer(ENABLE);
				}
				
				LedTimeCount++;			//ͳ������ʱ��
				if (LedTimeCount >= LED_SLOW_FLASH_MS)	//�ﵽ��Ϊ��
				{
					LedTimeCount = 0;
					LedStatus = LED_STATUS_OFF;
				}
			}
			else
			{
			}
			
			return;
		}		//ֽ�����������
		else	//���ϸ��������δ���֣���һ������ʱ
		{
			PEAlarmFlag = 0;  //��ȱֽ �������־
			LedTimeCount = 0;
			PaperEndLedOff();
			g_tSpeaker.SpeakPEnable = 0;
			if ((g_tError.UR_ErrorFlag == 0) && \
					(g_tError.R_ErrorFlag  == 0) && \
					(g_tError.AR_ErrorFlag == 0))
			{
				if(g_tSystemCtrl.CutLedFlag == 0 && (g_tError.DoorOpen == 0))	//2016.07.19  û��δȡ��ʱ�ر�   //2017.05.23  ���ӿ��ŵ��ж�
				{	
					if(BuzzerRunFlag == 0)	//2016.09.14 ��ֹ�Է�����ָ���Ӱ��
					{		
						DriveBuzzer(DISABLE);	//û�������ĸ��ִ���ʱҲ�رշ�����
					}	
				}
			}
		}	//�޸���ȱֽ����ʱ�������
	}		//����ֽ�ż�⴦�����
}
/*******************************************************************************
* Function Name  : ������CheckCoverStatus
* Description    : ����ϸ�״̬
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void CheckCoverStatus(void)
{
	static uint16_t TempNum = 0;
	if(++TempNum >=10)	
	{
		if (ReadCoverStatus() == PLATEN_STATUS_OPEN)	//2016.08.04
		{
			TempNum = 0;
			g_tError.DoorOpen = 1;	
			g_tSpeaker.SpeakDoorOpen = 1;
		}
		else
		{
			if(g_tError.DoorOpen == 1)
			{    
				if(TempNum>200)
				{    
					TempNum = 0;
					g_tError.DoorOpen = 0;
					g_tSpeaker.SpeakDoorOpen = 0;
					//if(TempNum>250)                                           //�ϸǺ��adʱ��������ֽ״̬
					{    
						 if(g_tSystemCtrl.PaperType == PAPER_TYPE_CONTINUOUS )  //����ֽ  && ((g_tError.PaperSensorStatusFlag & 0x01) == 0)
						 {
								//��ֹճֽ ��ֽn����
								g_tFeedButton.KeyDownFlag = 1;			//2016.06.28 �������±�־ ��λ
								g_tFeedButton.StepCount = 24;
						 }   
						 else if(g_tSystemCtrl.PaperType == PAPER_TYPE_LABLE && ((g_tError.PaperSensorStatusFlag & 0x01) == 0))  //��ǩֽ
						 {
								FeedToGapFlag = 1;                                      //�ϸ��Զ���ֽ�ұ�ǩ��־λ
								AutoFeedPaper = 0;
						 } 
						 else if(g_tSystemCtrl.PaperType == PAPER_TYPE_BLACKMARK )  //�ڱ�ֽ
						 {
								FeedToGapFlag = 1;  
						 }     		
					}
				}                    
			}    
			else
			{
				TempNum = 0;
			}    
		}
			
		if(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD)				//2016.08.04
		{
			//����ϸǿ���
			if (g_tError.DoorOpen ==1)			
				g_tError.AR_ErrorFlag |= 0x02;	//bit1=1 ��		  
			else
				g_tError.AR_ErrorFlag &= ~0x02;
		}
	}	
}
/*******************************************************************************
* Function Name  : ������PTKLedAndBuzzerFlash
* Description    : δȡ�����ⱨ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	PTKLedAndBuzzerFlash(void)
{
	static uint16_t	PtkCountNum = 0;	                //����ʱ��,�����Ѿ�Ϊ�߻��ߵ͵�ά��ʱ��,ms
	static uint8_t	AlarmStatus = LED_STATUS_OFF;	   //LED�Ƶ�״̬��=0ΪϨ��=1Ϊ����
	static uint16_t	PtkCheckAdNum = 0;
	static uint16_t	FlashCountNum = 0;
	
	if(g_tError.DoorOpen == 1)				//���Ǻ� �ָ��������͵ƴ�״̬ �����
	{
		PtkCountNum = 0;
		AlarmStatus = LED_STATUS_OFF;
		PtkCheckAdNum = 0;
		FlashCountNum = 0;
		g_tSystemCtrl.CutLedFlag = 0;
		BeepOff();		//�ط�����
		LineLedOn();
		return;
	}	
	
	if(g_tSysConfig.PaperTakeAwaySensorEnable == 1)   //����δȡ������ʱ
	{
		if(g_tError.PaperSensorStatusFlag & 0x04)  //��⵽����δȡֽ
		{
			if(g_tSpeaker.SpeakCtrl == 1)
			{
				g_tSpeaker.SpeakEnable = 1;
				if(g_tSpeaker.SpeakFlag == 0)
				{
						if(VoiceOverflag == 0)       //��Ҫ���������־
						{	
							Line_2A_WTN588H_Tim(0xFFF3);
						}	
						else
						{	
							Line_2A_WTN588H_Tim(g_tSysConfig.AlarmNumber);  //2018.06.23 ���ɵ�ѡ��

						}	
				}
			}
			PtkCheckAdNum = 0;
			if (AlarmStatus == LED_STATUS_OFF)		   //�ȴ�����
			{
				BeepOff();		//�ط�����
				LineLedOff();
				PtkCountNum++;			//ͳ�����ʱ��
				if (PtkCountNum >= LED_FAST_FLASH_MS)	 //�ﵽ��Ϊ��
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_ON;
				}
			}
			else if (AlarmStatus == LED_STATUS_ON)	//�ٴ�����
			{
				if(g_tSpeaker.SpeakCtrl == 0)
					BeepOn();  	//��������
				LineLedOn();
				
				PtkCountNum++;			                  //ͳ������ʱ��
				if (PtkCountNum >= LED_PTK_ON_MS)	      //�ﵽ��Ϊ��
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_OFF;
				}
			}
			else
			{
			}
		}
		else		//���ݱ�ȡ��
		{
			LineLedOn();
			PtkCheckAdNum++;
			if(PtkCheckAdNum>500)					//2016.07.13 ����
			{
				g_tSystemCtrl.CutLedFlag = 0;
				PtkCheckAdNum = 0;
				g_tSpeaker.SpeakEnable = 0;
			}		
			
		}	
	}	
	else if(g_tSysConfig.PaperTakeAwaySensorEnable == 2)   	//δ����δȡ������ ����ֽ�� ��������
	{
		if((g_tSpeaker.SpeakCtrl == 1)&&(g_tSpeaker.SpeakFlag == 0)) 
		{
			Line_2A_WTN588H_Tim(g_tSysConfig.AlarmNumber);  //2018.06.23 ���ɵ�ѡ��
		}
		else if(g_tSpeaker.SpeakCtrl == 0)
			g_tSystemCtrl.CutLedFlagBuf1 = 1;
		
		if(FlashCountNum < 0x03)  //��������
		{			
			if (AlarmStatus == LED_STATUS_OFF)		//�ȴ�����
			{
				BeepOff();					//�ط�����;
				LineLedOff();
				PtkCountNum++;			//ͳ�����ʱ��
				if (PtkCountNum >= LED_PTK_OFF_MS)	 //�ﵽ��Ϊ��
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_ON;
				}
			}
			else if (AlarmStatus == LED_STATUS_ON)	//�ٴ�����
			{
				if(g_tSpeaker.SpeakCtrl == 0)
					BeepOn();  	                          //��������;
				LineLedOn();
				
				PtkCountNum++;			                  //ͳ������ʱ��
				if (PtkCountNum >= LED_PTK_ON_MS)	      //�ﵽ��Ϊ��
				{
					PtkCountNum = 0;
					AlarmStatus = LED_STATUS_OFF;
					FlashCountNum++;
				}
			}
		}
		else		//
		{
			LineLedOn();
			FlashCountNum = 0;
			g_tSystemCtrl.CutLedFlagBuf2 = 1;
		}	
		if((g_tSystemCtrl.CutLedFlagBuf2 == 1) && (g_tSystemCtrl.CutLedFlagBuf1 == 1))
		{
				g_tSystemCtrl.CutLedFlag = 0;
			  g_tSystemCtrl.CutLedFlagBuf2 = 0;
				g_tSystemCtrl.CutLedFlagBuf1 = 0;
		}	

	}	
	else
	{
		g_tSystemCtrl.CutLedFlag = 0;
	}	
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
