/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   ��ֽ�����Լ���̫����λ���������صĳ���.
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
#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
//�����ⷽʽ,ͨ���ⲿ�жϷ�ʽ����IO��ѯ��ʽ
#define		FEED_BUTTON_CHECK_BY_INT
//#define		RESET_ETHERNET_BUTTON_CHECK_BY_INT
#define		MAX_FEED_PAPER   (30)
	 
#define FEED_BUTTON_GPIO_PORT								GPIOE
#define FEED_BUTTON_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOE
#define FEED_BUTTON_GPIO_PIN								GPIO_Pin_1

#ifdef	FEED_BUTTON_CHECK_BY_INT
	#define FEED_BUTTON_EXTI_LINE							EXTI_Line1
	#define FEED_BUTTON_GPIO_PORT_SOURCE			GPIO_PortSourceGPIOE
	#define FEED_BUTTON_GPIO_PIN_SOURCE				GPIO_PinSource1
	#define FEED_BUTTON_EXTI_IRQn							EXTI1_IRQn
#endif

/* ������ֽ����Ϊ���룬δ����ʱΪ�ߣ����º�Ϊ�� */
/* ���尴������ʱ�Ĳ�������ֵ */
#define	FEED_BUTTON_VALUE_CLOSE			(0)
#define	FEED_BUTTON_VALUE_OPEN			(1)

/* ���巵�صİ�����״̬ */
#define	FEED_BUTTON_STATUS_CLOSE		(0)
#define	FEED_BUTTON_STATUS_OPEN			(1)

/* ������̫����λ���� */
//��ⷽʽ����ͨ���ⲿ�жϷ�ʽ����IO��ѯ��ʽ
#if defined	STM32F10X_CL
	#ifdef	RESET_ETHERNET_BUTTON_CHECK_BY_INT
	#define RESET_ETHERNET_BUTTON_EXTI_LINE								EXTI_Line14
	#define RESET_ETHERNET_BUTTON_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOC
	#define RESET_ETHERNET_BUTTON_GPIO_PIN_SOURCE					GPIO_PinSource14
	#endif

	#define RESET_ETHERNET_BUTTON_GPIO_PORT								GPIOC
	#define RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOC
	#define RESET_ETHERNET_BUTTON_GPIO_PIN								GPIO_Pin_14
#else
	#ifdef	RESET_ETHERNET_BUTTON_CHECK_BY_INT
	#define RESET_ETHERNET_BUTTON_EXTI_LINE								EXTI_Line1
	#define RESET_ETHERNET_BUTTON_GPIO_PORT_SOURCE				GPIO_PortSourceGPIOB
	#define RESET_ETHERNET_BUTTON_GPIO_PIN_SOURCE					GPIO_PinSource1
	#endif

	#define RESET_ETHERNET_BUTTON_GPIO_PORT								GPIOB
	#define RESET_ETHERNET_BUTTON_RCC_APB2Periph_GPIO			RCC_APB2Periph_GPIOB
	#define RESET_ETHERNET_BUTTON_GPIO_PIN								GPIO_Pin_1
#endif

#if defined	STM32F10X_CL
	#define	PRT_CTRL_RESET_ETH_GPIO_PORT									GPIOD
	#define	PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO				RCC_APB2Periph_GPIOD
	#define	PRT_CTRL_RESET_ETH_GPIO_PIN										GPIO_Pin_13
#else
	#define	PRT_CTRL_RESET_ETH_GPIO_PORT									GPIOB
	#define	PRT_CTRL_RESET_ETH_RCC_APB2Periph_GPIO				RCC_APB2Periph_GPIOB
	#define	PRT_CTRL_RESET_ETH_GPIO_PIN										GPIO_Pin_0
#endif

/* ���尴������ʱ�Ĳ�������ֵ */
#ifdef	STM32F10X_CL
	/* ������̫����λΪ���룬δ����ʱΪ�ͣ����º�Ϊ�� */
	#define	RESET_ETHERNET_BUTTON_VALUE_CLOSE		(1)
	#define	RESET_ETHERNET_BUTTON_VALUE_OPEN		(0)
#endif

#ifdef	STM32F10X_HD
	/* ������̫����λΪ���룬δ����ʱΪ�ߣ����º�Ϊ�� */
	#define	RESET_ETHERNET_BUTTON_VALUE_CLOSE		(0)
	#define	RESET_ETHERNET_BUTTON_VALUE_OPEN		(1)
#endif

/* ���巵�صİ�����״̬ */
#define	RESET_ETHERNET_BUTTON_STATUS_CLOSE	(0)
#define	RESET_ETHERNET_BUTTON_STATUS_OPEN		(1)

#define	RESET_ETHERNET_MS										(20)

/* Funtion Declare -----------------------------------------------------------*/
void	InitFeedButtonPort(void);
uint8_t		ReadFeedButtonStatus(void);
void	CheckFeedButton(void);

void	InitResetEthernetButtonPort(void);
uint8_t		ReadResetEthernetButtonStatus(void);
void	InitPrtCtrlResetEthernetPort(void);
void	ResetEthernet(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
