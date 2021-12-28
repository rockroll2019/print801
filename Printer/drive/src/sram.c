/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-29
  * @brief   �ⲿSRAM��صĳ���.
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
#include	"sram.h"
#include	"debug.h"
#include	"defconstant.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : �ⲿSRAM���ƹܽų�ʼ����16λ�������ַ�߸���
* Input          : ��
* Output         : ���������
* Return         : ���ز�����
*******************************************************************************/
void	InitExtSRAMPort(void)
{
	GPIO_InitTypeDef		GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(FSMC_D0_TO_D3_RCC_APB2Periph_GPIO | \
												 FSMC_D4_TO_D12_RCC_APB2Periph_GPIO | \
												 FSMC_D13_TO_D15_RCC_APB2Periph_GPIO | \
												 FSMC_A0_TO_A3_RCC_APB2Periph_GPIO | \
												 FSMC_A4_TO_A12_RCC_APB2Periph_GPIO | \
												 FSMC_A13_TO_A18_RCC_APB2Periph_GPIO | \
												 FSMC_NE1_RCC_APB2Periph_GPIO | \
												 FSMC_NADV_RCC_APB2Periph_GPIO | \
												 FSMC_NOE_RCC_APB2Periph_GPIO | \
												 FSMC_NWE_RCC_APB2Periph_GPIO | \
												 FSMC_NBL1_RCC_APB2Periph_GPIO | \
												 FSMC_NBL0_RCC_APB2Periph_GPIO, ENABLE);
	
	/* �����߹ܽ����� */
	GPIO_InitStructure.GPIO_Pin = FSMC_D0_GPIO_PIN | \
																FSMC_D1_GPIO_PIN | \
																FSMC_D2_GPIO_PIN | \
																FSMC_D3_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(FSMC_D0_TO_D3_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D4_GPIO_PIN | \
																FSMC_D5_GPIO_PIN | \
																FSMC_D6_GPIO_PIN | \
																FSMC_D7_GPIO_PIN | \
																FSMC_D8_GPIO_PIN | \
																FSMC_D9_GPIO_PIN | \
																FSMC_D10_GPIO_PIN | \
																FSMC_D11_GPIO_PIN | \
																FSMC_D12_GPIO_PIN;
  GPIO_Init(FSMC_D4_TO_D12_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_D13_GPIO_PIN | \
																FSMC_D14_GPIO_PIN | \
																FSMC_D15_GPIO_PIN;
  GPIO_Init(FSMC_D13_TO_D15_GPIO_PORT, &GPIO_InitStructure);
	
	/* ��ַ�߹ܽ����� */
	GPIO_InitStructure.GPIO_Pin = FSMC_A0_GPIO_PIN | \
																FSMC_A1_GPIO_PIN | \
																FSMC_A2_GPIO_PIN | \
																FSMC_A3_GPIO_PIN;
  GPIO_Init(FSMC_A0_TO_A3_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A4_GPIO_PIN | \
																FSMC_A5_GPIO_PIN | \
																FSMC_A6_GPIO_PIN | \
																FSMC_A7_GPIO_PIN | \
																FSMC_A8_GPIO_PIN | \
																FSMC_A9_GPIO_PIN | \
																FSMC_A10_GPIO_PIN | \
																FSMC_A11_GPIO_PIN | \
																FSMC_A12_GPIO_PIN;
  GPIO_Init(FSMC_A4_TO_A12_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A13_GPIO_PIN | \
																FSMC_A14_GPIO_PIN | \
																FSMC_A15_GPIO_PIN | \
																FSMC_A16_GPIO_PIN | \
																FSMC_A17_GPIO_PIN | \
																FSMC_A18_GPIO_PIN;
  GPIO_Init(FSMC_A13_TO_A18_GPIO_PORT, &GPIO_InitStructure);
	
	/* ���������߹ܽų�ʼ�� */
	GPIO_InitStructure.GPIO_Pin = FSMC_NE1_GPIO_PIN;
  GPIO_Init(FSMC_NE1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_NADV_GPIO_PIN;
  GPIO_Init(FSMC_NADV_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_NOE_GPIO_PIN;
  GPIO_Init(FSMC_NOE_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_NWE_GPIO_PIN;
  GPIO_Init(FSMC_NWE_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_NBL1_GPIO_PIN;
  GPIO_Init(FSMC_NBL1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_NBL0_GPIO_PIN;
  GPIO_Init(FSMC_NBL0_GPIO_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : ������
* Description    : �ⲿSRAM���ó�ʼ����16λ�������ַ�߸���
* Input          : ��
* Output         : ���������
* Return         : ���ز�����
*******************************************************************************/
void	InitExtSRAM(void)
{
	FSMC_NORSRAMInitTypeDef					FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef		pTimSet;

	RCC -> AHBENR |= 0x00000114;       //ʹ��FSMCʱ��
	
	/*-- FSMC Configuration ------------------------------------------------------ */
	pTimSet.FSMC_AddressSetupTime				= 1;
	pTimSet.FSMC_AddressHoldTime				= 1;
	pTimSet.FSMC_DataSetupTime					= 1;
	pTimSet.FSMC_BusTurnAroundDuration	= 0;
	pTimSet.FSMC_CLKDivision						= 0;
	pTimSet.FSMC_DataLatency						= 0;
	pTimSet.FSMC_AccessMode							= FSMC_AccessMode_B;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank								= FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux			= FSMC_DataAddressMux_Enable;	//��ַ�����ݸ���
	FSMC_NORSRAMInitStructure.FSMC_MemoryType					= FSMC_MemoryType_PSRAM;			//�洢������
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth		= FSMC_MemoryDataWidth_16b;		//���ݿ��
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode		= FSMC_BurstAccessMode_Disable;	 //��ֹ����ģʽ����
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity	= FSMC_WaitSignalPolarity_Low;	 //�ȴ��źż���
	FSMC_NORSRAMInitStructure.FSMC_WrapMode						= FSMC_WrapMode_Disable;			//�������ģʽ
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive		= FSMC_WaitSignalActive_BeforeWaitState;	 //���õȴ�ʱ��
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation			= FSMC_WriteOperation_Enable;	//����FSMC�Դ洢��д����
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal					= FSMC_WaitSignal_Disable;		//�ȴ�ʹ��λ��ֹ
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode				= FSMC_ExtendedMode_Disable;	//��չģʽѡ��
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait		= FSMC_AsynchronousWait_Disable;	//��SRAM����
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst					= FSMC_WriteBurst_Disable;	//����дʹ��ѡ��
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct	= &pTimSet;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct			= &pTimSet;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	
	/* Enable FSMC Bank1_SRAM Bank	*/
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);	
}

/*******************************************************************************
* Function Name  : ������
* Description    : �ⲿSRAM���ԣ�ѡ�񼸸�λ��д��Ͷ���������Ƿ�һ�¡�
* Input          : ��
* Output         : һ���򷵻�0����һ���򷵻�1
* Return         : ���ز�����
*******************************************************************************/
uint32_t	TestReadWriteExtSRAM(void)
{
	uint32_t	i;
	uint8_t	* pWrite;
	uint8_t	* pRead;
	
	pWrite = (uint8_t	*)(ExtSRamBaseAddr);
	pRead  = (uint8_t	*)(ExtSRamBaseAddr);
	for (i = 0; i < 0x100; i++)
	{
		*pWrite = i;
		if (i != *pRead)
		{
			return	(1);
		}
		
		pWrite++;
		pRead++;
	}
	
	return	(0);
}

/*******************************************************************************
* Function Name  : ������
* Description    : �ⲿSRAM���ȼ��
* Input          : ��
* Output         : û��RAM�򷵻�0�����򷵻ؼ�鵽�ĳ��ȡ����1M����С64K��
* Return         : ���ز�����
*******************************************************************************/
uint32_t	GetExtSRAMLen(void)
{
	uint32_t	i;
	uint8_t	* pWrite;
	uint8_t	* pRead;
	
	for (i = EXT_SRAM_BYTE_LEN_1024K; i >= EXT_SRAM_BYTE_LEN_64K; i >>= 1)
	{
		pWrite = (uint8_t	*)(ExtSRamBaseAddr + i);
		pRead  = (uint8_t	*)(ExtSRamBaseAddr + i);
		pWrite--;
		pRead--;
		*pWrite = 0xAA;
		if (0xAA == *pRead)
		{
			pWrite--;
			pRead--;
			*pWrite = 0x55;
			if (0x55 == *pRead)
			{
				return	(i);
			}
		}
	}
	
	return	(0);
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
