/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ֽ����ȱֽ�����صĳ���.
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
#ifndef PNE_H
#define PNE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#ifndef	VREF
	#define	VREF						(3.3)				//AD�ο���ѹ
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE		(4095)			//ADת�����ֵ
#endif

#define	PNE_ADC												ADC1								//ֽ����ADC
#define	PNE_ADC_PORT									GPIOC								//ֽ����ADC�˿�
#define	PNE_ADC_PIN										GPIO_Pin_2					//ֽ����ADC�ܽ�
#define	PNE_ADC_CHANNEL								ADC_Channel_12			//ֽ����ADCͨ��
#define PNE_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* ����ֽ�������������ֵ�ѹ��Ӧ��ת��ֵ **********/
/*ʹ�ù�紫����,����LED��������470ŷ��VCC�����������ܼ��缫����100K�ӵ�VCC3.3V.*/
/*�����������������470ŷ,���������ܷ��伫�ӵ�.*/
/*�ź����Ϊ���缫*/
/*ֽ����ʱ��ѹֵ�ӽ�3V,ֽ����ʱ��ѹֵ�ӽ�0V*/
#define PNE_ADC_VALUE_LOW				(ADC_MAX_VALUE * 1.0 / VREF)
#define PNE_ADC_VALUE_MIDDLE		(ADC_MAX_VALUE * 1.6 / VREF)
#define PNE_ADC_VALUE_HIGH			(ADC_MAX_VALUE * 2.0 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitPaperNearEndPort(void);

#ifdef __cplusplus
}
#endif

#endif /* PNE_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
