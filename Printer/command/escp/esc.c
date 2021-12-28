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
#include	"esc.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"feedpaper.h"
#include	"print.h"
#include	"charmaskbuf.h"
#include	"timconfig.h"
#include	"init.h"
#include	"cutter.h"
#include	"defconstant.h"
#include	"drawer.h"

extern	uint8_t	g_bLineEditBufStatus;
extern  TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

extern	uint32_t	Pdf417_Print(uint8_t * pBuf, uint8_t Width, uint8_t CorID, uint16_t CharLen);
extern	uint32_t	Matrix_Print(uint8_t * pBuf, uint8_t vHeight, uint8_t vWidth, uint16_t charlen);
extern	uint32_t	QR_Print(uint8_t * pBuf, uint8_t Version, uint8_t CorID, uint16_t CharLen);

extern	void ClearInBuf(void);

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
* Function Name  : Command_1B0C
* Description    : ҳģʽ�´�ӡ����(ESC FF )�Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B0C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	
	{
		
	}
}

/*******************************************************************************
* Function Name  : Command_1B13
* Description    : ��ӡλͼ,�߶�8��,��Ȳ����������ɴ�ӡ��ȵĺڰ�λͼ,
*	���ַ��Ŵ�����,�����ӡ����Ӱ��
*	������:��������ֽڳ���;�������λͼ����;���ݷŴ�ָ�����������λͼ�������
*	����ӡ������;�����ʱ���ӡ��ǰ��;ֱ��������Ϊֹ
*	2012-10-7���
*	1B   4B   nL nH d1��dk 
*	0 �� nL �� 255 
* 0 �� nH �� 255
* 0 �� d �� 255
*	������ֻ�ܴ�ӡ�߶�Ϊ8��,��Ȳ������ɴ�ӡ����ĺڰ�λͼ.
* nL nH�ֱ�Ϊ�޷�����˫�ֽ�����N�ĵ�λ�͸�λ�ֽ�.��ʾˮƽ������λͼ�еĵ���.
* ��ӡ(nL + nH * 256) * 8����ͼ��.��ͼ��Ŀ��Ϊ(nL + nH * 256)��,�߶�Ϊ8��.
* ÿһ�е�8���������һ��8λ���ֽ�����ʾ,���λ����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1B13(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Line;
	uint8_t	Width;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint16_t	iMaxNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	GetChar = ReadInBuffer();
	iMaxNumber = GetChar + ReadInBuffer() * 256;	//���ݳ���
	//iMaxNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//���ݳ���
	if (iMaxNumber == 0)	//��������ֽ���Ϊ0ֱ���˳�
		return;
	
	//�ڱ༭�д�ӡ������֮ǰ��Ҫ�ȼ�����Ƿ����,��������ȴ���ӡ,����Ҫ�ȴ��д�ӡ��������ӡ����,
	//Ȼ�󽫱༭�д�ӡ�����������ݸ��Ƶ���ӡ�д�ӡ������,Ȼ��������ӡ,��ձ༭������������,�ٿ�ʼ�༭.
	//����༭�д�ӡ����,�༭��������,Ҳ��Ҫ�������ϴ���
	//if (g_bLineEditBufStatus == LEB_STATUS_WAIT_TO_PRINT)	//�༭�д�ӡ�������ȴ���ӡ
	{
	//	PrintOneLine();		//�ȴ�ӡ�ñ༭������
	}
	//WaitPreviousPrintFinish();
	
	if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))
	{
		RatioX = 2;
	}
	else
	{
		RatioX = g_tPrtModeChar.WidthMagnification;	//ȷ������Ŵ���,�������ڱ���
	}
	if (RatioX > MAX_RATIO_X)
		RatioX = MAX_RATIO_X;
	
	RatioY = g_tPrtModeChar.HeightMagnification;	//ȷ������Ŵ���
	if (RatioY > MAX_RATIO_Y)
		RatioY = MAX_RATIO_Y;
	
	RevNumber = iMaxNumber;
	if ((RevNumber * RatioX) > g_tPrtCtrlData.PrtLength)	//ȷ������������ֽڳ���
	{
		RevNumber = g_tPrtCtrlData.PrtLength / RatioX;
	}
	
	//ָ���ʼ�����������Ʊ�����ʼ��
	ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
	for (i = 0; i < RevNumber; i++)		//�������λͼ����
	{
		//����һ��������д�ӡ������
		if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
		{
			GetChar = ReadInBuffer();		//����һ��λͼ����
			if (GetChar)
			{
				if (RatioY > 1)
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, GetChar, TempBuf);		//����Ŵ�
					//�����ӡ������,��λͼ�Ŵ��ĸ�λMSB��ʼ����,��Ϊÿ�δ���һ��λͼ�ֽ�,
					//�������ʱ�����ֽ����ڸ߶���=RatioY,�ڿ����=RatioX
					//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Line = 0; Line < RatioY; Line++)
					{	//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
						p = pstart;
						for (Width = 0; Width < RatioX; Width++)	//����Ŵ���
						{
							*(p + Width) = TempBuf[Line];	//ˮƽ���������
						}
						pstart += PRT_DOT_NUMBERS;
					}		//һ��λͼ�ֽں�������������
				}
				else
				{
					TempBuf[0] = GetChar;
					//�����ӡ������,��λͼ�Ŵ��ĸ�λMSB��ʼ����,��Ϊÿ�δ���һ��λͼ�ֽ�,
					//�������ʱ�����ֽ����ڸ߶���=RatioY,�ڿ����=RatioX
					//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Width = 0; Width < RatioX; Width++)	//����Ŵ���
					{
						*(pstart + Width) = TempBuf[0];	//ˮƽ���������
					}
				}			//һ��λͼ�ֽں�������������
			}
			
			g_tPrtCtrlData.PrtDataDotIndex += RatioX;
		}
		else
		{
			break;					//��������ȫ������,���ٴ�ӡ
		}
	}		//End λͼ���
	
	//��鱾�α༭�������Ƿ��Ѿ���ӡ��, ���û��ӡ��������ӡ
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
		g_bLineEditBufStatus = LEB_STATUS_WAIT_TO_PRINT;
		g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_QT;	//������Ϊ����ģʽʵ��
		g_tPrtCtrlData.BitImageFlag = 1; 		//λͼ��ӡ��־
// 		if (g_tPrtCtrlData.MaxPrtDotHigh < (uint16_t)((uint16_t)RatioY << 3))
// 			g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//��ӡ�и�
		if (g_tPrtCtrlData.MaxPrtDotHigh < (RatioY * 8))
			g_tPrtCtrlData.MaxPrtDotHigh = (RatioY * 8);	//��ӡ�и�
// 		PrintOneLine();	//��ӡ�ñ༭������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	for (; i < iMaxNumber; i++)
		ReadInBuffer();		//���ն����λͼ����
	
	p = NULL;
	pstart = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B20
* Description    : �����ַ��Ҽ��,
*	�����ַ����Ҽ��Ϊ[n�� �����ƶ���λ�������ƶ���λ],��ǰ����λ����mm����
*	���ַ��Ŵ�ʱ,�����֮�Ŵ���ͬ�ı���
*	��ָ�Ӱ�캺�ֵ�����
*	��ָ�����õ�ֵ��ҳģʽ�ͱ�׼ģʽ�����໥������
*	����������ƶ���λ��GS Pָ��.�ı����������ƶ���λ���ı䵱ǰ�Ҽ��
*	��׼ģʽ��,ʹ�ú����ƶ���λ
*	��ҳģʽ��,��������ķ������ʼλ����ѡ��ʹ�ú����ƶ���λ�������ƶ���λ,
*	��ѡ��ʽ����:
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�ú����ƶ���λ��
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�������ƶ���λ��
* ����Ҽ����255/203Ӣ��.�κγ������ֵ�����ö��Զ�ת��Ϊ����Ҽ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B20(void)
{
	uint8_t	GetChar;
	uint32_t	nRightSpace;
	
	GetChar = ReadInBuffer();
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		nRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		if (nRightSpace > MAX_RIGHT_SPACE_HORIZONTAL)	//���,���ֵ����
		{
			nRightSpace = MAX_RIGHT_SPACE_HORIZONTAL;
		}
		g_tPrtModeChar.RightSpace = nRightSpace;
	}
	else
	{
		g_tPageMode.AscRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;	//ҳģʽ
	}
}

/*******************************************************************************
* Function Name  : Command_1B21
* Description    : �����ַ���ӡģʽ,�����ַ�����,����,����,�Ӵ�,�»��ߵ�
* λ	1/0  	HEX	Decimal		���� 
*	0		0			00		0				��׼ASCII����A (12 �� 24)
*	0		1			01		1				ѹ��ASCII����B (9 ��   17) 
*	1,2											δ����
*	3		0			00		0				ȡ���Ӵ�ģʽ
*	3		1			08		8				ѡ��Ӵ�ģʽ
*	4		0			00		0				ȡ������ģʽ
*	4		1			10		16			ѡ�񱶸�ģʽ
*	5		0			00		0				ȡ������ģʽ
*	5		1			20		32			ѡ�񱶿�ģʽ
*	6												δ����
*	7		0			00		0				ȡ���»���ģʽ
*	7		1			80		128			ѡ���»���ģʽ
*	����HT���õĿո��˳ʱ����ת90����ַ�,�����κ��ַ������Լ��»���
*	�»��߿����  ESC -  ȷ��,���ַ��޹�
*	��һ���в����ַ�Ϊ���߻����,�����ַ��Ե׶˶���
*	ESC EҲ��ѡ���ȡ���Ӵ�ģʽ,���ִ�е�ָ����Ч
*	ESC �CҲ��ѡ���ȡ���»���ģʽ,���ִ�е�ָ����Ч
*	GS ! Ҳ�������ַ���С,���ִ�е�ָ����Ч
*	�Ӵ�ģʽ���ַ��ͺ��ֶ���Ч,���Ӵ�ģʽ��,����ģʽֻ���ַ���Ч
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B21(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar & 0x01))	//ʹ��9 x 17 ��ASCII�ַ�(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_B;
		g_tPrtModeChar.CharWidth = ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh  = ASCII_FONT_B_HEIGHT;
	}
	else									//ʹ��12 x 24 ��ASCII�ַ�(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_A;
		g_tPrtModeChar.CharWidth = ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh  = ASCII_FONT_A_HEIGHT;
	}
	
	if ((GetChar & 0x08))	//ѡ���ַ��Ӵ�ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tPrtModeChar.EmphaSizeMode = 1;
		g_tPrtModeHZ.EmphaSizeMode = 1;
	}
	else									//���Ӵ�
	{
		g_tPrtModeChar.EmphaSizeMode = 0;
		g_tPrtModeHZ.EmphaSizeMode = 0;
	}
	
	if ((GetChar & 0x10))	//ѡ���ַ�����ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tPrtModeChar.HeightMagnification = 2;
	}
	else									//������
	{
		g_tPrtModeChar.HeightMagnification = 1;
	}
	
	if ((GetChar & 0x20))	//ѡ���ַ�����ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tPrtModeChar.WidthMagnification = 2;
	}
	else									//������
	{
		g_tPrtModeChar.WidthMagnification = 1;
	}
		
	if ((GetChar & 0x80))	//ѡ���ַ��»���ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_MASK;
	}
	else									//ȡ���»��߹���
	{
		g_tPrtModeChar.Underline &= UNDERLINE_DISABLE_MASK;
	}
}

/*******************************************************************************
* Function Name  : Command_1B24
* Description    : ���þ��Դ�ӡλ��,����ģʽ��,ֻ��1�ֽڲ���,���ֽڱ�ʾλ��
*	����ǰλ�����õ���������(nL + nH��256 )�� (����������ƶ���λ)��
*	�������λ����ָ����ӡ������,���������
*	����������ƶ���λ��GS P ����
*	��׼ģʽ��ʹ�ú����ƶ���λ
*	��ҳģʽ��,���ݴ�ӡ����ķ���ʹ�ӡ��ʼλ����ѡ��ʹ�ú����ƶ���λ������
*	�ƶ���λ,��ѡ��ʽ����: 
*	�� ����ӡ��ʼλ����ESC T����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�ú����ƶ���λ�� 
*	�� ����ӡ��ʼλ����ESC T����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�������ƶ���λ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B24(void)
{
	uint8_t	GetChar;
	uint16_t	temp;
	
	if (g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_IP)		//IP����������
	{
		g_tPrtCtrlData.DriverOffset = ReadInBuffer();
	}
	else
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//��׼ģʽ��
		{
			GetChar = ReadInBuffer();
			//temp = (ReadInBuffer() * 256 + GetChar) * g_tPrtCtrlData.StepHoriUnit;
			temp = ((uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8)) * g_tPrtCtrlData.StepHoriUnit;
			if ((temp >= g_tPrtCtrlData.PrtLeftLimit) && \
					(temp <= (g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtLength)) && \
					(temp < PRT_DOT_NUMBERS))       //2016.05.08  С�ڵ���576��ΪС��
			{
				g_tPrtCtrlData.PrtDataDotIndex = temp;    //2017.04.24
			}
			
		}
		else		//���,ҳģʽ���д�����
		{
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B25
* Description    : ѡ��/ȡ���û��Զ����ַ�,ֻ�����λD0��Ч
*	��n�����λΪ0ʱ,ȡ���û��Զ����ַ�
*	��n�����λΪ1ʱ,ѡ���û��Զ����ַ�
*	��ȡ��ʹ���û��Զ����ַ���ʱ��,�Զ�ʹ���ڲ��ֿ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B25(void)
{
	if (ReadInBuffer() & 0x01)
	{
		g_tSystemCtrl.UserSelectFlag = 1;
	}
	else
	{
		g_tSystemCtrl.UserSelectFlag = 0;
	}
}

/*******************************************************************************
* Function Name  : Command_1B26	 1B 26 3  c1 c2 [x1 d1...d(y �� x1)]...[xk d1...d(y �� xk)]
* Description    : �����û��Զ����ַ�,����ǰ���õ���������Զ�ѡ���ַ����12*24���9*17��
*	32 ��   c1 ��   c2 ��   126 
*	0  ��   x  ��   12	��׼ASCII����A(12��24) 
*	0  ��   x  ��   9		ѹ��ASCII����B(9��17)
*	0  ��   d1 ... d(y ��   xk)  ��   255
*	K = c2 - c2 + 1 
*	yָ�������ֽ���, xָ���������,c1����ʼ�ַ�����,c2����ֹ�ַ�����
* ������ַ����뷶Χ��ASCII��� <20>H  �� <7E>H (95���ַ�).
*	���������������ַ�,���ֻ����һ���ַ�c1 = c2
*	d�������ַ�������.��������ݴ���߿�ʼ
*	�Զ����ַ��Ĵ�С�� (y  ��   x) �ֽ�
*	���ݵĸ���λΪ1��ʾ��ӡ�����,Ϊ0��ʾ����ӡ
*	���������,�û��Զ����ַ������: 
*	��  ESC @  ��ִ�� 
*	��  ESC ? ��ִ��. 
*	��  ��ӡ����λ���Դ�ر�. 
*	���Զ����ַ�Ϊ����B(9 ��17)ʱ,�����3�ֽ�����ֻ�����λ��Ч
*	������ESC :ָ��ֿ�������ȫ�����Ƶ�RAM,Ȼ����ִ�б�ָ��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B26(void)
{
	uint8_t	i, j;
	uint8_t	CodeStart, CodeStop;
	uint8_t	CharWidth, Number;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	if (ReadInBuffer() != 3)		//�߶Ȳ�����3,Ϊ��Ч����
		return;
	
	CodeStart = ReadInBuffer();		//�����ַ�����ʼ�ͽ���
	CodeStop = ReadInBuffer();
	if ((CodeStop < CodeStart) || \
			(CodeStart < 0x20) || (CodeStart > 0x7E) || \
			(CodeStop < 0x20) || (CodeStop > 0x7E) || \
			((CodeStop - CodeStart + 1) > USER_ZF_SIZE)
			)	//������Ч
		return;		//c2>=c1, 0x20=< c <=0x7e
	
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)  //12*24�����ַ�
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12��24�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12��24�û������ַ���ģ��
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)  //9*17�����ַ�
	{
		CharWidth = ASCII_FONT_B_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_09;	//9��17�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_09;		//9*17�û������ַ���ģ��
	}
	else
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12��24�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12��24�û������ַ���ģ��
	}
	
	pZimo += ((CodeStart - 0x20) * 3 * CharWidth);	//ָ��˴ζ����ַ��洢��ģ��ʼ��ַ
	for (i = CodeStart; i <= CodeStop; i++)
	{
		pCode[i - 0x20] = i;			//�Զ����ַ�����,i��С��0x20��ʼ
		Number = ReadInBuffer();	//ָ���������
		if (Number > CharWidth)		//������Ч�˳�
			return;
		
		for (j = 0; j < (Number * 3); j++)	//�ַ����������Numberָ��,����CharWidth�Ĳ���0����
		{
			*pZimo++ = ReadInBuffer();	//�洢��ģֵ
		}
		for (; j < (CharWidth * 3); j++)
		{
			*pZimo++ = 0x00;						//����Ĳ���
		}	//End �ַ���,����Ϊ�޸ĳ���
	}	// End ������
	
	g_tRamBuffer.UserCharStruct.UserdefineFlag |= 0x01;	//�������Զ����ַ�
	
	pCode = NULL;
	pZimo = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B2A
* Description    : ��ѡ���λͼģʽ,��λͼ���ݴ����ӡ������
*	1B    2A  m   nL  nH  d1...dk
*	m = 0, 1, 32, 33
*	0 �� nL �� 255
*	0 �� nH �� 7
*	0 �� d  �� 255
* 1  ��  ( nL + nH ��  256)  ��  2047 (0 ��  nL ��  255, 0 ��  nH ��   7)
*	ѡ����mָ����һ��λͼģʽ,��λͼ���ݱ��浽��ӡ��������,λͼ���ݸ�����nL��nHȷ��:
*	m  ģʽ 					����  					����
*								����  �ֱ���  �ֱ���  	���ݸ���(k)
*	0  8�㵥�ܶ�  	8 	68 DPI  101 DPI   nL + nH �� 256
*	1  8��˫�ܶ�  	8  	68 DPI  203 DPI  	nL + nH �� 256
*	32 24�㵥�ܶ�  24  	203 DPI 101 DPI   (nL + nH �� 256) �� 3
*	33 24��˫�ܶ�  24  	203 DPI 203 DPI  	(nL + nH �� 256) �� 3
*	���m ��ֵ�����涨��Χ,nL  ���������ݱ���Ϊ��ͨ���ݴ���
*	�����ӡ������nL��nH����,�ܵĵ���Ϊ nL + nH �� 256
*	λͼ������ǰ����Ĳ��ֱ��ص�
*	d ��λͼ������.ÿ���ֽڵ���ӦλΪ1��ʾ��ӡ�õ�,Ϊ0����ӡ�õ�.
*	λͼ���ݷ�����ɺ�,��ӡ��������ͨ���ݴ���ģʽ
*	���˵���ģʽ,����ָ���������ӡģʽӰ��(�Ӵ�,˫�ش�ӡ,�»���,�ַ��Ŵ�ͷ���)
*	����ָ���������ӡģʽӰ�� (�Ӵ�,˫�ش�ӡ,�»���,�ַ��Ŵ�ͷ���).
* ����߶���ֻ��1�ֽڻ���3�ֽ�,�߶ȹ̶�.
* ���򳤶��ϲ��̶�,�п��ܳ�������ӡ����.����Ǳ���ģʽ,�п�����չΪ����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B2A(void)
{
	uint8_t	GetChar;
	uint8_t	BmpMode;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Width;
	uint8_t	Line;
	uint8_t	LEBFullFlag;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	BmpMode = ReadInBuffer();	
	if ((BmpMode == 0) || (BmpMode == 1) || (BmpMode == 32) || (BmpMode == 33))
	{
		if ((BmpMode == 0) || (BmpMode == 32))
		{
			RatioX = 2;					   //�����ܶ�
		}
		else
		{
			RatioX = 1;					   //����˫�ܶ�
		}
		if ((BmpMode == 0) || (BmpMode == 1))
		{
			RatioY = 3;					   //���ֽ�8��Ŵ�Ϊ24��
		}
		else
		{
			RatioY = 1;					   //���ֽ�24��
		}
		GetChar = ReadInBuffer();
		RevNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//ͼ�κ������
		if (RevNumber > g_tPrtCtrlData.PrtLength)
		{
			RevNumber = g_tPrtCtrlData.PrtLength;
		}
		if (RevNumber > PRT_DOT_NUMBERS)	//ȷ������������ֽڳ���
		{
			RevNumber = PRT_DOT_NUMBERS;		//ʣ��������Ϊ��ͨ���ݴ���
		}
		
		if ((BmpMode == 32) || (BmpMode == 33))
		{
			RevNumber *= 3;			//24����ͼ����������8����ͼ�δ�3��
		}
	}		//ģʽ������ȷ�������
	else
	{
		return;	//ģʽ��������,������
	}
	
	LEBFullFlag = 0;	//�ȼٶ�������
	if ((BmpMode == 0) || (BmpMode == 1))		//���´���8��ߵ����
	{
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - 3) * PRT_DOT_NUMBERS;
		for (i = 0; i < RevNumber; i++)		//�������λͼ����
		{	//����һ��������д�ӡ������
			if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
			{
				GetChar = ReadInBuffer();		//����һ��λͼ����
				if (GetChar)
				{
					//�����ӡ������,��λͼ�Ŵ��ĸ�λMSB��ʼ����,��Ϊÿ�δ���һ��λͼ�ֽ�,
					//�������ʱ�����ֽ����ڸ߶���=RatioY=1,�ڿ����=RatioX
					//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					//����Ŵ�����
					ByteZoomCtrl(RatioY, GetChar, TempBuf);	 //����Ŵ���
					for (Line = 0; Line < 3; Line++)
					{	//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
						p = pstart + Line * PRT_DOT_NUMBERS;
						for (Width = 0; Width < RatioX; Width++)	//����Ŵ���
						{
							*(p + Width) = TempBuf[Line];	//ˮƽ���������
						}		//һ��λͼ�ֽں���������
					}
				}
				g_tPrtCtrlData.PrtDataDotIndex += RatioX;
			}		//һ��λͼ�ֽں���������
			else
			{
				LEBFullFlag = 1;
				break;		//�������һ����ֹͣ�༭
			}
		}
		for (; i < RevNumber; i++)	//�������λͼ����
			ReadInBuffer();		//������������ȫ������
		
		//�����ô�ӡ���Ʊ���
		g_tPrtCtrlData.BitImageFlag = 1; 	//λͼ��ӡ��־
		if (g_tPrtCtrlData.MaxPrtDotHigh < 24)
		{
			g_tPrtCtrlData.MaxPrtDotHigh = 24;
		}
		if (LEBFullFlag)		//����༭��������һ�����ӡ
		{
      //��ӡ�ñ༭������
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
	}			//8��ߴ������
	else if ((BmpMode == 32) || (BmpMode == 33))	//���´���24��ߵ����
	{
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - 3) * PRT_DOT_NUMBERS;
		for (i = 0; i < RevNumber; i += 3)	//�������λͼ����,һ��ѭ��Ҫ����3�ֽ�
		{	//����һ��������д�ӡ������
			if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) <= g_tPrtCtrlData.PrtLength)
			{
				memset(TempBuf, 0x00, MAX_RATIO_Y);
				//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				TempBuf[0] = ReadInBuffer();		//����һ��λͼ����
				TempBuf[1] = ReadInBuffer();		//����һ��λͼ����
				TempBuf[2] = ReadInBuffer();		//����һ��λͼ����
				if (TempBuf[0] || TempBuf[1] || TempBuf[2])	//ͼ��ֵ��Ϊ0
				{
					//�����ӡ������,��λͼ�Ŵ��ĸ�λMSB��ʼ����,��Ϊÿ�δ���һ��λͼ�ֽ�,
					//�������ʱ�����ֽ����ڸ߶���=RatioY=1,�ڿ����=RatioX
					//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
					for (Line = 0; Line < 3; Line++)
					{	//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
						p = pstart + Line * PRT_DOT_NUMBERS;
						for (Width = 0; Width < RatioX; Width++)	//����Ŵ���
						{
							*(p + Width) = TempBuf[Line];	//ˮƽ���������
						}		//һ��λͼ�ֽں���������
					}
				}
				g_tPrtCtrlData.PrtDataDotIndex += RatioX;
			}
			else
			{
				LEBFullFlag = 1;
				break;		//�������һ����ֹͣ�༭
			}
		}		//End λͼ���
		for (; i < RevNumber; i++)	//�������λͼ����
			ReadInBuffer();		//������������ȫ������
		
		//�����ô�ӡ���Ʊ���
		g_tPrtCtrlData.BitImageFlag = 1; 	//λͼ��ӡ��־
		if (g_tPrtCtrlData.MaxPrtDotHigh < 24)
		{
			g_tPrtCtrlData.MaxPrtDotHigh = 24;
		}
		if (LEBFullFlag)		//����༭��������һ�����ӡ
		{
      //��ӡ�ñ༭������
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		}
	}			//24��ߴ������
	
	p = NULL;
	pstart = NULL;
	ptemp = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B2D
* Description    : ����/ȡ���ַ��»���
*	1B   2D   n
*	0  ��   n  �� 2, 48 �� n  ��   50 
*		n						���� 
*	0, 48			ȡ���»���ģʽ 
*	1, 49			ѡ���»���ģʽ(1 ���)  
*	2, 50			ѡ���»���ģʽ(2 ���)  
*	�»��߿ɼ��������ַ���(�����Ҽ��),��������HT���õĿո�
*	�»��߲���������˳ʱ����ת90�� �ͷ��Ե��ַ���
*	��ȡ���»���ģʽʱ,������ַ������»���,�»��ߵĿ�Ȳ��ı�.Ĭ�Ͽ����һ���
*	�ı��ַ���С��Ӱ�쵱ǰ�»��߿��
*	�»���ѡ��ȡ��Ҳ������  ESC ! ������.���ִ�е�ָ����Ч
*	��ָ�Ӱ�캺�ֵ�����
*	Ĭ��n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B2D(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)	   	//ȡ���»���
		g_tPrtModeChar.Underline &= UNDERLINE_DISABLE_MASK;
	else if (GetChar == 1 || GetChar == 49)	//����1��ģʽ
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_ONE_DOT_MASK;
	else if (GetChar == 2 || GetChar == 50)	//����2��ģʽ
		g_tPrtModeChar.Underline |= UNDERLINE_ENABLE_TWO_DOT_MASK;
}

/*******************************************************************************
* Function Name  : Command_1B31
* Description    : �����м��Ϊn����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B31(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
		g_tPrtCtrlData.CodeLineHigh = ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B32
* Description    : ����Ĭ���и�, ѡ��32��(4mm,Լ1/6Ӣ��)�и�,��ģʽ��ҳģʽ�ֿ�
*	�и��ڱ�׼ģʽ��ҳģʽ���Ƕ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B32(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
		g_tPrtCtrlData.CodeLineHigh = DEFAULT_CODE_LINE_HEIGHT;
	else
		g_tPageMode.PageLineHigh = DEFAULT_CODE_LINE_HEIGHT;
}

/*******************************************************************************
* Function Name  : Command_1B33n
* Description    : ������,��ģʽ��ҳģʽ�ֿ�
*	1B   33   n 
*	�����и�Ϊ  [ n �� ���������ƶ���λ]
*	�и������ڱ�׼ģʽ��ҳģʽ�����໥������
*	����������ƶ���λ��  GS P  ����,�ı�������ò�Ӱ�쵱ǰ�и�
*	��׼ģʽ��,ʹ�������ƶ���λ
*	��ҳģʽ��,���ݴ�ӡ����ķ���ʹ�ӡ��ʼλ����ѡ��ʹ�ú����ƶ���λ�������ƶ���λ,
*	��ѡ��ʽ����: 
*	�� ����ӡ��ʼλ����ESC T����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�������ƶ���λ�� 
*	�� ����ӡ��ʼλ����ESC T����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�ú����ƶ���λ�� 
* �����ֽ������1016 mm (40 Ӣ��).  ��������������,ȡ������
*	Ĭ���и�Ϊ 4mm (Լ1/6  Ӣ��)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B33(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//��׼ģʽ
	{
    g_tPrtCtrlData.CodeLineHigh = GetChar * g_tPrtCtrlData.StepVerUnit;				 //2017.02.09 �ͻ����ǲ���Ϊ�и�
  //g_tPrtCtrlData.CodeLineHigh = (GetChar /(2*g_tPrtCtrlData.StepVerUnit));   //���ݰ��������м��
		if (g_tPrtCtrlData.CodeLineHigh > MAX_LINE_SPACE_VERTICAL)	//���40��,1016mm,���ܳ�����ֵ
			g_tPrtCtrlData.CodeLineHigh = MAX_LINE_SPACE_VERTICAL;
	}
	else	//ҳģʽ
	{
		if ((g_tPageMode.PageFeedDir == 0) || (g_tPageMode.PageFeedDir == 2))
		{
			g_tPageMode.PageLineHigh = GetChar * g_tPrtCtrlData.StepHoriUnit;
			if (g_tPageMode.PageLineHigh > MAX_LINE_SPACE_HORIZONTAL)
			{
				g_tPageMode.PageLineHigh = MAX_LINE_SPACE_HORIZONTAL;
			}
		}
		else
		{
			g_tPageMode.PageLineHigh = GetChar * g_tPrtCtrlData.StepVerUnit;
			if (g_tPageMode.PageLineHigh > MAX_LINE_SPACE_VERTICAL)			//���
			{	
				g_tPageMode.PageLineHigh = MAX_LINE_SPACE_VERTICAL;				//
			}
		}
		g_tPageMode.PageCurrentLineHigh = g_tPageMode.PageLineHigh;
	}
}

/*******************************************************************************
* Function Name  : Command_1B3D
* Description    : ѡ���ӡ��
*	1B   3D   n 
*	1��   n  ��   3
*	ѡ���ӡ��,��ѡ��Ĵ�ӡ�����Խ�������������͵�����
*	Hex  Decimal	����
*	1,3		1,3  		��ӡ������
*	2			2  			��ӡ����ֹ
*	����ӡ������ֹʱ,��ӡ�����Գ�ʵʱָ��(DLE EOT, DLE ENQ, DLE DC4 )�������
*	����ָ��,ֱ���ø�ָ������Ϊֹ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//���µĳ�������ǰ���EPSON T88IV��,n=1,3����;n=2��ֹ��ӡ��
void Command_1B3D(void)
{
	if (g_tSystemCtrl.PrtDeviceDisable)	   //��ӡ���ѽ�ֹ
	{
		while (g_tSystemCtrl.PrtDeviceDisable)
		{
			if (ReadInBuffer() == 0x1B)
			{
				if (ReadInBuffer() == 0x3D)
				{
					switch (ReadInBuffer())
					{
						case	1:
						case	3:
						case	'1':
						case	'3':
							g_tSystemCtrl.PrtDeviceDisable = 0;		//�����ӡ��
							break;
						default:
							break;
					}
				}
			}
		}
	}
	else	//��ӡ��ԭ��������
	{
		switch (ReadInBuffer())
		{
			case	2:
			case	'2':
				g_tSystemCtrl.PrtDeviceDisable = 1;		//��ֹ��ӡ��
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B3F
* Description    : ȡ���û��Զ����ַ�
*	1B   3F   n
*	32 �� n  �� 126
*	ȡ���û��Զ����ַ�
*	ȡ���û��Զ����ַ��д���Ϊn���ַ�.ȡ����,���ַ�ʹ���ڲ��ֿ�
*	��ָ��ɾ����ESC ����ѡ������弯��ָ���������ģ
*	����Զ����ַ���û�и��ַ�,��ָ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B3F(void)
{
	uint8_t	GetChar;
	uint8_t	CharWidth;
	uint8_t	i;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)	//12*24�����ַ�
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12��24�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12��24�û������ַ������
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)	//9*17�����ַ�
	{
		CharWidth = ASCII_FONT_B_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_09;	//9��17�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_09;		//9*17�û������ַ��Ĵ洢BUFFER
	}
	else
	{
		CharWidth = ASCII_FONT_A_WIDTH;
		pCode = g_tRamBuffer.UserCharStruct.UserCode_12;	//12��24�û������ַ������
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_12;		//12��24�û������ַ������
	}
	
	if ((GetChar >= 0x20) && (GetChar < 0x7F) && (GetChar < (0x20 + USER_ZF_SIZE)))
	{
		pCode += (GetChar - 0x20);	//�ҵ����ַ�Ӧ�ô洢��λ��
		if (*pCode == GetChar)			//������ַ��Ѿ������������
		{
			*pCode = 0;								//�������
			pZimo += ((GetChar - 0x20) * CharWidth * 3);
			for (i = 0; i < (CharWidth * 3); i++)		//�����ģ����
				*pZimo++ = 0;
		}
	}
	
	pCode = NULL;
	pZimo = NULL;
}

/*******************************************************************************
* Function Name  : Command_1B40
* Description    : ��ʼ����ӡ��
*	�����ӡ����������,��ӡģʽ����Ϊ�ϵ�ʱ��Ĭ��ֵģʽ
*	DIP���ص����ò������ٴμ��
*	���ջ��������ݱ���
*	�궨�屣��
*	flashλͼ���ݲ�����
*	flash�û����ݲ�����
*	ά��������ֵ������
*	��GS ( E ָ��������ֵ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B40(void)
{
	while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus())
	{
		;
	}
	if(g_tPrtCtrlData.PCDriverMode == DRV_PRT_TYPE_NONE)	//2016.06.21
	{	
		InitParameter(1);
		//���ҳ��ӡ������
	}	
}

/*******************************************************************************
* Function Name  : Command_1B44
* Description    : ���ú�������λ��
*	1B   44  n1...nk  NUL
*	1  ��   n1 �� n2 �� ���� nk��   255
*	0  ��   k  ��   32
*	���ú�������λ��
*	���������n������һ������λ��
*	����k ������λ��
*	��������λ������ʽ����
*	�ַ���� �� n ,�ַ���Ȱ����Ҽ��,����ַ�Ϊ����,�������Ҳ��֮�ӱ�
*	��ָ��ȡ����ǰ������λ������
*	����λ�ð���������,������ΪNUL
*	��[n]kС�ڻ����ǰһ��[n]k-1ֵʱ,�������ý���,�����������Ϊ��ͨ���ݴ���
*	ESC D NUL  ȡ�����е�����λ������
*	�������32��( k = 32)����λ��,����32������λ�����ݱ���Ϊ��ͨ���ݴ���
*	�ı��ַ����,��ǰָ��������λ�ò��������仯
*	�ַ�����ڱ�׼ģʽ��ҳģʽ���Ƕ�����
*	Ĭ������������ÿ8����׼ASCII�ַ�(12  ��  24)һ������λ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B44(void)
{
	uint8_t	GetChar;
	uint8_t	PreChar;
	uint8_t	TebNum;
	uint16_t	Width;
	uint32_t	i;
	
	
 	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//��׼ģʽ
	{
		Width = (g_tPrtModeChar.CharWidth + g_tPrtModeChar.RightSpace) * g_tPrtModeChar.WidthMagnification;
	}
	else
	{
		Width = (g_tPrtModeChar.CharWidth + g_tPageMode.AscRightSpace) * g_tPrtModeChar.WidthMagnification;
	}
	
	PreChar = 0x00;
	TebNum = 0x00;
	
	while ( TebNum < MAX_HT_INDEX_NUM)				
	{
		GetChar = ReadInBuffer();
		if ((GetChar <= PreChar) || (GetChar == 0x00))				//���nk �� n[k-1] ��n��0x00�������
		{
			break;
		}
		else
		{
			PreChar = GetChar;
			//--����λ�� = ����ֵ x ����ǰASCII�ַ��� + ��ǰASCII�ַ��Ҽ�ࣩx ��ǰASCII�ַ�����Ŵ���
			g_tTab.TabPositionIndex[TebNum] = GetChar * Width;
			TebNum++;
		}
	}

	g_tTab.TabSetNumber = TebNum;
	g_tTab.TabIndex = 0;
	for (i = TebNum; i < MAX_HT_INDEX_NUM; i++)			//�����ǰ���������
	{
		g_tTab.TabPositionIndex[i] = 0;
	}
}

/*******************************************************************************
* Function Name  : Command_1B42
* Description    : 
1B 42 n t
1<=n<=9 1<=t<=9
��ӡ��������ӡ������ʾ����
n�Ƿ������еĴ���
t�Ƿ�����ÿ������ʱ��t*50ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B42(void)
{
	uint8_t	TempN;
	uint16_t	TempT;
	
	TempN = ReadInBuffer();
	TempT = ReadInBuffer();
	
	if(TempN >0 && TempN <10 )
	{
		if(TempT >0 && TempT <10 )
		{
			 BuzzerRunFlag = 1;
       BuzzerNum = TempN;
       BuzzerTime = TempT*50;
		}	
	}	
	
}
/*******************************************************************************
* Function Name  : Command_1B45
* Description    : ѡ��/ȡ���Ӵ�ģʽ,ֻ�����λ��Ч
*	1B   45   n
*	��n�����λΪ0ʱ,ȡ���Ӵ�ģʽ
*	��n�����λΪ1ʱ,ѡ��Ӵ�ģʽ
*	nֻ�����λ��Ч
* ESC !ͬ������ѡ��/ ȡ���Ӵ�ģʽ,�����յ�������Ч
*	Ĭ��n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B45(void)
{
	g_tPrtModeChar.EmphaSizeMode = ReadInBuffer() & 0x01;
	g_tPrtModeHZ.EmphaSizeMode = g_tPrtModeChar.EmphaSizeMode;
}

/*******************************************************************************
* Function Name  : Command_1B47
* Description    : ѡ��/ȡ��˫�ش�ӡģʽ,ֻ�����λ��Ч,��ӡЧ��ͬCommand_1B45
*	1B   47   n
*	��n�����λΪ0ʱ,ȡ��˫�ش�ӡģʽ
*	��n�����λΪ1ʱ,ѡ��˫�ش�ӡģʽ
*	nֻ�����λ��Ч
* ��ָ����Ӵִ�ӡЧ����ͬ
*	Ĭ��n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B47(void)
{
	g_tPrtModeChar.EmphaSizeMode = ReadInBuffer() & 0x01;
	g_tPrtModeHZ.EmphaSizeMode = g_tPrtModeChar.EmphaSizeMode;
}

/*******************************************************************************
* Function Name  : Command_1B4A
* Description    : ��ӡ����ֽ
*	1B   4A   n
*	��ӡ���������ݲ���ֽ [ n  ��  ���������ƶ���λ]  Ӣ��
*	��ӡ������,����ǰ��ӡλ����������
*	��ֽ���벻��  ESC 2 ��  ESC 3 ָ�����õ�Ӱ��
*	����������ƶ���λ��GS P�趨
*	��׼ģʽ��,ʹ�������ƶ���λ
*	��ҳģʽ��,���ݴ�ӡ����ķ���ʹ�ӡ��ʼλ����ѡ��ʹ�������ƶ���λ������ƶ���λ,
*	��ѡ��ʽ����: 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�������ƶ���λ�� 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�ú����ƶ���λ�� 
* �����ֽ������1016 mm (40 Ӣ��).  ��������������,ȡ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4A(void)
{
	uint8_t	GetChar;
	uint16_t	temp;
	uint32_t	LineSpace;
	
	GetChar = ReadInBuffer();
	if(g_tPrtCtrlData.BMPBitNum > 0 )	//2016.08.04
	{
		if( GetChar >=24 )
		{
			GetChar -=24;
		}
		else
		{
			if( GetChar >= g_tPrtCtrlData.BMPBitNum )
			{
				GetChar -= g_tPrtCtrlData.BMPBitNum;
			}
			else
				GetChar	=0;
		}
	}
	g_tPrtCtrlData.BMPBitNum =0;	
	
	LineSpace = GetChar / (2* g_tPrtCtrlData.StepVerUnit);         //���ݰ�����
	
	if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//���ֽ40Ӣ�� 1016mm
		LineSpace = MAX_LINE_SPACE_VERTICAL;		//�޸�,����Ҫ���ҳģʽʱ�Ĵ���
	
	//�ȴ��д�ӡ��������ӡ����,Ȼ�󽫱༭�д�ӡ�����������ݸ��Ƶ���ӡ�д�ӡ������,
	//Ȼ��������ӡ,��ձ༭������������,�ٿ�ʼ�༭.
	//��ѯ�д�ӡ������״̬,һֱ�ȵ��п����л��������Խ��д�ӡ
	//WaitPreviousPrintFinish();
	
	temp = g_tPrtCtrlData.CodeLineHigh;		//��ǰ��ӡ�м��
//	g_tPrtCtrlData.CodeLineHigh = g_tPrtCtrlData.LineHigh + LineSpace;	//��ָ��Ҫ�����м��
  g_tPrtCtrlData.CodeLineHigh =  LineSpace;	//��ָ��Ҫ�����м�� ���ݰ�����
// 	PrintOneLine();												//��ӡ�ñ༭������
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	g_tPrtCtrlData.CodeLineHigh = temp;		//�ָ�ԭ��ӡ�м��
}

/*******************************************************************************
* Function Name  : Command_1B4B
* Description    : ��ӡ��������ֽ
*	1B   4B   n
*	��ӡ���������ݲ�������ֽ [ n  ��  ���������ƶ���λ]  Ӣ��
*	��ӡ������,����ǰ��ӡλ����������
*	��ֽ���벻��  ESC 2 ��  ESC 3 ָ�����õ�Ӱ��
*	����������ƶ���λ��GS P�趨
*	��׼ģʽ��,ʹ�������ƶ���λ
*	��ҳģʽ��,���ݴ�ӡ����ķ���ʹ�ӡ��ʼλ����ѡ��ʹ�������ƶ���λ������ƶ���λ,
*	��ѡ��ʽ����: 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�������ƶ���λ�� 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�ú����ƶ���λ�� 
* �����ֽ�����ǻ������������ֵ.  ��������������,ȡ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4B(void)
{
#if	0
	uint8_t	GetChar;
	uint16_t	temp;
	uint32_t LineSpace;
	
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON���,��ӡ��������ֽ
	{
		GetChar = ReadInBuffer();
		LineSpace = GetChar * g_tPrtCtrlData.StepVerUnit;
		
		if (LineSpace > FEED_BACK_MAX_STEPS)	//���ֽ,�����ֽ���ֵ
			LineSpace = FEED_BACK_MAX_STEPS;		//����Ҫ���ҳģʽʱ�Ĵ���
		
		//�ȴ��д�ӡ��������ӡ����,Ȼ�󽫱༭�д�ӡ�����������ݸ��Ƶ���ӡ�д�ӡ������,
		//Ȼ��������ӡ,��ձ༭������������,�ٿ�ʼ�༭.
		
		//��ѯ�д�ӡ������״̬,һֱ�ȵ��п����л��������Խ��д�ӡ
		WaitPreviousPrintFinish();
		
		temp = g_tPrtCtrlData.CodeLineHigh;				//��ǰ��ӡ�м��
		g_tPrtCtrlData.CodeLineHigh = 0;					//��ָ��Ҫ�����м��
// 		PrintOneLine();														//��ӡ�ñ༭������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		g_tPrtCtrlData.CodeLineHigh = temp;				//�ָ�ԭ��ӡ�м��
		FeedPaper(FEED_BACKWARD, LineSpace);			//������ֽ
		DelayUs(1000 * 1000);											//�ȴ���ֽ���
	}
	else		//΢��ָ�,ͼ�δ�ӡ
	{
		Command_1B13();
	}
#endif

	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	Line;
	uint8_t	Width;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t	i;
	uint16_t	RevNumber;
	uint16_t	iMaxNumber;
	uint8_t	* p = NULL;
	uint8_t	* pstart = NULL;
	uint8_t	* ptemp = NULL;
	
	GetChar = ReadInBuffer();
	iMaxNumber = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);	//���ݳ���
	if (!iMaxNumber)	//��������ֽ���Ϊ0ֱ���˳�
		return;
	
	if ((g_tPrtModeChar.WidthMagnification == 1) && (g_tPrtCtrlData.OneLineWidthFlag == 1))
	{
		RatioX = 2;
	}
	else
	{
		RatioX = g_tPrtModeChar.WidthMagnification;	//ȷ������Ŵ���,�������ڱ���
	}
	RatioY = g_tPrtModeChar.HeightMagnification;	//ȷ������Ŵ���
	
	RevNumber = iMaxNumber;
	if ((RevNumber * RatioX) > PRT_DOT_NUMBERS)	//ȷ������������ֽڳ���
	{
		RevNumber = g_tPrtCtrlData.PrtLength / RatioX;
	}
	
	//�ڱ༭�д�ӡ������֮ǰ��Ҫ�ȼ�����Ƿ����,��������ȴ���ӡ,����Ҫ�ȴ��д�ӡ��������ӡ����,
	//Ȼ�󽫱༭�д�ӡ�����������ݸ��Ƶ���ӡ�д�ӡ������,Ȼ��������ӡ,��ձ༭������������,�ٿ�ʼ�༭.
	//����༭�д�ӡ����,�༭��������,Ҳ��Ҫ�������ϴ���
	if (g_bLineEditBufStatus == LEB_STATUS_WAIT_TO_PRINT)	//�༭�д�ӡ�������ȴ���ӡ
	{
// 		PrintOneLine();		//�ȴ�ӡ�ñ༭������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_QT;	//������Ϊ����ģʽʵ��
	
	//�����ô�ӡ���Ʊ�����ָ���ʼ��
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
	{
		g_tPrtCtrlData.BitImageFlag = 1; 	//�ɱ�λͼ��ӡ��־
		g_tPrtCtrlData.LineHigh = 8;			//�޸��и�
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//��ӡ�и�
	}
	ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
	
	for (i = 0; i < RevNumber; i++)		//�������λͼ����
	{
		//����һ��������д�ӡ������
		if ((g_tPrtCtrlData.PrtDataDotIndex + RatioX) < g_tPrtCtrlData.PrtLength)
		{
			GetChar = ReadInBuffer();		//����һ��λͼ����
			if (GetChar)
			{
				if (RatioY > 1)
				{
					memset(TempBuf, 0x00, MAX_RATIO_Y);
					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
					ByteZoomCtrl(RatioY, GetChar, TempBuf);		//����Ŵ�
				}
				else
				{
					TempBuf[0] = GetChar;
				}
				//�����ӡ������,��λͼ�Ŵ��ĸ�λMSB��ʼ����,��Ϊÿ�δ���һ��λͼ�ֽ�,
				//�������ʱ�����ֽ����ڸ߶���=RatioY,�ڿ����=RatioX
				//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
				pstart = ptemp + g_tPrtCtrlData.PrtDataDotIndex;
				for (Line = 0; Line < RatioY; Line++)
				{	//�д�ӡ��������ʼ + �ڼ��ֽ�����ʼ + ��ǰ������
					p = pstart;
					for (Width = 0; Width < RatioX; Width++)	//����Ŵ���
					{
						*(p + Width) = TempBuf[Line];	//ˮƽ���������
					}
					pstart += PRT_DOT_NUMBERS;
				}		//һ��λͼ�ֽں�������������
			}
			g_tPrtCtrlData.PrtDataDotIndex += RatioX;
		}
		else
		{
// 			PrintOneLine();	//��ӡ�ñ༭������
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
			i--;						//�����;��ӡһ��,����ֵ��ӦҪ����
			
			//�������ô�ӡ���Ʊ�����ָ���ʼ��
			if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
			{
				g_tPrtCtrlData.BitImageFlag = 1; 	//�ɱ�λͼ��ӡ��־
				g_tPrtCtrlData.LineHigh = 8;			//�޸��и�
				g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//��ӡ�и�
			}
			ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
		}
	}		//End λͼ���
		
	for (; i < iMaxNumber; i++)
		ReadInBuffer();		//���ն����λͼ����
}

/*******************************************************************************
* Function Name  : Command_1B4C
* Description    : ѡ��ҳģʽ
*	�ӱ�׼ģʽת����ҳģʽ,������ֻ���ڱ�׼ģʽ�����ײ���Ч
*	��ҳģʽ��,��ָ����Ч
*	��ִ��FF  ��  ESC S��,��ӡ�����ص���׼ģʽ
*	��ָ���ӡλ�����õ�ESC T �����ESC W����ȷ����λ����
*	��ָ�����ָ�������ת����ҳģʽ�µ�ֵ:
*	�� �����ַ��Ҽ��:ESC SP, FS S 
*	�� �����м��:ESC 2, ESC 3 
*	����ָ����ҳģʽ��ֻ�ı��־λ,ת������׼ģʽ��������.
*	�� ˳ʱ����ת90�� :ESC V 
*	�� ѡ���ַ�����ģʽ:ESC a 
*	�� ѡ����ģʽ:ESC { 
*	�� ������հ�:GS L 
*	�� ���ô�ӡ������:GS W 
*	����ָ����ҳģʽ�±�����. 
*	�� ִ�в��Դ�?.�GS ( A 
*	����ָ����ҳģʽ���ǲ�����
*	�� ��ӡ NV λͼ:FS p 
*	�� ���� NV λͼ:FS q 
*	�� д�û� NV ������:FS g 1 
*	�� ��ӡ��դλͼ:GS v 0 
*	����Դ��,��ӡ����λ,��ִ��ESC @  ָ��,��ӡ���ص���׼ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4C(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE && g_tPrtCtrlData.PrtDataDotIndex == 0)
	{
		g_tSystemCtrl.PrtModeFlag = PAGE_MODE;
	}
}

/*******************************************************************************
* Function Name  : Command_1B4D
* Description    : ѡ������
*	1B   4D   n
*	n = 0, 1,48, 49
*		n  							���� 
*	0,48  ѡ���׼ASCII���� (12 �� 24)  
*	1,49  ѡ��ѹ��ASCII���� (9 �� 17) 
*	Ĭ��n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B4D(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 0) || (GetChar == 48))	//ʹ��12x24��ASCII�ַ�
	{
 		g_tSysConfig.SysFontSize = ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh	= ASCII_FONT_A_HEIGHT;
	}
	else if ((GetChar == 1) || (GetChar == 49))	//ʹ��9x17��ASCII�ַ�(��׼ģʽ��ҳģʽͬʱ��Ч)
	{
		g_tSysConfig.SysFontSize = ASCII_FONT_B;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh	= ASCII_FONT_B_HEIGHT;
	}
}

/*******************************************************************************
* Function Name  : Command_1B52
* Description    : ѡ������ַ���
*	1B   52   n
*	0  �� n  ��   15	
*	���±�ѡ������ַ���n 
*	n  Character  Set 
*	0		U.S.A. 
*	1		France 
*	2		Germany 
*	3		U.K. 
*	4		Denmark�� 
*	5		Sweden 
*	6		Italy 
*	7		Spain �� 
*	8		Japan 
*	9		Norway 
*	10	Denmark �� 
*	11	Spain �� 
*	12	Latin 
*	13	Korea  
*	14	Slovenia/Croatia 
*	15	China
*	Ĭ�� n = 15  �ۼ������ģݣ�n = 0 �۳�����������������ͺţ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B52(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if (GetChar <= 15)
	{
		g_tSystemCtrl.CharacterSet = GetChar;
	}
}

/*******************************************************************************
* Function Name  : Command_1B53
* Description    : ѡ���׼ģʽ
*	���ñ�׼ģʽ
*	��ָ����ҳģʽ����Ч. 
*	��ָ�����ҳ��������ӡ����. 
*	��ָ���ǰλ����������. 
*	ҳģʽ���򱻳�ʼ��ΪĬ��ֵ. 
*	��ָ�����ָ�������ת������׼ģʽ�µ�ֵ:
*	�� �����Ҽ��:ESC SP, FS S 
*	�� ѡ���м��:ESC 2, ESC 3 
*	���������ڱ�׼ģʽ�¿����趨��ز��������趨ֵ����ҳģʽ���������: 
*	�� ����ҳģʽ�´�ӡ����:  ESC W 
*	�� ����ҳģʽ��������:  ESC T 
*	����ָ���ڱ�׼ģʽ�±�����: 
*	�� ҳģʽ������������Դ�ӡλ��:  GS $ 
*	�� ҳģʽ��������Դ�ӡλ��:GS \
*	��ӡ����Դ��,Ӳ����λ��ִ��ESC @  ָ���,��ӡ���ص���׼ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B53(void)
{
	//�ô�������Ҫ��һ���޸�
	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)
	{
		g_tSystemCtrl.PrtModeFlag = STD_MODE;
	}
}

/*******************************************************************************
* Function Name  : Command_1B54
* Description    : ��ҳģʽ��ѡ���ӡ������
*	1B   54   n
*	0  ��   n  ��  3    48  ��   n  ��  51
*	��ҳģʽ��ѡ���ӡ����ķ������ʼλ��
*		n  	��ӡ����  ��ʼλ�� 
*	0,48  ������  ����(ͼ��A )
*	1,49  ���µ���  ����(ͼ��B )
*	2,50  ���ҵ���  ����(ͼ��C)
*	3,51  ���ϵ���  ����(ͼ��D)
*	�����ǰģʽΪ��׼ģʽ,ֻ�����ڲ���־λ,��Ӱ���ӡ
*	��ָ��������ô�ӡ�����ڴ�ӡ�����е���ʼλ��
*	���������ӡ��ʼλ�õĲ�ͬ,����������ƶ���λ��ʹ��Ҳ����ͬ: 
*	�� �����ӡ��ʼλ��Ϊ��ӡ��������Ͻǻ����½�,��ô��ӡ�������з������ӡ��ֽ����ֱ. 
*  ��������ʹ�ú����ƶ���λ:ESC SP, ESC $, ESC \
*  ��������ʹ�������ƶ���λ:ESC 3, ESC J, GS $, GS \
*	�� �����ӡ����ʼλ��Ϊ��ӡ��������½ǻ����Ͻ�,���ӡ�������з������ӡ����ֽ������ͬ. 
*  ��������ʹ�ú����ƶ���λ:ESC 3, ESC J, GS $, GS \
*��������ʹ�������ƶ���λ:ESC SP, ESC $, ESC \
*	[ Ĭ��ֵ]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//��ȱ�ٸ�ָ���
void Command_1B54(void)
{
//	uint8_t	GetChar;
	
//	GetChar = ReadInBuffer();
	ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B55
* Description    : �����ַ�����Ŵ�(ASCII�ַ�)�Ŵ���,����HRI�ַ�����.
*	POSָ�����Ϊ˫���ӡ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B55(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON	���,ѡ��/ȡ��˫���ӡ
	{
		
	}
	else  //΢�����,����Ŵ�
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.WidthMagnification = GetChar;		//�ַ�������
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B56
* Description    : ѡ��/ȡ��˳ʱ����ת90��,������ֻ�ڱ�׼ģʽ����Ч
*	1B   56   n
*	0  ��   n  ��   2, 48 ��   n  ��   50
*		n  			����
*	0,48 ȡ��˳ʱ����ת90��ģʽ 
*	1,49	ѡ��˳ʱ����ת90��ģʽ 
*	2,50	ѡ��˳ʱ����ת90��ģʽ 
*	��ѡ���»���ģʽʱ,�»��߲���˳ʱ����ת90��
*	˳ʱ����ת90��ģʽ�µı��ߺͱ���������ģʽ�µķ����෴
*	[ Ĭ��ֵ]    n = 0
* 1B 56 n			΢������Ϊ�����ַ��߶ȷŴ�n��,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B56(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON	���,ѡ��/ȡ��˳ʱ����ת90��
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//������ֻ�ڱ�׼ģʽ����Ч
		{
			if ((GetChar == 0) || (GetChar == 1) || (GetChar == 48) || (GetChar == 49))
			{
				g_tPrtCtrlData.CharCycle90 = GetChar & 0x01;
			}
			else if ((GetChar == 2) || (GetChar == 50))		//�޸�,���2,50�����
			{
				g_tPrtCtrlData.CharCycle90 = 0x01;
			}
		}
	}
	else  //΢�����,����Ŵ�
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.HeightMagnification = GetChar;		//�ַ�������
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B57
* Description    : �����ַ�(ASCII�ַ�)�������Ŵ���,����HRI�ַ�����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B57(void)
{
	uint8_t	GetChar;
	uint32_t	i;
	
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON���,ҳģʽ�����ô�ӡ����
	{
		if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//�����������Ŵ���
		{
			GetChar = ReadInBuffer();
			GetChar &= 0x0F;
			if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
			{
				return;
			}
			else
			{
				g_tPrtModeChar.WidthMagnification	= GetChar;	//�ַ�������
				g_tPrtModeChar.HeightMagnification = GetChar;	//�ַ���������
			}
		}
		else if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)	//ҳģʽ�����ô�ӡ����
		{
			for (i = 0; i < 8; i++)
				GetChar = ReadInBuffer();	//��ʱֻ��ȡ������
		}
	}
	else  //΢�����,�������Ŵ�
	{
		GetChar = ReadInBuffer();
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeChar.WidthMagnification	= GetChar;	//�ַ�������
			g_tPrtModeChar.HeightMagnification = GetChar;	//�ַ�������
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B58
* Description    : �����ַ�(ASCII�ַ�)�������Ŵ���,
*	1B 58 n1 n2	΢������Ϊ�����ַ�����Ŵ�n1��,����Ŵ�n2��,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B58(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
	{
		;
	}
	else
	{
		g_tPrtModeChar.WidthMagnification	= GetChar;	//�ַ�������
	}
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeChar.HeightMagnification = GetChar;	//�ַ�������
	}
}

/*******************************************************************************
* Function Name  : Command_1B5C
* Description    : ������Ժ����ӡλ��
*	1B    5C   nL   nH
*	0  ��   nl ��   255     0 ��   nH ��  255
*	�Ժ���������ƶ���λ���ú������λ��
*	��ָ���ӡλ�����õ��൱ǰλ��[( nL + nH  ��  256) �� ����������ƶ���λ]��
*	�����ɴ�ӡ��������ý�������
*	����ӡλ�������ƶ�ʱ:nL+ nH ��  256 = N
*	����ӡλ�������ƶ�ʱ���ò���:nL+ nH ��  256 = 65536 �C N
*	��ӡ��ʼλ�ôӵ�ǰλ���ƶ���[ N  ��  �����ƶ���λ���������ƶ���λ] 
*	��ӡ��ʼλ�ôӵ�ǰλ���ƶ���[ N  ��  �����ƶ���λ���������ƶ���λ] 
*	�ڱ�׼ģʽ��,ʹ�ú����ƶ���λ
*	��ҳģʽ��,���ݴ�ӡ����ķ������ʼλ����ѡ��ʹ�ú����ƶ���λ�������ƶ���λ,
*	��ѡ��ʽ����: 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������Ͻǻ����½�ʱ,ʹ�ú����ƶ���λ�� 
*	�� ����ӡ��ʼλ����ESC T ����Ϊ��ӡ��������½ǻ����Ͻ�ʱ,ʹ�������ƶ���λ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B5C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp, Temp2;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		//Temp = GetChar + 256 * ReadInBuffer();
		Temp = (uint16_t)GetChar + (uint16_t)(((uint16_t)(ReadInBuffer())) << 8);
		Temp *= g_tPrtCtrlData.StepHoriUnit;
		//�ò��ֳ�����Ҫ���ݴ�ӡ�����޸�����ֵ
		Temp2 = Temp + g_tPrtCtrlData.PrtDataDotIndex;
		if (Temp2 < g_tSystemCtrl.LineWidth)		//�޸�
			g_tPrtCtrlData.PrtDataDotIndex = Temp2;
	}
	else	//����Ҫ���ҳģʽ�Ĵ���
	{
	}
}

/*******************************************************************************
* Function Name  : Command_1B61
* Description    : �����ַ�����ģʽ
*	1B    61   n
*	0  ��   n  ��   2, 48 ��   n  ��   50
*	ʹ���еĴ�ӡ���ݰ�ĳһָ�����뷽ʽ����
*		n			���뷽ʽ 
*	0, 48		����� 
*	1, 49		�м���� 
*	2, 50		�Ҷ���
*	��ָ��ֻ�ڱ�׼ģʽ�µ�������Ч
*	��ָ����ҳģʽ��ֻ�ı��ڲ���־λ
*	��ָ�����HT, ESC $ ��  ESC \ ָ���������հ�����
*	[ Ĭ��ֵ]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B61(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
		{
			if ((GetChar == 0) || (GetChar == 1) || (GetChar == 2) || \
					(GetChar == 48) || (GetChar == 49) || (GetChar == 50))
			{
				g_tPrtCtrlData.CodeAimMode = GetChar & 0x03;	//ֻ���������λ��ֵ
			}
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B63
* Description    : ѡ�񴫸���(����,ֽ��,ֽ�������Ʒ�ʽ),����΢��Ĵ�ӡ�������ָ��
[ ����]     ѡ��ֽ������ 
[ ��ʽ]   ASCII   ESC  c  0  n 
Hex   1B  63  30  n 
Decimal  27   99  40  n 
[ ��Χ]     0  ��   n  ��   2 
[ ����]    ѡ��ֽ������ 
   n = 0, ����ֽ������Ϊ����ֽ 
   n = 1, 2, ����ֽ������Ϊ���ֽ 
[ ע��]   .  ���ֽ��ָӡ�кڰױ�ǵĴ�ӡֽ 
   . ��ֽ����������Ϊ���ֽʱ,�벻Ҫʹ������ֽ,����ִ��GS FF ����ᵼ�´� 
    ӡ����ֽ�ܳ�����,��ֽ����������Ϊ����ֽʱ,�벻Ҫʹ�ñ��ֽ,�����ӡ�� 
        ����ȱֽ 
[ Ĭ��ֵ]    n = 0 
[ �ο�]     GS FF 

*	1B   63  33  n 
*	ѡ��ֽ��������������нӿڵ�ȱֽ�ź�
*	λ	0/1		Hex		Decimal		���� 
*	0		0			00		0					ֽ������������Ч
*			1			01		1					ֽ������������Ч
*	1		0			00		0					ֽ������������Ч
*			1			02		2					ֽ������������Ч
*	2		0			00		0					ֽ����������Ч
*			1			04		4					ֽ����������Ч
*	3		0			00		0					ֽ����������Ч
*			1			08		8					ֽ����������Ч
*	4-7												δ����
*	��ָ�����ѡ���������������ȱֽ�ź�,��������κ�һ����ѡ��Ĵ��������
*	��ȱֽʱ�����ȱֽ�ź�.
*	��ָ��ֻ���ڽӿڷ�ʽΪ����ʱ��Ч,���ӿ�Ϊ����ʱ��Ч
*	����� n  �ĵ�0λ���1λ��Ϊ1,��ôֽ�����������������ȱֽ�ź�
*	����� n  �ĵ�2λ���3λ��Ϊ1,��ôֽ���������������ȱֽ�ź�
*	������������������ֹʱ,���нӿڵ�ȱֽ�ź����������ֽ״̬
*	[ Ĭ��ֵ]    n = 15 
*
*	1B   63   34   n
*	ѡ��ֽ����������ֹ��ӡ
*	λ	0/1		Hex		Decimal		���� 
*	0		0			00		0					ֽ������������Ч
*			1			01		1					ֽ������������Ч
*	1		0			00		0					ֽ������������Ч
*			1			02		2					ֽ������������Ч
*	2-7												δ����
*	�� n  �ĵ�0λ���1 λΪ1 ʱ,ֽ������������Ч,����⵽ֽ����ʱ,��ӡ�굱 
*	ǰ�����ֹͣ��ӡ,��ӡ����������״̬.
*	[ Ĭ��ֵ]    n = 0 
*	
*	1B   63   35   n
*	����/ ��ֹ����
*	�� n  �����λΪ0 ʱ,����������. 
* �� n  �����λΪ1 ʱ,��������ֹ
*	ֻ�� n  �����λ��Ч
*	��ִ�к�����ʱ,�������ǿ��õ�,������ͨ����������ֽ
*	[ Ĭ��ֵ]    n = 0
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B63(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x00:			//ѡ�������ӡ,����T3
		{
			g_tPrtCtrlData.LineDirection = GetChar;
			break;
		}
		case 0x01:			//ѡ�����ӡ,����T3
		{
			g_tPrtCtrlData.LineDirection = GetChar;
			break;
		}
		case 0x30:			//ѡ��ֽ������
		{
			GetChar = ReadInBuffer();
			GetChar &= 0x03;
			if (GetChar == 0x00)	//ֽ����,0 = ����ֽ 1 = ���ֽ
				g_tSystemCtrl.PaperType = PAPER_TYPE_CONTINUOUS;
			else
				g_tSystemCtrl.PaperType = PAPER_TYPE_BLACKMARK;
			break;
		}
		case 0x33:			//ѡ��ֽ���������ֽ���ź�,ֻ�Բ�����Ч
		{
			GetChar = ReadInBuffer();
			if (g_tInterface.LPT_Type)				//����
			{
				if (GetChar & 0x03)
					g_tError.LPTPaperEndSensorSelect |= 0x01;	//ֽ������������Ч
				else
					g_tError.LPTPaperEndSensorSelect &= (uint8_t)(~0x01);
				if (GetChar & 0x0C)
					g_tError.LPTPaperEndSensorSelect |= 0x02;	//ֽ����������Ч
				else
					g_tError.LPTPaperEndSensorSelect &= (uint8_t)(~0x02);
			}
			break;
		}
		case 0x34:			//ѡ��ֽ����������ֹ��ӡ
		{
			GetChar = ReadInBuffer();
			if (GetChar & 0x03)
				g_tError.PaperEndStopSensorSelect |= 0x01;		//ֽ������������Ч
			else
				g_tError.PaperEndStopSensorSelect &= (uint8_t)(~0x01);
			#if	0																//���ֻ���ȱֽ������һֱ��Ч
			if (GetChar & 0x0C)
				g_tError.PaperEndStopSensorSelect |= 0x02;		//ֽ����������Ч
			else
				g_tError.PaperEndStopSensorSelect &= (uint8_t)(~0x02);
			#endif
			break;
		}
		case 0x35:			//����/��ֹ����,ֻ�����λ��Ч,��n�����λΪ0ʱ����������
		{
			g_tFeedButton.KeyEnable = ReadInBuffer() & 0x01;
			break;
		}
		case 0x39:			//������ǰ����
		{
			GetChar = ReadInBuffer();
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name  : Command_1B64
* Description    : ��ӡ����ֽn��
*	1B   64   n
*	��ӡ������������ݲ���ǰ��ֽn��(�ַ���)
*	��ָ���ӡ���Ĵ�ӡ��ʼλ������������
*	��ָ�Ӱ����ESC 2 ��  ESC 3���õ��м��
*	�����ֽ����Ϊ1016 mm,�������ֵ����1016 mmʱ,ȡ���ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B64(void)
{
	uint8_t	GetChar;
	uint16_t LineSpace;
	
	GetChar = ReadInBuffer();
    if(GetChar>0)
    {
        GetChar =GetChar-1;                     //���ݰ�����
    }
 	LineSpace = GetChar * g_tPrtCtrlData.CodeLineHigh;      
	if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//���ֽ40Ӣ�� 1016mm
		LineSpace = MAX_LINE_SPACE_VERTICAL;		//�޸�,����Ҫ���ҳģʽʱ�Ĵ���
	//�ȴ��д�ӡ��������ӡ����,Ȼ�󽫱༭�д�ӡ�����������ݸ��Ƶ���ӡ�д�ӡ������,
	//Ȼ��������ӡ,��ձ༭������������,�ٿ�ʼ�༭.
	
	//��ѯ�д�ӡ������״̬,һֱ�ȵ��п����л��������Խ��д�ӡ
	//WaitPreviousPrintFinish();
// 	PrintOneLine();								//��ӡ�ñ༭������
	PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	GoDotLine((uint32_t)(LineSpace));
}

/*******************************************************************************
* Function Name  : Command_1B66
* Description    : ��ӡ����ֽn���ո���߿���
*	1B	66	m	n
*	��ǰ��ֽn���ո���߿���(�ַ���)
* ���m=0,����ӡn���ո�n��ֵӦ���ڴ�ӡ�����п�֮�ڡ� 
*	���m=1,����ӡn�����С�n��ֵӦ����0~255֮�䡣
*	��ָ�Ӱ����ESC 2 ��  ESC 3���õ��м��
*	�����ֽ����Ϊ1016 mm,�������ֵ����1016 mmʱ,ȡ���ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B66(void)
{
	uint8_t	bMode;
	uint8_t	bValue;
	uint32_t LineSpace;
	
	bMode = ReadInBuffer();
	if (0 == bMode)	/* ��n���ո� */
	{
		bValue = ReadInBuffer();
		if ((g_tPrtCtrlData.PrtDataDotIndex + bValue) < g_tPrtCtrlData.PrtLength)
		{
			g_tPrtCtrlData.PrtDataDotIndex += bValue;
		}
		else
		{
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		}
	}
	else if (1 == bMode)	/* ��n������ */
	{
		bValue = ReadInBuffer();
		LineSpace = bValue * g_tPrtCtrlData.CodeLineHigh;
		if (LineSpace > MAX_LINE_SPACE_VERTICAL)	//���ֽ40Ӣ�� 1016mm
			LineSpace = MAX_LINE_SPACE_VERTICAL;
		GoDotLine(LineSpace);
	}
}

/*******************************************************************************
* Function Name  : Command_1B69
* Description    : ESC/POSָ���Ϊ��ֽ,
[ ����]   ִ��һ�ΰ���ֽ,����ֽ. 
[ ע��]   ��ӡ�����а���ֽ. 
΢��ָ���Ϊ����/��ֹ���״�ӡ,ֻ�����λ��Ч,����΢��ָ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B69(void)
{
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)	//EPSON	���,��ֽ
	{	//�е�ָ��ֻ�������ײ���Ч
		if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
			return;
		DriveCutPaper(CUT_TYPE_PART);
	  #ifdef	PAPER_OUT_RE_PRINT_ENABLE
		  if (g_tSysConfig.PaperOutReprint)
			  ClearInBuf();				//���Ѿ���ӡ��ɵĵ�������
	  #endif
	}
	else
	{
		g_tPrtCtrlData.CharNotFlag = ReadInBuffer() & 0x01;
	}
}


#ifdef	WH_SELFDEF_CUT_CMD_ENABLE
/*******************************************************************************
* Function Name  : Command_1B6B
* Description    : 쿻͹�˾�Զ���ָ��,��ֽ
*	1B		6B		n
*	n = 0, 1, 48, 49 
*		n			����
*		0			ȫ��
*		1			����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1B6B(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x00:			 						//ȫ��ֽ��ʽ
		{	//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		case 0x01:			 						//����ֽ��ʽ
		{	//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		default:
			break;
	}
}
#endif

/*******************************************************************************
* Function Name	: Command_1B6C
* Description		: ������߾�,�ڱ�׼ģʽʱ,ֻ��������Ч,
1B 6C	n			΢������Ϊ������߾�,
��߾�����Ϊ	[n �� ASCII�ַ����]
��	������ֻ�������ײ���Ч. 
��	������ó����������ô�ӡ���,��ȡ�����ô�ӡ��� 
[ Ĭ��ֵ]		n = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1B6C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	GetChar = ReadInBuffer();
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//����
	{
		Temp = GetChar * (g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth);
		if (Temp > g_tSystemCtrl.LineWidth)	//�����������п�,������Ϊ����п�
		{
			Temp = g_tSystemCtrl.LineWidth;
		}
		
		//������������ɴ�ӡ�����ǲ���һ���ַ�������Ϊ�ܷ���һ���ַ�
		if (g_tSystemCtrl.CharacterSet == INTERNATIONAL_CHAR_SET_INDEX_ENGLISH)	//Ӣ��ģʽ
		{
			MiniCharWidth = g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth;	//��С�ַ����
		}
		else	//����ģʽ
		{
			MiniCharWidth = g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth;	//��С�ַ����
		}
		if (MiniCharWidth > g_tSystemCtrl.LineWidth)	//ȷ�������Ϸ�
		{
			MiniCharWidth = g_tSystemCtrl.LineWidth;
		}
		if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
		{
			g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//������ӡ����ȷ���ܷ���һ���ַ�
		}
		
		//��߾����ӡ���򳬹����ɴ�ӡ����,��Ҫ������ӡ���������߾�,���������������
		if ((Temp + g_tPrtCtrlData.PrtWidth) > g_tSystemCtrl.LineWidth)
		{
			g_tPrtCtrlData.PrtWidth = g_tSystemCtrl.LineWidth - Temp;	//������ӡ����
			if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
			{
				g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//������ӡ����ȷ���ܷ���һ���ַ�
			}
			Temp = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtWidth;	//������߾�
		}
		
		g_tPrtCtrlData.PrtLeftLimit = Temp;
		g_tPrtCtrlData.PrtDataDotIndex = Temp;
		g_tPrtCtrlData.PrtLength = Temp + g_tPrtCtrlData.PrtWidth;
	}
}

/*******************************************************************************
* Function Name 	: void DriveDrawer (u8 DrwID, u8 Status )
* Description		: ����Ǯ�亯���� 2016.06.18
* Input				: DrwID:Ǯ���
				  	    Status��Ϊ0��ʾ�ر�Ǯ�䣬1��Ǯ��
* Output			: None
* Return			: None
*******************************************************************************/
void DriveDrawer(uint8_t DrwID, uint8_t Status)
{
	
	if (Status == DISABLE)			//ֹͣǮ��
	{
		if (DrwID == 0)
		{
			MBOX_CTRL1_CLR;			//��Ǯ��1����
		}
		else
		{
			MBOX_CTRL2_CLR;			//��Ǯ��2����
		}
	}
	else
	{
		if (DrwID == 0)
		{
			MBOX_CTRL1_SET;			//��Ǯ��1����
		}
		else
		{
			MBOX_CTRL2_SET;			//��Ǯ��2����
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1B70
* Description    : Ǯ�����ָ��,��������,
*	1B   70   m   t1   t2
*	m = 0, 1, 48, 49 
*	0  ��   t1 ��   255, 0  ��   t2 ��  255
*	�����t1 ��t2 �趨��Ǯ�俪�����嵽��m ָ��������
*		m			�������� 
*	0, 48		Ǯ����������� 2 
*	1, 49		Ǯ����������� 5 
*	Ǯ�俪�����忪��ʱ��Ϊ [ t1 ��  2 ms]  ,Ǯ�俪������ص�ʱ��Ϊ [ t2 ��  2 ms]
*	���t2 < t1,�ص�ʱ��Ϊ [ t1 ��  2 ms]
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B70(void)
{
	uint8_t	Number;
	uint16_t OpenTime;
	
	Number = ReadInBuffer();

	if ((Number == 0) || (Number == '0'))	//'0'=48,'1'=49
	{
		g_tDrawer1.Status = ENABLE;				  //Ϊ�˷�ֹ�ڽ���Ǯ�俪��ʱ���� ��ֹʵʱָ��101401n ��Ӱ��
		while (g_tDrawer1.ON_Time);					//�ȴ��ϴ�ִ�����
		
		g_tDrawer1.DrwID = Number & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		g_tDrawer1.OFF_Time	= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		if (g_tDrawer1.OFF_Time < OpenTime)
		{
			g_tDrawer1.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer1.OFF_Time;	//�ܿ���ʱ��
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//��Ǯ��
		}	
		g_tDrawer1.ON_Time = OpenTime;
		g_tDrawer1.Status = DISABLE;								//�ڶ�ʱ����ʵ�ֶ�ʱ�͹ر�Ǯ��
	}
	else if((Number == 1) || (Number == '1'))
	{
		g_tDrawer2.Status = ENABLE;				  //Ϊ�˷�ֹ�ڽ���Ǯ�俪��ʱ������   ��ֹʵʱָ��101401n 
		while (g_tDrawer2.ON_Time);					//�ȴ��ϴ�ִ�����
		
		g_tDrawer2.DrwID = Number & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		g_tDrawer2.OFF_Time	= (uint16_t)(((uint16_t)(ReadInBuffer())) << 1);
		if (g_tDrawer2.OFF_Time < OpenTime)
		{
			g_tDrawer2.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer2.OFF_Time;	//�ܿ���ʱ��
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer2.DrwID, ENABLE);		//��Ǯ��
		}	
		g_tDrawer2.ON_Time = OpenTime;
		g_tDrawer2.Status = DISABLE;								//�ڶ�ʱ����ʵ�ֶ�ʱ�͹ر�Ǯ��
	}	
}

/*******************************************************************************
* Function Name  : Command_1B72
* Description    : ѡ���ӡ��ɫ
*	1B   70   m
*	m = 0, 1, 48, 49 
*		m			�������� 
*	0, 48		��ɫ
*	1, 49		��ɫ
* Ĭ��0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B72(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
}

/*******************************************************************************
* Function Name  : Command_1B74
* Description    : ѡ�����ҳ
*	N  ����ҳ  									Code Page 
*	0  CP437 [����,ŷ�ޱ�׼]  CP437 [U. S.A., Standard  Europe] 
*	1  KataKana [Ƭ����]  Katakana 
*	2  PC850 [������]  PC850 [Multilingual]  
*	3  PC860 [������]  PC860 [Portuguese]  
*	4  PC863 [���ô�- ����]  PC863 [Canadian-French] 
*	5  PC865 [��ŷ]  PC865 [Nordic]  
*	6  WCP1251 [˹������]  WCP1251 [Cyrillic] 
*	7  CP866  ˹����2  CP866 Cyrilliec #2   
*	8  MIK[˹����/��������]  MIK[Cyrillic  /Bulgarian ] 
*	9  CP755  [��ŷ,����ά�� 2] CP755 [East Europe,Latvian 2] 
*	10  [ ����,��˹]  Iran   
*	11  ����  reserve 
*	12  ����  reserve 
*	13  ����  reserve 
*	14  ����  reserve 
*	15  CP862  [ϣ����]  CP862  [Hebrew] 
*	16  ��CP1252 [ ������ 1]  ��CP1252 Latin I 
*	17  WCP1253 [ϣ��]  WCP1253 [Greek]   
*	18  CP852 [������  2]  CP852 [Latina 2] 
*	19  CP858 [�������������� 1+ŷԪ��]  
*	CP858  Multilingual Latin ��+Euro) 
*	20  ���ʢ�[ ��˹��]  Iran  II   
*	21  ����ά��  Latvian 
*	22  CP864 [��������]  CP864  [Arabic] 
*	23  ISO-8859-1 [ ��ŷ]  ISO-8859-1 [West Europe]        
*	24  CP737 [ϣ��]  CP737  [Greek] 
*	25  WCP1257 [���޵ĺ�]  WCP1257 [Baltic] 
*	26  [̩��1]  Thai 1  
*	27  CP720[��������]  CP720[Arabic] 
*	28  CP855  CP855 
*	29  CP857[��������]  CP857[Turkish] 
*	30  WCP1250[��ŷ]  WCP1250[Central Eurpoe] 
*	31  CP775  CP775 
*	32  WCP1254[��������]  WCP1254 [Turkish] 
*	33  WCP1255[ϣ������]  WCP1255[Hebrew] 
*	34  WCP1256[��������]  WCP1256[Arabic] 
*	35  WCP1258[Խ����]  WCP1258[Vietnam] 
*	36  ISO-8859-2[������2]  ISO-8859-2[Latin 2 ]  
*	37  ISO-8859-3[������3]  ISO-8859-3[Latin 3 ]  
*	38  ISO-8859-4[���޵���]  ISO-8859- 4[Baltic] 
*	39  ISO-8859-5[˹������]  ISO-8859-5[Cyrillic]  
*	40  ISO-8859-6[��������]  ISO-8859-6[Arabic ]  
*	41  ISO-8859-7[ϣ����]  ISO-8859-7[Greek]  
*	42  ISO-8859-8[ϣ������]  ISO-8859-8[Hebrew]  
*	43  ISO-8859-9[��������]  ISO-8859-9[Turkish] 
*	44  ISO-8859-15[������9]  ISO-8859-15  [Latin 3] 
*	45  [̩��2]  Thai2 
*	46  CP856  CP856 
*	[ Ĭ��ֵ]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B74(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar < MAX_CODEPAGE_SET_NUM)
		g_tSysConfig.DefaultCodePage = GetChar;
}

/*******************************************************************************
* Function Name  : Command_1B7B
* Description    : ����/ȡ�������ӡ,ֻ�����λ��Ч	,ֻ��������Ч
*	��n �����λΪ0 ʱ,ȡ�����ô�ӡģʽ. 
* ��n �����λΪ1 ʱ,ѡ���ô�ӡģʽ.
*	ֻ��n�����λ��Ч�� 
* ��ָ��ֻ�ڱ�׼ģʽ�µ�������Ч. 
* ��ָ����ҳģʽ��,ֻ�ı��ڲ���־λ. 
* ��ָ���ҳģʽ��ӡ��Ӱ��. 
* �ڵ��ô�ӡģʽ��,��ӡ���Ƚ�Ҫ��ӡ������ת180��Ȼ���ӡ.
*	[ Ĭ��ֵ]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B7B(void)
{
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//����������
			g_tPrtCtrlData.LineDirection = ReadInBuffer() & 0x01;
	}
	else
	{
		ReadInBuffer();
	}
}

/*******************************************************************************
* Function Name  : Command_1B6D
* Description    : ִ�а���,����EPSON����ָ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B6D(void)
{
	//�е�ָ��ֻ�������ײ���Ч
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
		return;
	DriveCutPaper(CUT_TYPE_PART);
#ifdef	PAPER_OUT_RE_PRINT_ENABLE
	if (g_tSysConfig.PaperOutReprint)
		ClearInBuf();				//���Ѿ���ӡ��ɵĵ�������
#endif
}

/*******************************************************************************
* Function Name  : Command_1B75
* Description    : ��������״̬(Ǯ��״̬),Ϊ����EPSON����ָ��
*	��ָ����Ҫ�������������޸�
*	
*	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B75(void)
{
	uint8_t	GetChar;
	uint8_t	Status;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)
	{
		Status = READ_MONEYBOX_STATUS;	//Ǯ��ӿ�3�ŵ�ƽ 2016.06.19
		Status &= 0x01;
		UpLoadData(&Status, 1);					//�ϴ�����,ֻ�����λ��Ч
	}
}

/*******************************************************************************
* Function Name  : Command_1B76
* Description    : ����ֽ������״̬,Ϊ����EPSON����ָ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B76(void)
{
	uint8_t	Status;
	
	if ((g_tRealTimeStatus.PrinterStatus & 0x08) == 0x08)		//����״̬�²��ط�
		return;
	
	Status = 0;
	if (g_tError.PaperSensorStatusFlag & 0x01)		//ֽ��״̬
		Status |= 0x0C;
	if (g_tError.PaperSensorStatusFlag & 0x02)		//ֽ����״̬
		Status |= 0x03;
	
	UpLoadData(&Status, 1);					//�ϴ�����
}


/*******************************************************************************
* Function Name  : PrintBarCode_2D
* Description    : ��ӡ��ά����
* Input          : Buf:λͼ���׵�ַ,��ά�����ݰ������ú��ŵķ�ʽ����,
*                  X_Length,�������,Y_Length,�����ֽ���,
*	               RatioX:����Ŵ���,RatioY:����Ŵ�����
* Output         : None
* Return         : None
*******************************************************************************/
void PrintBarCode_2D(uint16_t X_Length, uint16_t Y_Length, uint8_t RatioX, uint8_t RatioY)
{
	uint8_t	 TempBuf[8];
	uint8_t	GetChar;
	uint8_t	ratiox;
	uint8_t	ratioy;
	uint16_t x;
	uint16_t y;
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbBuf = NULL;
	uint8_t	* p = NULL;
	uint16_t  Index;
	s16	Lines;
  
	//�ڱ�׼ģʽ�ж����˶�ά��ͼ�����������׵����
	
  if ((g_tSystemCtrl.PrtModeFlag == STD_MODE))			
	{
		Index = g_tPrtCtrlData.PrtDataDotIndex;
			
		for (y = 0; y < Y_Length; y++)		//����߶�,���ֽ�Ϊ��λ
		{	//ָ��ǰ�༭���ݻ�������ʼ�ֽ���
								//(Y_Length - y) * RatioY��ΪRatioY
			pbByteStart = g_tLineEditBuf + \
				(MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS + Index;   
				
			for (x = 0; x < X_Length; x++)	//������,�Ե�Ϊ��λ
			{
				pbTemp = pbByteStart + (x * RatioX);		//ָ��ǰ�༭���ݻ�������
				if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//�����ӡ
				{
					GetChar = g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[ X_Length * (Y_Length - 1 - y) + x];
				}
				else		//����������ӡ˳�����
				{
					GetChar = g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[ X_Length * y + x];
				}
				if (GetChar)
				{
					if (RatioY > 1)	//��Ҫ�Ŵ�
					{
						memset(TempBuf, 0x00, 8);     //2017.08.25 MAX_RATIO_Y��Ϊ8
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//����Ŵ���Ϊ1
					{
						TempBuf[0] = GetChar;
						//TempBuf[1] = GetChar;
					}
					for (ratiox = 0; ratiox < RatioX; ratiox++)
					{
						for (ratioy = 0; ratioy < RatioY; ratioy++)
						{
							*(pbTemp + ratiox + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
						}
					}	//һ���ֽ�������
				}
			}	//һ���ֽں���������
			g_tPrtCtrlData.PrtDataDotIndex = Index+(X_Length * RatioX);
			g_tPrtCtrlData.MaxPrtDotHigh = ((uint16_t)(RatioY) << 3);	//�����ܵ���
			
			//�����д�ӡ������
			pbBuf = g_tLineEditBuf + \
			       (MAX_PRT_HEIGHT_BYTE - RatioY) *  PRT_DOT_NUMBERS;  //�ַ��ڱ༭���������׵�ַ
					
			if( g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
			{										//�����ӡ
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
			}
			PrepareNewLine();	    //���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
			
		}	//������ȫ��������
	}	//�ֳɶ�δ�ӡ�������
	//������Ч�������
	//�������Ҷ���������λͼ�������
	
	pbByteStart = NULL;
	pbTemp = NULL;
     
}

/*******************************************************************************
* Function Name  : ProcessBarCode_2D
* Description    : ��ά����ָ���
* Input          : 
	Buf:λͼ���׵�ַ,
	X_Length,�����ֽ���,Y_Length,�����ֽ���,
	RatioX:����Ŵ���,RatioY:����Ŵ���
* Output         : None
* Return         : None
*******************************************************************************/
void ProcessBarCode_2D(uint8_t CommandType)
{
	uint8_t	 GetChar;
	uint8_t	 version, vColl, vRateX, vRateY;
	uint16_t i, datalen ;
	uint32_t res;
	uint8_t  QR_ECCLevel[]={'L','M','Q','H'};			    //QR CODE����ȼ�

	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =3;	//��ά���ӡ��־���޸ı�־��ʽ��ֵ

	version = ReadInBuffer();					//����V
	vColl = ReadInBuffer();						//����r

	if( CommandType == 0x5A )                   //1B 5Aָ�� ����
	{											
		vRateY = ReadInBuffer();				//����k
		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==2)					//QR CODE 1D5Aѡ��Ķ�ά������
		{
			for(i=0; i<4; i++)
			{
				if(vColl == QR_ECCLevel[i])		//����QR CODE����ȼ�
					break;
			}
			if(i<4)
				vColl = i+1;
			else
				vColl = 1;						//����ʱ�Զ�����Ϊ1

		}
	}
	else                                        //1D 6Bָ�����
	{											
		vRateY = g_tBarCodeCtrl.BarCodeThinWidth ;
		if(CommandType ==32 || CommandType ==97 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =2;
        }   
		else if(CommandType ==33 || CommandType ==98 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =1;
        }    
		else if(CommandType ==34 || CommandType ==99 )
        {    
			g_tRamBuffer.BarCode2DStruct.BarCode2DType =0;
        }    
	}

	//�Ŵ�������
	if(vRateY ==0)	
    {    
		vRateY =1;
    }    
	else if(vRateY > 8)         //���ķŴ���Ϊ8
    {    
		vRateY =8;
    }
	vRateX = vRateY;

	if(	CommandType >=32 && CommandType <=34) 
	{
		datalen=0;
		while(1)										//��ȡ��00�������������� ָ���ʽһ
		{
			GetChar =  ReadInBuffer();
			if(GetChar ==0x00)
				break;
			g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[datalen] = GetChar;
			datalen++;									

		}
	}
	else                                            //ָ���ʽ�� ���ղ�������
	{
		GetChar = ReadInBuffer();
		datalen = GetChar  + ReadInBuffer()*256;

		for (i = 0; i < datalen; i++)
        {
			g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[i] = ReadInBuffer();
        }    
	}

	if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)        //��ά�� PDF417
	{
		res = Pdf417_Print( g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		vRateX =2;				//PDF417����̶�Ϊ2����

	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 1)   //��ά�� Matrix��
	{
		res = Matrix_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//У���Ƿ��ܷ��¸�ͼ��
			res =0;
	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)   //��ά�� QR��
	{
		res = QR_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//У���Ƿ��ܷ��¸�ͼ��
			res =0;
	}
	if ((res) == 0x0000)
    {    
		PrintString("BarCode2D Data/Parameter Error\r\n");
    }    
	else
	{
		PrintBarCode_2D((res >> 16),(res & 0xffFF), vRateX, vRateY);
	}
	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =0;		//�޸ı�־��ʽ
}


/*******************************************************************************
* Function Name  : ProcessBarCode_2D_1D28
* Description    : ��ά����ָ���
* Input          : 
	Buf:λͼ���׵�ַ,
	X_Length,�����ֽ���,Y_Length,�����ֽ���,
	RatioX:����Ŵ���,RatioY:����Ŵ���
* Output         : None
* Return         : None
*******************************************************************************/
void ProcessBarCode_2D_1D28(void)
{
	uint8_t	 GetChar;
	uint8_t	 version, vColl, vRateX, vRateY;
	uint16_t i, datalen ;
	uint32_t res;
	uint8_t  QR_ECCLevelNum[]={48,49,50,51};			      //QR CODE����ȼ� ��ӦL M Q H

	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =3;	//��ά���ӡ��־���޸ı�־��ʽ��ֵ

	if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)	       //PDF417
	{
			vRateY = PDF417_Rate;				                          //�Ŵ���
		  version = PDF417_version;
	}		
  else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)	  //QR Code
	{
			vRateY = QR_Rate;				                                //�Ŵ���
		  version = QR_version;																		//�汾
	}		
		
	if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==2)					//QR CODE 1D5Aѡ��Ķ�ά������
	{
		for(i=0; i<4; i++)
		{
			if(QR_vColl == QR_ECCLevelNum[i])		                  //����QR CODE����ȼ�
				break;
		}
		if(i<4)
			vColl = i+1;
		else
			vColl = 1;						//����ʱ�Զ�����Ϊ1
	}
	else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType ==0)		 //PDF417
	{
		vColl = PDF417_vColl;
	}	


	//�Ŵ�������
	if(vRateY ==0)	
  {    
		vRateY =1;
  }    
	else if(vRateY > 8)            //���ķŴ���Ϊ8
  {    
		vRateY =8;
  }
	vRateX = vRateY;

  datalen = TwoCodeDataNum;   //�������ݳ���
	
	if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)        //��ά�� PDF417
	{
		res = Pdf417_Print( g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		vRateX =2;				//PDF417����̶�Ϊ2����

	}
	else if (g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)   //��ά�� QR��
	{
		res = QR_Print(g_tRamBuffer.BarCode2DStruct.BarCode2DBuf, version, vColl, datalen);
		if(((res >> 16) * vRateX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex ))	//У���Ƿ��ܷ��¸�ͼ��
			res =0;
	}
	if ((res) == 0x0000)
    {    
		PrintString("BarCode2D Data/Parameter Error\r\n");
    }    
	else
	{
		PrintBarCode_2D((res >> 16),(res & 0xffFF), vRateX, vRateY);
	}
	g_tRamBuffer.BarCode2DStruct.BarCode2DFlag =0;		//�޸ı�־��ʽ
}

/*******************************************************************************
* Function Name  : Command_1B5A
* Description    : ��ӡ��ά����
*                  PDF417  DATA MATRIX   QR CODE
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1B5A(void)
{
    if(g_bLineEditBufStatus == LEB_STATUS_ILDE)
    {    
        ProcessBarCode_2D(0x5A);
    }    
}

/*******************************************************************************
* Function Name  : EscCommand
* Description    : ESSC����Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ESC_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x09:
		{
			SetCommandFun();	//����ϵͳ����
			break;
		}
		case 0x0C:
		{
			Command_1B0C();	//ҳģʽ�´�ӡ����(ESC FF )�Ĵ���
			break;
		}
		case 0x13:
		{
			Command_1B13();	//쿻�������ӡ,ͬ1B4B
			break;
		}
		case 0x20:
		{
			Command_1B20();	//�����ַ��Ҽ��
			break;
		}
		case 0x21:				//���ô�ӡģʽ
		{
			Command_1B21();
			break;
		}
		case 0x24:			  //���þ��Դ�ӡλ��
		{
			Command_1B24();
			break;
		}
		case 0x25:			  //ѡ��/ȡ���û��Զ����ַ�ģʽ
		{
			Command_1B25();
			break;
		}
		case 0x26:			  //�����Զ����ַ�
		{
			Command_1B26();
			break;
		}
		case 0x2A:			  //��ӡλͼ
		{
			Command_1B2A();
			break;
		}
		case 0x2D:				//�»��ߴ�ӡ
		{
			Command_1B2D();
			break;
		}
		case 0x31:			  //�����м��
		{
			Command_1B31();
			break;
		}
		case 0x32:			  //����Ĭ���м��
		{
			Command_1B32();
			break;
		}
		case 0x33:			  //�����м��
		{
			Command_1B33();
			break;
		}
		case 0x3D:			  //ѡ���ӡ��
		{
			Command_1B3D();
			break;
		}
		case 0x3F:			  //����Զ����ַ�
		{
			Command_1B3F();
			break;
		}
		case 0x40:			  //��ʼ����ӡ��
		{
			Command_1B40();
			break;
		}
		case 0x42:			  //2016.09.13 ���Ʒ�����
		{
			Command_1B42();
			break;
		}		
		case 0x44:			  //���ú�������λ��
		{
			Command_1B44();
			break;
		}
		case 0x45:			  //ѡ��/ȡ���Ӵ�ģʽ
		{
			Command_1B45();
			break;
		}
		case 0x47:			  //ѡ��/ȡ��˫�ش�ӡģʽ
		{						
			Command_1B47();
			break;
		}
		case 0x4A:			  //��ӡ����ֽn����
		{
			Command_1B4A();
			break;
		}
		case 0x4B:			  //ESCPOS��ӡ��������ֽn����,΢��ָ�Ϊͼ�δ�ӡ
		{
			Command_1B4B();
			break;
		}
		case 0x4C:			  //ѡ��ҳģʽ
		{
// 			Command_1B4C();   //2017.03.16
			break;
		}
		case 0x4D:			  //ѡ������
		{
			Command_1B4D();
			break;
		}
		case 0x52:			  //ѡ������ַ�
		{
			Command_1B52();
			break;
		}
		case 0x55:			  //POSָ�ѡ��˫���ӡģʽ,΢��ָ����ú���Ŵ���
		{
			Command_1B55();
			break;
		}
		case 0x56:			  //POSָ�ѡ��˳ʱ�ӷ�����ת90�ȴ�ӡģʽ,΢��ָ���������Ŵ���
		{
			Command_1B56();
			break;
		}
		case 0x57:			  //POSָ����ô�ӡ����Χ,΢��ָ������������Ŵ���
		{
			Command_1B57();
			break;
		}
		case 0x58:			  //΢��ָ������������Ŵ���
		{
			Command_1B58();
			break;
		}
		case 0x5A:			  //��ӡ��ά����
		{
			Command_1B5A();
			break;
		}
		case 0x5C:			  //������Դ�ӡλ��
		{
			Command_1B5C();
			break;
		}
		case 0x61:			  //ѡ���ַ�����ģʽ
		{
			Command_1B61();
			break;
		}
		case 0x63:			  //ѡ��ֽ��������ֹ��ӡ
		{
			Command_1B63();
			break;
		}
		case 0x64:			  //��ӡ����ֽn��
		{
			Command_1B64();
			break;
		}
		case 0x66:			  //��ӡ����ֽn���ո���߿���
		{
			Command_1B66();
			break;
		}
		case 0x69:			  //POSָ���ֽ,΢��ָ�����/��ֹ���״�ӡ
		{
			Command_1B69();
			break;
		}
		#ifdef	WH_SELFDEF_CUT_CMD_ENABLE
		case 0x6B:
		{
			Command_1B6B();	//쿻�����ʵ����ָֽ��
			break;
		}
		#endif
		case 0x6C:
		{
			Command_1B6C();	//΢������Ϊ������߾�,n���ַ����
			break;
		}
		case 0x6D:
		{
			Command_1B6D();	//����ֽ
			break;
		}
		case 0x70:
		{
			Command_1B70();	//Ǯ�����
			break;
		}
		case 0x72:
		{
			Command_1B72();	//ѡ���ӡ��ɫ
			break;
		}
		case 0x74:			  //ѡ���ַ�����ҳ
		{
			Command_1B74();
			break;
		}
		case 0x75:
		{
			Command_1B75();	//��������״̬(Ǯ��)
			break;
		}
		case 0x76:
		{
			Command_1B76();	//����ֽ������״̬
			break;
		}
		case 0x7B:
		{
			Command_1B7B();	//�����ӡ
			break;
		}
		
		default:
			break;
	}
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
