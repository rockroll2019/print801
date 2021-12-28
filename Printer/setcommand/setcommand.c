/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-28
  * @brief   ���ò���ָ����صĳ���.
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
#include	"setcommand.h"
#include	"spiflash.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"charmaskbuf.h"
#include	"defconstant.h"
#include	"uip.h"
#include  "cutter.h"
#include  "speaker.h"
#include "spi.h"
#include "sflash.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t	m_bComBaudRate;
static uint8_t	m_bComProtocol;
extern void WriteFlashParameter(uint8_t Flag);
extern void	Update(void);

extern u8  *PrtModeString[];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
/*******************************************************************************
* Function Name  : ReadSetByte(uint8_t *Parameter,uint8_t MaxValue)
* Description    : ��ȡ���õĲ���ֵ�����ж��Ƿ���Ч������Ч������Ӧ�ĵ�ַ��
	�����ϴ�0Xff����ʾ����
* Input          : Parameter��������ַ�� MaxValue�����������ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void ReadSetByte(uint8_t *Parameter, uint8_t MaxValue)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar < MaxValue) || ((GetChar >= 0x30) && (GetChar < (MaxValue + 0x30))))
	{
		*Parameter = GetChar & 0xCF;	//���������ΪASCII��������֣���0x01����0x31
	}
	else
	{
		GetChar = 0xFF;								//��������,�ϴ�0Xff
	}
	
	UpLoadData(&GetChar, 1);				//�ϴ�����
}

/*******************************************************************************
* Function Name  : ReceiverString(uint8_t *Str ,uint8_t Length)
* Description    : �����ַ���
* Input          : Str:�����ַ�����ַ��Length���ַ�������
* Output         : None
* Return         : None
*******************************************************************************/
void ReceiverString(uint8_t *Str, uint8_t Length)
{
 	uint8_t i;

	for (i = 0; i < Length; i++)
	{
		*Str = ReadInBuffer();
		Str++;
	}
	*Str = 0;
}


/*******************************************************************************
* Function Name  : SetCommand_1F12
* Description    : ���ò��Դ�ӡģʽ
* Input          : None
* Output         : None
* Return         : None  
*******************************************************************************/
void SetCommand_1F12( void )
{
	  ReadSetByte(&g_tSysConfig.PrintMode, 6);
    PrintString("Print Test Mode��");   
    PrintString(PrtModeString[g_tSysConfig.PrintMode]);    
    PrintString("\r\n"); 
	  GoDotLine(CUT_OFFSET);			//�ߵ�˺ֽλ�� 2016.05.31
}
//����wifi����
void CloseWiFICommand(void)
{
	uint8_t DataBuf[40]={0},Status=0,i=0;
	DelayMs(200);
	for(i=0;i<3;i++)
	{
		Status = WIFI_ReadCommand("AT+SAVE\r", DataBuf);
		if(Status == 0)
			break;
	}
	DelayMs(100);
	Status = WIFI_ReadCommand("AT+REBOOT\r", DataBuf);
	DelayMs(300);
}
/*******************************************************************************
* Function Name  : SetCommand_1F1F
* Description    : �˳�����ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F1F(void)
{
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
	
//	if (g_tSysConfig.ComBaudRate != m_bComBaudRate)	//����Ƿ�ı���
//		g_tSysConfig.ComBaudRate = m_bComBaudRate;
//	if (g_tSysConfig.ComProtocol != m_bComProtocol)
//		g_tSysConfig.ComProtocol = m_bComProtocol;
	
	if( g_tInterface.BT_Type)		//�������ӿڰ�
 	{
		CloseSetupBluetooth();				
	}
	else if(g_tInterface.WIFI_Type) 	  //2017.10.25
	{
		CLI();
		CloseWiFICommand();
		SEI();
	}
	if(1 == Set_BTWFflg)
	{
		Set_BTWFflg = 0;
		DriveCutPaper(CUT_TYPE_DEFAULT);	//����ֽ
		
	}
	SysConfigUnionData.SysConfigStruct = g_tSysConfig;
	WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
	g_tSystemCtrl.SetMode = 0;
  DelayMs(200);						//��ʱ
	WriteFlashParameter(2);		        // ����ǰ���ݱ��棬��ֹ����д��ʱ���� 2016.05.31
  NVIC_SystemReset();			  				//��λ
}

/*******************************************************************************
* Function Name  : SetCommand_1F18
* Description    : ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F18(void)
{
	ReadSetByte(&g_tSysConfig.SysLanguage, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F28
* Description    : ����ָ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F28(void)
{
	ReadSetByte(&g_tSysConfig.CommandSystem, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F39
* Description    : �����е�����о����,��4λ�����е���00��ʾ���е�����4λ��ʾ��о����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F39(void)
{
	uint8_t GetChar, Temp;

	GetChar = ReadInBuffer();
	Temp = (GetChar & 0xf0) >> 4;
	if (Temp < 2)									//��������2���е�
		g_tSysConfig.CutType = Temp;
	else
		GetChar = 0xFF;
	UpLoadData(&GetChar, 1);					//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F13
* Description    : �������������ƣ��16�ַ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F13(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.Manufacturer, GetChar);
	UpLoadData(g_tSysConfig.Manufacturer, GetChar);		//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F14
* Description    : ���ô�ӡ���ͺţ��16�ַ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F14(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.Model, GetChar);
	UpLoadData(g_tSysConfig.Model, GetChar);			//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F2F
* Description    : ���ô�ӡ�����кţ��16�ַ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2F(void)
{
	uint8_t GetChar;

	GetChar = ReadInBuffer();
	if (GetChar > 16)
		GetChar = 16;	
	ReceiverString(g_tSysConfig.SerialNumber, GetChar);
	UpLoadData(g_tSysConfig.SerialNumber, GetChar);		//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F15
* Description    : ���ô�ӡ�ٶ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F15(void)
{
	ReadSetByte(&g_tSysConfig.MaxSpeed, 4);
}

/*******************************************************************************
* Function Name  : SetCommand_1F2E
* Description    : ���÷�����ʹ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2E(void)
{
	ReadSetByte(&g_tSysConfig.BuzzerEnable, 4);
}

/*******************************************************************************
* Function Name  : SetCommand_1F1A
* Description    : ���ô�ӡ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F1A(void)
{
	ReadSetByte(&g_tSysConfig.PrintWidthSelect, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F19
* Description    : ��������,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F19(void)
{
	ReadSetByte(&g_tSysConfig.SysFontSize, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F44
* Description    : ���úڱ�ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F44(void)
{
	ReadSetByte(&g_tSysConfig.BlackMarkEnable, 3);			//0��ֽͨ 1�ڱ�ֽ 2��ǩֽ
}
/*******************************************************************************
* Function Name  : SetCommand_1F49
* Description    : ����ȡֽ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F49(void)
{
	ReadSetByte(&g_tSysConfig.PaperTakeAwaySensorEnable, 3);			//0�رչ���  1��������δȡ�� 2�����е�������ʾ 
}
/*******************************************************************************
* Function Name  : SetCommand_1F4A
* Description    : ����ȡֽ���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F4A(void)
{
	ReadSetByte(&g_tSysConfig.PaperTakeAwaySensorSensitive, 3);			//0 1 2Ϊ���и�
}
/*******************************************************************************
* Function Name  : SetCommand_1F2C
* Description    : ���ô��ڲ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F2C(void)
{
	uint8_t GetChar, Temp;
	
	GetChar = ReadInBuffer();
	
	g_tSysConfig.ComBaudRate = GetChar & 0x87;
	Temp = (~GetChar & 0x18) >> 1;
	Temp |= ((~GetChar & 0x60) >> 5);
	g_tSysConfig.ComProtocol = Temp;
	
//	m_bComBaudRate = GetChar & 0x87;
//	m_bComProtocol = Temp;
	
	UpLoadData(&GetChar, 1);					//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F43
* Description    : ����ֽ������������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F43(void)
{
	ReadSetByte(&g_tSysConfig.PaperSensorSensitive, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F27
* Description    : ����ȱֽ�ش�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F27(void)
{
	ReadSetByte(&g_tSysConfig.PaperOutReprint, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F3B
* Description    : ����0D�س�ָ���Ƿ���Ч
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F3B(void)
{
	ReadSetByte(&g_tSysConfig.CREnable, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F30
* Description    : ����USB IDʹ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F30(void)
{
	ReadSetByte(&g_tSysConfig.USBIDEnable, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F3E
* Description    : ���ó�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F3E(void)		//2016.05.31
{
  uint8_t GetChar;

	GetChar = ReadInBuffer();
	if(GetChar > 3)		         // �ĸ����� ��һ��Ϊʱ��λ�� Ĭ��Ϊ3�� �����һλ���·���λ �磺505
	{	
		GetChar = 3;
	}	
	ReceiverString(g_tSysConfig.ProductiveData,GetChar);
	UpLoadData(g_tSysConfig.ProductiveData, GetChar);		//�ϴ�����
  PrintString("Production Date:");	
  PrintString(g_tSysConfig.ProductiveData);
  PrintString("\r\n");
  GoDotLine(CUT_OFFSET);			//�ߵ�˺ֽλ��
}

/*******************************************************************************
* Function Name  : SetCommand_1F17
* Description    : ���ô�ӡ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F17(void)
{
	ReadSetByte(&g_tSysConfig.PrintDir, 2);
}

/*******************************************************************************
* Function Name  : SetCommand_1F55
* Description    : ���ô���ҳ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F55(void)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar < MAX_CODEPAGE_SET_NUM)
		g_tSysConfig.DefaultCodePage = GetChar;
	else
		GetChar = 0xFF;
	UpLoadData(&GetChar, 1);					//�ϴ�����
}
/*******************************************************************************
* Function Name  : SetCommand_1F62
* Description    : ����ֽ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F62(void)   //2017.03.29
{
	uint8_t GetChar;
	
	ReadSetByte(&g_tSysConfig.PaperWith, 2);
}
/*******************************************************************************
* Function Name  : SetCommand_1FA1
* Description    : �ָ�����Ĭ�ϲ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1FA1(void)   //2018.08.16
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if(GetChar == 0x01)
	{
		  RestoreDefaultParas();
			ClearComInBufBusy();
      GoDotLine(20);		        //�ߵ���ֽλ��
      PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n\n\n\n\n\n\n\n\n");
      while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 );	//�ȴ�ǰ���м������
		  DelayMs(200);
			WriteFlashParameter(2);		        // ����ǰ���ݱ��棬��ֹ����д��ʱ���� 2016.05.31  
      NVIC_SystemReset();			  				//��λ
	}	
}

/*******************************************************************************
* Function Name  : SetCommand_1F42
* Description    : ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F42( void )       
{
    uint8_t Temp[7];
    uint8_t i,PrintInterface;
    
    for(i=0; i<7; i++)
    {
        Temp[i] =  ReadInBuffer();
    }
    if( memcmp(Temp,"upgrade",7) == 0  )
    {    
        //PrintInterface = g_tInterface.COM_Type;
        Update();
    }
    else
    {

    }    
} 
/*******************************************************************************
* Function Name  : SetCommand_1F16
* Description    : ���ô�ӡŨ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F16(void)
{
	ReadSetByte(&g_tSysConfig.Density, 3);
}

/*******************************************************************************
* Function Name  : SetCommand_1F60
* Description    : ����DHCP����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F60( void )
{
	ReadSetByte(&g_tSysConfig.DHCPEnable, 2);
}
/*******************************************************************************
* Function Name  : SetCommand_1F63
* Description    : ��������ģ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F63(void)
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	if(GetChar > 0x03)
		return;
	
	if(0 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERLOW);
	}
	else if(1 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERMIDDLE);
	}
	else if(2 == GetChar)
	{
		Line_2A_WTN588H_Dly(SPEAKERHIGHT);
	}
	else if(SPEAKECLOSED == GetChar)  //�ر�������·
	{
		g_tSpeaker.SpeakCtrl = 0;  //�ر�����
	}

	g_tSysConfig.SpeakerVolume = GetChar;
	if((g_tSpeaker.SpeakElecCtrl == 1)&&(GetChar<=2))
	{
		g_tSpeaker.SpeakCtrl = 1;
	}
		UpLoadData(&GetChar, 1);					//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F64
* Description    : ������������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F64(void)
{
	uint8_t GetChar;
	uint16_t TempNum;
	
	GetChar = ReadInBuffer();
	if(GetChar >= 0 && GetChar <40)	
	{
		g_tSysConfig.AlarmNumber = 0;     //(GetChar+11);  //2018.06.23��Ҫ����   ��ӡȡ������ѡ��
    g_tSysConfig.AlarmNumber = g_tSysConfig.AlarmNumber|GetChar;
		Line_2A_WTN588H_Dly(g_tSysConfig.AlarmNumber);

		UpLoadData(&GetChar, 1);					      //�ϴ�����
	}	
}


/*******************************************************************************
* Function Name  : SetCommand_1F75
* Description    : ����ȱֽ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F75(void) 
{
	uint8_t GetChar;
	
	GetChar = ReadInBuffer();
	
	if(GetChar >= 0 && GetChar <40)						      //ȱֽ����������оƬ
	{
		g_tSysConfig.POutVoiceNumber = 0;     //2018.06.23��Ҫ����    ������11��λ�ÿ�ʼ Ҳ���Ը�Ϊ1��ʼ ���߾������
    g_tSysConfig.POutVoiceNumber = g_tSysConfig.POutVoiceNumber|GetChar;
		Line_2A_WTN588H_Dly(g_tSysConfig.POutVoiceNumber);

		UpLoadData(&GetChar, 1);					//�ϴ�����
	}	
}
/*******************************************************************************
* Function Name  : SetCommand_1F74
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//��588H����flash��д��������
void SetCommand_1F74(void)
{
	uint8_t BuffCRC[2]={0},ceshi[256],cshi2[256];
	u32 size_1 =0,SectorAddress=0,i;
  uint8_t TempFlag;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
  TempFlag = 0;
	for(i=0; i<2; i++)
	{
		BuffCRC[i] = ReadInBuffer();
	}
	if((BuffCRC[0] == 0x55)&&(BuffCRC[1] == 0x31))
	{
		g_tSpeaker.SpeakWrite = 1;  //��������flash		
		Line_2A_WTN588H_Dly(0xFFFD);  //�Ͽ�����оƬ�ڲ�flash
	}
	
	
	SPI_Initializes();      //2018.06.22  ����оƬ��spi��ʼ����Ӱ���������� ֻ����������ʱ��io�ڳ�ʼ��

  SFLASH_EraseChip();     //2018.06.22 ������Ƭ

	DelayMs(300);          //��ʱ
	
  g_tSpeaker.SpeakWriteHandle = WRITE_START;
	while (1)
	{
		for (i=0; i < 256; i++)
		{
			cshi2[i] = ReadInBuffer();
			if (g_tSpeaker.SpeakWriteHandle == WRITE_END)
			{
				break;
			}
		}
		SFLASH_WriteNByte(cshi2, SectorAddress, i);
		DelayMs(1);
		SFLASH_ReadNByte(ceshi,SectorAddress,i);
		if(ceshi[0] !=cshi2[0] )
		{
			TempFlag = 1;
			break;
		}
		SectorAddress += 256;
		i = 0;
		
		if(g_tSpeaker.SpeakWriteHandle == WRITE_END)
			break;
	}
	if(TempFlag == 1)
	{
		UpLoadData("ERROR", 4);					//�ϴ����� 2018.06.23 ���󷵻� ��λ���ж�
	}
  else
  {		
		UpLoadData("OK", 2);					  //�ϴ�����
		//������ַ�ظ���00H��ȱֽ��ַ�ָ���01H
		g_tSysConfig.POutVoiceNumber = 1; 
		g_tSysConfig.AlarmNumber = 0;
		SysConfigUnionData.SysConfigStruct = g_tSysConfig;
		WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
	}	
	DelayMs(400);
	g_tSpeaker.SpeakWrite = 0;	//�ͷ�����flash
	g_tSpeaker.SpeakWriteHandle = 0;
	g_tSystemCtrl.SetMode = 0;
	NVIC_SystemReset();			  				//��λ
	
}
/*******************************************************************************
* Function Name  : SetCommand_1F34
* Description    : ����λͼ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	MAX_FLASH_BIT_BMP_BYTE		(64*1024 - 5)
void SetCommand_1F34(void)
{
	uint8_t	FlashBitBmpNum;
	uint32_t FlashBitBmpByte, SectorAddress, i;
	uint32_t	iMaxBitBmpByte;
	
	FlashBitBmpNum = ReadInBuffer();		//λͼ��
	g_tLineEditBuf[0] = FlashBitBmpNum;
	SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;

	for (i = 1; i < 5; i++)						 //��ȡλͼ��С
		g_tLineEditBuf[i] = ReadInBuffer();
	
 	iMaxBitBmpByte = \
		(g_tLineEditBuf[1] + 256 * g_tLineEditBuf[2]) * (g_tLineEditBuf[3] + 256 * g_tLineEditBuf[4]);
	if (iMaxBitBmpByte > MAX_FLASH_BIT_BMP_BYTE)
		FlashBitBmpByte = MAX_FLASH_BIT_BMP_BYTE;
	else
		FlashBitBmpByte = iMaxBitBmpByte;
	
	//if (FlashBitBmpByte == 0 || FlashBitBmpNum > 7)	//��Чλͼ����
	if (FlashBitBmpNum > 7)		//��Чλͼ����
	{
		UpLoadData("ERROR", 5);					//�ϴ�����
		return;
	}
	//SPI_FLASH_BlockErase( SectorAddress ); 	//ɾ����Ӧ����
	sFLASH_EraseBlock( SectorAddress ); 	//ɾ����Ӧ����
	
	while (FlashBitBmpByte > 0)
	{
		for (; i < 256; i++)
		{
			g_tLineEditBuf[i] = ReadInBuffer();
			FlashBitBmpByte--;
			if (FlashBitBmpByte == 0)
			{
				i++;
				break;
			}
		}
		sFLASH_WritePage(g_tLineEditBuf, SectorAddress, i);
		SectorAddress += 256;
		i = 0;
	}
	
	if (iMaxBitBmpByte > MAX_FLASH_BIT_BMP_BYTE)	//��ȡ��������ݶ���
	{
		for (i = 0; i < (iMaxBitBmpByte - MAX_FLASH_BIT_BMP_BYTE); i++)
			ReadInBuffer();
	}
	
	UpLoadData("OK", 2);					//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommandFun
* Description    : ѡ������ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommandFun(void)
{
	uint8_t GetChar;
	uint16_t Len;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
	
	//��ȡFLASH�еĲ���
	Len = sizeof( g_tSysConfig );	
	sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
	g_tSysConfig = SysConfigUnionData.SysConfigStruct;
	
	m_bComBaudRate = g_tSysConfig.ComBaudRate;
	m_bComProtocol = g_tSysConfig.ComProtocol;
	
	g_tSystemCtrl.SetMode = 1;
	if(g_tInterface.BT_Type)  		//�����ӿ�
 	{
		OpenSetupBluetooth();				
	}
	if(g_tInterface.WIFI_Type)				//2017.10.25
	{
			USART_ITConfig(WIFI_COM, USART_IT_RXNE, DISABLE);	
			WIFI_TranS_Config();                        //��͸������>ָ��ģʽ
			USART_ITConfig(WIFI_COM, USART_IT_RXNE, ENABLE);	
	}   
	
	while (g_tSystemCtrl.SetMode)
	{
		GetChar = ReadInBuffer();
		if (GetChar == 0x1B)				//������ESCָ� ���ݵ�����ָ��
		{
			GetChar = ReadInBuffer();
			if(g_tInterface.BT_Type)	  //Ϊ�������ͻ���
			{
				SetBluetooth(0X1B,GetChar);
			}
			else if(g_tInterface.WIFI_Type)	  //2017.10.25
			{
				SetWiFICommand(0X1B,GetChar);
			}
			switch( GetChar )
			{
				case 0x0A:			
				{
					SetCommand_1F12();		//���ò��Գ������� 
					break;
				}
				case 0x15:			
				{
					SetCommand_1F1F();		//�˳�����ģʽ
					break;
				}
				case 0x16:			
				{
					SetCommand_1F18();		//��������
					break;
				}
				case 0x18:
				{
					SetCommand_1F28();		//����ָ�
					break;
				}
				case 0x19:			
				{
					SetCommand_1F39();		//�����е�
					break;
				}
				case 0x1B:
				{
					SetCommand_1F13();		//������������
					break;
				}
				case 0x1C:
				{
					SetCommand_1F14();		//���ô�ӡ���ͺ�
					break;
				}
				case 0x1D:
				{
					SetCommand_1F2F();		//���ô�ӡ�����к�
					break;
				}
				case 0x1E:
				{
					SetCommand_1F15();		//���ô�ӡ�ٶ�
					break;
				}
				case 0x1F:
				{
					SetCommand_1F2E();		//���÷�����
					break;
				}
				case 0x20:
				{
					SetCommand_1F1A();		//���ô�ӡ
					break;
				}
				case 0x21:
				{
					SetCommand_1F19();		//��������
					break;
				}
				case 0x22:
				{
					SetCommand_1F44();		//���úڱ�ģʽ
					break;
				}
				case 0x23:
				{
					SetCommand_1F2C();		//���ô��ڲ���
					break;
				}
				case 0x28:
				{
					SetCommand_1F43();		//����ֽ������������
					break;
				}
				case 0x2A:
				{
					SetCommand_1F27();		//����ȱֽ�ش�
					break;
				}
				case 0x2B:
				{
					SetCommand_1F3B();		//����0Dָ��ʹ��
					break;
				}
				case 0x2C:
				{
					SetCommand_1F30();		//����USBIDʹ��
					break;
				}
				case 0x3C:
				{
					SetCommand_1F3E();		//���� �������� 2016.05.31
					break;
				}
				case 0x63:
				{
					SetCommand_1F17();		//���ô�ӡ����
					break;
				}
				case 0x74:
				{
					SetCommand_1F55();		//����Ĭ�ϴ���ҳ
					break;
				}            
        case 0xFA:
				{
					SetCommand_1F42();		//��������ģʽ
					break;
				}
				case 0xFD:
				{
					SetCommand_1F16();		//���ô�ӡŨ��
					break;
				}
				case 0xFE:
				{
					SetCommand_1F34();		//����λͼ
					break;
				}

				default:
					break;
			}		
		}	
		else if (GetChar == 0x1F)						   //1F����ָ��
		{
			GetChar = ReadInBuffer();
			if(g_tInterface.BT_Type)	  //Ϊ�������ͻ���
			{
				SetBluetooth(0X1F,GetChar);
			}
			else if(g_tInterface.WIFI_Type)	  //2017.10.25
			{
				SetWiFICommand(0X1F,GetChar);
			}
			switch( GetChar )
			{
				case 0x12:			
				{
					SetCommand_1F12();		//���ò��Գ������� 
					break;
				}
				case 0x13:
				{
					SetCommand_1F13();		//������������
					break;
				}
				case 0x14:
				{
					SetCommand_1F14();		//���ô�ӡ���ͺ�
					break;
				}
				case 0x15:
				{
					SetCommand_1F15();		//���ô�ӡ�ٶ�
					break;
				}
				case 0x16:
				{
					SetCommand_1F16();		//���ô�ӡŨ��
					break;
				}
				case 0x17:
				{
					SetCommand_1F17();		//���ô�ӡ����
					break;
				}
				case 0x18:			
				{
					SetCommand_1F18();		//��������
					break;
				}
				case 0x19:
				{
					SetCommand_1F19();		//��������
					break;
				}
				case 0x1A:
				{
					SetCommand_1F1A();		//���ô�ӡ
					break;
				}
				case 0x1F:			
				{
					SetCommand_1F1F();		//�˳�����ģʽ
					break;
				}
				case 0x27:
				{
					SetCommand_1F27();		//����ȱֽ�ش�
					break;
				}
				case 0x28:
				{
					SetCommand_1F28();		//����ָ�
					break;
				}
				case 0x2C:
				{
					SetCommand_1F2C();		//���ô��ڲ���
					break;
				}
				case 0x2E:
				{
					SetCommand_1F2E();		//���÷�����
					break;
				}
				case 0x2F:
				{
					SetCommand_1F2F();		//���ô�ӡ�����к�
					break;
				}
				case 0x30:
				{
					SetCommand_1F30();		//����USBIDʹ��
					break;
				}
				case 0x34:
				{
					SetCommand_1F34();		//����λͼ
					break;
				}
				case 0x39:			
				{
					SetCommand_1F39();		//�����е�
					break;
				}
				case 0x3B:
				{
					SetCommand_1F3B();		//����0Dָ��ʹ��
					break;
				}				
				case 0x3E:
				{
					SetCommand_1F3E();		//���� �������� 2016.05.31
					break;
				}
				case 0x42:
				{
					SetCommand_1F42();		//��������ģʽ
					break;
				}
				case 0x43:
				{
					SetCommand_1F43();		//����ֽ������������
					break;
				}
				case 0x44:
				{
					SetCommand_1F44();		//���úڱ�ģʽ
					break;
				}
				case 0x49:
				{
					SetCommand_1F49();		//����ȡֽ��⹦��
					break;
				}
				case 0x4A:
				{
					SetCommand_1F4A();		//����ȡֽ��⹦��
					break;
				}
				case 0x55:
				{
					SetCommand_1F55();		//����Ĭ�ϴ���ҳ
					break;
				}    
				case 0x60:
				{
					SetCommand_1F60();		//����Ĭ�ϴ���ҳ
					break;
				} 
				case 0x62:
				{
					SetCommand_1F62();		//����Ĭ�ϴ�ӡֽ���� 2017.03.29
					break;
				} 	
				case 0x63:
				{
					SetCommand_1F63();		//��������ģ������
					break;
				}
				case 0x64:
				{
					SetCommand_1F64();		//2018.06.23 ���ô�ӡ��������
					break;
				}
				case 0x74:
				{
					SetCommand_1F74();		//д��������
					break;
				}
				case 0x75:
				{
					SetCommand_1F75();		//2018.06.23 ����ȱֽ��������
					break;
				}						
				default:
					break;
			}
		}
	}		
}

/*******************************************************************************
* Function Name  : SetCommandTcp
* Description    : ѡ������ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommandTcp(void)
{
	uint16_t Newipaddr[2];
	uint8_t GetChar;
	uint8_t Temp1,Temp2,Temp3,Temp4;
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
	
	struct uip_eth_addr macaddr;
	uip_ipaddr_t ipaddr[3];
	char tmp[64];
	
	GetChar = ReadInBuffer();
	if(GetChar == 0x1f)
	{
		GetChar = ReadInBuffer();
		if(GetChar == 0x91)
		{
			GetChar = ReadInBuffer();
			if(GetChar == 0x00)
			{
				GetChar = ReadInBuffer();
				if(GetChar == 0x49)
				{
					GetChar = ReadInBuffer();
					if(GetChar == 0x50)
					{
						Temp1 = ReadInBuffer();   //�µ�ip��ַ 2017.02.07
						Temp2 = ReadInBuffer();
						Temp3 = ReadInBuffer();
						Temp4 = ReadInBuffer();								
						 //����IP����
						ReadNetworkAddr(ipaddr, &macaddr);            
						uip_ipaddr(Newipaddr, Temp1,Temp2,Temp3,Temp4);   //��ipд��uip_hostaddr
						uip_sethostaddr(Newipaddr);	
						uip_ipaddr(Newipaddr, Temp1,Temp2,Temp3,1);			  //������д��uip_draddr
						uip_setdraddr(Newipaddr);
						ipaddr[0][0] = uip_hostaddr[0];
						ipaddr[0][1] = uip_hostaddr[1];
						ipaddr[1][0] = uip_netmask[0];
						ipaddr[1][1] = uip_netmask[1];
						ipaddr[2][0] = uip_draddr[0];
						ipaddr[2][1] = uip_draddr[1];	
						//�µ�ip��ַд��flash						
						WriteNetworkAddr(ipaddr, macaddr);
						//��ӡ�µ�TCP��ַ
            PrintString(" Network Setting:\n");
					
						sprintf(tmp,"  IP Address:  %d.%d.%d.%d\n",
								(HTONS(ipaddr[0][0]) >>8) &0xff, HTONS(ipaddr[0][0]) &0xff,
								(HTONS(ipaddr[0][1]) >>8) &0xff, HTONS(ipaddr[0][1]) &0xff);
						PrintString((uint8_t*)tmp);

						sprintf(tmp,"  SubneMask:   %d.%d.%d.%d\n",
								(HTONS(ipaddr[1][0]) >>8) &0xff, HTONS(ipaddr[1][0]) &0xff,
								(HTONS(ipaddr[1][1]) >>8) &0xff, HTONS(ipaddr[1][1]) &0xff);
						PrintString((uint8_t*)tmp);

						sprintf(tmp,"  Gateway:     %d.%d.%d.%d\n\n",
								(HTONS(ipaddr[2][0]) >>8) &0xff, HTONS(ipaddr[2][0]) &0xff,
								(HTONS(ipaddr[2][1]) >>8) &0xff, HTONS(ipaddr[2][1]) &0xff);
						PrintString((uint8_t*)tmp);
						GoDotLine(CUT_OFFSETNUM);		//�ߵ���ֽλ��  2017.02.07 
            DriveCutPaper(CUT_TYPE_PART);
						DelayMs(200);						//��ʱ
            //�ر�DHCP���ܲ�д��flash������
            g_tSysConfig.DHCPEnable = 0;
						SysConfigUnionData.SysConfigStruct = g_tSysConfig;
						WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
						g_tSystemCtrl.SetMode = 0;
						DelayMs(200);						//��ʱ
						NVIC_SystemReset();			//��λ									
					}		
				}	
			}		
		}		
	}	
}	
/*******************************************************************************
* Function Name  : US_Command
* Description    : IP����ָ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void US_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x11:
		{
			SetCommandFun();	      //��������ϵͳ����
			break;
		}
		case 0x1b:								//1F1B����IP��ַ
		{
			SetCommandTcp();	      //��������ϵͳ����
			break;
		}
		default:
			break;
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
