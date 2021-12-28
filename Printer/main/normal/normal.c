/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   ������ӡ���ܵĳ���
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
#include	"normal.h"
#include	"reliable.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"
#include	"gvar.h"		//ȫ���ⲿ��������
#include	"extgvar.h"
#include	"apptype.h"
#include	"mode.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern	void LongToStr(uint32_t num, uint8_t *Str);
extern	void UpLoadData(volatile uint8_t * DataBuf, uint32_t Length);
extern  void PrintNetParameterReal(uint8_t cIndex);
void assert_failed(uint8_t * file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  uint8_t	bBuf[16];

	UpLoadData(file, strlen((const char *)file));
	LongToStr(line, bBuf);
	UpLoadData(bBuf, strlen((const char *)bBuf));
	
	/* Infinite loop */
  while (1)		
  {
		EndHeat();						//�ؼ���2016.06.04
		EndFeedMotor();				//����ֽ���
		ToggleErrorLed();
		DelayMs(1000);
  }
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ϵ�ʱϵͳ���ں��ֹ���ģʽ������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	ProcessModeType(void)
{	
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
	
	StopTimer(ROUTINE_TIMER);			//�ȹر�TIM2�������������ٿ���
	EnableReceiveInterrupt();     //�����ж� ��ֹ�رղ����ж϶԰���������Ӱ�죬���߹����жϺ���;
	SysModeType = GetModeType();			//�жϵ�ǰ��ģʽ
	InitKeyCtrlStructParameter();	//�����������
	StartTimer(ETH_DATA_TIMER);		//2016.07.07
	switch (SysModeType)							//����ϵ��Ĺ���ģʽ
	{
		case	MODE_NORMAL:					    //����ģʽ
			StartTimer(ROUTINE_TIMER);
			break;
		case	MODE_SELFTEST_PRT:		    //�Լ�ģʽ
			DelayUs(1000*500);
			StartTimer(ROUTINE_TIMER);
			SystemSelfTest();
			break;
		case	MODE_HEX_PRT:					    //16����ģʽ
			DelayUs(1000*500);
			StartTimer(ROUTINE_TIMER);
			ClearComInBufBusy();
			HexMode();
			break;
		case	MODE_SET_PARAS:				   //���ò���ģʽ
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_SET_PARAS\n");
			#endif
			//�������ò�������  ����wifi
				WIFI_ELINK_CLR;   //����WIFI����
				DelayMs(8000);
				WIFI_ELINK_SET;
				NVIC_SystemReset();			  				//��λ
			break;
		case	MODE_NORMAL_UPDATE:		   //��������ģʽ
			ClearComInBufBusy();
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_NORMAL_UPDATE\n");
			#endif
			Update();
			break;
		case	MODE_RESTORE_DEFAULT_PARAS:	//�ָ�Ĭ�ϲ���ģʽ
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n");
			#endif
			//����ָ�Ĭ�ϲ�������
			StartTimer(ROUTINE_TIMER);
      RestoreDefaultParas();
			ClearComInBufBusy();
      GoDotLine(20);		        //�ߵ���ֽλ��
      PrintString("Entered MODE_RESTORE_DEFAULT_PARAS\n");
      GoDotLine(CUT_OFFSET);		//�ߵ���ֽλ��
      while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 );	//�ȴ�ǰ���м������
      DelayMs(200);						  //��ʱ
			WriteFlashParameter(2);		        // ����ǰ���ݱ��棬��ֹ����д��ʱ���� 2016.05.31  
      NVIC_SystemReset();			  				//��λ
			break;
		case	MODE_PRINT_TEST:		           //��ӡ����ģʽ 2016.05.31
			DelayMs(1500);					           //2016.07.02
			StartTimer(ROUTINE_TIMER);
      PrtTestMode();
			break;
		case	MODE_RESTORE_NET_PARAS:		    //2016.07.14 �����ָ�Ĭ��IP����
			DelayMs(100);			
      g_tSysConfig.DHCPEnable = 0;			//2017.02.07  �ָ�Ĭ��ip �ر�DHCP	
      SysConfigUnionData.SysConfigStruct = g_tSysConfig;
	    WriteSPI_FlashParameter(SysConfigUnionData.bSysParaBuf, sizeof(g_tSysConfig));
      DelayMs(300);						          //��ʱ		
			InitNetworkAddr();		            //�ָ�IPΪĬ�ϲ���
		  DelayMs(1500);
			NVIC_SystemReset();			  				//��λ
			break;
		case	MODE_ERASE_APP:				        //��������ģʽ
			DelayUs(1000*500);
			#ifdef	DBG_MODE_SELECT
				PrintString("Entered MODE_ERASE_APP\n");
			#endif
			//�����������
			break;
		case	MODE_RELEASE_JTAG:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
			GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);	//SWJ,JTAGȫ������
			FLASH_Unlock();		/* Unlock the internal flash */
			break;
		default:
			break;
	}
	
	if (SysModeType == MODE_NORMAL)
	{
		DelayMs(10);    //��ǰ��Ķ�ʱ���ȳ�ʼ�����
		if( g_bMotorRunFlag == 0 && g_tError.DoorOpen ==0 				//���û������,�ϸ�û�� 
		&& (g_tError.PaperSensorStatusFlag & 0x01)==0	//û��ȱֽ
		&& (g_tError.AR_ErrorFlag | g_tError.R_ErrorFlag | g_tError.UR_ErrorFlag)==0)	//û�д���
		{
			if ((g_tSysConfig.BlackMarkEnable))	   //�Һڱ꿪����	�ϵ����Һڱ�	2016.08.04
			{		
// 				while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);  //2016.06.30			
// 				FindMarkPoint();   //ȥ�������Һڱ� 2016.11.17
			}
			else
			{	
				GoDotLine(FEED_STEPS_AFTER_CLOSE_SHELL);    //2016.05	�ϵ����ֽ24�� 3mm
			}	
		}	
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
#ifdef	VERSION_NORMAL
int main(void)
{
	Sys_Init();		//��ʼ��ʱ��,GPIO�˿�,ȫ�ֱ���,�ⲿ�豸
	
#ifdef WITHBOOT
	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif
	
	ProcessModeType();
	ClearComInBufBusy();
	
	while (1)
	{
		CheckFeedButton();				//���м�鰴���ȹ���
#ifdef	MAINTAIN_ENABLE
		CheckMaintainRoutin();		//���������ά�������洢
#endif
		ProcessCharMaskReadBuf();	//�����ַ������ȡ������
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
