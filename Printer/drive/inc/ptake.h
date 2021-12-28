/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-1
  * @brief   ��ֽ�Ƿ�ȡ�߼����صĳ���.
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
#ifndef PTAKE_H
#define PTAKE_H

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

#define	PTAKE_ADC											ADC1								//ֽȡ��ADC 
#define	PTAKE_ADC_PORT								GPIOC								//ADC�˿�
#define	PTAKE_ADC_PIN									GPIO_Pin_0					//ADC�ܽ�
#define	PTAKE_ADC_CHANNEL							ADC_Channel_10				//ADCͨ��
#define PTAKE_ADC_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOC
	 
//�ߵ�ƽʱΪȱֽ����ʾֽ�ű�ȡ�ߣ��͵�ƽΪ��ֽ����ʾֽ��δȡ��
/********* �����ֽ���������ֵ�ѹ��Ӧ��ת��ֵ **********/
#define PTAKE_ADC_VALUE_LOW						(ADC_MAX_VALUE * 1.5 / VREF)
#define PTAKE_ADC_VALUE_MIDDLE				(ADC_MAX_VALUE * 2.5 / VREF)  //2.0��Ϊ2.5
#define PTAKE_ADC_VALUE_HIGH					(ADC_MAX_VALUE * 2.9 / VREF)

/* Funtion Declare -----------------------------------------------------------*/
void	InitPaperTakeAwayPort(void);

#ifdef __cplusplus
}
#endif

#endif /* PTAKE_H ������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
