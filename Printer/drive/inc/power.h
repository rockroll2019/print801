/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ��Դ��������صĳ���.
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
#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#ifndef	VREF
	#define	VREF							(3.3)				//AD�ο���ѹ
#endif
#ifndef	ADC_MAX_VALUE
	#define	ADC_MAX_VALUE			(4095)			//ADת�����ֵ
#endif

#define	POWER_ADC												ADC1						//�����ѹADC 
#define	POWER_ADC_PORT									GPIOC						//�����ѹADC�˿�
#define	POWER_ADC_PIN										GPIO_Pin_5			//�����ѹADC�ܽ�
#define	POWER_ADC_CHANNEL								ADC_Channel_15	//�����ѹADCͨ��
#define POWER_ADC_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOC

#define POWER_CTRL_GPIO_PORT						GPIOE
#define POWER_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define POWER_CTRL_GPIO_PIN							GPIO_Pin_7

#define	HeatPowerOn()		GPIO_SetBits(POWER_CTRL_GPIO_PORT, POWER_CTRL_GPIO_PIN)
#define	HeatPowerOff()	GPIO_ResetBits(POWER_CTRL_GPIO_PORT, POWER_CTRL_GPIO_PIN)
//#define	HeatPowerOn()
//#define	HeatPowerOff()
//2017.08.16
#define PHY_RESET_GPIO_PORT						  GPIOD
#define PHY_RESET_RCC_APB2Periph_GPIO	  RCC_APB2Periph_GPIOD
#define PHY_RESET_GPIO_PIN							GPIO_Pin_14

#define	PHYResetLow()		GPIO_ResetBits(PHY_RESET_GPIO_PORT, PHY_RESET_GPIO_PIN)
#define	PHYResetHigh()	GPIO_SetBits(PHY_RESET_GPIO_PORT, PHY_RESET_GPIO_PIN)

/* �ⲿ��Դ�����ѹֵ����0��3.3V��ʵ��ʹ��ʱ����ʹ�õ�Դ��ѹ��ͬ��ĳֵ�������� */
#define	POWER_CHANGE_RATIO	(0.15)		//��Դ��ѹ�仯�ٷ���10%
#define	POWER_STD						(24.0)		//�����Դ��׼ֵ����λV
#define	POWER_MIN						(POWER_STD * (1.0 - POWER_CHANGE_RATIO))	//�����Դ��Сֵ����λV
#define	POWER_MAX						(POWER_STD * (1.0 + POWER_CHANGE_RATIO))	//�����Դ���ֵ����λV

#define	POWER_DIVIDE_RATIO	(10.0 / (10.0 + 100.0))	//��ѹ����ֵȷ���Ĺ����ѹ��

//������Դ,�͵�ѹ,�ߵ�ѹ���澯�رյ�ѹ��Ӧ��ת��ֵ
#define POWER_VALUE_STD			((uint16_t)(POWER_STD * POWER_DIVIDE_RATIO * ADC_MAX_VALUE / VREF))
#define POWER_VALUE_LOW			((uint16_t)(POWER_VALUE_STD * 0.85))	//��ѹ����ʱ��Ӧ��ת��ֵ
#define POWER_VALUE_HIGH		((uint16_t)(POWER_VALUE_STD * 1.15))	//��ѹ����ʱ��Ӧ��ת��ֵ
#define POWER_VALUE_CLOSE		((uint16_t)(POWER_VALUE_STD * 1.2))		//��ѹ�رմ�ӡʱ��Ӧ��ת��ֵ

#define POWER_COEFFICIENT		(0.1)		//�����ѹ����ϵ������ѹ�����ʱ���ת��б�ʣ�

/* Funtion Declare -----------------------------------------------------------*/
void	InitPowerCtrlPort(void);
void	InitPowerInPort(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
