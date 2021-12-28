/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.1
  * @date    2016-4-2
  * @brief   This file provides all the firmware functions.
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
#include	"print.h"
#include	"heat.h"
#include	"extgvar.h"
#include	"timconfig.h"
#include	"spiheat.h"
#include	"feedpaper.h"
#include	"defconstant.h"
#include	"debug.h"
#include	"parallel.h"
#include	"led.h"
#include	"beep.h"  //2016.06.24
extern	void LongToStr(uint32_t num, uint8_t *Str);
extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);

extern	uc16	MaxStepNum[];
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern  const uint16_t g_tFeedMotorRushTable[];

extern	TypeDef_StructInBuffer volatile g_tInBuf;  //2016.06.08

#ifdef	DBG_PROCESS_TIME
extern	volatile uint32_t	g_iDbgProcessTimeUs;
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//���������д�ӡ������
//TypeDef_StructLinePrtBuf	g_tLinePrtBuf;
uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];	//���Ȼ�оһ�������еļ��ȼĴ�����Ԫ��

#ifdef	SPI_PRT_TX_BY_DMA_ENABLE	//ȫ��ʹ��DMA��ʽ����
	volatile uint8_t	g_bPrtSPIDMATxStatusFlag;
	volatile uint8_t	g_bPrtSPIDMAIntForDriveMode;
#endif

//static uint8_t m_bReversePrintUnderlineFlag = 0;
static uint8_t ReverseByte(uint8_t cSrc);
static void ReverseStreamEachBit(uint8_t * pbBuf, uint32_t iNumber);
static uint16_t	GetLinePrtBufStartColumnIndex(void);
static void	FillHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines);
static void	LineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines);
static void	LineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines);
static void	SelectLineBufToPrint(void);
static void	WaitToPrintLineBufOfChar(void);
static void	WaitToPrintLineBufOfGraphic(void);
static void	WaitToPrintLineBuf(void);
static void	StartPrintLineBufChar(void);
static void	StartPrintLineBufGraphic(void);
static void	StartPrintLineBuf(void);
static void	CheckPrintContentFinishOfChar(void);
static void	CheckPrintContentFinishOfGraphic(void);
static void	CheckPrintContentFinish(void);
static void StartHeatAndRotateMotor(void);
static void SetGoLineSpace(void);
static void	UnderLineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines);
static void	UnderLineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines);
static void	FillUnderLineToHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines);

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�����͵���о��һ��������,
*	��ǰ��о��Ҫ�ܵ���640,ʵ�ʿɼ��ȵ���576,���Ҹ�32��ֵΪ�޷���������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitHeatElementBuf(void)
{
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
}

void	InitHeatElementBufLeftEmpty(void)
{
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ���д�ӡ������					
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#if	1
void	InitLinePrtBuf(void)
{
	uint8_t i,j;
	
	#if	0
	uint8_t	* pb = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	pb = ptLPB->Buf;
	if (ptLPB->PrtMode.DoUnderlineFlag)	//������»��ߴ�ӡ��ȫ������
	{
		memset(pb, 0x00, LINE_PRT_BUF_LEN);
	}
	else	//�������������»��߻�����
	{
		memset(pb, 0x00, BYTES_OF_LPB);
	}
	
	ptLPB->Status	=	LPB_STATUS_ILDE;
	pb = NULL;
	ptLPB = NULL;
	
	if (U_PrtBuf.g_tLinePrtBuf.PrtMode.DoUnderlineFlag)	//������»��ߴ�ӡ��ȫ������
	{
		memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, \
		sizeof(U_PrtBuf.g_tLinePrtBuf.Buf) / sizeof(g_tLinePrtBuf.Buf[0]));
		U_PrtBuf.g_tLinePrtBuf.PrtMode.DoUnderlineFlag = 0;
	}
	else	//�������������»��߻�����
	{
		memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, BYTES_OF_LPB);
	}
	#endif
	
	//2016.06.16 ��ʼ����ӡ������
	U_PrtBufCtrl.IdxWR = 0;				  //���ջ������ֽ���
	U_PrtBufCtrl.IdxRD =0;				  //���ջ�����
	U_PrtBufCtrl.BufHeight =0;			//���ջ�����
	
	for(i=0;i<DrvPrtBufDeep;i++)		//2016.06.27  ����д�ӡ������
	{
		for(j=0;j<BYTES_OF_ALL_ELEMENT;j++)
		{
			U_PrtBuf.DrivePrintBuf.DdotLine[i].Buf[j] = 0x00;
		}
	}
}
#else
void	InitLinePrtBuf(void)
{
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	* p = NULL;
	uint8_t	* pbSrc = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	p = ptLPB->Buf;
	
	//���㱾��������ַ�ռ�õ��������ֽ���,��8�ֽڶ���,����8�ֽڰ�����8�ֽڼ���
	if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
	{
		ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3) + 1;
	}
	else
	{
		ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3);
	}
	//���ݷŴ���������ʼλ��,������׵�ַ���ֽ���
	StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + ptLPB->PrtMode.PrtLeftLimit;
	ByteLen = ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit;		//һ�ֽ��еĳ���
	pbSrc = p + StartPositon;
	for (i = 0; i < ColumnByteNumber; i++)
	{
		memset(pbSrc, 0x00, ByteLen);	//����д�ӡ����������
		pbSrc += PRT_DOT_NUMBERS;
	}
	
	if (ptLPB->PrtMode.DoUnderlineFlag)	//������»��ߴ�ӡ��ȫ������
	{
		pbSrc = p + BYTES_OF_LPB + ptLPB->PrtMode.PrtLeftLimit;	//����߽翪ʼ
		memset(pbSrc, 0x00, (ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit));	//����»�������
	}
	
	ptLPB->Status	=	LPB_STATUS_ILDE;
	//���Ʊ����ڴ�ӡ��һ��֮ǰ�ᱻ���¸�ֵ,�ڴ�ӡ��ǰ�����Ѿ��ò���,��������ı�.	
}
#endif

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ���д�ӡ������,���ڴ�ӡ��������ʹ����ɺ����,���ݷŴ���
*	�������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	ClearLinePrtBuf(void)
{
#if	0
	uint16_t	ColumnByteNumber;
	uint16_t	StartPositon;
	uint16_t	ByteLen;
	uint32_t	i;
	uint8_t	* p = NULL;
	uint8_t	* pbSrc = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	p = ptLPB->Buf;
	if ((ptLPB->PrtMode.PrtDataDotIndex >= ptLPB->PrtMode.PrtLeftLimit) && \
			(ptLPB->PrtMode.PrtDataDotIndex <= PRT_DOT_NUMBERS))	//ȷ��������Խ��
	{
		//���㱾��������ַ�ռ�õ��������ֽ���,��8�ֽڶ���,����8�ֽڰ�����8�ֽڼ���
		if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
		{
			ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3) + 1;
		}
		else
		{
			ColumnByteNumber = (ptLPB->PrtMode.MaxPrtDotHigh >> 3);
		}
		if (ColumnByteNumber > MAX_PRT_HEIGHT_BYTE)
			ColumnByteNumber = MAX_PRT_HEIGHT_BYTE;
		
		ptLPB->PrtMode.MaxPrtDotHigh = 0;
		//���ݷŴ���������ʼλ��,������׵�ַ���ֽ���
		StartPositon = (MAX_PRT_HEIGHT_BYTE - ColumnByteNumber) * PRT_DOT_NUMBERS + ptLPB->PrtMode.PrtLeftLimit;
		ByteLen = ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit;		//һ�ֽ��еĳ���
		pbSrc = p + StartPositon;
		for (i = 0; i < ColumnByteNumber; i++)
		{
			memset(pbSrc, 0x00, ByteLen);			//����д�ӡ����������
			pbSrc += PRT_DOT_NUMBERS;
		}
		
		if (ptLPB->PrtMode.DoUnderlineFlag)	//������»��ߴ�ӡ��ȫ������
		{
			ptLPB->PrtMode.DoUnderlineFlag = 0;
			ptLPB->PrtMode.UnderlineMaxPrtHigh = 0;
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
			pbSrc = p + BYTES_OF_LPB + ptLPB->PrtMode.PrtLeftLimit;	//����߽翪ʼ
			memset(pbSrc, 0x00, (ptLPB->PrtMode.PrtDataDotIndex - ptLPB->PrtMode.PrtLeftLimit));	//����»�������
		}
	}		//����δԽ�紦�����
	else	//���Խ��̶�ȫ���
	{
		memset(p, 0x00, BYTES_OF_LPB);			//����д�ӡ����������
		if (ptLPB->PrtMode.DoUnderlineFlag)	//������»��ߴ�ӡ��ȫ������
		{
			ptLPB->PrtMode.DoUnderlineFlag = 0;
			ptLPB->PrtMode.UnderlineMaxPrtHigh = 0;
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
			pbSrc = p + BYTES_OF_LPB;
			memset(pbSrc, 0x00, PRT_DOT_NUMBERS);	//����»�������
		}
	}
	
	p = NULL;
	pbSrc = NULL;
	ptLPB = NULL;
#endif
	memset(U_PrtBuf.g_tLinePrtBuf.Buf, 0x00, LINE_PRT_BUF_LEN);
}

//��һ���д�ӡ����������,��ֵ��ӡ���Ʊ������д�ӡ���������ƽṹ.
void	SetLinePrtModeVariable(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	
	ptLPB->PrtMode.PrtDataDotIndex	=	g_tPrtCtrlData.PrtDataDotIndex;
	ptLPB->PrtMode.CodeLineHigh			=	g_tPrtCtrlData.CodeLineHigh;	//�����ַ��߶Ⱥ��м��ֵ֮��
	ptLPB->PrtMode.PrtLeftLimit			=	g_tPrtCtrlData.PrtLeftLimit;
	ptLPB->PrtMode.PrtWidth					=	g_tPrtCtrlData.PrtWidth;
	ptLPB->PrtMode.PrtLength				=	g_tPrtCtrlData.PrtLength;
	ptLPB->PrtMode.MaxPrtDotHigh 		= g_tPrtCtrlData.MaxPrtDotHigh;
	
	ptLPB->PrtMode.PrtModeFlag			=	g_tSystemCtrl.PrtModeFlag;
	ptLPB->PrtMode.BitImageFlag			=	g_tPrtCtrlData.BitImageFlag;
	ptLPB->PrtMode.VariableBitImageFlag	=	g_tPrtCtrlData.VariableBitImageFlag;
	ptLPB->PrtMode.MaxLineHigh			=	g_tPrtCtrlData.LineHigh;			//����ַ����и�,���յ�������
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	ptLPB->PrtMode.MaxRatioX				=	g_tPrtCtrlData.MaxRatioX;
	#endif
	ptLPB->PrtMode.MaxRatioY				=	g_tPrtCtrlData.MaxRatioY;
	ptLPB->PrtMode.CodeAimMode			=	g_tPrtCtrlData.CodeAimMode;	
	ptLPB->PrtMode.PrintDirection		=	g_tPrtCtrlData.LineDirection;
#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
	ptLPB->PrtMode.CharNotFlag			= g_tPrtCtrlData.CharNotFlag;
#endif
#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
	if (g_tPrtModeChar.EmphaSizeMode || g_tPrtModeHZ.EmphaSizeMode)
		ptLPB->PrtMode.EmphaSizeMode	= 1;
	else
		ptLPB->PrtMode.EmphaSizeMode	= 0;
#endif
	ptLPB->PrtMode.CharCycle90			= g_tPrtCtrlData.CharCycle90;
	ptLPB->PrtMode.CharHeightMagnification	=	g_tPrtModeChar.HeightMagnification;
	ptLPB->PrtMode.HZHeightMagnification		=	g_tPrtModeHZ.HeightMagnification;
	ptLPB->PrtMode.PrintLineIndex		=	0;
	
	ptLPB->PrtMode.DoUnderlineFlag	=	g_tPrtCtrlData.DoUnderlineFlag;
	ptLPB->PrtMode.UnderlineMaxPrtHigh = g_tPrtCtrlData.UnderlineMaxPrtHigh;
	ptLPB->PrtMode.UnderlinePrintIndex = 0;
	
	ptLPB = NULL;
}

/* extern functions ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ô�ӡ��������ǰ״̬
*									�����л������Ĵ�ӡ����,����ȫ�ֱ���g_tLinePrtBuf
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
uint32_t	GetPrintStatus(void)
{
	return	(U_PrtBuf.g_tLinePrtBuf.Status);
}

/*******************************************************************************
* Function Name  : CopyDotLineDataToHeatElementBuf()
* Description    : ���ư��պ�������õ�72�ֽ����ݵ��������ݻ�����.
* Input          : ���ӡ��о���ȵ�Ԫ���Ӧ���ַ�������.�ַ���������0��ʼ�����ֽ�,
*	��MSB��LSB����ȵ�Ԫ������һһ��Ӧ.���Ȼ�����Ϊ80�ֽ�.
* Output         : None
* Return         : None
*******************************************************************************/
void	CopyDotLineDataToHeatElementBuf(const uint8_t * pBuf)
{
	uint8_t	* pb = NULL;
	uint32_t	i;
	
	InitHeatElementBuf();												//���ԭֵ
	pb = g_tbHeatElementBuf;
	pb += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;			//��ʵ�ʿɼ��ȵ�λ�ÿ�ʼ
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pb++ = *pBuf++;
	}
	
	pb = NULL;
}

/*******************************************************************************
* Function Name	: IncDrvPrtBufAddr(u8 status, u16 ActiveDots)
* Description	: ����������ӡ���������� 2016.06.16
* Input			: status:�õ��д���״̬��ActiveDots����Ч��ӡ����
* Output		: None
* Return		: None
*******************************************************************************/
void IncDrvPrtBufAddr(u8 status, u16 ActiveDots)
{
	U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].ActiveDots= ActiveDots;   //��Ч��ӡ����

// 	NVIC_DisableIRQ(TIM5_IRQn);
// 	U_PrtBufCtrl.BufHeight++;									 //�޸���Ҫ��ӡ�ĵ�����	 2016.07.10
// 	NVIC_EnableIRQ(TIM5_IRQn);
	
	U_PrtBufCtrl.IdxWR++;										   //д��ָ��	
	if (U_PrtBufCtrl.IdxWR >= DrvPrtBufDeep)	 //2016.06.28 ��������ram��˴���Ҫ�޸�
	{	
		U_PrtBufCtrl.IdxWR = 0x00;
	}
}
/*******************************************************************************
* Function Name	: PrtDrvBufFull(void)
* Description	: �ȴ�������ӡ���������ݼ��ٵ�10���� 
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void PrtDrvBufFull(void)
{
// 	while (U_PrtBufCtrl.BufHeight  >= (DrvPrtBufDeep - 10));//�ȴ��������� 
	uint16_t Temp=0, IdxRD;			//2016.07.10
	
	while(Temp <5)
	{
		IdxRD =U_PrtBufCtrl.IdxRD;		//�ж��п��ܸı���ֵ
		
		if(U_PrtBufCtrl.IdxWR >=IdxRD )
			Temp =DrvPrtBufDeep -U_PrtBufCtrl.IdxWR + IdxRD;
		else
			Temp = IdxRD - U_PrtBufCtrl.IdxWR;
	}	 
}

/*******************************************************************************
* Function Name  : DotLinePutDrvBuf()
* Description    : ��������ӡ��һ����������������ӡ������
* Input          : ���ӡ��о���ȵ�Ԫ���Ӧ���ַ�������.�ַ���������0��ʼ�����ֽ�,
*	��MSB��LSB����ȵ�Ԫ������һһ��Ӧ.���Ȼ�����Ϊ76�ֽ�.
* Output         : None
* Return         : None
*******************************************************************************/
void	DotLinePutDrvBuf(uint8_t * pBuf)
{
	uint16_t	i, j, HeatDots = 0;
	uint8_t		mask = 0x01;
	
	PrtDrvBufFull();
	
	for (i=0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (*pBuf & (mask << j))
				HeatDots++;
		}
		U_PrtBuf.DrivePrintBuf.DdotLine[U_PrtBufCtrl.IdxWR].Buf[i] = *pBuf++;
	}
	IncDrvPrtBufAddr(0x00, HeatDots);	
}
/*******************************************************************************
* Function Name  : SendHeatElementBufToCore()
* Description    : ����������õ�80�ֽ����ݵ���о���ռĴ���.
* Input          : ���ӡ��о���ȵ�Ԫ���Ӧ���ַ�������.�ַ���������0��ʼ�����ֽ�,
*	��MSB��LSB����ȵ�Ԫ������һһ��Ӧ.���Ȼ�����Ϊ80�ֽ�.
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef	SPI_PRT_TX_BY_DMA_ENABLE	//ȫ��ʹ��DMA��ʽ����
void	SendHeatElementBufToCore(void)
{
	g_bPrtSPIDMATxStatusFlag = 1;
	g_bPrtSPIDMAIntForDriveMode = 1;
	sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);	//������оSPI����
	while (g_bPrtSPIDMATxStatusFlag);		//һֱ�ȴ�DMA���ͽ��� 2016.06.15
	g_bPrtSPIDMAIntForDriveMode = 0;
}
#else
void	SendHeatElementBufToCore(void)
{
	uint32_t	i;
	
	for (i = 0; i < BYTES_OF_ALL_ELEMENT; i++)	//���ͼ������ݵ���о
	{
		SPI_HeatSendByte(g_tbHeatElementBuf[i]);
	}
}
#endif

/*******************************************************************************
* Function Name  : PrintOneDotLine
* Description    : ��ӡ1��������,�Ƚ���Ӧ�������������о;�������ʱ��;
*	�ȴ�ǰ�������;����������;���������ֽʱ��;��ʼ�����м���.
* Input          : ���ӡ��о���ȵ�Ԫ���Ӧ���ַ�������.�ַ���������0��ʼ�����ֽ�,
*	��MSB��LSB����ȵ�Ԫ������һһ��Ӧ.
*	���øú���ʱӦȷ����һ�����Ѿ��������.��������������ݻ������һ����δ���
*	������,�����ص�.�������Ѿ�������.
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintOneDotLine(void)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	i;
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ,��ѯ����
		uint32_t	iTemp;
	#endif
	
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ,��ѯ����
		StartProcessTimeCounter();
	#endif
	SendHeatElementBufToCore();	//���ͼ������ݵ���о
	#ifdef	DBG_PROCESS_TIME_SPI_PRT_NORMAL_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ,��ѯ����
		StopProcessTimeCounter();
		iTemp = 0;
		g_iDbgProcessTimeUs = 0;
	#endif
	
	HeatTime = GetHeatTime();		//�������ʱ��
	SetHeatNumber();						//���㱾���м��ȴ���,���ü��ȿ��ƹܽ�
	
// 	WaitPrintAndFeedPaperFinish();	//�ȴ�ǰ������ֽ���߼������
// 	LatchHeatData();							  //�����о����
	
	g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//��ֵ���Ʊ���
	g_tHeatCtrl.HeaterGroupNumber = 0;
	for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
	{
		g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
		g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
	}
	
	g_HeatUs = HeatTime;
	totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//һ�����ܼ���ʱ��
	AdjustMotorStep(totalheatus);					//��������ʱ,�������ٲ���	
	
	WaitPrintAndFeedPaperFinish();	//�ȴ�ǰ������ֽ���߼������ 2016.06.15
	while (g_bPrtSPIDMATxStatusFlag);		//һֱ�ȴ�DMA���ͽ���
	g_bPrtSPIDMAIntForDriveMode = 0;
	LatchHeatData();							//�����о����	
	
	g_tHeatCtrl.HeatEnable = HEAT_ENABLE;	//�ÿ�ʼ���ȱ���
	SetTimerCounterWithAutoReload(HEAT_TIMER, g_HeatUs);	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
	TIM_SetCounter(HEAT_TIMER, 1);
	StartTimer(HEAT_TIMER);								//������ʱ����ʼ������Ⱥ��ߵ���ж�
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����д�ӡ�������Ĵ�ӡ����,���ݵ�ǰ�л�������״̬������Ӧ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#ifdef	DBG_LPB_INT_COUNT
volatile uint32_t	iDbgPrtTotalCount        = 0;
volatile uint32_t	iDbgPrtIldeCount         = 0;
volatile uint32_t	iDbgPrtWritingCount      = 0;
volatile uint32_t	iDbgPrtSendingDataCount  = 0;
volatile uint32_t	iDbgPrtWaitPrtCount      = 0;
volatile uint32_t	iDbgPrtStartPrtCount     = 0;
volatile uint32_t	iDbgPrtSendDataOverCount = 0;
volatile uint32_t	iDbgPrtSetLineSpaceCount = 0;
#endif

void	PrintLineBuffer(void)
{
	
	#ifdef	DBG_LPB_INT_COUNT
		iDbgPrtTotalCount++;
	#endif
	
	switch	(U_PrtBuf.g_tLinePrtBuf.Status)
	{
		case	LPB_STATUS_ILDE:					//����״̬
			StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtIldeCount++;
			#endif
			break;
		case	LPB_STATUS_WRITING:				//�༭״̬
      StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtWritingCount++;
			#endif
			break;
		case	LPB_STATUS_WAIT_TO_PRT:		//�ȴ���ӡ״̬
			WaitToPrintLineBuf();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtWaitPrtCount++;
			#endif
			break;
		case	LPB_STATUS_START_PRT:			//��ʼ��ӡ״̬
			StartPrintLineBuf();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtStartPrtCount++;
			#endif
			break;
		case	LPB_STATUS_SPI_SENDING:		//SPI����״̬
			StopTimer(PRINT_CHECK_TIMER);
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSendingDataCount++;
			#endif
			break;
		case	LPB_STATUS_SPI_SEND_OVER:	//��о��������״̬
			StartHeatAndRotateMotor();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSendDataOverCount++;
			#endif
      break;
		case	LPB_STATUS_SET_GO_LINE_SPACE:	//�������м��״̬
			SetGoLineSpace();
      #ifdef	DBG_LPB_INT_COUNT
        iDbgPrtSetLineSpaceCount++;
			#endif
			break;
		default:	//����״̬�����д���,����о��һ��������״̬ʱ��������ȴ�DMA�ж��޸�״̬��־λ
			break;	//�ڱ༭�п�ʼʱ�Կ����д�ӡ�йص�ȫ�ֱ�����ֵ��ѡ�е��д�ӡ��������PrtMode�ṹ����
	}	
}

uint32_t GetLPBProcessStatus(void)
{
	uint32_t	iStatus;
	
	iStatus = GetPrintStatus();
	if ((LPB_STATUS_WAIT_TO_PRT == iStatus) || \
			(LPB_STATUS_START_PRT == iStatus) || \
			(LPB_STATUS_SPI_SEND_OVER == iStatus) || \
			(LPB_STATUS_SET_GO_LINE_SPACE == iStatus)
		 )
	{
		if (ENABLE != GetTimerEnableStatus(PRINT_CHECK_TIMER))
		{
			return (LPB_PROCESS_STATUS_UNFINISH);
		}
		else
		{
			return (LPB_PROCESS_STATUS_FINISH);
		}
	}
	else
	{
		return (LPB_PROCESS_STATUS_FINISH);
	}
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ���ô�ӡ�����ȿ���ȫ�ֱ���,�ַ�ģʽʱ��Ҫ����:
	PrintLineIndex,UnderLinePrtOverFlag,
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	WaitToPrintLineBufOfChar(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
	{	//�»���֮������ݴ�ӡ��ʼ����,Ĭ��ʹ��12*24�ַ�ʱ��0��ʼ
		ptLPB->PrtMode.PrintLineIndex	= \
			(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY;
	}
	else		//�����ӡ
	{	//�»���֮�����������,�����������༭��������ַ��и�*�������Ŵ���		
		ptLPB->PrtMode.PrintLineIndex = MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY;
	}
	
	#ifdef	UNDER_LINE_PRT_ENABLE
	if (ptLPB->PrtMode.DoUnderlineFlag)		//2012-9-21��Ӵ����»��ߴ�ӡ
	{
		if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
		{
			ptLPB->PrtMode.UnderlinePrintIndex = ptLPB->PrtMode.UnderlineMaxPrtHigh;
		}
		else
		{
			ptLPB->PrtMode.UnderlinePrintIndex = 0;
		}
		ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_UNFINISHED;
	}
	#endif
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���ô�ӡ�����ȿ���ȫ�ֱ���,ͼ��ģʽʱ��Ҫ����:
	PrintLineIndex,UnderLinePrtOverFlag,MaxRatioY
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	WaitToPrintLineBufOfGraphic(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.BitImageFlag == 1)	//�����λͼ��ӡģʽ��Ŵ����1:1
	{
		ptLPB->PrtMode.MaxRatioY = 1;
	}
	
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)		//�����ӡ
	{	//�»���֮������ݴ�ӡ��ʼ����,Ĭ��ʹ��12*24�ַ�ʱ��0��ʼ
		ptLPB->PrtMode.PrintLineIndex	= 0;
	}
	else		//�����ӡ
	{	//�»���֮�����������,�����������༭��������ַ��и�*�������Ŵ���
		ptLPB->PrtMode.PrintLineIndex	= ptLPB->PrtMode.MaxPrtDotHigh;
	}
	
	#ifdef	UNDER_LINE_PRT_ENABLE
	if (ptLPB->PrtMode.DoUnderlineFlag)		//2012-9-21��Ӵ����»��ߴ�ӡ
	{
		ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
	}
	#endif
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����һ�δ�ӡ�����ߵ���Ƿ����,���ȫ�ֱ���,
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	WaitToPrintLineBuf(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	//�����ӡ����������Ҫ��ӡ����,���»���,ֱ�ӵ����м��
	if (ptLPB->PrtMode.PrtDataDotIndex == ptLPB->PrtMode.PrtLeftLimit)
	{
		ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
	}
	else	//��������д�ӡ
	{	//���³�ʼ����ӡ���Ƶ�ȫ�ֱ���
		if ((ptLPB->PrtMode.BitImageFlag == 1) || \
				(ptLPB->PrtMode.VariableBitImageFlag == 1))
		{
			WaitToPrintLineBufOfGraphic();
		}
		else
		{
			WaitToPrintLineBufOfChar();
		}
		
		ptLPB->Status = LPB_STATUS_START_PRT;	//����Ѿ����������л�����״̬
	}
	
	SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
	StartTimer(PRINT_CHECK_TIMER);
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����ӡʱ,���»��߻�����������ַ��ʼ������,���ո����и߶�ȡֵ,
*		ת���ɻ�о�������ģʽ,�����ӡʱ���»��߻�����һ���ֽ���BIT3->BIT0��˳����ȡ���
*		��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*		��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*		�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*									���������ֽ�����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ�������
*	pָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ���ֽ��еĵڼ�λ��,��Χ[0,8).,Ŀǰʹ�õ���һ���ֽڵ����3λ��ʾ���3���»���
* Input          : ��ǰ��Ҫ����Ļ�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	UnderLineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0ʱһ���ֽ�������
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	//���������Ŀհ�λ��������,��Ϊǰ���Ѿ�ȫ�������
	if (ColumnIndex % 8)				//ColumnIndex����8��������,ǰ����Ҫ�ν�
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0		
	}
	else
	{
		CurrentBit = 8;		//һ���ֽ������8λ
	}
	
	i = (ColumnIndex >> 3);	//׼�������ֽ�,���ֽڵĺ��治��Ĳ�����Ҫ����������в���
	if (i >= BYTES_OF_HEAT_ELEMENT)		//����������һ���ֽ�
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//��ֹ�±�Խ��
	}
	CopyIndex = i;
	//����Ϊ����հײ���
	
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//��ֹ�Ƿ��ڴ�Խ��
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (ColumnIndex = ptLPB->PrtMode.PrtLeftLimit; 
		ColumnIndex < ptLPB->PrtMode.PrtDataDotIndex; ColumnIndex++)
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ0~7, Ŀǰ���Χ0~3
		if (*PrintBuff & ((uint8_t)(0x01 << (Lines - 1))))	//�����Ӧλ��Ϊ1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));		//���һ�������ֽ�
		}
		CurrentBit--;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 0)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 8;			//���¿�ʼ׼�������һ���ֽ�,��MSB��ʼ���
			i++;								//ֻ�����ֽں��ִ��
		}
		
		PrintBuff++;					//׼�������һ���ֽ�
	}		//����ӡ�ֽڴ������
#if	0
	if (!CurrentBit)		//˵����ת��������һ���ֽڶ�û����ɵ�,��Ҫ����1��
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//��ֹ���
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += (BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT + CopyIndex);	//�������32λ�հ����ͼ�������Ϊ0������,�Ҳ�32λ�հ����Ѿ�����
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//�м�ʵ�ʴ�ӡ��
	}
	
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//��ӡ����
	//#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*	��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*	�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*	���д�ӡ������pBuffer[ ColumnIndex ]��ʼ������
*								pBuffer[ PrtDataDotIndex ]�������ַ��������ݵߵ�����䵽
*								pBuffer[ PrtLength + LeftLimit - ColumnIndex ]��ʼ�ݼ���
*								pBuffer[ PrtLength + LeftLimit - PrtDataDotIndex ]������λ�á�
*	���λ�ö�Ӧ����Buf[ PrtLength + LeftLimit - ColumnIndex ]��ʼ�ݼ���
*									Buf[ PrtLength + LeftLimit - PrtDataDotIndex ]������λ�á�
*	�������ӡʱ�ȷ���һ�е����λ�õ�Linesֵ,�ݼ����͵�һ�е����λ�õ�Linesֵ,
*	pBuf���Ǵ�������,���Դ������:
*	��lines��, ColumnIndex		�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���LSB,
*	��lines��,(ColumnIndex+1)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+1),
*	��lines��,(ColumnIndex+2)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+2),
*	��lines��,(ColumnIndex+3)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+3),
*	........
*	��lines��,(ColumnIndex+7)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+7),
*	����һ������ֽںϳ���ϡ�
*	������һ��ѭ����
*		  							����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��PrintBuff������Lines,
*	PrintBuffָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ��ӡ������,��Χ[0,����ַ�*�������Ŵ���).

* Description    : �����ӡʱ,���»��߻�����������ַ��ʼ������,���ո����и߶�ȡֵ,
*		ת���ɻ�о�������ģʽ,�����ӡʱ���»��߻�����һ���ֽ���BIT0->BIT3��˳����ȡ���
*		��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*		��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*		�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*									���������ֽ�����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ�������
*	pָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ���ֽ��еĵڼ�λ��,��Χ[0,8),Ŀǰʹ�õ���һ���ֽڵ����3λ��ʾ���3���»���
* Ŀǰ����Lines�仯˳��Ϊ��LSB��MSB(3/2/1->0)
* Input          : ��ǰ��Ҫ����Ļ�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#define	UNDER_LINE_MAX_BIT	(2)
static void	UnderLineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ8ʱһ���ֽ�������		
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	i = ptLPB->PrtMode.PrtLength - ColumnIndex;	//ָ�����ת������Ȼ�������ʼλ��
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//���ܳ��ֲ���8λ������λ��,������������λ��ʼ�����
	}
	else
	{
		CurrentBit = 0;		//�����8�����������0��ʼ
	}
	
	i >>= 3;													//ת��Ϊ�ֽ�
	if (i >= BYTES_OF_HEAT_ELEMENT)		//����������һ���ֽ�
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//��ֹ�±�Խ��
	}
	
	pBuf = Buf;
	pBuf += i;
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//��ֹ�Ƿ��ڴ�Խ��
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	if (Lines > UNDER_LINE_MAX_BIT)
		Lines = UNDER_LINE_MAX_BIT;
	for (i = ptLPB->PrtMode.PrtLeftLimit; i < ptLPB->PrtMode.PrtDataDotIndex; i++)	//��Ҫ�������(�ֽ�)��Ŀ
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ1~3
		if (*PrintBuff & ((uint8_t)(0x01 << Lines)))	//�����Ӧλ��Ϊ1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);		//���һ�������ֽ�
		}
		CurrentBit++;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 8)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 0;			//���¿�ʼ׼�������һ���ֽ�,��LSB��ʼ���
			pBuf--;							//ֻ����һ���ֽں�ż���			
		}
		
		PrintBuff++;					//׼�������һ�е��ֽ�,ÿ��ѭ��������
	}
	
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//�������32λ�հ���,�Ҳ�32λ�հ����Ѿ�����
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//�м�ʵ�ʴ�ӡ��
	}
	
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//��ӡ����
	//#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��������ַ��ʼ������,���ո����и߶�ȡֵ,ת���ɻ�о�������ģʽ
*		��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*		��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*		�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*									���������ֽ�����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ�������
*	pָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Column��ʾ���ֽ��еĵڼ�λ��,��Χ[0,8).
* Input          : ��ǰ��Ҫ����Ļ�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	FillUnderLineToHeatElementBuf(uint8_t * PrintBuff, uint8_t Lines)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	//uint8_t	* pbBuf = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ��ʼ
	{
		UnderLineBufToHeatElementNormalDir(PrintBuff, Lines);
	}	//�����ӡ�������
	else// if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_REVERSE)	//�����ӡ��ʼ
	{
		UnderLineBufToHeatElementReverseDir(PrintBuff, Lines);
		//UnderLineBufToHeatElementNormalDir(PrintBuff, Lines);
		//pbBuf = g_tbHeatElementBuf;
		//pbBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;
		//ReverseStreamEachBit(pbBuf, BYTES_OF_HEAT_ELEMENT);
	}	//�����ӡ�������
	
	ptLPB = NULL;
	//pbBuf = NULL;
}

#if	0
/*******************************************************************************
* Function Name  : ������
* Description    : ��һ���ֽڵ�������λ����, ��BIT0���ȡ�ø���λ, �ƶ�����Ӧλ
* Input          : ��ǰ��Ҫ������ֽ�ֵ
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static uint8_t ReverseByte(uint8_t cSrc)
{
	uint8_t cTemp;
	uint32_t iCount;
	
	cTemp = 0;
	for (iCount = 0; iCount < sizeof(uint8_t); iCount++)
	{
		if ((cSrc >> iCount) & 0x01)
		{
			cTemp |= (1 << (sizeof(uint8_t) - 1 - iCount));
		}
	}
	
	return (cTemp);
}
#endif

#if	0
/*******************************************************************************
* Function Name  : ������
* Description    : ����о�������������λ����, ������ָ��Ϊ׼
* Input          : ��ǰ��Ҫ������л�����ָ��, ��Ҫ������ֽ���
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void ReverseStreamEachBit(uint8_t * pbBuf, uint32_t iNumber)
{
	uint8_t bBuf[ BYTES_OF_ALL_ELEMENT ];
	uint32_t iCount;
	uint8_t * pbSrc = NULL;
	uint8_t * pbDst = NULL;

	memset(bBuf, 0x00, BYTES_OF_ALL_ELEMENT);	//����
	//memset(bBuf, 0x00, sizeof(bBuf) / sizeof(bBuf[0]));	//����	
	pbSrc = bBuf;
	pbDst = pbBuf;
	for (iCount = 0; iCount < iNumber; iCount++)	//ȡ�ô���ת���ݵı���
	{
		*pbSrc = *pbDst;
		pbSrc++;
		pbDst++;
	}
	
	pbSrc = bBuf;
	pbDst = pbBuf;
	for (iCount = 0; iCount < iNumber; iCount++)	//��ת����
	{
		if ((*pbSrc == 0x00) || (*pbSrc == 0xFF))		//���跴ת,��������Գ�
		{
			*pbDst = *pbSrc;
		}
		else
		{
			*pbDst = ReverseByte(*pbSrc);
		}
		pbSrc++;
		pbDst++;
	}
	
	pbSrc = NULL;
	pbDst = NULL;
}
#endif

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ��ӡ����,����Ƿ��д���,���û�м������Ƿ������,
*	δ��������򽫵�ǰ����ֵ����ɻ�о�������ģʽ,������оSPI����DMA,�����ַ���ӡ
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
	extern volatile uint32_t	g_iDbgSPISendCount;	//����оSPI�������ݵĴ���ͳ��
#endif
#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
	#define	DBG_EDIT_COUNT_BUF_LEN		(100)
	extern	volatile uint32_t	g_iDbgEditCountWhilePrt;
	volatile uint8_t	g_bDbgEditCharCountBuf[DBG_EDIT_COUNT_BUF_LEN];
	static	volatile uint8_t * pbDbgBuf = g_bDbgEditCharCountBuf;
	static	uint8_t	g_bDbgBufCount = 0;
#endif
static void	StartPrintLineBufChar(void)
{
	uint8_t	* pbBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
	{	//����Ϊ�����»���֮��Ĵ�ӡ����
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))	//�и�,Ĭ��Ϊ24
		{
			//�л����׵�ַ	�������Ŵ���	��ǰ�Ŵ���Ĭ��1	ÿ�е�	��Ϊ�и����24�������� ����/8,��0,1,2�����ֽ���
			pbBuf = ptLPB->Buf + \
					(MAX_RATIO_Y - ptLPB->PrtMode.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS + \
					(ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS + \
					ptLPB->PrtMode.PrtLeftLimit;
			//�˴���Ҫ��Ӵ���õ�������,ȡ����Ҫ���͵�һ�������ݲ��ŵ�������������.������оSPI����.
			//����ת��һ�����ݲ����浽����������
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);
			ptLPB->PrtMode.PrintLineIndex++;
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI����
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
				g_iDbgSPISendCount++;		//����оSPI�������ݵĴ���ͳ��
				if (g_iDbgSPISendCount >= 24)
				{
					*pbDbgBuf++ = (uint8_t)g_iDbgEditCountWhilePrt;
					g_bDbgBufCount++;
					if (g_bDbgBufCount >= DBG_EDIT_COUNT_BUF_LEN)
						pbDbgBuf = g_bDbgEditCharCountBuf;
				}
			#endif
		}		//�ȴ�ӡ���ַ������ٴ�ӡ�»���,�ַ���ӡ�������
		#ifdef	UNDER_LINE_PRT_ENABLE		//2012-9-21����»��ߴ�ӡ����
		else	//�ַ��Ѿ�����,����»���
		{	//����»��ߴ�ӡδ�������,����Ȼ���˳�
			if (ptLPB->PrtMode.UnderLinePrtOverFlag == UNDER_LINE_PRT_UNFINISHED)
			{
				if (ptLPB->PrtMode.UnderlinePrintIndex)
				{
					pbBuf = ptLPB->Buf;
					pbBuf += BYTES_OF_LPB;				//ָ���»��߻�����
					pbBuf += ptLPB->PrtMode.PrtLeftLimit;
					//���»��߻��������ݶ�Ӧ�е�����ȡ������䵽������Ȼ�����
					FillUnderLineToHeatElementBuf(pbBuf, ptLPB->PrtMode.UnderlinePrintIndex);
					ptLPB->PrtMode.UnderlinePrintIndex--;	//�»��ߴ�ӡ�߶�ֵ���1����,ֱ��Ϊ0��ʾ��ӡ���
					sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI_DMA����
					ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
					#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
						StartProcessTimeCounter();
					#endif
					#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
						g_iDbgSPISendCount++;	//����оSPI�������ݵĴ���ͳ��
					#endif
				}
				else	//�˴�����Ϊ�����Դ���,��ֹ��ӡ���̽����߼�����
				{
					ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
					ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
					SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
					StartTimer(PRINT_CHECK_TIMER);
				}
			}			//�»���δ��ӡ�괦�����
			else	//�ַ����»��߶��Ѵ�ӡ�괦��ʼ
			{
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}			//�ַ����»��߶��Ѵ�ӡ�괦�����
		}		//����»��ߴ������
		#endif
	}			//�����ӡ�������
	else	//�����ӡ
	{	//�ȴ�ӡ���»����ٴ�ӡ�ַ�����,2012-9-23����»��ߴ�ӡ����
		//����»��ߴ�ӡδ�������,����Ȼ���˳�
		if (ptLPB->PrtMode.UnderLinePrtOverFlag == UNDER_LINE_PRT_UNFINISHED)
		{
			if (ptLPB->PrtMode.UnderlinePrintIndex < ptLPB->PrtMode.UnderlineMaxPrtHigh)
			{
				pbBuf = ptLPB->Buf;
				pbBuf += BYTES_OF_LPB;				//ָ���»��߻�����
				pbBuf += ptLPB->PrtMode.PrtLeftLimit;
				//���»��߻��������ݶ�Ӧ�е�����ȡ������䵽������Ȼ�����
				//�»��ߴ�ӡ�߶�ֵ���1����,ֱ��Ϊ0��ʾ��ӡ���
				FillUnderLineToHeatElementBuf(pbBuf, ptLPB->PrtMode.UnderlinePrintIndex);
				ptLPB->PrtMode.UnderlinePrintIndex++;	//�»��ߴ�ӡ�߶�ֵ���1����,ֱ��Ϊ0��ʾ��ӡ���
				sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI_DMA����
				ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
				
				//m_bReversePrintUnderlineFlag = 1;
				#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
					StartProcessTimeCounter();
				#endif
				#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
					g_iDbgSPISendCount++;	//����оSPI�������ݵĴ���ͳ��
				#endif
			}
			else	//�˴�����Ϊ�����Դ���,��ֹ��ӡ���̽����߼�����
			{
				ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}
		}		//�»��ߴ�������ٴ�ӡ�ַ�����,�»��ߴ������
		//�ô���ӡ�е����Ŀ����ȡ�޷�����,����1Ϊֹ
		else	//��ʼ�����ַ���ӡ
		{
			if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
			{
				//����ӡ������������ת��Ϊ��о���ո�ʽ,��ͨ��SPI�ӿ��͵���о������,�����������ȵ���
				//����:PrintBuff:��ӡ��������ַ,Line����Ҫ����ĵ��к�
				ptLPB->PrtMode.PrintLineIndex--;	//��ʼ��ʱ��ż����ʼ,����ʵ����ʱ���ȴӼ�1��ʼ
				pbBuf = ptLPB->Buf + 
					(MAX_RATIO_Y - ptLPB->PrtMode.MaxRatioY) * (MAX_FONT_HEIGHT / 8) * PRT_DOT_NUMBERS + 
					(ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS + 
					ptLPB->PrtMode.PrtLeftLimit;
				//�˴���Ҫ��Ӵ���õ�������,ȡ����Ҫ���͵�һ�������ݲ��ŵ�������������.������оSPI����.			
				//����ת��һ�����ݲ����浽����������
				FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);			
				sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI����
				ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
				#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
					StartProcessTimeCounter();
				#endif
				#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
					g_iDbgSPISendCount++;	//����оSPI�������ݵĴ���ͳ��
				#endif
			}		//�ַ�δ��ӡ�괦�����
			else
			{
				ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
				SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
				StartTimer(PRINT_CHECK_TIMER);
			}		//�������»��ߺ��ַ�������
		}		//�������ӡ�ַ�����
	}		//�����ӡ�������
	
	pbBuf = NULL;
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ��ӡ����,����Ƿ��д���,���û�м������Ƿ������,
*	δ��������򽫵�ǰ����ֵ����ɻ�о�������ģʽ,������оSPI����DMA,����ͼ�δ�ӡ
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	StartPrintLineBufGraphic(void)
{
	uint8_t	cTemp;
	uint8_t	* pbBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
	{
		if (ptLPB->PrtMode.PrintLineIndex < ptLPB->PrtMode.MaxPrtDotHigh)
		{
			cTemp = ptLPB->PrtMode.MaxPrtDotHigh >> 3;
			if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
			{
				cTemp += 1;		//������������Ϊ�ֽ���,����һ���ֽڵ���Ҫ����
			}
			if (cTemp >= MAX_PRT_HEIGHT_BYTE)
				cTemp = (MAX_PRT_HEIGHT_BYTE - 1);
			pbBuf = ptLPB->Buf + (MAX_PRT_HEIGHT_BYTE - cTemp) * PRT_DOT_NUMBERS;	//ָ�����д�ӡ���ݵ��׵�ַ
			pbBuf += (ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS;	//ָ����е��׵�ַ
			pbBuf += ptLPB->PrtMode.PrtLeftLimit;				//ָ����������ӡ���ݵ��׵�ַ
			//����ת��һ�����ݲ����浽����������
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);
			ptLPB->PrtMode.PrintLineIndex++;
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI����
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
				g_iDbgSPISendCount++;	//����оSPI�������ݵĴ���ͳ��
			#endif
		}		//ͼ�δ�ӡ���
		else	//��ֹ����DMA��������
		{
			ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
			StartTimer(PRINT_CHECK_TIMER);
		}
	}
	else		//�����ӡ
	{
		if (ptLPB->PrtMode.PrintLineIndex > 0)
		{	//����ӡ������������ת��Ϊ��о���ո�ʽ,��ͨ��SPI�ӿ��͵���о������,�����������ȵ���
			//����:PrintBuff:��ӡ��������ַ,Line����Ҫ����ĵ��к�
			ptLPB->PrtMode.PrintLineIndex--;	//��ʼ��ʱ��ż����ʼ,����ʵ����ʱ���ȴӼ�1��ʼ
			cTemp = ptLPB->PrtMode.MaxPrtDotHigh >> 3;
			if (ptLPB->PrtMode.MaxPrtDotHigh % 8)
			{
				cTemp += 1;		//������������Ϊ�ֽ���,����һ���ֽڵ���Ҫ����
			}
			if (cTemp >= MAX_PRT_HEIGHT_BYTE)
				cTemp = (MAX_PRT_HEIGHT_BYTE - 1);
			pbBuf = ptLPB->Buf + (MAX_PRT_HEIGHT_BYTE - cTemp) * PRT_DOT_NUMBERS;	//ָ�����д�ӡ���ݵ��׵�ַ
			pbBuf += (ptLPB->PrtMode.PrintLineIndex >> 3) * PRT_DOT_NUMBERS;	//ָ����е��׵�ַ
			pbBuf += ptLPB->PrtMode.PrtLeftLimit;				//ָ����������ӡ���ݵ��׵�ַ
			FillHeatElementBuf(pbBuf, ptLPB->PrtMode.PrintLineIndex);	//����ת��һ�����ݲ����浽����������
			sHeat_StartDMATx((uint32_t *)g_tbHeatElementBuf);			//������оSPI����
			ptLPB->Status = LPB_STATUS_SPI_SENDING;	//����Ѿ����������л�����״̬
			#ifdef	DBG_PROCESS_TIME_SPI_PRT_DMA_SEND_ONE_LINE	//����һ���л�о���ݺ�ʱ
				StartProcessTimeCounter();
			#endif
			#ifdef	DBG_PROCESS_TIME_EDIT_COUNT_WHILE_PRT	//��ӡһ�й����м����༭�ַ��ĸ���
				g_iDbgSPISendCount++;	//����оSPI�������ݵĴ���ͳ��
			#endif
		}
		else	//��ֹ����DMA��������
		{
			ptLPB->Status = LPB_STATUS_SPI_SEND_OVER;	//����Ѿ����������л�����״̬
			SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
			StartTimer(PRINT_CHECK_TIMER);
		}
	}
	
	pbBuf = NULL;
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ��ӡ����,����Ƿ��д���,���û�м������Ƿ������,
*	δ��������򽫵�ǰ����ֵ����ɻ�о�������ģʽ,������оSPI����DMA
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	StartPrintLineBuf(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))
	{
		StartPrintLineBufGraphic();
	}
	else
	{
		StartPrintLineBufChar();
	}
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���㵱ǰ��ӡ�е��м�������,���ڽ�һ��8λ��ת��Ϊ��о��������ʱ
*	���ݴ�ӡ�������Ͷ��뷽ʽ��ͬ������,
*	�����ʱColumn������ֵ����PrtDataDotIndex,�����ҵ��������ӡ����
*	�Ҷ���ʱColumn������ֵ����PrtDataDotIndex,�������ҵ��������ӡ����
*	�м����ʱColumn������ֵ����PrtDataDotIndex,�������ҵ��������ӡ����
*	����ΪĬ������Ϊ0ʱ�ļ���.������಻Ϊ0,��Ҫ�ټ�������.
*	PrtDataDotIndex(��ֵ������˿�ʼ,���������ֵ),ָ��ǰ��������ʱ����
*			|�ܴ���ΧPrtLength = LeftLimit + PrtWidth(����ʵ�ʴ�ӡ��Χ)
*			|g_tPrtCtrlData.PrtWidth = 576;		//���ô�ӡ���� |
*			|g_tPrtCtrlData.PrtLength = PrtWidth + LeftLimit;//Ӳ����Ч��ӡ����,���576|
*			|<-------------------------|---------------------->|
*			|			 ����ʵ�ʴ�ӡ���ݳ���PrtDataDotIndex			   |
*			|<-----|----->			|<-----|----->	 |<-----|----->|
*	��߾�LeftLimit					|		  �м�			 |						�ұ߾�RightLimit
*	�����ʱColumn=PrtLeftLimit    |				 |�Ҷ���ʱColumn=PrtLength-PrtDataDotIndex
*																 |
*								�м����ʱColumn=(PrtLength-PrtDataDotIndex)/2
*									����ȫ�ֱ���
* Input          : ��Ҫ֪���Ǽ����ĸ��д�ӡ��������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static uint16_t	GetLinePrtBufStartColumnIndex(void)
{
	uint16_t	ColumnIndex;	//�����м�������
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_LEFT)		//�����
	{
		ColumnIndex = ptLPB->PrtMode.PrtLeftLimit;
	}
	else if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_MIDDLE)		//�м����
	{	// = ��Ч��ӡ���� - ��ӡ�����м���������(һ���д�ӡ��ӡ���������)
		if (ptLPB->PrtMode.PrtLength > ptLPB->PrtMode.PrtDataDotIndex)
			ColumnIndex = ptLPB->PrtMode.PrtLength - ptLPB->PrtMode.PrtDataDotIndex;
		else
			ColumnIndex = 0;
		
		ColumnIndex >>= 1;
		ColumnIndex += ptLPB->PrtMode.PrtLeftLimit;
	}
	else if (ptLPB->PrtMode.CodeAimMode == AIM_MODE_RIGHT)	//�Ҷ���
	{
		if (ptLPB->PrtMode.PrtLength > ptLPB->PrtMode.PrtDataDotIndex)
			ColumnIndex = ptLPB->PrtMode.PrtLength - ptLPB->PrtMode.PrtDataDotIndex;
		else
			ColumnIndex = 0;
		
		ColumnIndex += ptLPB->PrtMode.PrtLeftLimit;
	}
	else
	{
		ColumnIndex = 0;
	}
	ptLPB = NULL;
	
	return (ColumnIndex);
}

/*******************************************************************************
* Function Name  : ���������ӡʱ������ת��
* Description    : ��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*	��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*	�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��PrintBuff������Lines,
*	PrintBuffָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ��ӡ������,��Χ[0,����ַ�*�������Ŵ���).
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	LineBufToHeatElementNormalDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0ʱһ���ֽ�������
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	//���������Ŀհ�λ��������,��Ϊǰ���Ѿ�ȫ�������
	if (ColumnIndex % 8)				//ColumnIndex����8��������,ǰ����Ҫ�ν�
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0
	}
	else
	{
		CurrentBit = 8;		//һ���ֽ������8λ
	}
	i = (ColumnIndex >> 3);	//׼�������ֽ�,���ֽڵĺ��治��Ĳ�����Ҫ����������в���
	if (i >= BYTES_OF_HEAT_ELEMENT)		//����������һ���ֽ�
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//��ֹ�±�Խ��
	}
	CopyIndex = i;
	//����Ϊ����հײ���
	
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//��ֹ�Ƿ��ڴ�Խ��
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (ColumnIndex = ptLPB->PrtMode.PrtLeftLimit; \
		ColumnIndex < ptLPB->PrtMode.PrtDataDotIndex; ColumnIndex++)
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ0~7,��MSB��LSB
		if (*PrintBuff & (0x80 >> (Lines & 0x07)))	//�����Ӧλ��Ϊ1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));	//���һ�������ֽ�
		}
		CurrentBit--;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 0)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 8;			//���¿�ʼ׼�������һ���ֽ�,��MSB��ʼ���
			i++;								//ֻ�����ֽں��ִ��
		}
		
		PrintBuff++;					//׼�������һ���ֽ�
	}		//����ӡ�ֽڴ������
#if	0
	if (!CurrentBit)		//˵����ת��������һ���ֽڶ�û����ɵ�,��Ҫ����1��
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//��ֹ���
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	if (CopyIndex > BYTES_OF_HEAT_ELEMENT)	//��ֹ�Ƿ��ڴ�Խ��
		CopyIndex = BYTES_OF_HEAT_ELEMENT;
	//�������32λ�հ����ͼ�������Ϊ0������,�Ҳ�32λ�հ����Ѿ�����
	pBuf = g_tbHeatElementBuf + BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;
	pBuf += CopyIndex;
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//�м�ʵ�ʴ�ӡ��
	}
	
	pBuf = NULL;
	ptLPB = NULL;
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//��ӡ����
	//#endif
}

/*******************************************************************************
* Function Name  : �������ӡʱ������ת��
* Description    : ��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*	��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*	�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*	���д�ӡ������pBuffer[ ColumnIndex ]��ʼ������
*								pBuffer[ PrtDataDotIndex ]�������ַ��������ݵߵ�����䵽
*								pBuffer[ PrtLength + LeftLimit - ColumnIndex ]��ʼ�ݼ���
*								pBuffer[ PrtLength + LeftLimit - PrtDataDotIndex ]������λ�á�
*	���λ�ö�Ӧ����Buf[ PrtLength + LeftLimit - ColumnIndex ]��ʼ�ݼ���
*									Buf[ PrtLength + LeftLimit - PrtDataDotIndex ]������λ�á�
*	�������ӡʱ�ȷ���һ�е����λ�õ�Linesֵ,�ݼ����͵�һ�е����λ�õ�Linesֵ,
*	pBuf���Ǵ�������,���Դ������:
*	��lines��, ColumnIndex		�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���LSB,
*	��lines��,(ColumnIndex+1)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+1),
*	��lines��,(ColumnIndex+2)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+2),
*	��lines��,(ColumnIndex+3)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+3),
*	........
*	��lines��,(ColumnIndex+7)	�е��ַ��Ķ�Ӧ λֵ ��Ϊ����ַ���(LSB+7),
*	����һ������ֽںϳ���ϡ�
*	������һ��ѭ����
*		  							����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��PrintBuff������Lines,
*	PrintBuffָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ��ӡ������,��Χ[0,����ַ�*�������Ŵ���).
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	LineBufToHeatElementReverseDir(uint8_t * PrintBuff, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ8ʱһ���ֽ�������		
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	//memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	ColumnIndex = GetLinePrtBufStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	i = ptLPB->PrtMode.PrtLength - ColumnIndex;	//ָ�����ת������Ȼ�������ʼλ��
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//���ܳ��ֲ���8λ������λ��,������������λ��ʼ�����
	}
	else
	{
		CurrentBit = 0;		//�����8�����������0��ʼ
	}
	
	i >>= 3;													//ת��Ϊ�ֽ�
	if (i >= BYTES_OF_HEAT_ELEMENT)		//����������һ���ֽ�
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//��ֹ�±�Խ��
	}
	pBuf = Buf;
	pBuf += i;
	if (ptLPB->PrtMode.PrtDataDotIndex > PRT_DOT_NUMBERS)	//��ֹ�Ƿ��ڴ�Խ��
		ptLPB->PrtMode.PrtDataDotIndex = PRT_DOT_NUMBERS;
	for (i = ptLPB->PrtMode.PrtLeftLimit; i < ptLPB->PrtMode.PrtDataDotIndex; i++)	//��Ҫ�������(�ֽ�)��Ŀ
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ0~7
		if (*PrintBuff & (0x80 >> (Lines & 0x07)))	//�����Ӧλ��Ϊ1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);		//���һ�������ֽ�
		}
		CurrentBit++;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 8)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 0;			//���¿�ʼ׼�������һ���ֽ�,��LSB��ʼ���
			pBuf--;							//ֻ����һ���ֽں�ż���			
		}
		
		PrintBuff++;					//׼�������һ�е��ֽ�,ÿ��ѭ��������
	}
	
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//�������32λ�հ���,�Ҳ�32λ�հ����Ѿ�����	
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//�м�ʵ�ʴ�ӡ��
	}
	
	pBuf = NULL;
	ptLPB = NULL;
	//#ifdef	DBG_SPI_HEAT_DATA_SET
	//memset(g_tbHeatElementBuf, 0xAA, BYTES_OF_ALL_ELEMENT);	//��ӡ����
	//#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��һ��8λ���ݵ�ĳһλȡ��,��ϳ��ֽ�,��䵽��о����SPI������
*	��о�����Ҹ�32��(4�ֽ�)λ��Ϊ��,�м�576��(72)�ֽ���ʵ�ʿɼ�����,
*	�����ݰ��մ�ӡ����Ͷ��뷽ʽ�Ĳ�ͬ��������䵽�û�������
*									����ȫ�ֱ���
* Input          : ��ǰ��Ҫ������л�����ָ��PrintBuff������Lines,
*	PrintBuffָ��һ��8λ�ĵ�������,�������576,�����������м���
*	Lines��ʾ��ӡ������,��Χ[0,����ַ��߶�*�������Ŵ���).
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	FillHeatElementBuf(uint8_t *PrintBuff, uint8_t Lines)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ��ʼ
	{
		LineBufToHeatElementNormalDir(PrintBuff, Lines);
	}	//�����ӡ�������
	else if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_REVERSE)	//�����ӡ��ʼ
	{
		LineBufToHeatElementReverseDir(PrintBuff, Lines);
	}	//�����ӡ�������
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���Ŀǰ��ӡ�����������Ƿ������.����ַ�ģʽ������.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	CheckPrintContentFinishOfChar(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
	{
		#ifdef	UNDER_LINE_PRT_ENABLE	//2012-9-21����»��ߴ�ӡ����
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))
		{	//�����û�д������,����ʼ��ӡ�����ӡ��һ����
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//����»����Ƿ��ӡ���
			if (ptLPB->PrtMode.UnderlinePrintIndex == 0)	//�»��ߴ�ӡ���
			{
				ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;
				ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;	//���ȫ���������,׼���������м��
			}
			else	//�����û�д������,����ʼ��ӡ�����ӡ��һ�����»���
			{
				ptLPB->Status = LPB_STATUS_START_PRT;
			}
		}
		#else
		if (ptLPB->PrtMode.PrintLineIndex < 
				(MAX_FONT_HEIGHT * ptLPB->PrtMode.MaxRatioY))
		{	//�����û�д������,����ʼ��ӡ�����ӡ��һ����
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//���ȫ���������,׼���������м��
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
		#endif
	}			//�����ӡ�������
	else	//�����ӡ
	{
		#ifdef	UNDER_LINE_PRT_ENABLE	//2012-9-21����»��ߴ�ӡ����
		//�ȼ���»����Ƿ������
		if (ptLPB->PrtMode.UnderlinePrintIndex >= ptLPB->PrtMode.UnderlineMaxPrtHigh)
		{
			ptLPB->PrtMode.UnderLinePrtOverFlag = UNDER_LINE_PRT_FINISHED;	//�»��ߴ�ӡ���
			//m_bReversePrintUnderlineFlag = 0;
			//�ټ��������ӡ�����Ƿ������
			if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
			{	//�����û�д������,����ʼ��ӡ�����ӡ��һ����
				ptLPB->Status = LPB_STATUS_START_PRT;
			}
			else
			{	//���ȫ���������,׼���������м��
				ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
			}
		}
		else	//�����û�д������,����ʼ��ӡ�����ӡ��һ�����»���
		{
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		#else
		if (ptLPB->PrtMode.PrintLineIndex > \
				(MAX_FONT_HEIGHT - ptLPB->PrtMode.MaxLineHigh) * ptLPB->PrtMode.MaxRatioY)
		{		//�����û�д������,����ʼ��ӡ�����ӡ��һ����
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{		//���ȫ���������,׼���������м��
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
		#endif
	}			//�����ӡ�������
	//���������Ƿ�ȫ��������Ͻ���
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���Ŀǰ��ӡ�����������Ƿ������.���ͼ��ģʽ������.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	CheckPrintContentFinishOfGraphic(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;	
	if (ptLPB->PrtMode.PrintDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ
	{
		if (ptLPB->PrtMode.PrintLineIndex < ptLPB->PrtMode.MaxPrtDotHigh)
		{	//�����û�д������,����ʼ��ӡ�����ӡ��һ����
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{	//���ȫ���������,׼���������м��
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
	}
	else
	{
		if (ptLPB->PrtMode.PrintLineIndex > 0)
		{		//�����û�д������,����ʼ��ӡ�����ӡ��һ����
			ptLPB->Status = LPB_STATUS_START_PRT;
		}
		else
		{		//���ȫ���������,׼���������м��
			ptLPB->Status = LPB_STATUS_SET_GO_LINE_SPACE;
		}
	}
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���Ŀǰ��ӡ�����������Ƿ������.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void	CheckPrintContentFinish(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	ptLPB = &U_PrtBuf.g_tLinePrtBuf;	
	if ((ptLPB->PrtMode.BitImageFlag == 1) || \
			(ptLPB->PrtMode.VariableBitImageFlag == 1))
	{
		CheckPrintContentFinishOfGraphic();
	}
	else
	{
		CheckPrintContentFinishOfChar();
	}
	
	SetTimerCounterWithAutoReload(PRINT_CHECK_TIMER, LPB_CHECK_INTERVAL_US);
	StartTimer(PRINT_CHECK_TIMER);
	
	ptLPB = NULL;
}

/*******************************************************************************
* Function Name  : ������
* Description    : �����һ�μ����Ƿ����,���δ������˳�.
*	�������������о����,�������ʱ��,����ߵ���Ƿ����,���δ������˳�.
*	���������������ʱ,�ÿ�ʼ���ȱ���,������ʱ����ʼ������Ⱥ��ߵ���ж�.
*	�ɼ��ȴ����жϳ����޸Ŀ�ʼ���ȱ���ֵ.
*	���������Ƿ�ȫ���������,���ȫ���������,׼���������м��.
*	�����û�д������,����ʼ��ӡ
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static void StartHeatAndRotateMotor(void)
{
	static	uint8_t	HeatTimeCalcFlag = 0;	//���ε��м�����Ҫ��ʱ���Ƿ�������
	static	uint16_t	HeatTime = 0;
	uint16_t	totalheatus;
	uint32_t	i;
	#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
		uint16_t	temp;
		TypeDef_StructLinePrtBuf * ptLPB = NULL;
	#endif
	PELedOff();   //2016.06.24  ����
	
	StopTimer(PRINT_CHECK_TIMER);
	if (g_tHeatCtrl.HeatEnable == HEAT_ENABLE)
	{
		if (HeatTimeCalcFlag == 0)	//������ÿ���ʱ��������ǰ׼������
		{
			HeatTime = GetHeatTime();	//�������ʱ��
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
				{	//�ַ����Դ�ӡʱ����ʱ����봦��
					if ((ptLPB->PrtMode.CharNotFlag) && (!g_tPrtCtrlData.PCDriverMode))
					{
						temp = HeatTime >> 2;
						HeatTime >>= 1;
						HeatTime += temp;
					}
				}
			#endif
			SetHeatNumber();					//���㱾���м��ȴ���,���ü��ȿ��ƹܽ�
			HeatTimeCalcFlag = 1;			//��ֹ�ظ�����
		}
		return;		//���֮ǰ�ĵ��м��������Ƿ����,���δ������˳�
	}
	else		//�����һ�μ����Ѿ���ɴ���ʼ
	{
		if (HeatTimeCalcFlag == 0)	//������ÿ���ʱ��������ǰ׼������,�����ֱ�Ӽ������ڴ˷�ֹ©��
		{
			HeatTime = GetHeatTime();	//�������ʱ��
			
			#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
				ptLPB = &U_PrtBuf.g_tLinePrtBuf;
				if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
				{	//�ַ����Դ�ӡʱ����ʱ����봦��
					if ((ptLPB->PrtMode.CharNotFlag) && (!g_tPrtCtrlData.PCDriverMode))
					{
						temp = HeatTime >> 2;
						HeatTime >>= 1;
						HeatTime += temp;
					}
				}
			#endif
			SetHeatNumber();					//���㱾���м��ȴ���,���ü��ȿ��ƹܽ�
			HeatTimeCalcFlag = 1;			//��ֹ�ظ�����
		}
		
		if (g_tSystemCtrl.StepNumber != 0)	//����ߵ���Ƿ����,���δ������˳�
		{
			return;
		}
		else	//�ߵ������ɴ���ʼ
		{
			g_HeatUs = HeatTime;
			g_tHeatCtrl.HeaterGroupNumberCpy = g_tHeatCtrl.HeaterGroupNumber;	//��ֵ���Ʊ���
			for (i = 0; i < g_tHeatCtrl.HeaterGroupNumberCpy; i++)
			{
				g_tHeatCtrl.HeaterGroupElementCpy[ i ] = g_tHeatCtrl.HeaterGroupElement[ i ];
				g_tHeatCtrl.HeaterGroupElement[ i ] = 0;
			}
			
			LatchHeatData();		//�����о����
			
			totalheatus = g_HeatUs * g_tHeatCtrl.HeaterGroupNumberCpy;	//һ�����ܼ���ʱ��
			AdjustMotorStep(totalheatus);					//��������ʱ,�������ٲ���
			SetTimerCounterWithAutoReload(HEAT_TIMER, g_HeatUs);	//ͬʱ�����Զ�����ֵ�ͼ�ʱֵ
			g_tHeatCtrl.HeatEnable = HEAT_ENABLE;	//�ÿ�ʼ���ȱ���
			HeatTime = 0;
			HeatTimeCalcFlag = 0;				//����׼����һ���м���ʱ�����
			PELedOff();   //2016.06.24  ����
			BeepOff();
			TIM_SetCounter(HEAT_TIMER, 1);
			StartTimer(HEAT_TIMER);								//������ʱ����ʼ������Ⱥ��ߵ���ж�
			CheckPrintContentFinish();	//���������Ƿ�ȫ���������,������TIM4
		}			//�ߵ������ɴ������
	}				//�ϴμ�������ɴ������
	
	#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
		ptLPB = NULL;
	#endif
}

#define	STEP_TIME_COM	   		(1000000 *10*(PRT_DOT_NUMBERS/12)*2/3)/32	 	//���ڱ�׼��ӡʱ����С�߲�ʱ��ϵ������2/3�ܶȼ��㡣
#define	STEP_TIME_COM_DRIVER	 (850)	 									//����������ӡʱ����С�߲�ʱ��̶�Ϊ1000uS	125mm/s
uc32 BpsValue[8] ={1200,2400,4800,9600,19200,38400,57600,115200};	//�����ʱ� 2016.05.23
/*******************************************************************************
* Function Name  : MaxSetp(void )
* Description    : ѡ�������ٵ������
* Input          : None
* CONput         : SystemCtrlStruct.MaxStep[i]�д�ŵ�ǰ�����µ������ٲ���
* Return         : None
*******************************************************************************/
void MaxSetp(void)			//2016.05.23
{
	uint8_t Bps, i;
	uint32_t Time[2];
  uint32_t TempNum[2];
	if(g_tSystemCtrl.PrtSourceIndex == COM)	 			//����
	{
		{
			Bps = g_tSysConfig.ComBaudRate & 0x07;
			if( Bps <= 3 )										//С�ڵ���9600�̶�Ϊͬһ�ٶ�
				Bps =3;
		}
		//ȡ����߲�ʱ��
		Time[0] =STEP_TIME_COM / BpsValue[Bps];				//�ַ�ģʽ��
        
		Time[1] =STEP_TIME_COM_DRIVER;								//����ģʽ
		
		//�����ַ�ģʽ������ģʽ����������ٶ��µ������ٲ���
		for(i=0; i<2; i++)			//SystemCtrlStruct.MaxStep[i]��Ϊ����������MaxStep[SysConfigStruct.MaxSpeed]��Ϊ�����ٶȵ������
		{
			for(g_tSystemCtrl.MaxStepPrtNum[i] =0; g_tSystemCtrl.MaxStepPrtNum[i] < MaxStepNum[g_tSysConfig.MaxSpeed];g_tSystemCtrl.MaxStepPrtNum[i]++ )
			{
				if( g_tFeedMotorRushTable[g_tSystemCtrl.MaxStepPrtNum[i]] < Time[i])
					break;
			}
		}

	}
	else													//���� usb�ڵ�
	{
		g_tSystemCtrl.MaxStepPrtNum[0]  =	MaxStepNum[g_tSysConfig.MaxSpeed]; 	//������ٶȸ�ֵ
		
		if((g_tSysConfig.MaxSpeed >=1))			  		//������ʽ �����õ��ٶȵȼ�����1
		{
			g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[1];
		}
		else
		{	
			g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[g_tSysConfig.MaxSpeed];	
		}	
		if((g_tSystemCtrl.PrtSourceIndex == LPT))          //2016.08.09����������ʽ�� ����ٶ�����  && (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)
    {
			 if(g_tSystemCtrl.MaxStepPrtNum[1] > LPT_Drver_MaxStep)
			 {	 
					g_tSystemCtrl.MaxStepPrtNum[1] = LPT_Drver_MaxStep;  
			 } 
    }	
		if((g_tSystemCtrl.PrtSourceIndex == USB) )          //����������ʽ�� ����ٶ����� && (g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE)
    {
			 if(g_tSystemCtrl.MaxStepPrtNum[1] > USB_Drver_MaxStep)
			 {	 
					g_tSystemCtrl.MaxStepPrtNum[1] = USB_Drver_MaxStep;          // MAX_RUSH_STEPS
			 } 
    }
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ϴ��ߵ���Ƿ����,���δ������˳�.
*	���������������ʱ,�������м��ֵ,������ʱ����ʼ�����ߵ��,
*	���³�ʼ����ӡ������,״̬��ΪILDE.
* Input          : �������,�м��g_tPrtCtrlData.CodeLineHigh
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#if	0
static void SetGoLineSpace(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	//���֮ǰ�ļ��ȼ��ߵ���������,����������м�ദ��,���򲻴���
	if ((g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && (g_tSystemCtrl.StepNumber == 0))
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
				(ptLPB->PrtMode.VariableBitImageFlag == 1)))
		{	//ͼ�δ�ӡģʽʱ�����м��
			if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
			{
				g_tSystemCtrl.StepNumber = 
					(ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh) * STEP_NUMBERS_PER_DOT_HEIGHT;
			}
		}
		
		ptLPB->Status	=	LPB_STATUS_ILDE;	//����ŵ�����ʱ����
		TIM_SetCounter(FEED_TIMER, 1);
		StartTimer(FEED_TIMER);						//������ʱ����ʼ�����ߵ��
		InitHeatElementBuf();							//������Ȼ�����
	}
}
#else
static void SetGoLineSpace(void)
{
	TypeDef_StructLinePrtBuf * ptLPB = NULL;
	
	StopTimer(PRINT_CHECK_TIMER);
	//���ж�֮ǰ�ļ��ȼ��ߵ�������Ƿ����,ֱ�ӽ��������м�ദ��,��������м���򲻴���
	//���֮ǰ�ļ��ȼ��ߵ���������,����������м�ദ��,���򲻴���
	if ((g_tHeatCtrl.HeatEnable == HEAT_DISABLE) && (g_tSystemCtrl.StepNumber == 0))
	{
		ptLPB = &U_PrtBuf.g_tLinePrtBuf;
		if (!((ptLPB->PrtMode.BitImageFlag == 1) || \
					(ptLPB->PrtMode.VariableBitImageFlag == 1)))
		{	//ͼ�δ�ӡģʽʱ�����м��
			if (ptLPB->PrtMode.CodeLineHigh > ptLPB->PrtMode.MaxLineHigh)
			{
				g_tSystemCtrl.StepNumber += \
				(ptLPB->PrtMode.CodeLineHigh - ptLPB->PrtMode.MaxLineHigh) * STEP_NUMBERS_PER_DOT_HEIGHT;
			}
		}
		PELedOn();   //2016.06.24  ����
		BeepOn();
		ptLPB->Status	=	LPB_STATUS_ILDE;
		InitHeatElementBuf();							//������Ȼ�����
		ClearLinePrtBuf();
		TIM_SetCounter(FEED_TIMER, 1);
		StartTimer(FEED_TIMER);						//������ʱ����ʼ�����ߵ��
		
		ptLPB = NULL;
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
