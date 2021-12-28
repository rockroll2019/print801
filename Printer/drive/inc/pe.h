/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ȱֽ�����صĳ���.
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
#ifndef PE_H
#define PE_H

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

#define	PE_ADC											ADC1								//ֽ��ADC 
#define	PE_ADC_PORT									GPIOB								//ֽ��ADC�˿�
#define	PE_ADC_PIN									GPIO_Pin_0					//ֽ��ADC�ܽ�
#define	PE_ADC_CHANNEL							ADC_Channel_8				//ֽ��ADCͨ��
#define PE_ADC_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOB
	 
//ѹ��ֽʱ,�����ѹ0.15V;����ֽѹ�½���ʱ,���2.9V;ȫ�հ�,���3.23V
//�ߵ�ƽʱΪȱֽ���͵�ƽΪ��ֽ
/********* �����ֽ���������ֵ�ѹ��Ӧ��ת��ֵ **********/
#define PE_ADC_VALUE_LOW					(ADC_MAX_VALUE * 0.6 / VREF)
#define PE_ADC_VALUE_MIDDLE				(ADC_MAX_VALUE * 1.6 / VREF)
#define PE_ADC_VALUE_HIGH					(ADC_MAX_VALUE * 2.0 / VREF)
#define	PE_ADC_VALUE_ONLY_PLATEN	(ADC_MAX_VALUE * 2.8 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitPaperEndPort(void);

#ifdef __cplusplus
}
#endif

#endif /* PE_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
