/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   ENAһά������صĳ���.
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
#ifndef EAN_H
#define EAN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
/* ��������˳�� */
#define	TYPE_UPCA									(0)
#define	TYPE_UPCE									(1)
#define	TYPE_EAN13								(2)
#define	TYPE_EAN8									(3)

#define	EAN13_DATA_MAX_LEN				(13)
#define	EAN8_DATA_MAX_LEN					(8)
#define	UPCA_DATA_MAX_LEN					(12)
#define	UPCE_DATA_MAX_LEN					(8)

#define	EAN13_LEFT_DATA_LEN				(6)
#define	EAN13_RIGHT_DATA_LEN			(5)
#define	EAN8_LEFT_DATA_LEN				(4)
#define	EAN8_RIGHT_DATA_LEN				(3)

#define	UPCE_PRT_DATA_LEN					(6)

#define	EAN_CODE_CONTENT_VALID_BIT_LEN	(7)
#define	EAN_START_VALID_BIT_LEN					(3)
#define	EAN_STOP_VALID_BIT_LEN					(3)
#define	EAN_MIDDLE_VALID_BIT_LEN				(5)

#define	UPC_CODE_CONTENT_VALID_BIT_LEN	(7)
#define	UPC_START_VALID_BIT_LEN					(3)
#define	UPCE_STOP_VALID_BIT_LEN					(6)

#define	EAN13_PRE_CODE_MIN				('0')
#define	EAN13_PRE_CODE_MAX				('9')
#define	IS_EAN13_PRE_CODE(PreCode)	((PreCode >= EAN13_PRE_CODE_MIN) \
																					&& (PreCode <= EAN13_PRE_CODE_MAX))

#define	EAN8_DEFAULT_PRE_CODE			('0')
#define	IS_EAN8_PRE_CODE(PreCode)	(PreCode == EAN8_DEFAULT_PRE_CODE)

#define	UPCA_DEFAULT_PRE_CODE			('0')
#define	IS_UPCA_PRE_CODE(PreCode)	(PreCode == UPCA_DEFAULT_PRE_CODE)

#define	UPCE_DEFAULT_PRE_CODE			('0')
#define	IS_UPCE_PRE_CODE(PreCode)	(PreCode == UPCE_DEFAULT_PRE_CODE)

#define	EAN_UPC_CODE_MIN					('0')
#define	EAN_UPC_CODE_MAX					('9')
#define	IS_EAN_UPC_CODE(Code)			((Code >= EAN_UPC_CODE_MIN) \
																					&& (Code <= EAN_UPC_CODE_MAX))
																					


/* Funtion Declare -----------------------------------------------------------*/
/*
	uint8_t	bBuf[]		uint8_t	bCodeType			uint8_t	bCodeLen / bCodePosition
	����Ϊһ���ַ�����, һ����ʾ��������ı���, һ������볤����صı���
	���һ��Ϊ������ı�׼�ַ�����	uint8_t	bstrCodeBuf[]
	���һ��Ϊ�����Ŀɴ�ӡ��������	uint8_t	bPrtData[]
	���ָ�����ȵı����Ƿ�Ϸ�
	����ָ���ַ����к������У����
	����ָ��У�������ָ���ַ����к�����ı���
	��ָ�����Ⱥ������EAN8�ַ�����ת��Ϊ��׼EAN13�ַ�����
	��ָ�����Ⱥ������UPC-E�ַ�����ת��Ϊ��׼��UPC-A�ַ�����
	����׼EAN13�ַ�����ת��Ϊ�ɴ�ӡ��������
	��EAN8�ַ�����ת��Ϊ�ɴ�ӡ��������
	����׼UPC-A�ַ�����ת��Ϊ�ɴ�ӡ��������
	��UPC-E�ַ�����ת��Ϊ�ɴ�ӡ��������
	��ĳ��ָ���ַ���ָ�����뼯��ȡ����䵽ָ���ɴ�ӡ������
	���ʣ���ӡ��仺�����Ƿ��㹻
	static int8_t	CheckCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen, uint8_t	bCodeType)
	uint8_t	CalCheckCode(uint8_t	bBuf[], uint8_t	bCodeLen, uint8_t	bCodeType)
	uint8_t	GetCheckCode(uint8_t	bBuf[], uint8_t	bCodeLen, uint8_t	bCodeType)
	void	EAN8ChangeToEAN13(uint8_t	bBuf[], uint8_t	bCodeLen)
	void	UPCAChangeToEAN13(uint8_t	bBuf[], uint8_t	bCodeLen)
	void	UPCEChangeToUPCA(uint8_t	bBuf[], uint8_t	bCodeLen)
	uint16_t	EAN13ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
	uint16_t	EAN8ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
	uint16_t	UPCAChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
	uint16_t	UPCEChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
	uint8_t	GetPrtDataFromCodeContentBuf(uint8_t	bBuf[], uint8_t	bCode)
	void	SetDataToPrtBuf(uint8_t	bBuf[], uint8_t	bCode, uint8_t	bBitNum)
*/


ErrorStatus	CheckEANUPCCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen);
uint8_t	CalEAN13CheckCode(uint8_t	bBuf[]);
void	SetEAN13CheckCode(uint8_t	bBuf[], uint8_t	bCode);
uint8_t	GetEAN13CheckCode(uint8_t	bBuf[]);
uint8_t	CheckPrtSpace(uint8_t bType, uint8_t bCodeLen);
uint16_t	EAN13ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[]);
uint8_t	GetPrtDataFromCodeContentBuf(const uint8_t	bBuf[], uint8_t	bCode);
void	SetOneBitToPrtBuf(uint8_t	bBuf[], uint8_t	bCode, uint32_t	nWidth);
uint32_t	SetOneByteToPrtBuf(uint8_t	bBuf[], uint8_t	bCode, uint8_t	bBitNum, uint32_t	nWidth);
uint16_t	FormatCodeEAN13(uint8_t	bBuf[], uint8_t	bCodeLen);
uint16_t	FormatCodeUPCA(uint8_t	bBuf[], uint8_t	bCodeLen);

uint8_t	CalEAN8CheckCode(uint8_t	bBuf[]);
void	SetEAN8CheckCode(uint8_t	bBuf[], uint8_t	bCode);
uint8_t	GetEAN8CheckCode(uint8_t	bBuf[]);
uint16_t	EAN8ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[]);
uint16_t	FormatCodeEAN8(uint8_t	bBuf[], uint8_t	bCodeLen);

void	UPCERestoreUPCA(uint8_t	bBuf[]);
void	SetUPCECheckCode(uint8_t	bBuf[], uint8_t	bCode);
uint8_t	GetUPCECheckCode(uint8_t	bBuf[]);
uint16_t	UPCEChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[]);
uint16_t	FormatCodeUPCE(uint8_t	bBuf[], uint8_t	bCodeLen);

#ifdef __cplusplus
}
#endif

#endif /* EAN_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
