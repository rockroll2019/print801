/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-20
  * @brief   �е����е������صĳ���.
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
#ifndef CUTTER_H
#define CUTTER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"extgvar.h"
#include	"beep.h"

/******************************************************************************
* ����ע�����
* 1. �е������źż��ʱ��Ҫ��ȥ������������ֵΪ2ms������ʱ��������·�йء�
* 2. ���ת��״̬��任ʱ����Ҫֱ�ӱ仯����Ҫ��ֹͣ1ms��Ȼ���ٱ仯���µ�ת������
******************************************************************************/

/* Macro Definition ----------------------------------------------------------*/
#define CUT_CTRL_GPIO_PORT						GPIOE
#define CUT_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define CUT_PHASE_GPIO_PIN						GPIO_Pin_3
#define CUT_ENABLE_GPIO_PIN						GPIO_Pin_4
#define CUT_MODE_GPIO_PIN							GPIO_Pin_2

#define CUT_HOME_IN_GPIO_PORT							GPIOB
#define CUT_HOME_IN_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOB
#define CUT_HOME_IN_GPIO_PIN							GPIO_Pin_7

//���¹��ܽ��޻������߶��޷�����
//#define CUT_SLEEP_GPIO_PIN						GPIO_Pin_1
//#define CUT_FAULT_GPIO_PIN						GPIO_Pin_4
//#define CUT_MODE1_GPIO_PIN						GPIO_Pin_3
//#define CUT_MODE2_GPIO_PIN						GPIO_Pin_5

#define	CutMotorEnable()			GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_ENABLE_GPIO_PIN)		//ʹ��
#define	CutMotorDisable()			GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_ENABLE_GPIO_PIN)	//����

#define	CutMotorSetPhase()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_PHASE_GPIO_PIN)		//PHASE�ø�
#define	CutMotorResetPhase()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_PHASE_GPIO_PIN)	//PHASE�õ�

#define	CutMotorSetMode()			GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE_GPIO_PIN)		//MODE�ø�
#define	CutMotorResetMode()		GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE_GPIO_PIN)	//MODE�õ�

//���¹��ܽ��޻������߶��޷�����
//#define	CutMotorSetMode1()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE1_GPIO_PIN)		//MODE1�ø�
//#define	CutMotorResetMode1()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE1_GPIO_PIN)	//MODE1�õ�
//#define	CutMotorSetMode2()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_MODE2_GPIO_PIN)		//MODE2�ø�
//#define	CutMotorResetMode2()	GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_MODE2_GPIO_PIN)	//MODE2�õ�
//#define	CutMotorSleepOn()			GPIO_ResetBits(CUT_CTRL_GPIO_PORT, CUT_SLEEP_GPIO_PIN)	//��������
//#define	CutMotorSleepOff()		GPIO_SetBits(CUT_CTRL_GPIO_PORT, CUT_SLEEP_GPIO_PIN)		//ֹͣ����

/* �е�ʱ�����ǰ��������������ת���: */
/* ���е���ֱ��������� */
/* �����е������Ƿ�����ı�־λ */
#define		CUT_PROCESS_BUSY		(1)
#define		CUT_PROCESS_ILDE		(0)

/* �������Ƿ�ӵ�ı�־λ */
#define		CUT_POWER_YES				(1)
#define		CUT_POWER_NO				(0)

/* ����������,ǰ�����Ǻ��� */
#define		CUT_OUTWARD					(1)
#define		CUT_HOMEWARD				(0)

/* ��ǰ��·�����е���ԭλ��⵽�ͣ�û����ԭλ��⵽�� */
#define		CUT_AT_HOME					(0)
#define		CUT_NOT_AT_HOME			(1)

/* �е��������� */
#define		CUT_TYPE_FULL				(0)
#define		CUT_TYPE_PART				(1)
#define		CUT_TYPE_DEFAULT		(CUT_TYPE_PART)

/* �е���ʼ����� */
#define		CUT_INIT_SUCCESS		(0)		//�ɹ�
#define		CUT_INIT_ERROR1			(1)		//����״̬1����ʼ����
#define		CUT_INIT_ERROR2			(2)		//����״̬2���м����

#define		CUT_RESULT_0				(0)
#define		CUT_RESULT_1				(1)
#define		CUT_RESULT_2				(2)
#define		CUT_RESULT_3				(3)
#define		CUT_RESULT_4				(4)

/* �е���ֽ��� */
#define		CUT_PAPER_SUCCESS		(CUT_INIT_SUCCESS)
#define		CUT_PAPER_ERROR2		(CUT_INIT_ERROR2)

#define		CUT_FEED_PAPER_BACKWARD_STEPS   (32)

#define		FEED_PAPER_MM_AFTER_CUT					(2)		//ԭΪ8
#define		FEED_PAPER_DOT_LINE_AFTER_CUT		(FEED_PAPER_MM_AFTER_CUT * 8)

/* Funtion Declare -----------------------------------------------------------*/
/* �˺��������ⲿ���� */
void	InitCutPort(void);
void	InitCutPaper(void);
uint32_t	CutPaper(uint8_t CutMode);	//ʵ����ֽ����
void	DriveCutPaper(uint8_t CutMode);	//��ֽ
void	CutMotorStandby(void);					//���ֹͣ
void	StopCut(void);

static void	CutError(void);
static void CutMotorForward(void);		//���ǰת
static void CutMotorReverse(void);		//�����ת
static void CutMotorBrake(void);			//���ɲ��
static void	CutMotorFastDecay(void);
static uint8_t	GetCutSwitchStatus(void);	//��ȡ�е�λ�ü�⿪�ص�ֵ

#ifdef __cplusplus
}
#endif

#endif /* CUTTER_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
