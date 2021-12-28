/**
******************************************************************************
  * @file    x.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CHARPROCESS_H
#define CHARPROCESS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"cominbuf.h"
#include	"monobyte.h"
#include	"esc.h"
#include	"fs.h"
#include	"gs.h"

/*	�ַ�������صĳ���������ַ��ķ��ࡢ�ɴ�ӡ�ַ��ķ�������Ӧ��ģ�Ķ�ȡ��
		��ģ�ķŴ�/��С/���±�/����/�Ӵ�/���صȴ������д�ӡ�������Ĵ�š�
		�д�ӡ�������ַ�ȡ��һ���С��û��Զ����ַ����»��ߵ����ݡ�
*/
/* Macro Definition ----------------------------------------------------------*/
/******* ���������ռ���ֽ��� *************/
#define BYTES_OF_ASCII_FONT_A		(36)		//12 X 24 �����ַ��ĵ��������ֽ���
#define BYTES_OF_ASCII_FONT_B		(27)		//9 X 17 �����ַ��ĵ��������ֽ���
#define BYTES_OF_HZ_FONT_A			(72)		//24 X 24 �����ַ��ĵ��������ֽ���
#define	MAX_BYTES_OF_CHAR				(BYTES_OF_HZ_FONT_A)
#define	MAX_BYTES_OF_HZ					(BYTES_OF_HZ_FONT_A)

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
	uint8_t	CharRBFlag;							//�ַ��ߵ�����ת180�㣩��ӡ���ƣ�����Ч
	uint8_t	CharNotFlag;						//�ַ����ԵĿ��ƣ�����Ч
	uint8_t	Underline;							//�ַ��»��߿���,bit7 = on/off; bit1~0 = �»�����ʽ������˫�ߣ�
}TypeDef_StructTypeSet;		//Print_Mode_Struct

typedef	struct
{
	uint8_t	Status;											//�ַ���ȡ������״̬
	uint8_t	LanguageType;								//�ַ�����:Ӣ��,����,BIG5
	uint8_t	FontType;										//��������:9*17,12*24,24*24
	TypeDef_StructTypeSet	structTypeSet;		//�����Ű���ر���ֵ
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

uint8_t		JudgeLineFull(uint8_t CodeType);
void	FillDataPrintBuff(uint8_t Data, uint32_t Width, uint32_t Line);
void	FillPrintBuff(uint8_t * Source, uint8_t CharWidth, uint8_t CharHigh, uint8_t RatioX, uint8_t CodeType);
void	FillPrint(uint8_t * Source, uint8_t CodeType);

void	CodeRollCtrl(uint8_t	CodeWidth, uint8_t CodeHigh, uint8_t SrcPrt[]);
void	EmphaSizeProcess(uint8_t CodeWidth, uint8_t CodeHigh, uint8_t * SrcPrt);
void	ByteZoomCtrl(uint8_t ZoomRate, uint8_t ByteData, uint8_t * DscPrt);
void	ZoomByteHeight(uint8_t ByteData, uint8_t ZoomRate, uint8_t * DscPrt);

void	GetAscIIFontADotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIFontBDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetAscIIDotData(uint8_t AscIICode, uint8_t DotData[]);

/* Private functions ---------------------------------------------------------*/
/* Funtion Declare -----------------------------------------------------------*/
void	DataProcess(uint8_t GetChar);	//��������ͨ�Ž��ջ�����������

#ifdef __cplusplus
}
#endif

#endif /* CHARPROCESS_H������� */

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
