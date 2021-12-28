/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   ָ����س���.
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
#include	"monobyte.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"feedpaper.h"
#include	"charmaskbuf.h"
#include	"timconfig.h"
#include	"heat.h"
#include	"button.h"
#include	"led.h"
#include	"spiheat.h"
#include	"parallel.h"
#include	"drawer.h"

extern TypeDef_StructInBuffer volatile g_tInBuf;

extern void PrtDrvBufFull(void);
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

/*******************************************************************************
* Function Name  : Command_09
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_09(void)
{
	uint16_t	TempNum;
	
	if (g_tTab.TabIndex >= MAX_HT_INDEX_NUM) 	  //��ֹ�±�Խ��
	{
		return;
	}
	
	//�����ǰλ���� ��ӡ���+1, ��ӡ��ǰ������������ǰλ���ƶ�������
	if (g_tPrtCtrlData.PrtDataDotIndex >= g_tPrtCtrlData.PrtLength)
	{
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
		
	while(g_tTab.TabIndex < g_tTab.TabSetNumber)	//�����õ�����, û���������񲻴���
	{
    TempNum = g_tTab.TabPositionIndex[ g_tTab.TabIndex ] + g_tPrtCtrlData.PrtLeftLimit;
		//��һ������λ���Ƿ���ڵ�ǰλ��
		if (TempNum <= g_tPrtCtrlData.PrtDataDotIndex)
		{ //��һ������λ�ó�Խ�ɴ�ӡ����, ����ǰλ������Ϊ ��ӡ���+1
			g_tTab.TabIndex++;	//ִ����������1
		}
		else
		{
			if (TempNum >= g_tPrtCtrlData.PrtLength)
			{
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
			}
			else
			{
				g_tPrtCtrlData.PrtDataDotIndex = TempNum;
			}
			break;
		}	
	}
	
}

/*******************************************************************************
* Function Name  : Command_0A
* Description    : ���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0A(void)
{
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);	//��ӡ��ǰ�� 2016.06.28
}

/*******************************************************************************
* Function Name  : Command_0C
* Description    : ��ӡҳ,������ֻ��ҳģʽ����Ч,Ϊ����T3,�ڱ�׼ģʽ��Ϊ��ֽ���ڱ괦
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)		//����T3�Һڱ�
	{
// 		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);  2016.11.17 ȥ����ӡֻ���Һڱ�
		if ((g_tSysConfig.BlackMarkEnable))	//�Һڱ꿪����		
		{			
			while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);  //2016.06.30			
			FindMarkPoint();
		}
	}
	else
	{
		
	}
}

/*******************************************************************************
* Function Name  : Command_0D
* Description    : ���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_0D(void)
{	//���ʹ����CRָ��,���Ҳ��Ǵ��ڷ�ʽ
	if ((g_tSysConfig.CREnable) && (g_tSystemCtrl.PrtSourceIndex != COM))
	{
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
}

/*******************************************************************************
* Function Name  : Command_15
* Description    : ����������,��ֽn����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_15(void)
{
	uint32_t GetChar;
	uint32_t DotLineNumber;
	uint32_t i;
	
	GetChar = ReadInBuffer();	
	if (GetChar > 1)
	{	
		DotLineNumber = GetChar - 1;
		for (i = 0; i < DotLineNumber; i++)
		{
			PrtDrvBufFull();
			IncDrvPrtBufAddr(0x02, 0);
		}
	}
}

extern	uint8_t	g_tbHeatElementBuf[];	//���Ȼ�оһ�������еļ��ȼĴ�����Ԫ��
extern	TypeDef_StructHeatCtrl	g_tHeatCtrl;
extern	uint32_t	g_iDealOneLineTimeCount;

/*******************************************************************************
* Function Name  : Command_16
* Description    : ����������,����1��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	ONE_LINE_BYTES		(PRT_DOT_NUMBERS / 8)
void Command_16(void)
{
	volatile uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT]={0};
	uint8_t	Width, Number;
	uint8_t	* pSrc = NULL;
	uint32_t	i;
	uint8_t GetChar;	
	
	#if defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		g_iDealOneLineTimeCount = 0;
		SetTimerCounterWithAutoReload(DBG_TIMER, 1);
		StartTimer(DBG_TIMER);
	#endif
	
	Number = g_tPrtCtrlData.DriverOffset + ReadInBuffer();
	if (Number == 0)
	{
		return;
	}
	
	if (Number > (g_tSystemCtrl.LineWidth >> 3))
	{
		Width = (g_tSystemCtrl.LineWidth >> 3);
	}
	else
	{
		Width = Number;
	}
	
	memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);	//2016.06.21
	pSrc = DataBuf + g_tPrtCtrlData.DriverOffset;
	for (i = g_tPrtCtrlData.DriverOffset; i < Width; i++)
	{
		GetChar = ReadInBuffer();	//��ȡ���еĴ���ӡ����
		*pSrc++ = GetChar;
	}

// 	CopyDotLineDataToHeatElementBuf(DataBuf);	//���Ƶ����ȿ��ƻ�����  2016.06.16�������ô˷�ʽ	
// 	PrintOneDotLine();					//��ӡһ����
	DotLinePutDrvBuf( DataBuf );				//2016.06.16 ��һ�����ݵ�������ӡ������
	
	Command16RunFlag = 0;         //2016.06.15  16ָ�����н�����־  
	
	for (; i < Number; i++)			  //��������һ�е�����
	{	
		ReadInBuffer();
	}
	
	#if defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME
		StopTimer(DBG_TIMER);
	#endif
}

/*******************************************************************************
* Function Name  : Command_18
* Description    : ҳģʽ��,�����ӡ����������,������ֻ��ҳģʽ����Ч
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_18(void)
{
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	
	{
	
	}
}

/*******************************************************************************
* Function Name  : MonoByte_Command
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void MonoByte_Command(uint8_t CommandCode)
{
	switch (CommandCode)
	{
		case 0x09:				//09����,ִ�к�������
		{
			Command_09();
			break;
		}
		case 0x0A:				//0A����,����
		{
			Command_0A();
			break;
		}
		case 0x0C:
		{
			Command_0C();		//0C����,ҳģʽ��,��ӡҳ
			break;
		}
		case 0x0D:				//0D����,�س�
		{
			Command_0D();
			break;
		}
		case 0x15:				//15����,������ֽ����
		{
			Command_15();
			break;
		}
		case 0x16:				//16����,��������һ��������
		{
			Command_16();
			break;
		}
		case 0x18:				//18����,ҳģʽ�������ӡ����������
		{
			Command_18();
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1004
* Description    : ʵʱ״̬����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	DEFAULT_CMD_1004_STATUS_VALUE		(0x12)

void Command_1004(void)
{
	if ((g_tRealTimeCmd.RealTimeBuf[2] < 1) || (g_tRealTimeCmd.RealTimeBuf[2] > 4))
		return;
	
	//�õ�ֽ������״̬	10 04 n=4
	g_tRealTimeStatus.PaperStatus = DEFAULT_CMD_1004_STATUS_VALUE;	//�̶�ֵ
	if (g_tError.PaperSensorStatusFlag & 0x01)		//ȱֽ
		g_tRealTimeStatus.PaperStatus |= 96;
	if (g_tError.PaperSensorStatusFlag & 0x02)  	//ֽ����
		g_tRealTimeStatus.PaperStatus |=	12;
	
	//�õ�����״̬	10 04 n=3
	g_tRealTimeStatus.ErrorStatus = DEFAULT_CMD_1004_STATUS_VALUE;
	if (g_tError.R_ErrorFlag & 0x02)				//��е����(��ͷԭλ)
		g_tRealTimeStatus.ErrorStatus |= 4;
	if (g_tError.R_ErrorFlag & 0x01)				//�е�λ�ô���
		g_tRealTimeStatus.ErrorStatus |= 8;
	if (g_tError.UR_ErrorFlag)							//���ɻָ�����
		g_tRealTimeStatus.ErrorStatus |= 32;
	if (g_tError.AR_ErrorFlag)							//�ɻָ�����
		g_tRealTimeStatus.ErrorStatus |= 64;
	
	//�õ�����״̬	10 04 n=2
	g_tRealTimeStatus.OffLineStatus = DEFAULT_CMD_1004_STATUS_VALUE;
	if (g_tError.DoorOpen)			//�ϸ�  ���������Ŀ���״̬ 2016.08.04
		g_tRealTimeStatus.OffLineStatus |= 4;
	//������ֽ�����������ֽ
// #if defined	(FEED_BUTTON_CHECK_BY_INT)
// 	if (g_tFeedButton.KeyStatus)
// #else
// 	if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
// #endif
	{
		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())		        //2016.08.04
			g_tRealTimeStatus.OffLineStatus |= 8;
	}
	if (g_tRealTimeStatus.PaperStatus != DEFAULT_CMD_1004_STATUS_VALUE)	//ȱֹֽͣ
		g_tRealTimeStatus.OffLineStatus |= 32;
	if (g_tRealTimeStatus.ErrorStatus != DEFAULT_CMD_1004_STATUS_VALUE)		//����
		g_tRealTimeStatus.OffLineStatus |= 64;
	
	//�õ���ӡ��״̬  10 04 n=1
	g_tRealTimeStatus.PrinterStatus = DEFAULT_CMD_1004_STATUS_VALUE;

	if (READ_MONEYBOX_STATUS)	                          //Ǯ��ر�,��ȡ���ص�ƽΪ��  2016.08.02 
		g_tRealTimeStatus.PrinterStatus |= 4;	            //Ǯ�俪/���ź�Ϊ��(Ǯ���ͷ�ĵ�������),��ʾǮ��ر�
  else
		g_tRealTimeStatus.PrinterStatus &= ~4;
	if (g_tRealTimeStatus.OffLineStatus != DEFAULT_CMD_1004_STATUS_VALUE)	//����
		g_tRealTimeStatus.PrinterStatus |= 8;
	else
		g_tRealTimeStatus.PrinterStatus &= ~8;
	
// 	//������ֽ�����������ֽ
// #if defined	(FEED_BUTTON_CHECK_BY_INT)
// 	if (g_tFeedButton.KeyStatus)
// #else
// 	if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())
// #endif
// 	{
// 		if (g_tSystemCtrl.StepNumber)
// 			g_tRealTimeStatus.PrinterStatus |= 64;
// 	}
	
	switch (g_tRealTimeCmd.RealTimeBuf[2])
	{
	 	case 1:
		{
			UpLoadData(&g_tRealTimeStatus.PrinterStatus, 1);	//�ϴ�����
			break;
		}
	 	case 2:
		{
			UpLoadData(&g_tRealTimeStatus.OffLineStatus, 1);	//�ϴ�����
			break;
		}
	 	case 3:
		{
			UpLoadData(&g_tRealTimeStatus.ErrorStatus, 1);		//�ϴ�����
			break;
		}
	 	case 4:
		{
			UpLoadData(&g_tRealTimeStatus.PaperStatus, 1);		//�ϴ�����
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1005n
* Description    : ����ӡ��ʵʱ����,����ӡ���Ӵ���״̬�ָ����Ҽ������жϵĴ�ӡ,
*				   ���ǳ����˿ɻָ�����,�����ָ����ԡ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1005n(void)
{
	if (!g_tError.R_ErrorFlag)		//�пɻָ������ִ�и�ָ��
		return;
// 	if (g_tSystemCtrl.PrtSourceIndex == LPT)
// 	{
// 		if (g_tInBuf.BufFullFlag == COM_IN_BUF_FULL)	//���ڻ���æʱ������ִ��
// 			return;
// 	}
	//������������ִ��
	//����ӡ���Ӵ���״̬�ָ����Ҽ������жϵĴ�ӡ,���ǳ����˿ɻָ�����,�����ָ�����
	if (g_tRealTimeCmd.RealTimeBuf[2] == 1)
	{
		g_tError.R_ErrorRecoversFlag = 1;
	}
	//����ӡ���Ӵ���״̬�ָ��������������ջ������ʹ�ӡ������,���ǳ����˿ɻָ�����,�����ָ�����
	else if (g_tRealTimeCmd.RealTimeBuf[2] == 2)
	{
		//��ָ����ջ�����
		//ReceiveInterrupt(DISABLE);	//�ؽ����ж�
		DisableReceiveInterrupt();
		g_tInBuf.BytesNumber = 0;		//���ջ������ֽ���
		g_tInBuf.PutPosition = 0;		//���ջ�����
		g_tInBuf.GetPosition = 0;		//���ջ�����
		g_tInBuf.PE_BytesNumber = 0;
		g_tInBuf.PE_GetPosition = 0;
		g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;
		
		//���ӡ������
		PrepareNewLine();								//���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭			
		InitLinePrtBuf();								//��մ�ӡ������
		
		TIM_Cmd(FEED_TIMER, DISABLE);		//�رռ��ȿ��ƶ�ʱ��
		TIM_Cmd(FEED_TIMER, DISABLE);		//�رյ�����ƶ�ʱ��
		TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//�رմ�ӡ�����ƶ�ʱ��
		EndHeat();											//�ؼ��ȿ���
		EndFeedMotor();									//�رյ��
		g_bFeedTaskFlag = 0;
		g_tError.R_ErrorRecoversFlag = 2;
		
		//ReceiveInterrupt(ENABLE);		//��󿪽����ж�
		EnableReceiveInterrupt();
	}
	g_tError.R_ErrorFlag =0;				//
}

/*******************************************************************************
* Function Name  : Command_1006n
* Description    : ��ӡ��ʵʱ���󣬶�ȡ��ӡ��ִ��״̬ �����е�ִ����� �Ƿ��ӡ���ݵȡ�
*				           ��������������ʱ��־λ��λ�����е�ִ����ɺ��־λ����
* Input          : None
* Output         : None
* Return         : ��ǰ�д���                0001 0000  = 0x10
                   ��ǰ���ݴ�ӡ��ɲ��е�    0000 1000  = 0x08
                   ��ʼ״̬��������          0000 0001  = 0x01
*******************************************************************************/
void Command_1006n(void)	
{
	  uint8_t ErrTemp;
    if(g_tRealTimeCmd.RealTimeBuf[2] == 1)
    {       
			  if(g_tError.DoorOpen |g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag |(g_tError.PaperSensorStatusFlag & 0x01) )  //2018.02.08  ����ȱֽ
				{
					g_tSystemCtrl.BillPrtFlag |= 0x10;				  //�д�����λ��λ
				}	
				else
				{
					g_tSystemCtrl.BillPrtFlag &= ~0x10;
				}	
			
        UpLoadData(&g_tSystemCtrl.BillPrtFlag , 1);	   //����Ʊ�ݴ�ӡ״̬��
        return;
    }
		else if(g_tRealTimeCmd.RealTimeBuf[2] == 2)			  //����״̬����
    {       
			  ErrTemp = 0x01;
				if (g_tError.PaperSensorStatusFlag & 0x01)		//ȱֽ
					ErrTemp |= 0x04;
				if (g_tError.DoorOpen)												//�ϸ�  
					ErrTemp |= 0x08;
				if (g_tError.R_ErrorFlag & 0x01)				      //�е�λ�ô���	
					ErrTemp |= 0x10;
				if (g_tError.AR_ErrorFlag & 0x01)             //bit.0 = 1 ��ӡͷ����
				  ErrTemp |= 0x20;
				if (g_tError.PaperSensorStatusFlag & 0x02)  	//ֽ����
					ErrTemp |= 0x40;
				
        UpLoadData(&ErrTemp , 1);	   //����Ʊ�ݴ�ӡ״̬��
        return;
    }
}

/*******************************************************************************
* Function Name  : Command_1014fnmt
* Description    : ʵʱ����Ǯ�俪������,��ָ����Ǯ��������Ų����趨�Ŀ�������,
	������mָ���� 
fn = 1
m = 0, 1
1  ��  t  ��  8
m  ��������
0  Ǯ�������������2
1  Ǯ�������������5
����ߵ�ƽʱ��Ϊ[ t �� 100 ms]���͵�ƽ��ʱ��Ϊ[ t �� 100 ms]��
��  ���ڴ����ָ�����ӡ�������ڴ���״̬ʱ����ָ����ԡ�
��  ����ӡ������ִ��Ǯ�俪��ָ��(ESC p   ��   DEL DC4)ʱ����ָ����ԡ� 
��  ����ģʽ�£���ӡ�����ո�ָ�������ִ�С� 
��  ����ģʽ�£���ӡ��æ��ʱ���ָ�ִ�С� 
��  �����ӡ�����а����ʹ�ָ����ͬ�����ݣ�����Щ���ݽ���������ָ��ִ�С��û�
���뿼�ǵ���������� 
  ���磺ͼ�����ݿ���żȻ���������ָ��һ�µ����ݴ��� 
��  ��ָ�����Ҫ���� 2 ��������ֽڵ�ָ�������С� 
��  ��ʹ��ӡ���� ESC = (ѡ������)ָ������Ϊ��ֹ����ָ����Ȼ��Ч��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1014fnmt(void)
{
	if ((g_tRealTimeCmd.RealTimeBuf[2] == 1) && \
			(g_tRealTimeCmd.RealTimeAllow & 0x01))	//����Ǯ�俪��ʵʱ����
	{	//�ϴ�ִ�����,���κδ���,���ڲ�æʱ��ִ�и�ָ��		
		if(g_tRealTimeCmd.RealTimeBuf[3] <2 && (g_tRealTimeCmd.RealTimeBuf[4] >0 && g_tRealTimeCmd.RealTimeBuf[4] <9))	//m��tΪ��Ч�Ĳ���
		{
			if(	g_tRealTimeCmd.RealTimeBuf[3] ==0 && g_tDrawer1.Status == DISABLE && g_tDrawer1.ON_Time ==0)	//1B70ָ��δִ��
			{
				g_tDrawer1.DrwID = g_tRealTimeCmd.RealTimeBuf[3];
				g_tDrawer1.OFF_Time	= 100 * g_tRealTimeCmd.RealTimeBuf[4];	 
				g_tDrawer1.ON_Time	= 2 * g_tDrawer1.OFF_Time;						   
					
				DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//��Ǯ��
			}
			else if(g_tRealTimeCmd.RealTimeBuf[3] ==1 && g_tDrawer2.Status ==0 && g_tDrawer2.ON_Time ==0)		//��û����ִ��Ǯ��ָ����ͬ����������Ч��������Դ˴β���
			{
				g_tDrawer2.DrwID = g_tRealTimeCmd.RealTimeBuf[3];
				g_tDrawer2.OFF_Time	= 100 * g_tRealTimeCmd.RealTimeBuf[4];	//�رպ���ʱʱ��
				g_tDrawer2.ON_Time	= 2 * g_tDrawer2.OFF_Time;						//�ܿ���ʱ��
				
				DriveDrawer(g_tDrawer2.DrwID, ENABLE);							//��Ǯ��
			}
		}
	}	//ִ�йػ������д����
	else if (g_tRealTimeCmd.RealTimeBuf[2] == 2 && \
					(g_tRealTimeCmd.RealTimeAllow & 0x02))	//ִ�йػ�����
	{
		
	}
	else if ((g_tRealTimeCmd.RealTimeBuf[2] == 8) && \
					(g_tRealTimeCmd.RealTimeBuf[3] == 1)	&& \
					(g_tRealTimeCmd.RealTimeBuf[4] == 3)	&& \
					(g_tRealTimeCmd.RealTimeBuf[5] == 20)	&& \
					(g_tRealTimeCmd.RealTimeBuf[6] == 1)	&& \
					(g_tRealTimeCmd.RealTimeBuf[7] == 6)	&& \
					(g_tRealTimeCmd.RealTimeBuf[8] == 2)	&& \
					(g_tRealTimeCmd.RealTimeBuf[9] == 8)
					)	//�建�����ĳ����д����
	{
		//������ջ������ʹ�ӡ�������е����ݲ����������Ӧ,��������˿ɻָ��Ĵ���,��Ӵ����лָ�
		//�ڷ��͸�ָ���,ֱ�����յ�����״̬ǰ,��Ҫ�ٷ�����������
		//����ջ�����
		//ReceiveInterrupt(DISABLE);	//�ؽ����ж�
		DisableReceiveInterrupt();
		g_tInBuf.BytesNumber = 0;		//���ջ������ֽ���
		g_tInBuf.PutPosition = 0;		//���ջ�����
		g_tInBuf.GetPosition = 0;		//���ջ�����			
		//ReceiveInterrupt(ENABLE);		//�������ж�
		EnableReceiveInterrupt();
		//���ӡ������
		PrepareNewLine();								//���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
		InitLinePrtBuf();								//��մ�ӡ������
		EndHeat();											//�ؼ��ȿ���
		TIM_Cmd(FEED_TIMER, DISABLE);		//�رռ��ȿ��ƶ�ʱ��
		EndFeedMotor();									//�رյ��
		TIM_Cmd(FEED_TIMER, DISABLE);		//�رյ�����ƶ�ʱ��
		TIM_Cmd(PRINT_CHECK_TIMER, DISABLE);	//�رմ�ӡ�����ƶ�ʱ��
		
// 		g_tError.R_ErrorRecoversFlag = 1;
	}
}


/*******************************************************************************
* Function Name  : RealTimeCommand
* Description    : ʵʱָ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RealTimeCommand(void)
{
	//if ((g_tRealTimeCmd.RealTimeBuf[0] == 0x1B) && (g_tInterface.COM_Type == COM))
	if (g_tRealTimeCmd.RealTimeBuf[0] == 0x1B)
	{
		UpLoadData(g_tError.ASBStatus, 4); 		//����״̬��
	}
	else
	{
		switch (g_tRealTimeCmd.RealTimeBuf[1])
		{
			case 4:
				Command_1004();
				break;
			case 5:
				Command_1005n();
				break;
			case 6:
			  Command_1006n();
			  break;
			case 0x14:
				Command_1014fnmt();
				break;
		}
	}
	g_tRealTimeCmd.RealTimeFlag = 0;
	g_tRealTimeCmd.RealTimeCntr = 0;
}

/*******************************************************************************
* Function Name  : AutomaticStatusBack(void)
* Description    : �Զ�״̬���ش������,��T2�жϷ�������е���,ÿ����msִ��һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	ASB_INTERVAL_MS			(100)			   //2016.07.07  1000��Ϊ100
void AutomaticStatusBack(void)
{
	static	uint8_t	Cntr = 0;
	uint8_t	OffLineStatus;
	uint8_t	Status[4];
	uint8_t static OldOffLineStatus =0;		//��һ��״ֵ̬
	uint8_t	Temp =0;	
	
	Cntr++;
	if (Cntr == ASB_INTERVAL_MS)		     //50
	{
		Cntr = 0;
		memset(Status, 0x00, 4);	//Ĭ�Ϲ̶�ֵ,δ����
		Status[0] = 0x10;
		Status[3] = 0x0f;					//2016.07.07
				
		//ֽ״̬
		if (g_tError.PaperSensorStatusFlag & 0x01)		//ȱֽ
		{
			Status[2] |= 0x0C;
		}
//  if (!(g_tError.AR_ErrorFlag & 0x02))	//״̬��ѯ �����ϸ�״̬
		{
			if (g_tError.PaperSensorStatusFlag & 0x02)	//ֽ����
				Status[2] |= 0x03;
		}
		if(	Status[2] !=g_tError.ASBStatus[2])					
			Temp |= 8;								//ֽ��״̬���б仯
		
		
		//�õ���ӡ����Ϣ״̬
		Status[1] = 0;
		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//������ֽ��
			Status[1] |= 2;
		if (g_tError.R_ErrorFlag & 0x02)	//��е����(��ͷԭλ)
			Status[1] |= 4;
		if (g_tError.R_ErrorFlag & 0x01)	//�е�λ�ô���
			Status[1] |= 8;
		if (g_tError.UR_ErrorFlag)				//���ɻָ�����
			Status[1] |= 32;
		if (g_tError.AR_ErrorFlag)				//���Զ��ָ�����
			Status[1] |= 64;
		if(	Status[1] !=g_tError.ASBStatus[1])					
			Temp |= 4;								//����״̬�ź��б仯

		
		//�õ�����״̬
		OffLineStatus = 0x00;
		if (g_tError.AR_ErrorFlag & 0x02)	//�ϸ�
		{
			Status[0] |= 32;
			OffLineStatus |= 4;
		}
		else if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//������ֽ��
		{
			Status[0] |= 64;
			OffLineStatus |= 8;
		}
		else if (Status[2])								//ȱֹֽͣ
			OffLineStatus |= 32;
		else if (Status[1])								//����
			OffLineStatus |= 64;

		if( OldOffLineStatus != OffLineStatus )		//����״̬�仯
			Temp |= 2;								      //����״̬�ź��б仯

		if(OffLineStatus !=0x0)						//����
			Status[0] |= 8;		

		if( READ_MONEYBOX_STATUS )					//Ǯ��
			Status[0] |= 4;
		if(( Status[0] & 4) != (g_tError.ASBStatus[0] & 4) )					
			Temp |= 1;			
		
// 		//�õ���ӡ��״̬
// 		Status[0] = 0x10;
// 		#if	0
// 		if (MONEYBOX_STATUS_CLOSE == ReadMoneyBoxStatus())	//Ǯ��ر�,��ȡ���ص�ƽΪ��
// 		{
// 			Status[0] |= 4;
// 		}
// 		#endif
// 		if (OffLineStatus)					//����
// 		{
// 			Status[0] |= 8;
// 		}
// 		if (g_tError.AR_ErrorFlag & 0x02)		//�ϸ�
// 			Status[0] |= 32;
// 		if (FEED_BUTTON_STATUS_CLOSE == ReadFeedButtonStatus())	//������ֽ��
// 			Status[0] |= 64;
// 		
// 		for (OffLineStatus = 0; OffLineStatus < (sizeof(Status) / sizeof(Status[0])); OffLineStatus++)
// 			g_tError.ASBStatus[OffLineStatus] = Status[OffLineStatus];

		OldOffLineStatus = OffLineStatus;			//��������״̬
		g_tError.ASBStatus[0] =Status[0];
		g_tError.ASBStatus[1] =Status[1];
		g_tError.ASBStatus[2] =Status[2];
		g_tError.ASBStatus[3] =Status[3];
		
		if(g_tError.ASBAllowFlag & Temp)			//�Զ�״̬����ʹ��
		{	
			UpLoadData(g_tError.ASBStatus,4); 	//����״̬��
		}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
