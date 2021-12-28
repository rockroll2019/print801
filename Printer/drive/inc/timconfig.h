/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-6-4
  * @brief   ʱ��������صĳ���.
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
#ifndef TIMCONFIG_H
#define TIMCONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"debug.h"

#ifdef	DBG_PROCESS_TIME
#include	"stm32f10x_systick.h"
#endif

/* Macro Definition ----------------------------------------------------------*/
#define	DBG_TIMER						(TIM1)
#define	ROUTINE_TIMER				(TIM2)

#define	HEAT_TIMER					(TIM3)
#define	FEED_TIMER					(TIM3)
#define	FEED_HEAT_TIMER			(TIM3)
#define	ETH_DATA_TIMER  		(TIM4)
#define	PRINT_CHECK_TIMER		(TIM6)
	 
// #define	BACKUP_TIMER				(TIM5)
// #define	DELAY_TIMER					(TIM6)

#define	DealPrtData_TIMER		(TIM5)		//2016.06.15 ���������͵���ӡͷ

#define	LWIP_ROUTINE_TIMER	(TIM7)

#define	ROUTINE_NORMAL_INTERVAL_MS	(1)		//��������������Լ�鹤������ʱ
#define	ROUTINE_SLEEP_INTERVAL_MS		(250)	//������������������Լ�鹤������ʱ
	 
/* Funtion Declare -----------------------------------------------------------*/
void	IncreaseDelayMs(void);
void	DelayUs(uint32_t	delaytime);
void	DelayMs(uint32_t	DelayNumber);
void	TimerConfigPrescaler(TIM_TypeDef * TIMx, uint32_t Prescaler);
void  SetTimerCounterWithAutoReload(TIM_TypeDef * TIMx, uint16_t Counter);
void  StartTimer(TIM_TypeDef * TIMx);
void  StopTimer(TIM_TypeDef * TIMx);
uint32_t	GetTimerEnableStatus(TIM_TypeDef * TIMx);

#ifdef	DBG_PROCESS_TIME
	void	InitSystick(void);
	#define	StartProcessTimeCounter()	SysTick_CounterCmd(SysTick_Counter_Enable)
	void	StopProcessTimeCounter(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* TIMCONFIG_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
