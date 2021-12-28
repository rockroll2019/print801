/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : GlobalVariable.h
* Author             : 
* Version            : V1.0
* Date               : 2016-4-3
* Description        : Header for module
********************************************************************************
* 
*******************************************************************************/
#ifndef DEFCONSTANT_H
#define DEFCONSTANT_H

#include	"stm32f10x.h"
#include	"debug.h"
#include	"custom.h"
#include "wifi.h"
/**************************    �궨����    **************************************/
/*********************************************************************************
		ȫ��Ƭ�ڣ��ֽڣ��ڴ����:
		1.�д�ӡ������:��󱶸ߡ����и�/3�����п�,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
		  Ƭ��:���֧�� 8���� x24��ߡ�576�㡣��ַ�¶���
		  ���:     		13824B 8����24/8����576
		2.�»��߻�����:	576B   �п�
		3.�û��Զ����ַ�:	6175B 95��(2+36+27),�ֱ��¼95��12x24/9��17������û��Զ����ַ�
		4.����λͼ����:	12KB
		5.�û������:   	2KB(2048�ֽ�)
		6.��ֱ�Ʊ�:		256B
		7.ˮƽ�Ʊ�:	 	144B ��576/8����2
		8.�û������դ:	2K(2048�ֽ�)
		9.ָ����ջ���:	4K��4096�ֽڣ�,g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
		10.ҳģʽ��ӡ������:128K,Ƭ���ַ
**********************************************************************************/
#define	MODAL_NAME_STRING			"T801"			
#define	FIRMWARE_STRING				"T801M-V0.53"
#define	COMPANY_NAME_STRING		"AYHL"

#define	PARA_CHECK_STRING			"IPRT"		//ϵͳ����д��flashУ���ַ���

#if defined	STM32F10X_CL
	#define	DOWNLOAD_BITMAP_IN_RAM_ENABLE
#endif

//#define	MAINTAIN_ENABLE         // ά��������
//#define	BARCODE_2D_ENABLE       

#define	RX_UNFINISHED			(0)
#define	RX_FINISHED				(1)
#define	RX_FLAG_TIM				(TIM5)
#define	RX_DELAY_MS				(50)

#define	ROM_VERSION				(0x64)   
#define	FIRMWARE_VERSION	(0x01)

#define	DRV_PRT_TYPE_NONE	(0x00)
#define	DRV_PRT_TYPE_IP		(0x01)
#define	DRV_PRT_TYPE_QT		(0x02)

#define	LED_FLASH_MS											(160)
#define	LED_FLASH_TOTAL_INTERVAL_MS				(2560)

#define	UNDERLINE_ENABLE_MASK							(1 << 3)
#define	UNDERLINE_DISABLE_MASK						(~(1 << 3))
#define	UNDERLINE_ENABLE_ONE_DOT_MASK			(0x09)
#define	UNDERLINE_ENABLE_TWO_DOT_MASK			(0x0B)
#define	UNDERLINE_ENABLE_THREE_DOT_MASK		(0x0F)

/********* ��������������С **********/
/****************************/
#define	MAX_RATIO_X				(8)					//���������
#define	MAX_RATIO_Y				(8)					//���������

#define	DrvPrtBufDeep			(MAX_RATIO_Y*24)

#define	PRT_DOT_NUMBERS		(576)
#define PRT_BUF_LENGTH		(PRT_DOT_NUMBERS * MAX_RATIO_Y * 24 / 8)  //�д�ӡ��������С

#define	PAGE_HEIGHT_DOT_MAX	(1024 * 6)	//�൱�����򳤶�128mm * x(ռ�ڴ�72K * x) δ��
#define PAGE_BUF_LENGTH			(PRT_DOT_NUMBERS / 8 * PAGE_HEIGHT_DOT_MAX)  //ҳ��ӡ��������С

//��������λͼ�� X*Y���ֵ ,�����о��Ϊ576��,�����ػ�����٣���ӡ�������»��߻��壩
#ifdef	DOWNLOAD_BITMAP_IN_RAM_ENABLE
	#define	BMP_LENGTH			(48 * 256)	//һ��λͼ���1536�ֽ�
	#define	BMP_MAX_NUM			(1)
	#define	BMP_SIZE				((2 * BMP_MAX_NUM) + (BMP_MAX_NUM * BMP_LENGTH))	//��������λͼ��󻺳���
#else
	#define	BMP_LENGTH			(8 * 1024)	//һ��λͼ���7K�ֽ�
	#define	BMP_MAX_NUM			(1)
	#define	BMP_SIZE				((2 * BMP_MAX_NUM) + (BMP_MAX_NUM * BMP_LENGTH))	//��������λͼ��󻺳��� �����ӵ������ݴ�С
#endif

#define	RASTER_SIZE				BMP_SIZE		//�����ӡ��դ����,������ͼ�ι���
#define	BARCODE2D_SIZE		BMP_SIZE		//�����ά���뻺��,������ͼ�ι���
#define	MACRO_SIZE					(1)		//�����Ļ���  2015.06.16

#define	USER_HZ_SIZE				(10)			//�û��Զ��庺����,���10��
#define	USER_ZF_SIZE				(95)			//�û��Զ����ַ���,���95��,�Զ�����������ͼ�������á�

#define	COMMAND_ESCP				(0)
#define	COMMAND_IPRT				(1)
#define	COMMAND_WHKJ				(2)
#define	COMMAND_OTHER				(2)

/******* �������Դ��� *************/
#define	LANGUAGE_ENGLISH		(0)
#define	LANGUAGE_CHINESE		(1)
#define	LANGUAGE_GB18030		(1)
#define LANGUAGE_BIG5				(2)

#define	LANGUAGE_NUM				(3)
#define	DEFAULT_CHINESE_LANGUAGE	(LANGUAGE_CHINESE)

#ifdef	DBG_SPI_ZIKU
#define LANGUAGE_DEFAULT		(LANGUAGE_ENGLISH)
#else
#define LANGUAGE_DEFAULT		(LANGUAGE_CHINESE)
#endif

#define	INTERNATIONAL_CHAR_SET_INDEX_ENGLISH		(0)
#define	INTERNATIONAL_CHAR_SET_INDEX_CHINESE		(15)

/*********** �е����ӡλ�ò��� ******************/
/* �е��ʹ�ӡλ�õ�ֵ�治ͬ��ӡͷ�����շ�ʽ��ͬ����Ҫ���� */
#define CUT_OFFSETNUM					(230)	  //2017.02.07  230 ��ֽ����ֽ��
#define CUT_OFFSET						(128)	  // X mm * 8dot/mm, ��ӡλ������ֽλ�õ�ƫ��ֵ,����͡��е���� 2016.06.19 33 * 8��Ϊ128
#define NO_CUT_OFFSET					(112)	  // X mm * 8dot/mm, �е���˺ֽλ�õ�ƫ��ֵ,����͡��е����   14*8 ��Ϊ112
#define CUT_STATUS_ON_TIME		(150)		//�е���⿪�����պ�ʱ��(��ԭλ��,��λms 
#define CUT_STATUS_OFF_TIME		(600)		//�е���⿪������ʱ�� 

/********** ��о������� ****************/
#define STEP_NUMBERS_PER_DOT_HEIGHT	(1)		//ÿ���е������

/********** ��о���� ****************/
#define EPSON_DOTNUMBER		(PRT_DOT_NUMBERS - 512)		//���ֻ�о��ȲEPSON���ݣ�
#define CUSTOM_57MM_PRT_DOT_NUMBERS	(384)		//57mmֽ�����õĵ�����������

#define	MIN_STEP_UNIT_X		(0.125)		//������С�ƶ���λ
#define	MIN_STEP_UNIT_Y		(0.125)	 	//������С�ƶ���λ
#define TEMP_COEFFICIENT	(4)				//�����¶Ȳ���ϵ��

/********* ���������ٶȶ�Ӧ�ļ��ٲ��� **********/
#define	SPEED_LOW							58						//150mm/s ÿ��Լ835uS  
#define	SPEED_MIDDLE 					94						//190mm/s ÿ��Լ657uS		94
#define	SPEED_HIGH 						126						//220mm/sÿ��Լ568uS		
#define	SPEED_MAX							126						//250mm/s ÿ��Լ568uS		
#define LPT_Drver_MaxStep     74            //170mm/s ÿ��Լ744uS  74 ������������ٶ�  
#define USB_Drver_MaxStep     94            //150mm/s ÿ��Լ835uS 58 usb����������ٶ�

/********* �������߹ػ�ʱ�� **********/
#define SLEEP_TIME_MS			(500)		//�Զ���������ʱ��,��λΪmS, 5000mS

#define	PRINTABLE_START_CODE								(0x20)
#define	CODEPAGE_START_CODE									(0x80)

/*************FLASH�ֿ������*************/
//#define	BIG5_ZK_INDEPENDENT		//�ֿ��Ƕ����Ļ��ǲ���
#define	ZK_ENCRYPTION		//�ֿ��Ƕ����Ļ��ǲ���

//GB18030 24*24 ���ֻ���ַ,���� 28576*72=0x1F6500, ���� 0x3A64FF
#define	GB18030_24X24_BASE_ADDR							(0x180000)
#define	GB18030_16X16_BASE_ADDR							(0x180000)

//BIG5 24*24 ���ֻ���ַ������Ҫͨ��ת����ת�������޸�
// 13973*2 = 0x6D2A, 0x125B00-0x12C829
#define	BIG5_BASE_ADDR											(0x125B00)

//12*24�����ַ�����ַ, (12*16)*36=0x1B00, ���� 0xE277F
#define	INTERNATIONAL_CODE_12_24_BASE_ADDR	(0xE0C80)

//12*24ASCII�ַ�����ַ, 96*36=0xD80, ���� 0xE34FF
#define	ASCII_12_24_BASE_ADDR								(0xE2780)

//12*24����ҳ�ַ�����ַ, 59*128*36=0x42600, ���� 0x125AFF
#define	ASCII_12_24_CODEPAGE_BASE_ADDR			(0xE3500)

//9��17�����ַ�����ַ, (12*16)*27=0x1440, ���� 0x16E43F
#define	INTERNATIONAL_CODE_09_17_BASE_ADDR	(0x13D000)
#define	INTERNATIONAL_CODE_08_16_BASE_ADDR	(0x13D000)

//9*17ASCII�ַ�����ַ, 96*27=0XA20, ���� 0x16EE5F
#define	ASCII_09_17_BASE_ADDR								(0x13E440)
#define	ASCII_08_16_BASE_ADDR								(0x13E440)

//9*17����ҳ�ַ�����ַ, 59*128*27=0x31C80, ���� 0x1A0ADF
#define	ASCII_09_17_CODEPAGE_BASE_ADDR			(0x13EE60)
#define	ASCII_08_16_CODEPAGE_BASE_ADDR			(0x13EE60)

//�ֿ�汾��ַ
#define	ZK_Ver_ADDR		    	(0x13CFFB)

//Ԥ��λͼ����ַ,������64KΪ��λ����ʼ��ַ, 5*64K=320K, ���� 0x3FFFFF
#define	BIT_BMP_DATA_BASE_ADDR							(0x380000)

//Ԥ��λͼ���Ʋ�����ŵ�ַ ?
#define	BIT_BMP_PARA_BASE_ADDR							(0x3F0000)

//ʹ�������Ȳ�����ŵ�ַ ?
#define	MAINTAIN_PARA_BASE_ADDR							(0x3FC000)

//ϵͳ������ŵ�ַ, 4K, ���� 0x166FFF 
#define	SYS_PARA_BASE_ADDR									(0x136000)

#define	NV_FLASH_SPACE_MAX									(0x400000)	//�ֿ�����ַ
#define	NV_FLASH_END_ADDR										(0x400000)	//�ֿ�����ַ
#define	BIT_BMP_MAX_STORE_SPACE							(320*1024)	//����λͼԤ�������洢��ַ512K  5��64K�Ŀ�

/*************��������SRAM��ַ*************/
//����sram��Ϊ������ 1Ϊҳģʽ���û���256K��ʣ����Ϊ����������
#define ExtSRamBaseAddr							(0x60000000)	                    //�ⲿRAM�Ļ���ַ
#define PageModeBaseAddr			      ExtSRamBaseAddr	                  //�ⲿҳģʽ����������ַ
#define PageModeBufNum			        0x40000			                      //�ⲿҳģʽ��������С
#define ExtInBufBaseAddr				    (PageModeBaseAddr+PageModeBufNum) //�ⲿ���ջ���������ַ
                        
/*************����CODEPAGE*************/
#define	MAX_CODEPAGE_SET_NUM				(63)	//����ҳ���ѡ����Ŀ,59

/*************��ӡ������*************/
#define	PageX0						g_tPrtCtrlData.PageArea[0]	   //ҳ��ӡ����X����ʼ��
#define	PageY0						g_tPrtCtrlData.PageArea[1]	   //ҳ��ӡ����y����ʼ��
#define	PageDx						g_tPrtCtrlData.PageArea[2]	   //ҳ��ӡ����X�������
#define	PageDy						g_tPrtCtrlData.PageArea[3]	   //ҳ��ӡ����X�������

#define PageDotIndex			g_tPrtCtrlData.PageDataDotIndex   //ҳģʽ�µ�ǰ��λ��
#define	PageLineIndex			g_tPrtCtrlData.PageDataLineIndex  //ҳģʽ�µ�ǰ��λ��

#define	STD_MODE										(0)		//��׼��ӡģʽ
#define	PAGE_MODE										(1)		//ҳ��ӡģʽ

#define	ASCII_FONT_A								(0)		//�ַ���������,12*24����
#define	ASCII_FONT_B								(1)		//�ַ���������,9*17����
#define	HZ_FONT_A										(2)		//���ֵ�������,24*24����
#define	ASCII_FONT_C								(3)		//�ַ���������,8*16����
#define	HZ_FONT_B										(4)		//���ֵ�������,16*16����

#define	ASCII_FONT_DEFAULT					(ASCII_FONT_A)	//Ĭ���ַ���������
#define	HZ_FONT_DEFAULT							(HZ_FONT_A)

#define	ASCII_FONT_A_WIDTH					(12)
#define	ASCII_FONT_B_WIDTH					(9)
#define	ASCII_FONT_C_WIDTH					(8)
#define	HZ_FONT_A_WIDTH							(24)
#define	HZ_FONT_B_WIDTH							(16)
#define	MAX_ASCII_FONT_WIDTH				(12)
#define	MAX_FONT_WIDTH							(24)

#define	ASCII_FONT_A_HEIGHT					(24)
#define	ASCII_FONT_B_HEIGHT					(17)
#define	ASCII_FONT_C_HEIGHT					(16)
#define	HZ_FONT_A_HEIGHT						(24)
#define	HZ_FONT_B_HEIGHT						(16)
#define	MAX_ASCII_FONT_HEIGHT				(24)
#define	MAX_FONT_HEIGHT							(24)

#define	DEFAULT_ASCII_FONT_WIDTH		(ASCII_FONT_A_WIDTH)
#define	DEFAULT_ASCII_FONT_HEIGHT		(ASCII_FONT_A_HEIGHT)
#define	DEFAULT_HZ_FONT_WIDTH				(HZ_FONT_A_WIDTH)
#define	DEFAULT_HZ_FONT_HEIGHT			(HZ_FONT_A_HEIGHT)

#define	ASCII_FONT_WIDTH_DEFAULT		(ASCII_FONT_A_WIDTH)
#define	ASCII_FONT_HEIGHT_DEFAULT		(ASCII_FONT_A_HEIGHT)
#define	HZ_FONT_WIDTH_DEFAULT				(HZ_FONT_A_WIDTH)
#define	HZ_FONT_HEIGHT_DEFAULT			(HZ_FONT_A_HEIGHT)

/******* ���������ռ���ֽ��� *************/
#define BYTES_OF_ASCII_FONT_A				(36)	//12 X 24 �����ַ��ĵ��������ֽ���
#define BYTES_OF_ASCII_FONT_B				(27)	//9 X 17 �����ַ��ĵ��������ֽ���
#define BYTES_OF_ASCII_FONT_C				(16)	//8 X 16 �����ַ��ĵ��������ֽ���
#define BYTES_OF_HZ_FONT_A					(72)	//24 X 24 �����ַ��ĵ��������ֽ���
#define BYTES_OF_HZ_FONT_B					(32)	//16 X 16 �����ַ��ĵ��������ֽ���
#define	MAX_BYTES_OF_CHAR						(BYTES_OF_HZ_FONT_A)
#define	MAX_BYTES_OF_HZ							(BYTES_OF_HZ_FONT_A)

//���뷽ʽ
#define	AIM_MODE_LEFT								(0)
#define	AIM_MODE_MIDDLE							(1)
#define	AIM_MODE_RIGHT							(2)

#define	PRINT_DIRECTION_NORMAL			(0)		//Ĭ�������ӡ
#define	PRINT_DIRECTION_REVERSE			(1)		//�����ӡ

#define	MAX_HT_INDEX_NUM						(32)	//�������������Ŀ
#define	DEFAULT_HT_CHAR_NUM					(8)		//Ĭ�������ַ����ÿ��
#define	DEFAULT_HT_WIDTH						(DEFAULT_HT_CHAR_NUM * MAX_ASCII_FONT_WIDTH)	//Ĭ������������

#define	PAPER_TYPE_CONTINUOUS				(0)		//����ֽ
#define	PAPER_TYPE_BLACKMARK				(1)		//������ֽ,�ڱ�ֽ
#define	PAPER_TYPE_LABLE				    (2)		//��ǩֽ

#define CMD_POS                             (0)     //ָ������
#define CMD_CPCL                            (1)
#define CMD_CPCLU                           (2)

#define	PRINTER_STATUS_ILDE					(0)
#define	PRINTER_STATUS_BUSY					(1)

#define	PRINT_PROCESS_STATUS_ILDE		(0)
#define	PRINT_PROCESS_STATUS_BUSY		(1)

#define	DEFAULT_STEP_HORI_UNIT			(1)		//Ĭ�Ϻ�����С�ƶ���λ
#define	DEFAULT_STEP_VER_UNIT				(1)		//Ĭ��������С�ƶ���λ

//2016.07.29 �������ݳ���
#define SETSTRINGLEN	32	           //�����ַ�����

//2017-5-26  ��������ģ������
#define SPEAKERLOW				0xFFE2
#define SPEAKERMIDDLE			0xFFE5
#define SPEAKERHIGHT			0xFFED
#define SPEAKECLOSED      0x03

typedef struct
{
	uint16_t 	size;		  //����ָ��
	uint16_t 	ptr_in;		//����ָ��
	uint16_t	ptr_out;	//���ָ��
	uint16_t	length;		//���ݳ���
	uint8_t		State;		//״̬��־ 
	uint8_t		*Buffer;	//���ݻ�������ַ
}APP_Tx_Struct;


#ifdef	MAINTAIN_ENABLE
/******* ����ά�����������ͽṹ *************/
typedef struct
{
	u32	LineCounter;		//��ֽ�м�����
	u32	HeatCounter;		//���ȴ���������
	u32	CutCounter;			//�е�������
	u32	TimeCounter;		//����ʱ�������
}Maintenance_Counter;
#endif

/********** ����ϵͳ����ȫ�ֱ����ṹ ***********/
typedef struct 
{
	uint8_t	ComBaudRate;			//���ڲ����� ,D7У�����1,��'?'��ʾ,��0ʱ,����,
														//D2��D0,��ʾ������
	uint8_t	ComProtocol;			//���ڵ�����Э��,D3��ʾλ��,��0,8λ,��1,7λ,
														//D2��ʾ����Э��,��0,Ӳ��,��1,XON/XOFF,
														//D1��D0��ʾУ��,00,10��У��,11żУ��,01��У��
 	uint8_t	SysLanguage;			//ϵͳ���ԣ�ʹ������,0=English;1=GB;2=BIG5��
 	uint8_t	SysFontSize;			//ϵͳ����������С��ʹ������,0=12x24;1=9x17��
	uint8_t PrintWidthSelect; //���������,Ϊ�˼���EPSON TM-88�Ŀ��
 	uint8_t	Density;					//��ӡŨ������(0=��,1=��,2=��)
	uint8_t	MaxSpeed;					//����ٶ�
 	uint8_t	CutType;					//�е���־
	uint8_t	BuzzerEnable;			//������ʹ��,=0��ֹ;=1,ȱֽ,����ʱ����;=2,��ֽʱ����;=3,ȱֽ,����,��ֽʱ����
	uint8_t	PaperOutReprint;	//ȱֽ���´�ӡ
  uint8_t	BlackMarkEnable;	//�ڱ�����(0=��,1=��)
	uint8_t	BlackMarkPosition;	//�ڱ괫����λ��,=0�ұ�,=1���,Ĭ���ұ�
 	uint8_t	PaperSensorSensitive;	//��ֽ����������������(0=��,1=��,2=��)
 	uint8_t	BlackMarkSensitive;	//�ڱ괫��������������(0=��,1=��,2=��)
	uint8_t	SysShutDownMode;	//ϵͳ�ػ�ģʽ��ʹ������,0=�Ӳ�;1=10����;2=20����;3=30���ӣ�
	uint8_t	PrintDir;					//��ӡ����(0=����,1=����)
	uint8_t	CommandSystem;		//���,0��EPSON ESC/POS���,1��IP΢�����,2��쿻�΢�����
	uint8_t DefaultCodePage; 	//Ĭ�ϴ���ҳ
	uint8_t	CREnable;					//0D�س�ָ��ʹ��
	
	uint8_t	Manufacturer[21];	//������
	uint8_t	Model[21];				//�ͺ�
	uint8_t	SerialNumber[21];	//���к�
	
	uint8_t	USBIDEnable;			//ʹ��USBID
	uint8_t USB_Class;			 //USB���ͣ�0����ӡ���࣬1�����⴮���ࣨVCP�ࣩ
	
	uint8_t DHCPEnable;       //����DHCPЭ�� �Զ���ȡIP����ʹ���ֽ�
	uint8_t	PaperWith;        //2017.03.2
	
	uint8_t PaperNearEndSensorEnable;			  //ֽ����ʹ�ܿ���
	uint8_t PaperNearEndSensorSensitive;		//ֽ����������������
	uint8_t PaperTakeAwaySensorEnable;			//ȡֽʹ�ܿ���
	uint8_t PaperTakeAwaySensorSensitive;		//ȡֽ������������
  
  uint8_t ProductiveData[4];							//��Ʒ�������� 2016.05.31
	uint8_t PrintMode;											//��ӡ����ģʽ 2016.05.31
	uint8_t SpeakerVolume;
	uint16_t AlarmNumber;										//����ѡ��  ���ֽ�
	uint16_t POutVoiceNumber;								//����ѡ��
	uint8_t	ConfigFlag[7];		             //������Ч��־,��ЧֵΪ��IPRT"

}TypeDef_StructSysConfig;

/********** ����ϵͳ����ȫ�ֱ������� ***********/
typedef union
{
	TypeDef_StructSysConfig	SysConfigStruct;
	uint8_t bSysParaBuf[sizeof(TypeDef_StructSysConfig)];     //2016.04  128��Ϊsizeof����
}TypeDef_UnionSysConfig;

/******* ����ӿ����� *************/
enum g_eComPortType{ COM = 0, LPT, USB, ETHERNET,BLUETOOTH};

/******* ����ָ����� *************/
enum g_eCmdSystemType{CMD_SYS_ESCPOS = 0, CMD_SYS_IPMP, CMD_SYS_WHMP, CMD_SYS_OTHER};

/******* ����ӿ����ͽṹ *************/
typedef struct
{
	uint8_t	COM_Type;
	uint8_t	LPT_Type;
	uint8_t	USB_Type;
	uint8_t	ETH_Type;
  uint8_t	BT_Type;
	uint8_t	WIFI_Type;
}TypeDef_StructInterfaceType;

/******* ����Ǯ����ƽṹ *************/
typedef struct
{
	volatile uint8_t	Status;			//Ǯ��״̬,0Ǯ��δ����,1Ǯ��������
	volatile uint8_t	DrwID;			//Ǯ����
	volatile uint16_t	ON_Time;		//Ǯ�俪��ʱ��,����λ����
 	volatile uint16_t	OFF_Time;		//Ǯ��ر�ʱ��,����λ����
}TypeDef_StructDrawer;

/********** ����궨��ȫ�ֱ����ṹ ***********/
typedef struct
{
	uint16_t	Number;						//���ַ�����
	uint16_t	GetPointer;				//��ȡ��ָ��
	uint8_t		DefinitionFlag;		//��������־
	uint8_t		SetFlag;					//�Ѷ����ı�־
	uint8_t		RunFlag;					//�����к����ݱ�־
	uint8_t		Buffer[MACRO_SIZE];	//�����ݻ�����
}TypeDef_StructMacro;

/********** ����һά�������ȫ�ֱ����ṹ ***********/
typedef struct
{
	uint8_t		HRIPosition;				//HRI�Ĵ�ӡλ��,0=����ӡ,1=������,2=������,3=��������, 
	uint8_t		HRIFont;						//HRI���� 		0=12*24,1=9*17
	uint8_t		BarCodeThinWidth;		//����խ�����,��0.125mmΪ������λ�ı���
	uint8_t		BarCodeThickWidth;	//����������,��0.125mmΪ������λ�ı���
	uint8_t		BarCodeHigh;				//�����ӡ�߶�,��0.125mmΪ������λ�ı���
}TypeDef_StructBarCtrl;

/********** �����ַ��Ű����ȫ�ֱ����ṹ ***********/
typedef struct
{
 	uint8_t		WidthMagnification;		//����Ŵ���Ratio
	uint8_t		HeightMagnification;	//����Ŵ���Ratio
	uint8_t		EmphaSizeMode;				//�Ӵ�ģʽ
	uint8_t		Underline;						//�ַ��»��߿���,bit3 = on/off; bit1~0 = �»�����ʽ������˫�ߣ�
	uint8_t		CharWidth;						//�ַ����ֿ�
	uint8_t		CharHigh;							//�ַ����ָ�
	uint16_t	LeftSpace; 						//����
	uint16_t	RightSpace;						//�Ҽ��
}TypeDef_StructPrintMode;

/********** ����ҳģʽ��ӡ����ȫ�ֱ����ṹ ***********/
typedef struct
{
	uint32_t	PageLineHigh;					//ҳģʽ�µ��и߿���,�������
	uint32_t	PageCurrentLineHigh;	//ҳ׼ģʽ�µĵ�ǰ�и�

  uint32_t	PageArea[4];					//ҳģʽ�´�ӡ����,����ΪX0,Y0,DX,DY
	uint32_t	PageVerLoc;						//ҳģʽ�µ����λ��

	uint32_t	PageLeftLimit; 				//��ӡ������߾�,���׼ģʽ���ò�ͬ��ֵ
	uint32_t	PageRightLimit;				//��ӡ�����ұ߾�,���׼ģʽ���ò�ͬ��ֵ

	uint32_t	PageLineStartLoc;			//��Ч��ʼ��ӡ��λ��,�̶�������λ��
	uint32_t	PageLineWidth;				//��Ч��ӡ�е�����
	uint32_t	PageDataLineIndex;		//��ӡ����,�м�����������һ���ַ���ӡ�Ĵ�ӡ����������һ�δ�ӡ24�У�
	uint32_t	PageDataDotIndex;			//��ӡ����,�м�����������һ���д�ӡ��ӡ���������

	uint32_t	PageNumber;						//ҳģʽ�µ���ҳ������
	uint32_t	PageWritePoint;				//ҳģʽ�µ�ҳ�����Ƽ�����
	uint32_t	PageReadPoint;				//ҳģʽ�µ�ҳ��ӡ������

	uint8_t		AscRightSpace; 				//ASCII�ַ����Ҽ��	
	uint8_t		HZRightSpace;					//�����ַ����Ҽ��
	uint8_t		HZLeftSpace;					//�����ַ�������
	uint8_t		PageFeedDir;					//ҳģʽ�µĴ�ӡ����,0 = ����;1 = �µ���; 2 = �ҵ���; 3 = �ϵ���
	uint8_t		PageToStandMode;			//��ҳģʽ���ص���׼ģʽ��־  //1208
	uint8_t		PagePrted;						//�Ѿ���ӡ��ҳ�����е�����	  //1207
	uint8_t		PagePrtOe;						//ҳģʽ��,��ӡ�������
}TypeDef_StructPageMode;

/********** ���尴������ȫ�ֱ����ṹ ***********/
typedef struct
{
	uint32_t StepCount;      //�����߲�����
	uint8_t	KeyDownFlag;		//1 = ����, 0= �ɿ�  �������±�־
	uint8_t	KeyEnable;			//0 = ������, 1= ��ֹ����
	uint8_t	KeyStatus;			//������Ч��־,����Ч
	uint8_t	KeyCount;				//��������
}TypeDef_StructKeyCtrl;

/********** ����ϵͳ���̿���ʹ�õ�ȫ�ֱ����ṹ ***********/
typedef volatile struct
{
	volatile  uint32_t	SleepCounter;    	//ϵͳ�������߼�����
	uint32_t	ShutDownCounter;	//ϵͳ����ػ���ʱ��
	
	uint16_t	HeatTime;					//Ĭ�ϼ���ʱ��
	uint16_t	LineWidth;				//�п�
	uint16_t	StepNumber;
	
	uint8_t		HexMode;			    //ʮ�����ƴ�ӡģʽ
	uint8_t		SetMode;			    //����ģʽ
	uint8_t		SelfTestMode;		  //�Լ�ģʽ
  uint8_t		AgingTestMode;		//�ϻ�����ģʽ          //2016
  uint8_t		ParamRecoverMode;	//�����ָ�ģʽ 
	uint8_t		UserSelectFlag;		//ѡ�����Զ��ַ� 1B 25 ָ��
	uint8_t		CharacterSet;		  //�����ַ���
	uint8_t		PaperType;			  //ֽ����,0 = ����ֽ; 1 = ���ֽ  2 = ��ǩֽ 
	uint8_t		PrtDeviceDisable;	//��ӡ��������ֹ����,����Ч,=1,����ֹ;=0������
	uint8_t		PrtModeFlag;		  //��ӡģʽ���� ,0 = ��ͨģʽ and 1 = ҳģʽ
	uint8_t		PrtSourceIndex;		//��ӡ����Դ����, 0 = RS232 and 1 = BLUETOOTH, 2 = USB 3 = ETH
	uint8_t		MarkPositionFlag;	//�ڱ�״̬,����Ч
	uint8_t		CutRunFlag;			  //�е�������
  uint8_t 	MaxStepPrtNum[2];	//�����ٲ�������0�ݣ�ָ��ģʽ����1�ݣ�����ģʽ  
	uint8_t   CutLedFlag;				//�е����� �ƴ�ָʾ��־
	uint8_t   BillIDCutFlag;		//�е����� ÿһ����һ����Ϊ���ݽ�����־
  volatile uint8_t CMDType;    //ָ������  
  volatile uint8_t BillPrtFlag; //Ʊ�ݴ�ӡ״̬
	uint8_t   CutLedFlagBuf1;
	uint8_t   CutLedFlagBuf2;
}TypeDef_StructSystemCtrl;

/********* �����ӡ��״̬ȫ�ֱ����ṹ *************/
typedef struct
{
 	uint8_t		PrinterStatus;		//��ӡ��״̬, n = 1
	uint8_t		OffLineStatus;		//��ӡ���ѻ�״̬, n = 2
	uint8_t		ErrorStatus;			//��ӡ������״̬, n = 3
	uint8_t		PaperStatus;			//��ӡ������ֽ״̬, n = 4
}TypeDef_StructStatus;

/********* �����ӡ������ȫ�ֱ����ṹ *************/
typedef struct
{
	volatile uint8_t	PaperSensorStatusFlag;	//ֽ������״̬��־,λ����,����Ч,bit.0=ȱֽ;bit.1= ֽ����;bit.2=δȡֽ
	volatile uint8_t	AR_ErrorFlag;		//���Զ��ָ������־,λ����,����Ч,bit.0=��ӡͷ����;bit.1=�ϸ�
	volatile uint8_t	R_ErrorFlag;		//�ɻָ������־,λ����,����Ч,bit.0=�е�λ�ô���;bit.1=��ӡͷԭλ����ֻ��9��ʽ�У�,bit.2=��ֽ��
	volatile uint8_t	UR_ErrorFlag;		//���ɻָ������־,λ����,����Ч,bit.0=�ڴ��д����;bit.1=����ѹ;bit.2=Ƿ��ѹ;bit.3=CPUִ�д���;bit.4=UIB����(EPSON);bit.7=�¶ȼ���·����
	
	volatile uint8_t	PaperEndStopSensorSelect;	//ȱֹֽͣ��ӡ������ѡ��,bit0=1,PNE��Ч;bit1=1,PE��Ч
	volatile uint8_t	LPTPaperEndSensorSelect;	//����ȱֽ�ź����������ѡ��,bit0=1,PNE��Ч;bit1=1,PE��Ч
	volatile uint8_t	R_ErrorRecoversFlag;	//����ɻָ������־
	
	volatile uint8_t	ASBAllowFlag;		        //�Զ�״̬���������־,Bit0��ӦǮ��,Bit1����,Bit2����,Bit3ֽ״̬,
	volatile uint8_t	ASBStatus[4];		        //�Զ�����״̬�ֽ�
    
  volatile uint8_t    DoorOpen;                  //ֽ�ո�״̬
    
}TypeDef_StructError;

/********* �����������ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint8_t		TabSetNumber;	//ϵͳ��������,�����õ���Ч����������,��󲻳���32
	uint8_t		TabIndex;			//ϵͳ��������,��Ŀǰ������λ��,ÿִ��һ������,ֵ����1
	uint16_t	TabPositionIndex[MAX_HT_INDEX_NUM];	//����λ�ñ�,����λ�����ַ����,�Ҽ��,����Ŵ����й�
}TypeDef_StructTab;

/********* �����ӡ����ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint16_t	PrtDataDotIndex;		//�ַ���ģ���뻺����ָ��
	uint16_t	CodeLineHigh;				//�ַ�ģʽ�µ��и߿���,�������
	uint16_t	PrtLeftLimit; 			//��ӡ������߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
	uint16_t	PrtRightLimit; 			//��ӡ�����ұ߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
	uint16_t	PrtWidth; 					//��ӡ���,ʵ��ʹ�õĴ�ӡ����
	uint16_t	PrtLength; 					//�ܴ�ӡ���= ��߾� + ��ӡ���,������ߵĿհ׺͹涨�Ĵ�ӡ����
	uint16_t	MaxPrtDotHigh;			//�༭/��ӡ�����������Ի���Ϊ��׼����,���д�ӡ�����и߶���ߵĵ���ֵ,
																//��ʼ��Ϊ0,�༭�ַ�������,ͼ��,��դ,������ȹ�����ʵʱ���´�ֵ
	uint16_t	StepHoriUnit;				//��ӡ�����ƶ���λ,�������,��ʱ����
	uint16_t	StepVerUnit;				//��ӡ�����ƶ���λ,��������,Ӱ���ӡһ�к����ֽ
	uint16_t	BMPBitNum;				  //1D763000ָ�����в��� 2016.08.04
	
	uint8_t		PCDriverMode;				//����PC��������ģʽ 0:û���� 1:����
	uint8_t		DriverOffset;				//����ʱ���õľ���λ��
	uint8_t		BitImageFlag;				//λͼ��ӡģʽ��־
	uint8_t		VariableBitImageFlag;	//λͼ��ӡģʽ��־,����ͨ���Ŵ������
	uint8_t		CharWidth;					//ASCII�ַ����,12 �� 9
	uint8_t		CharHigh;						//ASCII�ַ���	24��17
	uint8_t		LineHigh;						//һ��������ַ�
	uint8_t		MaxRatioX;					//һ����������Ŵ���
	uint8_t		MaxRatioY;					//һ�����������Ŵ���
	uint8_t		CodeAimMode;				//�ַ�����ģʽ,0 = �����, 1 = �м����; 2 = �Ҷ���
	uint8_t		LineDirection;			//�����ӡ��180�㣩
	uint8_t		CharNotFlag;				//�ַ����ԵĿ���,����Ч
	uint8_t		DoUnderlineFlag;		//���ӡ�»��߱�־	
	uint8_t		CharCycle90;				//�ַ���ת90�����,����Ч
	uint8_t		UnderlineMaxPrtHigh;	    //һ�б༭�����г��ֵ��»�����ߵĵ���,��Χ0~3,�ݴ�ȷ����ӡʱ��ȡֵ,
											//��ʼ��Ϊ0,�༭�ַ������ֹ�����ʵʱ���´�ֵ
	uint8_t		OneLineWidthFlag;		    //���ڱ����־
}TypeDef_StructPrtCtrl;

/********* ����ʵʱָ��ʹ�õ�ȫ�ֱ����ṹ *************/
typedef struct
{
	uint8_t		RealTimeEnable;			//�Ƿ�ʹ��ʵʱָ���,0��ֹ,1ʹ��
	uint8_t		RealTimeFlag; 			//ʵʱָ����Ч��־,����Ч
	uint8_t		RealTimeCntr;				//ʵʱָ�����ݼ�����
	uint8_t		RealTimeAllow;			//ʵʱָ��10 14 ��fn����ʹ�ܱ�־,Bit0��Ӧfn1,Bit2fn2,...
	uint8_t		RealTimeBuf[16];		//ʵʱָ���
}TypeDef_StructRealTime;

/********** �����û��Զ����ַ�ȫ�ֱ��� ***********/
typedef struct
{
	uint8_t		UserdefineFlag;		//�������Զ����ַ�,1B 26ָ��,BIT0=1��ʾ�������ַ�;
															//1C 32ָ��, BIT1=1��ʾ�����˺���
	
	uint8_t		UserCode_12[ USER_ZF_SIZE ];		//12��24�û������ַ������
	uint8_t		Buffer_12[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_A ]; //12*24�û������ַ��Ĵ洢BUFFER
	
	uint8_t		UserCode_09[ USER_ZF_SIZE ];		//9��17�û������ַ������
	uint8_t		Buffer_09[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_B ];	//9*17�û������ַ��Ĵ洢BUFFER
#ifdef	CPCL_CMD_ENABLE	
	uint8_t		UserCode_08[ USER_ZF_SIZE ];		//8��16�û������ַ������
	uint8_t		Buffer_08[ USER_ZF_SIZE * BYTES_OF_ASCII_FONT_C ];	//8*16�û������ַ��Ĵ洢BUFFER
#endif	
	uint8_t		UserCode_HZ[ USER_HZ_SIZE ];		//�����û������ַ������
	uint8_t		Buffer_HZ[ USER_HZ_SIZE * BYTES_OF_HZ_FONT_A ];	//�����û������ַ��Ĵ洢BUFFER
#ifdef	CPCL_CMD_ENABLE	
	uint8_t		UserCode_HZ16[ USER_HZ_SIZE ];		//�����û������ַ������
	uint8_t		Buffer_HZ16[ USER_HZ_SIZE * BYTES_OF_HZ_FONT_B ];	//16*16�����û������ַ��Ĵ洢BUFFER
#endif
}TypeDef_StructUserChar;

/********** �����û�����λͼȫ�ֱ��� ***********/
typedef struct
{
	uint8_t		BitImageFlag;							//����λͼ��־,=2��ʾ����������RAMλͼ
	uint8_t		BitImageData[ BMP_SIZE ];	//����λͼ������,ͷ�����ֽڱ�ʾλͼ�ĳ��ȺͿ��
}TypeDef_StructBitImage;


typedef struct							//�����ά����ṹ
{
	uint8_t	BarCode2DFlag;	                //��ӡ��ά�����־
	uint8_t	BarCode2DType;	                //��ά��������,0��ʾѡ��PDF417,1ѡ��Data Matrix,2ѡ��QR Code
    uint8_t	BarCode2DBuf[BARCODE2D_SIZE];	//��ά���뻺����
}TypeDef_StructBarCode2D;

/********** �����û�����λͼ���Զ����ַ����������� ***********/
typedef union							//λͼ���Զ����ַ�������
{
	TypeDef_StructBitImage	BitImageStruct;		//����λͼ�ṹ
	TypeDef_StructUserChar	UserCharStruct;		//�Զ����ַ�������
 	TypeDef_StructBarCode2D	BarCode2DStruct;	//��ά���뻺����  
}TypeDef_UnionRAMBuffer;

//�����ݺ궨��
#define	BYTES_OF_ALL_ELEMENT						(76)			//2016.06.16 80��Ϊ76,608��
#define	BYTES_OF_HEAT_ELEMENT						(72)
#define	BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT			(0)				//2016.06.16 4��Ϊ0 
#define	BYTES_OF_RIGHT_EMPTY_HEAT_ELEMENT			(4)

#define	MAX_PRT_DOT_HEIGHT			(MAX_RATIO_Y * MAX_FONT_HEIGHT)	//����ӡ�߶ȵ���,��󱶸ߡ�24��
#define	MAX_PRT_HEIGHT_BYTE			(MAX_PRT_DOT_HEIGHT / 8)				//����ӡ�߶��ֽ���,�粻��������Ҫ����
#define BYTES_OF_LINE_PRT_BUF		(PRT_DOT_NUMBERS * MAX_PRT_HEIGHT_BYTE)	//�д�ӡ��������С
#define BYTES_OF_LPB				 BYTES_OF_LINE_PRT_BUF

/******* ����һ���д�ӡ�������ṹ *************/
typedef struct
{
	uint16_t			ActiveDots;						        //��Ҫ���ȵ��� 2016.06.15
	uint8_t				Buf[BYTES_OF_ALL_ELEMENT];	  //���ݻ����� ����һ���ֽ���
} DotLineStruct;

/********** ����������ӡ������ ***********/
typedef	struct
{
	uint32_t	Size;										          //��������С	2016.06.15
	DotLineStruct	DdotLine[DrvPrtBufDeep];			//��������ַ	   8*24����  һ��Ϊ
}PrintBuf_Struct;


typedef	struct
{
	volatile uint32_t	IdxWR;				//д��ӡ��������/
	volatile uint32_t	BufHeight;		//���������ݵ�����											
	volatile uint32_t	IdxRD;				//����ӡ��������/
}PrintBufCtrl_Struct;


typedef struct
{
	uint16_t	PrtDataDotIndex;				//�м�������
	uint16_t	CodeLineHigh;						//�ַ�ģʽ�µ��и߿���,�������,Ӱ�����м��	
	uint16_t	PrtLeftLimit;						//��ӡ������߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
	uint16_t	PrtRightLimit;					//��ӡ�����ұ߾�,��ҳģʽ���ò�ͬ��ֵ,�Ժ����ƶ���λ����
	uint16_t	PrtWidth; 							//��ӡ���,ʵ��ʹ�õĴ�ӡ����
	uint16_t	PrtLength;							//�ܴ�ӡ���= ��߾� + ��ӡ���,������ߵĿհ׺͹涨�Ĵ�ӡ����
	uint16_t	MaxPrtDotHigh;					//�༭/��ӡ�����������Ի���Ϊ��׼����,���д�ӡ�����и߶���ߵĵ���ֵ
	
	uint8_t	PrtModeFlag;							//��ӡģʽ����,	0 = ��ͨ and 1 = ҳģʽ
	uint8_t	BitImageFlag;							//λͼ��ӡģʽ��־
	uint8_t	VariableBitImageFlag;			//�ɷŴ�λͼ��ӡģʽ��־
	uint8_t	MaxLineHigh;							//һ������ߵ��ַ�/λͼ�ߴ�,һ���ַ������ַ����+�ַ��Ҽ��
#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	uint8_t	MaxRatioX;								//һ����������Ŵ���
#endif
	uint8_t	MaxRatioY;								//һ�����������Ŵ���	
	uint8_t	CodeAimMode;							//���뷽ʽ,0=�����,1=�м����,2=�Ҷ���
	uint8_t	PrintDirection;						//��ӡ����
#ifdef	HEAT_TIME_DEC_CHAR_NOT_PRT_ENABLE
	uint8_t	CharNotFlag;							//�ַ����ԵĿ���,����Ч
#endif
#ifdef	EMPHASIZE_PRT_FEED_SPEED_DEC_ENABLE
	uint8_t	EmphaSizeMode;
#endif
	uint8_t	CharCycle90;							//��ת90�ȴ�ӡ��־,=1��ʾ��ת90�ȴ�ӡ,=0��ʾ����ת��ӡ
	uint8_t	CharHeightMagnification;	//�ַ�����Ŵ���
	uint8_t	HZHeightMagnification;		//��������Ŵ���
	uint8_t	PrintLineIndex;						//��ǰ��ӡ����������
	uint8_t	DoUnderlineFlag;					//�ַ��»��߿���,1=on; 0= off
	uint8_t	UnderlineMaxPrtHigh;			//һ�б༭�����г��ֵ��»�����ߵĵ���,��Χ0~3,�ݴ�ȷ����ӡʱ��ȡֵ	
	uint8_t	UnderlinePrintIndex;			//��־�»��ߵ�����������̵ı���,����ʱ��0�����;����ʱ����ߵ�0
	uint8_t	UnderLinePrtOverFlag;
}TypeDef_StructPrtMode;

#define	LINE_PRT_BUF_LEN		(BYTES_OF_LINE_PRT_BUF + PRT_DOT_NUMBERS)
typedef	struct
{
	__IO uint8_t	Status;											//�д�ӡ��������״̬
	TypeDef_StructPrtMode			PrtMode;	      //�д�ӡ���Ʋ���
	//���»��߻������ӡ���ݻ������ϲ�,���PRT_DOT_NUMBERS���ֽ���Ϊ�»���ʹ��
	uint8_t	Buf[LINE_PRT_BUF_LEN];
}TypeDef_StructLinePrtBuf;

//�����ӡ������������ �������ַ���ӡ������
typedef union							//
{
	TypeDef_StructLinePrtBuf	g_tLinePrtBuf;	  	//�ַ���ӡ������
	PrintBuf_Struct           DrivePrintBuf;	    //������ӡ������
}Union_PrtBuf;


//CPCLҳģʽ �ṹ����
#ifdef	CPCL_CMD_ENABLE
typedef struct
{
	uint8_t		FontType;						//�����
	uint8_t		SizeNumber;						//�����С
	uint8_t		Width;								//�ַ����ֿ�
	uint8_t		Height;								//�ַ����ָ�
	uint16_t	LeftSpace; 						//����
	uint16_t	RightSpace;						//�Ҽ��
 	uint8_t		WidthMagnification;		//����Ŵ���Ratio
	uint8_t		HeightMagnification;	//����Ŵ���Ratio
	uint8_t		EmphaSizeMode;				//�Ӵ�ģʽ
	uint8_t		Underline;						//�ַ��»��߿���,bit3 = on/off; bit1~0 = �»�����ʽ������˫�ߣ�
	uint16_t	RotateType;						//������ת�Ƕ����ã�0������ 1:90��2:180�� 3:270��
}TypeDef_StructHZCharPara;

typedef struct
{
	uint8_t * PageBuf;
}TypeDef_StructPageEditPrtBuf;
#endif

#define	 MODEL_ID 				0x20				//80ֽ��ϵ��
#define  WRITE_START       1
#define  WRITE_END   			 2

typedef struct
{
	uint16_t   SpeakCnt;
	uint16_t SpeakBuf;	//1D f0ָ����
	uint8_t SpeakBufFlg; //ʹ��1D F0 ָ���־
	uint8_t SpeakTempCntBuf; //ʱ�Ӽ���������ֹ������ַû�з�����ʱ���˳�ָ�����´�ָ���ȷ
  uint8_t	 SpeakFlag;
	uint8_t  SpeakEnable;
	uint8_t  SpeakCtrl;
	uint8_t  SpeakElecCtrl;	
	uint8_t  SpeakPEnable;
	uint8_t  SpeakDoorOpen;
	uint8_t  SpeakWrite;
	uint8_t  SpeakWriteHandle;
	uint32_t SpeakWriteTime;
}TypeDef_StructSpeaker;


#endif /* DEFCONSTANT_H */

