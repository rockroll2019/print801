/**
	******************************************************************************
	* @file		x.c
	* @author	Application Team  
	* @version V0.0.1
	* @date		2012-5-14
	* @brief	 ָ����س���.
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
#include	"gs.h"
#include	"barcode.h"
#include	"charmaskbuf.h"
#include	"cominbuf.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"feedpaper.h"
#include	"timconfig.h"
#include	"button.h"
#include	"led.h"
#include	"maintain.h"
#include	"cutter.h"
#include	"hexmode.h"
#include	"selftest.h"
#include	"init.h"
#include	"drawer.h"

extern	void ProcessBarCode_2D(uint8_t CommandType);
extern	void ClearInBuf(void);
extern  TypeDef_StructHeatCtrl	g_tHeatCtrl;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name	: LongToStr(uint32_t num, uint8_t *Str)
* Description		: ��������ת��ΪASCII�ַ���
* Input					: num:������,Str:�ַ�����ַ
* Output				 : None
* Return				 : None
*******************************************************************************/
void LongToStr(uint32_t num, uint8_t * Str)
{
	uint32_t Data;
	uint8_t	i;
	uint8_t	bBuf[16];
	
	i = 0;
	Data = num;
	while (1)
	{
		if ((Data == 0) && (i > 0))
			break;
		bBuf[i++] = Data % 10 + 0x30;
		Data /= 10;
	}
	for ( ; i > 0; )
	{
	 	i--;
		*Str++ = bBuf[i];
	}
	*Str = 0;
}

/*******************************************************************************
* Function Name	: ������
* Description		: ����
* Input					: �������
* Output				 : �������
* Return				 : ���ز���
*******************************************************************************/
/*******************************************************************************
* Function Name	: Command_1D21
* Description		: �����ַ�(ASCII�ַ�,����)�Ŵ���,
*	����HRI�ַ�����.D0~D2���ø߱���,D4~D6���ÿ���
*	(1	��	 ����Ŵ���	��	8 ,1	��	 ����Ŵﱶ��	��	8 )
*	�� 0	�� 3	λѡ���ַ��߶�,4	�� 7	λѡ���ַ����
* EPSONʹ�� 0 �� 2 λѡ���ַ��߶�,4 �� 6 λѡ���ַ����
*				�� 1														 �� 2 
*		�ַ��߶�ѡ��							�ַ����ѡ�� 
*	Hex	Decimal	����Ŵ�		Hex	Decimal 		����Ŵ� 
*	00			0 		1(����) 	00			0				1	(����) 
*	01			1			2(2����)	10			16			2	(2����) 
*	02			2			3					20			32 			3 
*	03			3			4					30			48			4 
*	04 			4			5					40			64			5 
*	05			5			6 			 	50			80			6 
*	06			6			7					60			96			7 
*	07			7			8					70			112			8 
*	����ָ��������ַ�(ASCII�ַ��ͺ���)����Ч,����HRI�ַ�����
*	���n�����˹涨�ķ�Χ,������ָ�����. 
*	�ڱ�׼ģʽ��,�����ǽ�ֽ����,�����Ǵ�ֱ�ڽ�ֽ�ķ���.���ǵ��ַ�˳ʱ����ת90��ʱ,
*	���������ߵ�. 
*	ҳģʽ��,���������ȡ��������ķ���.
*	ͬһ���ַ��ķŴ�����ͬʱ,���е��ַ��Ե��߶���.
*	ESC !	ָ��Ҳ����ѡ�����ȡ���ַ�����ͱ���,�����յ�ָ����Ч.
*	[ Ĭ��ֵ]		n = 0 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D21(void)
{
	uint8_t	GetChar;
	uint8_t	bTemp;
	
	GetChar = ReadInBuffer();
    if(GetChar & 0x88)									//�����������巶Χ��������
    {    
		return;
    }
	bTemp = (GetChar & 0x07);
	if (bTemp < MAX_RATIO_Y)
	{
		g_tPrtModeChar.HeightMagnification = bTemp + 1;	//�ַ�������
		g_tPrtModeHZ.HeightMagnification	 = bTemp + 1;	//����������
	}
	
	bTemp = ((GetChar >> 4) & 0x07);
	if (bTemp < MAX_RATIO_X)
	{
		g_tPrtModeChar.WidthMagnification = bTemp + 1;	//�ַ�������
		g_tPrtModeHZ.WidthMagnification		= bTemp + 1;	//���ֺ�����
	}
}

/*******************************************************************************
* Function Name	: ������
* Description		: 1D 23 n 
*	1 �� n �� 8
	ѡ��ǰ������λͼͼ��,����λͼ�ʹ�ӡλͼ���ݴ�ͼ�Ž���
�� ������ֻ������ص�RAM�е�λͼ,�趨���ݵ����ʧ.
. ���ص�FLASH�е�λͼ��ʹ�ñ�����ָ����ͼ��.
* Input					: �������
* Output				 : �������
* Return				 : ���ز���
*******************************************************************************/
void Command_1D23(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar >= 1) && (GetChar <= 8))
	{
		g_bRAMBitMapNumber = GetChar;
	}
}

/*******************************************************************************
* Function Name	: ������
* Description		: 1D 24 nL nH
	ҳģʽ�������������λ��
[ ��Χ]     0  ��   nL ��   255, 0 ��   nH ��   255 
[ ����]     ��  ҳģʽ�����þ�������λ��. 
��  ����ָ��������λ�������� [(nL + nH �� 256) �� (���������ƶ���λ)]Ӣ�紦. 
[ ע��]     ��  ����ָ��ֻ����ҳģʽ����Ч. 
��  ���[(nL+nH �� 256)��(���������ƶ���λ)]�����趨�Ĵ�ӡ����,�����������.
��  ִ�и�ָ���,����λ�ò��ı�.
��  �ο�λ����  ESC T   ָ������.
��  ����ESC T ָ�����õĴ�ӡ���������ʼλ�õĲ�ͬ
��ָ��ʹ��ӡ���������²���: 
�������ӡ��ʼλ��Ϊ���Ͻǻ������½�,��ָ��������ֽ����ƽ�еķ��������þ���λ��
�������ӡ��ʼλ��Ϊ���Ͻǻ������½�,��ָ��������ֽ����ֱ�ķ��������þ���λ��. 
��  ����������ƶ���λ����  GS P  ָ�����õ�. 
* Input					: �������
* Output				 : �������
* Return				 : ���ز���
*******************************************************************************/
void Command_1D24(void)
{
// 	if (g_tSystemCtrl.PrtModeFlag == PAGE_MODE)
	{
		ReadInBuffer();
		ReadInBuffer();
	}
}


/*******************************************************************************
* Function Name	: Command_1D2841
* Description		: ִ�в��Դ�ӡ 
*	1D	28	41	pL	pH	n	 m
*	( pL+ pH	�� 256)=2	(pL=2, pH=0) 
*	0	��	n ��	2,		48	��	n ��	50 
*	1	��	m��	3,	49	��	m	��	51
*	ִ�в��Դ�ӡ,��ӡ��ʽ��n,m ����,pL,pH(pL+pH ��256 )��ʾpH����ֽ���(n��m)
*	n	��������ֽ������ 
*		n	 	ֽ������ 
*	0, 48		�������� ( ��ֽ) 
*	1, 49 	��ֽ 
*	2, 50 	��ֽ 
*	m ������ӡ���� 
*	m				��ӡ���� 
*	1, 49		ʮ������ת���ӡ 
*	2, 50		��ӡ���ڲ�������Ϣ��ӡ 
*	3, 51		ѭ���ַ���ӡ 
*	������ֻ�ڱ�׼ģʽ��������Ч 
*	��������ҳģʽ����Ч 
*	����ں궨������н��յ�������,��ֹͣ�궨�岢ִ�и����� 
*	ִ�б������,��ӡ���Զ���λ����ȡDIP��������. 
*	������ִ�н���ʱ,��ӡ����ֽ 
*	ִ�б�����ʱ,��ӡ������æ״̬,�������������
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D2841(void)
{
	uint8_t	pL;
	uint8_t	pH;
	uint8_t	n;
	uint8_t	m;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)	//��׼ģʽ
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//����
		{
			pL = ReadInBuffer();
			pH = ReadInBuffer();
			n	= ReadInBuffer();
			m = ReadInBuffer();
			if (	((pL == 2) && (pH == 0)) && \
						((n <= 2) || ((n >= 48) && (n <= 50))) && \
						(((m >= 1) && (m <= 3)) || ((m >= 49) && (m <= 51)))
					)	//�����Ƿ���Ч
			{
				SetComInBufBusy();		//ִ�б�����ʱ,��ӡ������æ״̬,�������������
				//ReceiveInterrupt(DISABLE);	//�ؽ����ж�
				DisableReceiveInterrupt();
				if (g_tMacro.DefinitionFlag)	//�궨����,�˳��궨��,�����
				{
					g_tMacro.Number = 0;				//���ַ���������
					g_tMacro.GetPointer = 0;
					g_tMacro.DefinitionFlag = 0;
					g_tMacro.SetFlag = 0;				//�����ı�־����
					g_tMacro.RunFlag = 0;
				}
				
				if ((m == 1) || (m == 49))	//ʮ������ת���ӡ
				{
					HexMode();
				}
				else if ((m == 2) || (m == 50))	//��ӡ���ڲ�������Ϣ��ӡ
				{
					SystemSelfTest();
				}
				else if ((m == 3) || (m == 51))	//ѭ���ַ���ӡ
			 	{
					SystemSelfTest();
				}
				
				while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus())
				{
					;
				}
			 	InitParameter(0);				//����ϵͳĬ��ֵ��ʼ��ȫ�ֽṹ
				ClearComInBufBusy();		//���æ�ź�
				//ReceiveInterrupt(ENABLE);		//�������ж�
				EnableReceiveInterrupt();
			}
		}
	}
}

/*******************************************************************************
* Function Name	: Command_1D2844
* Description		: ����,��ֹʵʱָ�� 
* 1D	28	44	pL	pH	m	[a1 b1] �� [ak bk] 
* 3	 ��	(pL + pH �� 256)	��	65535	(0 ��	pL	��	255, 0	��	pH	��	255) 
* m = 20 
* a = 1, 2 
* b = 0, 1, 48, 49
* ͨ�� a	ȷ��������ֹʵʱָ��
* pL,pH(pL+pH ��256)��ʾpH����ֽ���(m,[a1 b1]��[ak bk])
* a		b								����
* 1		0, 48	DLE DC4 fn m t (fn = 1): ������(��ֹ)
* 1		1, 49	DLE DC4 fn m t (fn = 1): ����(����) 
* 2		0, 48	DLE DC4 fn a b (fn = 2): ������(��ֹ)
* 2		1, 49	DLE DC4 fn a b (fn = 2): ����(����) 
* ���ͼ�������а�������DLE DC4 (fn = 1 or 2) һ�������ݴ�,����Ԥ��ʹ�ô�ָ��
* ����ֹʵʱָ��
* Input					: None
* Output				 : None
* Return				 : None
* 1D 28 44 pL pH m [a1 b1]��[ak bk],ͨ�� a ȷ��������ֹʵʱָ��
*******************************************************************************/
void Command_1D2844(void)
{
	uint8_t	pL;
	uint8_t	pH;
	uint8_t	m;
	uint8_t	a;
	uint8_t	b;
	uint16_t	DataLen;
	uint16_t	i;
	
	pL = ReadInBuffer();
	pH = ReadInBuffer();
	//DataLen	= pL + 256 * pH;	//��ʾ֮��Ӧ�ý��յ��ֽ���Ŀ
	DataLen	= pL + (uint16_t)(((uint16_t)pH) << 8);	//��ʾ֮��Ӧ�ý��յ��ֽ���Ŀ
	if (DataLen < 3)
		return;
	
	m = ReadInBuffer();
	if (m != 20)
		return;
	DataLen--;								//��ȥmռ�õ��ֽ���
	
	for (i = 0; i < DataLen; i += 2)
	{
		a = ReadInBuffer();
		b = ReadInBuffer();
		if (a == 1)
		{
			if ((b == 0) || (b == 48))	//������(��ֹ)DLE DC4 fn m t (fn = 1),10 14 fn m t ��Ǯ���ָ��
			{
				g_tRealTimeCmd.RealTimeAllow &= (uint8_t)(~0x01);	//bit0=0;��ֹʵʱ����
			}
			else if ((b == 1) || (b == 49))	//����(����)DLE DC4 fn m t (fn = 1)
			{
				g_tRealTimeCmd.RealTimeAllow |= 0x01;	//bit0=1;����ʵʱ����
			}
		}
		else if (a == 2)
		{
			if ((b == 0) || (b == 48))	//������(��ֹ)DLE DC4 fn a b (fn = 2),1014 fn a bִ�йػ�����ָ�� 
			{
				g_tRealTimeCmd.RealTimeAllow &= (uint8_t)(~0x02);	//bit1=0;��ֹ�ػ�����ִ��
			}	
			else if ((b == 1) || (b == 49))	//����(����) DLE DC4 fn a b (fn = 2):
			{
				g_tRealTimeCmd.RealTimeAllow |= 0x02;	
			}
		}
	}
	
}

/*******************************************************************************
* Function Name	: Command_1D2845
* Description		: Set user setup commands
Controls the user setting modes. The table below explains the functions available 
in this command. Function code fn  specifies the function.
fn 	Function No. 					Function name
1		Function 1		Change into the user setting mode.
2 	Function 2		End the user setting mode session.
3 	Function 3		Change the memory switch.
4 	Function 4		Transmit the settings of the memory switch.
5 	Function 5		Set  the customized setting values.
6 	Function 6		Transmit the customized setting values.
7 	Function 7		Copy the user-defined page.
8 	Function 8		Define the data (column format ) for the character code page.
9 	Function 9		Define the data (raster format) for the character code page.
10 	Function 10		Delete the data for the character code page.
11	Function 11		Set the communication item for the serial interface.
12	Function 12		Transmit the communication item  for the serial interface.
13	Function 13		Set the configuration item for the  Bluetooth interface.
14	Function 14		Transmit the configuration item  for the Bluetooth interface.
48	Function 48		Delete the paper layout
49	Function 49		Set the paper layout
50	Function 50		Transmit the paper layout information
	pL, pH specifies ( pL + pH �� 256) as the number of bytes after 
	pH ( fn and [parameters] ).
	Description of the [parameters] is described in each function.
[Notes]
�� The value of parameter fn determines the function number for this command. 
	Command operation differs, depending on the function number.
�� User setting mode is a special mode to change settings for the printer��s 
	built-in non-volatile memory.
�� To change a setting, you must change the printer into user setting mode, 
	using Function 1.
�� Execute function 2 of this command after making the setting. The setting value 
	is effective after executing the software reset of function 2.  
�� Note the rules below for the user se tting mode:
? The printer does not process character data.
? The printer does not process commands other than this command and the GS I command.
? The printer does not process real-time commands.
? Even if the ASB function is enabled, the printer does not transmit the ASB status.
�� Functions 4, 6, 12, 14 and 50, the functions used to transmit the current settings 
	to the host, can be used at any time, without changing into the user setting mode.
�� Data is written to the non-volatile memory by Functions 3, 5, 7, 11, 13, 48, and 49. 
	Note the following when using those functions:
? Do not turn off the power or reset the printer from the interface when the relevant 
	functions are being executed.
? The printer might be BUSY. In this case, be sure not to transmit a command from the 
	host because the printer will not receive the data.
? Excessive use of this function may destroy the non-volatile memory. As a guideline, 
	do not use any combination of the following commands more than 10 times per day for 
	writing data to the non-volatile memory: 
	GS(A (part of functions), GS (C (part of functions), GS(E (part of functions), 
	GS(L / GS 8 L (part of functions), GS(M (part of functions), GS g 0, FS g 1, FS q.
[Notes for transmission process]
�� Data is transmitted by Functions 1, 4, 6, 12, 14 and 50. When you use these functions, 
	obey the following rules.
? When the host PC transmits t he function data, transmit the next data after receiving 
	the corresponding data from the printer.
? With a serial interface printer, be sure to use this function when the host can receive data.
? With a parallel interface printer, data transmitted (excluding ASB status) with this command 
	(��Header to NUL��) is temporarily stored in the printer transmit buffer, as with other data. 
	When the host goes into reverse mode, the printer then transmits the data sequentially from 
	the beginning of the transmit buffer. Transmit buffer capacity is 99 bytes. Data exceeding 
	this amount is lost. Therefore, when using this command, promptly change into reverse mode 
	to start the data receive process.
? When communication with the printer uses XON/XOFF control with serial interface, 
	the XOFF code may interrupt the ��Header to NUL�� data string.
? The transmission information for each function can be identified to other 
	transmission data according to specific data of the transmi ssion data block. 
	When the header transmitted by the printer is [hex = 37H/decimal =55], treat 
	NUL [hex  = 00H/decimal =0] as a data group and identify it according to the 
	combination of the header and the identifier.

[Notes for ESC/POS Handshaking Protocol]
�� Use ESC/POS Handshaking Protocol below for Functions 14.
Step Host process									Printer process
	1 	Send  											GS(C < Function 14>. Start processing of Function 14. 
																	(Read specified record back to host.)
	2 	Receive data from printer.	Send device data.
	3 	Send response code.(*1)			Continue processing (*2) (*3) according to response.

(*1)
 Response code
ASCII		Hex		Decimal		Request
	ACK		06		6				Send next data.
	NAK		15		21			Resend previously sent data.
	CAN		18		24			Cancel send process.

(*2)
Processing according to response (unsent data exists, identified by send data 
set ��Identification status��)
Response code		Process
	ACK						Start send processing for next data.
	NAK						Resend previously sent data.
	CAN						End processing for this command.

�� Processing the codes except for ACK, NAK, and CAN performs the same processing as CAN.
(*3) Processing according to response (no unsent data, identified by send data set 
	��Identification status��)
Response code		Process
	ACK, CAN			End processing for this command.
	NAK						Resend previously sent data.


* Input					: None
* Output				 : None
* Return				 : None
* 1D 28 44 pL pH m [a1 b1]��[ak bk],ͨ�� a ȷ��������ֹʵʱָ��
*******************************************************************************/
void Command_1D2845Function1(void)
{
}

void Command_1D2845Function2(void)
{
}

void Command_1D2845Function3(void)
{
}

void Command_1D2845Function4(void)
{
}

void Command_1D2845Function5(void)
{
}

void Command_1D2845Function6(void)
{
}

void Command_1D2845Function7(void)
{
}

void Command_1D2845Function8(void)
{
}

void Command_1D2845Function9(void)
{
}

void Command_1D2845Function10(void)
{
}

void Command_1D2845Function11(void)
{
}

void Command_1D2845Function12(void)
{
}

void Command_1D2845Function13(void)
{
}

void Command_1D2845Function14(void)
{
}

void Command_1D2845Function48(void)
{
}

void Command_1D2845Function50(void)
{
}

void Command_1D2845(void)
{
	uint8_t	pL, pH, fn;
		
	pL = ReadInBuffer();
	pH = ReadInBuffer();
	fn = ReadInBuffer();
	
	if ((pL == 0x03) && (pH == 0x00) && (fn == 0x01))
	{	//1D 28 45 03 00 01 49 4E
		Command_1D2845Function1();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x02))
	{	//1D 28 45 04 00 02 4F 55 54
		Command_1D2845Function2();
	}
	else if (fn == 0x03)
	{	//1D 28 45 pL  pH 03 [a1 b18 ...b11]...[ak nk8  nk1]
		Command_1D2845Function3();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x04))
	{	//1D 28 45 02 00 04 a
		Command_1D2845Function4();
	}
	else if (fn == 0x05)
	{	//1D 28 45 pL pH 05 [a1 n1L n1H] ... [ak nkL nkH]
		Command_1D2845Function5();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x06))
	{	//1D 28 45 02 00 06 a
		Command_1D2845Function6();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x07))
	{	//1D 28 45 04 00 07 a d1 d2
		Command_1D2845Function7();
	}
	else if (fn == 0x08)
	{	//1D 28 45 pL pH 08 y c1 c2 [x d1...d(y �� x)]k
		Command_1D2845Function8();
	}
	else if (fn == 0x08)
	{	//1D 28 45 pL pH 08 x c1 c2 [x d1...d(x �� y)]k
		Command_1D2845Function9();
	}
	else if ((pL == 0x03) && (pH == 0x00) && (fn == 0x0A))
	{	//1D 28 45 03 00 0A c1 c2
		Command_1D2845Function10();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x0B))
	{	//1D 28 45 pL pH 0B a d1...dk
		Command_1D2845Function11();
	}
	else if (fn == 0x0B)
	{	//1D 28 45 pL pH 0B a
		Command_1D2845Function12();
	}
	else if (fn == 0x0D)
	{	//1D 28 45 pL pH 0D a d1...dk
		Command_1D2845Function13();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x0E))
	{	//1D 28 45 02 00 0E a
		Command_1D2845Function14();
	}
	else if ((pL == 0x04) && (pH == 0x00) && (fn == 0x30))
	{	//1D 28 45 04 00 30 d1 d2 d3
		Command_1D2845Function48();
	}
	else if ((pL == 0x02) && (pH == 0x00) && (fn == 0x32))
	{	//1D 28 45 02 00 32 n
		Command_1D2845Function50();
	}
}

/*******************************************************************************
* Function Name	: Command_1D2848
* Description		: 1D28ϵ��ָ�� ����Ӧ���״̬���� 
* Input					: None
* Output				 : None
* Return				 : None
*1D 28 48 pL pH fn m d1 d2 d3 d4
*******************************************************************************/
void Command_1D2848(void)
{
  uint8_t	Data[256];
	uint16_t Len, i;

	Len  = ReadInBuffer()+ 256 * ReadInBuffer();	//���ݳ���
	
	//����(pL+256*pH)����
	for(i=0; i< Len; i++)
	{
		if( i < 6 )
			Data[i]= ReadInBuffer();
		else								//��������ݶ���
			ReadInBuffer();
	}
	
	if(Data[0] ==48 && Len >= 6)			//fn=48,������������Ҫ��
	{
		if(Data[1] ==48 )					//m=48
		{
			Data[0] =0x37;					//���ع̶���ʽ
			Data[1] =0x22;

			for(i=2; i < 6; i++)					//ID�ų���Ϊ4�ֽ�
			{
				if( Data[i] <32 || Data[i] > 126)	// 32=< dn <=126
					break;
			}
			if(i==6)							//������Ч
			{
				Data[i]=0x00;
				while(g_bMotorRunFlag !=0  || g_tFeedButton.StepCount !=0 || U_PrtBufCtrl.IdxWR != U_PrtBufCtrl.IdxRD);				  //2016.06.30
 				//�ȴ�ǰ�����ݴ������	
				UpLoadData(Data, 7);			//�ϴ�����
			}
		}
	}
	else					//�Ժ���������������
	{
	
	}
}

void Command_1D284B(void)
{
}

void Command_1D284C(void)
{
}

void Command_1D284E(void)
{
}


/*******************************************************************************
* Function Name	: Command_1D286B
* Description		: 1D286Bϵ��ָ��  ��ӡ��ά�� ����Ө��ͨ��ӡ��
   1D286B PL PH cn fn parametersָ���ӡ��ά�룬����Ө��ָͨ��;
   cn: 48(0x30) ��ά��PDF417��49(0x31) ��ά��QR��;
   cn=48ʱ��
   fn=65, GS ( k pL pH cn fn n,           PDF417: Set the number of columns in the data region.
   fn=66, GS ( k pL pH cn fn n            PDF417: Set the number of rows.
   fn=67, GS ( k pL pH cn fn n            PDF417: Set the width of the module.
   fn=68, GS ( k pL pH cn fn n            PDF417: Set the row height.
   fn=69, GS ( k pL pH cn fn m n          PDF417: Set the error correction level.
   fn=70, GS ( k pL pH cn fn m            PDF417: Select the options.
   fn=80, GS ( k pL pH cn fn m d1...dk    PDF417: Store the data in the symbol storage area.
   fn=81, GS ( k pL pH cn fn m            PDF417: Print the symbol data in the symbol storage area.
   fn=82, GS ( k pL pH cn fn m            PDF417: Transmit the size information of the symbol data in the symbol storage area.
   
   cn=49ʱ��
   fn=65, GS ( k pL pH cn fn n1 n2      165 QR Code: Select the model.
   fn=67, GS ( k pL pH cn fn n          167 QR Code: Set the size of module.
   fn=69, GS ( k pL pH cn fn n          169 QR Code: Select the error correction level.
   fn=80, GS ( k pL pH cn fn m d1...dk  180 QR Code: Store the data into the symbol storage area.
   fn=81, GS ( k pL pH cn fn m          181 QR Code: Print the symbol data in the symbol storage area.
   fn=82, GS ( k pL pH cn fn m          182 QR Code: Transmit the size information of the symbol data in the symbol storage area.

* Input					 : None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D286B(void)
{	
	uint8_t PH;
	uint8_t PL;
	uint8_t	GetChar;	
	uint8_t FunType;
	uint8_t m;
  uint8_t n;
	uint16_t k;
	uint16_t DataNum;	
	
	PL = ReadInBuffer();
	PH = ReadInBuffer();
	DataNum = PL + PH*256;	 	   //���ݳ���
	GetChar = ReadInBuffer();		       //��������
	if(GetChar == 48)
	{
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = 0;   //��������PDF417
	}	
	else if(GetChar == 49)
	{
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = 2;   //��������QR��
	}
	else
	{
		return;
	}	
	FunType = ReadInBuffer();						//ָ������
	if(FunType == 65)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_version = ReadInBuffer();				 //PDF417 Ϊÿ���ַ��� �� QRΪ�汾		
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)		//QR Code: Select the model
		{
			m = ReadInBuffer();				               
			n = ReadInBuffer();	
			if(m==49)
			{
				QR_model = 1;
			}
      else if(m==50)
			{
				QR_model = 2;
			}		
		}		
	}	
	else if(FunType == 66)						 //��ȡ��������
	{		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{	
			PDF417_Rows = ReadInBuffer();
		}	
	}
	else if(FunType == 67)
	{		
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_ModuleWidth = ReadInBuffer();				              //��Χ2-8 Ĭ��Ϊ3	
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)	  //QR Code: Set the size of module
		{
			QR_Rate = ReadInBuffer();				                          //�Ŵ���
		}	
	}
	else if(FunType == 68)
	{			
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_Rate = ReadInBuffer();				 //row height ��Χ2-8 Ĭ��Ϊ3	
		}		
	}	
	else if(FunType == 69)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				                   //����ȼ�
			n = ReadInBuffer();	
			if(m==48)
			{
				if(n>=48 && n<=56)
				{
					PDF417_vColl = n-48;		
				}	
			}	
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();												 //����ȼ�
			if(m>=48 && m <=51)
			{
				QR_vColl = m;														
			}	
		}	
	}	
	else if(FunType == 70)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			PDF417_options = ReadInBuffer();				    //PDF417����ѡ��
		}		
	}	
	else if(FunType == 80)												  //�洢��������
	{
		TwoCodeDataNum = DataNum -3;
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				for(k=0;k<(TwoCodeDataNum);k++)
				{
					g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[k] = ReadInBuffer();
				}
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				for(k=0;k<(TwoCodeDataNum);k++)
				{
					g_tRamBuffer.BarCode2DStruct.BarCode2DBuf[k] = ReadInBuffer();
				}
			}
		}	
	}	
	else if(FunType == 81)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				ProcessBarCode_2D_1D28();									//��ӡpdf417����
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				ProcessBarCode_2D_1D28();									//��ӡQR
			}
		}	
	}	
	else if(FunType == 82)
	{
		if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 0)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				;                                         //�����
			}
		}	
		else if(g_tRamBuffer.BarCode2DStruct.BarCode2DType == 2)
		{
			m = ReadInBuffer();				   
      if(m==48)
		  {
				;                                         //�����
			}
		}
	}	
	else
	{
		return;
  }	
}

/*******************************************************************************
* Function Name	: Command_1D28
* Description		: 1D28ϵ��ָ��
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D28(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x41:
		{
			Command_1D2841();		//ִ�в��Դ�ӡ
			break;
		}
		case 0x44:
		{
			Command_1D2844();		//����,��ֹʵʱָ��
			break;
		}
		case 0x45:
		{
			Command_1D2845();		//�����û�����ָ���һϵ��ָ��,�����
			break;
		}
		case 0x48:			
		{
			Command_1D2848();		//����Ӧ���״̬���� 2016.06.14
			break;
		}
		case 0x4B:
		{
			Command_1D284B();		//�����
			break;
		}
		case 0x4C:
		{
			Command_1D284C();		//�����
			break;
		}
		case 0x4E:
		{
			Command_1D284E();		//�����
			break;
		}
		case 0x6B:
		{
			Command_1D286B();		//��ӡ��ά�� ����Ө��ͨ
			break;
		}
		default:
			break;
	}
}

/*******************************************************************************
* Function Name	: Command_1D2A
* Description		: ����һ������λͼ,������� x �� y ָ��
1D 2A x	y	d1...dk
1	��	x	��	255, 1 �� y �� 48
x	��	y	�� 768
0	��	d	��	255
k = x ��	y ��	8
����һ������λͼ,������� x �� y ָ��
��	x	ָ��λͼ�ĺ����ֽ�����
��	y	ָ��λͼ�������ֽ�����
��	d	ָ��λͼ������.
��	λͼ�����ϵĵ���Ϊ x �� 8��λͼ�����ϵĵ���Ϊ y �� 8.
��	��� x �� y	�����涨�ķ�Χ,������ָ����Ч. 
��	dΪλͼ����.���ݶ�ӦλΪ1��ʾ��ӡ�õ�,Ϊ0��ʾ����ӡ. 
��	���ص�λͼ����������±����: 
��	ִ��ESC @ָ�� 
��	��ӡ����Դ�رջ�λ. 

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define	DOWNLOAD_BITMAP_MAX_WIDTH					(255)
#define	DOWNLOAD_BITMAP_MAX_HEIGHT				(48)

#define	DOWNLOAD_BITMAP_MAX_DATA_BYTES		(768)		//���768
#define	DOWNLOAD_BITMAP_MAX_DATA_LENGTH		(BMP_MAX_NUM * BMP_LENGTH)
void Command_1D2A(void)
{
	uint8_t	X;
	uint8_t	Y;
//	uint16_t i;
	uint16_t RAMLength;
	uint8_t	* pbData = NULL;
	
	//˵��RAMBuffer��ֻ����1��λͼ������
	X = ReadInBuffer();										//�����ֽ���
	Y = ReadInBuffer();										//�����ֽ���
	//if (Y > DOWNLOAD_BITMAP_MAX_HEIGHT)		//���߶�ֵ,�ֽڼ���
	//	return;
	
	RAMLength = (uint16_t)(((uint16_t)(X) * Y) << 3);			//��������
	if (RAMLength > DOWNLOAD_BITMAP_MAX_DATA_LENGTH)
		return;
	
	g_tRamBuffer.BitImageStruct.BitImageFlag = 2;		//����λͼ��־
	//g_tRamBuffer.BitImageStruct.BitImageData[0] = X;
	//g_tRamBuffer.BitImageStruct.BitImageData[1] = Y;
	//pbData = g_tRamBuffer.BitImageStruct.BitImageData;
	pbData = g_tRamBuffer.BitImageStruct.BitImageData;
	*pbData = X;
	pbData++;
	*pbData = Y;
	pbData++;

	while (RAMLength)
	{
		*pbData++ = ReadInBuffer();
		RAMLength--;
	}
	
	pbData = NULL;
}

/*******************************************************************************
* Function Name	: Command_1D2F
* Description		: ��ӡ����λͼ,��ӡģʽ�� m	ָ��
1D	 2F	 m
0	��	 m ��	 3, 48 ��	 m ��	 51
	m		 ģʽ					����ֱ��� (DPI)	 ����ֱ��� (DPI)	
0, 48	����					203									203 
1, 49	����					203									101 
2, 50	����					101									203 
3, 51	����,����			101									101 
������ص�λͼû�б�����,����ָ�����. 
�ڱ�׼ģʽ��,ֻ�д�ӡ������û������ʱ,��ָ�����Ч. 
�����ô�ӡģʽ��,������ӡģʽ�Ը�ָ����Ч(�����Ӵ�,˫�ش�ӡ,�»���,
����Ŵ��Լ����Դ�ӡ��). 
������ص�λͼ�����˴�ӡ����,�򳬳��Ĳ��ֲ���ӡ. 
�������ӡλͼΪ���ص�RAM�е�λͼ,�������ص�FLASH�е�λͼ. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D2F(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	ratiox;
	uint8_t	ratioy;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint16_t X_Length;
	uint16_t Y_Length;
	uint16_t x;
	uint16_t y;
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbBuf = NULL;
	uint8_t	* p = NULL;
	uint16_t  Index;
	s16	Lines;
	
//�ڱ�׼ģʽ�ж�����λͼ��������
// 	if ((g_tRamBuffer.BitImageStruct.BitImageFlag == 2) && \
// 			(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit) && \
// 			(g_tSystemCtrl.PrtModeFlag == STD_MODE))

	if ((g_tRamBuffer.BitImageStruct.BitImageFlag == 2) && (g_tSystemCtrl.PrtModeFlag == STD_MODE))             //2017.02.09 ȥ����������			
	{
		GetChar = ReadInBuffer();
		if ((GetChar <= 3) || ((GetChar >= 48) && (GetChar <= 51)))	//ȷ��������Ч
		{
			RatioX = (GetChar & 0x01) + 1;					//����Ŵ���
			RatioY = ((GetChar & 0x02) >> 1) + 1;		//����Ŵ���
			//X_Length = g_tRamBuffer.BitImageStruct.BitImageData[0] * 8;	//����˫���ܶ�,�������
			X_Length = (uint16_t)(((uint16_t)g_tRamBuffer.BitImageStruct.BitImageData[0]) << 3);	//����˫���ܶ�,�������
			Y_Length = g_tRamBuffer.BitImageStruct.BitImageData[1];			//����˫���ܶ�,���ֽڼ���
			//������ӡ����Ĳ���ӡ
			if ((X_Length * RatioX) > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit))
			{
				X_Length = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit) / RatioX;
			}
			
			//WaitPrintAndFeedPaperFinish();		//�ȵȴ�֮ǰ�Ĵ�ӡ���
			
// 			if (MAX_PRT_HEIGHT_BYTE >= Y_Length * RatioY)	//ȷ�����������༭��������С,һ�δ�ӡ���
// 			{
// 				for (y = 0; y < Y_Length; y++)		//����߶�,���ֽ�Ϊ��λ
// 				{	//ָ��ǰ�༭���ݻ�������ʼ�ֽ���
// 					pbByteStart = g_tLineEditBuf + \
// 						(MAX_PRT_HEIGHT_BYTE - (Y_Length - y) * RatioY) * PRT_DOT_NUMBERS + \
// 						g_tPrtCtrlData.PrtLeftLimit;
// 					for (x = 0; x < X_Length; x++)	//������,�Ե�Ϊ��λ
// 					{
// 						pbTemp = pbByteStart + (x * RatioX);	//ָ��ǰ�༭���ݻ�������
// 						GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + y];	//����������ӡ˳�����
// 						if (GetChar)
// 						{
// 							if (RatioY > 1)		//��Ҫ�Ŵ�
// 							{
// 								memset(TempBuf, 0x00, MAX_RATIO_Y);
// 								//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
// 								ByteZoomCtrl(RatioY, GetChar, TempBuf);
// 							}
// 							else		//����Ŵ���Ϊ1
// 							{
// 								TempBuf[0] = GetChar;
// 								TempBuf[1] = GetChar;
// 							}
// 							for (ratiox = 0; ratiox < RatioX; ratiox++)
// 							{
// 								for (ratioy = 0; ratioy < RatioY; ratioy++)
// 								{
// 									*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
// 								}
// 								pbTemp++;
// 							}	//һ���ֽ�������
// 						}	//���ַ���Ϊ0�Ŵ������,����ֱ������
// 					}	//һ���ֽں���������
// 				}	//������ȫ��������
// 				
// 				g_tPrtCtrlData.PrtDataDotIndex += (X_Length * RatioX);
// 				g_tPrtCtrlData.BitImageFlag = 1; //λͼ��ӡ��־
// 				g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((Y_Length * RatioY) << 3);	//�����ܵ���
//         //��ӡ�ñ༭������
// 				PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
// 			}	//һ�δ�ӡ�༭���
// 			else	//�߶��ϳ����˿ɱ༭��������С,��Ҫ��Ϊ��δ�ӡ,ÿ�δ���1�ֽ���

		for (y = 0; y < Y_Length; y++)		//����߶�,���ֽ�Ϊ��λ
		{	//ָ��ǰ�༭���ݻ�������ʼ�ֽ���  (Y_Length - y) * RatioY ��ΪRatioY 2016.05 
			pbByteStart = g_tLineEditBuf + \
				(MAX_PRT_HEIGHT_BYTE - RatioY ) * PRT_DOT_NUMBERS + \
				g_tPrtCtrlData.PrtLeftLimit;
			for (x = 0; x < X_Length; x++)	//������,�Ե�Ϊ��λ
			{
				pbTemp = pbByteStart + (x * RatioX);		//ָ��ǰ�༭���ݻ�������
				if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//�����ӡ
				{
					GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + (Y_Length - 1 - y)];
				}
				else		//����������ӡ˳�����
				{
					GetChar = g_tRamBuffer.BitImageStruct.BitImageData[2 + Y_Length * x + y];
				}
				if (GetChar)
				{
					if (RatioY > 1)	//��Ҫ�Ŵ�
					{
						memset(TempBuf, 0x00, MAX_RATIO_Y);
						//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//����Ŵ���Ϊ1
					{
						TempBuf[0] = GetChar;
						TempBuf[1] = GetChar;
					}
					for (ratiox = 0; ratiox < RatioX; ratiox++)
					{
						for (ratioy = 0; ratioy < RatioY; ratioy++)
						{
							*(pbTemp + ratiox + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
						}
					}	//һ���ֽ�������
				}
			}	//һ���ֽں���������
			g_tPrtCtrlData.PrtDataDotIndex += (X_Length * RatioX);
			g_tPrtCtrlData.MaxPrtDotHigh = ((uint16_t)(RatioY) << 3);	//�����ܵ���
  				
			//�����д�ӡ������
			pbBuf = g_tLineEditBuf + \
			       (MAX_PRT_HEIGHT_BYTE - RatioY) *  PRT_DOT_NUMBERS;  //�ַ��ڱ༭���������׵�ַ
					
			if( g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
			{										//�����ӡ
				for( Lines=g_tPrtCtrlData.MaxPrtDotHigh; Lines >0; )              //��ӡ������
				{
					Lines--;
					p = pbBuf + (Lines>>3 )* PRT_DOT_NUMBERS;
					MovDotLine( p, Lines);	           
				}
			}
			else		 					 //�����ӡ
			{
				for( Lines=0; Lines < g_tPrtCtrlData.MaxPrtDotHigh; Lines++ )	  	     //��ӡ������ �����ڱ༭�������ĵײ������ݴ��ϵ��µĵ��У������0���������
				{
					p = pbBuf +  (Lines>>3 )* PRT_DOT_NUMBERS ;													 //����������ڵ��ֽ��е�λ��
					MovDotLine( p,  Lines);	                 			              
				}	
			}
			PrepareNewLine();	    //���g_tLineEditBuf���»��ߵȿ��Ʊ���׼����һ�б༭
		}	//������ȫ��������
	 }	//������Ч�������
	}	//�������Ҷ���������λͼ�������
	
	g_tTab.TabIndex = 0;		//2016.09.06
	
	pbByteStart = NULL;
	pbTemp = NULL;
}

/*******************************************************************************
* Function Name	: Command_1D3A
* Description		: ����/�����궨��
����������ʱ,��ӡ�����յ���ָ�ʼ�궨��.�ں궨��ʱ,��ӡ�����յ���ָ��
�����궨��. 
��	����ӡ���ڶ����ʱ���յ�	GS ^ָ��,������궨�岢������궨��. 
��	��ӡ���ϵ�ʱ,û�к궨��. 
��	ESC @��������궨��,��˺궨�����ݿ��԰���ESC @	ָ��. 
��	�����ӡ�ڸս�����GS :ָ����������յ�GS :ʱ,��ӡ����Ȼû�к궨��. 
��	�궨������ݿ��Դﵽ2048 �ֽ�.����궨������ݳ���2048 �ֽ�,���������� 
��������ͨ���ݴ���. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D3A(void)
{
	if (g_tMacro.DefinitionFlag == 0)
	{
		g_tMacro.DefinitionFlag = 1;	//�궨�忪ʼ��־
		g_tMacro.Number = 0;					//��������
		g_tMacro.GetPointer = 0;			//��ʼλ������
		g_tMacro.RunFlag = 0;					//������״̬
		g_tMacro.SetFlag = 0;					//�궨����ɱ�־����
	}
	else
	{
		g_tMacro.DefinitionFlag = 0;	//�궨�������־
		if (g_tMacro.Number >= 2)
			g_tMacro.Number -= 2;				//��ȥ������1D3A
		
		if (g_tMacro.Number == 0)
			g_tMacro.SetFlag = 0;				//����������ñ�־
		else
			g_tMacro.SetFlag = 1;				//�ú������ñ�־
	}
}

/*******************************************************************************
* Function Name	: Command_1D42
* Description		: ѡ��/ȡ������ģʽ
*	�� n	�����λΪ 0ʱ,ȡ�����Դ�ӡ.
*	�� n	�����λΪ 1ʱ,ѡ���Դ�ӡ.
*	n ֻ�����λ��Ч.
*	��������������ַ�(����HRI�ַ�)��Ч. 
��	ѡ���Դ�ӡ��,��ESC SP ָ�����õ��ַ����Ҳ����. 
��	����ָ�Ӱ��λͼ,�Զ���λͼ,����,HRI�ַ��Լ���HT, ESC $, and ESC \
�趨�Ŀհ�. 
��	����ָ�Ӱ��������֮��Ŀհ�. 
��	�ڰ׷��Դ�ӡģʽ���»���ģʽ���ȼ���.�ںڰ׷��Դ�ӡģʽѡ��ʱ,�»���ģ
ʽ��������,ȡ���ڰ׷���ģʽ��,�趨���»���ģʽ��������. 
[ Ĭ��ֵ]		n = 0 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D42(void)
{
	g_tPrtCtrlData.CharNotFlag = ReadInBuffer() & 0x01;
}

/*******************************************************************************
* Function Name	: Command_1D44
* Description		: ѡ��/ȡ������ģʽ
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D44(void)
{	//����PC��������ģʽ,,=0Ϊ������ӡģʽ,=1ΪSP������ӡģʽ,=2ΪWH������ӡģʽ
	uint8_t	GetChar;
	
	GetChar =  ReadInBuffer() & 0x01;
	g_tPrtCtrlData.PCDriverMode =	GetChar;
}

/*******************************************************************************
* Function Name	: Command_1D45
* Description		: ���ô�ӡŨ��,������������
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D45(void)
{
	ReadInBuffer();
}

/*******************************************************************************
* Function Name	: Command_1D48
* Description		: ����HRI��ӡλ��
1D	 48	 n
0	��	 n	��	 3, 48 ��	 n	��	 51 
��ӡ����ʱ,ΪHRI�ַ�ѡ���ӡλ�� 
nָ�� HRI	��ӡλ��: 
	n		 ��ӡλ�� 
0, 48	����ӡ
1, 49	�����Ϸ�
2, 50	�����·�
3, 51	������,�·�����ӡ 
��	HRI	�Ƕ���������ע�͵��ַ�. 
��	HRI	�ַ�����������	GS f ָ����ָ��.
[ Ĭ��ֵ]		n = 0
* Input					: None
* Output			 : None
* Return				 : None
*******************************************************************************/
void Command_1D48(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if ((GetChar <= 3) || ((GetChar >= 48) && (GetChar <= 51)))
	{
		g_tBarCodeCtrl.HRIPosition = GetChar & 0x03;
	}	
}

/*******************************************************************************
* Function Name	: Command_1D49
* Description		: ��ѯ��ӡ��ID
1D	49	n 
1	��	n ��	3, 49	��	n ��	51, 65	��	n ��	69
��ѯ��ӡ�� ID ��,ID��������n ����: 
	n		��ӡ�� ID ��		���ز��� 		 		ID (hexadecimal) 
1,49	��ӡ��ģ��ID	 	SP-POS88 ϵ��		20 
2,50	��ӡ������ID		���±� 
3,51	ROM�汾ID				���ӡ���ͺż������й� 
65		Firmware�汾ID	���ӡ���ͺż������й� 
66		����						WHKJ 
67		��ӡ������			POS88V 
68		��ӡ������			���ӡ���ͺż������й� 
69		֧�ֺ�������		��������: CHINA GB2312 
											��������(�����ֿ�): CHINA GB18030 
											��������:TAIWAN BIG-5

n = 2,	��ӡ������ ID 
Bit	Off/On	Hex	Decimal 		Function 
0		OFF			00	0						��֧��˫�ֽ��ַ�����
0		ON			01	1						֧��˫�ֽ��ַ����� 
1		ON			02	2						���е�(�̶�) 
2		OFF			00	0						���� 
3		OFF			00	0						���� 
4		OFF			00	0						����,�̶�Ϊ0 
5		_				_		_						δ���� 
6		_				_		_						δ���� 
7		OFF			00	0						����,�̶�Ϊ0 

��	��1	��	n �� 3	��49 ��	n ��	51 ʱ, ��ӡ�����ص��ֽ�ID 
��	��65 ��	n ��	69,	���ظ�ʽ����: 
ͷ��Ϣ: Hexadecimal = 5FH / Decimal = 95 (1 byte) 
����:	��ӡ����Ϣ 
NUL: Hexadecimal = 00H / Decimal = 0 (1 byte)

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D49(void)
{
	uint8_t	ID[2];
	uint8_t	DummyData[1];
	uint8_t	i;
	uint8_t * p = NULL;
	
	i = ReadInBuffer();
	if(( i<=4 )|| ( 48<= i && i <=52)||(65 <= i && i <=69) || (0x6F <= i && i <=0x71))	
	{
		ID[0] = 0x5F;
		ID[1] = 0x00;
		DummyData[0] = '\0';	//��ֹpָ�����ط�
		p = DummyData;
		switch (i)
		{
			case 0:					//����0x00
		  case 48:
			  ID[0] = 0x00;		  
				break;
			case 1:					//��ӡ���ͺ�ID	0x20
			case 49:
				ID[0] = 0x20;
				break;
			case 2:					//bit7-bit2=0,bit1-�е�,bit0=0-��֧��˫�ֽ��ַ�����,=1-֧��˫�ֽ��ַ�����
			case 50:
			{
				ID[0] = 0;
				if (g_tSysConfig.SysLanguage > 0)
				 	ID[0] |= 0x01;
				if (g_tSysConfig.CutType)
				 	ID[0] |= 0x02;
				break;
			}
			case 3:										
			case 51:				//����ROM �汾 ID	���ݰ�����
				ID[0] = ROM_VERSION;
				break;
			case 4:					//����0x00�����ݰ�����
		  case 52:
			  ID[0] = 0x00;		  
				break;
			case 65:				//���ع̼��汾
				p = FIRMWARE_STRING;
				break;
		 	case 66:				//���س���	WHKJ
			{
				if(ManufacturerType ==1)
					p = "EPSON";
				else
					p = g_tSysConfig.Manufacturer;
				break;
		  }
			case 67:				//���ش�ӡ������
				p = g_tSysConfig.Model;
				break;
			case 68:				//���ش�ӡ�����к����ӡ���ͺż������й�
				p = g_tSysConfig.SerialNumber;
				break;
			case 69:				//���ش�ӡ��֧�ֺ�������, CHINA GB18030
				p = LanguageString[g_tSysConfig.SysLanguage];
				break;
			case 0x6f:	           	//����@FDH��EPSON OPOS�õ�
				p = "@FDH";    
				break;		 
		  case 0x70:	           	//����0��EPSON OPOS�õ�
				p = "0";    
				break;		 
		  case 0x71:	           	//����1��EPSON OPOS�õ�
				p = "1";
				break;		
			default:
				break;
		}
		
		UpLoadData(ID, 1);			//�ϴ�����
		i = strlen((const char *)p);
		if (i)
			UpLoadData(p, (i + 1));
		
		p = NULL;
	}
}

/*******************************************************************************
* Function Name	: Command_1D4C
* Description		: ������߾�,�ڱ�׼ģʽʱ,ֻ��������Ч,
ҳģʽʱֻ�����ڲ���־,��Ӱ���ӡЧ��
1D	 4C	 nL	nH
��߾�����Ϊ	[(nL + nH	�� 256) �� �����ƶ���λ)]
��	�ڱ�׼ģʽ��,������ֻ�������ײ���Ч. 
��	��ҳģʽ��,��������Ч,��ӡ���������������ͨ�ַ�����. 
��	�����Ӱ����ҳģʽ�µĴ�ӡ. 
��	������ó����������ô�ӡ���,��ȡ�����ô�ӡ��� 
��	����������ƶ���λ����	GS P�������õ�,�ı�����ͺ����ƶ���λ��Ӱ�쵱ǰ
����߾�.
[ Ĭ��ֵ]		nL = 0, nH = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D4C(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	GetChar = ReadInBuffer();
	//Temp = (GetChar + 256 * ReadInBuffer()) * g_tPrtCtrlData.StepHoriUnit;
	Temp = ((uint16_t)GetChar + (((uint16_t)(ReadInBuffer())) << 8)) * g_tPrtCtrlData.StepHoriUnit;
	
	if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//����
	{
		if (Temp > g_tSystemCtrl.LineWidth)	//�����������п�,������Ϊ����п�
		{
			Temp = g_tSystemCtrl.LineWidth;
		}
		
		//������������ɴ�ӡ�����ǲ���һ���ַ�������Ϊ�ܷ���һ���ַ�
		if (g_tSystemCtrl.CharacterSet == INTERNATIONAL_CHAR_SET_INDEX_ENGLISH)	//Ӣ��ģʽ
		{
			MiniCharWidth = g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth;	//��С�ַ����
		}
		else	//����ģʽ
		{
			MiniCharWidth = g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth;	//��С�ַ����
		}
		if (MiniCharWidth > g_tSystemCtrl.LineWidth)	//ȷ�������Ϸ�
		{
			MiniCharWidth = g_tSystemCtrl.LineWidth;
		}
		if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
		{
			g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//������ӡ����ȷ���ܷ���һ���ַ�
		}
		
		//��߾����ӡ���򳬹����ɴ�ӡ����,��Ҫ������ӡ���������߾�,���������������
		if ((Temp + g_tPrtCtrlData.PrtWidth) > g_tSystemCtrl.LineWidth)
		{
			g_tPrtCtrlData.PrtWidth = g_tSystemCtrl.LineWidth - Temp;	//������ӡ����
			if (g_tPrtCtrlData.PrtWidth < MiniCharWidth)
			{
				g_tPrtCtrlData.PrtWidth = MiniCharWidth;	//������ӡ����ȷ���ܷ���һ���ַ�
			}
			Temp = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtWidth;	//������߾�
		}
		
		g_tPrtCtrlData.PrtLeftLimit = Temp;
		g_tPrtCtrlData.PrtDataDotIndex = Temp;
		g_tPrtCtrlData.PrtLength = Temp + g_tPrtCtrlData.PrtWidth;
	}
}

/*******************************************************************************
* Function Name	: Command_1D50
* Description		: ���ú���,�����ƶ���λ
1D	 50	 x	y
��	�ֱ𽫺����ƶ���λ�������ó�25.4/x mm(1/xӢ��)�����ƶ���λ���ó�25.4/y mm(1/yӢ��). 
��	�� x �� y Ϊ0ʱ, x �� y�����ó�Ĭ��ֵ.
��	��ֱ�ڽ�ֽ����Ϊ����,��ֽ����Ϊ����. 
��	�ڱ�׼ģʽ��,����������x����y,��ʹ�ַ���ת(���û���˳ʱ����ת90��Ҳ���ı䣻 
		�� �� x ������:	ESC SP, ESC $, ESC \, FS S, GS L, GS W 
		�� �� y ������:ESC 3, ESC J, GS V 
��	ҳģʽ��,�� x ���� y Ҫ����������ʹ�ӡ��ʼλ������: 
		��	����ӡ��ʼλ����ESC T �������ó����Ͻ�(��ӡ���������)�������½�
(��ӡ������ҵ���)ʱ: 
		�� x ������:	ESC SP, ESC $, ESC W, ESC \, FS S 
		�� y ������:	ESC 3, ESC J, ESC W, GS $, GS \, GS V 
		��	����ӡ��ʼλ����ESC T �������ó����Ͻ�(��ӡ������ϵ���)�������½�
(��ӡ������µ���)ʱ:	
		�� x ������:	ESC 3, ESC J, ESC W, GS $, GS \
		�� y ������:	ESC SP, ESC $, ESC W, ESC \,FS S, GS V 
��	�����Ӱ����ǰǰ�趨����������. 
��	��С�ƶ���λ�����ɸ���������������ۺ����õĽ��. 
��	һӢ�����25.4mm. .
[ Ĭ��ֵ]		x = 203, y = 203 ,��ʱһ���ƶ���λ����һ����ӡ��.�������Ϊ1/8mm,
�������Ϊ1/8mm. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D50(void)
{
	uint8_t	GetChar;
	double	dTemp;
	
	//���ú����ƶ���λ
	GetChar = ReadInBuffer();
	if (GetChar == 0)
	{
		g_tPrtCtrlData.StepHoriUnit = DEFAULT_STEP_HORI_UNIT;
	}
	else
	{
		dTemp = 25.4 / MIN_STEP_UNIT_X / GetChar;
		if (dTemp < 1.0)
		{
			g_tPrtCtrlData.StepHoriUnit = 1;
		}
		else
		{
			g_tPrtCtrlData.StepHoriUnit = (uint16_t)dTemp;
			if ((dTemp - (uint16_t)dTemp) >= 0.5)
			{
				g_tPrtCtrlData.StepHoriUnit++;
			}
		}
	}
	
	//���������ƶ���λ
	GetChar = ReadInBuffer();
	if (GetChar == 0)
	{
		g_tPrtCtrlData.StepVerUnit = DEFAULT_STEP_VER_UNIT;
	}
	else
	{
		dTemp = 25.4 / MIN_STEP_UNIT_Y / GetChar;
		if (dTemp < 1.0)
		{
			g_tPrtCtrlData.StepVerUnit = 1;
		}
		else
		{
			g_tPrtCtrlData.StepVerUnit = (uint16_t)dTemp;
			if ((dTemp - (uint16_t)dTemp) >= 0.5)
			{
				g_tPrtCtrlData.StepVerUnit++;
			}
		}
	}
}

#if	0
/*******************************************************************************
* Function Name	: Command_1D51
* Description		: ���������һ�п�ʼ����ӡλ�õľ���ΪN��ˮƽ���,Ĭ��Ϊ0,
*	��ָ��Ϊ�ٴﶨ��
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D51(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	
	giCodebarPositionSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
}
#else
/*******************************************************************************
* Function Name	: Command_1D51
* Description		: ��ӡ�ɱ�߶�λͼ,��ָ��ΪESC/POS����
*	Print variable vertical size bit image
	Hex 1D 51 30 m xL xH yL yH d1...dk
	0  ��   m  ��  3, 48 ��   m  ��  51
	1  ��  (xL + xH �� 256)  ��  4256 (0 �� xL �� 255, 0 �� xH �� 12)
	1  ��  (yL + yH �� 256)  ��  16 (1 �� yL �� 16, yH = 0)
	0  ��   d  ��  255
	k = (xL + xH ��  256)  ��  (yL + yH ��  256)
Prints a variable vertical size bit image using the  mode specified by m, as follows:
[ ����]	 ����m ȷ����ģʽ��ӡλͼ.
	m					ģʽ		����ֱ��� (DPI)	����ֱ��� (DPI)
0, 48		����						203									203
1, 49		����						203									101
2, 50		����						101									203
3, 51		����,����			101									101
xL,xH specifies(xL + xH �� 256) dots in horizontal direction for the bit image.
yL,yH specifies(yL + yH �� 256) bytes in vertical direction for the bit image.
d  specifies the bit image data (column format).
k indicates the number  of the bit image data. k is an explanation parameter ; 
therefore, it does not need to be transmitted.
�� xL, xH ��ʾ�����ֵ���Ϊ(xL + xH �� 256) .
�� yL, yH ��ʾ�����ֽ���Ϊ(yL + yH �� 256) .
�� d ��ʾָ��������.
Y  = (yL + yH  ��  256)
The relationship between bit	image data and the print re sult is as follows:
d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define	CMD_1D5130_DOT_XH_PAR_MAX			(12)
#define	CMD_1D5130_BYTE_YL_PAR_MIN		(1)
#define	CMD_1D5130_BYTE_YL_PAR_MAX		(16)
#define	CMD_1D5130_BYTE_YL_PAR_RATIOY_MAX	(12)
void Command_1D51(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX, RatioY;
	uint8_t	ratiox, ratioy;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint8_t	* pbByteStart = NULL;
	uint8_t	* pbTemp = NULL;
	uint8_t	* pbColumn = NULL;
	uint16_t X_Length, Y_Length, MaxX_width;
	uint16_t	x, y;
	uint16_t	SpaceDot;
	
	if (ReadInBuffer() != 0x30)
		return;
	
	if ((g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit) || \
			(g_tSystemCtrl.PrtModeFlag != STD_MODE)) //ֻ�ڱ�׼ģʽ���������ײ���Ч
		return;
	
	GetChar = ReadInBuffer();
	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m������Ч����
		return;
	
	RatioX = (GetChar & 0x01) + 1;					//��ȱ���,���Ϊ2
	RatioY = ((GetChar & 0x02) >> 1) + 1;		//�߶ȱ���,���Ϊ2
	
	GetChar = ReadInBuffer();
	X_Length = GetChar;					//�����ֵ�����λ
	GetChar = ReadInBuffer();		//�����ֵ�����λֵ
	if (GetChar <= CMD_1D5130_DOT_XH_PAR_MAX)
		X_Length += (uint16_t)(((uint16_t)GetChar) << 8);		//�����ȵ���ֵ
	else
		return;
	
	GetChar = ReadInBuffer();
	Y_Length = GetChar;					//�����ֽ�����λ
	GetChar = ReadInBuffer();		//�����ֽ�����λֵ
	if ((Y_Length >= CMD_1D5130_BYTE_YL_PAR_MIN) && \
			(Y_Length <= CMD_1D5130_BYTE_YL_PAR_MAX) && \
			(GetChar == 0x00))
		Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);		//�����ֽ���Ŀ
	else
		return;
	//��Ϊ������������һ���Ե�,Ŀǰ�Ĵ�ӡ���������Ϊ24�ֽڸ�,
	//�����ڷŴ�����������³���12���޷�����.
	//������ֵΪ����12,��ôֻ����������Ŵ���.
	if ((Y_Length > CMD_1D5130_BYTE_YL_PAR_RATIOY_MAX) && (RatioY > 1))
		RatioY = 1;
	
	x = (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit);	//����ɴ�ӡ������
	if (X_Length * RatioX > x)
		MaxX_width = x / RatioX;
	else
	 	MaxX_width = X_Length;
	
	SpaceDot = g_tPrtCtrlData.PrtLeftLimit;
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)			//�ַ�����ģʽ,����
		SpaceDot += ((x - MaxX_width * RatioX) >> 1);
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//����
		SpaceDot += (x - MaxX_width * RatioX);
	
	//ȷ�����������༭��������С,һ�δ�ӡ���
	if (MAX_PRT_HEIGHT_BYTE >= Y_Length * RatioY)
	{	//���м���������ʼָ��,ÿ������1��,�ı���ֵ
		pbByteStart = g_tLineEditBuf + SpaceDot + \
				(MAX_PRT_HEIGHT_BYTE - (Y_Length * RatioY)) * PRT_DOT_NUMBERS;
		for (x = 0; x < MaxX_width; x++)	//������,�Ե�Ϊ��λ,��ֵ
		{
			//ָ��ǰ�༭���ݻ�������ʼ�ֽ���,ÿ����������1���ֽ�,�ı���ֵ
			pbColumn = pbByteStart;
			for (y = 0; y < Y_Length; y++)	//����߶�,���ֽ�Ϊ��λ
			{
				GetChar = ReadInBuffer();
				if (GetChar)				//���ݲ�Ϊ0ʱȫ����
				{
					if (RatioY > 1)		//��Ҫ�Ŵ�
					{
						memset(TempBuf, 0x00, MAX_RATIO_Y);
						//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
						ByteZoomCtrl(RatioY, GetChar, TempBuf);
					}
					else		//����Ŵ���Ϊ1
					{
						TempBuf[0] = GetChar;
					}
					pbTemp = pbColumn;
					for (ratiox = 0; ratiox < RatioX; ratiox++)
					{
						for (ratioy = 0; ratioy < RatioY; ratioy++)
						{
							*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
						}
						pbTemp++;
					}
				}	//һ����Ϊ0�ֽ�������,����Ϊ0ʱֱ���޸�ָ��
				pbColumn += (RatioY * PRT_DOT_NUMBERS);
			}	//1������߶��ֽ�������
			pbByteStart += RatioX;		//������ʼָ������һ�����ֵ
		}		//�ɴ�ӡ��Χ�ڵĴ�ӡ����������
		
		g_tPrtCtrlData.PrtDataDotIndex += (SpaceDot + MaxX_width * RatioX);
		g_tPrtCtrlData.BitImageFlag = 1; 	//λͼ��ӡ��־
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((Y_Length * RatioY) << 3);	//�����ܵ���
		GetChar = g_tPrtCtrlData.LineDirection;
		g_tPrtCtrlData.LineDirection = PRINT_DIRECTION_NORMAL;	//����ģʽ������Ч
// 		PrintOneLine();										//��ӡ�ñ༭������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		//��ȡ���ܵĶ��������
		for (x = MaxX_width; x < X_Length; x++)	//������,�Ե�Ϊ��λ,��ֵ
		{
			for (y = 0; y < Y_Length; y++)	//����߶�,���ֽ�Ϊ��λ
			{
				ReadInBuffer();
			}
		}
		if (GetChar)
		{
			WaitPreviousPrintFinish();
			g_tPrtCtrlData.LineDirection = GetChar;	//�ָ����ô�ӡ����ģʽ����
		}
	}	//һ�δ�ӡ�༭���
}
#endif

/*******************************************************************************
* Function Name	: Command_1D53
* Description		: Ϊ�˼���GP����  ִ���е�����ͬ1D5642
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D53(void)
{
	if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
		return;
	GoDotLine(CUT_OFFSET);									//�ߵ���ֽλ�� 2016.08.04
	DriveCutPaper(CUT_TYPE_PART);
	#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
		GoDotLine(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
	#endif

}
/*******************************************************************************
* Function Name	: Command_1D55
* Description		: Ϊ�˼���GP����
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D55(void)
{
	ReadInBuffer();
	ReadInBuffer();
}

/*******************************************************************************
* Function Name	: Command_1D56
* Description		: �����е���ִ����ֽ
1D	 56	 m 
�� m = 0,48,1 ,	49

1D	56	 m	n 
�� m = 65,66, 0 �� n	�� 255
����m ��ֵѡ����ֽģʽ,������ʾ: 
//�˴�������д���,��Ҫ����EPSONָ������޸�
		M	 				��ֽģʽ 
0,1,48,49		����
65,66					��ֽ(��ֽλ��+[n	�� (�����ƶ���λ)Ӣ��] )���Ұ���ֽ

��	��������ֻ����������Ч.
��	m = 0,48,1 ,	49,��ӡ��ֱ����ֽ. 
��	��m = 65,66,��ӡ����ֽ[��ӡλ�õ��е�֮����� + n �� �����ƶ���λ]Ȼ����ֽ. 
��	�����ƶ���λ�������ƶ���λ����GS P �������õ�.
��	��ֽ���������ƶ���λ������.

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D56(void)
{
	static uint8_t CutPaperMode = 4;	//=0,���в���ֽ,��3����,��4����,��5 ȫ��
	uint8_t	GetChar;
	uint32_t i;
	
	GetChar = ReadInBuffer();
	
	switch (GetChar)
	{
		case 0x63:			 					//��������,������ֽ��ʽ
		{	//2016-8-9���,�������ʽΪ1D 56 63 m n
			CutPaperMode = ReadInBuffer();
			ReadInBuffer();					//��ȡ���õ���ֽ����
			return;
		}
		case 0x6D:								//��������,ִ����ֽ
		{
			if (CutPaperMode == 3)
			{
					GoDotLine(CUT_OFFSET);		//�ߵ���ֽλ��
			}
			else if (CutPaperMode == 4)
			{
				GoDotLine(CUT_OFFSET);		//�ߵ���ֽλ��
				DriveCutPaper(CUT_TYPE_PART);			//����
				#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
					KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ 2016.08.02
				#endif
			}
			else if (CutPaperMode == 5)
			{			
				GoDotLine(CUT_OFFSET);		//�ߵ���ֽλ��
				DriveCutPaper(CUT_TYPE_FULL);			//ȫ��
				#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
					KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
				#endif
			}
			break;
		}
		case 0:			 						//EPSON ȫ��ֽ��ʽ
		case 48:
		{	//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		case 1:			 						//EPSON ����ֽ��ʽ
		case 49:		 
		{	//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		case 65:			 					//EPSON ��ֽ��ȫ��ֽ��ʽ
		{
			GetChar = ReadInBuffer();
			//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			GoDotLine(CUT_OFFSET + GetChar * g_tPrtCtrlData.StepVerUnit);	//�ߵ���ֽλ��			
			DriveCutPaper(CUT_TYPE_FULL);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		case 66:			 					//EPSON ��ֽ�����ֽ��ʽ
		{
			GetChar = ReadInBuffer();
			//�е�ָ��ֻ�������ײ���Ч
			if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)
				return;
			GoDotLine(CUT_OFFSET + GetChar * g_tPrtCtrlData.StepVerUnit);	//�ߵ���ֽλ��
			DriveCutPaper(CUT_TYPE_PART);
			#ifdef	AUTO_FEED_AFTER_CUT_ENABLE
				KeyFeedPaper(AUTO_FEED_AFTER_CUT_STEPS);	//�Զ���ֽ
			#endif
			break;
		}
		default:
		{
			return;
		}
	}
	#ifdef	PAPER_OUT_RE_PRINT_ENABLE
	if (g_tSysConfig.PaperOutReprint)
	{
		ClearInBuf();				//���Ѿ���ӡ��ɵĵ�������
	}
	#endif
}

/*******************************************************************************
* Function Name	: Command_1D57
* Description		: ���ô�ӡ������,�ڱ�׼ģʽʱ,ֻ��������Ч,
ҳģʽʱֻ�����ڲ���־,��Ӱ���ӡЧ��
1D	57	nL	nH 
����ӡ����������Ϊ[(nL + nH	��	256) �� �����ƶ���λ)] Ӣ��
���ڱ�׼ģʽ��,���������������Ч. 
����ҳģʽ�´�������Ч,�������ݱ�������ͨ�ַ�����. 
�������Ӱ��ҳģʽ�µĴ�ӡ. 
�����[��߾�+��ӡ�����]�����ɴ�ӡ����,���ӡ������Ϊ�ɴ�ӡ�����ȼ�ȥ��߾�.
������������ƶ���λ��GS P��������.�ı����������ƶ���λ��Ӱ�쵱ǰ��߾��������.
�������ӡ�����ʹ�ú����ƶ���λ.
[ Ĭ��ֵ]		nL = 64, nH = 2 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D57(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	uint16_t	MiniCharWidth;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		//Temp = GetChar + 256 * ReadInBuffer();
		Temp = (uint16_t)GetChar + (((uint16_t)(ReadInBuffer())) << 8);
		Temp *= g_tPrtCtrlData.StepHoriUnit;
		
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)	//����
		{
			if (g_tPrtCtrlData.PrtLeftLimit > g_tSystemCtrl.LineWidth)
			{
				g_tPrtCtrlData.PrtLeftLimit = g_tSystemCtrl.LineWidth;
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
			}
			
			//����������ɴ�ӡ����
			if ((Temp + g_tPrtCtrlData.PrtLeftLimit) > g_tSystemCtrl.LineWidth)
			{
				Temp = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtLeftLimit;
			}
			
			//������Ƿ��ܹ�����һ���ַ�, ������չ�ɴ�ӡ����ֵ
			if (g_tSystemCtrl.CharacterSet == INTERNATIONAL_CHAR_SET_INDEX_ENGLISH)	//Ӣ��ģʽ
			{
				MiniCharWidth = g_tPrtModeChar.LeftSpace + g_tPrtModeChar.CharWidth;	//��С�ַ����
			}
			else	//����ģʽ
			{
				MiniCharWidth = g_tPrtModeHZ.LeftSpace + g_tPrtModeHZ.CharWidth;	//��С�ַ����
			}
			if (MiniCharWidth > g_tSystemCtrl.LineWidth)	//ȷ�������Ϸ�
			{
				MiniCharWidth = g_tSystemCtrl.LineWidth;
			}
			
			if (Temp < MiniCharWidth)	//ȷ���ܷ���һ���ַ�
			{
				Temp = MiniCharWidth;
			}
			
			g_tPrtCtrlData.PrtWidth = Temp;		//�������õĴ�ӡ������
			//��������߾��ֹԽ��, ���ܳ�Խ���õ�����п�
			if ((g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth) > g_tSystemCtrl.LineWidth)
			{
				g_tPrtCtrlData.PrtLeftLimit = g_tSystemCtrl.LineWidth - g_tPrtCtrlData.PrtWidth;
				g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;
			}
			g_tPrtCtrlData.PrtLength = g_tPrtCtrlData.PrtLeftLimit + g_tPrtCtrlData.PrtWidth;
		}
	}
}

/*******************************************************************************
* Function Name	: Command_1D5A
* Description		: ѡ���ά��������
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D5A(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	if(GetChar <3)	
    {    
		g_tRamBuffer.BarCode2DStruct.BarCode2DType = GetChar;
    }
}

/*******************************************************************************
* Function Name	: Command_1D5E
* Description		: ִ�к궨�� 
1D	 5E	 r t	m 
0	��	 r	��	 255 
0	��	 t	��	 255 
m =	0, 1
ִ�к�����. 
��	r	ָ��ִ�к�Ĵ���. 
��	t	ָ��ִ�к�ĵȴ�ʱ��. 
��	m ָ��ִ�к��ģʽ. 
	�� m	�����λΪ0ʱ: 
	����t��100msΪ���ʱ��,����ִ��r��. 
	�� m	�����λΪ1ʱ
	��ӡ���ȴ�t��100msʱ���,ָʾ����˸,ֱ���û����½�ֽ��,��ӡ����ִ�к�.���ѭ��r��. 
��	ÿ��ִ�к�ĵȴ�ʱ��Ϊ	t	��	100 ms. 
��	����ں궨��ʱ���յ�����������궨��ֹͣ,���ڶ���ĺ걻���.	
��	�����û�ж������ r ��0 ,���������Ч. 
��	����ִ��ʱ( m = 1) ,�����ý�ֽ����ֽ. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D5E(void)
{
	uint8_t	i;
	uint8_t	j;
	uint8_t	MacroRunCount;
	uint8_t	MacroRunDelayRatio;
	uint8_t	MacroRunMode;
	
	if (g_tMacro.DefinitionFlag)	//�궨����,�˳��궨��,�����
	{
		g_tMacro.Number = 0;				//���ַ���������
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//�����ı�־����
		g_tMacro.RunFlag = 0;
	}
	
	MacroRunCount				= ReadInBuffer();		//���к�Ĵ���
	MacroRunDelayRatio	= ReadInBuffer();		//���к����ʱʱ�䱶��
	MacroRunMode				= ReadInBuffer();		//��ģʽ		
	if ((g_tMacro.SetFlag == 1) && (MacroRunCount != 0))	//���û�к궨��,���������,��ָ����Ч
	{
		g_tMacro.RunFlag = 1;									//��ʼִ�к궨���ʶ
		for (i = 0; i < MacroRunCount; i++)		//ѭ������
		{
			DelayMs(MacroRunDelayRatio * 100);	//�ȴ�ʱ��
			if (MacroRunMode & 0x01)	//��Ҫ���°���ʱ
			{
				j = 0;
				while (FEED_BUTTON_STATUS_OPEN == ReadFeedButtonStatus())	//�ȴ���������
				{
					DelayMs(10);
					j++;
					if (j < 50)					//ָʾ����˸
					{
						PaperEndLedOn();
					}
					else
					{
						PaperEndLedOff();
						if (j > 100)
							j = 0;
					}
					
					g_tFeedButton.KeyStatus = 0;	//������Ч
					g_tFeedButton.KeyCount = 0;		//���֮ǰ���еļ���
				}
				PaperEndLedOff();
			}
			
			g_tMacro.GetPointer = 0;	//ִ�к�
			while (g_tMacro.GetPointer < g_tMacro.Number)
			{
				ProcessCharMaskReadBuf();
			}
		}
		
		g_tMacro.RunFlag = 0;		//��ִ�����
	}	
}

/*******************************************************************************
* Function Name	: Command_1D61
* Description		: ʹ��-ʧ���Զ�״̬����
1D	 61	 n
ȷ���Զ�״̬���ص�����,����n ��������:
Bit		Off/On	Hex	 Decimal	 ASB���״̬ 
0			off 	 	00	 	0					Ǯ���״̬ʹ�� 
0			on			01	 	1					Ǯ���״̬��Ч
1			off 	 	00	 	0					����/����״̬ʹ�� 
1			on 	 		02	 	2					����/����״̬��Ч 
2			off 	 	00	 	0					����״̬ʹ�� 
2			on 	 		04	 	4					����״̬��Ч 
3			off 	 	00	 	0					ֽ״̬ʹ�� 
3			on 	 		08	 	8					ֽ״̬��Ч 
4-7		-				-			-					δ����

��	����ϱ����κ�һ�����ݴ���ʹ��״̬,����״̬�ı�ʱ��ӡ���Զ������ĸ� 
		 �ֽڵĴ�ӡ��״̬ 
��	���������Ŀ��������Ч״̬,��ASB������Ч 
��	��ӡ���Զ�����״̬�ֽ�ʱ,��ȷ����������Ƿ�׼���� 
��	����������������һ��˳��ִ��,���Դӷ��͸�����Զ�״̬����������Ч 
��һ��ʱ���ӳ� 
��	��ʹ��ӡ����ESC = ��������Ϊ��Ч״̬,	��ӡ���԰����ý����Զ�״̬���� 
��	�Զ����ص�״̬��Ϣ����: 
��һ�ֽ�(��ӡ����Ϣ) 

* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
//�Ͱ��ֽ��κ�һ�����ݴ���ʹ��״̬,����״̬�ı�ʱ,��ӡ���Զ������ĸ��ֽڵĴ�ӡ��״̬
void Command_1D61(void)
{
	g_tError.ASBAllowFlag = ReadInBuffer() & 0x0F;
	if (g_tError.ASBAllowFlag)							//�Զ�״̬����ʹ��
	{
		UpLoadData(g_tError.ASBStatus, 4); 		//����״̬��
	}
}

/*******************************************************************************
* Function Name	: Command_1D66
* Description		: ����HRI��ӡ����
1D	 66	 n
n	= 0,	1, 48, 49
��ӡ����ʱ,Ϊ HRI	�ַ�ѡ��һ������ 
�� n	��ѡ����������: 
	n		���� 
0,48	��׼ASCII�ַ� (12	 ��	24) 
1,49	ѹ��ASCII�ַ� (9	��	17) 

��	HRI	�ַ��Ƕ���������ע�͵��ַ�. 
��	HRI	�ַ��Ĵ�ӡλ����	GS H	����ָ��.
[ Ĭ��ֵ]		n = 0
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D66(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if ((GetChar <= 1) || ((GetChar >= 48) && (GetChar <= 49)))
	{
		g_tBarCodeCtrl.HRIFont = GetChar & 0x01;
	}
}

/*******************************************************************************
* Function Name	: Command_1D6730
* Description		: ��ʼ��ά��������
1D	67	 30	m nL nH
m = 0 
(nL + nH �� 256) = 20, 21, 50, 70		(nL = 20, 21, 50, 70, nH = 0)
������(nL+nH �� 256 )ָ���Ŀ�����ά����������0
nL + nH �� 256 
ʮ������	ʮ���� 	ά���������۵�λ��
	14			20			��ֽ�������У� 
	15			21			��ӡͷ���ȴ����۴Σ� 
	32			50			�е����������۴Σ� 
	46			70			��ӡ������ʱ���Сʱ��
��	Ƶ��дִ��дNV������ָ����ܻ���NV������,��˽���ÿ��дNV��������Ҫ����10��. 
��	�����ִ�и�ָ��ʱ,�رմ�ӡ����Դ��ͨ���ӿڸ�λ��ӡ������ʹ��ӡ�����벻����״̬,
	�����ִ�и�ָ��ʱ��Ҫ�رմ�ӡ����Դ��ӽӿڸ�λ��ӡ��. 
��	��ӡ����ִ�и�ָ����NV������д������ʱ,��ӡ�����Ϊ��æ������ֹͣ��������,���
	����ӡ����æ��ʱ,������Ҫ�ٷ�������. 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D6730(void)
{
	uint8_t	GetChar;
	uint16_t	Temp;
	
	GetChar = ReadInBuffer();
	if (GetChar != 0)
		return;
	GetChar = ReadInBuffer();
	Temp = GetChar + (uint8_t)(((uint16_t)(ReadInBuffer())) << 8);
	//��׼ģʽ��ֻ����������Ч
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		if (g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)
		{
		#ifdef	MAINTAIN_ENABLE
			switch (Temp)
			{
				case 0x14:						//��ʼ���м�����
				{
					R_Counter.LineCounter = 0;
					break;
				}
				case 0x15:						//���ȴ���
				{
					R_Counter.HeatCounter = 0;
					break;
				}
				case 0x32:						//�е�����
				{
					R_Counter.CutCounter = 0;
					break;
				}
				case 0x46:						//��ӡ������ʱ��,Сʱ
				{
					R_Counter.TimeCounter = 0;
					break;
				}
				default:
				{
					return;
				}
			}
		#endif
			
			SetComInBufBusy();
		
		#ifdef	MAINTAIN_ENABLE
			WriteFlashParameter(2);				//дCPU_FLASHά��������
		#endif
			ClearComInBufBusy();
		}	//�����ײ�ִ��
	}	//��׼ģʽ��
}

/*******************************************************************************
* Function Name	: Command_1D67
* Description		: ά������������ָ��
1D	67	32	m	nL nH
m = 0
(nL + nH �� 256) = 20, 21, 50, 70 , 148 , 149, 178, 198 
(nL = 20, 21, 50, 70, , 148 , 149, 178, 198 , nH = 0) 
������(nL+nH �� 256 )ָ����ά����������ֵ
nL + nH �� 256 
ʮ������	ʮ����	ά���������۵�λ��			����������
	14			20			��ֽ�������У�					
	15			21			��ӡͷ���ȴ����۴Σ�		�����õ�
	32			50			�е����������۴Σ�			(������)
	46			70			��ӡ������ʱ���Сʱ��	
	94			148			��ֽ�������У�					
	95			149			��ӡͷ���ȴ����۴Σ� 		�ۻ���
	B2			178			�е����������۴Σ�			
	C6			198			��ӡ������ʱ���Сʱ��	
��	ά����������ֵ��һ����ֵ,������ֵ���ܶ�ʱ���,�����ص�Դ��ʲôʱ��ص�ԴӰ���. 
��	��ִ�и�ָ��ʱ,���������յ���������ǰ,��Ҫ������������.
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D6732(void)
{
	uint8_t	GetChar, DataBuf[16];
	uint32_t	iTemp;
	uint8_t	* p = NULL;
		
	DataBuf[1] = '\0';
	
	GetChar = ReadInBuffer();
	if (GetChar != 0)
		return;
	
	p = DataBuf;
	*p++ = 0x5F;
	GetChar = ReadInBuffer();
	//switch (GetChar + 256 * ReadInBuffer())
	#ifdef	MAINTAIN_ENABLE
	switch (GetChar + (uint8_t)(((uint16_t)(ReadInBuffer())) << 8))
	{
		case 0x14:						 //�����м�����
		{
			iTemp = R_Counter.LineCounter;
			break;
		}
		case 0x15:						 //���ͼ��ȼ�����
		{
			iTemp = R_Counter.HeatCounter;
			break;
		}
		case 0x32:						 //�����е�������
		{
			iTemp = R_Counter.CutCounter;
			break;
		}
		case 0x46:						 //����ʱ�������,���ʱ���Է���Ϊ��λ,���͸�����ΪСʱ
		{
			iTemp = R_Counter.TimeCounter / 60;
			break;
		}
		case 0x94:						 //�����м�����
		{
			iTemp = C_Counter.LineCounter;
			break;
		}
		case 0x95:						 //���ͼ��ȼ�����
		{
			iTemp = C_Counter.HeatCounter;
			break;
		}
		case 0xB2:						 //�����е�������
		{
			iTemp = C_Counter.CutCounter;
			break;
		}
		case 0xC6:						 //����ʱ�������
		{
			iTemp = C_Counter.TimeCounter / 60;
			break;
		}
		default:
		{
			return;				
		}
	}
	#endif
	
	LongToStr(iTemp, p);
	SetComInBufBusy();
	UpLoadData(DataBuf, strlen((const char *)DataBuf) + 1);
	ClearComInBufBusy();
}

/*******************************************************************************
* Function Name	: Command_1D67
* Description		: ά������������ָ��
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D67(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar == 0x30)			//1D 67 30,��ʼ��ά��������ֵ
	{
		Command_1D6730();
	}
	else if (GetChar == 0x32)	//1D 67 32, ����ά��������
	{
		Command_1D6732();
	}
}

/*******************************************************************************
* Function Name	: Command_1D68
* Description		: ��������߶�
1D	 68	 n 
1	��	 n	��	 255
����߶�Ϊ n	��
[ Ĭ��ֵ]		n = 162
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D68(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	if (GetChar)
		g_tBarCodeCtrl.BarCodeHigh = GetChar;
}

/*******************************************************************************
* Function Name	: Command_1D6B
* Description		: ��ӡ����
1D	 6B	 m	d1...d k	00
��0	��	 m ��	 6	(k ��d��ȡֵ��Χ������������������)

1D	 6B	 m	n d1... dn
��65 ��	 m ��	 73	(k ��d ��ȡֵ��Χ������������������)

ѡ��һ���������Ͳ���ӡ���� 
m ����ѡ����������,������ʾ: 
m	��������							�ַ�����								d
0	UPC-A					11 �� k �� 12 			48 �� d �� 57
1	UPC-E					11 �� k �� 12 			48 �� d �� 57
2	JAN13 (EAN13)	12 �� k �� 13 			48 �� d �� 57 
3	JAN 8 (EAN8)		7	�� k �� 8				48 �� d �� 57 
4	CODE39	 			1	�� k �� 255			45 �� d �� 57, 65 �� d �� 90,32,36,37,43 
5	ITF	 					1	�� k �� 255			48 �� d �� 57 
6	CODABAR	 			1	�� k �� 255			48 �� d �� 57, 65 �� d �� 68,36,43,45,46,47,58

65	UPC-A					11 �� n �� 12 			48 �� d �� 57 
66	UPC-E					11 �� n �� 12 			48 �� d �� 57 
67	JAN13 (EAN13) 12 �� n �� 13				48 �� d �� 57 
68	JAN 8 (EAN8) 	7	�� n �� 8				48 �� d �� 57 
69	CODE39	 			1	�� n �� 255			45 �� d �� 57, 65 �� d �� 90,32,36,37,43,d1=dk=42
70	ITF	 				1	�� n �� 255			48 �� d �� 57 
71	CODABAR	 		1	�� n �� 255	 		48 �� d �� 57, 65 �� d �� 68,36,43,45,46,47,58
72	CODE93	 			1	�� n �� 255	 		0	�� d �� 127	
73	CODE128				2	�� n �� 255	 		0	�� d �� 127 

��	�����������ָ�ʽ���� NULL	����. 
��	��ѡ��UPC-A ����UPC-E��ʱ,��ӡ�����յ�12�ֽ��������ݺ�,ʣ����ַ���������ͨ�ַ�����. 
��	��ѡ��JAN13 (EAN13)����ʱ,��ӡ�����յ�13�ֽ��������ݺ�,ʣ����ַ���������ͨ�ַ�����. 
��	��ѡ��JAN8 (EAN8),����ʱ,��ӡ�����յ�8�ֽ��������ݺ�,ʣ����ַ���������ͨ�ַ�����.
��	ITF�����ݸ���������ż��.���������������������,�����һ�����ݱ�����.
��	n����ָʾ�������ݵĸ���,��ӡ��������n�ֽ�������Ϊ�������ݴ���.
��	���n�����˹涨�ķ�Χ,��ӡ����������������,������������Ϊ��ͨ���ݴ���.

(��׼ģʽ):
��	�����������d �����˹涨�ķ�Χ,��������Ч. 
��	���������򳬳��˴�ӡ����,��Ч. 
��	�����������ESC 2 ��ESC 3�������õ��и��Ƕ���,��ֽ���붼���趨������߶����. 
��	��������ֻ���ڴ�ӡ������û������ʱ����Ч,�����ӡ������������,���������. 
��	��ӡ�����,����ӡλ������������. 
��	��ӡģʽ����(��Ӵ�,˫�ش�ӡ,�»���,�ַ���С,��ɫ�Լ��ַ�˳ʱ����ת90���)
��Ӱ����������,���ǵ���ģʽ�������ӡ��Ӱ��. 

(ҳģʽ):
��	��������ֻ������ͼ�����ɵ���ӡ������,���ǲ�����ӡ.�������������ݺ󽫴�
ӡλ���Ƶ�������ұ�. 
��	��� d	�����˹涨�ķ�Χ,�����������. 
��	��������ȳ����˴�ӡ����,���������. 


* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
#define		VELO_COMMAND

void Command_1D6B(void)
{
	uint8_t	GetChar;
	uint8_t	Number;
	uint8_t	Temp;
	uint8_t	MaxDataLen;
	uint8_t	Inbuf[255];
	uint8_t	Hribuf[255];
	uint16_t Width;
	
	//if (g_tPrtCtrlData.PrtDataDotIndex != g_tPrtCtrlData.PrtLeftLimit)	//�������� 2016.08.10  ȥ��
	//	return;
	
	GetChar = ReadInBuffer();		//ģʽm
	if ((GetChar >= 32 && GetChar <= 34) || (GetChar >= 97 && GetChar <= 99))
	{
		ProcessBarCode_2D(GetChar); 	//��ά���봦�� 
		return;
	}
	
	Width = 0;
	switch (GetChar)
	{
		case	0:				//UPC-A
			MaxDataLen = 12;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if ((Number == 11) || (Number == 12))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeUpc_A( Inbuf );
				Width = FormatCodeUPCA(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	65:				//UPC-A
		{
			Number = ReadInBuffer();
			if ((Number == 11) || (Number == 12))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeUpc_A( Inbuf );
					Width = FormatCodeUPCA(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	1:		//UPC-E
			MaxDataLen = 12;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if (((Number >= 6) && (Number <= 8)) || (Number == 11) || (Number == 12))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCodeUpc_E( Inbuf );
				//Width = FormatCodeUPCE(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	66:		//UPC-E
		{
			Number = ReadInBuffer();
			if (((Number >= 6) && (Number <= 8)) || (Number == 11) || (Number == 12))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCodeUpc_E( Inbuf );
					//Width = FormatCodeUPCE(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	2:		//EAN13
			MaxDataLen = 13;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if ((Number == 12) || (Number == 13))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeEan13( Inbuf );
				Width = FormatCodeEAN13(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	67:		//EAN13
		{
			Number = ReadInBuffer();
			if ((Number == 12) || (Number == 13))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeEan13( Inbuf );
					Width = FormatCodeEAN13(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	3:		//EAN8
			MaxDataLen = 8;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if ((Number == 7) || (Number == 8))
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				//Width = FormatCodeEan8( Inbuf );
				Width = FormatCodeEAN8(Inbuf, Number);
				Width += g_tPrtCtrlData.PrtDataDotIndex;
			}
			else
			{
				return;
			}
			break;
		case	68:		//EAN8
		{
			Number = ReadInBuffer();
			if ((Number == 7) || (Number == 8))
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) == Number)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					//Width = FormatCodeEan8( Inbuf );
					Width = FormatCodeEAN8(Inbuf, Number);
					Width += g_tPrtCtrlData.PrtDataDotIndex;
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	4:							 //Code39
			MaxDataLen = 0xFF;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if (Number >= 1)
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCode39( Inbuf );
			}
			else
			{
				return;
			}
			break;
		case	69:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode39( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	5:
			MaxDataLen = 0xFE;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if (Number >= 1)	//Ϊ1ʱ������
			{
				if ((Number % 2) == 1)	//����Ϊ����
				{
					Number -= 1;	//ȷ������Ϊż��,�����Ϊż����������һ���ַ�
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatItf25( Inbuf );
					ReadInBuffer();		//��������һ���ַ�
				}
				else	//����Ϊż��
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatItf25( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		case	70:		//ITF,���ݸ�������Ϊż��
		{
			Number = ReadInBuffer();
			if (Number >= 1)	//Ϊ1ʱ������
			{
				if ((Number % 2) == 1)	//����Ϊ����
				{
					Number -= 1;	//ȷ������Ϊż��,�����Ϊż����������һ���ַ�
					if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
					{
						//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
						Width = FormatItf25( Inbuf );
					}
					ReadInBuffer();		//��������һ���ַ�
				}
				else	//����Ϊż��
				{
					if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
					{
						//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
						Width = FormatItf25( Inbuf );
					}
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	6:
			MaxDataLen = 0xFF;	//������Ŀ���Ѱ��,ʵ��δ����˳�
			Number = GetCodeBarStr(Inbuf, MaxDataLen, GetChar);	//��NULΪֹ��ʵ�����ݳ���
			if (Number >= 1)
			{
				//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
				Width = FormatCodabar( Inbuf );
			}
			else
			{
				return;
			}
			break;
		case	71:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCodabar( Inbuf );
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	72:
		{
			Number = ReadInBuffer();
			if (Number >= 1)
			{
				if (GetCodeBarStr(Inbuf, Number, GetChar) != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode93(Inbuf, Number);
				}
			}
			else
			{
				return;
			}
			break;
		}
		case	73:
		{
			Number = ReadInBuffer();
			if (Number >= 2)
			{
				Temp = GetCode128Str(Inbuf, Hribuf, Number);
				if (Temp != 0)
				{
					//g_tPrtCtrlData.PrtDataDotIndex += giCodebarPositionSpace;
					Width = FormatCode128(Inbuf, Temp);
					strcpy((char *)Inbuf, (const char *)Hribuf);
				}
			}
			else
			{
				return;
			}
			break;
		}
		default:
			return;
	}
	
	//����������������,���߳�������ӡ����
	if ((Width == g_tPrtCtrlData.PrtDataDotIndex) || (Width > g_tPrtCtrlData.PrtLength))
	{
		GoDotLine((uint32_t)g_tBarCodeCtrl.BarCodeHigh);	//����д�����ֻ��ֽ
		memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);	 //�建����
		//memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
	}
	else
	{	//����ֵΪ��0��ʼ�ĵ���ӡͼ�ν������ܿ��,������Ҫ����Ϊֻ����ʵ��ͼ�εĿ��ֵ
		Width -= g_tPrtCtrlData.PrtDataDotIndex;		//�õ�ֻ����ʵ��ͼ�εĿ��ֵ
		PrintBar(Inbuf, Width);	//��ӡ���벢����»��߻�����
	}
}

/**********************
*********************************************************
* Function Name	: Command_1D72 n
* Description		: ������nֵָ����״̬,n=1����ֽ״̬,n=2����Ǯ��״̬
n = 1, 2, 49, 50
������nֵָ����״̬: 
	n					���� 
1,49		����ֽ������״̬ 
2,50		����Ǯ��״̬
4,52		����īˮ״̬
.	 ��ָ��ֻ�Դ��ڴ�ӡ����Ч. 
��	���ջ������д�����ǰ�����ݱ�������֮��,��ִ������ָ��,���,���͸�ָ��
����յ�����״̬��һ����ʱ���ͺ�. 
��	����״̬�ֽڸ�λ��Ӧ��ϵ������ʾ:
* Input					: None
* Output				: None
* Return				: None
*******************************************************************************/
void Command_1D72(void)
{
	uint8_t	GetChar, Status;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 1) || (GetChar == 49))
	{
		Status = 0;
		if (g_tError.PaperSensorStatusFlag & 0x01)		//ֽ��״̬
			Status |= 0x0C;
		if (g_tError.PaperSensorStatusFlag & 0x02)		//ֽ����״̬
			Status |= 0x03;
		
		UpLoadData(&Status, 1);							//�ϴ�����
	}
	else if ((GetChar == 2) || (GetChar == 50))
	{
		Status = READ_MONEYBOX_STATUS;		//Ǯ��ӿ�3�ŵ�ƽ 2016.06.19
		Status &= 0x01;
		UpLoadData(&Status, 1);							//�ϴ�����
	}
}

/*******************************************************************************
* Function Name	: Command_1D7630
* Description		: ��ӡ��դλͼ.1D 76 30 m xL xH yL yH d1...dk
*					��ָ���ܵ�ǰ��ӡλ��,����Ӱ��.�����ܷ����ӡӰ��
Hex	 1D	 76	30 m xL xH yL yH d1 �� dk
[��Χ] 0	��	m	��	3, 48	��	m	��	51
1	��	(xL + xH �� 256) ��	256 (0 ��	xL	��	255, xH = 0, 1)
1	��	(yL + yH �� 256) ��	2303 (0	��	yL	��	255, 0 ��	yH	��	8)
0	��	d	��	255
k = (xL + xH �� 256) �� (yL + yH �� 256)
[ ����]	 ����m ȷ����ģʽ��ӡ��դλͼ.
	m					ģʽ		����ֱ��� (DPI)	����ֱ��� (DPI)
0, 48		����						203									203
1, 49		����						203									101
2, 50		����						101									203
3, 51		����,����			101									101
�� xL, xH ��ʾ�����ֽ���Ϊ(xL + xH �� 256) .
�� yL, yH ��ʾ�����ֵ���Ϊ(yL + yH �� 256) .
�� d ��ʾָ��������(��դ��ʽ).
The relationship between bit	image data and the print re sult is as follows:
d1 			d2 			... 		dX
dX+1 		dX+2 		... 		dX*2
: 			: 			... 		:
... 		dk-2 		dk-1		dk			X = (xL + xH	��	256)
MSB	LSB MSB	LSB	MSB LSB	MSB	LSB
* Input					: None
* Output				: None
* Return				: None
*******************************************************************************/
#define	CMD_1D7630_BYTE_XH_PAR_MAX			(2)
#define	CMD_1D7630_DOT_YH_PAR_MAX				(8)
void Command_1D76(void)
{
	uint8_t	GetChar;
	uint8_t	RatioX;
	uint8_t	RatioY;
	uint8_t	SpaceByte;
	uint8_t	k;
	uint8_t	l;
	uint8_t	TempBuf[MAX_RATIO_Y];
	uint8_t	DataBuf[PRT_DOT_NUMBERS / 8];		//�����ܼ����ֽ���
	uint16_t	X_Length;
	uint16_t	Y_Length;
	uint16_t	MaxX_width;
	uint16_t	i;
	uint16_t	j;
	
	if (ReadInBuffer() != 0x30)
		return;
	
	if (g_tMacro.DefinitionFlag)	//�궨����,�˳��궨��,�����
	{
		g_tMacro.Number = 0;				//���ַ���������
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//�����ı�־����
		g_tMacro.RunFlag = 0;
	}
	
	GetChar = ReadInBuffer();
	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m������Ч����
		return;
	
	//ֻ�ڱ�׼ģʽ���������ײ���Ч 2016.09.18  ȥ������ && \(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit))
	if (!(g_tSystemCtrl.PrtModeFlag == STD_MODE))
		return;
	
	RatioX = (GetChar & 0x01) + 1;					//��ȱ���,���Ϊ2
	RatioY = ((GetChar & 0x02) >> 1) + 1;		//�߶ȱ���,���Ϊ2
	
	GetChar = ReadInBuffer();
	X_Length = GetChar;					//�����ֽ�����λ
	GetChar = ReadInBuffer();		//�����ֽ�����λֵ
	if (GetChar < CMD_1D7630_BYTE_XH_PAR_MAX)
	{	
		X_Length += (uint16_t)(((uint16_t)GetChar) << 8);
	}	
	else
	{	
		return;
	}
	GetChar = ReadInBuffer();
	Y_Length = GetChar;					//�����ֵ�����λ
	GetChar = ReadInBuffer();		//�����ֵ�����λֵ
	Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);
// 	if (GetChar <= CMD_1D7630_DOT_YH_PAR_MAX)    //2017.04.24  ȥ������
// 	{	
// 		Y_Length += (uint16_t)(((uint16_t)GetChar) << 8);
// 	}	
// 	else
// 	{	
// 		return;
// 	}
	
	
	i = ((g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit) >> 3);	//����ɴ�ӡ����ֽ���
	if (X_Length * RatioX > i)
	{	
		MaxX_width = i / RatioX;
	}	
	else
	{	
	 	MaxX_width = X_Length;
	}
	SpaceByte = (g_tPrtCtrlData.PrtLeftLimit >> 3);				//�����ǲ���8���������
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)		//�ַ�����ģʽ,����
	{	
		SpaceByte += ((i - MaxX_width * RatioX) >> 1);
	}	
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//����
	{	
		SpaceByte += (i - MaxX_width * RatioX);
	}
	if (SpaceByte > (sizeof(DataBuf) / sizeof(DataBuf[0])))
	{	
		SpaceByte = sizeof(DataBuf) / sizeof(DataBuf[0]);
	}
	memset(DataBuf, 0x00, SpaceByte);		//��ʼ�����������ܼ����ֽ���

	for (i = 0; i < Y_Length; i++)				//��������߶�
	{
		l = SpaceByte;
		for (j = 0; j < MaxX_width; j++)		//�����ֽ���
		{
			GetChar = ReadInBuffer();
			if (RatioX == 2)									//����
			{
				memset(TempBuf, 0x00, MAX_RATIO_Y);
				//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
				ByteZoomCtrl(RatioX, GetChar, TempBuf);	//���ݷŴ���
			}
			else
			{
				TempBuf[0] = GetChar;
			}
			for (k = 0; k < RatioX; k++)			//���򱶿���
			{
				DataBuf[l] = TempBuf[k];
				l++;
			}
		}
		
		for (; l < (sizeof(DataBuf) / sizeof(DataBuf[0])); l++)	 //����һ�е�����
		{
			DataBuf[l] = 0;									  //Ҫ��ֹ�±�Խ��
		}
		for (; j < X_Length; j++)						//����������ӡ�������
		{
			ReadInBuffer();										//������һ�п�ʼ��ȡ�Ĳ�����Ҫ��ֵ		
		}
		for (k = 0; k < RatioY; k++)				//���򱶸ߴ���
		{
			DotLinePutDrvBuf( DataBuf );			//2016.06.16 ��һ�����ݵ�������ӡ������
		}	
	}
	g_tPrtCtrlData.BMPBitNum = Y_Length *RatioY;	//2016.08.04
	g_tTab.TabIndex = 0;		//2016.09.06
}

/*******************************************************************************
* Function Name	: Command_1D77
* Description		: ����������
1D	 77	n
2	��	 n	��	 6 
�����������ģ����	
�� n	��ָ������ĺ���ģ����: 
n		������ģ����						˫����ģ����
				(mm)				խ����ģ��(mm)	�����ģ��(mm)
2				0.25								0.25						0.625 
3				0.375								0.375						1.0 
4				0.5									0.5							1.25 
5				0.625								0.625						1.625 
6				0.75								0.75						1.875 
��	������ģ����������: 
	UPC-A, UPC-E, JAN13 (EAN13), JAN8 (EAN8), CODE93, CODE128 
��	˫����ģ����������: 
	CODE39, ITF, CODABAR 
[ Ĭ��ֵ]		n = 2 
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void Command_1D77(void)
{
	uint8_t	GetChar;
	const uint8_t	ThickWidth[5] = {5, 8, 10, 13, 15};
	//�д���һ��ȷ��,����˵������
	GetChar = ReadInBuffer();
	if ((GetChar >= 2) && (GetChar <= 6))
	{
		//խ����,ÿ����λ��ʾһ�����ȵ���0.125mm,n��ʾn*0.125mm��
		g_tBarCodeCtrl.BarCodeThinWidth = GetChar;	 								//խ����
		g_tBarCodeCtrl.BarCodeThickWidth = ThickWidth[GetChar - 2];	//������
	}
}

/*******************************************************************************
* Function Name	: GSCommand
* Description		: GS����Ĵ���
* Input					: None
* Output				 : None
* Return				 : None
*******************************************************************************/
void GS_Command(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x21:			
		{
			Command_1D21();		//���÷Ŵ���
			break;
		}
		case 0x23:			
		{
			Command_1D23();		//�������ص�RAM�е�λͼͼ��,����λͼ�ʹ�ӡλͼ���ݴ�ͼ�Ž���
			break;
		}
		case 0x24:			
		{
			Command_1D24();		//ҳģʽ�������������λ��
			break;
		}
		case 0x28:
		{
			Command_1D28();		//ִ�в��Դ�ӡ,��ӡ��ʽ��n,m����
			break;
		}
		case 0x2A:			
		{
			Command_1D2A();		//����λͼ
			break;
		}
		case 0x2F:
		{
			Command_1D2F();		//��ӡ����λͼ
			break;
		}
		case 0x3A:
		{
			Command_1D3A();		//�����
			break;
		}
		case 0x42:
		{
			Command_1D42();		//ѡ��/ȡ�����״�ӡ
			break;
		}
		case 0x44:
		{
			Command_1D44();		//ѡ������ģʽ
			break;
		}
		case 0x45:
		{
			Command_1D45();		//���ô�ӡŨ��
			break;
		}
		case 0x48:
		{
			Command_1D48();		//����HRI��ӡλ��
			break;
		}
		case 0x49:
		{
			Command_1D49();		//���ش�ӡ��ID
			break;
		}
		case 0x4C:
		{
			Command_1D4C();		//������߾�
			break;
		}
		case 0x50:
		{
			Command_1D50();		//�����ƶ���λ
			break;
		}
		case 0x51:
		{
			Command_1D51();		//�����������ӡ�ƶ�λ��/����λͼ��ӡ
			break;
		}
		case 0x53:
		{
			Command_1D53();		//�е� 2016.08.04
			break;
		}
		case 0x55:
		{
			Command_1D55();		//Ϊ�˼���GP����
			break;
		}
		case 0x56:			
		{
			Command_1D56();		//�е�ָ��
			break;
		}
		case 0x57:			
		{
			Command_1D57();		//���ô�ӡ����
			break;
		}
		case 0x5A:			
		{
			Command_1D5A();		//ѡ���ά��������
			break;
		}
		case 0x5E:
		{
			Command_1D5E();		//ִ�к�
			break;
		}
		case 0x61:
		{
			Command_1D61();		//ʹ��/��ֹ�Զ�״̬����
			break;
		}
		case 0x66:
		{
			Command_1D66();		//ѡ��HRI����
			break;
		}
		case 0x67:
		{
			Command_1D67();		//ά����������
			break;
		}
		case 0x68:
		{
			Command_1D68();		//��������߶�
			break;
		}
		case 0x6B:			
		{
			Command_1D6B();		//��ӡ����
			break;
		}
		case 0x72:
		{
			Command_1D72();		//���ش�ӡ��״̬
			break;
		}
		case 0x76:
		{
			Command_1D76();		//��ӡ��դλͼ
			break;
		}
		case 0x77:
		{
			Command_1D77();		//����������
			break;
		}
		default:
			break;
	}			 	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
