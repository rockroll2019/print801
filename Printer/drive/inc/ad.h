/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-15
  * @brief   ADת����صĳ���.
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
#ifndef AD_H
#define AD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
/********* ����ADת������ **********/
#ifndef	VREF
	#define	VREF						(3.3)				//AD�ο���ѹ
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE		(4095)			//ADת�����ֵ
#endif
#ifdef	BLACKMARK_ADC_USE_PE_ADC	
	 #define	ADC_CHANNEL_NUMBER				(5)	//ADCͨ���� 
#else
	 #define	ADC_CHANNEL_NUMBER				(4)	//ADCͨ����
#endif
#define	ADC_CONVERT_SHIFT_NUMBER	(0)	//����ʹ����λѭ��ʵ��
//5��ͨ������һ��ת������,��Ҫ����16��
#define ADC_CONVERT_NUMBER				(1 << ADC_CONVERT_SHIFT_NUMBER)
#define	ADC_BUF_LEN								(ADC_CHANNEL_NUMBER * ADC_CONVERT_NUMBER)
#define	ADC_SOURCE								(ADC1)

/********** ����AD�ɼ�ȫ�ֱ����ṹ ***********/
typedef struct
{	
	volatile uint16_t	PaperEnd_ADC_Value;				//ȱֽ���AD����
	volatile uint16_t	Temperature_ADC_Value;		//��ӡ�¶�AD����
	volatile uint16_t	Power_ADC_Value;					//��Դ��ѹAD����	
	volatile uint16_t	PaperNearEnd_ADC_Value;		//ֽ�������AD����
	volatile uint16_t	BlackMark_ADC_Value;			//�ڱ�����
	volatile uint16_t	PaperTakeAway_ADC_Value;	//ֽ���Ƿ�ȡ�߼��AD����
	volatile uint16_t	ADBuffer[ADC_BUF_LEN];
}TypeDef_StructAD;

/* Funtion Declare -----------------------------------------------------------*/
void	InitADCPort(void);
void	OpenADC(void);			//��TIMER��ʹ�ñ�������ADת��
void	ADC_Config(void);
void	ADC_DMA_Config(void);
void	DMA1_Channel1_ADFunc(void);
uint16_t	GetADCValueOfPaperEnd(void);
uint16_t	GetADCValueOfTemperature(void);
uint16_t	GetADCValueOfPower(void);
uint16_t	GetADCValueOfPaperNearEnd(void);
uint16_t	GetADCValueOfBlackMark(void);
uint16_t	GetADCValueOfPaperTakeAway(void);

static void UpdateErrorStatusByADCResult(void);
static void AutoGoPaper(void);
#ifdef __cplusplus
}
#endif

#endif /* AD_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
