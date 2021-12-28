/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ������ֽ�ָǣ������صĳ���.
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
#ifndef PLATEN_H
#define PLATEN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//�����ⷽʽ,ͨ���ⲿ�жϷ�ʽ����IO��ѯ��ʽ
//#define		PLATEN_CHECK_BY_INT

#ifdef	PLATEN_CHECK_BY_INT
	#define GPIO_PORT_PLATEN										GPIOB
	#define RCC_APB2Periph_GPIO_PLATEN					RCC_APB2Periph_GPIOB
	#define GPIO_PIN_PLATEN											GPIO_Pin_1
	#define EXTI_LINE_PLATEN										EXTI_Line1
	#define GPIO_PORT_SOURCE_PLATEN							GPIO_PortSourceGPIOB
	#define GPIO_PIN_SOURCE_PLATEN							GPIO_PinSource1
#else
	#define PLATEN_GPIO_PORT										GPIOB
	#define PLATEN_RCC_APB2Periph_GPIO					RCC_APB2Periph_GPIOB
	#define PLATEN_GPIO_PIN											GPIO_Pin_1
	#define	ReadPlatenStatus()									ReadCoverStatus()
#endif

/* ���û�оѹ�˼�⴫����Ϊ��е���ء���е����һ�˽ӵأ���һ�˳������������ߵ�ƽ��
*  ��оѹ��ѹ�£���⵽��ƽ��Ϊ�ߡ���оѹ�˴򿪣�����ƽ��Ϊ�͡�
*/
/* �����оѹ�˿���ʱ�Ĳ�������ֵ�������ʵ�ֵ�·�й� */
#define		PLATEN_VALUE_CLOSE		(0)
#define		PLATEN_VALUE_OPEN			(1)

/* ���巵�صĻ�оѹ�˵�״̬��������·����ƽ״̬�����޹� */
#define		PLATEN_STATUS_CLOSE		(0)
#define		PLATEN_STATUS_OPEN		(1)

/* Funtion Declare -----------------------------------------------------------*/
#ifdef	PLATEN_CHECK_BY_INT
#else
	void	InitPlatenPort(void);
	uint8_t	ReadCoverStatus(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PLATEN_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
