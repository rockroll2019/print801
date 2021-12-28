/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   �е����е������صĳ���.
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
#include	"cutter.h"
#include	"feedpaper.h"
#include	"led.h"
extern void	DelayUs(uint32_t	delaytime);
extern void	DelayMs(uint32_t	DelayNumber);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	FORWARD_DETECT_STEP1_MAX_MS		(500)		//��תʱ��һ�׶μ����ʱ��
#define	FORWARD_DETECT_STEP2_MAX_MS		(1000)	//��תʱ�ڶ��׶μ����ʱ��
#define	REVERSE_DETECT_MAX_MS					(1000)	//��תʱ�����ʱ��
#define	BRAKE_TIME_MS									(100)		//ɲ��ʱ��

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ���е�������ƹܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitCutPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(CUT_HOME_IN_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= CUT_HOME_IN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;	//���븡��
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
	GPIO_Init(CUT_HOME_IN_GPIO_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(CUT_CTRL_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin			= CUT_ENABLE_GPIO_PIN | \
																		CUT_PHASE_GPIO_PIN | \
																		CUT_MODE_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;		//��Ϊ���
	GPIO_Init(CUT_CTRL_GPIO_PORT, &GPIO_InitStructure);

	CutMotorDisable();	//оƬ����
	CutMotorSetMode();	//ɲ��״̬
}

/*******************************************************************************
* Function Name  : void InitCutPaper
* Description    : ��ʼ���е�������е�����ԭλ���򷵻�ԭλ�������ʱ��Ϊû���е�.
*				    �е���ԭλʱ�����عر�״̬��״̬����Ϊ��
* Input          : None
* Output         : None
* Return         : 
*ע�⣺��ͬ���е�����ʱ�򣬸��ȴ�ʱ����ܲ�ͬ����Ҫ�������
*******************************************************************************/
void	InitCutPaper(void)
{
		uint32_t i;
	
		if (g_tSysConfig.CutType == 0)	    //���е� 2017.03.21
		{
			return;
		}
	
		if(GetCutSwitchStatus())					//���Ϊ��1��������ԭλ���򷵻�ԭλ���糬ʱ������Ϊ���е�
		{
				CutMotorReverse();					//��ת
				for(i=0; i<CUT_STATUS_OFF_TIME; i++)	//�ȴ�����ԭλ
				{
						DelayMs(1);
						if(!GetCutSwitchStatus())		//����ص�ԭλ��������
						{
								DelayMs(1);		//ȥ��
								i++;
								if(!GetCutSwitchStatus())	//����ص�ԭλ��������
									break;
						}
				}

				StopCut();						//ֹͣ
				if(i>= CUT_STATUS_OFF_TIME)		//��300ms��û�л�ԭλ
				{
						CutMotorForward();					//��ת
						for(i=0; i<CUT_STATUS_OFF_TIME; i++)//�ȴ�300ms
						{
								DelayMs(1);
								if(!GetCutSwitchStatus())	//����ص�ԭλ��������
								{
										DelayMs(1);	//ȥ��
										i++;
										if(!GetCutSwitchStatus())//����ص�ԭλ��������
											break;
								}
						}
			
						StopCut();					//ֹͣ
						if(i>=CUT_STATUS_OFF_TIME)	//��300ms��û�л�ԭλ
						{
								CutError();				//�е�����
						}
				}
		}
}

/******************************************************************************
* Name        : CutPaper
* Author      : 
* Version     : V0.01
* Date        : 2016-3��-20��
* Description : ��ֽ
* Others      : ע�⣺��ͬ�ͺ��е�����ʱ��ͬ,������ֻ���JX-3R-21�ͺ�.
*                     ��ֽЧ����ȫ�л��ǰ����ɾ���Ӳ��ʵ��.
* ����ֵ      : �ɹ�����CUTTERSUCESS,ʧ�ܷ���CUTTERERROR
* History     : 
******************************************************************************/
uint32_t CutPaper(uint8_t CutMode)
{
	//uint8_t	status;					//�е�λ���źŽ��
	uint8_t	CutStatus;			//��ֽ���
	uint32_t	i;
	
	#if defined	DBG_DUMMY_CUT
		return	(CUT_PAPER_SUCCESS);
	#endif
	
	if (!((CutMode == CUT_TYPE_PART) || (CutMode == CUT_TYPE_FULL)))
	{
		return	(CUT_PAPER_SUCCESS);
	}
	
	CutStatus = CUT_INIT_ERROR1;
	while (CutStatus)
	{
// 		if (!GetCutSwitchStatus())
// 		{
			CutStatus = CUT_INIT_SUCCESS;
			if (CutMode == 0)					//ȫ��
			{
				CutMotorForward();			//��ת
			}
			else
			{
				CutMotorForward();			//��ת
			}
			//2017.04.22  ��ʼ������ǰ 70 ms�������е�״̬����ֹ΢�����ض�����ɵ�Ӱ��
			for (i = 0; i < 70; i++)	
			{
				DelayMs(1);
			}
			
			//�ȴ��е�״̬���ش򿪣��е��뿪ԭλ
			for (; i < CUT_STATUS_ON_TIME; i++)	
			{
				DelayMs(1);
				if (GetCutSwitchStatus())		//�뿪ԭλ��������
				{
					DelayMs(1);				//ȥ��
					i++;
					if(GetCutSwitchStatus())		//�뿪ԭλ��������
					{
						break;
					}
				}
			}
			//���û�г�ʱ�������
			if (i < CUT_STATUS_ON_TIME)
			{
				//�ȴ��е�״̬���عرգ��е�����ԭλ
				for (i = 0; i < CUT_STATUS_OFF_TIME; i++)
				{
					DelayMs(1);
					if (!GetCutSwitchStatus())
					{
						DelayMs(1);			//ȥ��
						i++;
						if (!GetCutSwitchStatus())
						{
							break;
						}
					}
				}
				StopCut();
				
				//�����ʱ���е����󣬽����е��˻ز���
				if (i >= CUT_STATUS_OFF_TIME)			
				{
					if (CutMode == 0)			//ȫ��
					{
						CutMotorReverse();	//��ת
					}
					else
					{
						CutMotorReverse();	//��ת  2017.03.21
						//CutMotorForward();	//��ת
					}
					
					//�ȴ��е�״̬���عرգ��е�����ԭλ
					for (i = 0; i < CUT_STATUS_ON_TIME; i++)	
					{
						DelayMs(1);
						if (!GetCutSwitchStatus())
						{
							DelayMs(1);		//ȥ��
							i++;
							if (!GetCutSwitchStatus())
							{
								break;
							}
						}
					}
					
					CutStatus = CUT_INIT_ERROR2;	//����״̬2���м����
				}
			}
			else
			{
				CutStatus = CUT_INIT_ERROR1;		//����״̬1����ʼ����
			}
			
			if (g_tSystemCtrl.CutRunFlag)
			{
				g_tSystemCtrl.CutRunFlag = 0;
				DriveBuzzer(DISABLE);			//�رշ�����
			}
			
			if (CutStatus != CUT_PAPER_SUCCESS)     //2016.11.24
			{
				CutError();					//�е�������
				if(g_tError.R_ErrorRecoversFlag !=0)
				{
					//�е�����ָ���ʽ��100502ʱ������ִ����ֽ����
					if(g_tError.R_ErrorRecoversFlag ==2)
						CutStatus =CUT_INIT_SUCCESS;					
					g_tError.R_ErrorRecoversFlag =0;
				}
			}
			else
			{
				g_tSystemCtrl.CutLedFlag = 1;       //�е����б�־ ����ָʾT2ָʾδȡ�������ⱨ��
				g_tSystemCtrl.BillPrtFlag |= 8;     //�е���ɣ���ʾ��ǰƱ�ݴ�ӡ���

				//2017.08.14  �е������� ���ɱ���
				VoiceOverflag = 0;
				
				#ifdef	MAINTAIN_ENABLE
					 R_Counter.CutCounter++;					//�е�������
					 C_Counter.CutCounter++;
				#endif
			}	
	
// 		}
// 		else
// 		{
// 			InitCutPaper();						//��ʼ���е�
// 		}
	}
	
	return	(CutStatus);
}

/*******************************************************************************
* Function Name  : void DriveCutPaper
* Description    : �����е� 
* Input          : CutMode:��ֽģʽ����0 ȫ�У���1 ����
* Output         : None
* Return         : 
*******************************************************************************/
void	DriveCutPaper(uint8_t CutMode)
{
	uint16_t CutStatus;
	uint16_t i;
	
 	if (g_tSysConfig.CutType == 0)	    //���е�
	{
// 		GoDotLine(NO_CUT_OFFSET);		     //�ߵ���ֽλ�� 2017.03.21
		return;
	}
	
	CutStatus = CUT_PAPER_SUCCESS;
	
	while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);			 //�ȴ����ֹͣ,û�а������£��д�ӡ������û������
	
	if (g_tSysConfig.BuzzerEnable & 0x02)
	{
		g_tSystemCtrl.CutRunFlag = 1;
		if(g_tSpeaker.SpeakCtrl == 0)
			DriveBuzzer(ENABLE);			        //����������
	}
	
#ifdef	CUTTER_ENABLE	
	CutStatus = CutPaper(CutMode);
#endif	

}

/*******************************************************************************
* Function Name  : void CutError
* Description    : �е�����,�ȴ��е�����ԭλ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	CutError(void)
{
	StopCut();
	g_tError.R_ErrorFlag |= 0x01;		//�������е�λ�ô���
	
	//�ϸǴ򿪺��ٹرջ�1005nָ��ָ��е�����
	while((g_tError.R_ErrorFlag &0x01))
	{
		while( g_tError.DoorOpen ==0)
		{
			if((g_tError.R_ErrorFlag &0x01) == 0x00)			//1005ָ��ָ��е�����
			{	
				break;
			}	
		}
		while( (g_tError.R_ErrorFlag &0x01))				  	//�����ںϸǺ�ȡ������
		{
			if( g_tError.DoorOpen ==0)
			{	
				g_tError.R_ErrorFlag &= ~0x01;	
			}	
		}
	}	
}

/******************************************************************************
* Name        : CutMotorx
* Author      : 
* Version     : V0.01
* Date        : 2012-5��-17��
* Description : �����е���������ֶ���,ǰת,��ת,ɲ��,ֹͣ��
* Others      : ע�⣺  ���ĸ�����ֻ�Ǹı�������Ϊ,����ʱ��ȿ���,
*                       ʵ��ʹ��ʱһ��Ҫע����Ӧ������ʱ������Ա������
*                       ���ĸ�����ֻ����InitCut()��CutPaper()����
* History     : 
******************************************************************************/
static void CutMotorForward(void)		//�����ת
{
	//CutMotorSleepOff();		//��ֹ����
	CutMotorSetPhase();		//������ת����
	CutMotorEnable();			//�������
}

static void CutMotorReverse(void)		//�����ת
{
	//CutMotorSleepOff();		//��ֹ����
	CutMotorResetPhase();	//������ת����
	CutMotorEnable();			//�������
}

static void CutMotorBrake(void)			//���ɲ��,��˥��
{
	//CutMotorSleepOff();		//��ֹ��?
	//CutMotorSetMode1();		//MODE1 = H
	//CutMotorResetMode2();	//MODE2 = L
	CutMotorSetMode();		//MODE1 = H
	CutMotorDisable();		//��ֹ���
}

void CutMotorStandby(void)		//���ֹͣ
{
	//CutMotorSleepOn();		//��������
	CutMotorSetMode();		//MODE1 = H
	CutMotorDisable();		//��ֹ���
}

/*******************************************************************************
* Function Name  : void StopCut
* Description    : ֹͣ�е� 
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void StopCut(void)
{
	CutMotorStandby();	//ֹͣ
	DelayUs(1000);
 	CutMotorBrake();		//ɲ��
	DelayUs(150000);
	CutMotorStandby();	//ֹͣ
}

/******************************************************************************
* Name        : GetCutSwitchStatus
* Author      : 
* Version     : V0.01
* Date        : 2012-5��-17��
* Description : ��ȡ�е�������ش����������ֵ,ֻ������״̬��
* Output      : ����CUTTERON����CUTTEROFF,0Ϊ�պϣ�1Ϊ��
* Others      : 
* History     : 
******************************************************************************/
static uint8_t	GetCutSwitchStatus(void)
{
	uint8_t	Status;
	
	Status = GPIO_ReadInputDataBit(CUT_HOME_IN_GPIO_PORT, CUT_HOME_IN_GPIO_PIN);
	//Status = (~Status)&0x01;
	
	return (Status);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
