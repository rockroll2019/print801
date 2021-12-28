/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHARMASKBUF_H
#define CHARMASKBUF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"print.h"

/*	�ַ�������صĳ���������ַ��ķ��ࡢ�ɴ�ӡ�ַ��ķ�������Ӧ��ģ�Ķ�ȡ��
		��ģ�ķŴ�/��С/���±�/����/�Ӵ�/���صȴ������д�ӡ�������Ĵ�š�
		�д�ӡ�������ַ�ȡ��һ���С��û��Զ����ַ����»��ߵ����ݡ�
*/
/* Macro Definition ----------------------------------------------------------*/
#define	LINE_BUF_NOT_FULL				(0)
#define	LINE_BUF_FULL						(1)

/*�ַ���ȡ������������״̬*/
#define	CHAR_MASK_READ_BUF_STATUS_ILDE						(0)		/*����,����д��*/
#define	CHAR_MASK_READ_BUF_STATUS_WRITING					(1)		/*������SPIд��*/
#define	CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL		(2)		/*д�����,�ȴ�����*/
#define	CMRB_STATUS_ILDE						CHAR_MASK_READ_BUF_STATUS_ILDE
#define	CMRB_STATUS_WRITING					CHAR_MASK_READ_BUF_STATUS_WRITING
#define	CMRB_STATUS_WAIT_TO_DEAL		CHAR_MASK_READ_BUF_STATUS_WAIT_TO_DEAL

/*�༭�д�ӡ��������״̬*/
#define	LINE_EDIT_BUF_STATUS_ILDE						(0)		/*����,����д��*/
#define	LINE_EDIT_BUF_STATUS_WRITING				(1)		/*���ڱ༭��*/
#define	LINE_EDIT_BUF_STATUS_WAIT_TO_PRINT	(2)		/*���д������,�ȴ�����*/
#define	LEB_STATUS_ILDE							LINE_EDIT_BUF_STATUS_ILDE
#define	LEB_STATUS_WRITING					LINE_EDIT_BUF_STATUS_WRITING
#define	LEB_STATUS_WAIT_TO_PRINT		LINE_EDIT_BUF_STATUS_WAIT_TO_PRINT

/********** �ַ��Ű����ȫ�ֱ��� ***********/
typedef struct
{
	uint8_t	CharWidth;							//�ַ����ֿ�
	uint8_t	CharHigh;								//�ַ���24��17
	uint8_t	HeightMagnification;		//����Ŵ���Ratio
	uint8_t	WidthMagnification;			//����Ŵ���Ratio
	uint8_t OneLineWidthFlag;				//���ڱ����־
	
	uint8_t	LeftSpace;							//����
	uint8_t	RightSpace;							//�Ҽ��
	uint8_t	CodeAimMode;						//�ַ�����ģʽ,0 = �����, 1 = �м����, 2 = �Ҷ���
	
	uint8_t	EmphaSizeMode;					//�Ӵ�ģʽ
	uint8_t	CodeInversion;					//���ô�ӡ����,0 = normal Mode ; 1= Inversion Mode
	uint8_t	CharCycle90;						//�ַ���ת90�����, ����Ч
	uint8_t	CharRBFlag;							//�ַ��ߵ�����ת180�㣩��ӡ����,����Ч
	uint8_t	CharNotFlag;						//�ַ����ԵĿ���,����Ч
	uint8_t	Underline;							//�ַ��»��߿���,bit3 = on/off; bit1~0 = �»�����ʽ������˫�ߣ�
}TypeDef_StructTypeSet;		//Print_Mode_Struct

typedef	struct
{
	uint8_t	Status;											//�ַ���ȡ������״̬
	uint8_t	LanguageType;								//�ַ�����:Ӣ��,����,BIG5
	uint8_t	FontType;										//��������:9*17,12*24,24*24	
	uint8_t	Buf[MAX_BYTES_OF_CHAR];
}TypeDef_StructCharMaskReadBuf;

/* Private function prototypes -----------------------------------------------*/
void	PrintChar(uint8_t CharCode);
void	PrintAscII(uint8_t Code);
void	PrintBIG5(uint8_t CodeHigh, uint8_t CodeLow);
void	PrintGB18030(uint8_t Code1, uint8_t Code2, uint8_t Code3, uint8_t Code4);
void	PrintUserHZ(uint8_t CodeHigh, uint8_t CodeLow);

void	JudgePrintSelfDefChinese(uint8_t Ch1);
void	JudgePrintBIG5(uint8_t Ch1);
void	JudgePrintGB18030(uint8_t Ch1);

void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line);
void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t CodeType);
//void	FillPrintBuff(uint8_t *Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType);

void	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[]);
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t *SrcPrt);
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t *DscPrt);
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt);

void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIUserDefDotData(uint8_t AscIICode);

uint32_t	GetUSACharacterStartAddress(uint8_t AscIICode);
uint32_t	GetInternationalCharacterStartAddress(uint8_t	AscIICode);
uint32_t	GetCodepageCharacterStartAddress(uint8_t	AscIICode);

/*֧����������*/
uint32_t	GetUSACharStartAddress(uint8_t	AscIICode, uint8_t FontType);
uint32_t	GetInternationalCharStartAddress(uint8_t	AscIICode, uint8_t FontType);
uint32_t	GetCodepageCharStartAddress(uint8_t	AscIICode, uint8_t FontType);

void	GetAscIIFlashDotData(uint8_t AscIICode);

/* Funtion Declare -----------------------------------------------------------*/
uint32_t	GetPrintBusyStatus(void);
uint32_t	GetPrintProcessStatus(void);
uint32_t	GetFeedPaperStatus(void);

void	InitCharMaskBufParameter(void);
void	PrintOneLine(void);
void	ProcessCharMaskReadBuf(void);
void	DataProcess(uint8_t GetChar);	//��������ͨ�Ž��ջ�����������
void	UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);	//�ϴ�����
void	CopyEditDataToLinePrtStruct(void);
void	PrepareNewLine(void);
void	PrintString(const uint8_t * Str);
void	FlashDecryption(uint8_t	* Src, uint32_t ByteNum);

static uint8_t	SelectLineBufToEdit(void);
static uint8_t	GetRatioXValue(uint8_t CodeType);
uint8_t	JudgeLineBufFull(uint8_t CodeType);
void	FillLinePrintBuf(uint8_t *Source, uint8_t CodeType);

static void	ProcessCharMaskReadBufIlde(void);
static void	ProcessCharMaskReadBufEdit(void);

void MovDotLine (u8 *VBuf, u8 LineID);             //2016.06.28	
void PrtOneLineToBuf(uint32_t LineHigh);
void PrtAsciiToBuf(uint8_t Code);
#ifdef __cplusplus
}
#endif

#endif /* CHARMASKBUF_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
