/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-3-27
  * @brief   ����ģʽ�����صĳ���.
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
#include	"button.h"
#include	"platen.h"
#include	"led.h"
#include	"mode.h"
#include	"timconfig.h"
#include	"defconstant.h"
#include  "extgvar.h"

extern	uint8_t	GetFeedButtonCount(void);
extern	void	InitKeyCtrlStructParameter(void);
extern  TypeDef_StructSysConfig		g_tSysConfig; 		//��flash�ж���ϵͳ����
extern   Maintenance_Counter R_Counter;

extern void PrtTestFun(void);		//2017.03.20
static void OpenDrawer(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ����ϵ����ģʽ�����ֽ�ָǡ���ֽ�����,�µĲ�������׼
	��������˵������feed��ʱ ÿ��һ�� paper����һ�Σ��ϸǼ��㰴���Ĵ�����
	
	��ӡ�Լ���������
	ֽ�ָǺ��ϣ���סfeed���ϵ磻�������ſ���

	��������ģʽ��
	�ϸǴ򿪲��Ұ�סfeed���ϵ��
	����ƻ���0.5S��1s���Ƶ��һֱ��˸�������м�¼��˸������
	�ɿ�feed���󣬴����Ϩ������˸��
	��feed����ȱֽ�ƻ������
	�ɿ�feed����ȱֽ��Ϩ��
	����¼��feed���İ���������
	����ֽ�ոǺ󣬳�����ݴ������˸������Feed���İ��´��������벻ͬ��ģʽ��

	����hexģʽ��
	���ϸǣ���סfeed���ϵ磬
	ֱ�������ȱֽ��ͬʱ����,0.5s��Ϩ�𣬣��ϵ�ָʾ���ò�������ģʽ��һ�����������ӣ�
	�������һ�κ�0.5S��1s���Ƶ��һֱ��˸����¼��˸��������
	�ɿ�������
	����ֽ�ոǣ�����hexģʽ������ӡ��Hexadeciaml dumping��
	�˳�Ϊ�رյ�Դ���߰�����feed����

	��������ģʽ��
	���ϸǣ���סfeed���ϵ磬
	�������˸��0.5S��1s���Ƶ��һֱ��˸����¼��˸��������������˸2�κ�
	�ɿ�feed����
	�ٰ�feed��2�Σ�
	�ر��ϸ����������ģʽ�� 
	����ģʽ�£�����feed�����л�������Ŀ����ӡ�������̰�feed����ѭ��ѡ����Ŀ�µľ������ֵ������ӡ������
	���ϸǣ���סfeed���������ϸǺ����ɿ�feed�����򱣴����õĲ���ֵ���˳�����ģʽ��
	����Ϊfeed������2s��û���ɿ����ӡ�л���������Ŀ��
	�̰�Ϊfeed������1S���ж��������ɿ���

	��������ģʽ��
	���ϸǣ���סfeed���ϵ磬
	�������˸��0.5S��1s���Ƶ��һֱ��˸����¼��˸��������������˸4�κ�
	�ɿ�feed�����ɿ�feed�������ȱֽ��Ϩ�𣩣�
	�ٰ�feed��4�Σ�
	�ر��ϸ����������ģʽ��
	����ģʽ�´����ȱֽ�ƽ�����˸��

	��ӡ��������ʼ��ģʽ��
	���ϸǣ���סfeed���ϵ磬
	�������˸��0.5S��1s���Ƶ��һֱ��˸����¼��˸��������������˸5�κ�
	�ɿ�feed�����ɿ�feed�������ȱֽ��Ϩ�𣩣�
	�ٰ�feed��5�Σ�
	�ر��ϸ����ӡ�������ָ�Ĭ��ֵ�� 

	���ٽ�������ģʽ��
	���ϸǣ���סfeed���ϵ磬
	�򿪵�Դ������ɿ���
	ȱֽ������4�Σ�
	���ϸǺ�����ģʽ��
	����ģʽ�´����ȱֽ�ƽ�����˸��

	��������ģʽ��
	���ϸǣ���סfeed���ϵ磬
	�򿪵�Դ������ɿ���
	ȱֽ������8�κ�
	���ϸ����������
* Input          : ��
* Output         : ���������ģʽ����
* Return         : ���ز�����
*******************************************************************************/
#define		ERROR_LED_OFF_MS						(500)
#define		ERROR_LED_ON_MS							(500)

#define		FEED_KEY_RAPID_OPEN_MS			(200)	//200ms֮���ɿ������ǿ����ɿ�

#define		HEX_MODE_ERROR_LED_FLASH_COUNT										(1)
#define		SET_PARAS_MODE_ERROR_LED_FLASH_COUNT							(2)
#define		PRT_TEST_LED_FLASH_COUNT													(3)			//��ӡ����ģʽ
#define		NORMAL_UPDATE_MODE_ERROR_LED_FLASH_COUNT					(4)
#define		RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT	(5)
#define		EnterUpGrade	                                    (6)     //���ӽ�������ģʽ 2017.05.23

#define		SET_PARAS_MODE_FEED_KEY_COUNT											(2)
#define		PRT_TEST_FEED_KEY_COUNT														(3)
#define		NORMAL_UPDATE_MODE_FEED_KEY_COUNT									(4)
#define		RESTORE_DEFAULT_PARAS_MODE_FEED_KEY_COUNT					(5)

#define		HEX_MODE_FEED_KEY_COUNT														(0)
#define		DIRECT_UPDATE_MODE_FEED_KEY_COUNT									(4)
#define		ERASE_APP_MODE_FEED_KEY_COUNT											(8)

uint8_t	GetModeType(void)
{
	uint8_t		PlatenStatus;					//ֽ�ָ�״̬
	uint8_t		FeedKeyStatus;				//����״̬
	uint8_t		ModeType;							//ģʽ
	uint8_t		ErrorLedFlashCount;		//����ָʾ����˸����
	uint32_t	TimeCount;						//msʱ�����
	
	PlatenStatus	 = ReadPlatenStatus();
	FeedKeyStatus	 = ReadFeedButtonStatus();
	
	//ֽ�ָǹرգ������ɿ�
	if ((PlatenStatus == PLATEN_STATUS_CLOSE) && \
			(FeedKeyStatus == FEED_BUTTON_STATUS_OPEN))
	{
		ModeType = MODE_NORMAL;					//����ģʽ
	}
	//ֽ�ָǹرգ���������
	else if ((PlatenStatus == PLATEN_STATUS_CLOSE) && \
					 (FeedKeyStatus == FEED_BUTTON_STATUS_CLOSE))
	{
		ModeType = MODE_SELFTEST_PRT;		//�Լ�ģʽ
	}
	//ֽ�ָǴ򿪣��������£�������ӣ�
	else if ((PlatenStatus == PLATEN_STATUS_OPEN) && 
					(FeedKeyStatus == FEED_BUTTON_STATUS_CLOSE))
	{
		//�����ɿ�������
		for (TimeCount = 0; TimeCount < 20; TimeCount++)
		{
			DelayMs(10);				//��ʱ�ټ����ֽ��
			if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_OPEN)
			{
				break;
			}
		}
		
		if (TimeCount < 20)		//200ms֮���ɿ���ֽ��,�����������ģʽ����������
		{
//			PaperEndLedOn();								//ȱֽ����
//			InitKeyCtrlStructParameter();		//����հ����ж�ȫ�ֱ���ֵ
//			while (PLATEN_STATUS_OPEN == ReadPlatenStatus());	//�ȴ�����ֽ�ָ�
//			
//			if (GetFeedButtonCount() == DIRECT_UPDATE_MODE_FEED_KEY_COUNT)
//			{	//ȱֽ�������ٰ�FEED��4�Σ�������ֽ�ָǣ������������ģʽ
//				ModeType = MODE_DIRECT_UPDATE;	//��������ģʽ
//			}
//			else if (GetFeedButtonCount() == ERASE_APP_MODE_FEED_KEY_COUNT)
//			{	//ȱֽ�������ٰ�FEED��8�Σ�������ֽ�ָǣ������������ģʽ
//				ModeType = MODE_ERASE_APP;			//��������ģʽ
//			}
//			else
//			{
//				ModeType = MODE_NORMAL;				//�������ֶ����ǣ������Ĭ��ģʽ
//			}
//			
//			PaperEndLedOff();								//ȱֽ�ƹر�
		}			//�����ɿ������ִ������
		else	//����Ϊ���ð��µ�����
		{
			//����������ֽ����ͳ�ƴ������˸����
			ErrorLedFlashCount = 0;
			while (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
			{	
				ErrorLedFlashCount++;
				ErrorLedOn();
				DelayMs(ERROR_LED_ON_MS);
				ErrorLedOff();
				DelayMs(ERROR_LED_OFF_MS);     

				if(ErrorLedFlashCount == EnterUpGrade)   //2017.05.23  err����6�ν�������ģʽ
				{
					ClearComInBufBusy();
					Update();
					return;
				}	
			}
			
			//��ͳ�ƺ���ֽ�ָ�ǰ��ֽ�����µĴ���
			InitKeyCtrlStructParameter();		//����հ����ж�ȫ�ֱ���ֵ
			while (PLATEN_STATUS_OPEN == ReadPlatenStatus())	//�ȴ�����ֽ�ָ�
			{
				if (ReadFeedButtonStatus() == FEED_BUTTON_STATUS_CLOSE)
				{
					PaperEndLedOn();						//ȱֽ����
				}
				else
				{
					PaperEndLedOff();						//ȱֽ�ƹر�
				}
			}
			
			//���ݴ������˸�����ͺ���ֽ�ָ�ǰ��ֽ�����µĴ����ж�ģʽ
			if ((ErrorLedFlashCount   == HEX_MODE_ERROR_LED_FLASH_COUNT) && \
					(GetFeedButtonCount() == HEX_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_HEX_PRT;		//ʮ�����ƴ�ӡ
			}
			else if ((ErrorLedFlashCount   == SET_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == SET_PARAS_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_SET_PARAS;	//���ò���ģʽ
			}
			else if ((ErrorLedFlashCount   == NORMAL_UPDATE_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == NORMAL_UPDATE_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_NORMAL_UPDATE;	//��������ģʽ
			}
			else if ((ErrorLedFlashCount   == PRT_TEST_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == PRT_TEST_FEED_KEY_COUNT))
			{
				ModeType = MODE_PRINT_TEST;	        //��ӡ����ģʽ
			}
			else if ((ErrorLedFlashCount   == RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == RESTORE_DEFAULT_PARAS_MODE_FEED_KEY_COUNT))
			{
				ModeType = MODE_RESTORE_DEFAULT_PARAS;	//�ָ�Ĭ�ϲ���ģʽ
			} 
			else if ((ErrorLedFlashCount   == RESTORE_DEFAULT_PARAS_MODE_ERROR_LED_FLASH_COUNT) && \
							 (GetFeedButtonCount() == PRT_TEST_FEED_KEY_COUNT))		//��5�ΰ����λָ�Ĭ���������
			{
				ModeType = MODE_RESTORE_NET_PARAS;	//�ָ�Ĭ�ϲ���ģʽ �ָ�Ĭ��������� 
			}  
			else
			{
				ModeType = MODE_NORMAL;		//�������ֶ����ǣ������Ĭ��ģʽ
			}
		}			//���ð��µ����ִ������
	}		//ֽ�ָǴ򿪣��������´������
	//ֽ�ָǴ򿪣������ɿ�
	else
	{
		ModeType = MODE_NORMAL;					//����ģʽ
	}
	
	return	(ModeType);	
}

/*******************************************************************************
* Function Name  : PrtTestMode
* Description    : ��ӡ����ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrtTestMode(void)
{
  uint32_t i,j,k;
  uint8_t Temp;
	
	if(g_tSysConfig.PrintMode != 0)	
	{		 	
		  GoDotLine(24);													//����һ�з�ֹ�ϸǺ�ѹ��
			if(g_tSysConfig.PrintMode == 1)   			//2���Ӽ�� ��ӡ����
      {    
          i = 1;					     //���ʱ�� 2017.10.09 1*60��Ϊ1
				  j = 1;							 //��ӡ���Դ���
				  PrintString("Test PrintMode\r\n" );	
      } 
      if(g_tSysConfig.PrintMode == 2)   			//5���Ӽ�� ��ӡ����
      {
          i =50*60;					 //���ʱ��
          j= 1;						   //��ӡ���Դ��� 
          PrintString("5 Minutes PrintMode\r\n" );		 
      } 
      if(g_tSysConfig.PrintMode == 3)   		//20���Ӽ�� ��ӡ����
      {
          i =200*60;					 //���ʱ��
          j= 1;						   //��ӡ���Դ���
          PrintString("20 Minutes PrintMode\r\n" );	
      } 
      if(g_tSysConfig.PrintMode == 4)  			//50Km
      {
          i =100;					 //���ʱ��
          j= 6;						 //��ӡ���Դ���
          PrintString( "50 Kilometres PrintMode\r\n" );				 
      }  
      if(g_tSysConfig.PrintMode == 5)				//�е�����
      {	
          PrintString( "CutPaper TestMode\r\n" );
          while(1)
          {
							uint8_t Buff[16];							
              if(R_Counter.CutCounter%10 ==0)
              {
									LongToStr(R_Counter.CutCounter, Buff);
                  PrintString( "��ӡ������ֽ������" );
                  PrintString( Buff );
                  PrintString( "��\r\n" );
              }
              GoDotLine((u32)(18/MIN_STEP_UNIT_Y) );		//��18mm
              DriveCutPaper(0);
              DelayMs(2*1000);						//ÿ����30�� 
         }
      }			
      while(1)
      {
					uint8_t Buff[16];
          if(g_tSysConfig.PrintMode == 4)  	//50Km ����
          {   				 	
							LongToStr((u32)(R_Counter.LineCounter /(1000 /MIN_STEP_UNIT_Y)), Buff);	 //��������λΪ���У�ת��Ϊ��
              PrintString( "��ӡ������ֽ��" );
              PrintString( Buff );
              PrintString( "��\r\n" );
          }
          if(g_tSysConfig.PrintMode == 2)  //TEST_20MINUTE
          { 
              PrintString( "��ӡ��������ʱ�䣺" );
              LongToStr(R_Counter.TimeCounter/60, Buff);	 //Сʱ
              PrintString( Buff );
              PrintString( "Сʱ" );
              LongToStr(R_Counter.TimeCounter%60, Buff);	 //����
              PrintString( Buff );
              PrintString( "����\r\n" );
          }				
          for(k=j; k>0; k--) 						        //��ӡk���Լ���
					{
              //SystemSelfTest();
						  //2017.03.20  ����ģʽ��ӡ�Լ����Ϊ��ӡ���Ƶ�����
						  PrtTestFun();
						  OpenDrawer();
          }   
          DelayMs( i *100);                      //2017.10.09 1000��Ϊ100
			}
	}
}

/*******************************************************************************
* Function Name  : PutStrToInbuf
* Description    : ��һ��������������������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PutStrToInbuf( const uint8_t *Str )
{    
	uint8_t	Char;
    
	while(*Str)
	{
        Char = *Str;
        PutCharToInBuffer(Char);	//��������
        Str++;
	}
}

static void OpenDrawer(void)
{

	uint16_t OpenTime;
		g_tDrawer1.Status = ENABLE;				  //Ϊ�˷�ֹ�ڽ���Ǯ�俪��ʱ���� ��ֹʵʱָ��101401n ��Ӱ��
		while (g_tDrawer1.ON_Time);					//�ȴ��ϴ�ִ�����
		
		g_tDrawer1.DrwID = 0 & 0x01;
		OpenTime		= (uint16_t)(((uint16_t)(32)) << 1);
		g_tDrawer1.OFF_Time	= (uint16_t)(((uint16_t)(32)) << 1);
		if (g_tDrawer1.OFF_Time < OpenTime)
		{
			g_tDrawer1.OFF_Time = OpenTime;
		}
		OpenTime += g_tDrawer1.OFF_Time;	//�ܿ���ʱ��
		if(OpenTime)
		{	
			DriveDrawer(g_tDrawer1.DrwID, ENABLE);			//��Ǯ��
		}	
		g_tDrawer1.ON_Time = OpenTime;
		g_tDrawer1.Status = DISABLE;								//�ڶ�ʱ����ʵ�ֶ�ʱ�͹ر�Ǯ��
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
