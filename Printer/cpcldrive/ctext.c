/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
  * @brief   CPCLָ��ַ�������س���.�����ַ���
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
#include	"ctext.h"
#include	"charmaskbuf.h"
#include	"extgvar.h"
#include	"spiflash.h"

#ifdef	CPCL_CMD_ENABLE

/*�����ַ���ȡ������,ȫ�ֱ���,����CPCLָ��*/
TypeDef_StructCharMaskReadBuf		g_tpCharMaskReadBuf;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : ��ʼ���ַ���ȡ������
* Input          : �������
* Return         : ���ز���
*******************************************************************************/
void	InitPageCharMaskBufParameter(void)
{
	g_tpCharMaskReadBuf.Status       = CMRB_STATUS_ILDE;	//��Ϊ����
	g_tpCharMaskReadBuf.LanguageType = g_tSysConfig.SysLanguage;	//����
	g_tpCharMaskReadBuf.FontType     = g_tSysConfig.SysFontSize;	//����
	memset(g_tpCharMaskReadBuf.Buf, 0x00, MAX_BYTES_OF_HZ);
}

/*******************************************************************************
* Function Name  : InitCharMaskBufParameter
* Description    : �����ַ���ȡ�������е��������ţ����Ժ�
* Input          : �������
* Return         : ���ز���
*******************************************************************************/
void	SetPageCharParameter(uint8_t Language, uint8_t FontType)
{
	g_tpCharMaskReadBuf.LanguageType = Language;	//����,���ģ�GB18030��BIG5
	g_tpCharMaskReadBuf.FontType     = FontType;	//����,ASCIIA/B/C;HZA/B
}

/*******************************************************************************
* Function Name  : ������
* Description    : ҳģʽ�´����ӡ
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	GetPageCharDotData(uint8_t CharCode, uint8_t DotData[])
{
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)	//����ģʽ
	{
  	GetPageASCIIDotData(CharCode, DotData);
	}	//���Ĵ������
  else										//����ģʽ
	{	//�Զ��庺��
  	if ((g_tSystemCtrl.UserSelectFlag) && \
				(CharCode == 0xFE) && \
				((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x02) == 0x02))
		{
			//JudgePrintSelfDefChinese(CharCode);	//�����Զ��庺�ִ�ӡ
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)	//GB18030
		{
			JudgePagePrintGB18030(CharCode, DotData);	//����GB18030��ӡ  	
		}
		else if (g_tSysConfig.SysLanguage == LANGUAGE_BIG5)	//BIG5���ַ�ʽ
  	{
			//JudgePrintBIG5(CharCode);			//����BIG5���ִ�ӡ
		}
		else
		{
		}
	}	//���ִ������
}

/*******************************************************************************
* Function Name  : 
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ,��������ȡ
*	�ȴ����Ƿ����Զ����ַ�,ֱ�Ӵ�RAM�и���;��������ֿ�FLASH��,����SPI DMA
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24��9*17,8*16
* Output         : 
* Return         : 
*******************************************************************************/
void	GetPageASCIIDotData(uint8_t AscIICode, uint8_t DotData[])
{
	uint8_t	FontType;
	
	FontType = g_tPageModeCharPara.FontType;	//������
	
	//�������Զ�����ַ�,��ѡ����,�Ҷ�Ӧ�ַ��ж���
	if ((g_tSystemCtrl.UserSelectFlag) && \
			(AscIICode < 0x7F) && \
			((g_tRamBuffer.UserCharStruct.UserdefineFlag & 0x01) == 0x01))
	{
		GetUserDefCharDotData(AscIICode, FontType, DotData);
	}
	else	//���û��Զ����ַ�
	{
		GetCharFlashDotData(AscIICode, FontType, DotData);	//��ȡ�ֿ��еĵ�������
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : ȡ�ֿ����ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24��9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetCharFlashDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[])
{
	uint16_t	Len;
	uint32_t	AsciiCodeAddr;
	
	if (AscIICode < 0x80)
	{
		if (g_tSystemCtrl.CharacterSet)	//��������˹����ַ���ָ�� 0x1b 52 n,USA=0
		{
			AsciiCodeAddr = GetInternationalCharStartAddress(AscIICode, FontType);
		}		//End �����ַ�
		else
		{
			AsciiCodeAddr = GetUSACharStartAddress(AscIICode, FontType);
		}
	}			//End <0x80
	else	//----FLASH��0x80-0xFF�����ASCII�ַ�----//
	{
		AsciiCodeAddr = GetCodepageCharStartAddress(AscIICode, FontType);
	}
	
	if (FontType == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		Len = BYTES_OF_ASCII_FONT_A;
	}
	else if (FontType == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		Len = BYTES_OF_ASCII_FONT_B;
	}
	else if (FontType == ASCII_FONT_C)	//FLASH��Ӣ��8x16�����ASCII�ַ�
	{
		Len = BYTES_OF_ASCII_FONT_C;
	}
	else																//Ĭ��A
	{
		Len = BYTES_OF_ASCII_FONT_A;
	}
	
	sFLASH_ReadBuffer(DotData, AsciiCodeAddr, Len);
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(DotData, Len);
	#endif
	
	SetPageCharParameter(LANGUAGE_ENGLISH ,FontType);
}

/*******************************************************************************
* Function Name  : 
* Description    : ��ȡ�Զ����ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ,����ȡ
*	�Ѷ����ַ�,ֱ�Ӵ�RAM�и���;��������ֿ�FLASH�ж�ȡ
*	AscIICode = ASCII�ַ�ֵ
* FontType �ַ���������ͣ���ǰ����������12*24��9*17,8*16
*	DotData[] = �洢ASCII�ַ�����ģ����
* Output         : 
* Return         : 
*******************************************************************************/
void	GetUserDefCharDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[])
{
	uint8_t 	Flag;
	uint16_t	Len;
	uint32_t	StartAddress;
	uint8_t * pselfdef;
	uint8_t * pbuf;
	
	Flag = 0;
	if (FontType == ASCII_FONT_A)				//����A
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_A;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else if (FontType == ASCII_FONT_B)	//����B
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_B;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_09;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else if (FontType == ASCII_FONT_C)	//����C
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_08[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_C;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_C;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_08;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	else																//Ĭ������A
	{
		if (g_tRamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20])
		{
			Len = BYTES_OF_ASCII_FONT_A;
			StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
			pselfdef = g_tRamBuffer.UserCharStruct.Buffer_12;
			pselfdef += StartAddress;
			Flag = 1;
		}
	}
	
	if (Flag == 1)	//���ڶ����ַ�
	{
		pbuf = DotData;
		while (Len--)
		{
			*pbuf++ = *pselfdef++;
		}
		
		SetPageCharParameter(LANGUAGE_ENGLISH ,FontType);
	}
	else
	{
		GetCharFlashDotData(AscIICode, FontType, DotData);	//��ȡ�ֿ��еĵ�������
	}
}

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : �����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,����,��ӡ��ǰ��.
* Input          : Code1,Code2,Code3,Code4Ϊ���ִ���,��Code3=Code4=0ʱΪ˫�ֽڴ���
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void	GetPageGB18030DotData(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4, uint8_t DotData[])
{
	uint8_t	FontType;
	uint16_t	Len;
	uint32_t	CodeIndex;
	uint32_t	CodeAddr;
	
	//�����ַ���ַ
	if ((Code3 == 0) || (Code4 == 0))	 //˫�ֽڷ�ʽ
	{
		if ((Code1 >= 0xA1) && (Code1 <= 0xA9) && (Code2 >= 0xA1))			//1������
		{	
			CodeIndex = (Code1 - 0xA1) * 94 + (Code2 - 0xA1);
		}
		else if ((Code1 >= 0xA8) && (Code1 <= 0xA9) && (Code2 < 0xA1))	//5������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2������
		{
			CodeIndex = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 >= 0x81) && (Code1 <= 0xA0))		//3������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 7806;	//1038 + 6768
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))			//4������
		{
			if (Code2 > 0x7F)
			{
				Code2--;
			}
			CodeIndex = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 13886;	//1038 + 12848
		}
		else
		{
			CodeIndex = 0;		//���������Ϊ������
		}
	}		//˫�ֽڴ������
	else
	{
		//CodeIndex =	(Code1 - 0x81) * 12600 + (Code2 - 0x39) * 1260 + 
		//						(Code3 - 0xEE) * 10 + (Code4 - 0x39) + 22046;
		if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 == 0xEE) && (Code4 == 0x39))
		{	//��λ8139�ĵ���һ���ַ�
			CodeIndex = 22046;
		}
		else if ((Code1 == 0x81) && (Code2 == 0x39) && (Code3 >= 0xEF))
		{	//8139ҳʣ�ಿ��
			CodeIndex = (Code3 - 0xEF) * 10 + (Code4 - 0x30) + 22047;	//22046 + 1
		}
		else if (	(Code1 == 0x82) && \
							((Code2 >= 0x30) && (Code2 <= 0x34)) && \
							((Code3 >= 0x81) && (Code3 <= 0xFE)))
		{	//8230ҳ~8234ҳ,��Ϊ��ҳ
			CodeIndex = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
		}
		else if ((Code1 == 0x82) && \
						 (Code2 == 0x35) && \
						((Code3 >= 0x81) && (Code3 <= 0x87)))
		{	//8235ҳ,ֻ��69���ַ�
			CodeIndex = (Code2 - 0x30) * 1260 + (Code3 - 0x81) * 10 + (Code4 - 0x30) + 22207;	//22046 + 1 + 160
		}
		else
		{	//������һ��Ϊ����λ,ȫ��
			CodeIndex = 0;
		}
	}
	
	FontType = g_tPageModeHZPara.FontType;
	if (FontType == HZ_FONT_A)
	{
		Len = BYTES_OF_HZ_FONT_A;
		CodeAddr = GB18030_24X24_BASE_ADDR;
	}
	else if (FontType == HZ_FONT_B)
	{
		Len = BYTES_OF_HZ_FONT_B;
		CodeAddr = GB18030_16X16_BASE_ADDR;
	}
	else
	{
		Len = BYTES_OF_HZ_FONT_A;
		CodeAddr = GB18030_24X24_BASE_ADDR;
	}
	CodeAddr = CodeAddr + (CodeIndex * Len);
	
	sFLASH_ReadBuffer(DotData, CodeAddr, Len);		//��ȡ��ģ
	
	#ifdef	ZK_ENCRYPTION
		FlashDecryption(DotData, Len);
	#endif
	
	SetPageCharParameter(LANGUAGE_GB18030, FontType);
}

/*******************************************************************************
* Function Name  : ������
* Description    : GB18030�������ݴ������,�ж������Ƿ�ϸ�,�������ӡ
* Input          : �������,��һ�ֽڱ���
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	JudgePagePrintGB18030(uint8_t Ch1, uint8_t DotData[])
{
	uint8_t	Ch2;
	uint8_t	Ch3;
	uint8_t	Ch4;
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//��һ����Ϊ��Ч���ִ���
	{
		GetPageASCIIDotData(Ch1, DotData);	//��һ�ֽ�Ϊ�����ַ���ӡ
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//��Ч���ִ���
		{
			GetPageASCIIDotData(Ch1, DotData);	//��һ�ֽڰ������ַ���ӡ
			DecInBufferGetPoint();		//�Ż�����ȡ���ַ�
		}
		else if (((Ch2 >= 0x40) && (Ch2 <= 0x7E)) || \
						 ((Ch2 >= 0x80) && (Ch2 <= 0xFE)))
		{
			Ch3 = 0;
			Ch4 = 0;
			GetPageGB18030DotData(Ch1, Ch2, Ch3, Ch4, DotData);	//���ֽں���
		}
		else									//���ֽڷ�ʽ
		{
			Ch3 = ReadInBuffer();
			if ((Ch3 < 0x81) || (Ch3 > 0xFE))  	//�����ֽڴ�����Ч
			{
				GetPageASCIIDotData(Ch1, DotData);	//��һ�ֽڰ������ַ���ӡ
				GetPageASCIIDotData(Ch2, DotData);	//�ڶ��ֽڰ������ַ���ӡ
				DecInBufferGetPoint();	//�Ż�����ȡ���ַ�
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//�����ֽڴ�����Ч
				{
					GetPageASCIIDotData(Ch1, DotData);	//��һ�ֽڰ������ַ���ӡ
					GetPageASCIIDotData(Ch2, DotData);	//�ڶ��ֽڰ������ַ���ӡ
					GetPageASCIIDotData(Ch3, DotData);	//�����ֽ�Ϊ��Ч��ӡ�ַ�
					DecInBufferGetPoint();	//�Ż�����ȡ���ַ�,ch4�п����ǺϷ���˫�ֽڱ���ĵ�λ�ֽ�
				}
				else
				{
					GetPageGB18030DotData(Ch1, Ch2, Ch3, Ch4, DotData);	//���ֽں���
				}
			}
		}
	}
}

/* ����Ϊ�����ַ����ߺ��ֵĵ�����ָ���Ĳ������б任����䵽��ӡ�������Ĵ��� */
/*******************************************************************************
* Function Name  : FillPageCharHZToEditBuf
* Description    : ����Ϊ������ȡ���ַ����ߺ��ֵĵ�����ָ�������弰��С���Ӵ֣�
	�������Ŵ��������ԣ���ת���»��ߵ����ý��е���任�����任��Ľ���ŵ�ָ����
	��������
	������Դ����Ϊ�������ţ��任������һ�ɰ��յײ����룬�������Ŵ���
	Ŀǰʹ��ESC�ı༭���������б༭ g_tLineEditBuf[] ��
	Դ�������ݱ�����ȫ�ֱ��� g_tpCharMaskReadBuf.Buf[]����������ݵ������С�����ԡ�
	�������Ŵ������Ӵ֣����ԣ���ת���»��ߵ������� g_tPageModeCharPara,
	g_tPageModeHZPara �����С�
* Input          : �������
* Return         : ���ز���
*******************************************************************************/
void	FillPageCharHZToEditBuf(uint8_t * pDst, uint8_t * pSrc)
{
	uint8_t	CodeType;
	uint8_t	FontType;
	uint8_t	Width;
	uint8_t	High;
	uint8_t	Temp;
	uint8_t * pbuf;
	
	CodeType = g_tpCharMaskReadBuf.LanguageType;
	FontType = g_tpCharMaskReadBuf.FontType;
	
	if (CodeType == LANGUAGE_ENGLISH)
	{
		Width = g_tPageModeCharPara.Width;
 		High  = g_tPageModeCharPara.Height;
		if (g_tPageModeCharPara.RotateType == 1)	//��ת90
		{
			CodeRollCtrl(Width, High, pSrc);
			Temp 	= Width;
			Width	= High;
			High 	= Temp;
		}
		else if (g_tPageModeCharPara.RotateType == 2)	//��ת180
		{
			//δ�������
		}
		
	}
	else if (CodeType == LANGUAGE_GB18030)
	{
		Width = g_tPageModeHZPara.Width;
 		High  = g_tPageModeHZPara.Height;
	}
	else
	{
		Width = g_tPageModeHZPara.Width;
 		High  = g_tPageModeHZPara.Height;
	}
	
	
	
	
	
	
}

/*******************************************************************************
* Function Name  : ������
* Description    : �Զ��ص��ַ������������ת���Ӵ֣����أ����ԣ��������Ŵ�
	���»��ߵĴ���
	������Ч�������
	1.�Ӵּ���,void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
	2.����,void ReverseDisplayProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
	3.��ת
	4.����Ŵ�
	5.����Ŵ�
	6.�»���
	�ַ������С8*16,9*17,12*24,16*16,24*24
	��24��24���ַ���ת�������ȻΪ24*24����
	8*16,16*16���������Ϊ0��䣬
	
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/


/*******************************************************************************
* Function Name  : ������
* Description    : ���Դ�����ָ�����24*24����ռ����ֵ����ָ�����ȺͿ�ȷ���
* Input          : ����������������ţ������ҵ�˳���ŷ�
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void ReverseDisplayProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt)
{
	uint32_t	i;
	
	
}

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/

#endif	/* #ifdef	CPCL_CMD_ENABLE ���� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
