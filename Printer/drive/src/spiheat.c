/**
	******************************************************************************
	* @file		x.c
	* @author	Application Team  
	* @version V0.0.1
	* @date		2012-5-28
	* @brief	 ��оSPI��صĳ���.
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
#include	"spiheat.h"
#include	"defconstant.h"
#include	"print.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name	: ������
* Description		: ����
* Input					: �������
* Output				: �������
* Return				: ���ز���
*******************************************************************************/
/**
	* @brief	DeInitializes the peripherals used by the SPI FLASH driver.
	* @param	None
	* @retval None
	*/
#if	0
void	sHeat_DeInit(void)
{
	sHeat_LowLevel_DeInit();
}
#endif

/*******************************************************************************
* Function Name	: ������
* Description		: �ֿ�FLASH��SPI���ߵĳ�ʼ��,�����ܽ�ʱ�Ӻ�IO��������
* Input					: �������
* Output				: �������
* Return				: ���ز���
*******************************************************************************/
void	sHeat_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*2012-7-31���,����SPI3/I2S3�Ĳ���������JTAG���Ź���(SPI3_NSS/I2S3_WS��JTDI,
	SPI3_SCK/I2S3_CK��JTDO),�����Щ���Ų���IO����������,(��ÿ�θ�λ��)��Ĭ�ϱ���
	ΪJTAG��;.����û�����������ø�SPI3/I2S3,����(�ڵ���ʱ)�ر�JTAG���л���SWD�ӿ�,
	����(�ڱ�׼Ӧ��ʱ)ͬʱ�ر�JTAG��SWD�ӿ�.��������ʼ��ʱ��ִ�йر�JTAG����.
	*/
	
	/* Enable SPIx GPIOx clocks */
	RCC_APB1PeriphClockCmd(SPI_HEAT_RCC_APBPeriph_SPI, ENABLE);
	RCC_APB2PeriphClockCmd(SPI_HEAT_RCC_APBPeriph_GPIO, ENABLE);
	
	/************ ����FLASH��SPI�˿� ****************/
	GPIO_InitStructure.GPIO_Pin		= SPI_HEAT_CLK_PIN | SPI_HEAT_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;			//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(SPI_HEAT_PORT, &GPIO_InitStructure);
	
	//GPIO_InitStructure.GPIO_Pin		= SPI_HEAT_MISO_PIN;
	//GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//����
	//GPIO_Init(SPI_HEAT_PORT, &GPIO_InitStructure);
}

/**
	* @brief	Initializes the peripherals used by the SPI FLASH driver.
	* @param	None
	* @retval None
	*/
void	sHeat_Init(void)
{
	SPI_InitTypeDef	SPI_InitStructure;	
//	uint16_t tmpreg = 0;
	uint32_t	i;
	
	/* check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPI_HEAT_SPI));   
	
	sHeat_LowLevel_Init();
	
	SPI_Cmd(SPI_HEAT_SPI, DISABLE);
	
	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//����Ҫ����,������Ҫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//������,36/8=4.5M
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//������,36/4=9M   
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_HEAT_SPI, &SPI_InitStructure);
	
	/* Get the SPIx CR2 value */
  //tmpreg = SPI_HEAT_SPI->CR2;
	//tmpreg |= SPI_CR2_TXDMAEN;
	/* Write to SPIx CR2 */
  //SPI_HEAT_SPI->CR2 = tmpreg;
	
	/*!< Enable the SPI_HEAT_SPI	*/
	SPI_Cmd(SPI_HEAT_SPI, ENABLE);
	
	for(i = 0; i < MAX_HEAT_DATA_BYTES; i++)
	{
		SPI_HeatSendByte(0x00);
	}
}

/*******************************************************************************
* Function Name  : sHeat_DMAInit(void)
* Description    : ���û�о�õ�SPI DMA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	sHeat_DMAInit(void)				//��ʼ����оSPI��DMA����
{
	DMA_InitTypeDef		DMA_InitStructure ;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
  /* DMA2 Channel3 configuration ����SPI3�ķ��� ----------------------------------------------*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI3_DR_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = 0;	//�����ڴ��ַ,sHeat_StartDMATx()����������ʱ����
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;	//���䷽��:���͵�Ŀ�ĵ�ַ
  DMA_InitStructure.DMA_BufferSize = BYTES_OF_ALL_ELEMENT;	//�����ֽ���
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//��оֻ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;		//Դ�����Զ�����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�ֽ�Ϊ��λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		//�ֽ�Ϊ��λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;				//ֻ����1��,��ѭ������
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	//�ܸ����ȼ�
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;				//�ڴ浽����ģʽ
  DMA_Init(DMA2_Channel2, &DMA_InitStructure);				//SPI3��TXͨ��
	
  /* Enable DMA2 Channel2 Transfer Complete interrupt */
  DMA_ITConfig(DMA2_Channel2, DMA_IT_TC, ENABLE);			//DMA�������
	
  /* Enable SPI3 DMA Tx request */
  SPI_I2S_DMACmd(SPI_HEAT_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
}

/*******************************************************************************
* Function Name  : SPI3TX_DMA
* Description    : ����SPI3��DMA����
* Input          : SPI3_Buffer_Tx :��Ҫ�������ݵĵ�ַ
* Output         : None
* Return         : None
*******************************************************************************/
void	sHeat_StartDMATx(uint32_t * Buffer_Tx)
{
	//���û�оSPI���͵ĵ�ַ�������ֽ���
	DMA2_Channel2->CMAR = (uint32_t)Buffer_Tx;		//Դ��ַ
	DMA2_Channel2->CNDTR = BYTES_OF_ALL_ELEMENT;	//�����ֽ���Ŀ
	DMA_Cmd(DMA2_Channel2, ENABLE);								//����DMA
}

/*******************************************************************************
* Function Name	: SPI_HeatSendByte(uint8_t Byte)
* Description	: ͨ��SPI�ӿ�,���ò�ѯ�ķ�ʽ,��1�ֽڴ�ӡ���������������ӡͷ
* Input			: Data: 1�ֽڴ�ӡ����
* Output		: None
* Return		: None
*******************************************************************************/
#if	0
uint16_t SPI_HeatSendByte(uint8_t Byte)
{
	#if	0
 /* Loop while DR register in not empty */
	while ((SPI_HEAT_SPI->SR & SPI_I2S_FLAG_TXE) == 0);
  /* Send byte through the SPI1 peripheral */
	SPI_HEAT_SPI->DR = Byte;
	#endif
	
	uint8_t	GetChar;
	
	/*!< Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI_HEAT_SPI, Byte);
	
	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	GetChar = SPI_I2S_ReceiveData(SPI_HEAT_SPI);
	
	return (GetChar);
}
#else
uint16_t SPI_HeatSendByte(uint8_t Byte)
{
	/*!< Loop while DR register in not empty */
	while (!(SPI_HEAT_SPI->SR & SPI_I2S_FLAG_TXE));	//�ȴ�TXE=1
	
	/*!< Send byte through the SPI peripheral */
	SPI_HEAT_SPI->DR = Byte;	//д��һ���ַ�,��ʼ����,ͬʱ���TXE
	
	/*!< Wait to receive a byte */
	while (!(SPI_HEAT_SPI->SR & SPI_I2S_FLAG_RXNE));	//�ȴ�RXNE=1
	
	/*!< Return the byte read from the SPI bus */
	return (uint8_t)(SPI_HEAT_SPI->DR);	//��SPI���յ�������,ͬʱ�����RXNE
}
#endif

/**
	* @brief	Erases the specified FLASH sector.
	* @param	SectorAddr: address of the sector to erase.
	* @retval None
	*/
void	sHeat_EraseSector(uint32_t SectorAddr)
{
	/*!< Send write enable instruction */
	sHeat_WriteEnable();

	/*!< Sector Erase */
	/*!< Select the FLASH: Chip Select low */
	
	/*!< Send Sector Erase instruction */
	sHeat_SendByte(sHEAT_CMD_SE);
	/*!< Send SectorAddr high nibble address byte */
	sHeat_SendByte((SectorAddr & 0xFF0000) >> 16);
	/*!< Send SectorAddr medium nibble address byte */
	sHeat_SendByte((SectorAddr & 0xFF00) >> 8);
	/*!< Send SectorAddr low nibble address byte */
	sHeat_SendByte(SectorAddr & 0xFF);
	/*!< Deselect the FLASH: Chip Select high */
	
	
	/*!< Wait the end of Flash writing */
	sHeat_WaitForWriteEnd();
}

/**
	* @brief	Erases the entire FLASH.
	* @param	None
	* @retval None
	*/
void	sHeat_EraseBulk(void)
{
	/*!< Send write enable instruction */
	sHeat_WriteEnable();
	
	/*!< Bulk Erase */
	/*!< Select the FLASH: Chip Select low */
	
	/*!< Send Bulk Erase instruction	*/
	sHeat_SendByte(sHEAT_CMD_BE);
	/*!< Deselect the FLASH: Chip Select high */
	

	/*!< Wait the end of Flash writing */
	sHeat_WaitForWriteEnd();
}

/**
	* @brief	Writes more than one byte to the FLASH with a single WRITE cycle 
	*				 (Page WRITE sequence).
	* @note	 The number of byte can't exceed the FLASH page size.
	* @param	pBuffer: pointer to the buffer	containing the data to be written
	*				 to the FLASH.
	* @param	WriteAddr: FLASH's internal address to write to.
	* @param	NumByteToWrite: number of bytes to write to the FLASH, must be equal
	*				 or less than "sHEAT_PAGESIZE" value.
	* @retval None
	*/
void	sHeat_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	/*!< Enable the write access to the FLASH */
	sHeat_WriteEnable();

	/*!< Select the FLASH: Chip Select low */
	
	/*!< Send "Write to Memory " instruction */
	sHeat_SendByte(sHEAT_CMD_WRITE);
	/*!< Send WriteAddr high nibble address byte to write to */
	sHeat_SendByte((WriteAddr & 0xFF0000) >> 16);
	/*!< Send WriteAddr medium nibble address byte to write to */
	sHeat_SendByte((WriteAddr & 0xFF00) >> 8);
	/*!< Send WriteAddr low nibble address byte to write to */
	sHeat_SendByte(WriteAddr & 0xFF);

	/*!< while there is data to be written on the FLASH */
	while (NumByteToWrite--)
	{
		/*!< Send the current byte */
		sHeat_SendByte(*pBuffer);
		/*!< Point on the next byte to be written */
		pBuffer++;
	}

	/*!< Deselect the FLASH: Chip Select high */
	

	/*!< Wait the end of Flash writing */
	sHeat_WaitForWriteEnd();
}

/**
	* @brief	Writes block of data to the FLASH. In this function, the number of
	*				 WRITE cycles are reduced, using Page WRITE sequence.
	* @param	pBuffer: pointer to the buffer	containing the data to be written
	*				 to the FLASH.
	* @param	WriteAddr: FLASH's internal address to write to.
	* @param	NumByteToWrite: number of bytes to write to the FLASH.
	* @retval None
	*/
void	sHeat_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % sHEAT_SPI_PAGESIZE;
	count = sHEAT_SPI_PAGESIZE - Addr;
	NumOfPage =	NumByteToWrite / sHEAT_SPI_PAGESIZE;
	NumOfSingle = NumByteToWrite % sHEAT_SPI_PAGESIZE;

	if (Addr == 0) /*!< WriteAddr is sHEAT_PAGESIZE aligned	*/
	{
		if (NumOfPage == 0) /*!< NumByteToWrite < sHEAT_PAGESIZE */
		{
			sHeat_WritePage(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /*!< NumByteToWrite > sHEAT_PAGESIZE */
		{
			while (NumOfPage--)
			{
				sHeat_WritePage(pBuffer, WriteAddr, sHEAT_SPI_PAGESIZE);
				WriteAddr +=	sHEAT_SPI_PAGESIZE;
				pBuffer += sHEAT_SPI_PAGESIZE;
			}

			sHeat_WritePage(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	else /*!< WriteAddr is not sHEAT_PAGESIZE aligned	*/
	{
		if (NumOfPage == 0) /*!< NumByteToWrite < sHEAT_PAGESIZE */
		{
			if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sHEAT_PAGESIZE */
			{
				temp = NumOfSingle - count;

				sHeat_WritePage(pBuffer, WriteAddr, count);
				WriteAddr +=	count;
				pBuffer += count;

				sHeat_WritePage(pBuffer, WriteAddr, temp);
			}
			else
			{
				sHeat_WritePage(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else /*!< NumByteToWrite > sHEAT_PAGESIZE */
		{
			NumByteToWrite -= count;
			NumOfPage =	NumByteToWrite / sHEAT_SPI_PAGESIZE;
			NumOfSingle = NumByteToWrite % sHEAT_SPI_PAGESIZE;

			sHeat_WritePage(pBuffer, WriteAddr, count);
			WriteAddr +=	count;
			pBuffer += count;

			while (NumOfPage--)
			{
				sHeat_WritePage(pBuffer, WriteAddr, sHEAT_SPI_PAGESIZE);
				WriteAddr +=	sHEAT_SPI_PAGESIZE;
				pBuffer += sHEAT_SPI_PAGESIZE;
			}

			if (NumOfSingle != 0)
			{
				sHeat_WritePage(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

/**
	* @brief	Reads a block of data from the FLASH.
	* @param	pBuffer: pointer to the buffer that receives the data read from the FLASH.
	* @param	ReadAddr: FLASH's internal address to read from.
	* @param	NumByteToRead: number of bytes to read from the FLASH.
	* @retval None
	*/
void	sHeat_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	/*!< Select the FLASH: Chip Select low */
	

	/*!< Send "Read from Memory " instruction */
	sHeat_SendByte(sHEAT_CMD_READ);

	/*!< Send ReadAddr high nibble address byte to read from */
	sHeat_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte to read from */
	sHeat_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	sHeat_SendByte(ReadAddr & 0xFF);

	while (NumByteToRead--) /*!< while there is data to be read */
	{
		/*!< Read a byte from the FLASH */
		*pBuffer = sHeat_SendByte(sHEAT_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer++;
	}

	/*!< Deselect the FLASH: Chip Select high */
	
}

/**
	* @brief	Reads FLASH identification.
	* @param	None
	* @retval FLASH identification
	*/
uint32_t sHeat_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

	/*!< Select the FLASH: Chip Select low */
	
	/*!< Send "RDID " instruction */
	sHeat_SendByte(0x9F);

	/*!< Read a byte from the FLASH */
	Temp0 = sHeat_SendByte(sHEAT_DUMMY_BYTE);

	/*!< Read a byte from the FLASH */
	Temp1 = sHeat_SendByte(sHEAT_DUMMY_BYTE);

	/*!< Read a byte from the FLASH */
	Temp2 = sHeat_SendByte(sHEAT_DUMMY_BYTE);

	/*!< Deselect the FLASH: Chip Select high */
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

/**
	* @brief	Initiates a read data byte (READ) sequence from the Flash.
	*	 This is done by driving the /CS line low to select the device, then the READ
	*	 instruction is transmitted followed by 3 bytes address. This function exit
	*	 and keep the /CS line low, so the Flash still being selected. With this
	*	 technique the whole content of the Flash is read with a single READ instruction.
	* @param	ReadAddr: FLASH's internal address to read from.
	* @retval None
	*/
void	sHeat_StartReadSequence(uint32_t ReadAddr)
{
	/*!< Select the FLASH: Chip Select low */
	

	/*!< Send "Read from Memory " instruction */
	sHeat_SendByte(sHEAT_CMD_READ);

	/*!< Send the 24-bit address of the address to read from -------------------*/
	/*!< Send ReadAddr high nibble address byte */
	sHeat_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte */
	sHeat_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte */
	sHeat_SendByte(ReadAddr & 0xFF);
}

/**
	* @brief	Reads a byte from the SPI Flash.
	* @note	 This function must be used only if the Start_Read_Sequence function
	*				 has been previously called.
	* @param	None
	* @retval Byte Read from the SPI Flash.
	*/
uint8_t sHeat_ReadByte(void)
{
	return (sHeat_SendByte(sHEAT_DUMMY_BYTE));
}

/**
	* @brief	Sends a byte through the SPI interface and return the byte received
	*				 from the SPI bus.
	* @param	byte: byte to send.
	* @retval The value of the received byte.
	*/
uint8_t sHeat_SendByte(uint8_t byte)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI_HEAT_SPI, byte);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_HEAT_SPI);
}

/**
	* @brief	Sends a Half Word through the SPI interface and return the Half Word
	*				 received from the SPI bus.
	* @param	HalfWord: Half Word to send.
	* @retval The value of the received Half Word.
	*/
uint16_t sHeat_SendHalfWord(uint16_t HalfWord)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
	/*!< Send Half Word through the sFLASH peripheral */
	SPI_I2S_SendData(SPI_HEAT_SPI, HalfWord);
	
	/*!< Wait to receive a Half Word */
	while (SPI_I2S_GetFlagStatus(SPI_HEAT_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	/*!< Return the Half Word read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI_HEAT_SPI);
}

/**
	* @brief	Enables the write access to the FLASH.
	* @param	None
	* @retval None
	*/
void	sHeat_WriteEnable(void)
{
	/*!< Select the FLASH: Chip Select low */
	

	/*!< Send "Write Enable" instruction */
	sHeat_SendByte(sHEAT_CMD_WREN);

	/*!< Deselect the FLASH: Chip Select high */
	
}

/**
	* @brief	Polls the status of the Write In Progress (WIP) flag in the FLASH's
	*				 status register and loop until write opertaion has completed.
	* @param	None
	* @retval None
	*/
void	sHeat_WaitForWriteEnd(void)
{
	uint8_t flashstatus = 0;

	/*!< Select the FLASH: Chip Select low */
	

	/*!< Send "Read Status Register" instruction */
	sHeat_SendByte(sHEAT_CMD_RDSR);

	/*!< Loop as long as the memory is busy with a write cycle */
	do
	{
		/*!< Send a dummy byte to generate the clock needed by the FLASH
		and put the value of the status register in FLASH_Status variable */
		flashstatus = sHeat_SendByte(sHEAT_DUMMY_BYTE);

	}
	while ((flashstatus & sHEAT_WIP_FLAG) == SET); /* Write in progress */

	/*!< Deselect the FLASH: Chip Select high */
	
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
