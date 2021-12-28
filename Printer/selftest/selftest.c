/**
	******************************************************************************
	* @file	  x.c
	* @author	Application Team  
	* @version V0.0.1
  * @date		2012-5-30
	* @brief   �Լ���صĳ���.
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
#include	"selftest.h"
#include	"extgvar.h"
#include	"charmaskbuf.h"
#include	"feedpaper.h"
#include	"cutter.h"
#include	"esc.h"
#include	"debug.h"
#include	"mode.h"
#include "ETH_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//������Ϣ
const uint8_t * LanguageString[]	=
{
	"English", "GB18030", "BIG5"
};
u8  *PrtModeString[]= {"Normal Mode\n","Test PrintMode\n","5 Minutes PrintMode\n","20 Minutes PrintMode\n","50 Kilometres PrintMode\n","CutPaper TestMode\n"};

#define	LINE_SPLIT_BAR_STRING		"- - - - - - - - - - - - - - - - - - - - -\n"

extern void PrtAsciiToBuf(uint8_t Code);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void	PrintStartCommon(uint8_t cIndex);
static void	PrintPrinterSetting(void);
static void	PrintDirveSetting(void);
static void	PrintSpeedLevel(uint8_t cIndex);
static void	PrintPaperSetting(void);
static void	PrintPaperNearEndSet(uint8_t cIndex);
static void	PrintCodepageSetting(void);
static void	PrintBarCodeSetting(void);
static void	PrintStartTile(uint8_t cIndex);
static void	PrintModel(uint8_t cIndex);
static void	PrintSoftwareVersion(uint8_t cIndex);
static void	PrintInterfaceType(uint8_t cIndex);
static void PrintUsartParameterReal(uint8_t cIndex);
static void	PrintDensitySet(uint8_t cIndex);
static void	PrintPaperTypeSet(uint8_t cIndex);
static void	PrintTakePaperSensitiveSet(uint8_t cIndex);
static void	PrintPaperSensitiveSet(uint8_t cIndex);
static void	PrintDirectionSet(uint8_t cIndex);
static void	PrintCmdSystemSet(uint8_t cIndex);
static void	PrintCodepageSet(uint8_t cIndex);
static void	PrintBeepSet(uint8_t cIndex);
static void	PrintBlackmarkSet(uint8_t cIndex);
static void	CutPaperAfterPrint(void);
static void	PrintProductInfo(uint8_t cIndex);
void PrintNetParameterReal(uint8_t cIndex);
void PrtTestFun(void);
static void PrintBTParameterReal(uint8_t cIndex);
#ifdef	TEST_2MINUTE
static void	PrintTestMinute2(void);
#endif

#ifdef	TEST_5MINUTE
static void	PrintTestMinute5(void);
#endif

#ifdef	TEST_20MINUTE
static void	PrintTestMinute20(void);
#endif

#ifdef	TEST_50KM
static void	PrintTestKm50(void);
#endif

#ifdef	TEST_CUTTER_LIFTE
static void	PrintTestCutterLife(void);
#endif

/*******************************************************************************
* Function Name  : ������
* Description	  : ����
* Input	  	  	: �������
* Output  	  	 : �������
* Return  	  	 : ���ز���
*******************************************************************************/

/******************************************************************
//��������SystemSelfTest()
//����������������סFEED��ʱ�Ĵ�ӡ�Լ����
//�����������
//�����������
//����˵�����ڵ��ñ�����֮ǰ��������ڵ���GetConfigData����֮��
*******************************************************************/
void	SystemSelfTest(void)
{
	uint8_t	cTemp;
	
	g_tRealTimeStatus.PrinterStatus |= 0x08;	//�Ƚ�������״̬
	g_tSystemCtrl.SelfTestMode = 1;	//�Լ��ӡģʽ
	SetComInBufBusy();
	
	DisableReceiveInterrupt();	//�ؽ����ж�
	GoDotLine((uint32_t)g_tPrtCtrlData.CodeLineHigh * 5);
	
	//���²���Ϊ�Լ��ӡ����, �̶�����ΪӢ�Ĵ�ӡ
	cTemp = g_tPrtCtrlData.CodeAimMode;
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_MIDDLE;	//���д�ӡ

	if(SysModeType == MODE_PRINT_TEST)		           //2016.07.15 ��ӡ����ģʽ
	{
		PrintString(PrtModeString[g_tSysConfig.PrintMode]);
	}		
	
	PrintStartCommon(0);		//�ȴ�ӡ��˾��ʾ,����汾��,�����ͺ�,ͨ�Žӿڲ�������
	PrintPrinterSetting();
	PrintDirveSetting();
	PrintPaperSetting();
	PrintCodepageSetting();
	PrintBarCodeSetting();
	
	PrintString("INFO PRINT END\n");
	PrintString("\n");
	PrintString("\n");
	PrintString("\n");
	
	//�Լ��ӡ����
	#if !defined (TEST_2MINUTE) && !defined (TEST_5MINUTE) && !defined (TEST_20MINUTE) && !defined (TEST_50KM) && !defined (TEST_CUTTER_LIFE)
	if(SysModeType == MODE_PRINT_TEST && g_tSysConfig.PrintMode == 4 )		//2016.07.02  ����ģʽ�µ�50������Բ���ֽ
	{
		;
	}	
	else	
	{	
		CutPaperAfterPrint();				//��ӡ�����ֽ
	}	
	#endif
	
	g_tSystemCtrl.SelfTestMode = 0;		//�Լ��ӡģʽ
	g_tRealTimeStatus.PrinterStatus &= (uint8_t)(~0x08);	//��ӡ���,�Զ���������״̬
	
	g_tFeedButton.KeyStatus = 0;	//������Ч
	g_tFeedButton.KeyCount = 0;		//���֮ǰ���еļ���
	
	g_tPrtCtrlData.CodeAimMode = cTemp;
	
	ClearComInBufBusy();
	EnableReceiveInterrupt();		//�������ж�
}

static void	PrintStartCommon(uint8_t cIndex)
{
	PrintStartTile(cIndex);						//��ӡ�Լ쿪ͷ
	
	PrintModel(cIndex);								//�ͺ�
	PrintSoftwareVersion(cIndex);			//��ӡ�汾��
	
	PrintInterfaceType(cIndex);				//��ӡ�ӿ�����
	if (g_tInterface.COM_Type)
	{
		PrintUsartParameterReal(cIndex);	//��ӡ���ڲ���
	}
	if (g_tInterface.ETH_Type)
	{
		PrintNetParameterReal(cIndex);	//��ӡ���ڲ���
	}
	if(g_tInterface.BT_Type)					//2016.07.22��ӡ��������
	{
		PrintBTParameterReal(cIndex);
	}
	else if(g_tInterface.WIFI_Type)
	{
		Wifi_SelfTest();
		PrintString(LINE_SPLIT_BAR_STRING);
		PrintString("\n");
	}
}

static void	PrintPaperSetting(void)
{
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("PAPERSENSOR SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintPaperTypeSet(0);
	PrintPaperSensitiveSet(0);
	PrintTakePaperSensitiveSet(0);	
	PrintBlackmarkSet(0);
// 	PrintPaperNearEndSet(0);			//T801 û��ֽ���� �رմ�ѡ��
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintBarCodeSetting(void)
{
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("BARCODE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString("BarCode1D: EAN8,EAN13,UPCA,UPCE,ITF25,\n");
	PrintString("CODE39,CODE93,CODABAR,CODE128\n");
	PrintString("BarCode2D: PDF417, QR CODE, DATA Matrix\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintCodepageSetting(void)
{
	uint8_t	i;
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("CODEPAGE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintCodepageSet(0);
	
	for (i = 0x20; i < 0x80; i++)		//��ӡ�ɴ�ӡ����Ϣ
	{
		PrintAscII(i);								//��ӡASCII�ַ�
		ProcessCharMaskReadBuf();
	}
	if(g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH )	//�ַ���ʽ 2016.08.24
	{
		for(i=0x80;i<0xFF;i++)				//��ӡ��չASCII�ַ�
		{
			PrintAscII(i);
			ProcessCharMaskReadBuf();
		}
	}
	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintDirveSetting(void)
{
	const uint8_t	* StrCutter[] = 
	{
		"Cutter:  ",
		"Cutter:  ",
		"Cutter:  ",
	};
	
	const uint8_t * CutterStatus[]	=
	{
		"ON", "OFF"
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("DRIVE SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString(StrCutter[0]);
	PrintString(CutterStatus[0]);
	PrintString("\n");
	
	PrintBeepSet(0);
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

static void	PrintPrinterSetting(void)
{
	const uint8_t	* StrLanguage[] = 
	{
		"Language:  ",
		"Language:  ",
		"Language:  ",
	};
	const uint8_t	* StrFontSizeASCII[] = 
	{
		"FontDot:  ",
		"FontDot:  ",
		"FontDot:  ",
	};
	
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("PRINTER SETTING\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintCmdSystemSet(0);
	
	PrintString(StrLanguage[0]);
	if (g_tSysConfig.SysLanguage == LANGUAGE_ENGLISH)
	{
		PrintString("English\n");
	}
	else if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
	{
		PrintString("��������\n");
	}
	else
	{
		PrintString("BIG5\n");
	}
	
	PrintString(StrFontSizeASCII[0]);
	if (g_tSysConfig.SysFontSize == 0)
	{
		PrintString("12*24\n");
	}
	else
	{
		PrintString("9*17\n");
	}
	
	PrintDirectionSet(0);
	PrintSpeedLevel(0);
	PrintDensitySet(0);
	PrintProductInfo(0);				//2016.05.31 ��ӡ��Ʒ��Ϣ
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

//�Լ��ӡ�ٶȵȼ�
static void	PrintSpeedLevel(uint8_t cIndex)
{
	const uint8_t	* StrSpeedString[] = 
	{
		"Speed:  ",
		"Speed:  ",
		"Speed:  ",
	};
	//��ӡŨ����Ϣ
	const uint8_t * SpeedLel[] =
	{
		"250(mm/s)", "250(mm/s)", "250(mm/s)", "250(mm/s)"		//2016.07.30  ������һ��
	};
	
	PrintString(StrSpeedString[cIndex]);
	PrintString(SpeedLel[g_tSysConfig.MaxSpeed]);
	PrintString("\n");
}


static void	PrintDensitySet(uint8_t cIndex)
{
	const uint8_t	* StrPrintDensity[] = 
	{
		"Density:  ",
		"Density:  ",
		"Density:  ",
	};
	//��ӡŨ����Ϣ
	const uint8_t * DensityString[] =
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPrintDensity[cIndex]);
	PrintString(DensityString[g_tSysConfig.Density]);
	PrintString("\n");
}

static void	PrintProductInfo(uint8_t cIndex)
{
	const uint8_t	* StrPrintProductInfo[] = 
	{
		"Product Info:  ",
		"Product Info:  ",
		"Product Info:  ",
	};

	PrintString(StrPrintProductInfo[cIndex]);
	PrintString(g_tSysConfig.ProductiveData);			//2016.05.31 ��ӡ��Ʒ��Ϣ
	PrintString("\n");
}


static void	PrintPaperTypeSet(uint8_t cIndex)
{
	const uint8_t	* StrPaperType[] = 
	{
		"PaperType:  ",
		"PaperType:  ",
		"PaperType:  ",
	};
	
	const uint8_t * StrPaperTypeSet[]	=
	{
		"Continuous paper", "BlackMark Paper", "Label Paper"  //2016.11.17
	};
	
	PrintString(StrPaperType[cIndex]);
	PrintString(StrPaperTypeSet[g_tSystemCtrl.PaperType]);
	PrintString("\n");
}

static void	PrintTakePaperSensitiveSet(uint8_t cIndex)
{
	const uint8_t	* StrTakePaperSensitive[] = 
	{
		"TakePaperSensor:  ",
		"TakePaperSensor:  ",
		"TakePaperSensor:  ",
	};
	//��������������Ϣ
	const uint8_t * SensorString[]	=
	{
		"Low", "Medium", "High"
	};
	const uint8_t	* StrTakePaperEndEnable[] = 
	{
		"TakePaperMode:  ",
		"ȡֽ����:  ",
		"TakePaperMode:  ",
	};
	const uint8_t * TakePaperEnableStr[]	=
	{
		"TakePaperOFF", "TakePaperON", "BillPintOverON"														//2016.07.30
	};
	PrintString(StrTakePaperEndEnable[cIndex]);
	PrintString(TakePaperEnableStr[g_tSysConfig.PaperTakeAwaySensorEnable]);    //ȡֽ���ܿ���
	PrintString("\n");
	PrintString(StrTakePaperSensitive[cIndex]);
	PrintString(SensorString[g_tSysConfig.PaperTakeAwaySensorSensitive]);
	PrintString("\n");
}

static void	PrintPaperSensitiveSet(uint8_t cIndex)
{
	const uint8_t	* StrPaperSensitive[] = 
	{
		"PaperSensor:  ",
		"PaperSensor:  ",
		"PaperSensor:  ",
	};
	//��������������Ϣ
	const uint8_t * SensorString[]	=
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPaperSensitive[cIndex]);
	PrintString(SensorString[g_tSysConfig.PaperSensorSensitive]);
	PrintString("\n");
}

static void	PrintPaperNearEndSet(uint8_t cIndex)
{
	//������ʹ����Ϣ
	const uint8_t	* StrPaperNearEndEnable[] = 
	{
		"PaperNearlyEnable:  ",
		"PaperNearlyEnable:  ",
		"PaperNearlyEnable:  ",
	};
	const uint8_t * EnableStr[]	=
	{
		"OFF", "ON"
	};
	
	//��������������Ϣ
	const uint8_t	* StrPaperNearEndSensitive[] = 
	{
		"PaperNearlySensor:  ",
		"PaperNearlySensor:  ",
		"PaperNearlySensor:  ",
	};
	const uint8_t * SensitiveStr[]	=
	{
		"Low", "Medium", "High"
	};
	
	PrintString(StrPaperNearEndEnable[cIndex]);
	PrintString(EnableStr[g_tSysConfig.PaperNearEndSensorEnable]);
	PrintString("\n");
	PrintString(StrPaperNearEndSensitive[cIndex]);
	PrintString(SensitiveStr[g_tSysConfig.PaperNearEndSensorSensitive]);
	PrintString("\n");
}

static void	PrintDirectionSet(uint8_t cIndex)
{
	//��ӡ����
	const uint8_t	* StrDirection[] = 
	{
		"PrintDirection:  ",
		"PrintDirection:  ",
		"PrintDirection:  ",
	};
	const uint8_t * PrintDirection[]	=
	{
		"Normal", "Reverse"
	};
	
	PrintString(StrDirection[cIndex]);
	PrintString(PrintDirection[g_tSysConfig.PrintDir]);
	PrintString("\n");
}

static void	PrintCmdSystemSet(uint8_t cIndex)
{
	const uint8_t	* StrCmd[] = 
	{
		"CommandType:  ",
		"CommandType:  ",
		"CommandType:  ",
	};
	//ָ�
	const uint8_t * CommandSystemString[] =
	{
		"ESC&POS", "SPMP", "WHSP", "Other"
	};
	
	PrintString(StrCmd[cIndex]);
	PrintString(CommandSystemString[g_tSysConfig.CommandSystem]);
	PrintString("\n");
}

static void	PrintCodepageSet(uint8_t cIndex)
{
	//Ĭ�ϴ���ҳ,��56��
	const uint8_t	* StrCodepage[] = 
	{
		"Codepage:  ",
		"Codepage:  ",
		"Codepage:  ",
	};
	const uint8_t * CodePageString[]	=
	{
		"PC437", "KataKana", "PC850", "PC860", "PC863",
		"PC865", "WPC1251", "PC866", "MIK", "PC755", 
		"Iran", "Reserve", "Reserve", "Reserve", "Reserve", 
		"PC862", "WPC1252", "WPC1253", "PC852", "PC858", 
		"Iran II", "Latvian", "PC864", "ISO-8859-1", "PC737", 
		"WPC1257", "Thai", "PC720", "PC855", "PC857", 
		"WPC1250", "PC775", "WPC1254", "WPC1255", "WPC1256", 
		"WPC1258", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4", "ISO-8859-5",
		"ISO-8859-6", "ISO-8859-7", "ISO-8859-8", "ISO-8859-9", "ISO-8859-15", 
		"Thai2", "PC856", "", "", "", 
		"", "", "", "", "", "",
	};
	
	PrintString(StrCodepage[cIndex]);
	PrintString(CodePageString[g_tSysConfig.DefaultCodePage]);
	PrintString("\n");
}

static void	PrintBeepSet(uint8_t cIndex)
{
	const uint8_t	* StrBeep[] = 
	{
		"Buzzer:  ",
		"Buzzer:  ",
		"Buzzer:  ",
	};
	const uint8_t * BuzzerString[]	=
	{
		"OFF", "Error ON", "Cut ON", "All ON"
	};
	
	PrintString(StrBeep[cIndex]);
	PrintString(BuzzerString[g_tSysConfig.BuzzerEnable]);
	PrintString("\n");
}

static void	PrintBlackmarkSet(uint8_t cIndex)
{
	//�ڱ�ģʽ��Ϣ
	const uint8_t	* StrBlackmarkPosition[] = 
	{
		"BlackMarkPosition:  ",
		"�ڱ괫����λ��:  ",
		"BlackMarkPosition:  ",
	};
	const uint8_t * MarkPositionModeString[] =
	{
		"RightSide", "LeftSide"
	};
	const uint8_t	* StrBlackmarkSensitive[] = 
	{
		"BlackMarkSensor:  ",
		"�ڱ괫����������:  ",
		"BlackMarkSensor:  ",
	};
	const uint8_t * MarkSensitiveModeString[] =
	{
		"Low", "Medium", "High"
	};
	const uint8_t	* StrMarkEndEnable[] = 
	{
		"BlackMarkEnable:  ",
		"�ڱ깦��:  ",
		"BlackMarkEnable:  ",
	};
	const uint8_t * MarkEnableStr[]	=
	{
		"OFF", "ON"
	};
	PrintString(StrMarkEndEnable[cIndex]);
	PrintString(MarkEnableStr[g_tSysConfig.BlackMarkEnable]);
	PrintString("\n");
	PrintString(StrBlackmarkPosition[cIndex]);
	PrintString(MarkPositionModeString[g_tSysConfig.BlackMarkPosition]);
	PrintString("\n");
	PrintString(StrBlackmarkSensitive[cIndex]);
	PrintString(MarkSensitiveModeString[g_tSysConfig.BlackMarkSensitive]);
	PrintString("\n");
}

//1. ��ӡ�Լ쿪ͷ
static void	PrintStartTile(uint8_t cIndex)
{
	const uint8_t	* StrSelfTestTitle[] = 
	{
		"SYSTEM INFORMATION\n",
		"SYSTEM INFORMATION\n",
		"SYSTEM INFORMATION\n",
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString(StrSelfTestTitle[cIndex]);
	PrintString(LINE_SPLIT_BAR_STRING);
}

//3. ��ӡ�ͺ�
static void	PrintModel(uint8_t cIndex)
{
	const uint8_t	* StrModel[] = 
	{
		"Model:  ",
		"Model:  ",
		"Model:  ",
	};
	
	PrintString(StrModel[cIndex]);
	PrintString(g_tSysConfig.Model);
	PrintString("\n");
}

//4. ��ӡ�汾��
static void	PrintSoftwareVersion(uint8_t cIndex)
{
	const uint8_t	* StrSoftwareVersion[] = 
	{
		"FirmWare:  ",
		"FirmWare:  ",
		"FirmWare:  ",
	};
	
	PrintString(StrSoftwareVersion[cIndex]);
	PrintString(FIRMWARE_STRING);
	PrintString("\n");
	PrintString("BFFW:  ");
  PrintString(BootVerStr);
  PrintString("-");                 //����ӡ�ֿ�汾
  PrintString(ZIKUVerStr);
  PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}

//5. ��ӡ�ӿ�����
static void	PrintInterfaceType(uint8_t cIndex)
{
	const uint8_t	* StrInterface[] = 
	{
		"INTERFACE:	 ",
		"INTERFACE:	 ",
		"INTERFACE:	 ",
	};
	const uint8_t	* StrParallel[] = 
	{
		"PARALLEL  ",
		"PARALLEL  ",
		"PARALLEL  ",
	};
	const uint8_t	* StrSerial[] = 
	{
		"RS232  ",
		"RS232  ",
		"RS232  ",
	};
	const uint8_t	* StrNone[] = 
	{
		"None",
		"None",
		"None",
	};
	
	PrintString(LINE_SPLIT_BAR_STRING);
	
	PrintString(StrInterface[cIndex]);
	
	if (g_tInterface.LPT_Type)
	{
		PrintString(StrParallel[cIndex]); //��ӡ����
	}
	if (g_tInterface.COM_Type)
	{
		PrintString(StrSerial[cIndex]); 	//��ӡEIA��ƽRS232����
	}
	if (g_tInterface.USB_Type)
	{
		PrintString("USB  "); 					//��ӡUSB��CPU�ڴ�USB��
	}
	if (g_tInterface.ETH_Type)
	{
		PrintString("NET  "); 					//��ӡNET
	}
	if (g_tInterface.BT_Type)
	{
		PrintString("BT  "); 					//��ӡBT
	}
	if(g_tInterface.WIFI_Type)
	{
		PrintString("WIFI  "); 
	}
	if (!(g_tInterface.LPT_Type || g_tInterface.COM_Type || g_tInterface.USB_Type || g_tInterface.ETH_Type || g_tInterface.BT_Type))
	{
		PrintString(StrNone[cIndex]);		//��ӡû�а�װ�˿�
	}
	
	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
}

static void PrintUsartParameterReal(uint8_t cIndex)
{
	uint8_t	BitNumber;
	uint8_t	Parity;
	
	const uint8_t	* StrSerialPara[] = 
	{
		"USART Setting\n",
		"USART Setting\n",
		"USART Setting\n",
	};
	const uint8_t	* StrBuadRate[] = 
	{
		"Baud Rate:  ",
		"Baud Rate:  ",
		"Baud Rate:  ",
	};
	const uint8_t	* StrBitNumber[] = 
	{
		"Data bit:  ",
		"Data bit:  ",
		"Data bit:  ",
	};
	const uint8_t	* StrParity[] = 
	{
		" Parity:  ",
		" Parity:  ",
		" Parity:  ",
	};
	const uint8_t	* StrHandshaking[] = 
	{
		" Handshake:  ",
		" Handshake:  ",
		" Handshake:  ",
	};
	const uint8_t * USARTBaudRateString[] =
	{
		"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"
	};
	const uint8_t * USARTDataBitString[] =
	{
		"8 bit", "7 bit"
	};
	const uint8_t * USARTParityString[] = 
	{
		"None", "Odd", "None", "Even"
	};

	const uint8_t * USARTHandshakingString[] =
	{
		"RTS/CTS", "XON/XOFF"
	};
	
	BitNumber = (g_tSysConfig.ComProtocol & 0x08) >> 3;	//����λ��;=0,8λ;=1,7λ,
	Parity		= (g_tSysConfig.ComProtocol & 0x03);				//У��λ����;00,10��У��;11żУ��;01��У��
	if ((BitNumber == 0x01) && ((Parity & 0x01) == 0x00))	//7λ����λ+��У��
	{
		BitNumber--;	//��ʾ8λ����λ
	}
	
	PrintString(StrSerialPara[cIndex]);
	PrintString(StrBuadRate[cIndex]);			//������
	PrintString(USARTBaudRateString[g_tSysConfig.ComBaudRate & 0x07]);
	PrintString("\n");
	
	PrintString(StrBitNumber[cIndex]);		//��ӡ����λ��
	PrintString(USARTDataBitString[BitNumber]);
	PrintString("\n");
	
	PrintString(StrParity[cIndex]);				//��ӡУ��
	PrintString(USARTParityString[Parity]);
	PrintString("\n");
	
	PrintString(StrHandshaking[cIndex]);	//��ӡ���ַ�ʽ
	PrintString(USARTHandshakingString[(g_tSysConfig.ComProtocol & 0x04) >> 2]);
	PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
}

void PrintNetParameterReal(uint8_t cIndex)
{
	uint32_t ultmp, ultmp1;
	char str[20];
	const uint8_t * DHCPEnableStr[]	=
	{
		"OFF  ", "ON  "
	};
	
	PrintString("NET Setting\n");
		//2017.02.07 �Լ��д�ӡ�Զ���ȡDHCP����
	PrintString("DHCP:  ");		//MAC��ַ	
	PrintString(DHCPEnableStr[g_tSysConfig.DHCPEnable]);
	PrintString("\n");
	
	ultmp =ReadFlash(IP_FLASH_ADDR);
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("IP Address:");			  //IP��ַ
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(MASK_FLASH_ADDR);	
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("Sub Address:  ");		//MASK��ַ
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(GATE_FLASH_ADDR);
	sprintf(str,"%d.%d.%d.%d",
	(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
	PrintString("Gate Address:  ");		//GATE��ַ
	PrintString(str);
	PrintString("\n");
	
	ultmp =ReadFlash(EMAC_FLASH_ADDR);
	ultmp1 =ReadFlash((EMAC_FLASH_ADDR +4));
	sprintf(str,"%02x-%02x-%02x-%02x-%02x-%02x",
	ultmp &0xff, (ultmp>>8)&0xff, (ultmp>>16)&0xff, (ultmp>>24)&0xff,
	ultmp1 &0xff, (ultmp1>>8)&0xff);
	PrintString("MAC Address:  ");		//MAC��ַ
	PrintString(str);	
	PrintString("\n");
	PrintString(LINE_SPLIT_BAR_STRING);
// 	PrintString("\n");
}
/*******************************************************************************
* Function Name  : PrintBTParameterReal
* Description    : ��ӡ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void PrintBTParameterReal(uint8_t cIndex)
{
	const uint8_t	* BTVerStr[] = 
	{
		"BT_Version:  ",
		"�����汾:  ",
		"�����汾:  ",
	};
	const uint8_t	* BTNameStr[] = 
	{
		"BT_Name:  ",
		"��������:  ",
		"��������:  ",
	};
	const uint8_t	* BTAddrStr[] = 
	{
		"BT_Addr:  ",
		"������ַ:  ",
		"������ַ:  ",
	};
	const uint8_t	* BTAuthStr[] = 
	{
		"BT_Auth:  ",
		"��������ʹ��:  ",
		"��������ʹ��:  ",
	};
	const uint8_t	* BTClassStr[] =   //2018.02.10
	{
		"BT_Class:  ",
		"�����豸����:  ",
		"�����豸����:  ",
	};
	const uint8_t	* BTPasswordStr[] = 
	{
		"BT_Password:  ",
		"��������:  ",
		"��������:  ",
	};
	const uint8_t	* EnableStr[] = 
	{
		"No",
		"��",
		"��",
		"Yes",
		"��"
		"��"
	};
	PrintString("BlueTooth Setting\n");
	/****��ӡ�����汾��****************/
	PrintString(BTVerStr[cIndex] );
	PrintString(BluetoothStruct.Version);

	/****��ӡ����ģ������*****************/
	PrintString( BTNameStr[cIndex] );
	PrintString(BluetoothStruct.Name);
	//PrintString("\n");

	/****��ӡ������ַ*****************/
	PrintString( BTAddrStr[cIndex] );
	PrintString(BluetoothStruct.Laddr);
	PrintString("\n");

	/****��ӡ���������Ƿ��*****************/
	PrintString( BTAuthStr[cIndex] );
	PrintString(EnableStr[3*BluetoothStruct.Auth+cIndex]); 
  PrintString("\n");
	
	if(BluetoothStruct.Auth == 1)
	{
		/****��ӡ�����������*****************/
		PrintString( BTPasswordStr[cIndex] );
		PrintString(BluetoothStruct.Password);
// 	PrintString( "\n" );
	}  
		/****��ӡ�����豸����*****************/  //2018.02.10
	PrintString( BTClassStr[cIndex] );
	PrintString(BluetoothStruct.Class);
// 	PrintString("\n");
	
	PrintString(LINE_SPLIT_BAR_STRING);
	PrintString("\n");
}
//��ӡ�����ֽ
static void	CutPaperAfterPrint(void)
{
	GoDotLine(CUT_OFFSET);						//�ߵ���ֽλ��
	DriveCutPaper(CUT_TYPE_DEFAULT);	//����ֽ
}

/*******************************************************************************
* Function Name  : WriterCPUFlash(uint32_t Addr, uint32_t *Buf, uint32_t Len,uint8_t Flag)
* Description    : дCPU�ڲ�Flash
* Input          : Flag��0������ʱд��1�ػ�����ѹ��ʱд��2ָ��д
* Output         : None
* Return         : None
*********************************************************************************/
void WriterCPUFlash(uint32_t Addr, uint32_t *Buf, uint32_t Len,uint8_t Flag)
{
	FLASH_Status status;
	uint32_t i,j;
	
	FLASH_Unlock();
	
	for(j=0; j<3; j++)
	{
		for(i=0; i<Len; i++)
		{
			status = FLASH_ProgramWord(Addr, Buf[i]);
			if(status != FLASH_COMPLETE || *((uint32_t*)Addr) != Buf[i])
			{	
				break;
			}
			Addr +=4;
		}
		if(Flag ==1 || i ==Len )			//����д��ʱ��У���Ƿ���ȷ�����ϵ�ʱУ��
			break;
		else
		{
			Addr -= Addr%PAGE_SIZE;
			FLASH_ClearFlag(0x00000035);	//������д����־
			FLASH_ErasePage(Addr);
		}
	}
	FLASH_Lock();
		
	//������󱨾�
	if(j==3)
		g_tError.UR_ErrorFlag |= 0x20;		//bit5 CPU��Flashд�����
	else
		g_tError.UR_ErrorFlag &= ~0x20;
}

/*******************************************************************************
* Function Name  : PrtTestFun
* Description    : ��ӡ�������� 2017.03.20
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtTestFun(void)
{    
	uint8_t	i;
	
	Command_1C26();
	
	if(SysModeType == MODE_PRINT_TEST)		           //2016.07.15 ��ӡ����ģʽ
	{
		PrintString(PrtModeString[g_tSysConfig.PrintMode]);
	}	
	for(i=0;i<1;i++)
	{
		PrintString("012345678901234567890123456789012345678901234567\n");
		PrintString("abcdefghijklmnopqrstuvwxyz\n");
	}
	for(i=0;i<1;i++)
	{
		PrintString("������������������������������������������������\n");
	}
	
	Command_1C2E();        //����Ӣ��ģʽ
	
	for(i=0;i<48;i++)
	{
		PrtAsciiToBuf(0xC4);
	}
	PrintString("\n");
	for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0xC4);
		PrtAsciiToBuf(0x20);
	}
	PrintString("\n");

  for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0x20);
		PrtAsciiToBuf(0xDB);
	}
	PrintString("\n");
	for(i=0;i<24;i++)
	{
		PrtAsciiToBuf(0xDB);
		PrtAsciiToBuf(0x20);
	}
	PrintString("\n");
		
	GoDotLine(CUT_OFFSET);						//�ߵ���ֽλ��
	DriveCutPaper(CUT_TYPE_DEFAULT);	//����ֽ
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
