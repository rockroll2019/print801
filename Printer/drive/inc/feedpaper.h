/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-19
  * @brief   ��ֽ����ֽ�����صĳ���.
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
#ifndef FEEDPAPER_H
#define FEEDPAPER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"debug.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
#define FEED_PHASE_CTRL_GPIO_PORT						GPIOE
#define FEED_PHASE_CTRL_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOE
#define FEED_PHASE_CTRL_GPIO_PIN_1					GPIO_Pin_11
#define FEED_PHASE_CTRL_GPIO_PIN_2					GPIO_Pin_10			
#define FEED_PHASE_CTRL_GPIO_PIN_ALL				GPIO_Pin_10 | GPIO_Pin_11

#define FEED_EN_CTRL_GPIO_PORT							GPIOE
#define FEED_EN_CTRL_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE
#define FEED_EN_CTRL_GPIO_PIN_1							GPIO_Pin_8
#define FEED_EN_CTRL_GPIO_PIN_2							GPIO_Pin_9
#define FEED_EN_CTRL_GPIO_PIN_ALL						GPIO_Pin_8 | GPIO_Pin_9

#define	FeedPhaseASet()			GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_1)
#define	FeedPhaseAReset()		GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_1)
#define	FeedPhaseBSet()			GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_2)
#define	FeedPhaseBReset()		GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_2)
#define	FeedPhaseAllSet()		GPIO_SetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_ALL)
#define	FeedPhaseAllReset()	GPIO_ResetBits(FEED_PHASE_CTRL_GPIO_PORT, FEED_PHASE_CTRL_GPIO_PIN_ALL)

#define	FeedMotorEnable()		GPIO_ResetBits(FEED_EN_CTRL_GPIO_PORT, FEED_EN_CTRL_GPIO_PIN_ALL)	//ʹ��
#define	FeedMotorDisable()	GPIO_SetBits(FEED_EN_CTRL_GPIO_PORT, FEED_EN_CTRL_GPIO_PIN_ALL)		//����

//#define	FeedMotorForward()		GPIO_SetBits(FEED_CTRL_GPIO_PORT, FEED_DIR_GPIO_PIN)		//��ֽ�������
//#define	FeedMotorBackward()		GPIO_ResetBits(FEED_CTRL_GPIO_PORT, FEED_DIR_GPIO_PIN)	//��ֽ�������

#ifdef	FEED_MOTOR_MONITOR_ENABLE

#endif

//��ֽʱ�����ǰ��������������ת���:
//��ʼ��ס�׶Ρ���>�������׶Ρ���>���ٽ׶Ρ���>��ֹͣ�׶Ρ���>������ס�׶�
/*��������ֽ����,ǰ�����Ǻ���*/
#define	FEED_FORWARD					(0)
#define	FEED_BACKWARD					(1)

#define	FEED_VOLTAGE					(24.0)		//��������ѹ,��λV
#define	FEED_MAX_PPS					(1200)		//���������/��

//�����Ƕ�JX-3R-021�����Ĳ����궨��,����ʱ��Ͳ������ǰ���һ��һ�����������е�
//ע���¶���0~10��ʱ,��ӡʱ����ٶȲ�����1200PPS,�����ٲ�������
//��̬����ʱ������1200PPS
//�����������ڵ���450pps������������������߿�ֽ
#define	FEED_RUSH_STEPS												(126)	//��������ܲ���

// #define	MAX_RUSH_STEPS												(FEED_RUSH_STEPS)	//��������ܲ���
#define	MAX_RUSH_STEPS_UNDER_10_CENTIGRADE		(MAX_RUSH_STEPS / 2)	//�¶ȵ���10�ȴ�ӡʱ�����ټ��ٲ���
#define	MAX_RUSH_STEPS_ON_DYNAMIC_HEAT				(MAX_RUSH_STEPS / 2)	//��̬���ȴ�ӡʱ�����ټ��ٲ���
#define	MAX_RUSH_STEPS_ON_EMPHASIZE_PRT				(MAX_RUSH_STEPS / 2)	//���ش�ӡʱ����ֵ
#define	MAX_RUSH_STEPS_ON_PRT_BITMAP					(MAX_RUSH_STEPS / 2)	//ͼ�δ�ӡʱ����ֵ
#define	MAX_RUSH_STEPS_ON_CYCLE90_PRT					(MAX_RUSH_STEPS / 2)	//��ת90�ȴ�ӡʱ����ֵ
#define	MAX_RUSH_STEPS_ON_CHAR_NOT_PRT				(MAX_RUSH_STEPS / 2)	//���Դ�ӡʱ����ֵ
#define	MAX_RUSH_STEPS_ON_DIFFERENT_MAGNIFICATION_PRT		(MAX_RUSH_STEPS / 2)	//�������Ŵ�����һ��ʱ,60OK,80OK

#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT					(MAX_RUSH_STEPS / 3)	//WH������ӡʱ�����ټ��ٲ���,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_COM			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//����������ӡ����,10,15
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_ETHERNET	(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//����������ӡ����,20,38,30,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_USB			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//U��������ӡ����,20,38,30,20
#define	MAX_RUSH_STEPS_ON_WH_DRV_PRT_LPT			(MAX_RUSH_STEPS_ON_WH_DRV_PRT)	//����������ӡ����,15

#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT					(MAX_RUSH_STEPS / 2)	//SP������ӡʱ�����ټ��ٲ��� 15
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_COM			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//����������ӡ����,10
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_ETHERNET	(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//����������ӡ����,20,38,30
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_USB			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//U��������ӡ����,20,38,30
#define	MAX_RUSH_STEPS_ON_SP_DRV_PRT_LPT			(MAX_RUSH_STEPS_ON_SP_DRV_PRT)	//����������ӡ����,30

#define	FEED_BACK_MAX_STEPS						(118)		/*���������ֽ����*/
#define	FEED_ONE_TIME_STEPS						(FEED_BACK_MAX_STEPS)	/*���尴����ֽ��ʱ��ֽ�Ĳ���*/
#define	FEED_STEPS_AFTER_CLOSE_SHELL	(16)		/*����ÿ���ϵ��ʼ������߸���ֽ�ո�ʱ��ֽ�Ĳ���*/
#define	FEED_STEPS_ANTI_BACKLASH			(16)		//��ʼ��ʱ��ֹ���������ȷ�����ֽ��������ֽ�Ĳ���

#define	FEED_START_US									(4000)	//��ʼ��ס�׶εȴ�ʱ��us
#define	FEED_STOP_US									(10000)	//������ס�׶εȴ�ʱ��us
#define	FEED_CONST_SPEED_US						(884)		//���ٽ׶εĸ�����ʱus
#define	FEED_SINGLE_STEP_SPEED_US			(5000)	//�������ٵĸ�����ʱus
//�����Ƕ�JX-3R-021�����Ĳ����궨��

#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
#define	AUTO_FEED_AFTER_CUT_STEPS			FEED_STEPS_AFTER_CLOSE_SHELL
#endif

/* Funtion Declare -----------------------------------------------------------*/
//�˺��������ⲿ����
void	InitFeedPort(void);
void	FeedPaper(uint32_t direction, uint32_t step);	//�����ֽ����,���ݸ���ֵȷ����ֽ����
void	FindMarkPoint(void);

//��ֽʱ�����ǰ��������������ת���:
//��ʼ��ס�׶Ρ���>�������׶Ρ���>���ٽ׶Ρ���>��ֹͣ�׶Ρ���>������ס�׶�
//ֻ������λ�ͷ���,��������ʱ
void	StartFeedMotor(uint32_t direction); //��ʼ��ס�׶�
void	StopFeedMotor(uint32_t direction);  //������ס�׶�
void	StepFeedMotor(uint32_t direction);	//��ĳ������ת��1������
void	EndFeedMotor(void);   //ȫ����·�׶�
void	AdjustMotorStep(uint16_t minsteptime);
void	GoDotLine(uint32_t DotLineNumber);

void	WaitHeatAndFeedPaperFinish(void);
void	WaitHeatFinish(void);
void	WaitFeedPaperFinish(void);
void	WaitPrintAndFeedPaperFinish(void);
void	WaitPreviousPrintFinish(void);

uint16_t	GetMaxAccelerateSteps(void);

uint32_t	ErrorDetect(void);
void	SetEmergencyProtect(void);
void	ReleaseEmergencyProtect(void);
void KeyFeedPaper(uint16_t DotLine);

#ifdef __cplusplus
}
#endif

#endif /* FEEDPAPER_H�ļ�������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
