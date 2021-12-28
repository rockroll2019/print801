/**
	******************************************************************************
	* @file		x.c
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

/* Includes ------------------------------------------------------------------*/
#include	"spiflash.h"
#include	"charmaskbuf.h"
#include	"extgvar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern TypeDef_StructSysConfig	SysConfigStruct;
/* Private function prototypes -----------------------------------------------*/
extern void	DelayUs(uint32_t);
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
void	sFLASH_DeInit(void)
{
	sFLASH_LowLevel_DeInit();
}
#endif

/*******************************************************************************
* Function Name	: ������
* Description		: �ֿ�FLASH��SPI���ߵĳ�ʼ��,�����ܽ�ʱ�Ӻ�IO��������
* Input					: �������
* Output				: �������
* Return				: ���ز���
*******************************************************************************/
void	sFLASH_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable SPIx GPIOx clocks */
	RCC_APB2PeriphClockCmd(ZK_FLASH_RCC_APBPeriph_SPI, ENABLE);
	RCC_APB2PeriphClockCmd(ZK_FLASH_RCC_APBPeriph_GPIO, ENABLE);
	
	/************ ����FLASH��SPI�˿� ****************/
	GPIO_InitStructure.GPIO_Pin		= ZK_FLASH_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//����
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(ZK_FLASH_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= ZK_FLASH_CLK_PIN | ZK_FLASH_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;			//�������
	GPIO_Init(ZK_FLASH_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= ZK_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;			//�������
	GPIO_Init(ZK_FLASH_PORT, &GPIO_InitStructure);
}

/**
	* @brief	Initializes the peripherals used by the SPI FLASH driver.
	* @param	None
	* @retval None
	*/
void	sFLASH_Init(void)
{
	SPI_InitTypeDef	SPI_InitStructure;
	
	sFLASH_LowLevel_Init();
	
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	
	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(ZK_FLASH_SPI, &SPI_InitStructure);
	
	/*!< Enable the ZK_FLASH_SPI	*/
	SPI_Cmd(ZK_FLASH_SPI, ENABLE);
}

/*******************************************************************************
* Function Name  : sFLASH_DMAInit(void)
* Description    : �����ֿ��õ�SPI DMA
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	sFLASH_DMAInit(void)
{
	DMA_InitTypeDef		DMA_InitStructure ;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
  /* DMA1 Channel3 configuration ����SPI3�ķ��� ----------------------------------------------*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI1_DR_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = 0;	//�����ڴ��ַ,sFLASH_StartDMARx()����������ʱ����
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//���䷽��:������Ŀ�ĵ�ַ����
  DMA_InitStructure.DMA_BufferSize = MAX_BYTES_OF_HZ;	//�����ֽ���
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//��оֻ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;		//Դ�����Զ�����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�ֽ�Ϊ��λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		//�ֽ�Ϊ��λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;				//ֻ����1��,��ѭ������
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;	//�ܸ����ȼ�
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;				//�ڴ浽����ģʽ
	
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);				//SPI1��TXͨ��
	
  /* Enable DMA2 Channel2 Transfer Complete interrupt */
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);			//DMA�������
	
  /* Enable SPI3 DMA Tx request */
  SPI_I2S_DMACmd(ZK_FLASH_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
}

/**
	* @brief	Erases the specified FLASH sector.
	* @param	SectorAddr: address of the sector to erase.
	* @retval None
	*/
void	sFLASH_EraseSector(uint32_t SectorAddr)
{
	/*!< Send write enable instruction */
	sFLASH_WriteEnable();

	/*!< Sector Erase */
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send Sector Erase instruction */
	sFLASH_SendByte(sFLASH_CMD_SE);
	/*!< Send SectorAddr high nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/*!< Send SectorAddr medium nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/*!< Send SectorAddr low nibble address byte */
	sFLASH_SendByte(SectorAddr & 0xFF);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	
	/*!< Wait the end of Flash writing */
	sFLASH_WaitForWriteEnd();
}

/**
	* @brief	Erases the specified FLASH block.
	* @param	SectorAddr: address of the sector to erase.
	* @retval None
	*/
void	sFLASH_EraseBlock(uint32_t BlockAddr)
{
	/*!< Send write enable instruction */
	sFLASH_WriteEnable();

	/*!< Block Erase */
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send Block Erase instruction */
	sFLASH_SendByte(sFLASH_CMD_BE);
	/*!< Send BlockAddr high nibble address byte */
	sFLASH_SendByte((BlockAddr & 0xFF0000) >> 16);
	/*!< Send BlockAddr medium nibble address byte */
	sFLASH_SendByte((BlockAddr & 0xFF00) >> 8);
	/*!< Send BlockAddr low nibble address byte */
	sFLASH_SendByte(BlockAddr & 0xFF);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	
	/*!< Wait the end of Flash writing */
	sFLASH_WaitForWriteEnd();
}

/**
	* @brief	Erases the entire FLASH.
	* @param	None
	* @retval None
	*/
void	sFLASH_EraseAllChip(void)
{
	/*!< Send write enable instruction */
	sFLASH_WriteEnable();
	
	/*!< Bulk Erase */
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send Bulk Erase instruction	*/
	sFLASH_SendByte(sFLASH_CMD_EE1);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	
	/*!< Wait the end of Flash writing */
	sFLASH_WaitForWriteEnd();
}

/**
	* @brief	Writes more than one byte to the FLASH with a single WRITE cycle 
	*				 (Page WRITE sequence).
	* @note	 The number of byte can't exceed the FLASH page size.
	* @param	pBuffer: pointer to the buffer	containing the data to be written
	*				 to the FLASH.
	* @param	WriteAddr: FLASH's internal address to write to.
	* @param	NumByteToWrite: number of bytes to write to the FLASH, must be equal
	*				 or less than "sFLASH_PAGESIZE" value.
	* @retval None
	*/
void	sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	/*!< Enable the write access to the FLASH */
	sFLASH_WriteEnable();

	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Write to Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_WRITE);
	/*!< Send WriteAddr high nibble address byte to write to */
	sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/*!< Send WriteAddr medium nibble address byte to write to */
	sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	/*!< Send WriteAddr low nibble address byte to write to */
	sFLASH_SendByte(WriteAddr & 0xFF);

	/*!< while there is data to be written on the FLASH */
	while (NumByteToWrite--)
	{
		/*!< Send the current byte */
		sFLASH_SendByte(*pBuffer);
		/*!< Point on the next byte to be written */
		pBuffer++;
	}

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();

	/*!< Wait the end of Flash writing */
	sFLASH_WaitForWriteEnd();
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
void	sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
	count = sFLASH_SPI_PAGESIZE - Addr;
	NumOfPage =	NumByteToWrite / sFLASH_SPI_PAGESIZE;
	NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

	if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned	*/
	{
		if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
		{
			sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /*!< NumByteToWrite > sFLASH_PAGESIZE */
		{
			while (NumOfPage--)
			{
				sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
				WriteAddr +=	sFLASH_SPI_PAGESIZE;
				pBuffer += sFLASH_SPI_PAGESIZE;
			}

			sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	else /*!< WriteAddr is not sFLASH_PAGESIZE aligned	*/
	{
		if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
		{
			if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				sFLASH_WritePage(pBuffer, WriteAddr, count);
				WriteAddr +=	count;
				pBuffer += count;

				sFLASH_WritePage(pBuffer, WriteAddr, temp);
			}
			else
			{
				sFLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else /*!< NumByteToWrite > sFLASH_PAGESIZE */
		{
			NumByteToWrite -= count;
			NumOfPage =	NumByteToWrite / sFLASH_SPI_PAGESIZE;
			NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

			sFLASH_WritePage(pBuffer, WriteAddr, count);
			WriteAddr +=	count;
			pBuffer += count;

			while (NumOfPage--)
			{
				sFLASH_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
				WriteAddr +=	sFLASH_SPI_PAGESIZE;
				pBuffer += sFLASH_SPI_PAGESIZE;
			}

			if (NumOfSingle != 0)
			{
				sFLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

#if	0
/**
	* @brief	Reads a block of data from the FLASH.
	* @param	pBuffer: pointer to the buffer that receives the data read from the FLASH.
	* @param	ReadAddr: FLASH's internal address to read from.
	* @param	NumByteToRead: number of bytes to read from the FLASH.
	* @retval None
	*/
void	sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_READ);

	/*!< Send ReadAddr high nibble address byte to read from */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte to read from */
	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	sFLASH_SendByte(ReadAddr & 0xFF);

	while (NumByteToRead--) /*!< while there is data to be read */
	{
		/*!< Read a byte from the FLASH */
		*pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer++;
	}

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}
#endif

/**
	* @brief	Reads a block of data from the FLASH as fast as FLASH can.
	* @param	pBuffer: pointer to the buffer that receives the data read from the FLASH.
	* @param	ReadAddr: FLASH's internal address to read from.
	* @param	NumByteToRead: number of bytes to read from the FLASH.
	* @retval None
	*	�����������ͬsFLASH_ReadBuffer(),���������sFLASH_CMD_FASTREAD,
	*	�����3���ֽڵĵ�ַ��1�ֽ��޹���(��1�ֽ�����),
	*	A23~A16,A15~A8,A7~A0,dummy,(D7~D0)
	*/
void	sFLASH_FastReadBuffer(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "fast Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_FASTREAD);

	/*!< Send ReadAddr high nibble address byte to read from */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte to read from */
	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	sFLASH_SendByte(ReadAddr & 0xFF);
	/*�������һ���޹��ֽ�*/
	sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	
	while (NumByteToRead--) /*!< while there is data to be read */
	{
		/*!< Read a byte from the FLASH */
		*pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer++;
	}

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}


/**
	* @brief	Reads FLASH identification.
	* @param	None
	* @retval FLASH identification
	*/
uint32_t sFLASH_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "RDID " instruction */
	sFLASH_SendByte(0x9F);

	/*!< Read a byte from the FLASH */
	Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

	/*!< Read a byte from the FLASH */
	Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

	/*!< Read a byte from the FLASH */
	Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();

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
void	sFLASH_StartReadSequence(uint32_t ReadAddr)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_READ);

	/*!< Send the 24-bit address of the address to read from -------------------*/
	/*!< Send ReadAddr high nibble address byte */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte */
	sFLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte */
	sFLASH_SendByte(ReadAddr & 0xFF);
}

/**
	* @brief	Reads a byte from the SPI Flash.
	* @note	 This function must be used only if the Start_Read_Sequence function
	*				 has been previously called.
	* @param	None
	* @retval Byte Read from the SPI Flash.
	*/
uint8_t sFLASH_ReadByte(void)
{
	return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}

/**
	* @brief	Sends a byte through the SPI interface and return the byte received
	*				 from the SPI bus.
	* @param	byte: byte to send.
	* @retval The value of the received byte.
	*/
#if	0
uint8_t sFLASH_SendByte(uint8_t byte)
{
	/*!< Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(ZK_FLASH_SPI, byte);

	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(ZK_FLASH_SPI);
}
#endif
uint8_t sFLASH_SendByte(uint8_t byte)
{
	/*!< Loop while DR register in not empty */
	//while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	while (!(ZK_FLASH_SPI->SR & SPI_I2S_FLAG_TXE));	//�ȴ�TXE=1
	
	/*!< Send byte through the SPI1 peripheral */
	//SPI_I2S_SendData(ZK_FLASH_SPI, byte);
	ZK_FLASH_SPI->DR = byte;	//д��һ���ַ�,��ʼ����,ͬʱ���TXE
	
	/*!< Wait to receive a byte */
	//while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	while (!(ZK_FLASH_SPI->SR & SPI_I2S_FLAG_RXNE));	//�ȴ�RXNE=1
	
	/*!< Return the byte read from the SPI bus */
	//return SPI_I2S_ReceiveData(ZK_FLASH_SPI);
	return (uint8_t)(ZK_FLASH_SPI->DR);	//��SPI���յ�������,ͬʱ�����RXNE
}


/**
	* @brief	Sends a Half Word through the SPI interface and return the Half Word
	*				 received from the SPI bus.
	* @param	HalfWord: Half Word to send.
	* @retval The value of the received Half Word.
	*/
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
	/*!< Send Half Word through the sFLASH peripheral */
	SPI_I2S_SendData(ZK_FLASH_SPI, HalfWord);
	
	/*!< Wait to receive a Half Word */
	while (SPI_I2S_GetFlagStatus(ZK_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	/*!< Return the Half Word read from the SPI bus */
	return SPI_I2S_ReceiveData(ZK_FLASH_SPI);
}

/**
	* @brief	Enables the write access to the FLASH.
	* @param	None
	* @retval None
	*/
void	sFLASH_WriteEnable(void)
{
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Write Enable" instruction */
	sFLASH_SendByte(sFLASH_CMD_WREN);

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}

/**
	* @brief	Polls the status of the Write In Progress (WIP) flag in the FLASH's
	*				 status register and loop until write opertaion has completed.
	* @param	None
	* @retval None
	*/
void	sFLASH_WaitForWriteEnd(void)
{
	uint8_t flashstatus = 0;

	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();

	/*!< Send "Read Status Register" instruction */
	sFLASH_SendByte(sFLASH_CMD_RDSR);

	/*!< Loop as long as the memory is busy with a write cycle */
	do
	{
		/*!< Send a dummy byte to generate the clock needed by the FLASH
		and put the value of the status register in FLASH_Status variable */
		flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);

	}
	while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */

	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : WriteSPI_FlashParameter(uint8_t *Buf, uint8_t Len )
* Description    : дά��������
* Input          : Buf:��Ҫд������ݵĵ�ַ,Len�����ݳ���	
* Output         : None
* Return         : None
*******************************************************************************/
void WriteSPI_FlashParameter(uint8_t *Buf, uint8_t Len)
{
	uint8_t j, Temp[sizeof(g_tSysConfig)];
	
	g_bWriteFlashFlag = 1;
    
	for(j=0; j<4; j++)
	{
		sFLASH_EraseSector(SYS_PARA_BASE_ADDR); 
		sFLASH_WriteBuffer(Buf, SYS_PARA_BASE_ADDR, Len);
		sFLASH_ReadBuffer(Temp, SYS_PARA_BASE_ADDR, Len );
		if( memcmp((char *)Temp, (char *)Buf, Len) == 0 )		//�Ƚ�д���Ƿ���ȷ
        {    
			break;
        }    
	}
	if(j==4)
    {    
		g_tError.UR_ErrorFlag |= 0x40;		                    //spi flash ��д����
    }    
	else
    {    
		g_tError.UR_ErrorFlag &= ~0x40;    
    }
    g_bWriteFlashFlag = 0;
}
/*******************************************************************************
* Function Name  : void	ReadSysParameter(void)
* Description    : �Ӵ���Flash�ж�ϵͳ���ã���û��ϵͳ���ã���д��Ĭ�ϵ�ϵͳ����
* Input          : ��
* Output         : ��ʼ��ϵͳ�������ݽṹ
* Return         : None
*******************************************************************************/
uint8_t ReadSysParameter(TypeDef_StructSysConfig *Prt)
{
	uint16_t Len, i;
   
    Len = sizeof( SysConfigStruct );
	
	//��ϵͳ���������������10�����ǲ��������ù��ı�־��������ΪĬ��ֵ
	for(i=0; i<10; i++)
	{
    sFLASH_ReadBuffer((uint8_t *)Prt, SYS_PARA_BASE_ADDR, Len );
		//if( !strcmp((char *)Prt->Version, (char *)SysDefaultValue.Version))
		//break;
		DelayUs(10000);				//��ʱ10ms
	}
    //if(i ==10)
    //  return 1;
    //else
        return 0;  
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
