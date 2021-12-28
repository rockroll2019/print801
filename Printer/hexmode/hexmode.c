/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-30
  * @brief   16������صĳ���.
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
#include	"hexmode.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"feedpaper.h"
#include	"platen.h"
#include	"button.h"
#include	"timconfig.h"
#include	"charmaskbuf.h"

extern TypeDef_StructInBuffer volatile g_tInBuf;

extern	void	PrintString(const uint8_t * Str);
extern uint8_t Eth_ClearBusy;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void	HexModePrintStartHint(void);
static void	HexModePrintStopHint(void);
static void	FillHexModePrintOneLineData(uint8_t * pBuf, uint32_t bLen);

/* Private functions ---------------------------------------------------------*/

//�ֽ���յ��ַ��ĸ�4λ,����ASCIIֵ����
uint8_t	ChangeHighHalfOfByteToASCII(uint8_t	GetChar)
{
	uint8_t	Ch;
	
	Ch = GetChar >> 4;	//�����ַ��ĸ�4λȡ��,�����Χ0~F
	if (Ch > 9)		//��ΧA~F,ʮ����Ϊ10~15,����ASCII���A~F,ʮ����Ϊ65~70,��ֵΪʮ����55
	{
		Ch += 0x37;	//�任��ASCII��ֵ
	}
	else					//��Χ0~9,ʮ����Ϊ0~9,����ASCII���0~9,ʮ����Ϊ48~57,��ֵΪʮ����48
	{
		Ch += 0x30;	//�任��ASCII��ֵ
	}
	
	return	(Ch);
}

//�ֽ���յ��ַ��ĵ�4λ,����ASCIIֵ����
uint8_t	ChangeLowHalfOfByteToASCII(uint8_t	GetChar)
{
	uint8_t	Ch;

	Ch = GetChar & 0x0F;		//�ֽ��4λ
	if (Ch > 9)
	{
		Ch += 0x37;
	}
	else
	{
		Ch += 0x30;
	}
	
	return	(Ch);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ʮ�����ƴ�ӡģʽ,����16���ƺ��ӡ����ʾ,Ȼ��ȴ����������ַ�.
*	һ�д���8������,ʣ�����ݲ���һ�е���ʱ������.ÿ�ΰ�����ֽ����ֽ1��,���һ�ΰ���
*	��ֽ�����ӡ���������ʣ�������,����ӡ�˳���ʾ��,�˳�16����ģʽ.
*	Ŀǰ������58mmֽ���ӡ�ַ����12�����.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
/*01 02 03 04 05 06 07 08????????��384��*/
//һ�д����ӡ��16����������
#define	HEX_DEAL_CHARS_ONE_LINE	(8)

//һ�пɴ�ӡ�ַ�����,���ֲ��2���ַ�+1���ո�+1���ɴ�ӡ�ַ�=4���ַ�λ��
#define	HEX_PRT_CHAR_LEN				(HEX_DEAL_CHARS_ONE_LINE * 4)

//һ����Ҫ���ܴ洢�ռ�,ĩβ���NUL�ͻ����ַ�
#define	HEX_BUF_LEN							(HEX_PRT_CHAR_LEN + 2)

//һ�пɴ�ӡ�ַ������ַ���ʼλ��,������ʮ����������ӡ,�ճ�384������
#define	HEX_PRT_CHAR_VISIBLE_START_POSITION		(HEX_DEAL_CHARS_ONE_LINE * 3)

void	HexMode(void)
{
	uint8_t	PrtDataStr[ HEX_BUF_LEN ];
	uint32_t	LeftCharLen;
	
	SetComInBufBusy();					//��ӡ��ʾ��Ϣǰ����æ�رս���������Ϣ
	HexModePrintStartHint();		//��ӡ��ʾ��Ϣ
	g_tSystemCtrl.HexMode = 1;	//����16����ģʽ
	ClearComInBufBusy();				//���æ�ź�,��ӡ�����ʾ��Ϣ���ٿ�ʼ����
	EnableReceiveInterrupt();		//ȷ���������
	Eth_ClearBusy = 0;
	while (g_tSystemCtrl.HexMode)
	{
		if (g_tFeedButton.KeyStatus)	//��鰴��״̬
		{
			PrintString("\n");					//��ӡ1��
			g_tFeedButton.KeyStatus = 0;
		}
		//�����ֽ�����´���,<3��ʱ��������,>=3��ʱ��ӡ������ݲ��˳�16����ģʽ
		if (g_tFeedButton.KeyCount < 3)
		{
			if (GetInBufBytesNumber() >= HEX_DEAL_CHARS_ONE_LINE)
			{
				//g_tInBuf.BytesNumber ��ֵ���ڶ���ʱ�Զ�����
				FillHexModePrintOneLineData(PrtDataStr, HEX_DEAL_CHARS_ONE_LINE);
				PrintString(PrtDataStr);					//��ӡ����
			}	//����8�ֽ�ʣ���������������
			else
			{
				if(g_tInterface.ETH_Type) 
				{	
					GetETH_ConnectState();
				}	
			}	
		}	//��������<3�δ������
		else	//����3��FEED�����ӡʣ�����ݲ��˳�16����ģʽ
		{
			SetComInBufBusy();		//����æ�رս���������Ϣ
			//��ӡ�����ջ�����ʣ������
			while (GetInBufBytesNumber() > HEX_DEAL_CHARS_ONE_LINE)
			{
				//g_tInBuf.BytesNumber ��ֵ���ڶ���ʱ�Զ�����
				FillHexModePrintOneLineData(PrtDataStr, HEX_DEAL_CHARS_ONE_LINE);
				PrintString(PrtDataStr);					//��ӡ����
			}
			//��ӡ��ʣ�಻��1�е�����
			if (GetInBufBytesNumber())
			{
				LeftCharLen = GetInBufBytesNumber();
				FillHexModePrintOneLineData(PrtDataStr, LeftCharLen);
				PrintString(PrtDataStr);				//��ӡ����
			}	//ʣ���������������
			else	//��ӡ�����������ܵĲ���1�е�����
			{
				PrintString("\n");					//��ӡ1��
			}
			
			InitInBuf();									//�������ʣ��δ��ӡ������,����ӡ,ȫɾ��
			g_tFeedButton.KeyStatus = 0;
			g_tFeedButton.KeyCount = 0;
			g_tSystemCtrl.HexMode = 0;		//�˳�16����ģʽ
			ClearComInBufBusy();		//���æ�ź�,��ӡ�����ʾ��Ϣ���ٿ�ʼ����
		}	//������������3�δ������
	}	//16����ģʽ�������
	
	HexModePrintStopHint();
	
	g_tFeedButton.KeyStatus = 0;	//������ܵĶఴ�µİ���
	g_tFeedButton.KeyCount = 0;
}

#if	0
#define	HEX_DEAL_CHARS_ONE_LINE	(8)		//һ�д����ӡ��16����������
#define	HEX_PRT_CHAR_LEN				(HEX_DEAL_CHARS_ONE_LINE * 4 + 1)	//һ�пɴ�ӡ�ַ�����
#define	HEX_BUF_LEN							(HEX_PRT_CHAR_LEN + 2)						//һ����Ҫ���ܴ洢�ռ�
#define	HEX_PRT_CHAR_VISIBLE_START_POSITION	(HEX_DEAL_CHARS_ONE_LINE * 3 + 1)	//һ�пɴ�ӡ�ַ������ַ���ʼλ��

void	HexMode(void)
{
	uint8_t	GetChar, Ch;
	uint8_t	PrtDataStr[ HEX_BUF_LEN ];
	uint32_t	i;
	uint32_t	LeftCharLen;
	uint32_t  Temp1;
	
	SetComInBufBusy();		//��ӡ��ʾ��Ϣǰ����æ�رս���������Ϣ
	while (PLATEN_STATUS_OPEN == ReadPlatenStatus());	//�ȴ��ر�ֽ�ո��ٿ�ʼ��ӡ
	DelayUs(1000 * 1000);												//�ȴ�500ms
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	PrintString("Hexadecimal Dump \n");
	PrintString("To terminate hexadecimal dump,\n");
	PrintString("press FEED button three times.\n");
	PrintString("\n");
	g_tSystemCtrl.HexMode = 1;
	
	ClearComInBufBusy();		//���æ�ź�,��ӡ�����ʾ��Ϣ���ٿ�ʼ����
	//ReceiveInterrupt(ENABLE);		//�������ж�
	EnableReceiveInterrupt();
	while (g_tSystemCtrl.HexMode)
	{
		if (g_tFeedButton.KeyStatus)	//��鰴��״̬
		{
			PrintString("\n");					//��ӡ1��
			g_tFeedButton.KeyStatus = 0;
		}
		//�����ֽ�����´���,<3��ʱ��������,>=3��ʱ��ӡ������ݲ��˳�16����ģʽ
		if (g_tFeedButton.KeyCount < 3)
		{
			//2016.07.07  ���㻺���������ֽ���
			Temp1 = g_tInBuf.PutPosition;	//�ж��п��ܸı���ֵ
			if(Temp1 >= g_tInBuf.GetPosition)
				Temp1 = Temp1 - g_tInBuf.GetPosition;
			else
				Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
			g_tInBuf.BytesNumber = Temp1;
			
			if (g_tInBuf.BytesNumber >= HEX_DEAL_CHARS_ONE_LINE)
			{
				//����մ�������,ĩβ�Ļ��к�NUL�ַ������޸�
				memset(PrtDataStr, 0x20, HEX_BUF_LEN);		//�̶���ʽ����,ȫ������Ϊ�ո�
				//memset(PrtDataStr, 0x20, sizeof(PrtDataStr) / sizeof(PrtDataStr[0]));	//�̶���ʽ����,ȫ������Ϊ�ո�
				for (i = 0; i < HEX_DEAL_CHARS_ONE_LINE; i++)
				{
					GetChar = ReadInBuffer();
					//���յ����ַ��ֽ�Ϊ����16�����ַ�,��4λ��ǰ,��4λ�ں�,�ټ���1���ո�
					Ch = ChangeHighHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĸ�4λ
					PrtDataStr[3 * i] = Ch;	//�ֽ��ĸ�4λ
					Ch = ChangeLowHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĵ�4λ		
					PrtDataStr[3 * i + 1] = Ch;		//�ֽ��ĵ�4λ, 1���ո������ֶ�����
					
					if (GetChar < 0x20)						//�˴�����,Ӧ���ǲ��ɴ�ӡ�ַ�����'.'
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = '.';		//��ǰ����Ϊÿ8���ַ�һ��,����10��
					else
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = GetChar;	//��Ϊ�ɴ�ӡ�ַ���ӡ���ұ�
				}	//����8���ֽ����
				PrtDataStr[ HEX_PRT_CHAR_LEN ] = 0x0A;			//���ĩβ�Ļ��к�NUL�ַ�
				PrtDataStr[ HEX_PRT_CHAR_LEN + 1] = 0x00;		//ĩβһ���ַ���������
				PrintString(PrtDataStr);					//��ӡ����
			}	//����8�ֽ�ʣ���������������
		}	//��������<3�δ������
		else	//����3��FEED�����ӡʣ�����ݲ��˳�16����ģʽ
		{
			if(g_tInBuf.PutPosition != g_tInBuf.GetPosition) //2016.07.07(g_tInBuf.BytesNumber)
			{
				//����մ�������,ĩβ�Ļ��к�NUL�ַ������޸�
				memset(PrtDataStr, 0x20, HEX_BUF_LEN);		//�̶���ʽ����,ȫ������Ϊ�ո�
				//memset(PrtDataStr, 0x20, sizeof(PrtDataStr) / sizeof(PrtDataStr[0]));	//�̶���ʽ����,ȫ������Ϊ�ո�
				//2016.07.07 ����g_tInBuf.BytesNumber��ֵ
				Temp1 = g_tInBuf.PutPosition;	//�ж��п��ܸı���ֵ
				if(Temp1 >= g_tInBuf.GetPosition)
					Temp1 = Temp1 - g_tInBuf.GetPosition;
				else
					Temp1 =(g_tInBuf.BytesSize - g_tInBuf.GetPosition) + Temp1;
				g_tInBuf.BytesNumber = Temp1;
				LeftCharLen = g_tInBuf.BytesNumber;
				for (i = 0; i < LeftCharLen; i++)	//��ǰ����Ϊÿ8���ַ�һ��
				{
					GetChar = ReadInBuffer();
					//���յ����ַ��ֽ�Ϊ����16�����ַ�,��4λ��ǰ,��4λ�ں�,�ټ���1���ո�
					Ch = ChangeHighHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĸ�4λ
					PrtDataStr[3 * i] = Ch;	//�ֽ��ĸ�4λ
					Ch = ChangeLowHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĵ�4λ
					PrtDataStr[3 * i + 1] = Ch;		//�ֽ��ĵ�4λ, 1���ո������ֶ�����
					
					if (GetChar < 0x20)						//�˴�����,Ӧ���ǲ��ɴ�ӡ�ַ�����'.'
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = '.';		//��ǰ����Ϊÿ8���ַ�һ��,����10��
					else
						PrtDataStr[HEX_PRT_CHAR_VISIBLE_START_POSITION + i] = GetChar;	//��Ϊ�ɴ�ӡ�ַ���ӡ���ұ�
				}	//����ʣ���ֽ����
				PrtDataStr[ HEX_PRT_CHAR_LEN ] = 0x0A;			//���ĩβ�Ļ��к�NUL�ַ�
				PrtDataStr[ HEX_PRT_CHAR_LEN + 1] = 0x00;		//ĩβһ���ַ���������
				PrintString(PrtDataStr);				//��ӡ����
				InitInBuf();										//�������ʣ��δ��ӡ������,����ӡ,ȫɾ��
			}	//ʣ���������������
			
			g_tFeedButton.KeyStatus = 0;
			g_tFeedButton.KeyCount = 0;
			g_tSystemCtrl.HexMode = 0;		//�˳�16����ģʽ
			break;
		}	//������������3�δ������
	}	//16����ģʽ�������
	
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;
	PrintString("*** Hexadecimal Completed! *** \n\n");	//��16����ģʽ���˳������ӡ��ʾ
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	g_tFeedButton.KeyStatus = 0;
	g_tFeedButton.KeyCount = 0;
}
#endif

static void	HexModePrintStartHint(void)
{
	uint32_t i;
	
	//������ֶ���ʽ������ӵȴ���ʱ
	#ifdef	PLATEN_CHECK_ENABLE
	if (PLATEN_STATUS_OPEN == ReadPlatenStatus())
	{	
		//�ȴ��ر�ֽ�ո��ٿ�ʼ��ӡ
		while (PLATEN_STATUS_OPEN == ReadPlatenStatus());
		DelayUs(1000 * 1000);
	}
	#endif
	
	//�����ָ�ʽ������ֱ�ӿ�ʼ��ӡ
    GoDotLine(20);		//
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
	PrintString("Hexadecimal Dump \n");
	PrintString("To terminate hexadecimal dump,\n");
	PrintString("press FEED button three times.\n");
	
	if ((MODAL_NAME_STRING == "C04") || \
			(MODAL_NAME_STRING == "C05") || \
			(MODAL_NAME_STRING == "C04C05"))
	{
		for (i = 0; i < 10; i++)
		{
			PrintString("\n");
		}
	}
	else
	{
		PrintString("\n");
	}
}

static void	HexModePrintStopHint(void)
{
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;
	PrintString("*** Hexadecimal Completed! *** \n\n");	//��16����ģʽ���˳������ӡ��ʾ
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���ʮ�����ƴ�ӡģʽ������
*	
* Input          : �����Ļ������׵�ַ, ��������ַ��ĸ���
* Output         : 
* Return         : 
*******************************************************************************/
static void	FillHexModePrintOneLineData(uint8_t * pBuf, uint32_t bLen)
{
	uint8_t	GetChar;
	uint8_t	Ch;
	uint32_t	i;
	uint8_t * pData;
	
	//����մ�������, �̶���ʽ����,ȫ������Ϊ�ո�, ĩβ�Ļ��к�NUL�ַ������޸�
	pData = pBuf;
	for (i = 0; i < HEX_BUF_LEN; i++)
	{
		*pData++ = 0x20;
	}
	
	if (bLen > HEX_DEAL_CHARS_ONE_LINE)		//��ֹ�±�Խ��
	{
		bLen = HEX_DEAL_CHARS_ONE_LINE;
	}
	
	pData = pBuf;
	for (i = 0; i < bLen; i++)
	{
		GetChar = ReadInBuffer();
		//���յ����ַ��ֽ�Ϊ����16�����ַ�,��4λ��ǰ,��4λ�ں�,�ټ���1���ո�
		Ch = ChangeHighHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĸ�4λ
		*(pData + 3 * i) = Ch;			//�ֽ��ĸ�4λ
		Ch = ChangeLowHalfOfByteToASCII(GetChar);	//�ֽ���յ��ַ��ĵ�4λ		
		*(pData + 3 * i + 1) = Ch;	//�ֽ��ĵ�4λ, 1���ո������ֶ�����
		
		//��Ϊ�ɴ�ӡASCII�ַ���ӡ���ұ�, ���ɴ�ӡ�ַ�����'.', ��ǰ����Ϊÿ8���ַ�һ��
		if ((GetChar < PRINTABLE_START_CODE) || (GetChar >= CODEPAGE_START_CODE))
		{
			*(pData + HEX_PRT_CHAR_VISIBLE_START_POSITION + i) = '.';
		}
		else
		{
			*(pData + HEX_PRT_CHAR_VISIBLE_START_POSITION + i) = GetChar;
		}
	}	//�������8���ֽ����
	*(pData + HEX_PRT_CHAR_LEN) = 0x0A;			//���ĩβ�Ļ��к�NUL�ַ�
	*(pData + HEX_PRT_CHAR_LEN + 1) = 0x00;		//ĩβһ���ַ���������
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
