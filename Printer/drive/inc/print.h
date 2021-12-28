/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   ��ӡ������صĳ���.
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
#ifndef PRINT_H
#define PRINT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
//#define	MAX_PRT_DOT_HEIGHT			(MAX_RATIO_Y * MAX_FONT_HEIGHT)	//����ӡ�߶ȵ���,��󱶸ߡ�24��
//#define	MAX_PRT_HEIGHT_BYTE			(MAX_PRT_DOT_HEIGHT / 8)				//����ӡ�߶��ֽ���,�粻��������Ҫ����
//#define BYTES_OF_LINE_PRT_BUF		(PRT_DOT_NUMBERS * MAX_PRT_HEIGHT_BYTE)	//�д�ӡ��������С
//#define BYTES_OF_LPB						BYTES_OF_LINE_PRT_BUF

//��о���ȵ���
//#define	DOTS_OF_ALL_ELEMENT										(640)
//#define	DOTS_OF_HEAT_ELEMENT									(576)
//#define	DOTS_OF_LEFT_EMPTY_HEAT_ELEMENT				(32)
//#define	DOTS_OF_RIGHT_EMPTY_HEAT_ELEMENT			(32)

//#define	BYTES_OF_ALL_ELEMENT									(76)			//2016.06.16 80��Ϊ76,608��
//#define	BYTES_OF_HEAT_ELEMENT									(72)
//#define	BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT			(0)				//2016.06.16 4��Ϊ0 
//#define	BYTES_OF_RIGHT_EMPTY_HEAT_ELEMENT			(4)

#define	LINE_PRT_BUF_NONE											(0)
#define	LINE_PRT_BUF_A												(1)
#define	LINE_PRT_BUF_B												(2)

#define	LINE_PRT_BUF_STATUS_ILDE							(0)
#define	LINE_PRT_BUF_STATUS_WRITING						(1)
#define	LINE_PRT_BUF_STATUS_WAIT_TO_PRT				(2)
#define	LINE_PRT_BUF_STATUS_START_PRT					(3)
#define	LINE_PRT_BUF_STATUS_SPI_SENDING				(4)
#define	LINE_PRT_BUF_STATUS_SPI_SEND_OVER			(5)
#define	LINE_PRT_BUF_STATUS_SET_GO_LINE_SPACE	(6)

#define	LPB_STATUS_ILDE								LINE_PRT_BUF_STATUS_ILDE
#define	LPB_STATUS_WRITING						LINE_PRT_BUF_STATUS_WRITING
#define	LPB_STATUS_WAIT_TO_PRT				LINE_PRT_BUF_STATUS_WAIT_TO_PRT
#define	LPB_STATUS_START_PRT					LINE_PRT_BUF_STATUS_START_PRT
#define	LPB_STATUS_SPI_SENDING				LINE_PRT_BUF_STATUS_SPI_SENDING
#define	LPB_STATUS_SPI_SEND_OVER			LINE_PRT_BUF_STATUS_SPI_SEND_OVER
#define	LPB_STATUS_SET_GO_LINE_SPACE	LINE_PRT_BUF_STATUS_SET_GO_LINE_SPACE

#define	UNDERLINE_PRINT_UNFINISHED						(0)
#define	UNDERLINE_PRINT_FINISHED							(1)

#define	LPB_CHECK_WAIT_TO_PRT_INTERVAL_US			(1)
#define	LPB_CHECK_START_PRT_INTERVAL_US				(1)
#define	LPB_CHECK_SEND_OVER_INTERVAL_US				(1)
#define	LPB_CHECK_SET_LINE_SPACE_INTERVAL_US	(1)

#define	LINE_PRT_BUF_CHECK_INTERVAL_US				(1)
#define	LPB_CHECK_INTERVAL_US									LINE_PRT_BUF_CHECK_INTERVAL_US

#define	LPB_PROCESS_STATUS_UNFINISH						(0)
#define	LPB_PROCESS_STATUS_FINISH							(1)

/********** ��ӡ����Ҫ�Ĳ������� ***********/
//Ŀǰ��ӡ��������Ҫʹ�õĲ���,Ŀǰ׼��ʹ��һ���д�ӡ������
//typedef struct
//{
//	uint16_t	PrtDataDotIndex;				//�м�������
//	uint16_t	CodeLineHigh;						//�ַ�ģʽ�µ��и߿���,�������,Ӱ�����м��	
//	uint16_t	PrtLeftLimit;						//��ӡ������߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
//	uint16_t	PrtRightLimit;					//��ӡ�����ұ߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
//	uint16_t	PrtWidth; 							//��ӡ���,ʵ��ʹ�õĴ�ӡ����
//	uint16_t	PrtLength;							//�ܴ�ӡ���= ��߾� + ��ӡ���,������ߵĿհ׺͹涨�Ĵ�ӡ����
//	uint16_t	MaxPrtDotHigh;					//�༭/��ӡ�����������Ի���Ϊ��׼����,���д�ӡ�����и߶���ߵĵ���ֵ
//	
//	uint8_t	PrtModeFlag;							//��ӡģʽ����,	0 = ��ͨ and 1 = ҳģʽ
//	uint8_t	BitImageFlag;							//λͼ��ӡģʽ��־
//	uint8_t	VariableBitImageFlag;			//�ɷŴ�λͼ��ӡģʽ��־
//	uint8_t	MaxLineHigh;							//һ������ߵ��ַ�/λͼ�ߴ�,һ���ַ������ַ����+�ַ��Ҽ��
//#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
//	uint8_t	MaxRatioX;								//һ����������Ŵ���
//#endif
//	uint8_t	MaxRatioY;								//һ�����������Ŵ���	
//	uint8_t	CodeAimMode;							//���뷽ʽ,0=�����,1=�м����,2=�Ҷ���
//	uint8_t	PrintDirection;						//��ӡ����
//#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
//	uint8_t	CharNotFlag;							//�ַ����ԵĿ���,����Ч
//#endif
//#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
//	uint8_t	EmphaSizeMode;
//#endif
//	uint8_t	CharCycle90;							//��ת90�ȴ�ӡ��־,=1��ʾ��ת90�ȴ�ӡ,=0��ʾ����ת��ӡ
//	uint8_t	CharHeightMagnification;	//�ַ�����Ŵ���
//	uint8_t	HZHeightMagnification;		//��������Ŵ���
//	uint8_t	PrintLineIndex;						//��ǰ��ӡ����������
//	uint8_t	DoUnderlineFlag;					//�ַ��»��߿���,1=on; 0= off
//	uint8_t	UnderlineMaxPrtHigh;			//һ�б༭�����г��ֵ��»�����ߵĵ���,��Χ0~3,�ݴ�ȷ����ӡʱ��ȡֵ	
//	uint8_t	UnderlinePrintIndex;			//��־�»��ߵ�����������̵ı���,����ʱ��0�����;����ʱ����ߵ�0
//	uint8_t	UnderLinePrtOverFlag;
//}TypeDef_StructPrtMode;

//#define	LINE_PRT_BUF_LEN		(BYTES_OF_LINE_PRT_BUF + PRT_DOT_NUMBERS)
//typedef	struct
//{
//	__IO uint8_t	Status;											//�д�ӡ��������״̬
//	TypeDef_StructPrtMode			PrtMode;	//�д�ӡ���Ʋ���
//	//���»��߻������ӡ���ݻ������ϲ�,���PRT_DOT_NUMBERS���ֽ���Ϊ�»���ʹ��
//	uint8_t	Buf[LINE_PRT_BUF_LEN];
//}TypeDef_StructLinePrtBuf;

#define	UNDER_LINE_PRT_FINISHED				(0)
#define	UNDER_LINE_PRT_UNFINISHED			(1)

/* Funtion Declare -----------------------------------------------------------*/
void	InitLinePrtBuf(void);
void	ClearLinePrtBuf(void);

void	InitHeatElementBuf(void);
void	InitVariableToStartNewLine(void);
void	SetLinePrtModeVariable(void);

void	CopyDotLineDataToHeatElementBuf(const uint8_t * pBuf);
void	DotLinePutDrvBuf(uint8_t * pBuf);
void	SendHeatElementBufToCore(void);
void	PrintOneDotLine(void);
void	PrintLineBuffer(void);
uint32_t	GetLPBProcessStatus(void);
uint32_t	GetPrintStatus(void);
void MaxSetp(void);
#ifdef __cplusplus
}
#endif

#endif /* PRINT_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
