/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   ͨ�Ž��ջ�������صĳ���.
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
#include	"cominbuf.h"
#include	"extgvar.h"
#include	"feedpaper.h"
#include	"button.h"
#include	"usart.h"
//#include	"usb_prop.h"
#include	"parallel.h"
#include	"timconfig.h"

extern	void PrintString(uint8_t *Str);
extern	void MaxSetp(void);
extern  void USB_ClearBusy(void);
extern uint8_t ETH_ConnectState;   //2018.03.05
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// TypeDef_StructInBuffer volatile g_tInBuf;

uint8_t Eth_ClearBusy=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : void SetComInBufBusy(void)
* Description    : ���ͨ�Ŷ˿�æ
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	SetComInBufBusy(void)
{
	if (g_tInterface.LPT_Type)
		SetParallelBusy();			//������æ
	
	if (g_tInterface.COM_Type)
		SetUsartBusy();					//��æ
	if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))
			BT_BUSY_SET;
// 	if (g_tInterface.USB_Type)
// 		Set_USB_Busy();					//������USB������֡����
}

/*******************************************************************************
* Function Name  : void ClearComInBufBusy(void)
* Description    : ���ͨ�Ŷ˿�æ
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	ClearComInBufBusy(void)
{
	if (g_tInterface.LPT_Type)
		LPT_ACK_Out();					//������æ
	
	if (g_tInterface.COM_Type)
		ClearUsartBusy();				//��æ
	
	if (g_tInterface.USB_Type)
		USB_ClearBusy();				//����USB������֡����
	
	if(g_tInterface.ETH_Type) 
		Eth_ClearBusy =1;		//��̫����æ��־2016.10.08
		if (g_tInterface.BT_Type)
	{
		BT_BUSY_CLR;
	}	
	if(g_tInterface.WIFI_Type)
		WIFI_BUSY_CLR;
}

/*******************************************************************************
* Function Name  : void EnableReceiveInterrupt(void)
* Description    : �رջ�򿪽��յ��ж�
* Input          : NewState��DISABLE �رգ�ENABLE ��
* Output         : None
* Return         : None
*******************************************************************************/
void	EnableReceiveInterrupt(void)
{

	if (g_tInterface.USB_Type)					//USB
	{
		NVIC_EnableIRQ(OTG_FS_IRQn);
	}
	if (g_tInterface.COM_Type)					//USB+S
	{
		NVIC_EnableIRQ(USART_IRQ_NUMBER);
	}	
	  if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))					 //BT 2016.07.29 
	{
		NVIC_EnableIRQ(BT_UART_IRQn);
	}	

}

/*******************************************************************************
* Function Name  : void DisableReceiveInterrupt(void)
* Description    : �رջ�򿪽��յ��ж�
* Input          : NewState��DISABLE �رգ�ENABLE ��
* Output         : None
* Return         : None
*******************************************************************************/
void	DisableReceiveInterrupt(void)
{

	if (g_tInterface.USB_Type)					//USB
	{
		NVIC_DisableIRQ(OTG_FS_IRQn);
	}
	if (g_tInterface.COM_Type)					//S
	{
		NVIC_DisableIRQ(USART_IRQ_NUMBER);
	}	
		if ((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))					 //BT 2016.07.29 
	{
		NVIC_DisableIRQ(BT_UART_IRQn);
	}	
	//2016.06.03 �Ժ��������������ڵ���Ӧ����
}

/*******************************************************************************
* Function Name  : void ReceiveInterrupt(FunctionalState NewState)
* Description    : �رջ�򿪽��յ��ж�
* Input          : NewState��DISABLE �رգ�ENABLE ��
* Output         : None
* Return         : None
*******************************************************************************/
void	ReceiveInterrupt(FunctionalState NewState)
{
	if (NewState == DISABLE)
	{
		if (g_tInterface.COM_Type)							//����
		{
			NVIC_DisableIRQ(USART_IRQ_NUMBER);
		}
		if (g_tInterface.LPT_Type)							//����
		{
		#ifdef	STM32F10X_HD
			NVIC_DisableIRQ(PARALLEL_STB_INT_IRQn);
		#endif
		}
		if (g_tInterface.USB_Type)							//USB
		{
		#ifdef STM32F10X_CL
			NVIC_DisableIRQ(OTG_FS_IRQn);
		#else
			NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
		#endif
		}
	}
	else
	{
		if (g_tInterface.COM_Type)
		{
			NVIC_EnableIRQ(USART_IRQ_NUMBER);
		}
		if (g_tInterface.LPT_Type)							//����
		{
		#ifdef	STM32F10X_HD
			NVIC_EnableIRQ(PARALLEL_STB_INT_IRQn);
		#endif
		}
		if (g_tInterface.USB_Type)
		{
		#ifdef STM32F10X_CL
			NVIC_EnableIRQ(OTG_FS_IRQn);
		#else
			NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
		#endif
		}
	}
}

/*******************************************************************************
* Function Name  : void SetActiveComInBusy(void)
* Description    : �����Ծ��ͨ�Ŷ˿�æ
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	SetActiveComInBusy(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//����
	{
		SetUsartBusy();					//��æ
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//����
	{
		SetParallelBusy();			//������æ
	}
// 	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
// 	{
// 		Set_USB_Busy();					//������USB������֡����
// 	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void ClearActiveComInBusy(void)
* Description    : �����Ծ��ͨ�Ŷ˿�æ
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
void	ClearActiveComInBusy(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//����
	{
		ClearUsartBusy();				//��æ
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//����
	{
		LPT_ACK_Out();					//������æ
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
	{
		USB_ClearBusy();				//����USB������֡����
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void EnableActiveReceiveInterrupt(void)
* Description    : �򿪻�Ծ�Ľ��յ��ж�
* Input          : NewState��DISABLE �رգ�ENABLE ��
* Output         : None
* Return         : None
*******************************************************************************/
void	EnableActiveReceiveInterrupt(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)
	{
	#ifdef	STM32F10X_CL
		NVIC_EnableIRQ(UART4_IRQn);
	#else
		NVIC_EnableIRQ(USART_IRQ_NUMBER);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)		//����
	{
	#ifdef	STM32F10X_HD
		NVIC_EnableIRQ(PARALLEL_STB_INT_IRQn);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)
	{
	#ifdef STM32F10X_CL
		NVIC_EnableIRQ(OTG_FS_IRQn);
	#else
		NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	#endif
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name  : void DisableActiveReceiveInterrupt(void)
* Description    : �رջ�Ծ�Ľ��յ��ж�
* Input          : NewState��DISABLE �رգ�ENABLE ��
* Output         : None
* Return         : None
*******************************************************************************/
void	DisableActiveReceiveInterrupt(void)
{
	if (g_tSystemCtrl.PrtSourceIndex == COM)	//����
	{
	#ifdef	STM32F10X_CL
		NVIC_DisableIRQ(UART4_IRQn);
	#else
		NVIC_DisableIRQ(USART_IRQ_NUMBER);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == LPT)	//����
	{
	#ifdef	STM32F10X_HD
		NVIC_DisableIRQ(PARALLEL_STB_INT_IRQn);
	#endif
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)			//USB
	{
	#ifdef STM32F10X_CL
		NVIC_DisableIRQ(OTG_FS_IRQn);
	#else
		NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	#endif
	}
	else
	{
	}
}

/*******************************************************************************
* Function Name : void	InitInBuf(void)
* Description		: ��ʼ�����ջ�����
* Input	      	: 
* Output	     	: ���� g_tInBuf ȫ�ֱ����ṹ
* Return	     	: None
*******************************************************************************/
void	InitInBuf(void)
{
	g_tInBuf.BytesNumber = 0;
	g_tInBuf.PutPosition = 0;
	g_tInBuf.GetPosition = 0;
	g_tInBuf.PE_BytesNumber = 0;
	g_tInBuf.PE_GetPosition = 0;
	g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;
}

/*******************************************************************************
* Function Name : 
* Description		: ��ȡ���ջ��������ݸ���
* Input	      	: 
* Output	     	: ���� g_tInBuf ȫ�ֱ����ṹ
* Return	     	: 
*******************************************************************************/
uint32_t	GetInBufBytesNumber(void)
{
	uint32_t Temp1;								//2016.07.07
	Temp1 = g_tInBuf.PutPosition;	//�ж��п��ܸı���ֵ
	if(Temp1 >= g_tInBuf.GetPosition)
		Temp1 = Temp1 - g_tInBuf.GetPosition;
	else
		Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
	g_tInBuf.BytesNumber = Temp1;
	return	(g_tInBuf.BytesNumber);			
}

#ifdef	PAPER_OUT_RE_PRINT_ENABLE
extern	void	EndHeat(void);
extern	void	InitHeatCtrlStruct(void);
extern	void	CutMotorStandby(void);
extern	void	InitCharMaskBufParameter(void);
extern	void	PrepareNewLine(void);
extern	void	InitLinePrtBuf(void);

/*******************************************************************************
* Function Name  : ClearInBuf(void)
* Description    : 2016.10.08
                   ִ���е���ѵ�ǰ�Ķ�ָ����Ϊ��һ������ʼ�ŵ�g_tInBuf.PE_BytesNumber��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClearInBuf(void)
{
	//SetComInBufBusy();
	DisableReceiveInterrupt();				//�ؽ����ж�
	//g_tInBuf.PE_BytesNumber = g_tInBuf.BytesNumber;
	g_tInBuf.PE_GetPosition = g_tInBuf.GetPosition;	//�޸�ȡ��ָ��
	EnableReceiveInterrupt();					//�������ж�
}

/*******************************************************************************
* Function Name  : void PaperOutDetect(void)
* Description    : ȱֽ�Զ����´�ӡ����.���ȱֽ�ҵ�ǰ���ݽ��յ����ݲ���������,
*	׼�����´�ӡ.�����ǰ���ݽ��յ����ݴ�����������,�����´�ӡ.
�Ѷ���ָ���Ƶ�һ���Ŀ�ʼλ�ã�����༭�������ʹ�ӡ������������ֱ�־λ��
*	PE_BytesNumberͳ�Ʊ����ݵ��ֽ���.PE_GetPosition��ű����ݴ���ʼ��ַ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	PaperOutDetect(void)
{
	static uint8_t	PaperEndNum = 0;
	static uint8_t	PaperEndFlag = 0x55;
	
	if (g_tSysConfig.PaperOutReprint)
	{
		if ((g_tError.PaperSensorStatusFlag & 0x01) && \
				(g_tSystemCtrl.PaperType == 0))				
		{
			if(PaperEndFlag == 0x55)
			{	
				PaperEndNum++;
			}	
		}
		else
		{
			PaperEndNum = 0;
			PaperEndFlag = 0x55;
		}	
    if( PaperEndNum >50)
    {			
			PaperEndNum = 0;
			PaperEndFlag = 0xAA;
			//SetComInBufBusy();
			DisableReceiveInterrupt();			//�ؽ����ж�
			//g_tInBuf.BytesNumber = g_tInBuf.PE_BytesNumber;	 //�޸������ֽ���
			g_tInBuf.GetPosition = g_tInBuf.PE_GetPosition;	   //�޸�ȡ��ָ��
			EnableReceiveInterrupt();				//�������ж�
			GetInBufBytesNumber();
// 			if (g_tInBuf.BytesNumber > (g_tInBuf.BytesSize - MIN_RSV_BYTES_OF_IN_BUF))
// 			{	//������С����С������
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			     //����������־
// 			}
// 			if (g_tInBuf.BufFullFlag == COM_IN_BUF_NOT_FULL)   //������������æ
// 			{
// 				ClearComInBufBusy();
// 			}
			
			//��մ�ӡ�������ͱ༭������,������ǰ�Ĵ�ӡ����
			TIM_Cmd(HEAT_TIMER, DISABLE);					//�ض�ʱ��3
			TIM_Cmd(DealPrtData_TIMER, DISABLE);  //�ض�ʱ��5
			EndHeat();										//�ؼ���
			EndFeedMotor();								//����ֽ���
			#if defined (CUTTER_ENABLE)
				CutMotorStandby();					//���е����
			#endif
			InitHeatCtrlStruct();					//��ʼ�����ȿ��ƽṹ
			InitCharMaskBufParameter();		//�����ģ��ȡ������,�ַ���������־��ΪILDE
			PrepareNewLine();
			InitLinePrtBuf();
			g_tSystemCtrl.StepNumber = 0;
			g_bMotorRunFlag = 0;
			//һֱ�ȴ�����װֽ�ҹر�ֽ�ָǲ��˳�
			//while ((g_tError.PaperSensorStatusFlag & 0x01) || (g_tError.AR_ErrorFlag & 0x02));
		}
	}
}
#endif

/*******************************************************************************
* Function Name : static void FillRealTimeCommand(uint8_t Data)
* Description		: ���ʵʱָ��
* Input	      	: �ֽ���ʽ�Ľ�������
* Output	     	: ���� g_tRealTimeCmd ȫ�ֱ����ṹ
* Return	     	: None
*******************************************************************************/
static void FillRealTimeCommand(uint8_t Data)
{
	static uint8_t CommandLength = 0;
	
	//----ʵʱָ�����ݵ�������----//
	if (g_tRealTimeCmd.RealTimeCntr == 0)
	{
		#ifdef	DBG_REALTIME_1B76_ENABLE
	 	if ((Data == 0x10) || (Data == 0x1B))
		#else
		if (Data == 0x10)
		#endif
		{
			g_tRealTimeCmd.RealTimeCntr = 1;			//ʵʱָ�����ݼ�����
			g_tRealTimeCmd.RealTimeBuf[0] = Data;
		}
	}
	else if (g_tRealTimeCmd.RealTimeCntr == 1)
	{
		#ifdef	DBG_REALTIME_1B76_ENABLE
		if ((Data == 0x04) || (Data == 0x05) || (Data == 0x06) || (Data == 0x14) || (Data == 0x76))
		#else
		if ((Data == 0x04) || (Data == 0x05) || (Data == 0x06) || (Data == 0x14))		//2016.08.02����1006
		#endif
		{
			g_tRealTimeCmd.RealTimeCntr = 2;			//ʵʱָ�����ݼ�����
			g_tRealTimeCmd.RealTimeBuf[1] = Data;
			#ifdef	DBG_REALTIME_1B76_ENABLE
			if (g_tRealTimeCmd.RealTimeBuf[1] == 0x76)	//1B 76
			{
				g_tRealTimeCmd.RealTimeFlag = 1;
			}
			#endif
		}
		else
		{
			g_tRealTimeCmd.RealTimeCntr = 0;			//ʵʱָ�����ݼ�����
		}
	}
	else if (g_tRealTimeCmd.RealTimeCntr == 2)
	{
		if ((Data > 0x00) && (Data < 0x08))
		{
			g_tRealTimeCmd.RealTimeCntr = 3;			//ʵʱָ�����ݼ�����
			g_tRealTimeCmd.RealTimeBuf[2] = Data;
			if ((g_tRealTimeCmd.RealTimeBuf[1] == 4) || (g_tRealTimeCmd.RealTimeBuf[1] == 5)|| (g_tRealTimeCmd.RealTimeBuf[1] == 6))	//1004, 1005 ��1006
			{
				g_tRealTimeCmd.RealTimeFlag = 1;
			}
			else 		//10 14
			{
				if (Data < 0x03)
				{
					CommandLength =	g_tRealTimeCmd.RealTimeCntr + 2;
				}
				else if (Data == 0x08)
				{
					CommandLength =	g_tRealTimeCmd.RealTimeCntr + 7;
				}
				else
				{
					g_tRealTimeCmd.RealTimeCntr = 0;
				}
			}
		}
		else
		{
			g_tRealTimeCmd.RealTimeCntr = 0;			//ʵʱָ�����ݼ�����
		}
	}
	else if ((g_tRealTimeCmd.RealTimeCntr < CommandLength) && \
					 (g_tRealTimeCmd.RealTimeFlag != 1)) //10 14ָ��
	{
		g_tRealTimeCmd.RealTimeBuf[g_tRealTimeCmd.RealTimeCntr] = Data;
		g_tRealTimeCmd.RealTimeCntr++;
		if (g_tRealTimeCmd.RealTimeCntr == CommandLength)
		{
			g_tRealTimeCmd.RealTimeFlag = 1;
			CommandLength = 0;
		}
	}
}

/*******************************************************************************
* Function Name  : void	PutNetCharToBuffer(uint8_t Data);
* Description		: �����յ�һ�ֽ����ݴ��뵽���뻺������
* Input	      	: �ֽ���ʽ�Ľ�������
* Output	     	: ���� g_tInBuf ȫ�ֱ����ṹ
* Return	     	: None
*******************************************************************************/
#ifdef	DBG_RCV_DATA_LOSE
volatile	uint32_t	g_uiTotalRcvByte = 0;
volatile	uint32_t	g_uiTotalReadByte = 0;
#endif

uint8_t PutNetCharToBuffer(uint8_t *Bufer, uint16_t Length, uint8_t Port)//06
{
	uint32_t Temp;
	uint16_t i;
	uint8_t	Data;
	
	g_tSystemCtrl.PrtSourceIndex = Port;			//��������Դ
	g_tSystemCtrl.SleepCounter=0;	 			//��������߼���

	for(i=0; i< Length; i++)
	{
		Data = Bufer[i];
		g_tInBuf.Buf[g_tInBuf.PutPosition] = Data;				//���뵱ǰ�ַ�
		g_tInBuf.PutPosition++;														//����ͷָ������
		if (g_tInBuf.PutPosition >= g_tInBuf.BytesSize)	  //���ﻺ�������λ��
		{
			g_tInBuf.PutPosition = 0;												//���´�0��ʼ����
		}
		//g_tInBuf.BytesNumber++;														//�޸������ֽ��� 2016.07.28
		
		FillRealTimeCommand(Bufer[i]);									  //���ʵʱָ��

		
		if ((g_tRealTimeCmd.RealTimeEnable == 1) && \
					(g_tRealTimeCmd.RealTimeFlag == 1))
		{
			RealTimeCommand();	//ʵʱָ��
		}
	}

  
	//ȱֽ�ش�ӡ�����,PutPosition��PE_GetPosition֮���ֵС��MIN_RSV_BYTES_OF_IN_BUF����æ
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
// 		if (g_tSysConfig.PaperOutReprint)
// 		{
// 			if(g_tInBuf.PutPosition >= g_tInBuf.PE_GetPosition )
// 				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.PE_GetPosition;
// 			else
// 				Temp = g_tInBuf.PE_GetPosition - g_tInBuf.PutPosition;

// 			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE���ڵ���USB����̫���Ľ��հ�����Ϊ�˱���һ������
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//��������		
// 		}		
// 		else		//�������
		{
				//���㻺����ʣ��ռ�
			if(g_tInBuf.PutPosition >= g_tInBuf.GetPosition )
				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.GetPosition;
			else
				Temp = g_tInBuf.GetPosition - g_tInBuf.PutPosition;

			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE���ڵ���USB����̫���Ľ��հ�����Ϊ�˱���һ������
				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//��������
		}	
	#endif
		
  return g_tInBuf.BufFullFlag;
}
/*******************************************************************************
* Function Name  : void	PutCharToInBuffer(uint8_t Data);
* Description		: �����յ�һ�ֽ����ݴ��뵽���뻺������
* Input	      	: �ֽ���ʽ�Ľ�������
* Output	     	: ���� g_tInBuf ȫ�ֱ����ṹ
* Return	     	: None
*******************************************************************************/

void PutCharToInBuffer(uint8_t Data)
{
	uint32_t Temp;
	
	g_tSystemCtrl.SleepCounter = 0;                    //2016.06.11
// 	if (g_tInBuf.BytesNumber < g_tInBuf.BytesSize)			//��������δ��������
	{
		g_tInBuf.Buf[g_tInBuf.PutPosition] = Data;				//���뵱ǰ�ַ�
		g_tInBuf.PutPosition++;														//����ͷָ������
		if (g_tInBuf.PutPosition >= g_tInBuf.BytesSize)	//���ﻺ�������λ��
		{
			g_tInBuf.PutPosition = 0;												//���´�0��ʼ����
		}
			
 		//if (g_tInBuf.BytesNumber < g_tInBuf.BytesSize)
		//g_tInBuf.BytesNumber++;														//�޸������ֽ��� 2016.07.28
		#ifdef	DBG_RCV_DATA_LOSE
		g_uiTotalRcvByte++;
		#endif		

	  //ȱֽ�ش�ӡ�����,PutPosition��PE_GetPosition֮���ֵС��MIN_RSV_BYTES_OF_IN_BUF����æ
  	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
// 		if (g_tSysConfig.PaperOutReprint)
// 		{
// 			if(g_tInBuf.PutPosition >= g_tInBuf.PE_GetPosition )
// 				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.PE_GetPosition;
// 			else
// 				Temp = g_tInBuf.PE_GetPosition - g_tInBuf.PutPosition;

// 			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE���ڵ���USB����̫���Ľ��հ�����Ϊ�˱���һ������
// 				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//��������		
// 		}		
// 		else		//�������
		{		
			if(g_tInBuf.PutPosition >= g_tInBuf.GetPosition )			//2016.07.28 ����ʣ��ռ�
				Temp = (g_tInBuf.BytesSize - g_tInBuf.PutPosition) + g_tInBuf.GetPosition;
			else
				Temp = g_tInBuf.GetPosition - g_tInBuf.PutPosition;

			if( Temp < MIN_RSV_BYTES_OF_IN_BUF)  	//MIN_RSVBUF_SIZE���ڵ���USB����̫���Ľ��հ�����Ϊ�˱���һ������
			{	
				g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//��������
			}
		}		
		#endif
		
		if(g_tPrtCtrlData.PCDriverMode != 0)			  //2016.06.11 ����ģʽ������ʵʱָ��
		{
			return;
		}	
		
		FillRealTimeCommand(Data);									//���ʵʱָ��
	}
// 	else
// 	{
// 		g_tInBuf.BufFullFlag = COM_IN_BUF_FULL;			//����������־
// 	}
    
        
	if ((g_tRealTimeCmd.RealTimeEnable == 1) && \
			(g_tRealTimeCmd.RealTimeFlag == 1))
	{
		RealTimeCommand();	//ʵʱָ��
	}
}

/*******************************************************************************
* Function Name  : void ReadInBuffer(void)
* Description    : �����뻺������ȡһ�ڣ������������û�����ݣ���ȴ����Զ��������ߡ�
* Input          : None
* Output         : 
* Return         : 
*******************************************************************************/
#define	DRV_PRT_COM_MIN_DELAY_MS		(1)
#define	DRV_PRT_MIN_DELAY_MS				(1)

uint8_t	ReadInBuffer(void)
{
	#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
		static uint8_t DriverFullFlag = 0;
	#endif
	uint32_t Temp;
	static uint8_t ReadWaitFlag = 0;			//2016.05.23  
	uint8_t	GetChar;
	uint16_t i;
	uint8_t Bps;
	
	if (g_tMacro.RunFlag)		//ִ�к�ʱ�Ӻ껺����ȡ��
	{
		if (g_tMacro.GetPointer < g_tMacro.Number)
		{
			GetChar = g_tMacro.Buffer[g_tMacro.GetPointer];
			g_tMacro.GetPointer++;
		}
		else
		{
			GetChar = 0;
		}
		return	(GetChar);
	}
	
	while (1)
	{
		if (g_tInBuf.PutPosition == g_tInBuf.GetPosition)	//û������ 2016.07.07
  	{
			#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
				DriverFullFlag = 0;
			#endif
			CheckFeedButton();
			if(g_tInterface.ETH_Type) 
			{	
				GetETH_ConnectState();
			}	
			if (g_tSystemCtrl.SleepCounter > SLEEP_TIME_MS)	//���߶�ʱʱ�䵽
			{
				g_tSystemCtrl.SleepCounter = 0;
				#ifndef DEBUG
//					EnterLowPower();	//��������
				#endif
			}
			ReadWaitFlag = 0;	//2016.05.23			
			if(NetReloadCounter>65000)           //2018.12.10 ��ʱ1���� ��λ����
			{																		 
				ETH_ConnectState=0;
				if(g_tInterface.ETH_Type) 
				{
					uIPMain();    //��ʼ������
        }	               
				NetReloadCounter = 0;
			}	
			//��������
			if(g_tSpeaker.SpeakWriteHandle == WRITE_END)
				return 0xFF;
		}			//�����������������
		else	//������������
		{
			//#ifdef	DRIVE_PRT_MODE_RX_DELAY_ENABLE
			#if	0
			if ((g_tSystemCtrl.PrtSourceIndex == COM) && \
					(g_tPrtCtrlData.PCDriverMode != DRV_PRT_TYPE_NONE) && \
					(DriverFullFlag == 0))	//����ģʽ�һ���������
			{
				while (1)
				{	//�ȴ��������������ʱ
					if ((g_tInBuf.BufFullFlag == COM_IN_BUF_FULL) || \
							(g_tSystemCtrl.SleepCounter > DRV_PRT_MIN_DELAY_MS) || \
							(g_tInBuf.BytesNumber > 576)
						)
					{
						DriverFullFlag = 1;
						break;
					}
				}
			}
			#endif
			
			if(ReadWaitFlag ==0 )	
			{
				MaxSetp();									//�ӿ����ͳ�ʼ�������ٲ��� 2016.06.04
				ReadWaitFlag = 1;
				
				//�������ڻ�����û��ʱ����ʼ��������ʱ���ȴ�2.0��
				if(g_tSystemCtrl.PrtSourceIndex == COM ) //2016.06.08 COM 
				{
// 					if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)			//����ģʽ
					{
						//�ȴ������û�����ݣ����˳��ȴ�
						for(i=0; i<3000; i++)					
						{
							DelayMs(1);					//��ʱ��׼ ���ʱ�� 
							if(g_tSystemCtrl.SleepCounter >30)	//30msû���յ����˳�  2016.05.23
								break;
						}
					}	
				}
			}
			
			g_tSystemCtrl.SleepCounter = 0;								//��λ���߼�����
						
			GetChar = g_tInBuf.Buf[g_tInBuf.GetPosition];	//ȡ��
			
 			DisableReceiveInterrupt();										//�ؽ����ж�  2016.07.28 ֻ���Ŷ�usb�Ĳ���
			g_tInBuf.GetPosition++;												//����ȡ��ָ��
			if (g_tInBuf.GetPosition >= g_tInBuf.BytesSize)
			{
				g_tInBuf.GetPosition = 0;										//�������ֵʱ���´�0��ʼ
			}
// 			
// // 	g_tInBuf.BytesNumber--;												//�޸������ֽ���
// 			
 			EnableReceiveInterrupt();											//�������ж�
			
			#ifdef	DBG_RCV_DATA_LOSE
				g_uiTotalReadByte++;
			#endif
			
			if (g_tInBuf.BufFullFlag == COM_IN_BUF_FULL) //2016.07.08
			{				
// 				if (g_tSysConfig.PaperOutReprint)						//2016.10.09 ȱֽ�ش�ӡ���
// 				{
// 					Temp = g_tInBuf.PutPosition;	//�ж��п��ܸı���ֵ
// 				  if(Temp >= g_tInBuf.PE_GetPosition)
// 						Temp =(g_tInBuf.BytesSize - Temp) + g_tInBuf.PE_GetPosition;
// 					else
// 						Temp =g_tInBuf.PE_GetPosition - Temp;
// 				}		
// 				else		//�������
				{		
					Temp = g_tInBuf.PutPosition;	//�ж��п��ܸı���ֵ
				  if(Temp >= g_tInBuf.GetPosition)
						Temp =(g_tInBuf.BytesSize - Temp) + g_tInBuf.GetPosition;
					else
						Temp =g_tInBuf.GetPosition - Temp;
				}		
			
				if( Temp > MAX_RSV_BYTES_OF_IN_BUF)	
				{										  
					g_tInBuf.BufFullFlag = COM_IN_BUF_NOT_FULL;	//����������
					ClearComInBufBusy();					
				}
			}
			
			if ((g_tMacro.DefinitionFlag == 1) && (g_tMacro.Number < MACRO_SIZE))	//��ִ�к궨��
		 	{
				g_tMacro.Buffer[g_tMacro.Number] = GetChar;	//�����ݴ���껺����
				g_tMacro.Number++;
			}
			g_tSpeaker.SpeakWriteTime = 0;  //����flash������
			return (GetChar);
		}		//�����������������
	}		//�����������
}

/*******************************************************************************
* Function Name  : DecInBufferGetPoint
* Description    : ���뻺����ȡ��ָ���1��Ŀ����Ϊ�˽�֮ǰ��ȡ���������·Ż�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DecInBufferGetPoint(void)
{
#if	1
// 	if (g_tMacro.RunFlag == 0)				//���Ǻ�ִ�ж��� 2016.07.07
// 	{
// 		if (g_tInBuf.GetPosition == 0)
// 		{
// 			g_tInBuf.GetPosition = (g_tInBuf.BytesSize - 1);
// 		}
// 		else
// 		{
// 			g_tInBuf.GetPosition--;
// 		}

// 		DisableReceiveInterrupt();			//�ؽ����ж�
// 		g_tInBuf.BytesNumber++;			  	//�޸������ֽ���
// 		EnableReceiveInterrupt();				//�������ж�
// 		#ifdef	DBG_RCV_DATA_LOSE
// 			g_uiTotalReadByte--;
// 		#endif
// 		
// 		if (g_tInBuf.BufFullFlag == COM_IN_BUF_NOT_FULL)
// 		{
// 			ClearComInBufBusy();
// 		}
// 	}
// 	
// 	if (g_tMacro.DefinitionFlag == 1)	//�궨����
// 	{
// 		g_tMacro.Number--;							//��ȥ����껺�����
// 	}
#endif
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
