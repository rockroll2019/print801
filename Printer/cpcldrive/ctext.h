/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
  * @brief   CPCLָ��ַ�������س���.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CTEXT_H
#define CTEXT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<stdint.h>

/* Private typedef -----------------------------------------------------------*/
/* Macro Definition ----------------------------------------------------------*/

/* Funtion Declare -----------------------------------------------------------*/
void	InitPageCharMaskBufParameter(void);
void	SetPageCharParameter(uint8_t Language, uint8_t FontType);
void	GetPageCharDotData(uint8_t CharCode, uint8_t DotData[]);

/*����Ϊ�����ȡ�����ַ�*/
void	GetPageASCIIDotData(uint8_t AscIICode, uint8_t DotData[]);
void	GetUserDefCharDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[]);
void	GetCharFlashDotData(uint8_t AscIICode, uint8_t FontType, uint8_t DotData[]);

/*����Ϊ�����ȡGB18030�ַ�*/
void	JudgePagePrintGB18030(uint8_t Ch1, uint8_t DotData[]);
void	GetPageGB18030DotData(uint8_t Code1, uint8_t Code2, \
														uint8_t Code3, uint8_t Code4, uint8_t DotData[]);



#ifdef __cplusplus
}
#endif

#endif /* CTEXT_H ������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
