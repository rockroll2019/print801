/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team
  * @version V0.0.1
  * @date    2012-12-24
  * @brief   С�����ƿɿ��Ժ��������Թ��ܵĳ���
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
#include	"reliable.h"
#include	"stm32f10x.h"
#include	"apptype.h"
#include	"mode.h"
#include	"normal.h"
#include	"init.h"
#include	"stm32f10x_it.h"
#include	"selftest.h"
#include	"hexmode.h"
#include	"update.h"

extern	void LongToStr(uint32_t num, uint8_t *Str);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
uint8_t	gcReliableRoutineFlag;				//��־�Ƿ���Կ�ʼһ�οɿ��Բ���
uint32_t	giReliableTimerCount;					//�ɿ��Բ���1ms��ʱ����

static	uint16_t	m_ReliableStoreIndex;	//��ʾ��ǰ�ɿ��Բ��Դ���ֵ�洢��ʼ��ַ����һ�����
static	uint32_t	m_iReliableTestNumber;	//��ִ�еĿɿ��Բ��ԵĴ���ֵ
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE)
/*******************************************************************************
* Function Name  : ������
* Description    : 
	2����,5����,20���ӵ�ÿ����ʱʱ���ӡһ���Լ�,ͳ���Ѵ�ӡ��������ӡ����,
	����洢�����򱣴浽�ֿ���.ÿ��2,5,20���Ӵ�ӡһ���Լ�.����ӡ���ܵ�ͳ��ʱ��,
	��ӡ����.�����ݱ��浽�ⲿFLASH��.ʹ��һ����ʱ������ʱ,���������н��ж�ʱ��
	�ĳ�ʼ��������,�ڶ�ʱ�����ж��Ƿ񵽴��趨ʱ��,������������ñ�־,�������м��
	��־,���ÿɿ��Գ������ɿɿ��Դ�ӡ���Լ�����,ͳ�ƴ���,����������ֿ�FLASH.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static	void	ReliabilityTestMinute(void)
{
	uint32_t	iTemp;
	
	PrintReliabilityTitle();		//��������,�ɿ��Բ���ʱ�Ŵ�ӡ
	
	#if !defined	DBG_QUICK_TEST_ENABLE
		SystemSelfTest();					//ִ��1���Լ�
	#endif
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;		//��ִ�еĿɿ��Բ��ԵĴ���ֵ
	
	#if defined	(TEST_2MINUTE)	//��ӡ�ɿ����Լ������Ϣ
		PrintString("2");
	#elif	defined	(TEST_5MINUTE)
		PrintString("5");
	#elif	defined	(TEST_20MINUTE)
		PrintString("20");
	#endif
	PrintString("����ѭ����ӡ�Լ�ɿ��Բ��Գ���");
	PrintString("\n");
	
	PrintReliabitityResultTotalNumber();		//��ӡ�ۼƲ��Դ���
	PrintReliabitityResultTotalTime();			//��ӡ�ۼƲ���ʱ��
	
	iTemp = m_iReliableTestNumber * (ONE_SELFTEST_LEN + ADDITIONAL_TITLE_LEN);
	PrintReliabitityResultTotalLen(iTemp);	//��ӡ�ۼƲ��Գ���
	
	WaitPrintAndFeedPaperFinish();
	ReliableTestSaveParameter();	//����Ϊ����洢����ֵ���ֿ�
}
#endif	//2,5,20���ӿɿ��Բ��Դ���������


#ifdef	TEST_50KM
/*******************************************************************************
* Function Name  : ������
* Description    : 
	ÿ��һ���趨��ʱʱ���ӡ�涨�������Լ�,ͳ���Ѵ�ӡ��������ӡ����,����洢������
	���浽�ֿ���.��ӡ���ܵ�ͳ��ʱ��,������õ�����ֽ����.
	�����ݱ��浽�ⲿFLASH��.ʹ��һ����ʱ������ʱ,���������н��ж�ʱ���ĳ�ʼ��������,
	�ڶ�ʱ�����ж��Ƿ񵽴��趨ʱ��,������������ñ�־,�������м���־,���ÿɿ���
	�������ɿɿ��Դ�ӡ���Լ�����,ͳ�ƴ���,����������ֿ�FLASH.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static	void	ReliabilityTestKm50(void)
{
	uint32_t	iTemp;
	
	PrintReliabilityTitle();	//��������,�ɿ��Բ���ʱ�Ŵ�ӡ
	for (iTemp = 0; iTemp < RELIABLE_TEST_50KM_SELFTEST_CYCLE; iTemp++)
	{
	#if !defined	DBG_QUICK_TEST_ENABLE
		SystemSelfTest();				//ִ�����ɴ��Լ�
	#endif
	}
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;		//��ִ�еĿɿ��Բ��ԵĴ���ֵ
	
	PrintString("50Km�ɿ��Բ��Գ���");
	PrintString("\n");	
	PrintReliabitityResultTotalNumber();		//��ӡ�ۼƲ��Դ���
	PrintReliabitityResultTotalTime();			//��ӡ�ۼƲ���ʱ��
		
	iTemp = m_iReliableTestNumber * \
		(RELIABLE_TEST_50KM_SELFTEST_CYCLE * ONE_SELFTEST_LEN + ADDITIONAL_TITLE_LEN);
	PrintReliabitityResultTotalLen(iTemp);	//��ӡ�ۼƲ��Գ���
	
	WaitPrintAndFeedPaperFinish();
	ReliableTestSaveParameter();	//����Ϊ����洢����ֵ���ֿ�
}
#endif	//50KM�ɿ��Բ��Դ���������


#ifdef	TEST_CUTTER_LIFE
/*******************************************************************************
* Function Name  : ������
* Description    : 
	ÿ��һ���趨��ʱʱ����ֽ����ֽ,ͳ������ֽ��������ӡ����,����洢�����򱣴浽��
	����.��ӡ���ܵ�ͳ�ƴ���,������õ�����ֽ����.
	�����ݱ��浽�ⲿFLASH��.ʹ��һ����ʱ������ʱ,���������н��ж�ʱ���ĳ�ʼ��������,
	�ڶ�ʱ�����ж��Ƿ񵽴��趨ʱ��,������������ñ�־,�������м���־,���ÿɿ���
	�������ɿɿ��Դ�ӡ���Լ�����,ͳ�ƴ���,����������ֿ�FLASH.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static	void	ReliabilityTestCutterLife(void)
{
	uint32_t	iTemp;
	
	#if !defined	DBG_QUICK_TEST_ENABLE
		GoDotLine(RELIABLE_TEST_FEED_PAPER_DOTLINE);	//����ֽ
		DriveCutPaper(CUT_TYPE_DEFAULT);							//����ֽ
		while ((g_tError.R_ErrorFlag & 0x01));			//����д���
	#endif
	//WaitPrintAndFeedPaperFinish();
	m_iReliableTestNumber++;			//���û�д���,��ִ�еĿɿ��Բ��ԵĴ���ֵ
	
	if (!(m_iReliableTestNumber % RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM))	//�ﵽԤ������
	{
		PrintReliabilityTitle();	//��������,�ɿ��Բ���ʱ�Ŵ�ӡ
		
		PrintReliabitityResultTotalNumber();		//��ӡ�ۼƲ��Դ���
		PrintReliabitityResultTotalTime();			//��ӡ�ۼƲ���ʱ��
		
		//������ֽ����,����Ϊ����ֽ��ÿ����ֽ���Զ���ֽ����ֵ
		iTemp = m_iReliableTestNumber * (RELIABLE_TEST_FEED_PAPER_LEN + FEED_PAPER_MM_AFTER_CUT);
		//�ټ���ÿ���̶��洢����ʱ��ӡ�����ݵĳ���
		iTemp += ((m_iReliableTestNumber / RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM) * ADDITIONAL_TITLE_LEN);
		PrintReliabitityResultTotalLen(iTemp);	//��ӡ�ۼƲ��Գ���
		
		WaitPrintAndFeedPaperFinish();
		ReliableTestSaveParameter();	//����Ϊ����洢����ֵ���ֿ�
	}
}
#endif	//�е��ɿ��Բ��Դ���������


#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
//�ɿ��Բ���ʱ�Ŵ�ӡ
static	void	PrintReliabilityTitle(void)
{
#ifdef TEST_2MINUTE						//2���Ӵ�ӡһ��
	PrintString("2 Minutes Test\n");	
#endif

#ifdef TEST_5MINUTE						//5���Ӵ�ӡһ��
	PrintString("5 Minutes Test\n");	
#endif

#ifdef TEST_20MINUTE					//20���Ӵ�ӡһ��
	PrintString("20 Minutes Test\n");
#endif

#ifdef TEST_50KM							//50�������
	PrintString("50Km Test\n");
#endif

#ifdef TEST_CUTTER_LIFE			//�е�����
	PrintString("Cutter Life Test\n");
#endif
}

static	void	PrintReliabitityResultTotalNumber(void)
{
	uint8_t	bBuf[16];
	
	//��ӡ����ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	LongToStr(m_iReliableTestNumber, bBuf);
	PrintString("�ۼƲ��Դ�����");
	PrintString(bBuf);					//��ӡ����
	PrintString("  \n");
}

static	void	PrintReliabitityResultTotalTime(void)
{
	uint8_t	cHour;
	uint8_t	cMinute;
	uint8_t	bBuf[16];
	uint16_t	iDay;
	uint32_t	iTime;
	
	iTime = m_iReliableTestNumber * RELIABLE_TEST_INTERVAL_S;
	
	iDay = iTime / SECOND_PER_DAY;					//����
	iTime -= (iDay * SECOND_PER_DAY);
	cHour = iTime / SECOND_PER_HOUR;				//Сʱ��
	iTime -= (cHour * SECOND_PER_HOUR);
	cMinute = iTime / SECOND_PER_MINUTE;		//������
	iTime -= (cMinute * SECOND_PER_MINUTE);	//ʣ������
	PrintString("�ۼƲ���ʱ�䣺");
	LongToStr(iDay, bBuf);			//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);					//��ӡ����
	PrintString(" �� ");
	LongToStr(cHour, bBuf);			//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);					//��ӡСʱ��
	PrintString(" Сʱ ");
	LongToStr(cMinute, bBuf);		//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);					//��ӡ������
	PrintString(" ���� ");
	LongToStr(iTime, bBuf);			//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);					//��ӡ����
	PrintString(" �� ");
	PrintString("\n");
}

static	void	PrintReliabitityResultTotalLen(uint32_t	iMicrometer)
{
	uint8_t	cKilometer;
	uint8_t	cCentimeter;	
	uint8_t	bBuf[16];
	uint16_t	Meter;
	
	cKilometer = iMicrometer / MM_PER_KM;			//Km
	iMicrometer -= (cKilometer * MM_PER_KM);
	Meter = iMicrometer / MM_PER_M;						//M
	iMicrometer -= (Meter * MM_PER_M);
	cCentimeter = iMicrometer / MM_PER_CM;		//Cm
	iMicrometer -= (cCentimeter * MM_PER_CM);	//ʣ��mm
	PrintString("�ۼƲ��Գ��ȣ�");
	LongToStr(cKilometer, bBuf);	//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);						//��ӡKm
	PrintString(" ���� ");
	LongToStr(Meter, bBuf);				//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);						//��ӡM
	PrintString(" �� ");
	LongToStr(cCentimeter, bBuf);	//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����	
	PrintString(bBuf);						//��ӡCm
	PrintString(" ���� ");
	LongToStr(iMicrometer, bBuf);	//ת��Ϊ��λ����ASCII��,����bBuf[0]�洢������߿�ʼ����
	PrintString(bBuf);						//��ӡmm
	PrintString(" ���� ");
	PrintString("\n");
}

static	void	ReliableTestSaveParameter(void)
{
	uint8_t	bBuf[16];
	uint32_t	iCount;
	uint8_t	* pbBuf;
	
	if (!(m_iReliableTestNumber % RELIABLE_TEST_PARA_SAVE_INTERVAL_NUM))
	{
		ReliableStoreWarnOn();	//�洢������ʼ
		
		pbBuf = bBuf;						//����С�˴洢�ֲ�
		for (iCount = 0; iCount < sizeof(int); iCount++)
		{
			*pbBuf = (uint8_t)(m_iReliableTestNumber >> (iCount * 8));
			pbBuf++;
		}
		
		//���ʼ�ͳ����洢��ʱ�Ȳ���,Ȼ��ֱ�Ӵ�ԭʼ��ַ0�洢,������ֱ�Ӵ洢�ٵ�����ַ
		if ((m_ReliableStoreIndex == 0x00) || \
				(m_ReliableStoreIndex >= (sFLASH_SPI_SECTORSIZE / sizeof(int))))
		{
			sFLASH_EraseSector(RELIABLE_TEST_PARA_BASE_ADDRESS);	//���²�������,��ͷ��ʼʹ��
			m_ReliableStoreIndex = 0;
		}		//���������β��ַ�����ֱ�Ӵ洢
		iCount = RELIABLE_TEST_PARA_BASE_ADDRESS + m_ReliableStoreIndex * sizeof(int);
		pbBuf = bBuf;
		sFLASH_WriteBuffer(pbBuf, iCount, sizeof(int));	//����洢��
		m_ReliableStoreIndex++;	//ָ���¸��洢��ʼ��ַ
		ReliableStoreWarnOff();	//�洢��������
	}	//�Ƿ񵽴�洢����
}

/*******************************************************************************
* Function Name  : ������
* Description    : 
	ͳ�ƴ�����ֵ���浽�ֿ�FLASH��.�㷨����:
	ֵ�������ֽڱ���,�������׵�ַ��ʼ��4K�ֽڵ�һ����������.
	���׵�ַ��ʼ���α���ֵ�����0~8λ,��8~16λ,��16~24λ,��24~32λ.
	��ʼ��ʱ�����׵�ַ������ַ���ĸ��ֽ�ֵ�Ƿ�ȫΪ0xFF,�жϸ������Ƿ���Ҫ����.
	����ʱ���׵�ַ��ʼ,���α���.ֱ������ĸ��ֽ�Ҳ��ʹ��ʱ,���²���,�ٴ�ͷ����.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
//�õ��Ϸ��洢�����ĵ�ǰ��ŵ���һ�����,��0��ʼ,��1024����.
//���׵�ַ��ʼ�����ȡ,ֱ������ȫΪFF�ĵ�ַ����.
//����ֵΪ�Ϸ����ݵ���Ч��ʼ��ŵ���һ�����,ÿ4���ֽڴ洢1��ֵ
//�����ʼ������ΪFF,�򷵻�0.
//�����ʼΪ0xFF,����0;
//�������ĸ��ֽڲ�����FF,�򷵻�1024.
//���������λ��,���ظ����洢��ʼ��ַ���ڱ�ŵ���һ�����.
static	uint32_t	GetReliableTestParaStoreIndex(void)
{
	uint8_t	cTemp;
	uint8_t	bBuf[sizeof(int)];
	uint32_t	iAddress;
	uint32_t	iIndex;
	uint32_t	iCount;
	uint8_t	* pBuf = NULL;
	
	iAddress = RELIABLE_TEST_PARA_BASE_ADDRESS;
	for (iIndex = 0; iIndex < (sFLASH_SPI_SECTORSIZE / sizeof(int)); iIndex++)
	{
		pBuf = bBuf;
		sFLASH_ReadBuffer(pBuf, iAddress, sizeof(int));	//��ȡ1��int�Ĵ洢ֵ
		iAddress += sizeof(int);
		cTemp = 0xFF;
		for (iCount = 0; iCount < sizeof(int); iCount++)
		{
			cTemp &= *pBuf++;		//�õ�����ֵ���ĸ��ֽڵ�ֵ������
		}
		if (cTemp == 0xFF)		//������ĩ��Ч�洢��ַ��һ���洢��
		{
			pBuf = NULL;
			return (iIndex);
		}
	}
	pBuf = NULL;
	
	return (iIndex);
}

//����ָ���洢��ŵ�ֵ,int�洢ʱ��С�˷�ʽ
static	uint32_t	GetReliableTestStoreValue(uint32_t iIndex)
{
	uint8_t	bBuf[sizeof(int)];
	uint32_t	iAddress;
	uint32_t	iNumber;
	uint32_t	iCount;
	uint8_t	* pBuf = NULL;
	
	if (iIndex > (sFLASH_SPI_SECTORSIZE / sizeof(int)))	//���Ĵ洢��
	{
		iIndex = (sFLASH_SPI_SECTORSIZE / sizeof(int));		//�Ϸ�������ΧӦ��Ϊ[0~1024]
	}
	if (iIndex)
		iIndex--;
	iAddress = RELIABLE_TEST_PARA_BASE_ADDRESS + iIndex * sizeof(int);
	pBuf = bBuf;
	sFLASH_ReadBuffer(pBuf, iAddress, sizeof(int));
	for (iNumber = 0, iCount = 0; iCount < sizeof(int); iCount++)
	{
		iNumber |= ((uint32_t)(*pBuf) << (iCount * 8));
		pBuf++;
	}
	if (iNumber == 0xFFFFFFFF)	//������ȫ��Ϊ��
	{
		iNumber = 0;
	}
	pBuf = NULL;
	
	return (iNumber);
}

/*******************************************************************************
* Function Name  : ������
* Description    : �ɿ��Դ�ӡ�ĳ�ʼ��,��ʼ����ʱ���ͱ�Ǳ���
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static	void InitReliableTest(void)
{
	giReliableTimerCount = 0;
	gcReliableRoutineFlag = 0;
	
	//��ȡ������������ֵ
	m_ReliableStoreIndex = GetReliableTestParaStoreIndex();
	//��ȡ���浽FLASH�е��Ѵ���Ĵ���
	m_iReliableTestNumber = GetReliableTestStoreValue(m_ReliableStoreIndex);
	
 	SetTimerCounterWithAutoReload(BACKUP_TIMER, 10*1000);	//��ʱ����ʱʱ��Ϊ1ms*1000 = 1s
	StartTimer(BACKUP_TIMER);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���ݲ������ݲ�ͬ���ɲ�ͬ�Ĵ�ӡ����,�������ӡ.
	
	ʹ��BACKUP_TIMER.
	������������ȫ���߱�.
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
static	void ReliableTest(void)
{
#if defined	(TEST_2MINUTE) || defined	(TEST_5MINUTE) || defined	(TEST_20MINUTE)
	ReliabilityTestMinute();
#elif defined	(TEST_50KM)
	ReliabilityTestKm50();
#elif defined	(TEST_CUTTER_LIFE)
	ReliabilityTestCutterLife();
#endif
}

#endif	//ȫ���ɿ��Բ��Դ��������� 


#ifdef	VERSION_RELIABILITY
/*******************************************************************************
* Function Name  : ������
* Description    : �ɿ��Բ��Եĳ���.����2����,20����,
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
int main(void)
{
	Sys_Init();		//��ʼ��ʱ��,GPIO�˿�,ȫ�ֱ���,�ⲿ�豸
	
#ifdef WITHBOOT
	CheckBootLegality();	//����BOOTLOAD����Ϸ���
#endif
	
	ProcessModeType();
	ClearComInBufBusy();

#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
	InitReliableTest();
#endif
	
	while (1)
	{
#if defined	(TEST_2MINUTE) || defined (TEST_5MINUTE) || defined (TEST_20MINUTE) || defined (TEST_50KM) || defined (TEST_CUTTER_LIFE)
		if (gcReliableRoutineFlag)
		{
			gcReliableRoutineFlag = 0;
			ReliableTest();
		}
#endif
		CheckFeedButton();						//���м�鰴���ȹ���
		CheckMaintainRoutin();				//���������ά�������洢
		ProcessCharMaskReadBuf();			//�����ַ������ȡ������
	}
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
