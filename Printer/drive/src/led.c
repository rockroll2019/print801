/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-19
  * @brief   led��صĳ���.
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

/* Includes ------------------------------------------------------------------*/
#include	"led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ȱֽLEDָʾ�ƶ˿ڳ�ʼ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void PELedPortInit(void)
{	//���ƹܽ�����Ϊ���,��ʼ��Ϊ1
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(PE_LED_RCC_APB2Periph_GPIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin		= PE_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(PE_LED_GPIO_PORT, &GPIO_InitStructure);
	PELedOff();
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����LEDָʾ�ƶ˿ڳ�ʼ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void ErrorLedPortInit(void)
{	//���ƹܽ�����Ϊ���,��ʼ��Ϊ1  
	GPIO_InitTypeDef GPIO_InitStructure;
	//�����
	RCC_APB2PeriphClockCmd(ERROR_LED_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= ERROR_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(ERROR_LED_GPIO_PORT, &GPIO_InitStructure);
	ErrorLedOff();
	//LED�ƴ�
	RCC_APB2PeriphClockCmd(LINE_LED_RCC_APB2Periph_GPIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin		= LINE_LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init(LINE_LED_GPIO_PORT, &GPIO_InitStructure);
	LineLedOn();
	
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ָ���˿ڵ�ָ���ܽŵ����״̬ȡ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if (GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin))
	{
		GPIO_ResetBits(GPIOx, GPIO_Pin);
	}
	else
	{
		GPIO_SetBits(GPIOx, GPIO_Pin);
	}	
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȱֽLEDָʾ��ȡ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	TogglePELed(void)
{
	TogglePin(PE_LED_GPIO_PORT, PE_LED_GPIO_PIN);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����״̬LEDָʾ��ȡ��
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	ToggleErrorLed(void)
{
	TogglePin(ERROR_LED_GPIO_PORT, ERROR_LED_GPIO_PIN);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
