/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� spi.h
  * @����       �� strongerHuang
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��07��29��
  * @ժҪ       �� SPIͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _SPI_H
#define _SPI_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm32f10x.h"


/* �궨�� --------------------------------------------------------------------*/
#define PORT_SPI_CS               GPIOE
#define PORT_SPI_SCK              GPIOD
#define PORT_SPI_MISO            GPIOC //GPIOD
#define PORT_SPI_MOSI            GPIOC //GPIOD

// #define PORT_SPI_SCK              GPIOA
// #define PORT_SPI_MISO             GPIOA
// #define PORT_SPI_MOSI             GPIOA

#define PIN_SPI_CS                GPIO_Pin_15
#define PIN_SPI_SCK               GPIO_Pin_13//GPIO_Pin_7
#define PIN_SPI_MISO             GPIO_Pin_14// GPIO_Pin_4
#define PIN_SPI_MOSI              GPIO_Pin_15//GPIO_Pin_1

// #define PIN_SPI_SCK               GPIO_Pin_5
// #define PIN_SPI_MISO              GPIO_Pin_6
// #define PIN_SPI_MOSI              GPIO_Pin_7

#define SPI_CS_ENABLE             GPIO_ResetBits(PORT_SPI_CS, PIN_SPI_CS)
#define SPI_CS_DISABLE            GPIO_SetBits(PORT_SPI_CS, PIN_SPI_CS)

#define SPI_SCK_LOW               GPIO_ResetBits(PORT_SPI_SCK, PIN_SPI_SCK)
#define SPI_SCK_HIGH              GPIO_SetBits(PORT_SPI_SCK, PIN_SPI_SCK)

#define SPI_MISO_READ             GPIO_ReadInputDataBit(PORT_SPI_MISO,PIN_SPI_MISO)

#define SPI_MOSI_LOW              GPIO_ResetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)
#define SPI_MOSI_HIGH             GPIO_SetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)


/* �������� ------------------------------------------------------------------*/
void SPI_Initializes(void);
void SPI_WriteByte(uint8_t TxData);
uint8_t SPI_ReadByte(void);


#endif /* _SPI_H */

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
