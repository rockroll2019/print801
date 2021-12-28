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
#include  "stm32f10x_conf.h"
#include	"update.h"
#include	"led.h"
#include	"cominbuf.h"
#include	"extgvar.h"
extern uint32_t __Vectors_End, __Vectors;  
extern TypeDef_StructInBuffer volatile g_tInBuf;

/* Private typedef -----------------------------------------------------------*/
typedef uint32_t (*pFunction)(uint8_t * Str, uint32_t Length);

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define BKP_DR_NUMBER 			10

/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t  JumpAddress;
uint16_t BKPDataReg[BKP_DR_NUMBER] = 
{
	BKP_DR1, BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5,
	BKP_DR6, BKP_DR7, BKP_DR8, BKP_DR9, BKP_DR10
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ������
* Description    : ����
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
/**
  * @brief  Writes data Backup DRx registers.
  * @param FirstBackupData: data to be written to Backup data registers.
  * @retval : None
  */
void	WriteToBackupReg(uint16_t FirstBackupData)
{
  uint32_t index;
	
  for (index = 0; index < BKP_DR_NUMBER; index++)
  {
    BKP_WriteBackupRegister(BKPDataReg[index], FirstBackupData + (index * 0x5A));
  }
}

uint8_t	CheckBackupReg(uint16_t FirstBackupData)
{
  uint8_t 	Str[10];
  uint16_t	Temp;
	uint32_t	index;
	
  for (index = 0; index < 5; index++)
  {
    Temp = (BKP_ReadBackupRegister(BKPDataReg[index]));
    Str[(index << 1)]			= Temp >> 8;
    Str[(index << 1) + 1]	= Temp & 0xFF;
  }
  if (memcmp(Str, "UPDATE_APP", 10) == 0)
  {
		return ((uint8_t)BKP_ReadBackupRegister(BKPDataReg[(index + 1)]));   // <<��Ϊ+1��ֹԽ��
  }
  else
  {
	  return (0xFF);
  }
}

/*******************************************************************************
* Function Name  : System_Upgrade
* Description    : ����ϵͳ������ʽ
* Input          : Interface:=0,=1,=2,=3
* Output         : None
* Return         : None
*******************************************************************************/
void System_Upgrade(uint8_t Interface)
{
	uint32_t	i;
	uc8	* Str = "UPDATE_APP";
	uint16_t	Temp;
	
  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);
  for (i = 0; i < 5; i++)
  {
  	Temp = Str[(i << 1)];
		Temp = Temp << 8 | Str[(i << 1) + 1];
		BKP_WriteBackupRegister(BKPDataReg[i], Temp);
  }
  BKP_WriteBackupRegister(BKPDataReg[(i + 1)], Interface);
	
  ErrorLedOn();
	PELedOn();
	NVIC_SystemReset();			  //����ϵͳ��λ
}

/*******************************************************************************
* Function Name  : Erase_Flash_Inside
* Description    : ���NVIC_VectTab_FLASH���ڵ�FLASH����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	Erase_Flash_Inside(void)
{
  FLASH_Unlock();
	FLASH_ErasePage(NVIC_VectTab_FLASH);
	FLASH_Lock();
  NVIC_SystemReset();			  //����ϵͳ��λ
}

void	Update(void)
{
	uint8_t	Interface_Type;
	
	//while (g_tInBuf.BytesNumber == 0);	//�ȴ����յ��ⲿ����  ����Ҫ���ⲿ����
	
	if (g_tSystemCtrl.PrtSourceIndex == LPT)				//��ǰ����ԴΪLPT
	{
		Interface_Type = LPT;
		//���������ӿ��ж�
	}
	else if (g_tSystemCtrl.PrtSourceIndex == COM)	//��ǰ����ԴΪCOM
	{
		Interface_Type = COM;
		//���������ӿ��ж�
	}
	else if (g_tSystemCtrl.PrtSourceIndex == USB)	//��ǰ����ԴΪUSB
	{
		Interface_Type = USB;
		//���������ӿ��ж�
	}
	else if (g_tSystemCtrl.PrtSourceIndex == ETHERNET)	//��ǰ����ԴΪETHERNET
	{
		Interface_Type = ETHERNET;
		//���������ӿ��ж�
	}
	
	System_Upgrade(Interface_Type);
}

/*******************************************************************************
* Function Name  : CheckBootLegality
* Description    : ���boot�ĺϷ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    CheckBootLegality(void)
{
	uint32_t i,checkSum =0,*p;
	uint8_t	*Str;

	Str =(uint8_t *) (NVIC_VectTab_FLASH+0x90);       //�����ַA
    
	p =(uint32_t*)(0x8000000 + 0x154);	             //boot�汾��ַ 0x13c 2016.07.13
  BootVerStr = (uint8_t *)(*p);	
	if(strncmp(BootVerStr,"WBV",3) != 0)
	{    
			BootVerStr = "WBV01";      
	}
	else
	{
			BootVerStr = BootVerStr+2;                //��ȡ�����ΪV01 ǰ���WBȥ��2016.07.13
	}  
    
	for(i=0; i<0x10; i++)
	{
		checkSum += Str[i];
	}
// 	if(FLASH_GetUserOptionByte()>>8 !=0x4159)	//AY  ����֤�˴�
// 	{
// 		while(1);								//��������
// 	}

	JumpAddress = *(__IO uint32_t*) (0x8000000 + 0x150);	  //CheckSumFun��ַ ָ��boot�����е���ͺ���
	Jump_To_Application = (pFunction)JumpAddress;		      	//ȡУ�������ڵ�ַ	
	if(Jump_To_Application(Str, i) != checkSum)	            //����ͺ���������ֵ��ַA�͸���i ʹ��boot�е���ͺ��������ۼӺͣ��Ƚ������������
	{
		while(1);								//У�����
	}

}

/******************************
  * @brief  ����Flashѡ���ֽ�,ʹ�ܶ�������IAP����ռ�д��������־��
  * @param  None
  * @retval : None
  *************************/
void ConfigOptionByte(void)        //2017.10.18
{
	uint32_t BlockNbr = 0, UserMemoryMask = 0;
	
  /* Get the number of block (4 or 2 pages) from where the user program will be loaded */
  BlockNbr = ((USER_FLASH_FIRST_PAGE_ADDRESS - 0x08000000) >> 12)-2;	//2020.01.09 ����BootLoad���4���������ܱ���������ά��������

  /* Compute the mask to test if the Flash memory, where the user program will be
     loaded, is write protected */
#ifdef STM32F10X_MD
  UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* STM32F10X_HD */
  
  if (BlockNbr < 62)
  {
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
  }
	
  else
  {
    UserMemoryMask = ((uint32_t)0x80000000);
  }
#endif /* STM32F10X_MD */

  //���û�����ö������������Զ����ã�
  if(!FLASH_GetReadOutProtectionStatus())	
  {
  	FLASH_Unlock();

	  FLASH_ReadOutProtection(ENABLE);				//���ö�������
	  FLASH_EnableWriteProtection(~UserMemoryMask);	//����IAP����ռ�Ϊд����
  	FLASH_Lock();
  	NVIC_SystemReset();			  					//����ϵͳ��λ
  }
}
/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
