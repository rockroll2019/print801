/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : GlobalVariable.h
* @author  Application Team  
* @version V0.0.1
* @date    2012-5-28
* @brief   ȫ�ֱ�������
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
//	ȫ��Ƭ�ڣ�64K�ֽڣ��ڴ���䣺
//		1.ָ����ջ��棺16K��16384�ֽڣ�,g_tPrtCtrlData.PrtCodeBuf[MAXCODELENGTH]
//
//		2.��׼��ӡ��������16K��16384�ֽڣ�,g_tPrtCtrlData.PrtDataBuf[PRTBUFLENGTH]
//		  	Ƭ�ڣ����֧�� 512 x 256 �㡣��Ŀǰ֧��384 x 256��,��ַ�¶���
//
//		3.�û��Զ����ַ���4K��4096�ֽڣ�����¼96�� 12 x 24 ������û��Զ����ַ�
//
//		4.����λͼ���棺 8K(8192 + 2�ֽ�)
//
//		5.�û�����꣺2K(2048�ֽ�)
//
//		6.�û������դ��2K(2048�ֽ�)
//
//		7.ҳģʽ��ӡ��������128K��Ƭ���ַ
//
#ifndef EXTGVAR_H
#define EXTGVAR_H

#include	"defconstant.h"
#include	"cominbuf.h"
#include	"bluetooth.h"
extern	TypeDef_StructSysConfig	g_ctSysDefaultValue;

/********** �궨��ȫ�ֱ��� ***********/
extern	TypeDef_StructMacro	g_tMacro;

/********** �û��Զ����ַ�ȫ�ֱ��� ***********/
extern TypeDef_UnionRAMBuffer	g_tRamBuffer;

/********** һά�������ȫ�ֱ��� ***********/
extern	TypeDef_StructBarCtrl	g_tBarCodeCtrl;

/********** �ַ��Ű����ȫ�ֱ��� ***********/
extern TypeDef_StructPrintMode	g_tPrtModeChar;
extern TypeDef_StructPrintMode	g_tPrtModeHZ;

/********** ��ӡ��״̬����ȫ�ֱ��� ***********/
extern TypeDef_StructStatus	g_tRealTimeStatus;

/********** ��ӡ���������ȫ�ֱ��� ***********/
extern TypeDef_StructError	g_tError;

/**********************************************************/
//	��ӡ��ϵͳ�������ò��֣�
extern	TypeDef_StructSysConfig			g_tSysConfig; 
extern	TypeDef_StructPageMode	 		g_tPageMode;
extern	TypeDef_StructKeyCtrl				g_tFeedButton;
extern	TypeDef_StructSystemCtrl		g_tSystemCtrl;
extern	TypeDef_StructPrtCtrl				g_tPrtCtrlData;
//extern	TypeDef_StructBarCode2D			g_tBarCode2D;			//��ά����ṹ
extern	TypeDef_StructDrawer  			g_tDrawer1,g_tDrawer2;				//Ǯ��ṹ
extern	TypeDef_StructRealTime			g_tRealTimeCmd;		//ʵʱָ��ṹ
extern	TypeDef_StructTab						g_tTab;						//����ṹ
extern	TypeDef_StructInterfaceType	g_tInterface;			//�ӿ����ͽṹ

extern	uint8_t	g_tLineEditBuf[ PRT_BUF_LENGTH ];			//��ӡ������
extern	uint8_t	g_tUnderlineBuf[ PRT_DOT_NUMBERS ];		//�»��߻�����
extern	uint8_t	g_bSleepModeFlag;					//�������߱�־
extern	volatile uint8_t	g_bRoutinFlag;				   	//T2�жϱ�־

extern	PrintBufCtrl_Struct U_PrtBufCtrl;	
extern	Union_PrtBuf U_PrtBuf;          //2016.06.16 ��ӡ������������

//g_StepCount��ֽ������ٱ���λ�ñ�־,��ʼ��ʱ����,
//�ڼ����и��ݼ�����ʱ�����AdjustMotorStep()������ֻ���������߲��䣬
//�� SystemTimer3Func()���ߵ��ʱ����,���ֹͣʱ���㡣
//ÿ����ӡ�л���GoDotLine()ʵ�������ļ��ٹ��̣�
//���Ǵ�ӡ���м���λ�ÿ�����ʱ���ݼ�����ʱ���������
extern	volatile uint16_t	g_StepCount;
extern	volatile uint16_t	g_HeatUs;				//����һ�ε�ʱ��

extern	uint8_t	g_bMotorRunFlag;					//���б�־
extern	uint8_t	g_bWriteFlashFlag;				//SPIFLASH�Ƿ��Ѿ�д���ı�־
extern	uint8_t	g_bRAMBitMapNumber;
extern	uint8_t	g_bFeedTaskFlag;		//��ֽ������ɱ�־

#ifdef	DBG_025
	extern	uint8_t	g_bDbg025EnalbeFlag;
	extern	uint8_t	g_bDbg025FontFlag;
#endif

#ifdef	MAINTAIN_ENABLE
	/******* ����ά��������*************/
// 	extern	TypeDef_StructMaintenanceCounter g_tResetableCounter;
// 	extern	TypeDef_StructMaintenanceCounter g_tCumulateCounter;
// 	extern	TypeDef_StructMaintenanceCounter g_tResetableCounterBakeup;
  extern	Maintenance_Counter R_Counter, C_Counter, R_CounterBK;
	extern	volatile uint8_t	g_bMaintainRountinWriteFlag;
#endif

//extern	uc16	MaxStep[];						//��ӡ�ٶ��趨ֵ
//extern	uc16	HeatTime[];  					//����ʱ�䣬��λΪuS
//extern	uint8_t	MotorMoveCntr;				  //�������
//extern	uint8_t	InterfaceType;		      //�ӿ����� 
//extern	uc16	MotorMoveSequence[8];	//���������������
//extern	const uint32_t BpsValue[8];
//extern	uint32_t	ExtRamNumber;						//�ⲿRAM����
//extern	TypeDef_StructKeyCtrl		g_tResetEthButton;	//��̫����������
//extern	uc16 	ShutTime[4];					//�Զ��ػ�ʱ��,��λΪ�룬��10���ӣ�20����30����
/******* �����ӡ������*************/
//extern	TypeDef_StructPrinter		PrintBuf;

/******* �������뻺�������� *************/
//extern	TypeDef_StructInBuffer	gstructInBuf;
//extern	uint32_t	giCodebarPositionSpace;	//��ʾ�����ӡλ�þ�����ص�ֵ.��λΪ�������.
//extern	uint8_t	CutType;					 			//�Ƿ��е���־��0���е���1���е�

#ifdef	CPCL_CMD_ENABLE
	extern TypeDef_StructHZCharPara	g_tPageModeCharPara;
	extern TypeDef_StructHZCharPara	g_tPageModeHZPara;
	extern TypeDef_StructPageEditPrtBuf	g_tPageEditPrtBuf;	//ҳ�༭��ӡ������;
#endif

extern	uint32_t ExtSRamSize;
extern  uint8_t LableEffectiveFlag;
extern  uint8_t FeedPaperFlag;
extern  uint8_t FeedToGapFlag;
extern  uint8_t AutoFeedPaper;
extern  uint8_t	*BootVerStr;
extern  uint8_t  ZIKUVerStr[5]; 
extern  uint8_t  MAX_RUSH_STEPS;
extern  uint8_t  WriteFlashFlag;
extern  volatile uint8_t  Command16RunFlag;
extern  volatile uint32_t	FeedStepCount; 		//2016.06.11
extern  volatile uint8_t  MotoFeedInc;
extern  uint8_t	SysModeType;
extern TypeDef_StructInBuffer volatile g_tInBuf;
//2016.07.10
extern volatile uint8_t T3_over;
extern volatile uint8_t SPI3_Over;
extern volatile uint8_t HeatOver;
extern uint8_t ManufacturerType;

//2016.08.20
extern uint8_t QR_version;
extern uint8_t PDF417_version;			     //�汾
extern uint8_t QR_vColl;
extern uint8_t PDF417_vColl;						 //����ȼ�
extern uint8_t PDF417_Rows;
extern uint8_t PDF417_ModuleWidth;
extern uint8_t PDF417_RowHeight;
extern uint8_t PDF417_options;
extern uint8_t PDF417_Rate;
extern uint8_t QR_model;
extern uint8_t QR_Rate;
extern uint16_t TwoCodeDataNum;

extern  uint8_t		BuzzerRunFlag;
extern  uint8_t		BuzzerNum;
extern  uint16_t  BuzzerTime;
extern  uint8_t		DhcpGetFlag;
extern  uint8_t		VoiceOverflag;
extern  uint8_t		CutGoPaperflag;
extern TypeDef_StructSpeaker  g_tSpeaker;
extern Bluetooth_Struct    BluetoothStruct;		
extern  uint8_t BtOnStatus;
extern  USART_InitTypeDef   USART_BT_InitStructure;
extern  uint8_t		BtTimeOut;
extern  volatile  uint8_t		BtParaReloadFlag;

extern uint8_t Set_BTWFflg;
extern  uint32_t  NetReloadCounter;

#endif
