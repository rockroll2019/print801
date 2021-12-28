/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ��о�¶ȼ����صĳ���.
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
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

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

#define	TEMPERATURE_ADC												ADC1							//�����ѹADC 
#define	TEMPERATURE_ADC_PORT									GPIOC							//�����ѹADC�˿�
#define	TEMPERATURE_ADC_PIN										GPIO_Pin_4				//�����ѹADC�ܽ�
#define	TEMPERATURE_ADC_CHANNEL								ADC_Channel_14		//�����ѹADCͨ��
#define TEMPERATURE_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

/********* �����¶ȴ������������������¶ȶ�Ӧ��ת��ֵ **********/
/******** �¶ȼ�⹫ʽ  *******************/
/*ADת��ֵAD=ADC_MAX_VALUE*(RT/(RT+RH)��
*	 RTΪ��Ӧ�¶ȵĵ���ֵ,RHΪ������ѹ����*/
#define	RH						(8.2)			//������ѹ���� K��

/* ע����������Ĳ���������ΪR25=30K�� ,B=3950K�ĵ��裬
* Rx=R25*EAP^(B*(1/(273+Tx)-1/(273+25)),EAP=2.71828
* ����Ϊ��ǰ��о��������ֵ�ڲ�ͬ�¶��µ���ֵ����-20��100�� 
*/
#define	RTH_20				(269)
#define	RTH_15				(208)
#define	RTH_10				(178)
#define	RTH_5					(124)
#define	RTH0					(100)
#define	RTH5					(78)
#define	RTH10					(60)
#define	RTH15					(47.1)
#define	RTH20					(37.5)
#define	RTH25					(30.0)
#define	RTH30					(24.2)
#define	RTH35					(19.6)
#define	RTH40					(15.9)
#define	RTH45					(13.1)
#define	RTH50					(10.8)
#define	RTH55					(8.91)
#define	RTH60					(7.41)
#define	RTH65					(6.2)
#define	RTH70					(5.21)
#define	RTH75					(4.4)
#define	RTH80					(3.74)
#define	RTH85					(3.18)
#define	RTH90					(2.72)
#define	RTH95					(2.33)
#define	RTH100				(2.00)

/* �¶ȱ�������ֵ 60�� (FOR LTPFx47��о)���¶ȱ�������ֵ 85�� */
#define TEMPERATURE_WARN_ADC_LOW		((uint16_t)(ADC_MAX_VALUE * RTH60 / (RH + RTH60)))
#define TEMPERATURE_WARN_ADC_HIGH		((uint16_t)(ADC_MAX_VALUE * RTH85 / (RH + RTH85)))

/* �¶ȴ�������ֵ -20�� (FOR LTPFx47��о)���¶ȴ�������ֵ 100�� */
#define TEMPERATURE_ERROR_ADC_LOW		((uint16_t)(ADC_MAX_VALUE * RTH_20 / (RH + RTH_20)))
#define TEMPERATURE_ERROR_ADC_HIGH	((uint16_t)(ADC_MAX_VALUE * RTH100 / (RH + RTH100)))

#define TEMPERATURE_TABLE_VALUE_LOW						(-20)	//�����¶�ֵ���һ����Ӧ���¶�ֵ
#define TEMPERATURE_TABLE_VALUE_TOTAL_NUMBER	(25)	//�����¶�ֵ��������

/* Funtion Declare -----------------------------------------------------------*/
void	InitTemperaturePort(void);

#ifdef __cplusplus
}
#endif

#endif /* TEMPERATURE_H�ļ�������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
