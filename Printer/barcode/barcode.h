/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   һά������صĳ���.
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
#ifndef BARCODE_H
#define BARCODE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"ean.h"


/* Macro Definition ----------------------------------------------------------*/
/********** ����һά�������ȫ�ֱ����ṹ ***********/
typedef struct
{
	uint8_t	HRIPosition;				//HRI�Ĵ�ӡλ�ã�0=����ӡ,1=�����ϣ�2=������,3=��������, 
	uint8_t	HRIFont;						//HRI���� 0=12*24,1=9*17
	uint8_t	BarCodeThinWidth;		//����խ�����
	uint8_t	BarCodeThickWidth;	//����������
	uint8_t	BarCodeHigh;				//�����ӡ�߶�
}TypeDef_StructBarcodeCtrl;

/* Funtion Declare -----------------------------------------------------------*/
void	EanCheckCode(uint8_t *Inbuf, uint8_t Length);
uint16_t FormatOne(uint8_t *Dest, uint16_t Data ,uint8_t Blength);
uint16_t EnaFillToBuf(uint8_t *inbuf, uint8_t Length, uint8_t PreCode, uint8_t Tpye);
uint16_t FormatCodeUpc_A(uint8_t *inbuf);
uint16_t FormatCodeUpc_E(uint8_t *inbuf);
uint16_t FormatCodeEan13(uint8_t *inbuf);
uint16_t FormatCodeEan8(uint8_t *inbuf);
uint16_t FormatTwo(uint8_t *Dest, uint16_t Data ,uint8_t Blength);
uint16_t FormatCode39(uint8_t *inbuf);
uint16_t FormatItf25(uint8_t *inbuf);
uint16_t FormatCodabar(uint8_t *inbuf);
uint16_t FormatCode93(uint8_t *inbuf, uint8_t Length);
uint16_t FormatCode128(uint8_t *Inbuf, uint8_t Length);
uint8_t GetCode128Str(uint8_t *Codebuf, uint8_t *Hribuf, uint8_t Length);
uint8_t GetCodeBarStr(uint8_t *Inbuf, uint8_t Number,uint8_t Type);
void	PrintBar(uint8_t *HriStr, uint16_t Width);
void	PrintBarHRI(uint8_t *HriStr, uint16_t Width);
void	PrintBarBitmap(uint8_t *HriStr, uint16_t Width);

static uint16_t	GetStartColumnIndex(void);
static void	FillCodebarBufToHeatElementBuf(uint8_t * bBuf, uint8_t Lines);
static void	CodebarBufToHeatElementNormalDir(uint8_t * bBuf, uint8_t Lines);
static void	CodebarBufToHeatElementReverseDir(uint8_t * bBuf, uint8_t Lines);

#ifdef __cplusplus
}
#endif

#endif /* BARCODE_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
