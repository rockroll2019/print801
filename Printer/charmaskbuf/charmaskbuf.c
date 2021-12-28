/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   �ַ���ģ��ȡ��������صĳ���.
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
#include	"charmaskbuf.h"
#include	"esc.h"
#include	"fs.h"
#include	"gs.h"
#include	"rs.h"
#include	"monobyte.h"
#include	"extgvar.h"
#include	"cominbuf.h"
#include	"spiflash.h"
#include	"usart.h"
#include	"feedpaper.h"
#include	"heat.h"
#include	"print.h"
#include	"timconfig.h"
#include	"debug.h"
//#include	"usb_prop.h"
#include	"button.h"
#include	"led.h"
#include	"beep.h"
#include	"uip.h"

/*�ⲿ��������*/
extern TypeDef_StructInBuffer volatile g_tInBuf;		//ͨ�Ž��ջ�����
//extern TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
extern TypeDef_StructHeatCtrl		g_tHeatCtrl;
extern	const unsigned char	CodepageFontA[];
extern	const unsigned char	CodepageFontB[];
extern  APP_Tx_Struct ETH_Tx;
extern  APP_Tx_Struct USB_Tx;	//USB����������ṹ 2016.07.29

extern uip_ipaddr_t uip_hostaddr, uip_draddr, uip_netmask;  //2017.02.07
extern uint8_t ETH_ConnectState;   //2018.03.05
#ifdef	DBG_PROCESS_TIME
extern	volatile uint32_t	g_iDbgProcessTimeUs;
#endif

#ifdef	DBG_RX_TOTAL_THEN_PRT
extern volatile uint8_t	g_bRxFinishFlag;	//��ʾ����ͨ���������Ƿ��Ѿ��������
#endif

/*�ⲿ��������*/
extern	void	DelayUs(uint32_t	delaytime);
extern void GetETH_ConnectState(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t	g_bLineEditBufStatus = LEB_STATUS_ILDE;

//���滻�Ĺ����ַ���
uc8  m_tbCharacterSetTable[] = 
{
	0x23,0x24,0x40,0x5B,0x5C,0x5D,0x5E,0x60,0x7B,0x7C,0x7D,0x7E
};

#define	BYTES_OF_CHAR_SET_TABLE	(sizeof(m_tbCharacterSetTable) / sizeof(m_tbCharacterSetTable[0]))

/*�����ַ���ȡ������,ȫ�ֱ���*/
TypeDef_StructCharMaskReadBuf		g_tCharMaskReadBuf;

#ifdef	DBG_ONE_LINE_TIME
uint32_t	g_iDealOneLineTimeCount;

#define	TIME_COUNT_END			(0)
#define	TIME_COUNT_START		(1)
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : ��ʼ���ַ���ȡ������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitCharMaskBufParameter(void)
{
	g_tCharMaskReadBuf.Status 			= CMRB_STATUS_ILDE;	//��Ϊ����
	g_tCharMaskReadBuf.LanguageType = g_tSysConfig.SysLanguage;	//����
	g_tCharMaskReadBuf.FontType 		= g_tSysConfig.SysFontSize;	//����
	memset(g_tCharMaskReadBuf.Buf, 0x00, MAX_BYTES_OF_HZ);
	//memset(g_tCharMaskReadBuf.Buf, 0x00, sizeof(g_tCharMaskReadBuf.Buf) / sizeof(g_tCharMaskReadBuf.Buf[0]));
}

/*******************************************************************************
* Function Name  : ProcessCharMaskReadBuf
* Description    : �����ַ���ȡ������,����ʱ�����������������ַ�,����д��ʱ
* �����ӡ�л�����,д�����ʱ�����ַ���д���л�����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	ProcessCharMaskReadBuf(void)
{
	//�������,��ȡͨ�����뻺�����ַ�����������д���
	if (g_tCharMaskReadBuf.Status == CMRB_STATUS_ILDE)
	{
		ProcessCharMaskReadBufIlde();	//�ַ����󻺳������д������
	}
	//���д�����,��ʼ������ַ�
	else if (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
	{
		ProcessCharMaskReadBufEdit();	//�ַ����󻺳����ȴ��༭�������
	}
}

//����ӡ���Ƿ�������û�д������.δ���ʱΪæ,������������:
//���ջ��������ݲ�Ϊ��;��ӡ���ƻ�����״̬��ΪILDE;
//���Ⱥ���ֽδ���;
uint32_t	GetPrintBusyStatus(void)
{
	if ((g_tInBuf.PutPosition != g_tInBuf.GetPosition) || (0 != g_bMotorRunFlag) ||(0 != g_tFeedButton.StepCount) ||(U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD))				//2016.07.07 �����ж�
	{
		return	PRINTER_STATUS_BUSY;
	}
	else
	{
		return	PRINTER_STATUS_ILDE;
	}
}

uint32_t	GetPrintProcessStatus(void)
{
	if ((0 != g_tSystemCtrl.StepNumber) || \
			(HEAT_ENABLE == g_tHeatCtrl.HeatEnable) || \
			(LPB_STATUS_ILDE != GetPrintStatus()))
	{
		return	PRINT_PROCESS_STATUS_BUSY;
	}
	else
	{
		return	PRINT_PROCESS_STATUS_ILDE;
	}
}

uint32_t	GetFeedPaperStatus(void)
{
#if	0
	if ((LPB_STATUS_ILDE != GetPrintStatus()) || \
			(g_tHeatCtrl.HeatEnable == HEAT_ENABLE) || \
			(g_tSystemCtrl.StepNumber != 0)
		)
#endif
	if (g_bMotorRunFlag)
		return	PRINTER_STATUS_BUSY;
	else
		return	PRINTER_STATUS_ILDE;
}


/*******************************************************************************
* Function Name  : CopyEditDataToLinePrtStruct
* Description    : ��g_tLineEditBuf�����ݼ����Ʊ�����ֵ�����ƽṹ.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#if	0
//ֻ�ܸ���,���������ӡ������,��ӡ�����������Ѿ����
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	
	//���������Ϊ�հ�,���ٸ���
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)		//����ȫ���Ƶ�ʱ��
		{
			pbSrc = g_tLineEditBuf;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
			memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//�����д�ӡ����������
		}
		else
		{
			//���㱾��������ַ�ռ�õ��������ֽ���,��8�ֽڶ���,����8�ֽڰ�����8�ֽڼ���
			if (g_tPrtCtrlData.MaxPrtDotHigh % 8)
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3) + 1;
			}
			else
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3);
			}
			//���ݷŴ���������ʼλ��,������׵�ַ���ֽ���
			StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
			ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//һ�ֽ��еĳ���
			pbSrc = g_tLineEditBuf + StartPositon;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + StartPositon;
			for (i = 0; i < ColumnByteNumber; i++)
			{
				memcpy(pbDst, pbSrc, ByteLen);	//�����д�ӡ����������
				pbSrc += PRT_DOT_NUMBERS;
				pbDst += PRT_DOT_NUMBERS;
			}
		}
	}
	
	if (g_tPrtCtrlData.DoUnderlineFlag)	//��Ҫ��ӡ�»���ʱ�Ÿ���,�������踴��
	{
		pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//����߽翪ʼ
		pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB + g_tPrtCtrlData.PrtLeftLimit;
		memcpy(pbDst, pbSrc, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//��ֵ�»�������
	}
	
	SetLinePrtModeVariable();		//��һ���д�ӡ����������,��ֵ��ӡ���Ʊ������д�ӡ���������ƽṹ
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	StopTimer(DBG_TIMER);					//ֹͣ��ʱ
	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
}
#endif

#if	0
//�����ӡ���������ݵĲ����ŵ��˴�,���Ƶ�ͬʱ�����ԭ��������,��һ����Ҫȫ����ģʽ
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
	uint32_t	iTemp;
	#endif
	
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint16_t	MaxPrtDotHigh;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
		StartProcessTimeCounter();
	#endif
	
	//���������Ϊ�հ�,���ٸ���
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)	//����ȫ���Ƶ�ʱ��
		{
			pbSrc = g_tLineEditBuf;
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
			memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//�����д�ӡ����������
			
			if (g_tPrtCtrlData.DoUnderlineFlag)	//��Ҫ��ӡ�»���ʱ�Ÿ���,�������踴��
			{
				pbSrc = g_tUnderlineBuf;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
				memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//��ֵ�»�������
			}
		}
		else	//���ָ��Ƶ�ʱ��,�ö�������ߵ��Ǹ�Ϊ��׼���Ƽ���
		{
			if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//ȷ��������Խ��
			{
				//���㱾��������ַ�ռ�õ��������ֽ���,��8�ֽڶ���,����8�ֽڰ�����8�ֽڼ���
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (g_tPrtCtrlData.MaxPrtDotHigh > ptLPB->PrtMode.MaxPrtDotHigh)
					MaxPrtDotHigh = g_tPrtCtrlData.MaxPrtDotHigh;
				else
					MaxPrtDotHigh = ptLPB->PrtMode.MaxPrtDotHigh;
				
				if (MaxPrtDotHigh % 8)
				{
					ColumnByteNumber = (MaxPrtDotHigh >> 3) + 1;
				}
				else
				{
					ColumnByteNumber = (MaxPrtDotHigh >> 3);
				}
				if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
					ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
				
				//���ݷŴ���������ʼλ��,������׵�ַ���ֽ���
				StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
				ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//һ�ֽ��еĳ���
				pbSrc = g_tLineEditBuf + StartPositon;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + StartPositon;
				for (i = 0; i < ColumnByteNumber; i++)
				{
					memcpy(pbDst, pbSrc, ByteLen);	//�����д�ӡ����������
					pbSrc += PRT_DOT_NUMBERS;
					pbDst += PRT_DOT_NUMBERS;
				}
				
				if (g_tPrtCtrlData.DoUnderlineFlag)	//��Ҫ��ӡ�»���ʱ�Ÿ���,�������踴��
				{
					pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//����߽翪ʼ
					pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB + g_tPrtCtrlData.PrtLeftLimit;
					memcpy(pbDst, pbSrc, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//��ֵ�»�������
				}
			}		//���ָ��Ʋ����Ϸ��������
			else	//�������Ϸ�,Խ��,ֱ��ȫ����
			{
				pbSrc = g_tLineEditBuf;
				pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
				memcpy(pbDst, pbSrc, BYTES_OF_LPB);	//�����д�ӡ����������
				
				if (g_tPrtCtrlData.DoUnderlineFlag)	//��Ҫ��ӡ�»���ʱ�Ÿ���,�������踴��
				{
					pbSrc = g_tUnderlineBuf;
					pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
					memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//��ֵ�»�������
				}
			}		//�������Ϸ��������
		}		//���ָ��ƴ������
	}		//�������ǿմ������
	else	//�༭������Ϊ�հ�,���Ǵ�ӡ���������ܲ��ǿհ�,��ʱ��Ҫ��մ�ӡ������
	{
		ClearLinePrtBuf();
	}
	
	SetLinePrtModeVariable();		//��һ���д�ӡ����������,��ֵ��ӡ���Ʊ������д�ӡ���������ƽṹ
	
	pbSrc = NULL;
	pbDst = NULL;
	ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
		StopTimer(DBG_TIMER);					//ֹͣ��ʱ
		DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}
#endif

#if	1
//ȫ����,ֻ�����ƣ������ԭ��ӡ������
void	CopyEditDataToLinePrtStruct(void)
{
	#ifdef	DBG_COPY_ONE_LINE_TIME
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
	uint32_t	iTemp;
	#endif
	
	uint8_t	*	pbSrc = NULL;
	uint8_t	*	pbDst = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
	if (DealOneLineTimeCountFlag == TIME_COUNT_END)
	{
		DealOneLineTimeCountFlag = TIME_COUNT_START;
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	}
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
		StartProcessTimeCounter();
	#endif
	
	pbSrc = g_tLineEditBuf;
	pbDst = U_PrtBuf.g_tLinePrtBuf.Buf;
	memcpy(pbDst, pbSrc, BYTES_OF_LINE_PRT_BUF);	//�����д�ӡ����������
	if (g_tPrtCtrlData.DoUnderlineFlag)	//��Ҫ��ӡ�»���ʱ�Ÿ���,�������踴��
	{
		pbSrc = g_tUnderlineBuf;
		pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LINE_PRT_BUF;
		memcpy(pbDst, pbSrc, PRT_DOT_NUMBERS);	//��ֵ�»�������
	}
	#if	0
	else
	{
		if (ptLPB->PrtMode.DoUnderlineFlag)
		{
			pbDst = U_PrtBuf.g_tLinePrtBuf.Buf + BYTES_OF_LPB;
			memset(pbDst, 0x00, PRT_DOT_NUMBERS);	//���LPB�»���
		}
	}
	#endif
	
	SetLinePrtModeVariable();		//��һ���д�ӡ����������,��ֵ��ӡ���Ʊ������д�ӡ���������ƽṹ
	
	pbSrc = NULL;
	pbDst = NULL;
	ptLPB = NULL;
	
	#ifdef	DBG_COPY_ONE_LINE_TIME
		StopTimer(DBG_TIMER);					//ֹͣ��ʱ
		DealOneLineTimeCountFlag = TIME_COUNT_END;
	#endif
	#ifdef	DBG_PROCESS_TIME_COPY_ONE_FULL_LPB	//���Ʊ༭�д�ӡ���������ݵ���ӡ��������ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}
#endif

/*******************************************************************************
* Function Name  : PrepareNewLine
* Description    : ���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	PrepareNewLine(void)
{
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	*	pbSrc = NULL;
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//��ձ༭�д�ӡ��������ʱ
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//��ձ༭�д�ӡ��������ʱ
		StartProcessTimeCounter();
	#endif
	
	
	#if	0
	if (MAX_RATIO_Y == g_tPrtCtrlData.MaxRatioY)		//����ȫ�����ʱ��
	{
		memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
		//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
	}
	else
	{
		if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//ȷ��������Խ��
		{
			//���㱾��������ַ�ռ�õ��������ֽ���,��8�ֽڶ���,����8�ֽڰ�����8�ֽڼ���
			if (g_tPrtCtrlData.MaxPrtDotHigh % 8)
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3) + 1;
			}
			else
			{
				ColumnByteNumber = (g_tPrtCtrlData.MaxPrtDotHigh >> 3);
			}
			if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
				ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
			
			//���ݷŴ���������ʼλ��,������׵�ַ���ֽ���
			StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtLeftLimit;
			ByteLen = g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit;		//һ�ֽ��еĳ���
			pbSrc = g_tLineEditBuf + StartPositon;
			for (i = 0; i < ColumnByteNumber; i++)
			{
				memset(pbSrc, 0x00, ByteLen);			//����д�ӡ����������
				pbSrc += PRT_DOT_NUMBERS;
			}
		}
		else	//���Խ��̶�ȫ���
		{
			//pbSrc = g_tLineEditBuf;
			memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);	//����д�ӡ����������
			//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
		}
	}
	#else
	memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
	//memset(g_tLineEditBuf, 0x00, sizeof(g_tLineEditBuf) / sizeof(g_tLineEditBuf[0]));
	#endif
	
	#ifdef	UPPER_LINE_ENABLE	
	if (g_tPrtCtrlData.DoUpperlineFlag) 	//����ϻ��߻�����
	{
		g_tPrtCtrlData.DoUpperlineFlag = 0;
		pbSrc = gstructUpperLine.DataBuf + g_tPrtCtrlData.PrtLeftLimit;	//����߽翪ʼ
		memset(pbSrc, 0x00, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//����»�������
	}
	#endif
	
	#ifdef	UNDER_LINE_PRT_ENABLE	
	if (g_tPrtCtrlData.DoUnderlineFlag)	//����»��߻�����
	{
		g_tPrtCtrlData.DoUnderlineFlag = 0;
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		if ((g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLeftLimit) && \
				(g_tPrtCtrlData.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//ȷ��������Խ��
		{
			pbSrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;	//����߽翪ʼ
			memset(pbSrc, 0x00, (g_tPrtCtrlData.PrtDataDotIndex - g_tPrtCtrlData.PrtLeftLimit));	//����»�������
		}
		else	//��ʹ����Խ��,ȷ��������Խ��
		{
			pbSrc = g_tUnderlineBuf;
			memset(pbSrc, 0x00, PRT_DOT_NUMBERS);	//����»�������
			//memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
		}
	}
	#endif
	
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
	g_tPrtCtrlData.MaxPrtDotHigh = 0;
	g_tPrtCtrlData.BitImageFlag = 0; 					//��λͼ��ӡ��־
	g_tPrtCtrlData.VariableBitImageFlag = 0; 	//��λͼ��ӡ��־
	g_tPrtCtrlData.CharWidth = 0;
	g_tPrtCtrlData.CharHigh = 0;
	g_tPrtCtrlData.LineHigh = 0;							//�и�
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	g_tPrtCtrlData.MaxRatioX = 1;					//����������Ŵ���
	#endif
	g_tPrtCtrlData.MaxRatioY = 1;					//�������������
	g_tPrtCtrlData.OneLineWidthFlag = 0;	//ȡ�����ڱ���
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;
	g_bLineEditBufStatus = LEB_STATUS_ILDE;
	pbSrc = NULL;
	
	#ifdef	DBG_PROCESS_TIME_CLEAN_ONE_FULL_LPB	//��ձ༭�д�ӡ��������ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	g_tTab.TabIndex = 0;		//2016.09.06
	
}

/*******************************************************************************
* Function Name	: MovDotLine( u8 *PrintBuff, u8 Lines )
* Description	: �����������е�һ��������������ӡ����
* Input			:   VBuf:	����������ַ����������
				        LineID:	��ǰ�账����к�
* Output		: None
* Return		: None
*******************************************************************************/
void MovDotLine (u8 *VBuf, u8 LineID)			  //2016.06.28
{
	uint16_t	i, SpaceDot, HeatDots;
	uint8_t	ByteDots;
	
	//ת����һ�ֽں����������
	ByteDots = 0;
	
	//����ȵ���
	HeatDots  = 0;

	if(g_tPrtCtrlData.LineDirection ==PRINT_DIRECTION_NORMAL)			//������ӡ
	{
		if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	           //����
		{	
			SpaceDot = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex)/2;
		}	
		else if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)				//�Ҷ���
		{	
			SpaceDot = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		}	
		else					//�����
		{	
			SpaceDot = 0;
		}	
// 		SpaceDot += g_tPrtCtrlData.PrtLeftLimit;

	}
	else	   								//�����ӡ
	{
		if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	//�ַ�����ģʽ��0=����룬1=����, 2=�Ҷ���
		{	
			SpaceDot = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex)/2;
		}	
		else if( g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)
		{	
			SpaceDot = 0;
		}	
		else
		{	
			SpaceDot = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		}	
// 		SpaceDot += g_tPrtCtrlData.PrtLeftLimit;
	}

	PrtDrvBufFull();						      //�д�ӡ�������пռ�

	for(i=0; i< SpaceDot/8; i++)			//��߲��û���������
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = 0x00;
	}
	if(g_tPrtCtrlData.LineDirection ==PRINT_DIRECTION_NORMAL)			//�����ӡ
	{
		for(i=0; i < g_tPrtCtrlData.PrtDataDotIndex; i++)
		{
			if(VBuf[i] & (0x80 >> (LineID & 0x07)))		 //���ϵ��µ�˳��  
			{
				ByteDots |= 0x80>> (SpaceDot & 0x07);
				HeatDots++;
			}
			SpaceDot++;
			if((SpaceDot & 0x07 )==0)
			{
				U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[SpaceDot/8-1] = ByteDots;
				ByteDots = 0;
			}
		}
	}
	else										//�����ӡ
	{	
		for(i=0; i < (g_tPrtCtrlData.PrtDataDotIndex) ; i++)
		{
			if(VBuf[g_tPrtCtrlData.PrtDataDotIndex-1-i] & (0x80 >> (LineID & 0x07)))		
			{
				ByteDots |= 0x80>> (SpaceDot & 0x07);
				HeatDots++;
			}
			SpaceDot++;
			if((SpaceDot & 0x07 )==0)
			{
				U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[ (SpaceDot/8-1)] = ByteDots;
				ByteDots = 0;
			}
		}
	}

	i= SpaceDot/8;
	if( (SpaceDot &0x07) !=0)				//�����8�������Ĵ���
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = ByteDots;
		i++;
	}
		
	for(; i< PRT_DOT_NUMBERS/8; i++)			//�ұ߲��û���������
	{
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = 0;
	}	
	IncDrvPrtBufAddr(0x00, HeatDots);
}

/*******************************************************************************
* Function Name  : 
* Description    : ��ӡ��ǰ�д�ӡ�������е�����.����ӡ������״ֱ̬������,
* Ȼ�����д�ӡ����������,���ƿ��Ʊ���,����༭�д�ӡ������,��ʼ��ӡ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
	volatile uint32_t	g_iDbgSPISendCount;			//����оSPI�������ݵĴ���ͳ��
	volatile uint32_t	g_iDbgEditCountWhilePrt;
#endif
void	PrtOneLineToBuf(uint32_t LineHigh)
{
	s16	Lines;
	uint8_t	*pbBuf,*p;
	
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//��ӡһ�к�ʱ
		uint32_t	iTemp;
	#endif
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		StartProcessTimeCounter();
	#endif
	
 //WaitPreviousPrintFinish();	//��ѯ�д�ӡ������״̬,һֱ�ȵ��п����л��������Խ��д�ӡ
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//��ӡһ�к�ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	pbBuf = g_tLineEditBuf + \
			(MAX_RATIO_Y - g_tPrtCtrlData.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS;  //�ַ��ڱ༭���������׵�ַ
					
	if( g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
	{										//�����ӡ
		for(Lines=(8-g_tPrtCtrlData.UnderlineMaxPrtHigh); Lines<8; Lines++)  //��ӡ�»���
		{			
			MovDotLine( g_tUnderlineBuf, Lines);	  
		}
		for( Lines=g_tPrtCtrlData.MaxPrtDotHigh; Lines >0; )              //��ӡ������
		{
			Lines--;
			p = pbBuf + (Lines>>3 )* PRT_DOT_NUMBERS;
			MovDotLine( p, Lines);	           
		}
	}
	else		 					 //�����ӡ
	{
		for( Lines=0; Lines < g_tPrtCtrlData.MaxPrtDotHigh; Lines++ )	  	     //��ӡ������ �����ڱ༭�������ĵײ������ݴ��ϵ��µĵ��У������0���������
		{
			p = pbBuf +  (Lines>>3 )* PRT_DOT_NUMBERS ;													 //����������ڵ��ֽ��е�λ��
			MovDotLine( p,  Lines);	                 			              
		}

		for(Lines=(8-g_tPrtCtrlData.UnderlineMaxPrtHigh); Lines<8; Lines++)  //��ӡ�»���
		{			
			MovDotLine( g_tUnderlineBuf, Lines);	  
		}			
	}
	
	g_tPrtCtrlData.MaxPrtDotHigh +=g_tPrtCtrlData.UnderlineMaxPrtHigh;
	//���м��
	if(LineHigh > g_tPrtCtrlData.MaxPrtDotHigh)
	{	
		GoDotLine((LineHigh - g_tPrtCtrlData.MaxPrtDotHigh));			
	}
	PrepareNewLine();	//���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
	
	//����һ�еı༭��������ʱ,�������ֿ�ͱ༭���ʱ��
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		StartProcessTimeCounter();
	#endif
}



void	PrintOneLine(void)
{
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//��ӡһ�к�ʱ
		uint32_t	iTemp;
	#endif
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		StartProcessTimeCounter();
	#endif
	
	WaitPreviousPrintFinish();	//��ѯ�д�ӡ������״̬,һֱ�ȵ��п����л��������Խ��д�ӡ
	
	#ifdef	DBG_PROCESS_TIME_WAIT_LPB_ILDE	//��ӡһ�п�ʼǰ�ȴ�״̬��Ϊ���еĺ�ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//��ӡһ�к�ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	if ((g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
			(g_tPrtCtrlData.DoUnderlineFlag == 0))
	{
		if (!((g_tPrtCtrlData.BitImageFlag) || (g_tPrtCtrlData.VariableBitImageFlag)))
		{	//ֱ�����м��,ͼ�δ�ӡģʽʱ�����м��
			GoDotLine((uint32_t)g_tPrtCtrlData.CodeLineHigh);
		}
	}
	else
	{
		CopyEditDataToLinePrtStruct();	//��g_tLineEditBuf�����ݼ����Ʊ�����ֵ�����ƽṹ
		U_PrtBuf.g_tLinePrtBuf.Status = LPB_STATUS_WAIT_TO_PRT;	//��ֵ��ӡ���Ʊ���
		SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);	//������һ�еĴ�ӡ
		StartTimer(PRINT_CHECK_TIMER);
		
		#ifdef	DBG_PROCESS_TIME_PRT_ONE_FULL_LPB	//��ӡһ�к�ʱ
			StartProcessTimeCounter();
		#endif
		#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
			g_iDbgSPISendCount = 0;
			g_iDbgEditCountWhilePrt = 0;
		#endif
	}
	
	PrepareNewLine();	//���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
	
	//����һ�еı༭��������ʱ,�������ֿ�ͱ༭���ʱ��
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		StartProcessTimeCounter();
	#endif
}


#if defined	DBG_DEAL_ONE_LINE_TIME || defined DBG_COPY_AND_CLEAR_ONE_LINE_TIME || defined DBG_PRT_SPEED
	static uint8_t	DealOneLineTimeCountFlag = TIME_COUNT_END;
#endif

static void	ProcessCharMaskReadBufIlde(void)
{
	uint8_t	ReadChar;
	static uint8_t WaitFlag = 0;			//2016.05.23  
	uint16_t i;
	uint8_t Bps;
	//������ջ�����������,�����п��Խ��б༭���д�ӡ������
	if ((g_tInBuf.PutPosition != g_tInBuf.GetPosition) || g_tMacro.RunFlag)	//2016.07.07
	{
			if(WaitFlag ==0 )	
			{
				WaitFlag = 1;
				MaxSetp();									//�ӿ����ͳ�ʼ�������ٲ���
				
				//�������ڻ�����û��ʱ����ʼ��������ʱ���ȴ�2.0��
				if(g_tSystemCtrl.PrtSourceIndex == COM ) 		//COM 2016.06.08 
				{
 					//if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)			//����ģʽ
					{
						//�ȴ������û�����ݣ����˳��ȴ�
						for(i=0; i<3000; i++)			//2016.06.30  8000��Ϊ3000		
						{
							DelayMs(1);					//��ʱ��׼ ���ʱ�� 
							if(g_tSystemCtrl.SleepCounter >30)	//30msû���յ����˳�  2016.05.23
								break;
						}
					}	
				}
		  }
		  g_tSystemCtrl.SleepCounter = 0;								//��λ���߼�����
			
		#ifdef	DBG_RX_TOTAL_THEN_PRT				//�����ٴ�
		if (g_bRxFinishFlag != RX_FINISHED)
		{
			return;		//����һ��ͨ�Ž������
		}
		else	//ͨ�Ź����Ѿ����,��ʼ�����û�������ߴ���ӡ����			
		{
			#if defined	DBG_DEAL_ONE_LINE_TIME || defined DBG_PRT_SPEED
				if (DealOneLineTimeCountFlag == TIME_COUNT_END)
				{
					DealOneLineTimeCountFlag = TIME_COUNT_START;
					g_iDealOneLineTimeCount = 0;
					SetTimerCounterWithAutoReload(DBG_TIMER, 1);
					StartTimer(DBG_TIMER);
				}
			#endif
			
			if (g_tSystemCtrl.PrtDeviceDisable) 	//��ӡ������ֹ
			{
				Command_1B3D();
			}
			else
			{
				ReadChar = ReadInBuffer();	//��ȡ���ջ�����
				DataProcess(ReadChar);			//������ַ�,������������߿ɴ�ӡ�ַ�,����ǿɴ�ӡ�ַ�������SPI��ȡ
			}
		}
		#else	//�������ձߴ�ĳ�����,�����ͨ���Ƿ����
		if (g_tSystemCtrl.PrtDeviceDisable) 	//��ӡ������ֹ
		{
			Command_1B3D();
		}
		else
		{
			ReadChar = ReadInBuffer();	//��ȡ���ջ�����
			DataProcess(ReadChar);			//������ַ�,������������߿ɴ�ӡ�ַ�,����ǿɴ�ӡ�ַ�������SPI��ȡ
		}
		#endif
	}			//ͨ�Ž��ջ������ǿմ������
	else	//ͨ�Ž��ջ�����Ϊ�մ���ʼ
	{
		WaitFlag = 0;	//2016.05.23
	  if(NetReloadCounter>65000)           //2018.12.10  ��ʱ����1���Ӹ�λ����
		{			
			ETH_ConnectState=0;
			if(g_tInterface.ETH_Type) 
			{
				uIPMain();    //��ʼ������
			}	                        //���ڳ�ʼ��
			NetReloadCounter = 0;
		}		
		if (g_tSystemCtrl.SleepCounter > SLEEP_TIME_MS)	//���߶�ʱʱ�䵽
		{
				g_tSystemCtrl.SleepCounter = 0;
		}
		if(g_tInterface.ETH_Type) 
		{	
			GetETH_ConnectState();
		}	
		CheckFeedButton();
		
		if(g_tSysConfig.DHCPEnable)
		{	
			if((DhcpGetFlag ==0) && (uip_hostaddr[0] != 0))
			{	
				ETH_SelfTest();               //2017.02.07 ��ӡ�Զ���ȡ��IP��ַ
				DhcpGetFlag = 1;							//��λ��־λ
			}	
		}
		
		#if defined DBG_PRT_SPEED
			StopTimer(DBG_TIMER);					//ֹͣ��ʱ
			DealOneLineTimeCountFlag = TIME_COUNT_END;
		#endif
	}			//ͨ�Ž��ջ�����Ϊ�մ������
}

static void	ProcessCharMaskReadBufEdit(void)
{
	uint8_t	Result;
	uint8_t	CodeType;
	#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//����һ�еı༭��������ʱ
		uint32_t	iTemp;
	#endif
	//����һ�еı༭��������ʱ,�������ֿ�ͱ༭���ʱ��
	#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
		uint32_t	iTemp;
	#endif

	if (g_bLineEditBufStatus == LEB_STATUS_ILDE)	//�п����л��������Խ��б༭
	{
		
		#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//����һ�еı༭��������ʱ
			StartProcessTimeCounter();
		#endif
		CodeType = g_tCharMaskReadBuf.LanguageType;	//�ַ����Ǻ���
		Result = JudgeLineBufFull(CodeType);
		if (Result ==	LINE_BUF_NOT_FULL)	//���в������Էŵ��µ�ǰ�ַ�
		{	//�����ַ����뵱ǰ�༭�л�����
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);
			InitCharMaskBufParameter();		//�����ģ��ȡ������,�ַ���������־��ΪILDE
			#ifdef	DBG_PROCESS_TIME_EDIT_ONE_FULL_LPB	//����һ�еı༭��������ʱ
				StopProcessTimeCounter();
				iTemp = 0;
				g_iDbgProcessTimeUs = 0;
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
				g_iDbgEditCountWhilePrt++;
			#endif
		}
		else if (Result == LINE_BUF_FULL)	//��ǰ�л����������޷����µ�ǰ�ַ�
		{
			g_bLineEditBufStatus = LEB_STATUS_WAIT_TO_PRINT;	//�ø���״̬Ϊ�ȴ���ӡ
			//����һ�еı༭��������ʱ,�������ֿ�ͱ༭���ʱ��
			#ifdef	DBG_PROCESS_TIME_READ_AND_EDIT_ONE_FULL_LPB
				StopProcessTimeCounter();
				iTemp = 0;
				g_iDbgProcessTimeUs = 0;
			#endif

			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);         //2016.06.28
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);		//�������
			InitCharMaskBufParameter();		//�����ģ��ȡ������,�ַ���������־��ΪILDE
			
			#ifdef	DBG_DEAL_ONE_LINE_TIME
				StopTimer(DBG_TIMER);					//ֹͣ��ʱ
				DealOneLineTimeCountFlag = TIME_COUNT_END;
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
				g_iDbgEditCountWhilePrt++;
			#endif
		}
	}		//�п��б༭�д�ӡ�������������
	else	//û�п��б༭�д�ӡ������ʱ��鴦���ӡ�л�����
	{
		if (LPB_STATUS_ILDE == GetPrintStatus())	//��ѯ�д�ӡ������״̬
		{			
			
			#ifdef	DBG_COPY_AND_CLEAR_ONE_LINE_TIME
				if (DealOneLineTimeCountFlag == TIME_COUNT_END)
				{
					DealOneLineTimeCountFlag = TIME_COUNT_START;
					g_iDealOneLineTimeCount = 0;
					SetTimerCounterWithAutoReload(DBG_TIMER, 1);
					StartTimer(DBG_TIMER);
				}
			#endif
			
			CopyEditDataToLinePrtStruct();		//��g_tLineEditBuf�����ݼ����Ʊ�����ֵ�����ƽṹ
			U_PrtBuf.g_tLinePrtBuf.Status = LPB_STATUS_WAIT_TO_PRT;	//��ֵ��ӡ���Ʊ���
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);	//������һ�еĴ�ӡ
			StartTimer(PRINT_CHECK_TIMER);
			
			#ifdef	DBG_COPY_AND_CLEAR_ONE_LINE_TIME
				StopTimer(DBG_TIMER);					//ֹͣ��ʱ
				DealOneLineTimeCountFlag = TIME_COUNT_END;
			#endif
			
			PrepareNewLine();							//���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
		}
	}		//û�п��б༭�д�ӡ������ʱ��鴦���ӡ�л������������
}

/*******************************************************************************
* Function Name  : static void	FillLinePrintBuf(uint8_t * pstruct, uint8_t CodeType)
* Description    : ����ǰ��ģ��ȡ�����������ݷ���ָ���д�ӡ��������ַ
* Input          : Source����ģ��ַ,CodeType����������,0������,=1����
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void	FillLinePrintBuf(uint8_t * Source, uint8_t CodeType)
{
	//uint8_t	Width, High, RatioX, Temp;
	uint8_t	Width, High, Temp;
	
	//RatioX = GetRatioXValue(CodeType);
	if (CodeType)
	{
		Width = g_tPrtModeHZ.CharWidth;
 		High  = g_tPrtModeHZ.CharWidth;	//CharHigh��ʱδ����
	}
	else
	{
		Width = g_tPrtModeChar.CharWidth;
		High  = 24;             //g_tPrtModeChar.CharHigh;  2016.07.01 ���и߶��̶���24�� ������Ҫ���� ����6*8 9*17 �¶�����ֲ��ȸߵ�һ�� �����ϲ�Ϊ�հ�
	}	
	
	if (g_tPrtCtrlData.CharCycle90)		//��ת90��
	{
		CodeRollCtrl(Width, High, Source);
		Temp 	= Width;
		Width	= High;
		High 	= Temp;
	}
	if (g_tPrtModeChar.EmphaSizeMode)	//�Ӵּ���
	{
		EmphaSizeProcess(Width, 3, Source);
	}
	
	FillPrintBuff(Source, Width, High, CodeType);
//	FillPrintBuff(Source, Width, High, RatioX, CodeType);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���ݴ������,�Ǵ�ӡ�ַ������ӡ,�����������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
//#define	DBG_RS232_ENABLE
void	DataProcess(uint8_t GetChar)
{
	#if	0
		while (g_tSystemCtrl.PrtDeviceDisable) 	//��ӡ������ֹ
		{
			Command_1B3D();						//��ֹ/�����ӡ��
		}
	#endif
	
	#ifdef	DBG_RS232_ENABLE
		UpLoadData(&GetChar, 1);		//ԭ���ط�
	#else
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//��׼ģʽ
		{
			if (GetChar >= 0x20)
			{
				PrintChar(GetChar);	  //�ɴ�ӡ�ַ�����
			}
			else
			{
				if (GetChar == 0x1B)
				{
					ESC_Command();		          //ESC(1B)ָ���
				}
				else if (GetChar == 0x1C)
				{
					FS_Command();				        //FS(1C)ָ���
				}
				else if (GetChar == 0x1D)
				{
					GS_Command();				        //GS(1D)ָ���
				}
				#ifdef	DBG_SELF_DEF_CMD_ENABLE
					else if (GetChar == 0x1E)
					{
						RS_Command();			        //1Eָ���
					}
				#endif
				else if (GetChar == 0x1F)			
				{
					US_Command();				        //1F���ò���ָ���
				}
				else
				{
					MonoByte_Command(GetChar);	//���ֽ�ָ���
				}
			}	//ָ������
		}	//��׼ģʽ�������
		else							//ҳģʽ����
		{
		}
	#endif
}

/*******************************************************************************
* Function Name  : void UpLoadData(uint8_t *DataBuf, uint32_t Length)
* Description    : �ϴ����ݵ�����
* Input          : Length�����ݳ���
*				   DataBuf�����ݵ�ַָ��
* Output         : None
* Return         : None
*******************************************************************************/
void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length)
{
	uint32_t i;
	unsigned char * pbtemp;
	
	uint32_t Len;
  APP_Tx_Struct *Tx_Buf=NULL;	
	
	if (g_tSystemCtrl.PrtSourceIndex == USB)			//��ǰ����ԴΪUSB
	{
		Tx_Buf =&USB_Tx;
		i =0;
		while(Length)
		{
			//���㻺����ʣ��ռ�
			Len =Tx_Buf->ptr_out;	//�ж��п��ܸı���ֵ
			if(Tx_Buf->ptr_in >= Len )
				Len =Tx_Buf->size - (Tx_Buf->ptr_in - Len);
			else
			{
				Len =Len - Tx_Buf->ptr_in;
			}
			if( Len )
			{
				Tx_Buf->Buffer[Tx_Buf->ptr_in]= DataBuf[i];
				Tx_Buf->ptr_in++;
				if(Tx_Buf->ptr_in ==Tx_Buf->size)
				{
					Tx_Buf->ptr_in=0;

					if(g_tInterface.WIFI_Type)
					{
						//clear_busy = 0 ;
					}
				}			
				Length--;
				i++;
			}
			else
			{
				if (g_tSystemCtrl.PrtSourceIndex == USB)		//��ǰ����ԴΪUSB
				{		
					break;
				}
			}
		}
		return;
	}
	if (g_tSystemCtrl.PrtSourceIndex == COM)			//��ǰ����ԴΪCOM
	{
		for (i = 0; i < Length; i++)
		{
			//while (!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE));	//�ȴ����ͻ����
			/* Loop until the end of transmission */
			while ((!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE))  && (USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TC) == RESET));   //2017.10.18  ����GDоƬ
			USART_SendData(USART_SOURCE, DataBuf[i]);
		}
		return;
	}
	if(g_tSystemCtrl.PrtSourceIndex == ETHERNET )		//2016.07.07
	{	
		Tx_Buf =&ETH_Tx;
		
		i =0;
		while(Length)
		{
			//���㻺����ʣ��ռ�
			Len =Tx_Buf->ptr_out;	//�ж��п��ܸı���ֵ
			if(Tx_Buf->ptr_in >= Len )
				Len =Tx_Buf->size - (Tx_Buf->ptr_in - Len);
			else
			{
				Len =Len - Tx_Buf->ptr_in;
			}
			if( Len )
			{
				Tx_Buf->Buffer[Tx_Buf->ptr_in]= DataBuf[i];
				Tx_Buf->ptr_in++;
				if(Tx_Buf->ptr_in ==Tx_Buf->size)
				{
					Tx_Buf->ptr_in=0;
				}			
				Length--;
				i++;
			}
			else
			{
				if ((g_tSystemCtrl.PrtSourceIndex == ETHERNET))		//��ǰ����ԴΪ����
				{		
					
				}
			}
		}
		return;
	}	
	if (g_tSystemCtrl.PrtSourceIndex == BLUETOOTH)			//2018.04.19 ��ǰ����ԴΪ��������wifi  ����2
	{
		for (i = 0; i < Length; i++)
		{
			//while (!USART_GetFlagStatus(USART_SOURCE, USART_FLAG_TXE));	//�ȴ����ͻ����
			/* Loop until the end of transmission */
			while ((!USART_GetFlagStatus(WIFI_COM, USART_FLAG_TXE))  && (USART_GetFlagStatus(WIFI_COM, USART_FLAG_TC) == RESET));   //2017.10.18  ����GDоƬ
			USART_SendData(WIFI_COM, DataBuf[i]);
		}
		return;
	}
}

/*******************************************************************************
* Function Name  : PrintChar(uint8_t CharCode)
* Description    : ���յ�����Ч��ӡ����Ĵ���
* Input          : CharCode�����յ����ַ�����
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintChar(uint8_t CharCode)
{
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)	//����ģʽ
	{
  	PrintAscII(CharCode);
	}	//���Ĵ������
  else										//����ģʽ
	{	//�Զ��庺��
                                            //(g_tSystemCtrl.UserSelectFlag) && 
	if ( (CharCode == 0xFE) && \
		 ((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x02) == 0x02))
		{
			JudgePrintSelfDefChinese(CharCode);	//�����Զ��庺�ִ�ӡ
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)	//BIG5���ַ�ʽ
  	{
			JudgePrintBIG5(CharCode);			//����BIG5���ִ�ӡ
		}
		else								//GB18030
		{
			JudgePrintGB18030(CharCode);	//����GB18030��ӡ  	
		}
	}	//���ִ������
}

/*******************************************************************************
* Function Name  : ������
* Description    : �û��Զ��庺�����ݴ������,�ж������Ƿ�ϸ�,�������ӡ
* Input          : �������,��һ�ֽڱ���
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	JudgePrintSelfDefChinese(uint8_t Ch1)
{
	uint8_t	Ch2;
	uint8_t	* p = NULL;
	uint32_t	i;
	
	Ch2 = ReadInBuffer();
	if ((Ch2 > 0xA0) && (Ch2 < (0xA1 + USER_HZ_SIZE)))
	{
		p = g_tRamBuffer.UserCharStruct.UserCode_HZ;	//�û����庺�ֵ��ֽڴ����
		for (i = 0; i < USER_HZ_SIZE; i++, p++)
		{
			if (Ch2 == *p)
				break;
		}
		if (i < USER_HZ_SIZE)
		{
			PrintUserHZ(Ch1, Ch2);			//��ӡ�Զ��庺��
		}
		else
		{
			PrintGB18030(0xA1, 0xA1, 0, 0);	//���û���Զ��庺��,���ӡһȫ�ǿ�
		}
	}
	else
	{
		PrintAscII(Ch1);
		PrintAscII(Ch2);
	}
	
	p = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : BIG5�������ݴ������,�ж������Ƿ�ϸ�,�������ӡ
* Input          : �������,��һ�ֽڱ���
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	JudgePrintBIG5(uint8_t Ch1)
{
	uint8_t	Ch2;
	
	if ((Ch1 < 0xA1) || (Ch1 > 0xF9))	//��һ����Ϊ��Ч���ִ���
	{
		PrintAscII(Ch1);				//��һ�ֽ�Ϊ�����ַ�
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x40) || ((Ch2 > 0x7E) && (Ch2 < 0xA1)) || (Ch2 > 0xFE))
		{	//�ڶ��ֽ�Ϊ��Ч���ִ���
			PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
			DecInBufferGetPoint();	//�Ż�����ȡ���ַ�
		}
		else							//��Ч���ִ���
		{
			PrintBIG5(Ch1, Ch2);		//��ӡ����
		}
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : GB18030�������ݴ������,�ж������Ƿ�ϸ�,�������ӡ
* Input          : �������,��һ�ֽڱ���
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	JudgePrintGB18030(uint8_t Ch1)
{
	uint8_t	Ch2, Ch3, Ch4;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//����һ�����ֵĵ����ֿ��ַ����Ϣ��ʱ
		StartProcessTimeCounter();
	#endif
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//��һ����Ϊ��Ч���ִ���
	{
		PrintAscII(Ch1);				//��һ�ֽ�Ϊ�����ַ���ӡ
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//��Ч���ִ���
		{
			PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
 			DecInBufferGetPoint();		//�Ż�����ȡ���ַ����޸�
		}
		else if (((Ch2 >= 0x40) && (Ch2 <= 0x7E)) || \
						 ((Ch2 >= 0x80) && (Ch2 <= 0xFE)))
		{
			Ch3 = 0;
			Ch4 = 0;
			PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//���ֽں���
		}
		else									//���ֽڷ�ʽ
		{
			Ch3 = ReadInBuffer();
			if ((Ch3 < 0x81) || (Ch3 > 0xFE))  	//�����ֽڴ�����Ч
			{
				PrintAscII(Ch1);				//��һ�ֽڰ������ַ���ӡ
				PrintAscII(Ch2);				//�ڶ��ֽڰ������ַ���ӡ
				DecInBufferGetPoint();	//�Ż�����ȡ���ַ�
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//�����ֽڴ�����Ч
				{
					PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
					PrintAscII(Ch2);			//�ڶ��ֽڰ������ַ���ӡ
					PrintAscII(Ch3);			//�����ֽ�Ϊ��Ч��ӡ�ַ�
					DecInBufferGetPoint();	//�Ż�����ȡ���ַ�,ch4�п����ǺϷ���˫�ֽڱ���ĵ�λ�ֽ�
				}
				else
				{
					PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//���ֽں���
				}
			}
		}
	}
	
	#ifdef	DBG_PROCESS_TIME_ANALYSE_AND_READ_ONE_HZ	//����һ�����ֵĵ����ֿ��ַ����Ϣ��ʱ
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
}

/*******************************************************************************
* Function Name  : PrintAscII
* Description    : �����ӡ���ܷ��µ�ǰASCII,��ȡASCII�ַ���ģ����ӡ������,����,
*	��ӡ��ǰ��.
* Input          : ASCII�ַ�����
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
//����POS���̵Ĵ���ʽ
//�������ַ���SPI_DMA��ȡ
//����ǰ�ַ������������õ��ַ���ȡ�������ṹ����
void	PrintAscII(uint8_t Code)
{
	//uint8_t	TempBuf[MAX_BYTES_OF_CHAR];
	//GetAscIIDotData(Code, TempBuf);	
	GetAscIIDotData(Code, g_tCharMaskReadBuf.Buf);	
}

/*******************************************************************************
* Function Name  : PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : �����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,�����ӡ��ǰ��.
* Input          : CodeHigh=���ִ����λ,CodeLow=���ִ����λ
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
#ifdef	BIG5_ZK_INDEPENDENT
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset;
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
	{	
		Offset = CodeLow - 0x40;
	}
	else if ((CodeLow >= 0xA1) && (CodeLow <= 0xFE))
	{
		Offset = CodeLow - 0xA1 + 0x3F;
	}
	else
	{
		Offset = 0;
	}
	
	CodeAddr = BIG5_BASE_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * BYTES_OF_HZ_FONT_A;
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_BIG5;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//��Ϊ����д��
	//�д��������SPI DMA����
	//sFLASH_ReadBuffer(TempBuf, CodeAddr, BYTES_OF_HZ_FONT_A);
	sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
	
	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);	//�ϴ���PC
		InitCharMaskBufParameter();	//��Ϊ����,�����BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
	#endif
}
#else
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset;
	uint8_t	bBuf[2];			//���2�ֽ�GB��
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
	{	
		Offset = CodeLow - 0x40;
	}
	else if ((CodeLow >= 0xA1) && (CodeLow <= 0xFE))
	{
		Offset = CodeLow - 0xA1 + 0x3F;
	}
	else
	{
		Offset = 0;
	}
	
	CodeAddr = BIG5_BASE_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * 2;	//BIG5��Ӧ��GB���Ϊ2���ֽ�
	sFLASH_ReadBuffer(bBuf, CodeAddr, 2);		//��ȡת�����ַ
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(bBuf, 2);
	#endif
	PrintGB18030(bBuf[0], bBuf[1], 0, 0);		//���д����˶�ȡ�������������ü����Դ������ܴ���
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_BIG5;
	
}
#endif

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : �����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,����,��ӡ��ǰ��.
* Input          : Code1,Code2,Code3,Code4Ϊ���ִ���,��Code3=Code4=0ʱΪ˫�ֽڴ���
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
{
	uint32_t	CodeAddr;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iDbgProcessTimeTemp;
	#endif
	
	//�����ַ���ַ
	if ((Code3 == 0) || (Code4 == 0))	 //˫�ֽڷ�ʽ
	{
		if ((Code1 >= 0xA1) && (Code1 <= 0xA9) && (Code2 >= 0xA1))			//1������
		{	
			CodeAddr = (Code1 - 0xA1) * 94 + (Code2 - 0xA1);
		}
		else if ((Code1 >= 0xA8) && (Code1 <= 0xA9) && (Code2 < 0xA1))	//5������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2������
		{
			CodeAddr = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 >= 0x81) && (Code1 <= 0xA0))		//3������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 7806;	//1038 + 6768
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))			//4������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeAddr = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 13886;	//1038 + 12848
		}
		else
		{
			CodeAddr = 0;		//���������Ϊ������
		}
	}		//˫�ֽڴ������
	else
	{
	    CodeAddr =	(Code1 - 0x81) * 12600 + (Code2 - 0x39) * 1260 + (Code3 - 0xEE) * 10 + (Code4 - 0x39) + 22046;
		 						  
// 		if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 == 0xEE) && (Code4 == 0x39))
// 		{	//��λ8139�ĵ���һ���ַ�
// 			CodeAddr = 22046;
// 		}
// 		else if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 >= 0xEF))
// 		{	//8139ҳʣ�ಿ��
// 			CodeAddr = (Code3 - 0xEF) * 10 + (Code4 - 0x30) + 22047;	//22046 + 1
// 		}
// 		else if (	(Code1 == 0x82) && \
// 							((Code2 >= 0x30) && (Code2 <= 0x34)) && \
// 							((Code3 >= 0x81) && (Code3 <= 0xFE)))
// 		{	//8230ҳ~8234ҳ,��Ϊ��ҳ
// 			CodeAddr = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
// 		}
// 		else if ((Code1 == 0x82) && \
// 						 (Code2 == 0x35) && \
// 						((Code3 >= 0x81) && (Code3 <= 0x87)))
// 		{	//8235ҳ,ֻ��69���ַ�
// 			CodeAddr = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
// 		}
// 		else
// 		{	//������һ��Ϊ����λ,ȫ��
// 			CodeAddr = 0;
// 		}
	}
	
	CodeAddr = GB18030_24X24_BASE_ADDR + CodeAddr * BYTES_OF_HZ_FONT_A;
	//��ȡ��ģ
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_GB18030;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//��Ϊ����д��
	#if defined	DBG_PROCESS_TIME_READ_ZIKU_ONE_HZ	//��һ�����ֵĵ����ֿ����ݺ�ʱ
		StartProcessTimeCounter();
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
		StopProcessTimeCounter();
		iDbgProcessTimeTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#else
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, CodeAddr, BYTES_OF_HZ_FONT_A);
	#endif
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);
	#endif

	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, BYTES_OF_HZ_FONT_A);	//�ϴ���PC
		InitCharMaskBufParameter();	//��Ϊ����,�����BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
	#endif
}

/*******************************************************************************
* Function Name  : PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : ��ӡ�Զ��庺��,�����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,
*				   ����,��ӡ��ǰ��.
* Input          : CodeHigh=���ִ����λ,CodeLow=���ִ����λ
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
//����Ϊ����POS��������
void PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint32_t	i;
	uint8_t	* pselfdef;	
	uint8_t * pbuf = NULL;
	
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_CHINESE;
	g_tCharMaskReadBuf.FontType = HZ_FONT_A;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//��Ϊ����д��
	pbuf		 = g_tCharMaskReadBuf.Buf;
	pselfdef = g_tRamBuffer.UserCharStruct.Buffer_HZ + (CodeLow - 0xA1) * BYTES_OF_HZ_FONT_A;	//�û����庺����ģ���׵�ַ
	for (i = 0; i < BYTES_OF_HZ_FONT_A; i++)
	{
		 *pbuf++ = *pselfdef++;
	}
	pbuf = NULL;
	
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
}


/*******************************************************************************
* Function Name  : void GetAscIIDotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ��������������9*17��12*24
* Output         : 
* Return         : 
*******************************************************************************/
//����Ϊ��������ϲ�����ĳ���
/*******************************************************************************
* Function Name  : 
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ,��������ȡ
*	�ȴ����Ƿ����Զ����ַ�,ֱ�Ӵ�RAM�и���;��������ֿ�FLASH��,����SPI DMA
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII�ַ����������
{
	//�������Զ�����ַ�,��ѡ����,�Ҷ�Ӧ�ַ��ж���
	if ((g_tSystemCtrl.UserSelectFlag) && \
			(AscIICode < 0x7F) && \
			((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x01) == 0x01))
	{
		if (g_tCharMaskReadBuf.FontType == ASCII_FONT_A)	//����A
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//��ȡ�û��Զ����ַ��ĵ�������
			else
				GetAscIIFlashDotData(AscIICode);		//��ȡ�ֿ��еĵ�������
		}
		else if (g_tCharMaskReadBuf.FontType == ASCII_FONT_B)	//����B
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//��ȡ�û��Զ����ַ��ĵ�������
			else
				GetAscIIFlashDotData(AscIICode);		//��ȡ�ֿ��еĵ�������
		}
		else	//Ĭ��A
		{
			if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
				GetAscIIUserDefDotData(AscIICode);	//��ȡ�û��Զ����ַ��ĵ�������
			else
				GetAscIIFlashDotData(AscIICode);		//��ȡ�ֿ��еĵ�������
		}
	}
	else
	{
		GetAscIIFlashDotData(AscIICode);		//��ȡ�ֿ��еĵ�������
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ,��������ȡ
*	�����Ƿ����Զ����ַ�,ֱ�Ӵ�RAM�и���
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIUserDefDotData(uint8_t AscIICode)	//��ȡ�û��Զ����ַ��ĵ�������
{	
	uint32_t	len;
	uint32_t	StartAddress;
	uint8_t * pselfdef;
	uint8_t * pbuf;
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
		pselfdef += StartAddress;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_B;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_B;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_09;
		pselfdef += StartAddress;
	}
	else		//��ֹ��������,ʹ��Ĭ��ֵ
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
		pselfdef += StartAddress;
	}
	
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//��Ϊ����д��
	pbuf = g_tCharMaskReadBuf.Buf;
	while (len--)
	{
		*pbuf++ = *pselfdef++;
	}
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_ENGLISH;
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
}

#if	defined (DBG_SELF_DEF_CODEPAGE)
void	innerFLASH_ReadBuffer(uint8_t * pChar, uint32_t iAddressBuf, uint32_t	iLen)
{
	uc8	* pbFlash;
	
	if (g_tCharMaskReadBuf.FontType == ASCII_FONT_A)
	{
		pbFlash = CodepageFontA;
		pbFlash += iAddressBuf;
	}
	else if (g_tCharMaskReadBuf.FontType == ASCII_FONT_B)
	{
		pbFlash = CodepageFontB;
		pbFlash += iAddressBuf;
	}
	
	while (iLen--)
	{
		*pChar++ = *pbFlash++;
	}
}
#endif

#ifdef	DBG_025
uc8	mcAscII_0_0917_0[] = 
{
	/*  0[0x0030]   9x17  */
0x0F,0xF8,0x00,0x1F,0xFE,0x00,0x20,0x02,0x00,0x20,0x03,0x00,0x60,0x01,0x00,0x20,
0x03,0x00,0x30,0x06,0x00,0x1F,0xFC,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_2_0917_0[] = 
{
	/*  2[0x0032]   9x17  */
0x08,0x07,0x00,0x18,0x0F,0x00,0x30,0x13,0x00,0x60,0x23,0x00,0x60,0x63,0x00,0x60,
0xC3,0x00,0x31,0x83,0x00,0x1F,0x03,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_0917_0[] = 
{
	/*  5[0x0035]   9x17  */
0x00,0x0C,0x00,0x3F,0x86,0x00,0x21,0x03,0x00,0x21,0x01,0x00,0x21,0x01,0x00,0x21,
0x03,0x00,0x21,0xCE,0x00,0x20,0x78,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_0_0917_1[] = 
{
	/*  0[0x0030]   9x17  */
0x00,0x00,0x00,0x03,0xF8,0x00,0x04,0x04,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,
0x02,0x00,0x04,0x04,0x00,0x03,0xF8,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_2_0917_1[] = 
{
	/*  2[0x0032]   9x17  */
0x00,0x00,0x00,0x07,0x06,0x00,0x08,0x0A,0x00,0x08,0x12,0x00,0x08,0x22,0x00,0x08,
0xC2,0x00,0x07,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_0917_1[] = 
{
	/*  5[0x0035]   9x17  */
0x00,0x00,0x00,0x0F,0xCC,0x00,0x08,0x42,0x00,0x08,0x82,0x00,0x08,0x82,0x00,0x08,
0x82,0x00,0x08,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
uc8	mcAscII_0_1224_0[] = 
{
	/*  0[0x0030]  12x24  */
0x0F,0xFF,0xE0,0x1F,0xFF,0xF0,0x18,0x00,0x18,0x30,0x00,0x0C,0x30,0x00,0x0C,0x30,
0x00,0x0C,0x30,0x00,0x0C,0x30,0x00,0x0C,0x18,0x00,0x18,0x1F,0xFF,0xF0,0x07,0xFF,
0xC0,0x00,0x00,0x00,
};
uc8	mcAscII_2_1224_0[] = 
{
	/*  2[0x0032]  12x24  */
0x06,0x00,0x3C,0x0E,0x00,0xFC,0x18,0x01,0xCC,0x30,0x03,0x8C,0x30,0x07,0x0C,0x60,
0x06,0x0C,0x60,0x0C,0x0C,0x30,0x18,0x0C,0x38,0x30,0x0C,0x1F,0xE0,0x0C,0x0F,0x80,
0x0C,0x00,0x00,0x00,
};
uc8	mcAscII_5_1224_0[] = 
{
	/*  5[0x0035]  12x24  */
0x00,0x00,0x00,0x00,0x00,0x70,0x3F,0xF0,0x38,0x30,0x30,0x0C,0x30,0x60,0x0C,0x30,
0x60,0x0C,0x30,0x60,0x0C,0x30,0x60,0x0C,0x30,0x70,0x18,0x30,0x3C,0x78,0x30,0x1F,
0xE0,0x00,0x00,0x00,
};
uc8	mcAscII_0_1224_1[] = 
{
	/*  0[0x0030]  12x24  */
0x00,0x00,0x00,0x00,0x7F,0x80,0x01,0xFF,0xE0,0x03,0x80,0x70,0x06,0x00,0x18,0x04,
0x00,0x08,0x04,0x00,0x08,0x06,0x00,0x18,0x03,0x80,0x70,0x01,0xFF,0xE0,0x00,0x7F,
0x80,0x00,0x00,0x00,

};
uc8	mcAscII_2_1224_1[] = 
{
	/*  2[0x0032]  12x24  */
0x00,0x00,0x00,0x01,0xC0,0x38,0x02,0xC0,0x58,0x04,0x00,0x98,0x04,0x01,0x18,0x04,
0x02,0x18,0x04,0x04,0x18,0x06,0x1C,0x18,0x03,0xF8,0x18,0x01,0xE0,0xF8,0x00,0x00,
0x00,0x00,0x00,0x00,
};
uc8	mcAscII_5_1224_1[] = 
{
	/*  5[0x0035]  12x24  */
0x00,0x00,0x00,0x00,0x00,0xE0,0x07,0xFC,0xD0,0x06,0x08,0x08,0x06,0x10,0x08,0x06,
0x10,0x08,0x06,0x10,0x08,0x06,0x18,0x38,0x06,0x0F,0xF0,0x06,0x07,0xC0,0x00,0x00,
0x00,0x00,0x00,0x00,
};

void	innerFLASH_ReadBuffer(uint8_t * pChar, uint32_t iAddressBuf, uint32_t	iLen)
{
	uc8	* pbFlash;
	
	if (iAddressBuf == 0x00)	//0,
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_0_1224_0;
			else
				pbFlash = mcAscII_0_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_0_0917_0;
			else
				pbFlash = mcAscII_0_0917_1;
		}
	}
	else if (iAddressBuf == 0x02)	//2,
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_2_1224_0;
			else
				pbFlash = mcAscII_2_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_2_0917_0;
			else
				pbFlash = mcAscII_2_0917_1;
		}
	}
	else if (iAddressBuf == 0x05)	//5
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_5_1224_0;
			else
				pbFlash = mcAscII_5_1224_1;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			if (g_bDbg025FontFlag)
				pbFlash = mcAscII_5_0917_0;
			else
				pbFlash = mcAscII_5_0917_1;
		}
	}
	
	//pbFlash += iAddressBuf;
	
	while (iLen--)
	{
		*pChar++ = *pbFlash++;
	}
}

#endif

/*******************************************************************************
* Function Name  : 
* Description    : ���ASCII�ַ��ĵ�����ʼ��ַ,
*	AscIICode = �ַ�ֵ
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetUSACharacterStartAddress(uint8_t AscIICode)
{
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);

#ifdef	DBG_025
	if (g_bDbg025EnalbeFlag)
	{
		if ((AscIICode == 0x30) || (AscIICode == 0x32) || (AscIICode == 0x35))	//0,2,5
		{
			AsciiCodeAddr = AscIICode & 0x0F;
		}
		else
		{
			if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
			{
				AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			}
			else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
			{
				AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
			}
		}
	}
	else
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		}
	}
#else
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
	}
#endif
	
	return (AsciiCodeAddr);
}

/*֧����������*/
uint32_t	GetUSACharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
	{
		return	(0);
	}
	
	if (FontType == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_09_17_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH��Ӣ��8x16�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_08_16_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_C;
	}
	else
	{
		AsciiCodeAddr = ASCII_12_24_BASE_ADDR + (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
	}
	
	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : ��ù����ַ��ĵ�����ʼ��ַ,�粻�ǹ����ַ��򷵻�ASCII�ַ��ĵ�ַ
*	AscIICode = �ַ�ֵ
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetInternationalCharacterStartAddress(uint8_t	AscIICode)
{
	uint32_t	i;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);
	
	for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//���Ƿ�Ϊ�����ַ�
	{
		if (AscIICode == m_tbCharacterSetTable[i])
			break;
	}
	if (i < BYTES_OF_CHAR_SET_TABLE)							//��Ҫ�滻���ַ�����
	{
		if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
		else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_09_17_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
		}
	}		//��Ҫ�滻�ַ��������
	else
	{
		AsciiCodeAddr = GetUSACharacterStartAddress(AscIICode);
	}		//��ͬ���ַ��������

	return (AsciiCodeAddr);
}

/*֧����������*/
uint32_t	GetInternationalCharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint32_t	i;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < PRINTABLE_START_CODE)
		return	(0);
	
	for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//���Ƿ�Ϊ�����ַ�
	{
		if (AscIICode == m_tbCharacterSetTable[i])
		{
			break;
		}
	}
	
	if (i < BYTES_OF_CHAR_SET_TABLE)							//��Ҫ�滻���ַ�����
	{
		if (FontType == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
		else if (FontType == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_09_17_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
		}
		else if (FontType == ASCII_FONT_C)	//FLASH��Ӣ��8x16�����ASCII�ַ�
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_08_16_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_C;
		}
		else
		{
			AsciiCodeAddr = INTERNATIONAL_CODE_12_24_BASE_ADDR + 
			(g_tSystemCtrl.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
		}
	}		//��Ҫ�滻�ַ��������
	else
	{
		AsciiCodeAddr = GetUSACharStartAddress(AscIICode, FontType);
	}		//��ͬ���ַ��������

	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : ���ָ������ҳ�ַ��ĵ�����ʼ��ַ
*	AscIICode = �ַ�ֵ
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t	GetCodepageCharacterStartAddress(uint8_t	AscIICode)
{
	uint8_t	CodePage;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < CODEPAGE_START_CODE)
		return	(0);
	
	CodePage = g_tSysConfig.DefaultCodePage;		 //���ҵĴ���ҳ��ѡ��
#if	defined (DBG_SELF_DEF_CODEPAGE)
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = (CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		AsciiCodeAddr = (CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
#else
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
#endif

	return (AsciiCodeAddr);
}

/* ֧���������� */
uint32_t	GetCodepageCharStartAddress(uint8_t	AscIICode, uint8_t FontType)
{
	uint8_t	CodePage;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < CODEPAGE_START_CODE)
	{
		return	(0);
	}
	
	CodePage = g_tSysConfig.DefaultCodePage;		 //���ҵĴ���ҳ��ѡ��
	if (FontType == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH��Ӣ��8x16�����ASCII�ַ�
	{
		AsciiCodeAddr = ASCII_08_16_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_C;
	}
	else
	{
		AsciiCodeAddr = ASCII_12_24_CODEPAGE_BASE_ADDR + \
			(CodePage * 128 + (AscIICode - 0x80)) * BYTES_OF_ASCII_FONT_A;
	}
	
	return (AsciiCodeAddr);
}

/*******************************************************************************
* Function Name  : 
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ,��������ȡ
*	�������ֿ�FLASH��,����SPI DMA
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFlashDotData(uint8_t AscIICode)	//��ȡ�ֿ��еĵ�������
{
	uint32_t	len;
	uint32_t	AsciiCodeAddr;
	#ifdef	DBG_PROCESS_TIME
		uint32_t	iDbgTemp;
	#endif
	
	if (AscIICode < 0x80)
	{
		if (g_tSystemCtrl.CharacterSet)	//��������˹����ַ���ָ�� 0x1b 52 n,USA=0
		{
			AsciiCodeAddr = GetInternationalCharacterStartAddress(AscIICode);
		}		//End �����ַ�
		else
		{
			AsciiCodeAddr = GetUSACharacterStartAddress(AscIICode);
		}
	}			//End <0x80
	else	//----FLASH��0x80-0xFF�����ASCII�ַ�----//
	{
		AsciiCodeAddr = GetCodepageCharacterStartAddress(AscIICode);
	}
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)			//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_A;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_A;
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_B;
		g_tCharMaskReadBuf.FontType = ASCII_FONT_B;
	}
	g_tCharMaskReadBuf.LanguageType = LANGUAGE_ENGLISH;
	
	//��������SPI�ֿ�DMA��ȡ��ʼ
	g_tCharMaskReadBuf.Status = CMRB_STATUS_WRITING;	//��Ϊ����д��
	
	#if	defined (DBG_SELF_DEF_CODEPAGE)
		if (AscIICode < 0x80)
		{
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
		else	//Ŀǰֻ�д���ҳ����
		{
			innerFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
	#elif	defined (DBG_025)
		//Ŀǰֻ����0,2,5
		if ((AscIICode == 0x30) || (AscIICode == 0x32) || (AscIICode == 0x35))
		{
			innerFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
		else
		{
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		}
	#else
		#ifdef	DBG_PROCESS_TIME_READ_ZIKU_ONE_ASCII		//��һ��ASCII�ĵ����ֿ����ݺ�ʱ
			StartProcessTimeCounter();
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
			StopProcessTimeCounter();
			iDbgTemp = 0;
			g_iDbgProcessTimeUs = 0;
		#else
			sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, AsciiCodeAddr, len);
		#endif
	#endif
	
	#ifdef	DBG_SPI_ZIKU
		UpLoadData(g_tCharMaskReadBuf.Buf, len);	//�ϴ���PC
		InitCharMaskBufParameter();	//��Ϊ����,�����BUF
	#else
		g_tCharMaskReadBuf.Status = CMRB_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
	#endif

	#ifdef	ZK_ENCRYPTION
		FlashDecryption(g_tCharMaskReadBuf.Buf, len);
	#endif
}

/*******************************************************************************
* Function Name  : FlashDecryption()
* Description    : �ֿ���ģ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	FlashDecryption(uint8_t	* Src, uint32_t ByteNum)
{
	uint32_t	i;
	
	if (ByteNum > MAX_BYTES_OF_HZ)
	{
		return;
	}
	
	for (i = 0; i < ByteNum; i++)
	{
		Src[i] = Src[i] ^ 0x79;        
		Src[i] = ~Src[i];
	}
}

/*******************************************************************************
* Function Name  : PrtAsciiToBuf
* Description    : ��ӡ�������� 2017.03.20
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtAsciiToBuf(uint8_t Code)
{
		PrintAscII(Code);
		while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
		{
			ProcessCharMaskReadBuf();
		}
}	
/*******************************************************************************
* Function Name  : PrintString
* Description    : ��ӡ�ַ���.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrintString(const uint8_t * Str)
{
	uint8_t	Ch;
	uint8_t	bTemp;
	const uint8_t * pStr = NULL;
	
	pStr = Str;
	bTemp = *pStr;
	while (bTemp)
	{
		if ((bTemp == 0x0A) || (bTemp == 0x0D))
		{
// 			PrintOneLine();
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
		else if ((bTemp >= 0x20) && (bTemp < 0x80))
		{
			while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
			{
				;
			}
			PrintAscII( bTemp );			//��������
			while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
			{
				ProcessCharMaskReadBuf();
			}
		}
		else if	(bTemp >= 0x80)
		{
			if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
			{
				Ch = *pStr;
				pStr++;
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintGB18030(Ch, *pStr, 0, 0);
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
			else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)
			{
				Ch = *pStr;
				pStr++;
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintBIG5(Ch, *pStr);
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
			else
			{
				while (g_tCharMaskReadBuf.Status != CMRB_STATUS_ILDE)
				{
					;
				}
				PrintAscII( *pStr );
				while (g_tCharMaskReadBuf.Status == CMRB_STATUS_WAIT_TO_DEAL)
				{
					ProcessCharMaskReadBuf();
				}
			}
		}
		
		pStr++;
		bTemp = *pStr;
	}
#if	0	
	while (*pStr)
	{
		if ((*pStr == 0x0A) || (*pStr == 0x0D))
		{
			PrintOneLine();
		}
		else if ((*pStr >= 0x20) && (*pStr < 0x80))
		{
			PrintAscII( *pStr );			//��������
			ProcessCharMaskReadBuf();
		}
		else if	(*pStr >= 0x80)
		{
			if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
			{
				Ch = *pStr;
				pStr++;
				PrintGB18030(Ch, *pStr, 0, 0);
				ProcessCharMaskReadBuf();
			}
			else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)
			{
				Ch = *pStr;
				pStr++;
				PrintBIG5(Ch, *pStr);
				ProcessCharMaskReadBuf();
			}
			else
			{
				PrintAscII( *pStr );
				ProcessCharMaskReadBuf();
			}
		}
		
		pStr++;
	}
#endif
}

/*******************************************************************************
* Function Name  : GetRatioXValue
* Description    : ���㵱ǰ�ַ��ĺ���Ŵ���
*				   
* Input          : CodeType: =0,����,=1����
* Output         : 
* Return         : ���غ���Ŵ���
*******************************************************************************/
static uint8_t	GetRatioXValue(uint8_t CodeType)
{
	uint8_t	RatioX;
		
	if (CodeType)	//���ַ�ʽ,ȡ����Ŵ���
	{
		if ((g_tPrtModeHZ.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeHZ.WidthMagnification;
		}		
	}
	else					//�ַ���ʽ
	{
		if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeChar.WidthMagnification;
		}		
	}
	
	return RatioX;
}

/*******************************************************************************
* Function Name  : JudgeLineBufFull
* Description    : �ж��Ƿ��ܷ��¸��ַ�,����,��ӡ��ǰ��.
* Input          : CodeType: =0,����,=1����
* Output         : 
* Return         : �ܷ��·���0,���ܷ��·���1
*******************************************************************************/
uint8_t	JudgeLineBufFull(uint8_t CodeType)
{
	uint8_t	RatioX;
	uint8_t	Result;
	uint16_t	Width;
	
	if (CodeType)	//���ַ�ʽ,ȡ����Ŵ���
	{
		if ((g_tPrtModeHZ.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeHZ.WidthMagnification;
		}
		
		if (g_tPrtCtrlData.CharCycle90)
		{
			Width = RatioX * g_tPrtModeHZ.CharHigh;
		}
		else
		{
			Width = RatioX * (g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth);
		}
	}
	else					//�ַ���ʽ
	{
		if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = g_tPrtModeChar.WidthMagnification;
		}
		
		if (g_tPrtCtrlData.CharCycle90)
		{
			Width = RatioX * g_tPrtModeChar.CharHigh;
		}
		else
		{
			Width = RatioX * (g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth + g_tPrtModeChar.RightSpace);    //����ַ�ʱ�� ��Ҫ�������м��
		}
	}		//�����ȵ���
	#if	1
	//���������,��֤���õ���߾������ܴ�ӡһ�ַ�
	if ((g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
			((g_tPrtCtrlData.PrtDataDotIndex + Width) >= g_tPrtCtrlData.PrtLength))
	{
		if (g_tPrtCtrlData.PrtLength > Width)
		{
			g_tPrtCtrlData.PrtLeftLimit = g_tPrtCtrlData.PrtLength - Width;
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
		}
	}
	#endif
	
	if ((g_tPrtCtrlData.PrtDataDotIndex + Width) > g_tPrtCtrlData.PrtLength)
	{
		Result = LINE_BUF_FULL;
	}
	else
	{
		Result = LINE_BUF_NOT_FULL;
	}
	
	return (Result);
}

/*******************************************************************************
* Function Name  : FillDataPrintBuff(uint8_t Data, uint8_t Width, uint8_t Line)
* Description    : ��ӡ�����������ͬ������.
* Input          : Data����������, Width�����ݿ��,Line:����
* Output         : �ı����ָ��
* Return         : 
*******************************************************************************/
void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line)
{
	uint32_t	i,j;
	uint8_t	* p = NULL;
	uint8_t	* ptemp = NULL;
	
	if (Line <= MAX_PRT_HEIGHT_BYTE)		//��ֹ����������������±�Խ��
	{
		//���ҵ��е��׵�ַ
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - Line) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		for (i = 0; i < Line; i++)
		{
			//p = ptemp + i * PRT_DOT_NUMBERS;
			p = ptemp;
			for (j = 0; j < Width; j++)
			{
				*p++ = Data;
			}
			ptemp += PRT_DOT_NUMBERS;
		}
	}
	
	p = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t RatioX ,uint8_t CodeType)
* Description    : ָ����ַ���ݴ����ӡ������.
* Input          : *Source���������ݵ�ַ, CharWidth���ַ����, RatioX:������, 
*										CodeType: �ַ����� =1����,=0�ַ�
* Output         : �ı����ָ��
* Return         : 
*******************************************************************************/
void	FillPrintBuff(uint8_t * Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t CodeType)
{
	uint8_t	Line;
	uint8_t	RatioY; 
	uint8_t	Ratio;
	uint8_t	i;
	uint8_t	Temp;
	uint8_t	RatioX;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	iTemp;
	uint16_t	Width;
	uint16_t	LeftSpace;
	uint16_t	RightSpace;
	uint8_t	* p = NULL;
	uint8_t	* ptemp = NULL;
	
	if (CharWidth > MAX_FONT_WIDTH)
		CharWidth = MAX_FONT_WIDTH;
	if (CharHigh > MAX_FONT_HEIGHT)
		CharHigh = MAX_FONT_HEIGHT;
	
	RatioX = GetRatioXValue(CodeType);
	
	//�ַ���.�Ҽ��,����Ŵ�����
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)			//��ģʽ
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//�ַ�
		{
			LeftSpace		= RatioX * g_tPrtModeChar.LeftSpace;
			RightSpace	= RatioX * g_tPrtModeChar.RightSpace;
			RatioY			= g_tPrtModeChar.HeightMagnification;
			if (g_tPrtCtrlData.MaxRatioY < g_tPrtModeChar.HeightMagnification)
				g_tPrtCtrlData.MaxRatioY = g_tPrtModeChar.HeightMagnification;
			
			#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
			if (g_tPrtCtrlData.MaxRatioX < g_tPrtModeChar.WidthMagnification)
				g_tPrtCtrlData.MaxRatioX = g_tPrtModeChar.WidthMagnification;
			#endif
		}
		else		//����
		{
			LeftSpace		= RatioX * g_tPrtModeHZ.LeftSpace;
			RightSpace	= RatioX * g_tPrtModeHZ.RightSpace;
			RatioY			= g_tPrtModeHZ.HeightMagnification;
			if (g_tPrtCtrlData.MaxRatioY < g_tPrtModeHZ.HeightMagnification)
				g_tPrtCtrlData.MaxRatioY = g_tPrtModeHZ.HeightMagnification;
			
			#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
			if (g_tPrtCtrlData.MaxRatioX < g_tPrtModeHZ.WidthMagnification)
				g_tPrtCtrlData.MaxRatioX = g_tPrtModeHZ.WidthMagnification;
			#endif
		}
	}
	else			//ҳģʽ
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//�ַ�
		{
			LeftSpace	 = 0;
			RightSpace = RatioX * g_tPageMode.AscRightSpace;
			RatioY		 = g_tPrtModeChar.HeightMagnification;
		}
		else		//����
		{
			LeftSpace	 = RatioX * g_tPageMode.HZLeftSpace;
			RightSpace = RatioX * g_tPageMode.HZRightSpace;
			RatioY		 = g_tPrtModeHZ.HeightMagnification;
		}
	}
	
	if (g_tPrtCtrlData.LineHigh < CharHigh)	//һ��������ַ�����
	{
		g_tPrtCtrlData.LineHigh = CharHigh;		//�����ַ��߶�
	}
	if (g_tPrtCtrlData.MaxPrtDotHigh < CharHigh * RatioY)	//һ������ߴ�ӡ��������
	{
		g_tPrtCtrlData.MaxPrtDotHigh = CharHigh * RatioY;	//��ʱ����
	}
	
	
	//��������
	if (g_tPrtCtrlData.CharNotFlag == 1)		//���״�ӡ(����)
	{
		FillDataPrintBuff(0xFF, LeftSpace, 3 * RatioY);
	}
	g_tPrtCtrlData.PrtDataDotIndex += LeftSpace;		//�����м���ֵ
	
	//�����ַ�����
	//����24�������ֳ�3��,ÿ�δ���1��,����ÿ��ʱ������,ÿ�δ���1���ֽ�,
	//ÿ�Ŵ���ǰָ������׵�ַ
	ptemp = g_tLineEditBuf + (MAX_RATIO_Y - RatioY) * 3 * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
	for (Line = 0; Line < 3; Line++)
	{
		p = ptemp + RatioY * Line * PRT_DOT_NUMBERS;
		for (Width = 0; Width < CharWidth; Width++)	//�ַ���
		{
			Temp = Source[3 * Width + Line];					//���δ���ÿ��,��3��,��Ϊ�ַ����24���
			if (Temp)	//��Ϊ��
			{
				if (RatioY == 1)
				{
					TempBuf[0] = Temp;
				}
				else
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);	//�ֽڴ�������ʹ��ǰ�����
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, Temp, TempBuf);	//����Ŵ���
				}
			}
			else	//Ϊ��
			{
				if (RatioY == 1)
				{
					TempBuf[0] = Temp;
				}
				else
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				}
			}
			
			for (Ratio = 0; Ratio < RatioX; Ratio++)
			{
				for (i = 0; i < RatioY; i++)
				{
					if (g_tPrtCtrlData.CharNotFlag == 0)
						*(p + PRT_DOT_NUMBERS * i) = TempBuf[i];
					else
						*(p + PRT_DOT_NUMBERS * i) = (uint8_t)(~TempBuf[i]);	//���״�ӡ
				}		//End ����Ŵ���
				p++;
			}			//End ����Ŵ���
		}			 	//Een �ַ���
	}				 	//End �ַ���
	g_tPrtCtrlData.PrtDataDotIndex += (RatioX * CharWidth);	//�����м���ֵ
	
	//�����Ҽ��
	//�����Ҽ����,��֤�ܷ��¸��ַ�,�ַ����Ҽ�� < �ɴ�ӡ��
	if ((g_tPrtCtrlData.PrtDataDotIndex + RightSpace) > g_tPrtCtrlData.PrtLength)
		RightSpace = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
	if (g_tPrtCtrlData.CharNotFlag == 1)		//�����״�ӡ(����)
	{
		FillDataPrintBuff(0xFF, RightSpace, 3 * RatioY);
	}
	g_tPrtCtrlData.PrtDataDotIndex += RightSpace;	//�����м���ֵ
	
	//�»������,���� / ��ת90 ��ӡʱ����ӡ�»���
	#ifdef	UNDER_LINE_PRT_ENABLE		//2012-9-21��Ӵ����»��ߴ�ӡ������
	if ((g_tPrtCtrlData.CharNotFlag == 0) && (g_tPrtCtrlData.CharCycle90 == 0))
	{
		Temp = 0;
		if (CodeType == LANGUAGE_ENGLISH)	  //�ַ�
		{
			if (g_tPrtModeChar.Underline & UNDERLINE_ENABLE_MASK)
				Temp = g_tPrtModeChar.Underline & 0x07;
		}
		else				  //����
		{
			if (g_tPrtModeHZ.Underline & UNDERLINE_ENABLE_MASK)
				Temp = g_tPrtModeHZ.Underline & 0x07;
		}
		
		if (Temp)	//�»��ߵ�����Ϊ0
		{
			if (g_tPrtCtrlData.DoUnderlineFlag == 0)
			{
				g_tPrtCtrlData.DoUnderlineFlag = 1;
			}
			
			if (Temp == 0x07)				//3���
			{
				g_tPrtCtrlData.UnderlineMaxPrtHigh = 3;
			}
			else if (Temp == 0x03)	//2���
			{
				//if (g_tPrtCtrlData.UnderlineMaxPrtHigh < Temp)
				{
					g_tPrtCtrlData.UnderlineMaxPrtHigh = 2;
				}
			}
			else if (Temp == 0x01)	//1���
			{
				//if (g_tPrtCtrlData.UnderlineMaxPrtHigh < Temp)
				{
					g_tPrtCtrlData.UnderlineMaxPrtHigh = 1;
				}
			}
			//2012-9-21��ӽ���
			iTemp = LeftSpace + (RatioX * CharWidth) + RightSpace;	//ǰ��ȫ��������
			p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex - iTemp;
			for (Width = 0; Width < iTemp; Width++)
			{
				*p++ |= Temp;
			}
		}		//�»��ߵ�����Ϊ0�޸Ľ���
		else
		{
			if((g_tPrtModeChar.Underline & UNDERLINE_ENABLE_MASK == 0) && (g_tPrtModeHZ.Underline & UNDERLINE_ENABLE_MASK == 0))
			{
				g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;			//2016.07.09
			}	
		}	
	}		//�»��ߴ������
	#endif
	
	p = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : void ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t DscPrt[])
* Description    : ��һ�ֽڵ����ݽ��зŴ���
* Input          :	ZoomRate:�Ŵ���
*									ByteData: ԭʼ����
*									DscPrt[]:�������ָ��
* Output         : 
* Return         : 
* �ر�˵����	 ��
*******************************************************************************/
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t *DscPrt)
{
	uint8_t	Temp0, i;
	unsigned long long	Temp1;
	
	typedef union
	{
		unsigned long long	Temp;
		uint8_t	bBuf[sizeof(unsigned long long)];
	}Temp_Union;
	
	Temp_Union	TempUnion;
	
	if (ZoomRate > 8)	//�жϷŴ����Ƿ񳬹������ֵ  //2017.08.25 MAX_RATIO_Y��Ϊ8
	{
		return;
	}
	
	if (ZoomRate < 2)		//�Ŵ���Ϊ1ʱ
	{
		DscPrt[0] = ByteData;
	}
	else
	{
		Temp0 = ByteData;
		Temp1 = 0xFF >> (8 - ZoomRate);
		TempUnion.Temp = 0;
		for (i = 0; i < 8; i++)
		{
			if (Temp0 & 0x01)
				TempUnion.Temp |= Temp1 ;
			Temp1 <<= ZoomRate;
			Temp0 >>= 1;
		}
		
		for (i = 0; i < ZoomRate; i++)
			DscPrt[i] = TempUnion.bBuf[ZoomRate - 1 - i];
			//DscPrt��ַ�ʹ�ŵ����ֽڸ�,��ַ�ߴ�ŵ��ֽڵ�
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : �������һ���ַ��ڸ߶��Ͻ��б�������,Ŀǰ�ĳ���ֻ���������Ŵ�
*	�Ŵ�ʱ�����Ŵ���������.�ڴ������ڿ��Խ������ⱶ���ķŴ�.�Ŵ������ʱ,
*	ָ��ָ�����ʼ��ַ��ŵ����ַ������MSB���ַŴ��任�Ľ��,���յ�ַ��ŵ���
*	�ַ������LSB���ַŴ��任�Ľ��.�Ŵ���ַ�����Ϊ1�ĵ������Ŀû�б仯,����
*	��ӡ�������Եøߵ���ϡ��.
*	������㷨����MSB��ʼ����ȡ���ֽڵ�ĳ1λ,���Ϊ1,���ڶ�Ӧ���λ�ô���1,Ȼ��
*	����0.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt)
{
	uint32_t	i;
	unsigned long long temp;		//��ű任����,�Ŵ�n����Ҫn���ֽڿռ�,��ǰ���÷����ɷŴ�8��
	
	if (ZoomRate > MAX_RATIO_Y)	//�жϷŴ����Ƿ񳬹������ֵ
	{
		return;
	}
	
	if (ZoomRate == 1)		//�Ŵ���Ϊ1ʱ,����Ҫ���д���
	{
		DscPrt[0] = ByteData;
	}
	else									//�Ŵ��� > 1ʱ
	{
		temp = 0;
		for (i = 0; i < 8; i++)		//�Ӹ��ֽڵ�MSB��ʼ����ÿλ��ֵ
		{
			if ((ByteData >> 7) == 1)	//�жϸ��ֽ�MSBλ�Ƿ�Ϊ1
			{
				temp |= (1<<(ZoomRate * (8 - i) - 1));	//���Ŵ�����Ķ�Ӧλ��1
			}
			ByteData <<= 1;					//ȥ��ǰ�洦�����λ,������������λ�Ƶ����λ
		}
		//������Ϻ�,temp��(ZoomRate * 8 - 1)λ��ʼ��ŵ���ByteData��MSB��ʼ�����Ľ��.
		for (i = 0; i < ZoomRate; i++)	//��ͬ�Ŵ���ʱռ�õ���λ�β�ͬ
		{
			DscPrt[i] = (uint8_t)(temp >> (8 * (ZoomRate - i - 1)));
		}
	}

}

/*******************************************************************************
* Function Name  :	EmphaSizeProcess(uint8_t CodeWidth,uint8_t CodeHigh, uint8_t *SrcPrt)
* Description    : 	���ַ�����ģ���ݼӴ�
* Input          : 	CodeWidth���ַ�����ߴ�, ʹ�õ�Ϊ��λ
* Input          : 	CodeHigh���ַ�����ߴ�,ʹ�õ�Ϊ��λ
*				   					SrcPrt[]����������ָ��
* Output         : 	SrcPrt[]���Ӵֺ���������
* Return         : 	None
* �ر���ʾ��	 ��		�������ݰ���׼����
*	ʹ��9*17��12*24�����ַ�����ת����Ϊ90��ʱ,��������ֽڻ���������ֽ�,�ڵ���ʱ,
*	ע�Ᵽ֤SrcPrt[]�Ĵ�С,���ײ����뷽ʽ�����������
*	�д�����
*******************************************************************************/
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
{
	uint8_t	ArrayBuf;
	uint8_t	DataBuf;
	uint8_t	i;
	uint8_t	j;
	
	if (CodeWidth > MAX_FONT_WIDTH)
	{
		CodeWidth = MAX_FONT_WIDTH;
	}
	if (CodeHigh > MAX_FONT_HEIGHT)
	{
		CodeHigh = MAX_FONT_HEIGHT;
	}
	
	for (i = 0; i < CodeHigh; i++)
	{
		ArrayBuf = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			DataBuf = SrcPrt[j * CodeHigh + i];
			SrcPrt[j * CodeHigh + i] |= ArrayBuf;
			ArrayBuf = DataBuf;
		}
	}
}

/*******************************************************************************
* Function Name  : uint8_t	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh,  uint8_t SrcPrt[])
* Description    : ��ָ���ַ�����ģ������ת˳ʱ��90��ĳ���
* Input          : CodeWidth���ַ�����ߴ�, ʹ�õ�Ϊ��λ
* Input          : CodeHigh���ַ�����ߴ�,ʹ�õ�Ϊ��λ
*				   SrcPrt[]����������ָ��
* Output         : SrcPrt[]����ת����������
* Return         : ��ת�������������� 
* �ر���ʾ��	 ���������ݰ���׼����
				   ʹ�� 9*17 �� 12*24 �����ַ�����ת����Ϊ90��ʱ.
					��������ֽڻ���������ֽ�,�ڵ���ʱ,ע�Ᵽ֤SrcPrt[]�Ĵ�С
				   ���ײ����뷽ʽ�����������
*	�д�����
*******************************************************************************/
void	CodeRollCtrl(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[])
{
	uint8_t i, j;
	uint8_t	HighByte;
	uint8_t	bBuf[MAX_BYTES_OF_HZ], bBuf1[MAX_BYTES_OF_HZ];
	uint32_t	ChangedByte;
	uint8_t	* ptr = NULL;
	
	if (CodeWidth > MAX_FONT_WIDTH)
		CodeWidth = MAX_FONT_WIDTH;
	if (CodeHigh > MAX_FONT_HEIGHT)
		CodeHigh = MAX_FONT_HEIGHT;
	
	memset(bBuf, 0x00, MAX_BYTES_OF_HZ);
  memset(bBuf1, 0x00, MAX_BYTES_OF_HZ);
	//memset(bBuf, 0x00, sizeof(bBuf) / sizeof(bBuf[0]));
	//memset(bBuf1, 0x00, sizeof(bBuf1) / sizeof(bBuf1[0]));
	HighByte = CodeHigh >> 3;
	if (CodeHigh % 8)
		HighByte++;
	for (i = 0; i < CodeWidth * HighByte; i++)
		bBuf[i] = SrcPrt[i];
	
	HighByte--;
 	ptr = bBuf1;
	for (i = 0; i < CodeHigh; i++)		//˳ʱ��90��
	{
		ChangedByte = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			ChangedByte <<= 1;
			//�ȰѺ����������ֽڵĵ�λȡ��������ת����ĵ�λ��
			//if (bBuf[HighByte - i / 8 + j * 3] & (0x01 << (i % 8)))
			if (bBuf[HighByte - (i >> 3) + j + (j << 1)] & (uint8_t)(0x01 << (i % 8)))
				ChangedByte |= 0x01;
		}		
		*ptr++ = (ChangedByte >> 16);
		*ptr++ = (ChangedByte >> 8);
		*ptr++ = ChangedByte;
	}
	memcpy(SrcPrt, bBuf1, MAX_BYTES_OF_HZ);
	
	ptr = NULL;
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
