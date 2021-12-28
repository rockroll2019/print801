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
#include	"rs.h"
#include	"cominbuf.h"
#include	"extgvar.h"					//ȫ���ⲿ��������
#include	"charmaskbuf.h"
#include	"spiflash.h"
#include	"timconfig.h"
#include	"extgvar.h"
#include	"debug.h"

#ifdef	DBG_SELF_DEF_CMD_ENABLE

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
* Function Name  : Command_1E01
* Description    : ����SPI�ֿ�,д��
* 1E 01 xL xH d1 ... dk
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E01(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	uint32_t	iCount;
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	if (iLen)
	{
#if	0
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//�������Ʋ����洢����,������Ϊ��λ����,ÿ����4K
		sFLASH_EraseSector(iAddress);				//ɾ���洢��
		iCount= iLen;
		while (iCount--)
		{
			GetChar = ReadInBuffer();					//��һ���ֽ�
			sFLASH_WriteBuffer(&GetChar, iAddress, 1);	//��1���ֽ�
			sFLASH_ReadBuffer(&bBuf, iAddress, 1);			//���ظ��ֽ�
			UpLoadData(&bBuf, 1);												//�ش�
			iAddress++;
		}
#else
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//�������Ʋ����洢����,������Ϊ��λ����,ÿ����4K
		sFLASH_EraseSector(iAddress);				//ɾ���洢��
		iCount= iLen;
		while (iCount--)
		{
			GetChar = ReadInBuffer();					//��һ���ֽ�
			sFLASH_WriteBuffer(&GetChar, iAddress++, 1);	//��1���ֽ�
		}
	#if	0
		DelayMs(1);
		iAddress = BIT_BMP_PARA_BASE_ADDR;	//�������Ʋ����洢����,������Ϊ��λ����,ÿ����4K
		iCount= iLen;
		while (iCount--)
		{
			sFLASH_ReadBuffer(&bBuf, iAddress++, 1);			//���ظ��ֽ�
			UpLoadData(&bBuf, 1);													//�ش�
		}
	#endif
#endif
	}

}

/*******************************************************************************
* Function Name  : Command_1E02
* Description    : ����SPI�ֿ�,ֻ��
* 1E 02 xL xH
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E02(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	iAddress = BIT_BMP_PARA_BASE_ADDR;	//�������Ʋ����洢����,������Ϊ��λ����,ÿ����4K
	while (iLen--)
	{
		sFLASH_ReadBuffer(&GetChar, iAddress++, 1);	//���ظ��ֽ�
		UpLoadData(&GetChar, 1);										//�ش�
	}
}

/*******************************************************************************
* Function Name  : Command_1E03
* Description    : ����SPI�ֿ�,����ָ���洢����ʼ�����ĵط�д��,�����ݲ�д��
* 1E 03 A0 A1 A2 A3 B0 B1 B2 B3 B4 xL xH d1 ... dk
* д��ʼ��ַ:A3A2A1A0
* д������ַ:B3B2B1B0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E03(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	uint32_t	iStartAddress;
	uint32_t	iStopAddress;
	uint32_t	iCount;
	
	GetChar = ReadInBuffer();								//A0
	iStartAddress = GetChar;
	GetChar = ReadInBuffer();								//A1
	iStartAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//A2
	iStartAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//A3
	iStartAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//B0
	iStopAddress = GetChar;
	GetChar = ReadInBuffer();								//B1
	iStopAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//B2
	iStopAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//B3
	iStopAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	if (iStopAddress > iStartAddress)
	{
		iAddress = iStartAddress;
		for (	iCount = 0; \
					iCount < ((iStopAddress - iStartAddress) / sFLASH_SPI_SECTORSIZE); \
					iCount++)
		{
			sFLASH_EraseSector(iAddress);					//ɾ���洢��
			iAddress += sFLASH_SPI_SECTORSIZE;
		}
	}
	
	while (iLen--)
	{
		GetChar = ReadInBuffer();					//��һ���ֽ�
		sFLASH_WriteBuffer(&GetChar, iStartAddress++, 1);	//��1���ֽ�
	}
}

/*******************************************************************************
* Function Name  : Command_1E04
* Description    : ����SPI�ֿ�,ֻ��,����ָ����ַ����ָ�����ȵ����ݻش�
* 1E 04 A0 A1 A2 A3 xL xH
* ����ʼ��ַ:A3A2A1A0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E04(void)
{
	uint8_t	GetChar;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	GetChar = ReadInBuffer();								//A0
	iAddress = GetChar;
	GetChar = ReadInBuffer();								//A1
	iAddress += (((uint32_t)GetChar) << 8);
	GetChar = ReadInBuffer();								//A2
	iAddress += (((uint32_t)GetChar) << 16);
	GetChar = ReadInBuffer();								//A3
	iAddress += (((uint32_t)GetChar) << 24);
	
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	while (iLen--)
	{
		sFLASH_ReadBuffer(&GetChar, iAddress++, 1);	//���ظ��ֽ�
		UpLoadData(&GetChar, 1);										//�ش�
	}
}

#ifdef	DBG_025
void	Command_1E10(void)	//�����ֿ�
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x01;
	g_bDbg025EnalbeFlag = GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x01;
	g_bDbg025FontFlag = GetChar;
}
#endif

#ifdef	DBG_GB18030_ASCII

#include	"charmaskbuf.h"
#include	"print.h"

/*�ⲿ��������*/
extern	TypeDef_StructCharMaskReadBuf		g_tCharMaskReadBuf;

/*******************************************************************************
* Function Name  : 
* Description    : ����GB18030�ֿ�,��ָ����ַ��ʼ��ָ�����ȵ��ַ���,����ӡ����,
	һ���ַ�ռ��72�ֽڿռ�.
*	�����ʽ:
1E 20 A0 A1 A2 A3 xL xH
* ����ʼ��ַ:A3A2A1A0
*	k = xL + 256 * xH 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Command_1E20(void)
{
	uint8_t	GetChar;
	uint8_t	CodeType;
	uint8_t	Result;
	uint32_t	iLen;
	uint32_t	iAddress;
	
	//1.�����ʼ��ַ
	iAddress = 0;
	for (iLen = 0; iLen < 4; iLen++)
	{
		GetChar = ReadInBuffer();
		iAddress |= (((uint32_t)GetChar) << (iLen << 3));
	}
	
	//2.��ö�ȡ�ַ�����
	GetChar = ReadInBuffer();								//xL
	iLen = GetChar + 256 * ReadInBuffer();	//xH
	
	//3.ѭ����ȡÿ���ַ�
	while (iLen--)
	{
		//3.1	��ȡ��ģ
		g_tCharMaskReadBuf.LanguageType = LANGUAGE_CHINESE;
		g_tCharMaskReadBuf.FontType = HZ_FONT_A;
		sFLASH_ReadBuffer(g_tCharMaskReadBuf.Buf, iAddress, BYTES_OF_HZ_FONT_A);
		iAddress += BYTES_OF_HZ_FONT_A;			//׼���´ζ�ȡ�ĵ�ַ
		//3.2	����ӡ������
		CodeType = g_tCharMaskReadBuf.LanguageType;	//�ַ����Ǻ���
		Result = JudgeLineBufFull(CodeType);	//3.2.1	�ж��Ƿ��Ѿ�����
		if (Result ==	LINE_BUF_NOT_FULL)	//���в������Էŵ��µ�ǰ�ַ�
		{																	//3.2.1.1�����ַ����뵱ǰ�༭�л�����
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);
			InitCharMaskBufParameter();			//�����ģ��ȡ������,�ַ���������־��ΪILDE
		}
		else if (Result == LINE_BUF_FULL)	//��ǰ�л����������޷����µ�ǰ�ַ�
		{
// 		PrintOneLine();									//3.2.1.2����������ӡ����
			PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh); 
			FillLinePrintBuf(g_tCharMaskReadBuf.Buf, CodeType);	//3.2.1�����
			InitCharMaskBufParameter();			//�����ģ��ȡ������,�ַ���������־��ΪILDE
		}
	}
	
	//4.ѭ����ȡ����,�жϱ༭�������Ƿ��д���ӡ����,�����,��ӡ����
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
	{
// 		PrintOneLine();										//��ӡ����
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
}
#endif

/*******************************************************************************
* Function Name  : RsCommand
* Description    : RS����Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RS_Command(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	switch (GetChar)
	{
		case 0x01:
		{
			Command_1E01();		//����SPI�ֿ�
			break;
		}
		case 0x02:
		{
			Command_1E02();		//����SPI�ֿ�
			break;
		}
		case 0x03:
		{
			Command_1E03();		//����SPI�ֿ�
			break;
		}
		case 0x04:
		{
			Command_1E04();		//����SPI�ֿ�
			break;
		}
#ifdef	DBG_025
		case 0x10:
		{
			Command_1E10();		//�����ֿ�
			break;
		}
#endif
#ifdef	DBG_GB18030_ASCII
	case 0x20:
		{
			Command_1E20();		//����GB18030�ֿ�
			break;
		}
#endif

		default:
			break;
	}
}

#endif	/*��չ�Զ�������������*/

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
