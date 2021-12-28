/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.1
  * @date    2016-4-9
  * @brief   ָ����س���.
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
#include	<math.h>
#include	"cpcl.h"
#include	"ctext.h"
#include	"cominbuf.h"
#include	"debug.h"
#include	"defconstant.h"
#include	"extgvar.h"

#ifdef	CPCL_CMD_ENABLE

#ifdef	DBG_RX_TOTAL_THEN_PRT
	extern volatile uint8_t	g_bRxFinishFlag;	//��ʾ����ͨ���������Ƿ��Ѿ��������
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* CPCLָ���ΪASCII�� */
/* CPCLÿ����ָ�����󳤶�,�����ж�ָ������һ��,��ÿ��ָ�ʼ������һ���ո�Ϊ���� */
#define		CPCL_CMD_LINE_LEN_MAX			(32)

/* һ��ָ����ÿ�����ֵķָ��� */
#define		CPCL_CMD_SPACE_CHAR				(uint8_t)(' ')

/* �����б����Ŀ */
#define		CPCL_CMD_LIST_MUMBER			(uint8_t)(100)

/* Private variables ---------------------------------------------------------*/

/* ��ȡ�����ַ���,�������ַ��������� */
uint8_t		g_tCmdBuf[CPCL_CMD_LINE_LEN_MAX + 1];

/* CPCLָ����ַ����б� */
const	uint8_t		m_tcCPCLCmdList[CPCL_CMD_LIST_MUMBER][CPCL_CMD_LINE_LEN_MAX + 1] = 
{
	"!",				//0
	"TEXT",			//1
	"T",				//2
	"VTEXT",		//3
	"VT",				//4
	"TEXT90",		//5
	"T90",			//6
	"TEXT180",	//7
	"T180",			//8
	"TEXT270",	//9
	"T270",			//10
	"PAGE-WIDTH",
	"PW",           	//12
	"LINE",
	"L",              //12~13
	"INVERSE-LINE",
	"IL",             //14~15
	"BOX",            //16
	"SETBOLD",        //17 
	"BARCODE",
	"B",
	"VBARCODE",
	"VB",							//18~21
	"BARCODE-TEXT",
	"BT",             //22~23
	"EXPANDED-GRAPHICS",
	"EG",
	"EXPANDED-GRAPHICS",
	"VEG",
	"COMPRESSED-GRAPHICS",
	"CG",
	"VCOMPRESSED-GRAPHICS",
	"VCG",						//24~31
	"LEFT",						//32    
	"CENTER",					//33
	"RIGHT",					//34
	"FORM",						//35
	"PRINT",					//36
	";",							//37
	"GAP-SENSE",			//38
	"SETSP",					//39
	"INVERSE-TEXT",
	"IT",							//40~41
	"UNDERLINE-TEXT",
	"UT",							//42~43
	"PAGE-ROTATE",
	"PR",							//44~45
	"USER-DEFINE-HZ",
	"UDH",						//46~47
	"BA",							//48
	"CUT-PAPER",
	"CP",							//49~50 
};

/* Private function prototypes -----------------------------------------------*/
static void	CPCLCmdProcess(void);
static uint8_t * GetCPCLStringUntilSpace(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t * GetCPCLStringUntilChangeLine(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t * GetCPCLString(uint8_t * pBuf, const uint32_t	maxLen);
static uint8_t GetCPCLCmdNumber(const uint8_t * Str);
static void	GetCPCLChangeLine(void);

static void CPCLCmdPageStart(void);

static void SetTextFont(void);
static void SetTextSize(void);
static void SetTextStartPosition(void);
static void CPCLCmdText(uint8_t Type);

/* Public functions ----------------------------------------------------------*/
void	InitPageEditPrtBuf(void)
{
	#ifdef	EXT_SRAM_ENABLE
		g_tPageEditPrtBuf.PageBuf = (uint8_t *)(EXT_RAM_PAGE_BUF_BASE_ADDR);
	#endif
}

/*******************************************************************************
* Function Name  : ������
* Description    : CPCLָ��Ĵ������ڵ���ʱһ���Խ������ݷ��㡣
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
void	CPCL_Command(void)
{
	//������ջ�����������,�����п��Խ��б༭���д�ӡ������
	if (GetInBufBytesNumber() > 0)
	{
		#ifdef	DBG_RX_TOTAL_THEN_PRT				//�����ٴ�
			if (g_bRxFinishFlag != RX_FINISHED)
			{
				return;		//����һ��ͨ�Ž������
			}
			else	//ͨ�Ź����Ѿ����,��ʼ�����û�������ߴ���ӡ����			
			{
				CPCLCmdProcess();
			}
		#else	//�������ձߴ�ĳ�����,�����ͨ���Ƿ����
			CPCLCmdProcess();
		#endif
	}	//ͨ�Ž��ջ������ǿմ������
}

/*******************************************************************************
* Function Name  : ������
* Description    : CPCLָ��Ĵ�����������ָ�����ִ�и���ָ��.�������£�
	1. �ӽ��ջ��������������ַ�������ַ��������������ĵ�һ���ո�Ϊֹ��
		 ��������Ϊ����ֵ������Ϊ���ַ�����
	2. ��������ַ������б��еı��
	3. ���ݱ�Ž����Ӧ��ָ���������������ʣ�������ֱ���������д�����ϡ�
	4. ִ�и�ָ����˳�������
	5. �����������ٴν��뱾������
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
static void	CPCLCmdProcess(void)
{
	uint8_t	CmdNumber;
	
	GetCPCLString(g_tCmdBuf, CPCL_CMD_LINE_LEN_MAX);	//��ȡ�����ַ���
	CmdNumber = GetCPCLCmdNumber(g_tCmdBuf);	//�ж��ַ�����Ӧ��������
	switch	(CmdNumber)
	{
		case	0:
			CPCLCmdPageStart();
			break;
		case	1:
		case	2:
			CPCLCmdText(CmdNumber);
			break;
		default:
			break;
	}
}

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : ������
* Description    : ��ָ����뻺�������ӽ��ջ�������ʼ���������ո�����������
	�����ݣ����Ҳ�ǿո��������ֱ�����ǿո�Ϊֹ����ֹ��������ո�������
	����󳤶�֮ǰû��������һ���ո�Ҳ�˳�
* Input          : �����뻺�������׵�ַ
* Output         : ���뻺�������׵�ַ
*******************************************************************************/
static uint8_t * GetCPCLStringUntilSpace(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//�����������
		if (GetChar == CPCL_CMD_SPACE_CHAR)		//�����ո�ָ����ͽ���
		{
			while (CPCL_CMD_SPACE_CHAR == ReadInBuffer());	//�˴�������ɾ������ո�
			DecInBufferGetPoint();							//�Żض��Ĳ��ǿո���ַ�
			break;
		}
		else		//������ǿո�ʹ洢
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	*pBuf = '\0';	//������û�������ո񣬶�����ַ�����������
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ָ����뻺�������ӽ��ջ�������ʼ��������0D����0A��Ϊֹ
* Input          : �����뻺�������׵�ַ
* Output         : ���뻺�������׵�ַ
*******************************************************************************/
static uint8_t * GetCPCLStringUntilChangeLine(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//�����������
		if ((GetChar == 0x0D) || (GetChar == 0x0A))	//�������з��ͽ���
		{
			DecInBufferGetPoint();							//�Żض��Ĳ��ǿո���ַ�
			break;
		}
		else		//������ǿո�ʹ洢
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	*pBuf = '\0';	//����ַ�����������
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ���ַ����뻺�������ӽ��ջ�������ʼ���𣬵���󳤶�֮ǰ��ֱ��
	������һ���ո�֮ǰ������
* Input          : �����뻺�������׵�ַ
* Output         : ���뻺�������׵�ַ
*******************************************************************************/
static uint8_t * GetCPCLString(uint8_t * pBuf, const uint32_t	maxLen)
{
	uint32_t	i;
	uint8_t		GetChar;
	uint8_t		SpaceFlag;
	uint8_t	* pTemp;
	
	assert_param(maxLen <= CPCL_CMD_LINE_LEN_MAX);
	
	pTemp = pBuf;
	SpaceFlag = 0;
	for (i = 0; i < maxLen; i++)
	{
		GetChar = ReadInBuffer();							//�����������
		if (GetChar == CPCL_CMD_SPACE_CHAR)		//�����ո�ָ����ͽ���
		{
			SpaceFlag = 1;
			break;
		}
		else		//������ǿո�ʹ洢
		{
			*pBuf = GetChar;
			pBuf++;
		}
	}
	
	if (0 == SpaceFlag)		//û���������ո�
	{
		pBuf = pTemp;
		for (i = 0; i < maxLen; i++)
		{
			*pBuf = 0x00;		//��ս���
		}
	}
	else		//�������ո�����ַ�����������
	{
		*pBuf = '\0';
	}
	
	return	(pTemp);
}

/*******************************************************************************
* Function Name  : ������
* Description    : �������б��е�ֵ���бȽϣ��õ�����ı����
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
static uint8_t GetCPCLCmdNumber(const uint8_t * Str)
{
	uint8_t	i;
	
	for (i = 0; i < CPCL_CMD_LIST_MUMBER; i++)
	{
		if (strcmp(Str, m_tcCPCLCmdList[i]) == 0)
		{
			break;
		}
	}
	
	return	(i);
}


/*******************************************************************************
* Function Name  : �ó��������⣬'0'-'0'�ͱ�� NUL��
* Description    : ��ASCII��ֵ'0'��'9','a'��'f','A'��'F'��ɵ��ַ���ת��Ϊ16����0~F��ֵ
* Input          : 
* Output         : ��
* Return         : ��
*******************************************************************************/
void	StrAsciiToStrHex(uint8_t Str[])
{
	uint8_t	i;
	uint8_t	Len;
	uint8_t	buf[255];
	uint8_t	* pTemp;
	
	pTemp = Str;
	Len = 0;
	while (*Str)
	{
		if ((*Str >= '0') && (*Str <= '9'))
		{
			*Str -= '0';
		}
		else if ((*Str >= 'A') && (*Str <= 'F'))	//'A'ASCII��ֵΪʮ����65
		{
			*Str -= 55;
		}
		else if ((*Str >= 'a') && (*Str <= 'f'))	//'a'ASCII��ֵΪʮ����97
		{
			*Str -= 87;
		}
		else
		{
		}
		
		buf[Len] = *Str;
		Str++;
		Len++;
		if (Len == 254)		//��ֹ�±�Խ��
		{
			break;
		}
	}
	
	Str = pTemp;
	for (i = 0; i < Len; i++)
	{
		*Str = buf[i];
		Str++;
	}
	*Str = '\0';		//����ַ�����β����
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ASCII��ֵ��ɵ��ַ���ת��Ϊ����ֵ,ǰ���Ϊ��λ����͵�Ϊ��λ��
	��ֵ��Ϊ��0��~��9��֮�䡣ʵ�ʰ������5λ����
	���ַ������մӸߵ��͵�˳����������������HEX�������浽������������λ����
	�ٰ���λ���������ֵ
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
uint32_t	StrAsciiToInt(uint8_t Str[])
{
	uint8_t	i;
	uint8_t	Len;
	uint8_t	buf[8];
	uint32_t	result;
	uint32_t	ratio;
	uint8_t	* pTemp;
	
	memset(buf, 0x00, 8);
	Len = 0;
	while (*Str)
	{
		if ((*Str >= '0') && (*Str <= '9'))
		{
			*Str -= '0';
		}
		else		//������Ч�ַ�����
		{
			break;
		}
		
		buf[Len] = *Str;
		Str++;
		Len++;
		if (Len == 8)		//��ֹ�±�Խ��
		{
			break;
		}
	}
	
	pTemp = buf;
	pTemp += (Len - 1);		//����λ���Ͽ�ʼ���ݼ�
	result = 0;
	ratio = 1;
	for (i = 0; i < Len; i++)
	{
		result += (*pTemp * ratio);
		ratio *= 10;
		pTemp--;
	}
	
	return	(result);
}

typedef	struct CPCLCtrl
{
	uint16_t	HoriOffSet;						//������ǩ��ˮƽƫ����,UNIT�����ĵ�λ
	uint16_t	HoriResolution;				//ˮƽ�ֱ��ʣ�203��/Ӣ�磨8��/mm��
	uint16_t	VerticalResolution;		//��ֱ�ֱ��ʣ�203��/Ӣ�磨8��/mm��
	uint16_t	MaxHeight;						//��ǩ���߶ȣ���Ϊ��λ
	uint16_t	Quantity;							//��ӡ��ǩ�����������1024��
	uint16_t	HoriStart;						//���ݱ༭ʱ������ʼλ�ã���Ϊ��λ
	uint16_t	VerticalStart;				//���ݱ༭ʱ������ʼλ�ã���Ϊ��λ
	
}TypeDef_StructCPCLCtrl;

TypeDef_StructCPCLCtrl	g_tCPCLCtrl;

/*******************************************************************************
* Function Name  : ������
* Description    : ��ȡ���У�����Ӧ��Ϊ0D + 0A�����⸽�ӹ��򵥶�0D �򵥶� 0A����
 0A + 0D����Ϊ���С�
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
static void	GetCPCLChangeLine(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();		//�����������
	if (GetChar == 0x0D)				//ǰ���Ϊ0D
	{
		GetChar = ReadInBuffer();	//�����������
		if (GetChar == 0x0A)			
		{
			return;									//1.��׼��Ԥ��0D+0A
		}
		else
		{
			DecInBufferGetPoint();	//2.ֻ��0D���������д��������ֻ���Ķ���λ��ֵ
		}
	}
	else if (GetChar == 0x0A)		//ǰ���Ϊ0A
	{
		GetChar = ReadInBuffer();	//�����������
		if (GetChar == 0x0D)			
		{
			return;									//3.����Ԥ��0A+0D
		}
		else
		{
			DecInBufferGetPoint();	//4.ֻ��0A���������д��������ֻ���Ķ���λ��ֵ
		}
	}
	else	//������
	{
		DecInBufferGetPoint();		//���ǻ��е�λ��
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ҳ��ʼָ��� {offset}  200  200  {height}  {qty}
 ��������£�!�Ϳո��Ѿ��ڽ���֮ǰ����ȡ�ˡ����ζ�ȡ����ֵ�����ÿ���ȫ�ֱ�����
 {offset}��������ǩ��ˮƽƫ����
	200��ˮƽ�ֱ��ʣ�203��/Ӣ�磨8��/mm��
	200����ֱ�ֱ��ʣ�203��/Ӣ�磨8��/mm��
	{height}:��ǩ���߶ȣ���Ϊ��λ
	{qty}:��ӡ��ǩ�����������1024��
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
static void CPCLCmdPageStart(void)
{
	uint8_t	Buf[8];
	
	GetCPCLString(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ,������ǩ��ˮƽƫ����
	g_tCPCLCtrl.HoriOffSet = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ, ˮƽ�ֱ���
	g_tCPCLCtrl.HoriResolution = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ, ��ֱ�ֱ���
	g_tCPCLCtrl.VerticalResolution = StrAsciiToInt(Buf);
	
	GetCPCLString(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ, ��ǩ���߶ȣ���Ϊ��λ
	g_tCPCLCtrl.MaxHeight = StrAsciiToInt(Buf);
	
	GetCPCLStringUntilChangeLine(Buf, 6);	//���뻻��Ϊֹ�����65535����ֵ, ��ӡ��ǩ�����������1024��
	g_tCPCLCtrl.Quantity = StrAsciiToInt(Buf);
	
	GetCPCLChangeLine();		//���뻻�з�
}

/*******************************************************************************
* Function Name  : ������
* Description    : CPCL�ı���������趨�Ĳ���������ı���ҳ��ӡ������
	{command} {font} {size} {x} {y} {data}
	[˵��]       {command} :���±���ѡ����Ҫʹ�õ�ָ��
	ָ��	Ч��
	TEXT��or T��				��ӡ��������
	VTEXT��or VT��			��ӡ����(����)����ʱ����ת90��
	TEXT90��or T90��		��VTEXTָ�����ͬ
	TEXT180��or T180��	��ӡ����(����)����ʱ����ת180��
	TEXT270��or T270��	��ӡ����(����)����ʱ����ת270��

	{font}:ѡ�������
	{size}:ѡ�������С
	{x}��  ˮƽ��ӡ��ʼλ��
	{y}:   ��ֱ��ӡ��ʼλ��
	{data}:��ӡ���ı�����

	font	�������
	24	�ַ�(12*24)������(24*24)
	55	�ַ�(8*16)������(16*16)

	�ַ��߶�ѡ��	�ַ����ѡ��
	size	����Ŵ�	size	����Ŵ�
	0	1��������			0	1 ��������
	1	2��2���ߣ�		10	2 ��2����
	2	3							20	3
	3	4							30	4
	4	5							40	5
	5	6							50	6
	6	7							60	7
	7	8							70	8
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/
/* �����ı����壬�ַ���С���� */
static void SetTextFont(void)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	
	GetCPCLStringUntilSpace(Buf, 5);	//����ո�Ϊֹ�����65535����ֵ
	GetChar = StrAsciiToInt(Buf);			//�õ�font�������
	if (GetChar == 24)			//24	�ַ�(12*24)������(24*24)
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_A;
		g_tPageModeCharPara.Width				= ASCII_FONT_A_WIDTH;
		g_tPageModeCharPara.Height			=	ASCII_FONT_A_HEIGHT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_A;
		g_tPageModeHZPara.Width					= HZ_FONT_A_WIDTH;
		g_tPageModeHZPara.Height				=	HZ_FONT_A_HEIGHT;
	}
	else if (GetChar == 55)	//55	�ַ�(8*16)������(16*16)
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_C;
		g_tPageModeCharPara.Width				= ASCII_FONT_C_WIDTH;
		g_tPageModeCharPara.Height			=	ASCII_FONT_C_HEIGHT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_B;
		g_tPageModeHZPara.Width					= HZ_FONT_B_WIDTH;
		g_tPageModeHZPara.Height				=	HZ_FONT_B_HEIGHT;
	}
	else	//����ΪĬ��
	{
		g_tPageModeCharPara.FontType		= ASCII_FONT_DEFAULT;
		g_tPageModeCharPara.Width				= ASCII_FONT_WIDTH_DEFAULT;
		g_tPageModeCharPara.Height			=	ASCII_FONT_HEIGHT_DEFAULT;
		
		g_tPageModeHZPara.FontType			= HZ_FONT_DEFAULT;
		g_tPageModeHZPara.Width					= HZ_FONT_WIDTH_DEFAULT;
		g_tPageModeHZPara.Height				=	HZ_FONT_HEIGHT_DEFAULT;
	}
}

/* �����ı��Ŵ���*/
static void SetTextSize(void)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	uint8_t	temp;
	
	GetCPCLStringUntilSpace(Buf, 5);	//����ո�Ϊֹ�����65535����ֵ
	GetChar = StrAsciiToInt(Buf);			//�õ������С
	temp = GetChar / 10;		//�������Ŵ���,��׼������Χ0~7
	if (temp < MAX_RATIO_X)
	{
		temp++;								//��Ϊ0����Ŵ���1
		g_tPageModeCharPara.WidthMagnification	= temp;
		g_tPageModeHZPara.WidthMagnification		= temp;
	}
	else	//��������򲻴���
	{
	}
	temp = GetChar % 10;		//��������Ŵ���,��׼������Χ0~7
	if (temp < MAX_RATIO_Y)
	{
		temp++;								//��Ϊ0����Ŵ���1
		g_tPageModeCharPara.HeightMagnification	= temp;
		g_tPageModeHZPara.HeightMagnification		= temp;
	}
	else	//��������򲻴���
	{
	}
}

/* �����ı���ӡ��ʼλ�� */
static void SetTextStartPosition(void)
{
	uint8_t	Buf[8];
	uint16_t	StartPos;
	
	GetCPCLStringUntilSpace(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ
	StartPos = StrAsciiToInt(Buf);		//�õ�x
	g_tCPCLCtrl.HoriStart = StartPos;
	
	GetCPCLStringUntilSpace(Buf, 6);	//����ո�Ϊֹ�����65535����ֵ
	StartPos = StrAsciiToInt(Buf);		//�õ�y
	g_tCPCLCtrl.VerticalStart = StartPos;
}

/* GetPageCharDotData(,)�����ַ��������룬��ȡ�ַ�������仺������ֱ���������з��� */
/* �Զ��ص��ַ������������ת���Ӵ֣����أ��������Ŵ��»��ߵĴ���*/
static void	ProcessTextData(void)
{
	uint8_t	GetChar;
	uint8_t	Temp;
	uint8_t	DotBuf[MAX_BYTES_OF_HZ];
	
	while (1)
	{
		GetChar = ReadInBuffer();
		if (GetChar >= PRINTABLE_START_CODE)
		{
			GetPageCharDotData(GetChar, DotBuf);	//�ɴ�ӡ�ַ�����
			
		}
		else if ((GetChar != 0x0D) || (GetChar != 0x0A))	//����ĩβ
		{
			DecInBufferGetPoint();
			break;
		}
	}
}

/* Textָ��� */
static void CPCLCmdText(uint8_t	Type)
{
	uint8_t	Buf[8];
	uint8_t	GetChar;
	uint8_t	temp;
	uint16_t	StartPos;
	
	if ((Type == 1) || (Type == 2))		//��ת0��
	{
		g_tPageModeCharPara.RotateType	= 0;
		g_tPageModeHZPara.RotateType		= 0;
	}
	else if ((Type == 3) || (Type == 4) || (Type == 5) || (Type == 6))	//��ת90��
	{
		g_tPageModeCharPara.RotateType	= 1;
		g_tPageModeHZPara.RotateType		= 1;
	}
	else if ((Type == 7) || (Type == 8))		//��ת180��
	{
		g_tPageModeCharPara.RotateType	= 2;
		g_tPageModeHZPara.RotateType		= 2;
	}
	else if ((Type == 9) || (Type == 10))		//��ת270��
	{
		g_tPageModeCharPara.RotateType	= 3;
		g_tPageModeHZPara.RotateType		= 3;
	}
	else	//�������󣬲�����
	{
		return;
	}
	
	SetTextFont();		//�ȴ���FONT����	
	SetTextSize();		//�ٴ��������С
	SetTextStartPosition();	//�ٴ����ӡ��ʼλ��
	ProcessTextData();	//����ʵ�ʴ�ӡ�ַ�������ӡ������,��������ַ�ֱ������0D,0A��
											//����ַ�Ϊ�ɴ�ӡ�ַ�����
	GetCPCLChangeLine();	//�������н�������
}

/*******************************************************************************
* Function Name  : ������
* Description    : 
* Input          : ��
* Output         : ��
* Return         : ��
*******************************************************************************/



#endif	/* #ifdef	CPCL_CMD_ENABLE ���� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
