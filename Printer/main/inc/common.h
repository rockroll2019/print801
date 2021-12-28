/**
******************************************************************************
  * @file    x.h
  * @author  WH Application Team  
  * @version V0.0.1
  * @date    2012-5-15
  * @brief   �������ͼ��궨��ĳ���
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
  * <h2><center>&copy; COPYRIGHT 2012 WH</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"

/* Macro Definition ----------------------------------------------------------*/
#if	0
/******* ����ά�����������ͽṹ *************/
typedef struct
{
	volatile uint32_t	LineCounter;		//��ֽ�м�����
	volatile uint32_t	HeatCounter;		//���ȴ���������
	volatile uint32_t	CutCounter;			//�е�������
	volatile uint32_t	TimeCounter;		//����ʱ�������
}Maintenance_Counter;
#endif

/********** ����ϵͳ����ȫ�ֱ����ṹ ***********/
typedef struct 
{
//	uint8_t	COM_Para;			//���ڵĲ���ȫ��һ���ֽ��б�ʾ
	uint8_t	COM_BaudRate;		//���ڲ����� ��D7У�����1����'?'��ʾ����0ʱ�����ԣ�
							//			   D2��D0����ʾ������
	uint8_t	COM_Protocol;		//���ڵ�����Э�飬D3��ʾλ������0��8λ����1��7λ��
							//                D2��ʾ����Э�飬��0��Ӳ������1��XON/XOFF��
							//				  D1��D0��ʾУ�飬00��У�飬01żУ�飬10��У��

 	uint8_t	SysLanguage;		//ϵͳ���ԣ�ʹ��������0=English;1=GB;2=BIG5��
 	uint8_t	SysFontSize;		//ϵͳ����������С��ʹ��������0=12x24;1=9x17��
	uint8_t 	PrintWidth;  		//��������ã�Ϊ�˼���EPSON TM-88�Ŀ��
 	uint8_t	Density;			//��ӡŨ������(0=��,1=��,2=��)

	uint8_t	MaxSpeed;			//����ٶ�
 	uint8_t	CutType;			//�е���־
	uint8_t	BuzzerEnable;		//������ʹ��
	uint8_t	PaperOutReprint;		//ȱֽ���´�ӡ
  uint8_t	BlackMark;			//�ڱ�����(0=��,1=��)
	uint8_t	RightMark;			//�ڱ괫����λ�ã�=0�ұ�,=1��ߣ�Ĭ���ұ�

 	uint8_t	PaperSensor;		//��ֽ����������������(0=��,1=��,2=��)
 	uint8_t	BlackMarkSensor;	//�ڱ괫��������������(0=��,1=��,2=��)
	uint8_t	SYS_ShdnMode;		//ϵͳ�ػ�ģʽ��ʹ������,0=�Ӳ�;1=10����;2=20����;3=30���ӣ�

	uint8_t	PrintDir;			//��ӡ����(0=����,1=����)
	uint8_t	CommandSystem;		//�����0��EPSON ESC/POS�����1��SP΢�������2��쿻�΢�����
	uint8_t 	DefaultCodePage;  	//Ĭ�ϴ���ҳ
	uint8_t	CR_Enable;			//0D�س�ָ��ʹ��

	uint8_t	Manufacturer[21];	//������
	uint8_t	Model[21];			//�ͺ�
	uint8_t	SerialNumber[21];	//���к�
	
	uint8_t	USBIDEnable;		//ʹ��USBID
	uint8_t	CfgFlag[7];			//������Ч��־����ЧֵΪ��WHKJ"
}Sys_Cconfig_Struct;

/********** ����ϵͳ����ȫ�ֱ������� ***********/
typedef union				
{
	Sys_Cconfig_Struct	SysConfigStruct;
	uint8_t Sys_Para_Buff[128];
}Sys_Config_Union;

/********* �����ӡ��״̬ȫ�ֱ����ṹ *************/
typedef struct
{
	volatile uint8_t	PrinterStatus;			//��ӡ��״̬�� n = 1
	volatile uint8_t	OffLineStatus;			//��ӡ���ѻ�״̬�� n = 2
	volatile uint8_t	ErrorStatus;				//��ӡ������״̬�� 	n = 3
	volatile uint8_t	PaperStatus;				//��ӡ������ֽ״̬�� n = 4
}Status_Struct;

typedef struct
{
	volatile uint8_t	PrinterStatus;			//��ӡ��״̬�� n = 1
	volatile uint8_t	OffLineStatus;			//��ӡ���ѻ�״̬�� n = 2
	volatile uint8_t	ErrorStatus;				//��ӡ������״̬�� 	n = 3
	volatile uint8_t	PaperStatus;				//��ӡ������ֽ״̬�� n = 4
}Typedef_struStatus;

/********* �����ӡ������ȫ�ֱ����ṹ *************/
typedef struct
{
	volatile uint8_t	PaperSensorFlag;	//ֽ������״̬��־��λ����������Ч��bit.0=ȱֽ��bit.1= ֽ����
	volatile uint8_t	AR_ErrorFlag;			//���Զ��ָ������־��λ����������Ч��bit.0=��ӡͷ���ȣ�bit.1=�ϸ�
	volatile uint8_t	R_ErrorFlag;			//�ɻָ������־��λ����������Ч��bit.0=�е�λ�ô���bit.1=��ӡͷԭλ����ֻ��9��ʽ�У�
	volatile uint8_t	UR_ErrorFlag;			//���ɻָ������־��λ����������Ч��bit.0=�ڴ��д����bit.1=����ѹ��bit.2=Ƿ��ѹ��bit.3=CPUִ�д���bit.4=UIB����EPSON����bit.7=�¶ȼ���·����
	
	volatile uint8_t	R_ErrorRecoversFlag;	//�ɻָ������־
	volatile uint8_t	PaperEndSensor;				//����ȱֽ�ź����������ѡ��
	volatile uint8_t	PaperEndStop;					//ȱֹֽͣ��ӡ������ѡ��
	
	volatile uint8_t	ASBAllowFlag;					//�Զ�״̬���������־��Bit0��ӦǮ�䣬Bit1���ߣ�Bit2����Bit1ֽ״̬��
	volatile uint8_t	ASBStatus[4];					//�Զ�����״̬�ֽ�
}Error_Struct;

/********* �����ӡ������ȫ�ֱ����ṹ *************/
typedef struct
{
	volatile uint8_t	PaperSensorFlag;	//ֽ������״̬��־��λ����������Ч��bit.0=ȱֽ��bit.1= ֽ����
	volatile uint8_t	AR_ErrorFlag;			//���Զ��ָ������־��λ����������Ч��bit.0=��ӡͷ���ȣ�bit.1=�ϸ�
	volatile uint8_t	R_ErrorFlag;			//�ɻָ������־��λ����������Ч��bit.0=�е�λ�ô���bit.1=��ӡͷԭλ����ֻ��9��ʽ�У�
	volatile uint8_t	UR_ErrorFlag;			//���ɻָ������־��λ����������Ч��bit.0=�ڴ��д����bit.1=����ѹ��bit.2=Ƿ��ѹ��bit.3=CPUִ�д���bit.4=UIB����EPSON����bit.7=�¶ȼ���·����

	volatile uint8_t	R_ErrorRecoversFlag;	//�ɻָ������־
	volatile uint8_t	PaperEndSensor;				//����ȱֽ�ź����������ѡ��
	volatile uint8_t	PaperEndStop;					//ȱֹֽͣ��ӡ������ѡ��

	volatile uint8_t	ASBAllowFlag;			//�Զ�״̬���������־��Bit0��ӦǮ�䣬Bit1���ߣ�Bit2����Bit1ֽ״̬��
	volatile uint8_t	ASBStatus[4];			//�Զ�����״̬�ֽ�
}Typedef_struError;

/********* �����������ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint16_t	TabIndex[32];				//����λ�ñ�����λ�����ַ�����й�
	uint8_t	TabNumber;					//ϵͳ����������
	uint8_t	TabNumIndex;				//ϵͳ����������
}Tab_Struct;

/********* �����������ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	volatile uint8_t		TabNumber;			//ϵͳ����������
	volatile uint8_t		TabIndex;				//ϵͳ����������
	volatile uint16_t	TabTable[32];		//����λ�ñ�����λ�����ַ�����й�
}Typedef_struTab;

/********* ����ʵʱָ��ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint8_t	RealTimeFlag; 				//ʵʱָ����Ч��־������Ч
	uint8_t	RealTimeCntr;				//ʵʱָ�����ݼ�����
	uint8_t	RealTimeAllow;				//ʵʱָ��10 14 ��fn����ʹ�ܱ�־��Bit0��Ӧfn1��Bit2fn2��...
	uint8_t	RealTimeBuf[16];			//ʵʱָ���
}RealTime_Struct;

/********* ����ʵʱָ��ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint8_t	EnableFlag; 	//ʵʱָ����Ч��־������Ч
	uint8_t	CmdCounter;		//ʵʱָ�����ݼ�����
	uint8_t	FnFlag;				//ʵʱָ��10 14 ��fn����ʹ�ܱ�־��Bit0��Ӧfn1��Bit2fn2��...
	uint8_t	Buf[16];			//ʵʱָ���
}Typedef_struRealTimeCommand;

#if	0
#define	USERHZSIZE				10					//�û��Զ��庺���������10��
#define	USERZFSIZE				95					//�û��Զ����ַ��������95�����Զ�����������ͼ�������á�
/********** �����û��Զ����ַ�ȫ�ֱ��� ***********/
typedef struct
{
	uint8_t  UserdefineFlag;					//�������Զ����ַ� ��1B 26ָ��

	uint8_t	UserCode_12[USERZFSIZE];		//12��24�û������ַ������
	uint8_t	Buffer_12[USERZFSIZE*3*12];  	//12*24�û������ַ��Ĵ洢BUFFER

	uint8_t	UserCode_09[USERZFSIZE];		//9��17�û������ַ������
	uint8_t	Buffer_09[USERZFSIZE*3*9];	 	//9*17�û������ַ��Ĵ洢BUFFER
									
	uint8_t	UserCode_HZ[USERHZSIZE];		//�����û������ַ������
	uint8_t	Buffer_HZ[USERHZSIZE*3*24];		//�����û������ַ��Ĵ洢BUFFER
}UserChar_Struct;

#define	USER_DEF_HZ_NUMBERS			(10)	//�û��Զ��庺���������10��
#define	USER_DEF_CHAR_NUMBERS		(95)	//�û��Զ����ַ��������95�����Զ�����������ͼ�������á�
/********** �����û��Զ����ַ�ȫ�ֱ��� ***********/
typedef struct
{
	volatile uint8_t  UserdefineFlag;					//�������Զ����ַ� ��1B 26ָ��	
	volatile uint8_t	UserCode917[USER_DEF_CHAR_NUMBERS];					//9��17�û������ַ������
	volatile uint8_t	Buffer917[USER_DEF_CHAR_NUMBERS * 9 * 3];	 	//9*17�û������ַ��Ĵ洢BUFFER
	volatile uint8_t	UserCode1224[USER_DEF_CHAR_NUMBERS];				//12��24�û������ַ������
	volatile uint8_t	Buffer1224[USER_DEF_CHAR_NUMBERS * 12 * 3]; //12*24�û������ַ��Ĵ洢BUFFER
	volatile uint8_t	UserCodeHZ[USER_DEF_HZ_NUMBERS];					//�����û������ַ������
	volatile uint8_t	BufferHZ[USER_DEF_HZ_NUMBERS * 24 * 3];		//�����û������ַ��Ĵ洢BUFFER
}Typedef_struUserDefChar;
#endif

#if	0
/********** �����û�����λͼȫ�ֱ��� ***********/
typedef struct
{
	uint8_t  BitImageFlag;					//����λͼ��־�������� 
	uint8_t	BitImageData[ BMPSIZE ];		//����λͼ������
}BitImage_Struct;

/********** �����û�����λͼ���Զ����ַ����������� ***********/
typedef union							//λͼ���Զ����ַ�������
{
	BitImage_Struct BitImageStruct;		//����λͼ�ṹ
	UserChar_Struct	UserCharStruct;		//�Զ����ַ�������
	uint8_t	BarCode2DBuf[BARCODE2D_SIZE];	//��ά���뻺����
}RAM_BUFFER;
#endif

typedef struct							//�����ά����ṹ
{
	uint8_t	BarCode2DFlag; 				   	//��ӡ��ά�����־
	uint8_t	BarCode2DType; 					//��ά�������ͣ�0��ʾѡ��PDF417��1ѡ��Data Matrix��2ѡ��QR Code
//	uint8_t	version;
//	uint8_t 	vRate;
//	uint8_t	vColl;
//	uint8_t	*map;
//	uint8_t 	*pSrc;
//	uint8_t  *pDes;
//	uint16_t	datalen;
//	uint16_t	res;
}BarCode2D_Struct;

/*----ϵͳ���̿���ʹ�õ�ȫ�ֱ����ṹ----*/
typedef struct
{
	volatile uint32_t	SleepCounter;		//ϵͳ�������߼�����
	volatile uint32_t	ShutDownCnt;		//ϵͳ����ػ���ʱ��
	volatile uint16_t	StepNumber;			//�����ֽ����
	int16_t		StepTime;				//�����ʱʱ��
	
	volatile uint8_t	SystemMode;			//����ģʽ
	volatile uint8_t	HexMode;				//ʮ�����ƴ�ӡģʽ
	volatile uint8_t	SetMode;				//����ģʽ
	volatile uint8_t	FeedCount;			//Feed������
	
	volatile uint8_t	PaperCtrl;			//ֽ�������ļ�����,λ����,bit.1 = 0�������ȱֽ��bit.1 = 1,���ȱֽ
	volatile uint8_t UserSelectFlag;	//ѡ�����Զ��ַ�	1B 25 ָ��
	volatile uint8_t BitImageFlag;		//ѡ��λͼ��ӡ		1B 2A ָ��
	
	volatile uint8_t	CodePAge;				//��0X80�Ժ�����ݰ�ѡ��Ĺ��ҺŴ�ӡ����
	volatile uint8_t CharacterSet;		//�����ַ���
	
	volatile uint8_t	ucBT_Type;			//�Ƿ�������ģ���־,��0û��,��1��
	
	volatile uint8_t	PaperType;			//ֽ����,0 = ����ֽ�� 1 = ���ֽ
	volatile uint8_t	PrtDevOe;				//��ӡ��������ֹ����,����Ч
	volatile uint8_t	PrtBufFlag;			//��ӡ��־, 0 = ���� and 1 = ����
	volatile uint8_t	PrtSourceIndex;	//��ӡ����Դ����, 0 = ���� and 1 = ����, 2 = USB
	
	volatile uint8_t	LowPowerMode;		//ϵͳ�ػ��Ŀ����֣�1 = On��		
	volatile uint8_t	MarkFlag;				//�ڱ�״̬,����Ч		
}Typedef_struSystemCtrl;	//SystemCtrl_Struct

/*----PARTC:ϵͳ��ӡ����ʹ�õ�ȫ�ֱ����ṹ----*/
typedef struct
{
	//----��ӡ�豸���Ʋ���,----
	volatile uint8_t	PrtBusyFlag;			//��ӡ���̱�־, 0 = ���� 1 = ����ӡ

	volatile uint8_t	CharWidth;				//ASCII�ַ����,12 �� 9
	volatile uint8_t	CharHigh;				//ASCII�ַ���	24��17
	volatile uint8_t	LineHigh;
	volatile uint8_t	MaxRatioY;				//һ�����������Ŵ���

	//----��ӡ���̿��Ʋ���,----
	//--------��ӡ������估���Ŀ���--------
	volatile uint8_t	RealTimeFlag;			//ʵʱָ����Ч��־,����Ч
	volatile uint8_t	RealTimeCntr;			//ʵʱָ�����ݼ�����
	volatile uint8_t	RealTimeBuf[8];			//ʵʱָ���

	volatile uint32_t	PaperPrtMove;		//��ӡ���������ݲ���ֽ����,��׼ģʽ��ҳģʽ����

	//////////////////////////////////////////////////////////
	//----��׼ģʽ��ӡ�������ò��ֶ���----
	//////////////////////////////////////////////////////////
	//--------Ƭ�ڴ�ӡ���������ݹ������--------
//	uint32_t	PrtLineStartLoc;		//��Ч��ʼ��ӡ��λ��,�̶�������λ��
//	uint32_t	PrtLineWidth;			//��Ч��ӡ�е�����
	volatile uint32_t	PrtDataLineIndex;	//��ӡ����,�м�����������һ���ַ���ӡ�Ĵ�ӡ����������һ�δ�ӡ24�У�
	volatile uint32_t	PrtDataDotIndex;	//��ӡ����,�м�����������һ���д�ӡ��ӡ���������

	volatile uint8_t PCDriverMode;			//����PC��������ģʽ 0��û���� 1������
	volatile uint8_t	DriverOffset;			//����ʱ���õľ���λ��
	volatile uint8_t PCPrtLine;				//������ӡ������ 

	volatile uint16_t	StepHoriUnit;		//��ӡ�����ƶ���λ,�������,��ʱ����
	volatile uint16_t	StepVerUnit;		//��ӡ�����ƶ���λ,��������,Ӱ���ӡһ�к����ֽ

	volatile uint16_t MarkFeedLen;			//���þ���ڱ�λ�õĳ��ȡ�

	volatile uint8_t	TabNumber;				//ϵͳ��������,
	volatile uint8_t	TabNumIndex;			//ϵͳ��������,
	volatile uint16_t	TabIndex[32];		//����λ�ñ�,����λ�����ַ�����й�

	volatile uint32_t	CodeLineHigh;		//�ַ�ģʽ�µ��и߿���,�������
	volatile uint32_t	StdCurrentLineHigh;	//��׼ģʽ�µĵ�ǰ�и�
	volatile uint8_t	CodeAimMode;			//�ַ�����ģʽ,0 = �����, 1 = �м����, 2 = �Ҷ���
	volatile uint8_t	CodeInversion;			//���ô�ӡ����,0 = normal Mode, 1= Inversion Mode

	//-- һ���ַ������ַ����+�ַ��Ҽ��----
	volatile uint8_t	MaxLineHigh;			//һ������ߵ��ַ�/λͼ�ߴ�

	//-- PrtLeftLimit + PrtLength + PrtRightLimit = �ɴ�ӡ����
	volatile uint16_t PrtLeftLimit;			//��ӡ������߾�,��ҳģʽ���ò�ͬ��ֵ
	volatile uint16_t PrtLength;			//��Ч��ӡ����,��ҳģʽ���ò�ͬ��ֵ
	volatile uint16_t PrtRightLimit;			//��ӡ�����ұ߾�,��ҳģʽ���ò�ͬ��ֵ

	volatile uint8_t	CharCycle90;			//�ַ���ת90�����,����Ч
	volatile uint8_t	CharRBFlag;				//�ַ��ߵ�����ת180�㣩��ӡ���ƣ�����Ч
	volatile uint8_t	CharNotFlag;			//�ַ����ԵĿ��ƣ�����Ч

//	uint8_t  RasterFlag;				//��ӡ��դ�ı�־	1113
//	uint8_t  RasterHigh;				//��դ�ĸ߶�		1113
	volatile uint8_t OneLineWidthFlag;		//���ڱ����־
	volatile uint8_t	DoUnderlineFlag;		//���ӡ�»��߱�־

	volatile uint8_t	PrintCurveFlag;			//���ߴ�ӡ��־����1�����ߴ�ӡ
}Typedef_struPrtCtrl;		//PRTCtrl_Struct

/* Funtion Declare -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H������� */

/******************* (C) COPYRIGHT 2012 WH *****END OF FILE****/
