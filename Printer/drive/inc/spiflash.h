/**
******************************************************************************
	* @file		x.h
	* @author	Application Team  
	* @version V0.0.1
	* @date		2016-3-18
	* @brief	 SPI�ֿ���صĳ���.
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
#ifndef SPIFLASH_H
#define SPIFLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "speaker.h"

/* SPIʹ��DMA����һ���޹��ֽ�,ʹ��RX�жϽ��մ��ص����� */

/* Macro Definition ----------------------------------------------------------*/
/**
	* @brief	M25P SPI Flash supported commands
	*	W25X32ϵ��FLASH�󲿷�ָ����֮��ͬ,�������������Ϳ������ָ�ͬ,��ȷ��
	*/
	/*
	W25Q32BV֧��SPIģʽ0(0,0),ģʽ3(1,1).
	ģʽ0ʱCLK�ź�����ʱΪ��.ģʽ1ʱCLK�ź�����ʱΪ��.
	W25Q32BVÿ����С�ɱ������Ϊ256�ֽ�,Ϊ1ҳpage.
	ÿ������sectorΪ4K,ÿһ��blockΪ32K/64K.
	��1024���ɲ�������/64���ɲ��������໥������.
	����ʱ����������Ϊ��λ,Ҳ�����Կ�Ϊ��λ.
	4K�����ı��Ϊ���ݺͲ����Ĵ洢�ṩ�˷���.
	*/
//����Ϊ����/���ָ��
#define sFLASH_CMD_WREN					0x06	/*!< Write enable instruction */
#define sFLASH_CMD_WRVSREN			0x50	/*!< Write enable for volatile status register instruction */
#define sFLASH_CMD_WRDISEN			0x04	/*!< Write disable instruction */

#define sFLASH_CMD_RDSR					0x05	/*!< Read Status Register 1 instruction,S7~S0	*/
																			/*!< ״̬�Ĵ�������S7~S0���������ϵ��ͳ�ֱ��/CS�жϸ�ָ�� */
#define sFLASH_CMD_RDSR1				0x05	/*!< Read Status Register 2 instruction,S7~S0	*/
																			/*!< ״̬�Ĵ�������S7~S0���������ϵ��ͳ�ֱ��/CS�жϸ�ָ�� */
#define sFLASH_CMD_RDSR2				0x35	/*!< Read Status Register 2 instruction,S15~S8 */
																			/*!< ״̬�Ĵ�������S18~S8���������ϵ��ͳ�ֱ��/CS�жϸ�ָ�� */
#define sFLASH_CMD_WRSR					0x01	/*!< Write Status Register instruction */

#define sFLASH_CMD_WRITE				0x02	/*!< Write to Memory instruction, */
																			/*!<ҳ���,�����3���ֽڵĵ�ַ(��1�ֽ�����), */
																			/*!< A23~A16,A15~A8,A7~A0,(D7~D0) */
#define sFLASH_CMD_PAGEWRITE		0x02	/*!< ҳд instruction,ͬsFLASH_CMD_WRITE */
#define sFLASH_CMD_SE						0x20	/*!< Sector Erase instruction,4K */
																			/*!< ��������,4K,�����3���ֽڵĵ�ַ */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_BE1					0x52	/*!< Block Erase instruction,32K */
																			/*!< �����,32K,�����3���ֽڵĵ�ַ */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_BE						0xD8	/*!< Block Erase instruction,64K */
																			/*!< �����,64K,�����3���ֽڵĵ�ַ */
																			/*!< A23~A16,A15~A8,A7~A0 */
#define sFLASH_CMD_EE1					0xC7	/*!< ȫƬ Erase instruction */
#define sFLASH_CMD_EE2					0x60	/*!< ȫƬ Erase instruction */
#define sFLASH_CMD_EPS					0x75	/*!< Erase/Program Suspend instruction */
#define sFLASH_CMD_EPR					0x7A	/*!< Erase/Program Resume instruction */
#define sFLASH_CMD_PD						0xB9	/*!< Power down instruction */

//����Ϊ������ָ��
#define sFLASH_CMD_READ					0x03	/*!< Read from Memory instruction */
																			/*!< �����3���ֽڵĵ�ַ(��1�ֽ�����), */
																			/*!< A23~A16,A15~A8,A7~A0,(D7~D0) */
#define sFLASH_CMD_FASTREAD			0x0B	/*!< fate Read from Memory instruction */
																			/*!< �����3���ֽڵĵ�ַ��1�ֽ��޹���(��1�ֽ�����), */
																			/*!< A23~A16,A15~A8,A7~A0,dummy,(D7~D0) */
#define sFLASH_CMD_RDID					0x9F	/*!< Read identification, ID15~ID0 */

//����ΪĳЩ��������
#define sFLASH_WIP_FLAG					0x01	/*!< Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE				0xA5
#define sFLASH_SPI_PAGESIZE			0x100		/* ҳ��С��Ҫ�����ֲ�ֵȷ��, 256�ֽ� */
#define sFLASH_SPI_SECTORSIZE		0x1000	/* ������С��Ҫ�����ֲ�ֵȷ��, 4K */
#define sFLASH_SPI_BLOCKSIZE1		0x8000	/* ���С1��Ҫ�����ֲ�ֵȷ��, 32K */
#define sFLASH_SPI_BLOCKSIZE2		0x10000	/* ���С2��Ҫ�����ֲ�ֵȷ��, 64K */
#define sFLASH_SPI_BLOCKSIZE		0x10000	/* ���С��Ҫ�����ֲ�ֵȷ��, 64K */


#define sFLASH_M25P128_ID				0x202018
#define sFLASH_M25P64_ID				0x202017
#define sFLASH_W25Q32_ID				0x4016

/* �ֿ�SPI���ƶ˿ڶ��� */
#define ZK_FLASH_RCC_APBPeriph_GPIO		RCC_APB2Periph_GPIOA
#define ZK_FLASH_RCC_APBPeriph_SPI		RCC_APB2Periph_SPI1
#define ZK_FLASH_SPI									SPI1
#define ZK_FLASH_PORT									GPIOA
#define ZK_FLASH_CS_PIN								GPIO_Pin_4
#define ZK_FLASH_CLK_PIN							GPIO_Pin_5
#define ZK_FLASH_MISO_PIN							GPIO_Pin_6
#define ZK_FLASH_MOSI_PIN							GPIO_Pin_7

/* Select SPI FLASH: ChipSelect pin low	*/
#define sFLASH_CS_LOW()			GPIO_ResetBits(ZK_FLASH_PORT, ZK_FLASH_CS_PIN)

/* Select SPI FLASH: ChipSelect pin high */
#define sFLASH_CS_HIGH()		GPIO_SetBits(ZK_FLASH_PORT, ZK_FLASH_CS_PIN)

#define SPI1_DR_ADDR				(SPI1_BASE + 0x0C)

#define	sFLASH_ReadBuffer		sFLASH_FastReadBuffer

/* Funtion Declare -----------------------------------------------------------*/
/**
	* @brief	High layer functions
	*/
void	sFLASH_DeInit(void);
void	sFLASH_LowLevel_Init(void);		//��ʼ���ܽź�ʱ��
void	sFLASH_Init(void);						//���øú���������ɳ�ʼ��
void	sFLASH_DMAInit(void);					//��ʼ���ֿ�SPI��DMA����
void	sFLASH_StartDMARx(uint32_t * Buffer_Rx);	//������оSPI��DMA����

void	sFLASH_EraseSector(uint32_t SectorAddr);	//��������,4K
void	sFLASH_EraseBlock(uint32_t BlockAddr);		//�����,64K
void	sFLASH_EraseAllChip(void);								//ȫƬɾ��

void	sFLASH_WritePage(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void	sFLASH_WriteBuffer(uint8_t * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
//void	sFLASH_ReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void	sFLASH_FastReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

uint32_t	sFLASH_ReadID(void);
void	sFLASH_StartReadSequence(uint32_t ReadAddr);

/**
	* @brief	Low layer functions
	*/
uint8_t	sFLASH_ReadByte(void);
uint8_t	sFLASH_SendByte(uint8_t byte);
uint16_t	sFLASH_SendHalfWord(uint16_t HalfWord);
void	sFLASH_WriteEnable(void);
void	sFLASH_WaitForWriteEnd(void);
void	WriteSPI_FlashParameter(uint8_t *Buf, uint8_t Len);

#ifdef __cplusplus
}
#endif

#endif /* SPIFLASH_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
