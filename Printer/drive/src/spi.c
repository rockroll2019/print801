/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� spi.c
  * @����       �� strongerHuang
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��07��29��
  * @ժҪ       �� SPIԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-07-29 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "spi.h"


/************************************************
�������� �� SPI_Delay
��    �� �� SPI��ʱ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SPI_Delay(void)
{
  uint16_t cnt = 5;

  while(cnt--);
}

/************************************************
�������� �� SPI_GPIO_Configuration
��    �� �� SPI��������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SPI_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* CS */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_CS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_CS, &GPIO_InitStructure);

  /* SCK */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_SCK;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_SCK, &GPIO_InitStructure);

  /* MISO */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(PORT_SPI_MISO, &GPIO_InitStructure);

  /* MOSI */
  GPIO_InitStructure.GPIO_Pin = PIN_SPI_MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(PORT_SPI_MOSI, &GPIO_InitStructure);
}

/************************************************
�������� �� SPI_Initializes
��    �� �� SPI��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SPI_Initializes(void)
{
  SPI_GPIO_Configuration();

  SPI_CS_DISABLE;
  SPI_SCK_HIGH;
  SPI_MOSI_HIGH;
}

/************************************************
�������� �� SPI_WriteByte
��    �� �� SPIдһ�ֽ�����
��    �� �� TxData --- ���͵��ֽ�����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SPI_WriteByte(uint8_t TxData)
{
  uint8_t cnt;

  for(cnt=0; cnt<8; cnt++)
  {
    SPI_SCK_LOW;                                 //ʱ�� - ��
    SPI_Delay();

    if(TxData & 0x80)                            //��������
      SPI_MOSI_HIGH;
    else
      SPI_MOSI_LOW;
    TxData <<= 1;

    SPI_Delay();
    SPI_SCK_HIGH;                                //ʱ�� - ��
    SPI_Delay();
  }
}

/************************************************
�������� �� SPI_ReadByte
��    �� �� SPI��һ�ֽ�����
��    �� �� ��
�� �� ֵ �� ���������ֽ�����
��    �� �� strongerHuang
*************************************************/
uint8_t SPI_ReadByte(void)
{
  uint8_t cnt;
  uint8_t RxData = 0;

  for(cnt=0; cnt<8; cnt++)
  {
    SPI_SCK_LOW;                                 //ʱ�� - ��
    SPI_Delay();

    RxData <<= 1;
    if(SPI_MISO_READ)                            //��ȡ����
    {
      RxData |= 0x01;
    }

    SPI_SCK_HIGH;                                //ʱ�� - ��
    SPI_Delay();
  }

  return RxData;
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
