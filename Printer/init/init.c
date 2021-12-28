/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-31
  * @brief   ��ʼ����صĳ���.
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
#include "init.h"
#include "reliable.h"
#include "usb_main.h"
#include "speaker.h"
#include "bluetooth.h"
#include "spi.h"
#include "sflash.h"
//���Ȼ�оһ�������еļ��ȼĴ�����Ԫ��
extern uint8_t	g_tbHeatElementBuf[BYTES_OF_ALL_ELEMENT];
extern TypeDef_StructInBuffer volatile g_tInBuf;
extern uint8_t	g_bLineEditBufStatus;
extern uc16	MaxStepNum[];


#ifdef	EXT_SRAM_ENABLE

#else
	uint8_t	bInBuf[MAX_BYTES_OF_IN_BUF];
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Static Funtion Declare ----------------------------------------------------*/
static void	InitCommonGPIO(void);
static void	InitStdPeriph(void);
static void	GetConfigData(void);
static void	InitBarCodeParameter(void);
static void	InitUserDefCodeParameter(void);
static void	InitPrintModeParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitRealTimeStructParameter(void);
static void	InitRealTimeStatusStructParameter(void);
static void	InitPrtCtrlDataParameter(void);
static void	InitTabStructParameter(void);
static void	InitPageModeStructParameter(void);
static void	InitSystemCtrlStructParameter(void);
static void	InitDrawerParameter(void);
static void	InitInbufParameter(void);
static void	InitParaOnlyTurnOn(void);
static void	InitInterfaceParameter(void);
static void	InitMacroStructParameter(void);
static void	CheckBT_WIFI(void);
#ifdef	CPCL_CMD_ENABLE
static void	InitPageModeCharPara(void);
static void	InitPageModeHZPara(void);
#endif
static void LEDPowerOn(void);
extern void uIPMain(void);
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ��,����ʱ��,GPIO�˿�,�ӿڼ��,����,�ж�,��ʱ��,ȫ�ֱ���,
*	ϵͳ����,�е�,��ֽ���,�ӿ�����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#define	DEBUG       //2016.06.18  st link������Ҫ��  ulink���ã� ����ʱ���ڶϵ㲻ֹͣ ��ӡ�쳣��

void	Sys_Init(void)
{
	SystemInit();					//��ʼ��ʱ��
	InitCommonGPIO();			//��ʼ��GPIO�˿�
	
	#ifdef DEBUG		 		  //���õ���ʱ��ʱ���ر�
	{
		u32 *p;
		p= (u32*)0xE0042004;
		*p= 0x00043800;
	}
  #endif
	
	sFLASH_Init();				//��ʼ���ֿ�SPI
  //EXT_SRAM_Init();      //�ⲿram�ĳ�ʼ��������
	InitParameter(0);			//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ����,��ʱ���д�ⲿSPI�ֿ�FLASH
	
	//���ͨ�Žӿ����Ͳ���ֵȫ�ֱ���,Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ
	InitStdPeriph();			//��ʼ��MCU�������ⲿ�豸,��������,ADC,SPI,EXTI,ETH,USB,TIMx
  CheckBT_WIFI();				//���ģ������  �����������ж�ǰ��ģ�����͸�ֵ���Ա���������ж����ȼ�
	InitNVIC();						//����ϵͳ����ʹ�õ��ж�����
	InitSysTick();				//2016.05.31 ��ʼ���δ�ʱ��
	LEDPowerOn();
	
	if(g_tSpeaker.SpeakCtrl == 1)
	{
		DelayMs(250);
		if(g_tSysConfig.SpeakerVolume == 0)
			Line_2A_WTN588H_Dly(SPEAKERLOW);
		else if(g_tSysConfig.SpeakerVolume == 1)
			Line_2A_WTN588H_Dly(SPEAKERMIDDLE);
		else if(g_tSysConfig.SpeakerVolume == 2)
			Line_2A_WTN588H_Dly(SPEAKERHIGHT);
		else if(g_tSysConfig.SpeakerVolume == SPEAKECLOSED)
			g_tSpeaker.SpeakCtrl = 0;
	}

	Init_BT();   //2017-10-09  �������
	
	CLI();                 //�ر����ж� 
	Init_WiFi();					//��ʼ��wifiģ��
	SEI();  
	
	StartTimer(ROUTINE_TIMER);	
	
	InitCutPaper();//WK	2016.11.24 ���е���ʼ���Ƶ�T2������ ��ֹ�е������û��T2���ܱ�����ʾ
	CutMotorDisable();	//оƬ����
	CutMotorSetMode();	//ɲ��״̬	
	
	SetComInBufBusy();					//�ȹر�ͨ�Ŷ˿�
	DisableReceiveInterrupt();
}

/*******************************************************************************
* Function Name  : InitParameter
* Description    : ���ù̶��Ĵ�ӡ��ʽ,��ʹ��Ĭ�ϵĴ�ӡ��ʽ��ӡ�Լ��ַ�
* Input          : initflag=1Ϊ��ʼ��ָ�1B 40��
* Output         : ��ʼ������ȫ�ֱ���
* Return		 : None
*******************************************************************************/
void	InitParameter(uint8_t initflag)
{
	GetConfigData();		//����ϵͳ����
	
	InitBarCodeParameter();						//�������ݽṹ��ʼ��
	InitUserDefCodeParameter();				//�û��Զ������ݽṹ
	InitPrintModeParameter();					//�ַ��ͺ����Ű����ݽṹ
	//InitErrorStructParameter();				//�������ݽṹ
	InitSystemCtrlStructParameter();	//ϵͳ�������ݽṹ
	InitKeyCtrlStructParameter();			//�������ݽṹ
	InitRealTimeStructParameter();		//ʵʱָ�����ݽṹ
	InitRealTimeStatusStructParameter();	//ʵʱ״̬���ݽṹ
	InitPrtCtrlDataParameter();				//��ӡ�������ݽṹ
	InitTabStructParameter();					//�������ݽṹ
	InitPageModeStructParameter();		//ҳģʽ���ݽṹ	
	
	InitCharMaskBufParameter();				//��ʼ���ֿ�����ȡ�������ṹ
	InitHeatElementBuf();							//��ʼ�����Ⱥ���ת�����������
	InitHeatCtrlStruct();							//��ʼ�����ȿ��ƽṹ
	
	memset(g_tLineEditBuf, 0x00, PRT_BUF_LENGTH);
	memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);
	g_bLineEditBufStatus = LEB_STATUS_ILDE;

	g_tError.PaperEndStopSensorSelect = 0x02;		//ѡ��ֽ������ֹͣ��ӡ,ȱֽһֱ��Ч
	g_tError.LPTPaperEndSensorSelect = 0x03;	  //ѡ��ֽ���������ȱֽ�ź�,ֽ��������ֽ���������κ�һ����⵽ȱֽ�����PE�ź�
	g_tSystemCtrl.BillPrtFlag = 0x01;           //Ʊ�ݴ�ӡ״̬�ֳ�ʼ״̬����ʾδ��ӡ������ 2016.10.25  02��Ϊ01
	g_tPrtCtrlData.BMPBitNum = 0;								//2016.08.04
	
	if (!(initflag))
	{
		InitLinePrtBuf();							  //��ʼ���д�ӡ������ 2016.06.21
		#ifdef	MAINTAIN_ENABLE
			ReadFlashParameter();				  //��Flash�ж�ȡά��������ֵ
		#endif
		InitInbufParameter();						//�ڳ�ʼ��ָ��ʱ��ʼ�����뻺��������
		InitMacroStructParameter();			//��ʼ����ṹ
		InitParaOnlyTurnOn();
	}
	
	#ifdef	CPCL_CMD_ENABLE
		InitPageModeCharPara();
		InitPageModeHZPara();
		
	#endif
}

/*******************************************************************************
* Function Name  : void	InitSysTick()
* Description    : ��ʼ���δ�ʱ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	InitSysTick(void)
{
		//��ʼ���δ�ʱ��
		/* Setup SysTick Timer for 1 msec interrupts  */
  	if (SysTick_Config(SystemFrequency / 1000))
  	{ 
    	/* Capture error */ 
    	while (1);
  	}
}
/*******************************************************************************
* Function Name  : void	InitNVIC()
* Description    : ����ϵͳ����ʹ�õ��ж�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	InitNVIC(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);			//�����������λ�ú�ƫ��,��������flash��,ƫ��0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�������ȼ����飺��ռ���ȼ��ʹ����ȼ�,��ռ���ȼ�2λ,�����ȼ�2λ
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//������ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0
	
	//----����DMA1��ͨ��1�ж�(ADת��)----//
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
	
	//������ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�1

	if (g_tInterface.COM_Type)
	{
		//----����USART�ж�----//
		NVIC_InitStructure.NVIC_IRQChannel = USART_IRQ_NUMBER;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);
	}
		if((g_tInterface.BT_Type) || (g_tInterface.WIFI_Type))
	{
		NVIC_InitStructure.NVIC_IRQChannel = BT_UART_IRQn;          //BT�жϵȼ� 2016.07.22
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_Init(&NVIC_InitStructure);
	}	
	//----�����ⲿ�жϣ����ڣ�----//
	if (g_tInterface.LPT_Type)
	{																																 //�����ԣ����ڵ����ȼ��� ��������ӡ�а׵��� ��Ҫ�Ѳ��ڵ����ȼ����������Ӳ����æ �������жϺͰ���ĿǰӲ��Ϊ����һ���ж����ȼ�
		NVIC_InitStructure.NVIC_IRQChannel = PARALLEL_STB_INT_IRQn;		 //�˳���Ͱ������õ��ж��ߣ�ʵ�����ȼ�Ϊ2-2 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	//----����TIM2�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//T2���ȼ���Ϊ1-3����T3T5�� ��ֹ�߲��������д��� T2���ܸ��¿���״̬
	NVIC_Init(&NVIC_InitStructure);
	
	//������ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	//----����TIM3�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		//���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	//----����TIM5�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;		//��ӡ���ݴ��� 2016.06.15
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);	

  #if	1
	//----����DMA2��ͨ��2�ж�(SPI3����)----//
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;	//�������ݷ��� 2016.06.21
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_Init(&NVIC_InitStructure);
  #endif	
	
	//������ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	/* Enable the Ethernet global Interrupt */   //2017.07.28 ���ڽ��������ն�
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
	
	
	//----����TIM4�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;		//��ӡ
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_Init(&NVIC_InitStructure);

	#if defined	(DBG_ADC) || defined	(DBG_DEAL_SPRT_DRV_ONE_LINE_TIME) || defined	(DBG_RX_TOTAL_THEN_PRT)
	//----����TIM5�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	#endif
	#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
	//----����TIM5�ж�----//
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	#endif
	
			
	#if defined	(FEED_BUTTON_CHECK_BY_INT) || defined (RESET_ETHERNET_BUTTON_CHECK_BY_INT)
	//----����EXTI�ж�,������ֽ,��̫����λ��ֽ�ָ�----//
	NVIC_InitStructure.NVIC_IRQChannel = FEED_BUTTON_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	#endif	
	
	if (g_tInterface.USB_Type)
	{
			//----USB���ݶ˵��ж�----//
		#ifdef USE_USB_OTG_HS   
			NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
		#else
			NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
		#endif

			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);  
	}
	
  //�δ�ʱ�����ȼ����Ϊ15���ȼ� ��Ϊ3-3 ;Ϊ�˳�ͻ 3-3�ڴ˴����������ȼ�;

}

void	InitErrorStructParameter(void)
{
	g_tError.PaperSensorStatusFlag = 0;		//ֽ������״̬��־,λ����,����Ч,bit.0=ȱֽ��bit.1= ֽ����
	g_tError.AR_ErrorFlag = 0;			//���Զ��ָ������־,λ����,����Ч,bit.0=��ӡͷ���ȣ�bit.1=�ϸ�
	g_tError.R_ErrorFlag = 0;				//�ɻָ������־,λ����,����Ч,bit.0=�е�λ�ô���bit.1=��ӡͷԭλ����ֻ��9��ʽ�У�
	g_tError.UR_ErrorFlag = 0;			//���ɻָ������־,λ����,����Ч,bit.0=�ڴ��д����bit.1=����ѹ��bit.2=Ƿ��ѹ��bit.3=CPUִ�д���bit.4=UIB����EPSON����bit.7=�¶ȼ���·����
	g_tError.R_ErrorRecoversFlag = 0;	//�˳��ɻָ������־
}

void	InitKeyCtrlStructParameter(void)
{
	//�������ݽṹ
	g_tFeedButton.KeyEnable = 0;	//0 = ������, 1= ��ֹ����
	g_tFeedButton.KeyStatus = 0;	//������Ч��־,����Ч
	g_tFeedButton.KeyCount = 0;		//FEED������
}

uint8_t	GetFeedButtonCount(void)
{
	return	(g_tFeedButton.KeyCount);
}

/*******************************************************************************
*	���º���Ϊ���غ���,ֻ�������й��õĶ˿ڳ�ʼ������.
*	����:��Դ����,��о����,Ǯ��,�е�,��ֽ,������,ָʾ��,ֽ�ָ�,��ֽ����,�˿ڼ��
*******************************************************************************/
static void	InitCommonGPIO(void)
{
	GPIO_AFIODeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	#ifdef	WITHBOOT
		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);		//�ر�JTAG/SWJ����
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	#else
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//�ر�JTAG����
	#endif
	
	InitPowerCtrlPort();
	InitHeatPort();
	InitCutPort();
	InitFeedPort();
	InitBeepPort();
	PELedPortInit();
	ErrorLedPortInit();
	InitPlatenPort();
	InitFeedButtonPort();
	InitDrawerPort();
	InitSpeakerPort();
// 	CheckBT_WIF();
}

//SRAM�����úͳ�ʼ��
static void	EXT_SRAM_Init(void)
{	
    InitExtSRAM();   
    
    #ifndef	DBG_EXT_SRAM
        Test_Ext_Sram();           //�ⲿram�Ĳ��Ե�
    #endif
}


static void	Test_Ext_Sram(void)
{	
    uint8_t	    Result;
	
    Result = 0;
    DelayMs(100);
				
    ExtSRamSize = GetExtSRAMLen();          //���Եõ�����sram��С
    if(ExtSRamSize)
    {    
        Result = TestReadWriteExtSRAM();
	
    }
    if (Result == 0)
    {
        g_tError.UR_ErrorFlag &= ~0x01;   	//bit.0 = 0
    }
    else if (Result == 1)
    {
        g_tError.UR_ErrorFlag |= 0x01;   	//bit.0 = 1, ��չSRAM��,����Ч 
    }	
    
	DelayMs(100); 
}

static void	InitStdPeriph(void)
{
	InitADCPort();
	ADC_DMA_Config();				//DMA������ǰ,ADC�����ں�
	ADC_Config();						//AD����
	
	sHeat_Init();						//��ʼ����оSPI,ע���оSPI��ʼ��Ҫ�ŵ���оDMA��ʼ��֮ǰ,�������
	sHeat_DMAInit();				//���û�оSPI��������MDA
	
	CheckInterface();			  //���ͨ�Žӿ����Ͳ���ֵȫ�ֱ���,Ӧ�÷����ڱ�����ʼ��֮��,�˿ڳ�ʼ��֮ǰ 2016.07.12

	if (g_tInterface.LPT_Type)	//��������
	{
		InitParallelDataPort();
		InitParallelCtrlPort();
	}
	
	if (g_tInterface.COM_Type)	//�첽��������
	{
		ConfigUsart();
		InitUsartPort();
	}

				
	if (g_tInterface.USB_Type)	//��ʼ��USB�ӿ� 2016.09.14 
	{
    USB_Port_Init();				  //��ʼ��USB��IO��USB_DISCONNECT_PIN
		USB_Main();
		DelayUs(1000000);		      //��ʱ10ms
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);		//2016.07.21  ��ʼ����USB�� ����D+��������
	}
	
	if(g_tInterface.ETH_Type)  //��ʼ��ETH�ӿ�
	{							
		uIPMain();
	}
	//����T2Ϊ����AD��ʱ��,��ʱ100uS,����������ADCת��,һֱ����,��������ǰ�ر�,�˳�����ǰ����
	TimerConfigPrescaler(ROUTINE_TIMER, 10000);
	SetTimerCounterWithAutoReload(ROUTINE_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);
	
	//����T3Ϊ���ȶ�ʱ��,Ԥ��ƵΪ1000000,1uS,��������ֽ����ͼ���,��Ҫ���Ȼ���
	//�ж�������ֽ���ʱ�ɳ�������,���ж��м���Ƿ������,�����������Լ��ر�
	TimerConfigPrescaler(FEED_HEAT_TIMER, 1000000);
	SetTimerCounterWithAutoReload(FEED_HEAT_TIMER, 1);
	
	//����T4Ϊϵͳʱ��,100uS,�����д�ӡ�������Ĵ�ӡ,һֱ����,�����Լ��������Ϣ ����1ms
	TimerConfigPrescaler(ETH_DATA_TIMER, 10000);
	SetTimerCounterWithAutoReload(ETH_DATA_TIMER, ROUTINE_NORMAL_INTERVAL_MS * 10);  //2016.07.07
		
	#if defined	DBG_ADC || defined	DBG_DEAL_SPRT_DRV_ONE_LINE_TIME || defined	DBG_RX_TOTAL_THEN_PRT
		//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,1uS,����
		TimerConfigPrescaler(BACKUP_TIMER, 1000000);
	#endif
	
	#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
		//����T5Ϊ���ö�ʱ��,Ԥ��ƵΪ1000000,100uS,�ɿ��Բ���ʹ��
		TimerConfigPrescaler(BACKUP_TIMER, 10000);
	#endif
	
	//����T5Ϊus��ʱ��,Ԥ��ƵΪ1000000,1uS, 2016.06.14 �����ͻ�оǰ���ݴ���  ��֤��������ʱ���������жϸ�����ɵ�������ӡʱ����ʱ��������ֹͣ����ӡ��������
	TimerConfigPrescaler(DealPrtData_TIMER, 1000000);	//1000000Ϊ1us 2016.06.16 ��Ϊ1us
	
	
// 	#ifdef	DBG_PROCESS_TIME
// 		InitSystick();
// 	#endif
}
/*******************************************************************************
* Function Name  : void CheckBT_WIFI(void)
* Description    : �������ģ������	 ��ģ���Ƿ����            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void	CheckBT_WIFI(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t Temp;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		GPIO_InitStructure.GPIO_Pin		=	BT_WIFI_TYPE;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);	
		Temp = GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   			//��������
		GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);
		DelayUs(40);
		if(Temp != GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE))     //�����Ϊ���ڣ��ⲿ���� û������ģ�� 
		{
	    g_tInterface.BT_Type  = 0;
			g_tInterface.WIFI_Type  = 0;
		}
		else  //�н�����������
		{	
			GPIO_InitStructure.GPIO_Pin		=	BT_WIFI_TYPE;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  		
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
			GPIO_Init(BT_TYPE_PORT, &GPIO_InitStructure);
			Temp = GPIO_ReadInputDataBit(BT_TYPE_PORT, BT_WIFI_TYPE);
			if(Temp)										//2016.09.12 ģ����BT_wifi ��������ΪBTģ�� ����ΪWIFIģ�� 
			{
				g_tInterface.BT_Type  = 1;
			}	
			else
			{
				g_tInterface.WIFI_Type  = 1;
			}		
		}		
		if(g_tInterface.BT_Type  == 1)
		{	
			InitBluetoothPort();					                       //��ʼ��������wifi����
			BT_RST_HIGH;									//��λ����ģ��
			BT_BUSY_CLR;				          //������ģ��ӿ�æ 
		}	
		else if(g_tInterface.WIFI_Type  == 1)
		{	
			WIFI_Config_Gpio();
		}
			
// 		Judge_BT_Type();						  //��������

}
/*******************************************************************************
* Function Name  : void	GetConfigData(void)
* Description    : �Ӵ���Flash�ж�ϵͳ����,��û��ϵͳ����,��д��Ĭ�ϵ�ϵͳ����
* Input          : ��
* Output         : ��ʼ��ϵͳ�������ݽṹ
* Return         : None
*******************************************************************************/
static void	GetConfigData(void)
{
	TypeDef_UnionSysConfig	SysConfigUnionData;		//ϵͳ����	
	uint16_t Len;
	uint8_t	i;
	volatile FLASH_Status FLASHStatus;
    
	Len = sizeof(g_tSysConfig);
  BluetoothStruct.reInitFlag = 0;      
  Flash_Inside_ReadByte(SysConfigUnionData.bSysParaBuf,FLASH_INSIDE_PARA_ADDR, Len);   
    //�鿴�ڲ�flash�����ĺϷ���
	if( strcmp((char *)SysConfigUnionData.SysConfigStruct.ConfigFlag, PARA_CHECK_STRING) != 0 )	
  {							

		memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
		g_tSysConfig = SysConfigUnionData.SysConfigStruct;
		FLASH_Unlock();    
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
		FLASHStatus = FLASH_ErasePage(FLASH_INSIDE_PARA_ADDR); 
		if (FLASHStatus == FLASH_COMPLETE)                      
		{   
				FLASHStatus = Flash_Inside_BufWrite(SysConfigUnionData.bSysParaBuf, FLASH_INSIDE_PARA_ADDR, Len);
		}
		FLASH_Lock();  
		//�ڲ����������������ڲ����ⲿ������      
		memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
		WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
		BluetoothStruct.reInitFlag = 1;       //���³�ʼ��������־
  }
  else
  {           
		for(i=0; i<8; i++)
		{
				sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
				if ((strcmp((const char *)SysConfigUnionData.SysConfigStruct.ConfigFlag, PARA_CHECK_STRING)) == 0)
				{    
						break;
				}    
				DelayUs(8000);				//��ʱ10mm
		}

		if(8 ==i)		//2012.12.10
		{
				memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
				WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
		}
  }
	g_tSysConfig = SysConfigUnionData.SysConfigStruct;
    
	sFLASH_ReadBuffer(ZIKUVerStr, ZK_Ver_ADDR, 4);
	if(ZIKUVerStr[0] == 0xFF)   
	{
			strcpy(ZIKUVerStr,"0.01");
	}    
	else
	{    
			ZIKUVerStr[4] = 0;
	}    
}

void    RestoreDefaultParas(void)   //�ָ�Ĭ�ϲ���
{
	TypeDef_UnionSysConfig	SysConfigUnionData;		//ϵͳ����	
	uint16_t Len;
	volatile FLASH_Status FLASHStatus;
    
    Len = sizeof(g_tSysConfig);
    
    //�ڲ��������ָ�Ĭ�ϲ���
    memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
    g_tSysConfig = SysConfigUnionData.SysConfigStruct;
    FLASH_Unlock();    
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);  
    FLASHStatus = FLASH_ErasePage(FLASH_INSIDE_PARA_ADDR); 
    if (FLASHStatus == FLASH_COMPLETE)                      
    {   
        FLASHStatus = Flash_Inside_BufWrite(SysConfigUnionData.bSysParaBuf, FLASH_INSIDE_PARA_ADDR, Len);
    }
    FLASH_Lock();  
    //�ⲿ������ �ָ�Ĭ�ϲ�   
    memcpy(SysConfigUnionData.bSysParaBuf, &g_ctSysDefaultValue, Len);
    WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, Len);
     
    g_tSysConfig = SysConfigUnionData.SysConfigStruct;
}


static void	InitBarCodeParameter(void)
{
	g_tBarCodeCtrl.HRIPosition = 0;				//Ĭ��HRI��ӡλ��,����ӡ
	g_tBarCodeCtrl.HRIFont = ASCII_FONT_DEFAULT;	//Ĭ��HRI����,12��24
	g_tBarCodeCtrl.BarCodeHigh = 60;			//Ĭ������߶�Ϊ60����
	g_tBarCodeCtrl.BarCodeThinWidth = 2;		//Ĭ������խ��ģ����Ϊ2 0.25mm
	g_tBarCodeCtrl.BarCodeThickWidth = 5;	//Ĭ���������ģ����Ϊ5 0.625mm
}

static void	InitUserDefCodeParameter(void)
{
	//�û��Զ������ݽṹ
	g_tSystemCtrl.UserSelectFlag = 0;			//�û��Զ���
	memset(g_tRamBuffer.UserCharStruct.UserCode_12, 0x0 ,USER_ZF_SIZE);	//��12��24�Զ����ַ�������
	memset(g_tRamBuffer.UserCharStruct.UserCode_09, 0x0 ,USER_ZF_SIZE);	//��9��17�Զ����ַ�������
	memset(g_tRamBuffer.UserCharStruct.UserCode_HZ, 0x0 ,USER_HZ_SIZE);	//���Զ��庺�ִ�����
#if	0	
	memset(g_tRamBuffer.UserCharStruct.UserCode_12, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_12) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_12[0]));	//��12��24�Զ����ַ�������
	memset(g_tRamBuffer.UserCharStruct.UserCode_09, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_09) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_09[0]));	//��9��17�Զ����ַ�������
	memset(g_tRamBuffer.UserCharStruct.UserCode_HZ, 0x0,\
		sizeof(g_tRamBuffer.UserCharStruct.UserCode_HZ) / sizeof(g_tRamBuffer.UserCharStruct.UserCode_HZ[0]));	//���Զ��庺�ִ�����
#endif
}

static void	InitPrintModeParameter(void)
{
	//�ַ��Ű����ݽṹ
	g_tPrtModeChar.HeightMagnification = 1;	//����Ŵ���Ratio
	g_tPrtModeChar.WidthMagnification = 1;	//����Ŵ���Ratio
	g_tPrtModeChar.EmphaSizeMode = 0;	//�Ӵ�ģʽ
	g_tPrtModeChar.Underline = 1;			//�ַ��»��߿���,bit3=on/off;bit1~0=�»�����ʽ������˫�ߣ�,Ĭ��1��
	g_tPrtModeChar.LeftSpace = 0;			//�ַ�����
	g_tPrtModeChar.RightSpace = 0;		//�ַ��Ҽ��
	
	//�����Ű����ݽṹ
	g_tPrtModeHZ.HeightMagnification = 1;	//����Ŵ���Ratio
	g_tPrtModeHZ.WidthMagnification = 1;	//����Ŵ���Ratio
	g_tPrtModeHZ.EmphaSizeMode = 0;	//�Ӵ�ģʽ
	g_tPrtModeHZ.Underline	= 1;		//�ַ��»��߿���,bit3=on/off;bit1~0=�»�����ʽ������˫�ߣ�,Ĭ��1��
	g_tPrtModeHZ.LeftSpace	= 0;		//��������
	g_tPrtModeHZ.RightSpace	= 0;		//�����Ҽ��	
  g_tPrtModeHZ.CharWidth	= HZ_FONT_A_WIDTH;	//���ֿ��
	g_tPrtModeHZ.CharHigh		= HZ_FONT_A_HEIGHT;	//���ָ߶�
}

static void	InitSystemCtrlStructParameter(void)
{
	// ϵͳ�������ݽṹ
	g_tSystemCtrl.HexMode = 0;			//ʮ�����ƴ�ӡģʽ
	g_tSystemCtrl.SelfTestMode = 0;	//�Լ��ӡģʽ
	
#if	1
	//�����ַ�Ĭ������������ѡ���й�.���Ļ������ѡ��������,Ĭ��ֵΪ15,����Ϊ0.
	if (g_tSysConfig.SysLanguage == LANGUAGE_GB18030)
		g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_CHINESE;	//Ĭ������
	else
		g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_ENGLISH;	//Ĭ��Ӣ��
#else
	g_tSystemCtrl.CharacterSet = INTERNATIONAL_CHAR_SET_INDEX_ENGLISH;	//Ĭ��Ӣ��
#endif
	
	if (g_tSysConfig.BlackMarkEnable == 1)	//ȷ���ڱ�ʹ������, 0=��,1=��
	{	//ֽ����,0 = ����ֽ 1 = ���ֽ
		g_tSystemCtrl.PaperType = PAPER_TYPE_BLACKMARK;
	}
    else if (g_tSysConfig.BlackMarkEnable == 2)	//ȷ���ڱ�ʹ������, 0 ��ֽͨ 1�ڱ� 2��ǩֽ 2016.05
	{	//ֽ����,0 = ����ֽ 1 = ���ֽ 2��ǩֽ
		g_tSystemCtrl.PaperType = PAPER_TYPE_LABLE;
	}
	else
	{
		g_tSystemCtrl.PaperType = PAPER_TYPE_CONTINUOUS;
	}
    g_tSystemCtrl.CMDType = CMD_POS;       
    
	g_tSystemCtrl.PrtDeviceDisable = 0;				//��ӡ��������ֹ����,"1����ֹ
	g_tSystemCtrl.PrtModeFlag = STD_MODE;	//��ӡģʽ���� ,0 = ��ͨ and 1 = ҳģʽ
	g_tSystemCtrl.SleepCounter = 0;				//ϵͳ�������߼�����
	g_tSystemCtrl.ShutDownCounter = 0;		//ϵͳ����ػ���ʱ��
	
	if(g_tSysConfig.PaperWith == 1)				//2017.03.29
	{	
		g_tSystemCtrl.LineWidth = CUSTOM_57MM_PRT_DOT_NUMBERS;
	}	
	else
	{	
		g_tSystemCtrl.LineWidth = PRT_DOT_NUMBERS - g_tSysConfig.PrintWidthSelect * EPSON_DOTNUMBER;
	}
}

static void	InitRealTimeStructParameter(void)
{
	//ʵʱָ�����ݽṹ
	g_tRealTimeCmd.RealTimeEnable = 1;		//����ʵʱָ��ִ��
// 	g_tRealTimeCmd.RealTimeFlag = 0;			//ʵʱָ����Ч��־,����Ч
// 	g_tRealTimeCmd.RealTimeCntr = 0;			//ʵʱָ�����ݼ�����
	g_tRealTimeCmd.RealTimeAllow = 0xFF;	//����ʵʱָ���־��10 14 fn��
}

static void	InitRealTimeStatusStructParameter(void)
{
	g_tRealTimeStatus.PrinterStatus = 0x12;
	g_tRealTimeStatus.OffLineStatus = 0x12;
	g_tRealTimeStatus.ErrorStatus = 0x12;
	g_tRealTimeStatus.PaperStatus = 0x12;
}

static void	InitPrtCtrlDataParameter(void)
{
	//��ӡ�������ݽṹ
	g_tPrtCtrlData.PCDriverMode = DRV_PRT_TYPE_NONE;	//����PC��������ģʽ,=0Ϊ������ӡģʽ,=1ΪSP������ӡģʽ,=2ΪWH������ӡģʽ,
	g_tPrtCtrlData.DriverOffset = 0;			//������ӡʱ�ľ���λ�� ��1B 24 n ��
	
	g_tPrtCtrlData.StepHoriUnit = DEFAULT_STEP_HORI_UNIT;	//��ӡ�����ƶ���λ,�������
	g_tPrtCtrlData.StepVerUnit = DEFAULT_STEP_VER_UNIT;		//��ӡ�����ƶ���λ,�������

	g_tPrtCtrlData.CodeLineHigh = DEFAULT_CODE_LINE_HEIGHT;		//�ַ�ģʽ�µ��и�,�������
	g_tPrtCtrlData.LineHigh = 0;
	if (g_tSysConfig.SysFontSize == ASCII_FONT_A)
	{
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;			//�ַ����
		g_tPrtModeChar.CharHigh	= ASCII_FONT_A_HEIGHT;		//�ַ���
	}
	else if (g_tSysConfig.SysFontSize == ASCII_FONT_B)
	{
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;			//�ַ����		
		g_tPrtModeChar.CharHigh	= ASCII_FONT_B_HEIGHT;		//�ַ���
	}
	g_tPrtCtrlData.CharWidth	= 0;
	g_tPrtCtrlData.CharHigh	= 0;
	
	g_tPrtCtrlData.PrtLeftLimit = 0;		//��׼ģʽ��߾�
	g_tPrtCtrlData.PrtWidth = g_tSystemCtrl.LineWidth;	//��ӡ���,ʵ��ʹ�õĴ�ӡ����,Ӳ������ʵ�ֵ����Ĵ�ӡ��Χ
	g_tPrtCtrlData.PrtRightLimit = g_tSystemCtrl.LineWidth;	
	g_tPrtCtrlData.PrtLength = g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth;	//��׼ģʽ���ô�ӡ����	
	if (g_tPrtCtrlData.PrtLength > g_tSystemCtrl.LineWidth)
	{
		g_tPrtCtrlData.PrtLength = g_tSystemCtrl.LineWidth;
	}
	
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;	//��׼ģʽ,�������
	
	g_tPrtCtrlData.CharCycle90 = 0;			//�ַ���ת90�����,����Ч
	g_tPrtCtrlData.CharNotFlag = 0;			//�ַ����ԵĿ���,����Ч
	g_tPrtCtrlData.LineDirection = g_tSysConfig.PrintDir;	  //�������ӡ	
	g_tPrtCtrlData.DoUnderlineFlag	= 0;	//�»��߱�־
	g_tPrtCtrlData.CodeAimMode = AIM_MODE_LEFT;	//�ַ�����ģʽ,0 = �����, 1 = �м���룻 2 = �Ҷ���	
	g_tPrtCtrlData.OneLineWidthFlag = 0;	//���ڱ����־
	#ifdef	DIFFERENT_MAGNIFICATION_PRINT_ADJUST_ENABLE
	g_tPrtCtrlData.MaxRatioX = 1;				//����������Ŵ���
	#endif
	g_tPrtCtrlData.MaxRatioY = 1;				//�����������Ŵ���
	g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;	
	g_tPrtCtrlData.MaxPrtDotHigh = 0;
	g_tPrtCtrlData.BitImageFlag = 0;			//��λͼ��ӡ��־
	g_tPrtCtrlData.VariableBitImageFlag = 0;	//��λͼ��ӡ��־
}

//�������ݽṹ
static void	InitTabStructParameter(void)
{
	uint32_t	i;
	
	//ϵͳĬ����������
	g_tTab.TabSetNumber = g_tSystemCtrl.LineWidth / DEFAULT_HT_WIDTH;
	if (g_tTab.TabSetNumber > MAX_HT_INDEX_NUM)	//���ܳ������������Ŀ
		g_tTab.TabSetNumber =	MAX_HT_INDEX_NUM;
	g_tTab.TabIndex = 0;
	for (i = 0; i < g_tTab.TabSetNumber; i++)	//Ĭ������λ��
		g_tTab.TabPositionIndex[i] = (i + 1) * DEFAULT_HT_WIDTH;
}

static void	InitPageModeStructParameter(void)
{
	//ҳģʽ���ݽṹ
	g_tPageMode.PagePrtOe = 0;			//ҳģʽ��,��ӡ�������
	g_tPageMode.PageNumber = 0;			//ҳģʽ�µ���ҳ������
	g_tPageMode.PageWritePoint = 0;		//ҳģʽ�µ�ҳ�����Ƽ�����
	g_tPageMode.PageReadPoint = 0;		//ҳģʽ�µ�ҳ��ӡ������
	
	g_tPageMode.PageLineStartLoc = 0;	//��Ч��ʼ��ӡ��λ��,�̶�������λ��
	g_tPageMode.PageLineWidth = 0;		//��Ч��ӡ�е�����
	g_tPageMode.PageDataLineIndex = 0;	//��ӡ����,�м�����������һ���ַ���ӡ�Ĵ�ӡ����������һ�δ�ӡ24�У�
	g_tPageMode.PageDataDotIndex = 0;	//��ӡ����,�м�����������һ���д�ӡ��ӡ���������

	g_tPageMode.PageFeedDir = 0;		//ҳģʽ�µĴ�ӡ����,0 = ���ң�1 = �µ��ϣ� 2 = �ҵ��� 3 = �ϵ���
	g_tPageMode.PageArea[0] = 0;		//ҳģʽ�´�ӡ����,����ΪX0,Y0,DX,DY
	g_tPageMode.PageArea[1] = 0;
	g_tPageMode.PageArea[2] = g_tSystemCtrl.LineWidth;
	g_tPageMode.PageArea[3] = g_tSystemCtrl.LineWidth;
	g_tPageMode.PageVerLoc = 0;			//ҳģʽ�µ����λ��
	g_tPageMode.PageLeftLimit = 0;		//��ӡ������߾�,���׼ģʽ���ò�ͬ��ֵ
	g_tPageMode.PageRightLimit= 0;		//��ӡ�����ұ߾�,���׼ģʽ���ò�ͬ��ֵ
	g_tPageMode.AscRightSpace = 0;		//ҳģʽ��ASCII�ַ����Ҽ��	
	g_tPageMode.HZRightSpace = 0;		//ҳģʽ�º����ַ����Ҽ��
	g_tPageMode.HZLeftSpace = 0;			//ҳģʽ�º����ַ����Ҽ��
}

static void	InitDrawerParameter(void)
{
	//��ʼ��Ǯ�����
		g_tDrawer1.Status =DISABLE;
		g_tDrawer2.Status =DISABLE;
		g_tDrawer1.ON_Time	= 0;
		g_tDrawer2.ON_Time	= 0;
}

static void	InitInbufParameter(void)
{
	g_tInBuf.BytesNumber = 0;				//���ջ������ֽ���
	g_tInBuf.PutPosition = 0;				//���ջ�����
	g_tInBuf.GetPosition = 0;				//���ջ�����
	g_tInBuf.PE_BytesNumber = 0;		//���ջ������ֽ���
	g_tInBuf.PE_GetPosition = 0;		//���ջ�����
	g_tInBuf.BufFullFlag = 0;
	
	if(0 == ExtSRamSize)
  {
		g_tInBuf.Buf = bInBuf;                          //������sram
		g_tInBuf.BytesSize = MAX_BYTES_OF_IN_BUF;       //ram�Ĵ�С
		memset(g_tInBuf.Buf, 0x00, MAX_BYTES_OF_IN_BUF);
  }
  else
  {
		g_tInBuf.Buf = (uint8_t *)(ExtInBufBaseAddr);                 //����չsram  ��Ҫ���� 2016
		g_tInBuf.BytesSize =  (ExtSRamSize-PageModeBufNum);       //�ⲿ�����������Ĵ�С��
		memset(g_tInBuf.Buf, 0x00, sizeof(g_tInBuf.Buf) / sizeof(g_tInBuf.Buf[0]));
  }    
  
	//ע����Ϊ�˴��ⲿSRAM������δ���У����Բ��ܽ����ⲿSRAM������������ڲ�SRAM�Ŀ��ԡ�
	//memset(g_tInBuf.Buf, 0x00, MAX_BYTES_OF_IN_BUF);
	//memset(g_tInBuf.Buf, 0x00, sizeof(g_tInBuf.Buf) / sizeof(g_tInBuf.Buf[0]));
			
}


//2016.07.15 ֻ���ϵ�ʱ��ʼ���ı�������
static void	InitParaOnlyTurnOn(void)
{
	//2016.06.11  �ϵ��ʼ�� ����1B40����ʼ���ı������� ������
	g_tSystemCtrl.MaxStepPrtNum[0] = MaxStepNum[g_tSysConfig.MaxSpeed];		
	g_tSystemCtrl.MaxStepPrtNum[1] = MaxStepNum[g_tSysConfig.MaxSpeed];
	
	//ʵʱָ����� 2016.07.18
	g_tRealTimeCmd.RealTimeFlag = 0;			//ʵʱָ����Ч��־,����Ч 
	g_tRealTimeCmd.RealTimeCntr = 0;			//ʵʱָ�����ݼ�����	

	g_tSystemCtrl.PrtSourceIndex =ETHERNET;	
	
	InitDrawerParameter();						    //��ʼ��Ǯ�����2016.07.18
	InitErrorStructParameter();				    //�������ݽṹ
	
	if(memcmp(g_tSysConfig.Manufacturer,"EPSON",5)==0)
	{	
		ManufacturerType=1;
	}	
	else
	{	
		ManufacturerType=0;
	}
	
}

static void	InitMacroStructParameter(void)
{
	g_tMacro.Number = 0;					//���ַ�����
	g_tMacro.GetPointer = 0;			//��ȡ��ָ��
	g_tMacro.DefinitionFlag = 0;	//��������־
	g_tMacro.SetFlag = 0;				//�Ѷ����ı�־
	g_tMacro.RunFlag = 0;				//�����к����ݱ�־
	memset(g_tMacro.Buffer, 0x00, MACRO_SIZE);	//�����ݻ�����
	//memset(g_tMacro.Buffer, 0x00, sizeof(g_tMacro.Buffer) / sizeof(g_tMacro.Buffer[0]));
}

#ifdef	CPCL_CMD_ENABLE
	static void	InitPageModeCharPara(void)
	{
		g_tPageModeCharPara.FontType 						= 0;
		g_tPageModeCharPara.SizeNumber 					= 0;
		g_tPageModeCharPara.Width			 					= 12;
		g_tPageModeCharPara.Height							= 24;
		g_tPageModeCharPara.LeftSpace						= 0;
		g_tPageModeCharPara.RightSpace					= 0;
		g_tPageModeCharPara.WidthMagnification	= 1;
		g_tPageModeCharPara.HeightMagnification	= 1;
		g_tPageModeCharPara.EmphaSizeMode				= 0;
		g_tPageModeCharPara.Underline						= 0;
		g_tPageModeCharPara.RotateType					= 0;
	}
	static void	InitPageModeHZPara(void)
	{
		g_tPageModeCharPara.FontType 						= 0;
		g_tPageModeCharPara.SizeNumber 					= 0;
		g_tPageModeCharPara.Width			 					= 24;
		g_tPageModeCharPara.Height							= 24;
		g_tPageModeCharPara.LeftSpace						= 0;
		g_tPageModeCharPara.RightSpace					= 0;
		g_tPageModeCharPara.WidthMagnification	= 1;
		g_tPageModeCharPara.HeightMagnification	= 1;
		g_tPageModeCharPara.EmphaSizeMode				= 0;
		g_tPageModeCharPara.Underline						= 0;
		g_tPageModeCharPara.RotateType					= 0;
	}
#endif

#if	0
static void	InitInterfaceParameter(void)
{
	g_tInterface.LPT_Type = 0;
	g_tInterface.COM_Type = 0;
	g_tInterface.USB_Type = 0;
	g_tInterface.ETH_Type = 0;
}
#endif

//�������ϵ������ȱֽ��ͬʱ����0.5S��Ϩ��
//ָʾ���ٸ��ݴ�ӡ��״̬���߰���״̬�ڿ�ʼ��˸��
static void LEDPowerOn(void)
{
	PaperEndLedOn();
	ErrorLedOn();
	DelayMs(500);				
	PaperEndLedOff();
	ErrorLedOff();
	LineLedOn();			//Ĭ�ϵƴ�����
}


/******************************************************************************
*
* Function Name  : FLASH_BufferRead
* Description    : �ڲ�flash������ȡ
* Input          : - pBuffer : pointer to the buffer that receives the data 
read 
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_Inside_ReadByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	uint32_t	i;
	
	for (i = 0; i < NumByteToRead; i++)	
	{
		*pBuffer++ = *(vu8 *)(ReadAddr++);		
	}
}

/******************************************************************************
*
* Function Name  : Inner_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE 
sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to 
be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
FLASH_Status Flash_Inside_BufWrite(const u8* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{		
	uint16_t	* pData;
	volatile  FLASH_Status  FLASHStatus;
	uint32_t	i;
    
	pData = (uint16_t *)pBuffer;
	i = 0;
	FLASHStatus = FLASH_COMPLETE;
	
	while (i < NumByteToWrite)
	{
		FLASHStatus = FLASH_ProgramHalfWord(WriteAddr, *pData);
		if ((*(vu16*)WriteAddr) != *pData)
		{
  			FLASHStatus = FLASH_ERROR_PG;
			break;
		}
		WriteAddr += sizeof(u16);
		i += sizeof(u16);
		pData++;		
	}
	
	return (FLASHStatus);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
