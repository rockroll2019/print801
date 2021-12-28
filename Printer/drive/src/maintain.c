/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-30
  * @brief   ��صĳ���.
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
#include	<string.h>
#include	"maintain.h"
#include	"spiflash.h"
#include	"extgvar.h"

#ifdef	MAINTAIN_ENABLE

extern	uint8_t	GetPrintBusyStatus(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static	uint8_t	m_bStoreIndex = 0;

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
* Function Name  : ������
* Description    : �õ��Ϸ��洢�����ĵ�ǰ��ŵ���һ�����,��0��ʼ,��127����.
	ͳ�ƴ�����ֵ���浽�ֿ�FLASH��.�㷨����:
	����Ϊ4K,һ����Ҫ�洢���ֽ���Ϊ32,���������Դ洢128�εĽ��.
	ÿ��intֵ�������ֽڱ���,�������׵�ַ��ʼ��4K�ֽڵ�һ����������.
	���׵�ַ��ʼ���α���ֵ�����0~8λ,��8~16λ,��16~24λ,��24~32λ.
	��ʼ��ʱ�����׵�ַ������ַ���ĸ��ֽ�ֵ�Ƿ�ȫΪ0xFF,�жϸ������Ƿ���Ҫ����.
	����ʱ���׵�ַ��ʼ,���α���.ֱ������ĸ��ֽ�Ҳ��ʹ��ʱ,���²���,�ٴ�ͷ����.

	���׵�ַ��ʼ�����ȡ32���ֽ�,ֱ������32���ֽ�ȫΪFF�ĵ�ַ����.
	����ֵΪ�Ϸ����ݵ���Ч��ʼ���,
	�����ʼ������ΪFF,�򷵻�0.
	�����ʼ32���ֽھ�Ϊ0xFF,����0;
	������32���ֽڲ�����FF,�򷵻�128.
	���������λ��,���ظ����洢��ʼ��ַ���ڱ�ŵ���һ�����.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
uint8_t	GetMaintainParaStoreIndex(void)
{

}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ָ���洢��Ŵ��ı���ֵ,int�洢ʱ��С�˷�ʽ
	��ֵ������ά�������ṹ��.
	���洢���ֵ����0ʱ��Ҫ�ȵݼ�1��.��Ϊ�ñ�����ζ��ȫΪFF�����к�.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/


/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ��ά������ֵ���洢���к�,��ֵ������ά�������ṹ��ͱ��ر���.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitMaintainParameter(void)
{
	m_bStoreIndex = GetMaintainParaStoreIndex();
}

/*******************************************************************************
* Function Name  : void	ReadFlashParameter(void)
* Description    : ���ֿ�Flash�ж�ά������������,
	����Ϊ4K,һ����Ҫ�洢���ֽ���Ϊ32,���������Դ洢128�εĽ��.
	������㷨��ɿ��Բ��Բ����洢�㷨��ͬ.
	ͬ�ڳ�ʼ�����ֵĲ���.
* Input          : ��
* Output         : �������Ч����,�򽫲�������ά��������������
* Return         : 
*******************************************************************************/
void ReadFlashParameter(void)
{
    uint32_t *p, *p1;
	uint8_t i;

	WriteFlashFlag =1;
	p= (u32*)TIMECOUNTADDR;

	//����û��д������ĵ�ַ
	while( (p <(u32*)(TIMECOUNTADDR +PAGE_SIZE)) && (*p != 0xffffffff))	//����BUG���ȱȽϵ�ַ���ٱȽ���ֵ	
	{
		p +=8;
	}

	//���оƬ�ǵ�һ�����У����ʼ����������
	if(p == (u32*)TIMECOUNTADDR)
	{
		R_Counter.LineCounter=0;
		R_Counter.HeatCounter=0;
		R_Counter.CutCounter =0;
		R_Counter.TimeCounter=0;
		C_Counter.LineCounter=0;
		C_Counter.HeatCounter=0;
		C_Counter.CutCounter =0;
		C_Counter.TimeCounter=0;

	}
	else
	{
		p1 = p;
		
		//У���ϴ�д��������Ƿ�������Ч
		while(p != (u32*)TIMECOUNTADDR)					
		{
			p -=8;
			for(i=0; i<8; i++)
			{
				if( *(p+i) ==0xffffffff)	   	//�����Ƿ���Ч 
					break;
			}
			if(i == 8)							//������Ч
				break;
		}
		
		R_Counter.LineCounter =*p++;
		R_Counter.HeatCounter =*p++;
		R_Counter.CutCounter  =*p++;
		R_Counter.TimeCounter =*p++;
		C_Counter.LineCounter =*p++;
		C_Counter.HeatCounter =*p++;
		C_Counter.CutCounter  =*p++;
		C_Counter.TimeCounter =*p++;

		//����ϴ�д���д����ҳβ�����ҳ
		if(p !=	p1 || p >= (u32*)(TIMECOUNTADDR +PAGE_SIZE)) 	
		{								//
			FLASH_Unlock();
			FLASH_ErasePage(TIMECOUNTADDR);
			FLASH_Lock();
			WriteFlashParameter(2);		//����ǰ���ݱ��棬��ֹ����д��ʱ����
		}
	}	
	R_CounterBK = R_Counter;
	WriteFlashFlag =0;
}

/*******************************************************************************
* Function Name  : void	WriteFlashParameter(uint8_t Flag)
* Description    : д�ֿ�Flash�е�ά��������
* Input          : Flag��0������ʱд,1�ػ�����ѹ��ʱд,2ָ��д
* Output         : 
* Return         : None
*******************************************************************************/
void	WriteFlashParameter(uint8_t Flag)
{
	uint8_t	cTemp1;
	uint8_t	cTemp2;
	uint8_t	cTemp3;
	
	union 				
	{
		Maintenance_Counter	Counter[2];
		u32 Buff[8];
	} ui;

    uint32_t i, j, Addr;
	
	//�ȹر�ʵʱָ��ִ��,�ر�ASB�ش�,д������ٿ���
	cTemp1 = g_tRealTimeCmd.RealTimeEnable;
	g_tRealTimeCmd.RealTimeEnable = 0;
	cTemp2 = g_tError.ASBAllowFlag;
	g_tError.ASBAllowFlag = 0;
	cTemp3 = g_tFeedButton.KeyEnable;
	g_tFeedButton.KeyEnable = 1;
	


	//ÿ30����дһ�λ��ѹʱ
	if((R_Counter.TimeCounter > R_CounterBK.TimeCounter + 30 ) || Flag)	
	{
	
		WriteFlashFlag =1;

	 	Addr = TIMECOUNTADDR;
		
		//����û��д��������ĵ�ַ
		while( Addr <(TIMECOUNTADDR +PAGE_SIZE) && (*((u32*)Addr) != 0xffffffff))	//����BUG���ȱȽϵ�ַ���ٱȽ���ֵ						
		{
			Addr +=32;
		}
		
		ui.Counter[0]=  R_Counter;		//���޸ĵ�
		ui.Counter[1]=  C_Counter;
	
		FLASH_Unlock();

		//����Ƕ�ʱ��ָ��д������Ƿ���ҳ�����һ�Σ����������ҳ
		//Ϊ�˱�֤д��ʱ�䣬����д��ʱ������в�����������ÿ��д��󣬱�֤���ٻ���д��һ��
		if(Flag !=1 && (Addr+32) == (TIMECOUNTADDR +PAGE_SIZE))	
		{
			FLASH_ErasePage(TIMECOUNTADDR);
			Addr = TIMECOUNTADDR;
		}

		//д��У�飬����ظ�д��3�λ��ǲ���ȷ�����ô���
		for(j=0; j<3; j++)		
		{
			for(i=0; i<8; i++)
			{
				FLASH_ProgramWord(Addr, ui.Buff[i]);
				if(( *((u32*)Addr) != ui.Buff[i]))
					break;
				Addr +=4;
			}
			if(Flag ==1 || i ==8 )		//����д��ʱ��У���Ƿ���ȷ�����ϵ�ʱУ��
				break;
			else
			{
				FLASH_ErasePage(TIMECOUNTADDR);
				Addr = TIMECOUNTADDR;
			}
		}
		FLASH_Lock();
		
		//���󱨾�
		if(j==3)
    {    
			g_tError.UR_ErrorFlag |= 0x20;		//bit5 CPU��Flashд�����
    }    
		else
    {    
			g_tError.UR_ErrorFlag &= ~0x20;
		}	
		R_CounterBK = R_Counter;
		WriteFlashFlag =0;
	}
	
	//�ȹر�ʵʱָ��ִ��,�ر�ASB�ش�,д������ٿ���
	g_tRealTimeCmd.RealTimeEnable = cTemp1;
	g_tError.ASBAllowFlag = cTemp2;
	g_tFeedButton.KeyEnable = cTemp3;
	
}

void	CheckMaintainRoutin(void)
{
	if (g_bMaintainRountinWriteFlag)
	{
		if (GetPrintBusyStatus() == PRINTER_STATUS_ILDE)
		{
			WriteFlashParameter(0);
			g_bMaintainRountinWriteFlag = 0;
		}
	}
}

#endif	/*����֧��ά���������궨�����*/

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
