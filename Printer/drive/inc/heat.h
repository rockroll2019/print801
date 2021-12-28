/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   ���ȿ�����صĳ���.
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
#ifndef HEAT_H
#define HEAT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
#include	"defconstant.h"

/* Macro Definition ----------------------------------------------------------*/
#define HEAT_CTRL_GPIO_PORT							GPIOE
#define HEAT_CTRL_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOE
#define HEAT_PDST1_GPIO_PIN							GPIO_Pin_5
#define HEAT_PDST2_GPIO_PIN							GPIO_Pin_6
	 
#define HEAT_PLATCH_GPIO_PORT						GPIOB
#define HEAT_PLATCH_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOB
#define HEAT_PLATCH_GPIO_PIN						GPIO_Pin_6 

#define StartHeatPartOne()	GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN)
#define StartHeatPartTwo()	GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST2_GPIO_PIN)
#define StartHeatAll()			GPIO_SetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN)
#define StopHeatPartOne()		GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN)
#define StopHeatPartTwo()		GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST2_GPIO_PIN)
#define StopHeatAll()				GPIO_ResetBits(HEAT_CTRL_GPIO_PORT, HEAT_PDST1_GPIO_PIN | HEAT_PDST2_GPIO_PIN)

#define	MAX_HEAT_SUM					(2)				//һ������������ȿ��ƴ��� 
#define	MAX_HEAT_DOT_ONE_TIME	(352)			//һ������������ͬʱ���ȵ��� 352
//���Ҹ��ճ�32�㲻����
#define	DST1_HEAT_DOT					(352)			//���DST1���Ƶļ��ȵ���
#define	DST2_HEAT_DOT					(224)			//�Ҳ�DST2���Ƶļ��ȵ���
#define	DST1_HEAT_BYTE				(44)			//(DST1_HEAT_DOT >> 3)
#define	DST2_HEAT_BYTE				(28)			//(DST2_HEAT_DOT >> 3)

#define	HEAT_DISABLE					(0)
#define	HEAT_ENABLE						(1)

#define	HEAT_ACTION_DISABLE		(0)
#define	HEAT_ACTION_ENABLE		(1)

#define	HEAT_TEMP_COEFFICENT	(0.0076)
#define	ROOM_TEMPERATURE			(25)			//�����µı�׼�¶�ֵ

#define	MIN_HEAT_TIME					(240)		//240
#define	MAX_HEAT_TIME					(1000)		//�����ʱ��,us,1000

/********* ��������Ũ�ȶ�Ӧ��ʱ��, ��λuS **********/
#define HEAT_TIME_LOW					(240)			//��Ũ�ȼ���ʱ��,��λuS
#define HEAT_TIME_MIDDLE			(360)			//��Ũ�ȼ���ʱ��, 360
#define HEAT_TIME_HIGH				(400)			//��Ũ�ȼ���ʱ��,
#define HEAT_TIME_MIN					(240)			//��С����ʱ��,240

typedef struct
{
	uint8_t	HeatEnable;				  			//ʹ�ö�ʱ���ļ��ȿ��Ʊ�־
	uint8_t	HeaterGroupNumber;				//һ�е������ݴ�ӡ���ȷ������
	uint8_t	HeaterGroupNumberCpy;			//һ�е������ݴ�ӡ���ȷ����������
	
	uint16_t	HeaterGroupElement[ MAX_HEAT_SUM ];		//һ�е������ݴ�ӡ���ȷ����������
	uint16_t	HeaterGroupElementCpy[ MAX_HEAT_SUM ];	//һ�е������ݴ�ӡ���ȷ���������ݸ���
}TypeDef_StructHeatCtrl;

/* Funtion Declare -----------------------------------------------------------*/
void	InitHeatPort(void);
void	LatchHeatData(void);
void	InitHeatCtrlStruct(void);

void	StartHeat(uint16_t	HeatPins);
void	EndHeat(void);

int16_t	GetTemperature(void);
void	SetHeatNumber(void);
uint16_t	GetHeatTime(void);

static uint8_t	GetHighBitNumberInByte(uint8_t DataIn);
static double	GetPowerVoltage(void);
static uint16_t	GetAdjustHeatTimeOfPowerVoltage(uint16_t htime);
static uint16_t	GetAdjustHeatTimeOfFirstLineHeat(uint16_t htime);
static uint16_t	GetAdjustHeatTimeOfTemperature(int32_t	Temperature);

#ifdef __cplusplus
}
#endif

#endif /* HEAT_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
