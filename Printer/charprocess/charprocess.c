/**
  ******************************************************************************
  * @file    x.c
  * @author  WH Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   �ַ�������صĳ���.
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
  * <h2><center>&copy; COPYRIGHT 2012 WH</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"charprocess.h"
#include	"extgvar.h"
//#include	"charmaskbuf.h"

extern TypeDef_StructInBuffer	gstructInBuf;
extern TypeDef_StructCharMaskReadBuf	gstructCharMaskReadBuf;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//���滻�Ĺ����ַ���
uc8  CharacterSet[] = 
{
	0x23,0x24,0x40,0x5b,0x5c,0x5d,0x5e,0x60,0x7b,0x7c,0x7d,0x7e
};

#define	BYTES_OF_CHAR_SET_TABLE		(sizeof(CharacterSet) / sizeof(CharacterSet[0]))

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ���ݴ������,�Ǵ�ӡ�ַ������ӡ,�����������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	DataProcess(uint8_t GetChar)
{
	while (SystemCtrlStruct.PrtDevOe == 1) 	//��ӡ������ֹ
	{
		Command_1B3D();						//��ֹ/�����ӡ��
	}
	
	if (SystemCtrlStruct.PrtModeFlag == 0)	//��׼ģʽ
	{
		if (GetChar >= 0x20)
		{
			PrintChar(GetChar);		   //�ɴ�ӡ�ַ�����jsf����һ���޸ĸú���
		}
		else
		{
			if (GetChar == 0x1B)
			{
				ESC_Command();		  //ESC��1B��ָ���
			}
			else if (GetChar == 0x1C)
			{
				FS_Command();				//FS��1C��ָ���
			}
			else if (GetChar == 0x1D)
			{
				GS_Command();			//GS��1D��ָ���
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
	uint8_t	Ch1;
	uint8_t	*p;
	uint32_t	i;
	Ch1 = CharCode;
	
	if (SysConfigStruct.SYS_Language == LANGUAGE_ENGLISH)		//����ģʽ
  {
  	PrintAscII(Ch1);			
	}	//���Ĵ������
  else										//����ģʽ
	{
  	if (Ch1 == 0xFE)			//�Զ��庺��
		{
			JudgePrintSelfDefChinese(Ch1);	//�����Զ��庺�ִ�ӡ
		}
		else if (SysConfigStruct.SYS_Language == LANGUAGE_BIG5)	//BIG5���ַ�ʽ
  	{
			JudgePrintBIG5(Ch1);			//����BIG5���ִ�ӡ
		}
		else								//GB18030
		{
			JudgePrintGB18030(Ch1);		//����GB18030��ӡ  	
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
	uint8_t	*p;
	uint32_t	i;
	
	Ch2 = ReadInBuffer();
	if ((Ch2 > 0xA0) && (Ch2 < (0xA1 + USERHZSIZE)))
	{
		p = RamBuffer.UserCharStruct.UserCode_HZ;	//�û����庺�ֵ��ֽڴ����
		for (i = 0; i < USERHZSIZE; i++, p++)
		{
			if (Ch2 == *p)
				break;
		}
		if (i < USERHZSIZE)
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
		if ((Ch2 < 0x40) || (Ch2 > 0x7E && Ch2 < 0xA1) || (Ch2 > 0xFE))	//�ڶ��ֽ�Ϊ��Ч���ִ���
		{
			PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
			DecInBufferGetPoint();		//�Ż�����ȡ���ַ�
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
	
	if ((Ch1 < 0x81) || (Ch1 > 0xFE))		//��һ����Ϊ��Ч���ִ���
	{
		PrintAscII(Ch1);				//��һ�ֽ�Ϊ�����ַ���ӡ
	}
	else
	{
		Ch2 = ReadInBuffer();
		if ((Ch2 < 0x30) || (Ch2 > 0x39 && Ch2 < 0x40) || (Ch2 == 0x7F) || (Ch2 > 0xFE))	//��Ч���ִ���
		{
			PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
			DecInBufferGetPoint();		//�Ż�����ȡ���ַ�jsf���޸�
		}
		else if ((Ch2 >= 0x40 && Ch2 <= 0x7E) || (Ch2 >= 0x80 && Ch2 <= 0xFE))
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
				DecInBufferGetPoint();			//�Ż�����ȡ���ַ�
			}
			else
			{
				Ch4 = ReadInBuffer();
				if ((Ch4 < 0x30) || (Ch4 > 0x39))	//�����ֽڴ�����Ч
				{
					PrintAscII(Ch1);			//��һ�ֽڰ������ַ���ӡ
					PrintAscII(Ch2);			//�ڶ��ֽڰ������ַ���ӡ
					PrintAscII(Ch3);			//�����ֽ�Ϊ��Ч��ӡ�ַ�
					DecInBufferGetPoint();		//�Ż�����ȡ���ַ�
				}
				else
				{
					PrintGB18030(Ch1, Ch2, Ch3, Ch4);		//���ֽں���
				}
			}
		}
	}
}

/*******************************************************************************
* Function Name  : PrintAscII
* Description    : �����ӡ���ܷ��µ�ǰASCII,��ȡASCII�ַ���ģ����ӡ������,����,
*	��ӡ��ǰ��.
* Input          : ASCII�ַ�����
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
#if	0
//ԭ���̵Ĵ���ʽ
void	PrintAscII(uint8_t Code)
{
	uint8_t	TempBuff[MAX_BYTES_OF_CHAR];
	
	GetAscIIDotData(Code, TempBuff);
	
	FillPrint(TempBuff, LANGUAGE_ENGLISH);	
}
#endif
//����POS���̵Ĵ���ʽ
//
//�������ַ���SPI_DMA��ȡ
//����ǰ�ַ������������õ��ַ���ȡ�������ṹ����
void	PrintAscII(uint8_t Code)
{
	uint8_t	TempBuff[MAX_BYTES_OF_CHAR];
	
	GetAscIIDotData(Code, TempBuff);
	
	FillPrint(TempBuff, LANGUAGE_ENGLISH);	
}
/*******************************************************************************
* Function Name  : PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : �����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,�����ӡ��ǰ��.
* Input          : CodeHigh=���ִ����λ,CodeLow=���ִ����λ
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	Offset, TempBuff[MAX_BYTES_OF_HZ];
	uint32_t	CodeAddr;
	
	if ((CodeLow >= 0x40) && (CodeLow <= 0x7E))
		Offset = CodeLow - 0x40;
	else
		Offset = CodeLow - 0xA1 + 0x3F;
	CodeAddr = BIG5_ADDR + ((CodeHigh - 0xA1) * 0x9D + Offset) * BYTES_OF_HZ_FONT_A;
	
	sFLASH_ReadBuffer(TempBuff, CodeAddr, BYTES_OF_HZ_FONT_A);
	
	FillPrint(TempBuff, LANGUAGE_CHINESE);
}

/*******************************************************************************
* Function Name  : void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
* Description    : �����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,����,��ӡ��ǰ��.
*	
* Input          : Code1��Code2��Code3��Code4Ϊ���ִ���,��Code3=Code4=0ʱΪ˫�ֽڴ���
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4)
{
	uint8_t	TempBuff[MAX_BYTES_OF_HZ];
	uint32_t	CodeAddr;
	
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
				Code2--;
			CodeAddr = (Code1 - 0xA8) * 96 + (Code2 - 0x40) + 846;
		}		
		else if ((Code1 >= 0xB0) && (Code1 <= 0xF7) && (Code2 >= 0xA1))	//2������
		{
			CodeAddr = (Code1 - 0xB0) * 94 + (Code2 - 0xA1) + 1038;
		}
		else if ((Code1 < 0xA1) && (Code1 >= 0x81))					//3������
		{
			if (Code2 > 0x7F)
				Code2--;
			CodeAddr = (Code1 - 0x81) * 190 + (Code2 - 0x40) + 1038 + 6768;
		}
		else if ((Code1 >= 0xAA) && (Code2 < 0xA1))					//4������
		{
			if (Code2 > 0x7F)
				Code2--;
			CodeAddr = (Code1 - 0xAA) * 96 + (Code2 - 0x40) + 1038 + 12848;
		}
		else
			CodeAddr = 0;
	}
	else
	{
		CodeAddr =	(Code1 - 0x81) * 12600 + 
								(Code2 - 0x39) * 1260 + 
								(Code3 - 0xEE) * 10 + 
								(Code4 - 0x39) + 
								22046;
	}
	
	CodeAddr = GB18030_ADDR + CodeAddr * BYTES_OF_HZ_FONT_A;
	
	//��ȡ��ģ
	sFLASH_ReadBuffer(TempBuff, CodeAddr, BYTES_OF_HZ_FONT_A);

	FillPrint(TempBuff, LANGUAGE_CHINESE);
}

/*******************************************************************************
* Function Name  : PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
* Description    : ��ӡ�Զ��庺��,�����ӡ���ܷ��µ�ǰ����,��ȡ������ģ����ӡ������,
*				   ����,��ӡ��ǰ��.
* Input          : CodeHigh=���ִ����λ,CodeLow=���ִ����λ
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow)
{
	uint8_t	i, TempBuff[MAX_BYTES_OF_HZ], *pZimo;
	
	//�û����庺����ģ���׵�ַ
	pZimo = RamBuffer.UserCharStruct.Buffer_HZ + (CodeLow - 0xA1) * BYTES_OF_HZ_FONT_A;	
	for (i = 0; i < BYTES_OF_HZ_FONT_A; i++, pZimo++)
	{
		TempBuff[i] = *pZimo;
	}
	
	FillPrint(TempBuff, LANGUAGE_CHINESE);
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
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII�ַ����������
{
	if (SysConfigStruct.SYS_FontSize == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		GetAscIIFontADotData(AscIICode, DotData);
	}
	else if (SysConfigStruct.SYS_FontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		GetAscIIFontBDotData(AscIICode, DotData);
	}
}


void	GetAsciiUserDefDotData(uint8_t AscIICode)	//��ȡ�û��Զ����ַ��ĵ�������
{
	uint32_t	i;
	uint32_t	len;
	uint32_t	StartAddress;	
	uint8_t * p;
	
	if (SysConfigStruct.SYS_FontSize == ASCII_FONT_A)				//FLASH��Ӣ��12x24�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_A;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A;
		p = RamBuffer.UserCharStruct.Buffer_12;
	}
	else if (SysConfigStruct.SYS_FontSize == ASCII_FONT_B)	//FLASH��Ӣ��9x17�����ASCII�ַ�
	{
		len = BYTES_OF_ASCII_FONT_B;
		StartAddress = (AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B;
		p = RamBuffer.UserCharStruct.Buffer_09;
	}
	else
	{
		return;
	}
	
	gstructCharMaskReadBuf.Status = CHAR_MASK_READ_BUF_STATUS_WRITING;	//��Ϊ����д��
	for (i = 0; i < len; i++)
	{
		gstructCharMaskReadBuf.Buf[i++] = *p++;
	}
	gstructCharMaskReadBuf.Status = CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL;	//��Ϊ�ȴ�����
}

/*******************************************************************************
* Function Name  : void GetAscIIFontBDotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������9*17
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII�ַ����������
{
	uint8_t	i, CodePage;
	uint32_t	AsciiCodeAddr = 0;
	
	CodePage = SysConfigStruct.DefaultCodePage;		 //���ҵĴ���ҳ��ѡ��
	
	if (AscIICode < 0x80)
	{
		if ((RamBuffer.UserCharStruct.UserdefineFlag == 1) && 
				(SystemCtrlStruct.UserSelectFlag) && 
				(RamBuffer.UserCharStruct.UserCode_09[AscIICode - 0x20]))	//�������Զ�����ַ�,��ѡ����
		{
			for (i = 0; i < BYTES_OF_ASCII_FONT_B; i++)
				DotData[i] = RamBuffer.UserCharStruct.Buffer_09[(AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B + i];
		}
		else	//�����Զ����ַ�
		{
			if (SystemCtrlStruct.CharacterSet)	//��������˹����ַ���ָ�� 0x1b 52 n
			{
				for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//���Ƿ�Ϊ�����ַ�
				{
					if (AscIICode == CharacterSet[i])
						break;
				}
				if (i < BYTES_OF_CHAR_SET_TABLE)							//��Ҫ�滻���ַ�����
				{
					AsciiCodeAddr = INTERNATION_09_17_ADDR + 
						(SystemCtrlStruct.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_B;
				}
				else
				{
					AsciiCodeAddr = ASCII_09_17_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B);
				}
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);
			}		//End �����ַ�
			else
			{
				AsciiCodeAddr = ASCII_09_17_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_B);
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);
			}
		}		//End �Զ����ַ�
	}		   //End <0x80
	else		//----FLASH��0X80-0xFF �� 9 x 17 �����ASCII�ַ�----//
	{
		AsciiCodeAddr = ASCII_09_17_CODEPAGEADDR + 
										(CodePage * 128 * BYTES_OF_ASCII_FONT_B) + 
										((AscIICode - 0x80) * BYTES_OF_ASCII_FONT_B);
		sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_B);	
	}
}

/*******************************************************************************
* Function Name  : void GetAscIIFontADotData(uint8_t AscIICode,uint8_t DotData[])
* Description    : ȡ�ַ���ģ����,����ø���ASCII�ַ��ĵ����ַ
*	AscIICode = ASCII�ַ�ֵ
*	DotData[] = �洢ASCII�ַ�����ģ����
*	��ǰ����������12*24
* Output         : 
* Return         : 
*******************************************************************************/
void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[])	//ASCII�ַ����������
{
	uint8_t	i, CodePage;
	uint32_t	AsciiCodeAddr = 0;

	CodePage = SysConfigStruct.DefaultCodePage;		 //���ҵĴ���ҳ��ѡ��
	
	if (AscIICode < 0x80)
	{
		if ((RamBuffer.UserCharStruct.UserdefineFlag == 1) && 
				(SystemCtrlStruct.UserSelectFlag) && 
				(RamBuffer.UserCharStruct.UserCode_12[AscIICode - 0x20]))	  //�������Զ�����ַ�,��ѡ����
		{
			for (i = 0; i < BYTES_OF_ASCII_FONT_A; i++)
			{
				DotData[i] = RamBuffer.UserCharStruct.Buffer_12[(AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A + i];
			}
		}
		else	//�����Զ����ַ�
		{
			if (SystemCtrlStruct.CharacterSet)		//��������˹����ַ���ָ�� 0x1b 52 n
			{
				for (i = 0; i < BYTES_OF_CHAR_SET_TABLE; i++)	//���Ƿ�Ϊ�����ַ�
				{
					if (AscIICode == CharacterSet[i])
						break;
				}
				if (i < BYTES_OF_CHAR_SET_TABLE)							//��Ҫ�滻���ַ�����
				{
					AsciiCodeAddr = INTERNATION_12_24_ADDR + 
						(SystemCtrlStruct.CharacterSet * BYTES_OF_CHAR_SET_TABLE + i) * BYTES_OF_ASCII_FONT_A;
				}
				else
				{
					AsciiCodeAddr = ASCII_12_24_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A);
				}
				
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
			}	//End �����ַ�
			else	//ascii�ַ�
			{
				AsciiCodeAddr = ASCII_12_24_CHI_ADDR + ((AscIICode - 0x20) * BYTES_OF_ASCII_FONT_A);
				sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
			}	//end ascii�ַ�
		}		//End �Զ����ַ�
	}		  //End <0x80
	else
	{	//----FLASH��0X80-0xFF �� 12 x 24 �����ASCII�ַ�----//
		AsciiCodeAddr = ASCII_12_24_CODEPAGEADDR + 
										(CodePage * 128 * BYTES_OF_ASCII_FONT_A) + 
										((AscIICode - 0x80) * BYTES_OF_ASCII_FONT_A);
		sFLASH_ReadBuffer(DotData, AsciiCodeAddr, BYTES_OF_ASCII_FONT_A);
	}
}

/*******************************************************************************
* Function Name  : void FillPrint(uint8_t *Source, uint8_t CodeType)
* Description    : �����ӡ���ܷ��µ�ǰ�ַ�,��ȡ������ģ����ӡ������,
*				   ����,��ӡ��ǰ��.
* Input          : Source����ģ��ַ,CodeType����������,0������
* Output         : �޸Ĵ�ӡ��������ǰָ��
* Return         : None
*******************************************************************************/
void	FillPrint(uint8_t *Source, uint8_t CodeType)
{
	uint8_t	Width, High, RatioX, Temp;
	
	RatioX = JudgeLineFull(CodeType);
	
	if (CodeType)
	{
		Width = PrtModeHZ.CharWidth;
 		High  = PrtModeHZ.CharWidth;
	}
	else
	{
	 	Width = PrtCtrlData.CharWidth;
		High  = PrtCtrlData.CharHigh;
	}
	
	if (PrtCtrlData.CharCycle90 == 1)	 				//��ת90��
	{
		CodeRollCtrl(Width, High, Source);
		Temp 	= Width;
		Width	= High;
		High 	= Temp;
	}
	if (PrtModeChar.EmphaSizeMode == 1)
	{
		EmphaSizeProcess(Width, 3, Source);
	}
	
	FillPrintBuff(Source, Width, High, RatioX, CodeType);
}

/*******************************************************************************
* Function Name  : JudgeLineFull
* Description    : �ж��Ƿ��ܷ��¸��ַ�,����,��ӡ��ǰ��.
*				   
* Input          : CodeType: =0,����,=1����
* Output         : 
* Return         : ���غ���Ŵ���
*******************************************************************************/
uint8_t	JudgeLineFull(uint8_t CodeType)
{
	uint8_t	RatioX;
	uint16_t	Width;
	
	if (CodeType)	//���ַ�ʽ,ȡ����Ŵ���
	{
		if ((PrtModeHZ.WidthMagnification == 1) && (PrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = PrtModeHZ.WidthMagnification;
		}
		Width = RatioX * (PrtModeHZ.CharWidth + PrtModeHZ.LeftSpace);
	}
	else					//�ַ���ʽ
	{
		if ((PrtModeChar.WidthMagnification == 1) && (PrtCtrlData.OneLineWidthFlag == 1))	//���ڱ���
		{
			RatioX = 2;
		}
		else
		{
			RatioX = PrtModeChar.WidthMagnification;
		}
		Width = RatioX * PrtModeChar.CharWidth;
	}
	
	//���������,��֤���õ���߾������ܴ�ӡһ�ַ�
	if ((PrtCtrlData.PrtDataDotIndex == PrtCtrlData.PrtLeftLimit) && 
			((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength))
		PrtCtrlData.PrtDataDotIndex = PrtCtrlData.PrtLength - Width;
	
	if ((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength)
	{
		MoveLine(PrtCtrlData.CodeLineHigh);
		//���������,��֤���õ���߾������ܴ�ӡһ�ַ�
		if ((PrtCtrlData.PrtDataDotIndex + Width) > PrtCtrlData.PrtLength)
			PrtCtrlData.PrtDataDotIndex = PrtCtrlData.PrtLength - Width;
	}
	
	return RatioX;
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
	uint8_t	*p;

	for (i = 0; i < Line; i++)
	{
		p = PrtDataBuf + i * PRTDOTNUMBER + PrtCtrlData.PrtDataDotIndex;
		for (j = 0; j < Width; j++)
		{
			*p = Data;
			p++;
		}
	}
}

/*******************************************************************************
* Function Name  : FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t RatioX ,uint8_t CodeType)
* Description    : ָ����ַ���ݴ����ӡ������.
* Input          : *Source���������ݵ�ַ, CharWidth���ַ����, RatioX:������, 
*										CodeType: �ַ����� =1����,=0�ַ�
* Output         : �ı����ָ��
* Return         : 
*******************************************************************************/
void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType)
{
	uint8_t	Line;
	uint8_t	Width, RatioY, Ratio, RightSpace, LeftSpace;
	uint8_t	*p, i, Temp;
	uint8_t	TempBuff[8];
	
	//�ַ����Ҽ��,����Ŵ�����
	if (SystemCtrlStruct.PrtModeFlag == 0)			//��ģʽ
	{
		if (CodeType == LANGUAGE_ENGLISH) 		//�ַ�
		{
			LeftSpace		= RatioX * PrtModeChar.LeftSpace;
			RightSpace	= RatioX *PrtModeChar.RightSpace;
			RatioY			= PrtModeChar.HeightMagnification;
		}
		else		//����
		{	LeftSpace		= RatioX * PrtModeHZ.LeftSpace;
			RightSpace	= RatioX * PrtModeHZ.RightSpace;
			RatioY			= PrtModeHZ.HeightMagnification;
		}
	}
	else			//ҳģʽ
	{
		if (CodeType == 0) 		//�ַ�
		{
			LeftSpace	 = 0;
			RightSpace = RatioX * PageModeStruct.AscRightSpace;
			RatioY		 = PrtModeChar.HeightMagnification;
		}
		else		//����
		{
			LeftSpace	 = RatioX * PageModeStruct.HZLeftSpace;
			RightSpace = RatioX * PageModeStruct.HZRightSpace;
			RatioY		 = PrtModeHZ.HeightMagnification;
		}
	}
	//�����ַ��߶�
	if (PrtCtrlData.LineHigh < CharHigh * RatioY)
		PrtCtrlData.LineHigh = CharHigh * RatioY;
	
	//�����Ҽ����,��֤�ܷ��¸��ַ�,�ַ����Ҽ�� < �ɴ�ӡ��
	if ((PrtCtrlData.PrtDataDotIndex + (RatioX * CharWidth) + LeftSpace + RightSpace) > PrtCtrlData.PrtLength)
		RightSpace = PrtCtrlData.PrtLength - (PrtCtrlData.PrtDataDotIndex + (RatioX * CharWidth) + LeftSpace);

	//��������
	
	if (PrtCtrlData.CharNotFlag == 1)		//���״�ӡ�����ԣ�
	{
		FillDataPrintBuff(0xff, LeftSpace, 3 * RatioY);
	}
	
	PrtCtrlData.PrtDataDotIndex	+= LeftSpace;
	
	for (Line = 0; Line < 3; Line++)
	{
		p = PrtDataBuf + ((RatioY * Line * PRTDOTNUMBER) + PrtCtrlData.PrtDataDotIndex);
		
		for (Width = 0; Width < CharWidth; Width++)		//�ַ���
		{
			Temp = Source[3 * Width + 2 - Line];
			ByteZoomCtrl(RatioY, Temp, TempBuff);				//����Ŵ���
			
			for (Ratio = 0; Ratio < RatioX; Ratio++)
			{
				for (i = 0; i < RatioY; i++)
				{
					if (PrtCtrlData.CharNotFlag == 0)
						*(p + PRTDOTNUMBER * i) = TempBuff[RatioY - i - 1];
					else
						*(p + PRTDOTNUMBER * i) = ~TempBuff[RatioY - i - 1];	//���״�ӡ
				}		//End ����Ŵ���
				p++;
			}			//End ����Ŵ���
		}			 	//Een �ַ���
	}				 	//End �ַ���
	
	//�»������
	if (PrtCtrlData.CharNotFlag == 0 && PrtCtrlData.CharCycle90 == 0)	//���ԡ���ת90��ӡʱ����ӡ�»���
	{
		if (((CodeType == 0) && ((PrtModeChar.Underline & 0x80) != 0)) || 
				((CodeType != 0) && ((PrtModeHZ.Underline & 0x80) != 0)))		//����»���
		{
			p = UnderlineBuf + PrtCtrlData.PrtDataDotIndex - LeftSpace;
			if (CodeType == 0)	  //�ַ�
			{
				Temp = PrtModeChar.Underline & 0x03;
			}
			else				  //����
			{
				Temp = PrtModeHZ.Underline & 0x03;
			}
			
			PrtCtrlData.DoUnderlineFlag	= 1;
			if (PrtCtrlData.UnderlineMaxPrtHigh < Temp)
				PrtCtrlData.UnderlineMaxPrtHigh = Temp;
			
			for (Width = 0; Width < (RatioX * CharWidth + LeftSpace + RightSpace); Width++)
			{
				*p = Temp;
				p++;
			}
		}
	}
	
	PrtCtrlData.PrtDataDotIndex += (RatioX * CharWidth);
	
	//�����Ҽ��
	if (PrtCtrlData.CharNotFlag == 1)		//�����״�ӡ�����ԣ�
	{
		FillDataPrintBuff(0xff, RightSpace, 3 * RatioY);
	}
	PrtCtrlData.PrtDataDotIndex += RightSpace;
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
		uint8_t	Buff[sizeof(unsigned long long)];
	}Temp_Union;
	
	Temp_Union	TempUnion;
	
	if (ZoomRate > MAX_RATIO_Y)	//�жϷŴ����Ƿ񳬹������ֵ
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
		Temp1 = 0xff >> (8 - ZoomRate);
		TempUnion.Temp = 0;
		for (i = 0; i < 8; i++)
		{
			if (Temp0 & 0x01)
				TempUnion.Temp |= Temp1 ;
			Temp1 <<= ZoomRate;
			Temp0 >>= 1;
		}
		
		for (i = 0; i < ZoomRate; i++)
			DscPrt[i] = TempUnion.Buff[ZoomRate - 1 - i];
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
			DscPrt[i] = (uint8_t)(temp >> (8*(ZoomRate - i - 1)));
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
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t *SrcPrt)
{
	uint8_t	ArrayBuf,DataBuf;
	uint8_t	i,j;
	
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
* Description    : ���ƶ��ַ�����ģ������ת˳ʱ��90��ĳ���
* Input          : CodeWidth���ַ�����ߴ�, ʹ�õ�Ϊ��λ
* Input          : CodeHigh���ַ�����ߴ�,ʹ�õ�Ϊ��λ
*				   SrcPrt[]����������ָ��
* Output         : SrcPrt[]����ת����������
* Return         : ��ת�������������� 
* �ر���ʾ��	 ���������ݰ���׼����
				   ʹ�� 9*17 �� 12*24 �����ַ�����ת����Ϊ90��ʱ.��������ֽڻ���������ֽ�,�ڵ���ʱ,ע�Ᵽ֤SrcPrt[]�Ĵ�С
				   ���ײ����뷽ʽ�����������
*	�д�����
*******************************************************************************/
void	CodeRollCtrl(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[])
{
	uint32_t	ChangedByte;
	uint8_t 	i, j;
	uint8_t	*ptr;
	uint8_t	Buff[MAX_BYTES_OF_HZ], Buff1[MAX_BYTES_OF_HZ];
	uint8_t	HighByte;
	
  HighByte = CodeHigh / 8;
	if (CodeHigh % 8)
		HighByte++;
	
	ptr = Buff1;
  MemSet(Buff1, 0x00, MAX_BYTES_OF_HZ);
  MemSet(Buff, 0x00, MAX_BYTES_OF_HZ);
	
	for (i=0; i < CodeWidth * HighByte; i++)
		Buff[i] = SrcPrt[i];
	
	HighByte--;
	//˳ʱ��90��
 	for (i=0; i < CodeHigh; i++)
	{
		ChangedByte = 0;
		for (j = 0; j < CodeWidth; j++)
		{
			ChangedByte <<= 1;
			//�ȰѺ����������ֽڵĵ�λȡ��������ת����ĵ�λ��
			if (Buff[HighByte - i / 8 + j * 3] & (0x01 << (i % 8)))
				ChangedByte |= 0x01;
		}
		
		*ptr++ = (ChangedByte >> 16);
		*ptr++ = (ChangedByte >> 8);
		*ptr++ = ChangedByte;
	}
	memcpy(SrcPrt, Buff1, MAX_BYTES_OF_HZ);
}

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
