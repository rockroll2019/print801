/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : .h
* @author  Application Team  
* @version V0.1
* @date    2016-3-18
* @brief   ȫ�ֱ�������
********************************************************************************
* 
*******************************************************************************/
/*****	ȫ��Ƭ�ڣ�48K�ֽڣ��ڴ���䣺
		1.�д�ӡ����������󱶸ߡ��и�/3���п�,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
		  Ƭ�ڣ����֧�� 8���� x24��ߡ�576�㡣��ַ�¶���
		  ���     		13824B 8����24/8����576
		2.�»��߻�������	576B   �п�
		3.�û��Զ����ַ���	6175B   95����2��36��27�����ֱ��¼95�� 12 x 24��9��17 ������û��Զ����ַ�
		4.����λͼ���棺	12KB
		5.�û�����꣺   	2KB(2048�ֽ�)
		6.��ֱ�Ʊ�		256B
		7.ˮƽ�Ʊ�	 	144B ��576/8����2
		8.�û������դ��	2K(2048�ֽ�)
		9.ָ����ջ��棺	8K��16384�ֽڣ���g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
		10.ҳģʽ��ӡ��������128K��Ƭ���ַ
*********/

#ifndef GVAR_H
#define GVAR_H

#include "defconstant.h"
#include "bluetooth.h"
/**********************************************************/
//	��ӡ��ϵͳ�������ò��֣�
TypeDef_StructSysConfig	g_ctSysDefaultValue = 
{
	0x87,		//uint8_t	ComBaudRate;		//���ڲ�����,0x83ʱΪBPS9600,0x87ʱΪ115200
	0x00,		//uint8_t	ComProtocol;		//���ڵ�����Э��,
	LANGUAGE_DEFAULT,		//uint8_t	SYS_Language;		//ϵͳ���ԣ�ʹ��������0=English;1=GB;2=BIG5��,Ĭ��Ϊ��������
 	ASCII_FONT_DEFAULT,	//uint8_t	SYS_FontSize;		//ϵͳ����������С��ʹ��������0=12x24;1=9x17��
	
	#if defined	CUSTOM_LOTTERY_PAPER_ENABLE
		0x01,		//uint8_t 	PrintWidthSelect;  		//���������,58mm�̶�
	#else
		0x00,		//uint8_t 	PrintWidthSelect;  		//���������,Ϊ�˼���EPSON TM-88�Ŀ��
	#endif
	
	0x01,		//uint8_t	Density;				//��ӡŨ������(0=��,1=��,2=��)
	0x01,		//uint8_t	MaxSpeed;				//����ٶ�,0=150mm/s,1=190mm/s,2=220mm/s,3=250mm/s
	0x01,		//uint8_t	CutType;				//�е���־
 	0x01,		//uint8_t	BuzzerEnable;			//������ʹ�� 2017.03.21
	0x00,		//uint8_t	PaperOutReprint;		//ȱֽ���´�ӡ
	
	#if defined	CUSTOM_LOTTERY_PAPER_ENABLE
  0x01,		//uint8_t	BlackMarkEnable;	//�ڱ�����(0=��,1=��)
	#else
	0x00,		//uint8_t	BlackMarkEnable;	//�ڱ�����(0=��,1=��)  0��ֽͨ 1�ڱ�ֽ 2��ǩֽ 0��Ϊ2 2016.03
	#endif
	
	0x00,		//uint8_t	RightMark;				    //�ڱ괫����λ�ã�=0�ұ�,=1��ߣ�Ĭ���ұ�
 	0x01,		//uint8_t	PaperSensor;			    //��ֽ����������������(0=��,1=��,2=��)
  0x01,		//uint8_t	BlackMarkSensor;	    //�ڱ괫��������������(0=��,1=��,2=��)
 	0x00,		//uint8_t	SysShutDownMode;	    //ϵͳ�ػ�ģʽ��ʹ������,0=�Ӳ�;1=10����;2=20����;3=30���ӣ�
	0x00,		//uint8_t	PrintDir;				      //��ӡ����(0=����,1=����)
	0x00,		//uint8_t	CommandSystem;		    //�����0��EPSON ESC/POS�����1��WH �����2��IP���
	0x00,		//uint8_t DefaultCodePage; 	    //Ĭ�ϴ���ҳ
	0x00,		//uint8_t	CREnable;				      //0D�س�ָ��ʹ��
	
	COMPANY_NAME_STRING,	                  //uint8_t	Manufacturer[12];	//����
	MODAL_NAME_STRING,		                  //uint8_t	Model[12];			//�ͺ�
	"11101800002",                          //uint8_t	SerialNumber[21];	//���к�	
			
	0x00,		//uint8_t	USBIDEnable;		      //ʹ��USBID
  0x00,		//uint8_t	USB_Class;			      //USB���ͣ�0����ӡ�࣬1��VCP��
	
	0x00,   //uint8_t DHCPEnable;           //����DHCPЭ�� �Զ���ȡIP����ʹ���ֽ�
	0x00,		//uint8_t	PaperWith;		        //2017.03.29 0Ϊ80ֽ 1Ϊ58ֽ
	
	0x00,		//uint8_t	PaperNearEndSensorEnable;			  //ֽ����ʹ�ܿ���
	0x01,		//uint8_t	PaperNearEndSensorSensitive;		//ֽ����������������
	0x02,		//uint8_t	PaperTakeAwaySensorEnable;			//ȡֽʹ�ܿ��� Ĭ��Ϊ2 0�ر� 1����δȡ������ 2��������
  0x01,		//uint8_t	PaperTakeAwaySensorSensitive;		//ȡֽ������������
	
	"531",  //uint8_t ProductiveData[4];							//��Ʒ�������� 2016.05.31  
	0x01,		//uint8_t PrintMode;											//��ӡ����ģʽ 2016.05.31  00Ϊ��ͨģʽ 01Ϊ2����һ���Լ��ӡ 02Ϊ5����һ���Լ��ӡ 03Ϊ20����һ���Լ��ӡ 04Ϊ50KM�ϻ����� 05 �е�����
  0x02,   //SpeakerVolume;						//����ģ�����������  0��С����   1 ������      2������
	0x0000,   //AlarmNumber;							//��ֽ������ѡ��
	0x0001,   //POutVoiceNumber           //ȱֽ����ѡ��
	PARA_CHECK_STRING,	//uint8_t	ConfigFlag[7];	//������Ч��־ 
	
};

/********** �궨��ȫ�ֱ��� ***********/
TypeDef_StructMacro			g_tMacro;

/********** �����û�����λͼ���Զ����ַ���������ά�������ȫ�ֱ��� ***********/
TypeDef_UnionRAMBuffer	g_tRamBuffer;

/********** һά�������ȫ�ֱ��� ***********/
TypeDef_StructBarCtrl		g_tBarCodeCtrl;

/********** �ַ��Ű����ȫ�ֱ��� ***********/
TypeDef_StructPrintMode	g_tPrtModeChar;
TypeDef_StructPrintMode	g_tPrtModeHZ;

/********** ��ӡ��״̬����ȫ�ֱ��� ***********/
TypeDef_StructStatus		g_tRealTimeStatus;

/********** ��ӡ���������ȫ�ֱ��� ***********/
TypeDef_StructError				g_tError;
TypeDef_StructSysConfig		g_tSysConfig; 		//��Flash�ж����ϵͳ����
TypeDef_StructPageMode 		g_tPageMode;			//ҳģʽ
TypeDef_StructKeyCtrl			g_tFeedButton;		//��������
TypeDef_StructSystemCtrl	g_tSystemCtrl;		//ϵͳ����
TypeDef_StructPrtCtrl			g_tPrtCtrlData;		//��ӡ����
//TypeDef_StructBarCode2D		g_tBarCode2D;			//��ά����ṹ
TypeDef_StructDrawer  		g_tDrawer1,g_tDrawer2;	//Ǯ��ṹ
TypeDef_StructRealTime		g_tRealTimeCmd;		//ʵʱָ��ṹ
TypeDef_StructTab					g_tTab;						//����ṹ
TypeDef_StructInterfaceType	g_tInterface;		//�ӿ����ͽṹ

TypeDef_StructInBuffer volatile g_tInBuf;   //2016.07.08

uint8_t	g_tLineEditBuf[ PRT_BUF_LENGTH ];		//��ӡ������
uint8_t	g_tUnderlineBuf[ PRT_DOT_NUMBERS ];	//�»��߻�����
uint8_t	g_bSleepModeFlag;					   				//�������߱�־
volatile uint8_t	g_bRoutinFlag;						//T2�жϱ�־

PrintBufCtrl_Struct U_PrtBufCtrl;						//2016.06.15
Union_PrtBuf U_PrtBuf;                      //2016.06.16 ��ӡ������������

//g_StepCount��ֽ������ٱ���λ�ñ�־,��ʼ��ʱ����,
//�ڼ����и��ݼ�����ʱ����� AdjustMotorStep()������ֻ���������߲��䣬
//�� SystemTimer3Func()���ߵ��ʱ����,���ֹͣʱ���㡣
//ÿ�� ��ӡ�л��� GoDotLine()ʵ�������ļ��ٹ��̣�
//���� ��ӡ���м���λ�ÿ�����ʱ���ݼ�����ʱ���������
volatile uint16_t	g_StepCount;
volatile uint16_t	g_HeatUs;							//����һ�ε�ʱ��

uint8_t	g_bMotorRunFlag = 0;		//���б�־
uint8_t	g_bWriteFlashFlag = 0;	//SPIFLASH�Ƿ��Ѿ�д���ı�־
uint8_t	g_bRAMBitMapNumber = 1;	//RAM����λͼ��ͼ��
uint8_t	g_bFeedTaskFlag = 0;		//��ֽ������ɱ�־

#ifdef	DBG_025
	uint8_t	g_bDbg025EnalbeFlag = 0;
	uint8_t	g_bDbg025FontFlag = 0;
#endif

#ifdef	MAINTAIN_ENABLE
	/******* ����ά��������*************/
	//g_tResetableCounter:�����õĿ�����ļ���ֵ,
	//g_tCumulateCounter:�ۻ��ļ���ֵ,
// 	TypeDef_StructMaintenanceCounter g_tResetableCounter;
// 	TypeDef_StructMaintenanceCounter g_tCumulateCounter;
// 	TypeDef_StructMaintenanceCounter g_tResetableCounterBakeup;
  Maintenance_Counter R_Counter, C_Counter, R_CounterBK;
	volatile uint8_t	g_bMaintainRountinWriteFlag = 0;
#endif

#ifdef	CPCL_CMD_ENABLE
	TypeDef_StructHZCharPara	g_tPageModeCharPara;
	TypeDef_StructHZCharPara	g_tPageModeHZPara;
	TypeDef_StructPageEditPrtBuf	g_tPageEditPrtBuf;	//ҳ�༭��ӡ������
#endif

uc16	MaxStepNum[]={SPEED_LOW, SPEED_MIDDLE, SPEED_HIGH, SPEED_MAX};		//��ӡ�ٶ��趨ֵ 2016.05.31
//�ⲿEXTSRAM��������С
uint32_t ExtSRamSize;

//��ǩ��϶��Ч��־
uint8_t LableEffectiveFlag;
//��ֽ��־
uint8_t FeedPaperFlag = 0;
//��ֽ�ұ�־
uint8_t FeedToGapFlag = 0;
//�Զ���ֽ��־
uint8_t AutoFeedPaper = 0;
//boot����汾
uint8_t	*BootVerStr = "NoBoot";	
//�ֿ�汾
uint8_t  ZIKUVerStr[5]; 
//�����ٲ���
uint8_t  MAX_RUSH_STEPS = 94;		//Ĭ���ٶ�190
//дflash��־
uint8_t  WriteFlashFlag = 0;
//16ָ�����б�־	2016.06.08
volatile uint8_t  Command16RunFlag = 0;
volatile uint32_t	FeedStepCount = 0;   //2016.06.11  T3����ֽ������¼
//���ٱ�־	2016.06.16
volatile uint8_t  MotoFeedInc = 1;
//2016.07.02  ��ӡ��ģʽ
uint8_t	SysModeType = 0;
//2016.07.10
volatile uint8_t T3_over =0;
volatile uint8_t SPI3_Over=0;
volatile uint8_t HeatOver;
//2016.07.15
uint8_t ManufacturerType;
//2016.08.20 ��ά����ر���
uint8_t QR_version =0,PDF417_version=0;			     //�汾
uint8_t QR_vColl =48 ,PDF417_vColl=0;						 //����ȼ�
uint8_t PDF417_Rows =0,PDF417_ModuleWidth =3,PDF417_RowHeight=3,PDF417_options=0,PDF417_Rate=3;
uint8_t QR_model=2,QR_Rate=3;
uint16_t TwoCodeDataNum=0;

//2016.09.14 ���������Ʋ���
uint8_t		BuzzerRunFlag = 0;
uint8_t		BuzzerNum = 0;
uint16_t  BuzzerTime = 0;

//2017.02.07 DHCP��ȡ��־
uint8_t		DhcpGetFlag = 0;
//2017.08.14 �ȴ��������
uint8_t		VoiceOverflag = 0;
//2017.08.14 �е������־ �е�����ֽ
uint8_t		CutGoPaperflag = 0;
TypeDef_StructSpeaker  g_tSpeaker={0};
//2016.07.22 ���������ṹ��
Bluetooth_Struct    BluetoothStruct;		
uint8_t BtOnStatus = 0x55;     //2016.10.13 ��������δAA  ������δ55
USART_InitTypeDef   USART_BT_InitStructure;
uint8_t		BtTimeOut = 0;     //������ʱ��׼
volatile  uint8_t		BtParaReloadFlag = 0;
uint8_t Set_BTWFflg = 0;
//2018.03.05 ���ڸ�λ������
uint32_t  NetReloadCounter =0;
#endif /* GVAR_H */

/******************* (C) COPYRIGHT 20010 Sprinter *****END OF FILE****/
