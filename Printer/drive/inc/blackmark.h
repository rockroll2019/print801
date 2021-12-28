/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   �ڱ�����صĳ���.
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
#ifndef BLACKMARK_H
#define BLACKMARK_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//#define	BLACKMARK_ADC_USE_PE_ADC		//�ڱ���ֽ���������Ƿ����

#ifndef	VREF
	#define	VREF						(3.3)				//AD�ο���ѹ
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE		(4095)			//ADת�����ֵ
#endif

#define	BLACKMARK_ADC												ADC1						//�ڱ�ADC
#define	BLACKMARK_ADC_PORT									GPIOC						//�ڱ�ADC�˿�
#define	BLACKMARK_ADC_PIN										GPIO_Pin_1			//�ڱ�ADC�ܽ�
#define	BLACKMARK_ADC_CHANNEL								ADC_Channel_11	//�ڱ�ADCͨ��
#define BLACKMARK_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* ����ڱ괫�������ֵ�ѹ��Ӧ��ת��ֵ **********/
/* �ںڱ�λ��ʱBM��⵽�ߵ�ƽ������Ϊ2V���� */
/* ���ںڱ�λ��ʱ��⵽�͵�ƽ���ӽ�0.3V */
#define BLACKMARK_ADC_VALUE_LOW			(ADC_MAX_VALUE * 1.0 / VREF)
#define BLACKMARK_ADC_VALUE_MIDDLE	(ADC_MAX_VALUE * 1.6 / VREF)
#define BLACKMARK_ADC_VALUE_HIGH		(ADC_MAX_VALUE * 2.0 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitBlackMarkPort(void);

#ifdef __cplusplus
}
#endif

#endif /* BLACKMARK_H */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
