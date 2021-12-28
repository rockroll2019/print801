/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
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
#include	"fs.h"
#include	"cominbuf.h"
#include	"extgvar.h"					//ȫ���ⲿ��������
#include	"charmaskbuf.h"
#include	"spiflash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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
* Function Name  : Command_1C21
* Description    : ���ú��ִ�ӡģʽ����������,����,���ߵ�
λ  	0/1  Hex  Decimal 		���� 
0, 1        							δ���� 
2			0  		00  		0  		ȡ������
2			1  		04  		4  		ѡ�񱶿�
3			0  		00  		0  		ȡ������
3			1  		08  		8  		ѡ�񱶸�
4-6        								δ���� 
7			0  		00  		0  		ȡ���»���
7			1  		80  		128		ѡ���»���
��  �������뱶�߶�ģʽͬʱ���趨ʱ���ַ����������ͬʱ���Ŵ�����(�������Ҽ��)�� 
��  ��ӡ���ܶ������ַ����»��ߣ��������Ҽ�ࡣ�����ܶ�����HTָ��(��������)����
�Ŀո���мӻ��ߣ�Ҳ����˳ʱ����ת90�ȵ��ַ����»��ߡ� 
��  �»����߿���FS - �趨�����ַ���С�޹ء� 
��  ��һ�����ַ��߶Ȳ�ͬʱ�������е������ַ��Ե��߶��롣 
��  ������FS W  ����  GS ! ���ַ��Ӵ֣����һ��ָ����Ч�� 
��  Ҳ������FS �C ѡ���ȡ���»���ģʽ�����һ��ָ����Ч��
[ Ĭ��ֵ]    n = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C21(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	//----ѡ���ַ�����ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)----
	if (GetChar & 0x04)
	{
		g_tPrtModeHZ.WidthMagnification = 2;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification = 1;
	}
	
	//----ѡ���ַ�����ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)----
	if (GetChar & 0x08)
	{
		g_tPrtModeHZ.HeightMagnification = 2;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = 1;
	}
	
	//----ѡ���ַ��»���ģʽ(��׼ģʽ��ҳģʽͬʱ��Ч)----
	if (GetChar & 0x80)
	{
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_MASK;
	}
	else
	{
		g_tPrtModeHZ.Underline &= UNDERLINE_DISABLE_MASK;
	}
}

/*******************************************************************************
* Function Name  : Command_1C26
* Description    : ���뺺�ַ�ʽ��//���Ĭ��ΪӢ��ģʽ�������������ģ��������Ĭ�ϵ�����ģʽ
��  ��ѡ�к���ģʽʱ����ӡ���ж��ַ��Ƿ�Ϊ�������룬���Ǻ������룬�ȴ����һ�ֽڣ�
Ȼ���жϵڶ��ֽ��Ƿ�Ϊ�������롣 
��  ��ӡ���ϵ���Զ�ѡ����ģʽ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C26(void)
{
	TypeDef_UnionSysConfig 	SysConfigUnionData;		//ϵͳ����
	uint16_t Len;
	
	Len = sizeof(g_tSysConfig);
	sFLASH_ReadBuffer(SysConfigUnionData.bSysParaBuf, SYS_PARA_BASE_ADDR, Len);
	
	//���Ĭ��ΪӢ��ģʽ�������������ģ��������Ĭ�ϵ�����ģʽ
	if (SysConfigUnionData.SysConfigStruct.SysLanguage == LANGUAGE_ENGLISH)
		g_tSysConfig.SysLanguage = LANGUAGE_CHINESE;
	else
		g_tSysConfig.SysLanguage = SysConfigUnionData.SysConfigStruct.SysLanguage;
}

/*******************************************************************************
* Function Name  : Command_1C2D
* Description    : ����/ȡ�������»���
1C   2D   n
0  ��   n  ��   2, 48 ��   n  ��   50
���� n  ��ֵ��ѡ���ȡ�����ֵ��»��ߣ� 
	n  					���� 
0, 48  ȡ�������»��� 
1, 49  ѡ�����»���(1���) 
2, 50  ѡ�����»���(2���)
��  ��ӡ���ܶ������ַ����»��ߣ��������Ҽ�ࡣ�����ܶ���HTָ��(��������)�����
�ո���»��ߣ�Ҳ����˳ʱ����ת90�ȵ��ַ����»��ߡ� 
��  ���»���ģʽ�󣬲���ִ���»��ߴ�ӡ����ԭ�����õ��»����߿���ı䡣Ĭ���»���
�߿�Ϊ1�㡣 
��  ��ʹ�ı��ַ���С���趨���»����߿�Ҳ����ı䡣 
��  ��FS  !Ҳ��ѡ���ȡ���»���ģʽ�����һ��ָ����Ч�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C2D(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if (GetChar == 0 || GetChar == 48)	   	//ȡ���»���
		g_tPrtModeHZ.Underline &= UNDERLINE_DISABLE_MASK;
	else if (GetChar == 1 || GetChar == 49)	//����1��ģʽ
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_ONE_DOT_MASK;
	else if (GetChar == 2 || GetChar == 50)	//����2��ģʽ
		g_tPrtModeHZ.Underline |= UNDERLINE_ENABLE_TWO_DOT_MASK;
}

/*******************************************************************************
* Function Name  : Command_1C2E
* Description    : �˳����ַ�ʽ,ȡ������ģʽ
��  ������ģʽ��ȡ��ʱ�������ַ���������ASCII�ַ�����ÿ��ֻ����һ���ֽڡ�  
��  �ϵ��Զ�ѡ����ģʽ�� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C2E(void)
{
	g_tSysConfig.SysLanguage = LANGUAGE_ENGLISH;
}

/*******************************************************************************
* Function Name  : Command_1C32
* Description    : �Զ��庺��,��ඨ��10�����֣�������ֽ�=0xfe,���ֽ�=0xa1~0xaa
1C   32   c1  c2  d1...dk
������c1��c2ָ���ĺ���
c1 ,c2 �������ַ����ַ�����
c1 = FEH   
A1H ��    c2 ��   FEH 
0  ��   d  ��   255 
k = 72
��  c1  ��c2�����û��Զ��庺�ֵı��룬c1ָ����һ���ֽڣ�c2ָ���ڶ����ֽڡ� 
��  d�������ݡ�ÿ���ֽڵ���ӦλΪ1��ʾ��ӡ�õ㣬Ϊ0����ӡ�õ㡣
[ Ĭ��ֵ]   û���Զ��庺��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C32(void)
{
	uint8_t	CodeStart;
	uint8_t	CodeStop;
	uint32_t	i;
	uint8_t	* pZimo = NULL;
		
	CodeStart = ReadInBuffer();
	CodeStop = ReadInBuffer();
	//��������Χ
	//code1 = 0xFE, 0xA1<= code2 <= 0xFE, �����ڴ�����ֻ�ܶ������޵ĺ���
	if ((CodeStart == 0xFE) && \
			(CodeStop > 0xA0) && \
			(CodeStop < (0xA1 + USER_HZ_SIZE)))
	{
		g_tRamBuffer.UserCharStruct.UserCode_HZ[CodeStop - 0xA1] = CodeStop;	//�û����庺�ֵ��ֽڴ����
		pZimo = g_tRamBuffer.UserCharStruct.Buffer_HZ;												//�û����庺����ģ���׵�ַ
		pZimo += (MAX_BYTES_OF_HZ * (CodeStop - 0xA1));
		for (i = 0; i < MAX_BYTES_OF_HZ; i++)
		{
			*pZimo++ = ReadInBuffer();
		}
		g_tRamBuffer.UserCharStruct.UserdefineFlag |= 0x02;	//�������Զ��庺��
		
		pZimo = NULL;
	}
}

/*******************************************************************************
* Function Name  : Command_1C3F
* Description    : ȡ���û��Զ��庺��
*	1C   3F   c1	c2
*	ȡ����c1��c2ָ�����û��Զ��庺��
c1 ,c2 �������ַ����ַ�����
c1 = FEH   
A1H ��    c2 ��   FEH 
*	ȡ����,���ַ�Ϊ�հ�
*	��ָ��ɾ�����Զ��庺�ֵ���ģ
*	����Զ����ַ���û�и��ַ�,��ָ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C3F(void)
{
	uint8_t	CodeStart;
	uint8_t	CodeStop;
	uint32_t	i;
	uint8_t	* pCode = NULL;
	uint8_t	* pZimo = NULL;
	
	CodeStart = ReadInBuffer();
	CodeStop = ReadInBuffer();
	if (g_tSysConfig.SysLanguage != LANGUAGE_ENGLISH)		//��Ӣ��
	{
		//��������Χ
		//code1 = 0xFE, 0xA1<= code2 <= 0xFE, �����ڴ�����ֻ�ܶ������޵ĺ���
		if ((CodeStart == 0xFE) && \
				(CodeStop > 0xA0) && \
				(CodeStop < (0xA1 + USER_HZ_SIZE)))
		{
			pCode = g_tRamBuffer.UserCharStruct.UserCode_HZ;	//�������Ƿ��Ѿ���������
			pCode += (CodeStop - 0xA1);												//�ҵ��ú���Ӧ�ô洢��λ��
			if (*pCode == CodeStop)			//����ú����Ѿ������������
			{
				*pCode = 0;								//�������
				pCode = NULL;
				pZimo = g_tRamBuffer.UserCharStruct.Buffer_HZ;	//�û����庺����ģ���׵�ַ
				pZimo += (MAX_BYTES_OF_HZ * (CodeStop - 0xA1));
				
				for (i = 0; i < MAX_BYTES_OF_HZ; i++)		//�����ģ����
					*pZimo++ = 0;
				pZimo = NULL;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1C43
* Description    : ѡ���ִ��뼯��n��0��48 ���壬n1��49 ���� 
1C   43   n1  n2 
n=0 ��1��48��49
ѡ���ִ���ϵͳ 
	n			ѡ���ִ���ϵͳ 
0, 48		��������(GB2312��GB18030) 
1, 49		��������(BIG5) 
��  ��ָ��ı�Flash�еĲ������á� 
��  ��ִ��ESC @ָ��,�رյ�Դ���ӡ����λ�󣬻ָ�ΪĬ��ֵ 
[ Ĭ��ֵ]		n = 0	���������ͺ� 
						n = 1	���������ͺ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C43(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	
	if ((GetChar == 0) || (GetChar == 48))
	{	
		g_tSysConfig.SysLanguage = LANGUAGE_CHINESE;
	}
	else if ((GetChar == 1) || (GetChar == 49))
	{
		g_tSysConfig.SysLanguage = LANGUAGE_BIG5;
	}
}

/*******************************************************************************
* Function Name  : Command_1C50
* Description    : ��ӡԤ����λͼ
[����]  ��ӡԤ�洢λͼ 
[��ʽ] ASCII ��   FS P   n 
ʮ�������� 1C 50  n 
ʮ������  28 80  n 
[��Χ] 0  ��  n  ��  7 
[����]  �������ӡԤ�ȴ洢�ڴ�ӡ������ʧ�洢���е�2ֵλͼ����ӡ������ʧ�洢���е�
λͼ��ͨ��PC���ϵ�ר�ù���������ɲ�д�룬λͼ������Ϊ576�����߶�Ϊ64��
ͼ�����Ϊ64K.
n Ϊָ����λͼ��š�
[ע��]   
	ָ����ŵ�λͼ��δ����ʱ����������Ч��
  λͼ������2ֵλͼ�� 
  ������ܴ�ӡģʽ(����,�ص�,�»���,�ַ���С,�򷴰״�ӡ)Ӱ�졣   
  ���Ҫ��ӡ��λͼ��ȳ���һ�У��򳬳��Ĳ��ֲ���ӡ�� 
  ����ר�õĹ����ϴ���ӡλͼ����μ�����,JPM112 ��ӡ�������������ͨ�����ַ�ʽ
	�ϴ���λͼ���ᶪʧ�����������ϴ�����λͼ���串�ǡ�
	λͼ�洢ʱ���պ���ȡģ,��������,��������,����ȡģ��ʽ.����һ���ֽڵĲ���һ���ֽ�.
	��ȷ����ϰ����ֽڼ���, �߶ȷ����ϰ��յ����.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C50(void)
{
	uint8_t	FlashBitBmpNum;
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint16_t High;
	uint16_t Width;
	uint16_t RealWidth;
	uint32_t SectorAddress;
	uint32_t i;
	
	FlashBitBmpNum = ReadInBuffer();	//ͼƬ�����к�
	if (FlashBitBmpNum < MAX_DOWNLOAD_NVFLASH_BITMAP_NUM)	//�������Ϸ���Χ, ���򲻴���  7��ͼ
	{
		memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
		//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
		SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;	//��ʼ�洢��ַ
		sFLASH_ReadBuffer(DataBuf, SectorAddress, 5);
		if (DataBuf[0] == FlashBitBmpNum)	//���ͼƬ�Ƿ��Ѿ�������
		{
			//Width = DataBuf[1] + 256 * DataBuf[2];
			//High = DataBuf[3] + 256 * DataBuf[4];
			Width	= (uint16_t)DataBuf[1] + (uint16_t)(((uint16_t)DataBuf[2]) << 8);	//��ȷ������ֽ���
			High	= (uint16_t)DataBuf[3] + (uint16_t)(((uint16_t)DataBuf[4]) << 8);	//�߶ȷ����ϵ���
			
			if (Width > (g_tSystemCtrl.LineWidth >> 3))
				RealWidth = (g_tSystemCtrl.LineWidth >> 3);		//ʵ�ʴ�ӡ�Ŀ���ֽ���
			else
				RealWidth = Width;
			
			//while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus());	//�ȴ�֮ǰ��ӡ����
			
			SectorAddress += 5;
			for (i = 0; i < High; i++)				//��ӡһ����
			{
				memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
				//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
				sFLASH_ReadBuffer(DataBuf, SectorAddress, RealWidth);
				SectorAddress += Width;
				DotLinePutDrvBuf( DataBuf );				//2016.06.16 ��һ�����ݵ�������ӡ������
			}
			g_tTab.TabIndex = 0;		//2016.09.06
		}	//����ӡͼƬ�Ѿ������˴������
	}	//������Χ�Ϸ�����
}

/*******************************************************************************
* Function Name  : Command_1C53
* Description    : ���ú������Ҽ��	��
1C   53   n1  n2
�ֱ𽫺��ֵ�������Ҽ������Ϊ n1  �� n2 �� 
��  ����ӡ��֧��GS Pָ��ʱ�������� [n1 �� ������������ƶ���λ] Ӣ�磬�Ҽ����
	[n2 ��  ������������ƶ���λ] Ӣ�硣 
��  ���ñ���ģʽ�����Ҽ��Ҳ�ӱ��� 
��  �ƶ���λ����GS P ָ�����õġ�  ��ʹ����������ƶ���λ��GS P�ı䣬ԭ���趨��
	�ַ����Ҳ���ı䡣 
��  �ڱ�׼ģʽ�£��ú����ƶ���λ��  
��  ��ҳģʽ�£����ݴ�ӡ������ʼλ����ѡ�����ú����ƶ���λ���������ƶ���λ��  
	�� ����ʼλ���ڴ�ӡ��������Ͻǻ����½ǣ��ú����ƶ���λ�� 
	�� ����ʼλ���ڴ�ӡ��������Ͻǻ����½ǣ��ú����ƶ���λ�� 
	�� ���ֵ�����Ҽ��ԼΪ36mm ��������ֵȡ���ֵ��
[ Ĭ��ֵ]    n1 = 0, n2 = 0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C53(void)
{
	uint8_t	GetChar;
	
	if (g_tSystemCtrl.PrtModeFlag == STD_MODE)
	{
		GetChar = ReadInBuffer();
		g_tPrtModeHZ.LeftSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		GetChar = ReadInBuffer();
		g_tPrtModeHZ.RightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
	}
	else																	 //	ҳģʽ
	{
		GetChar = ReadInBuffer();
		g_tPageMode.HZLeftSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
		GetChar = ReadInBuffer();
		g_tPageMode.HZRightSpace = GetChar * g_tPrtCtrlData.StepHoriUnit;
	}
}

/*******************************************************************************
* Function Name  : Command_1C55
* Description    : ���ú��ֺ���Ŵ���,
*	1C 55 n	΢������Ϊ���ú��ֺ���Ŵ�n��,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C55(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification	= GetChar;	//���ֺ�����
	}
}

/*******************************************************************************
* Function Name  : Command_1C56
* Description    : ���ú�������Ŵ���,
*	1C 56 n	΢������Ϊ���ú�������Ŵ�n��,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C56(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = GetChar;	//����������
	}
}

/*******************************************************************************
* Function Name  : Command_1C57
* Description    : ѡ��/ȡ�����ֱ�����
1C   57   n
ѡ�����ȡ�����ֱ��߱���ģʽ�� 
��  ��n�����λΪ0��ȡ�����ֱ��߱���ģʽ�� 
��  ��n�����λΪ1��ѡ���ֱ��߱���ģʽ�� 
��  ֻ��n�����λ��Ч              
��  �ں��ֱ��߱���ģʽģʽ�£���ӡ���ֵĴ�С����ͬʱѡ�񱶿�ͱ���ģʽʱ��ͬ��  
��  ȡ�����ֱ��߱���ģʽ���Ժ��ӡ���ĺ���Ϊ������С�� 
��  ��һ�������ַ��߶Ȳ�ͬʱ�������е������ַ��������߶��롣 
��  Ҳ����ͨ��FS ! ����  GS !ָ��(ѡ�񱶸ߺͱ���ģʽ)��ѡ���ȡ�����ֱ��߱���
	ģʽ�������յ���ָ����Ч�� 
[ Ĭ��ֵ]    n = 0 
΢������Ϊ���ú��ֺ�������Ŵ�n��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C57(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();
	if (g_tSysConfig.CommandSystem == CMD_SYS_ESCPOS)		//EPSON���,ҳģʽ�����ô�ӡ����
	{
		GetChar &= 0x01;		//���λ��Ч
		GetChar++;					//Ϊ0ʱ�Ŵ���Ϊ1
		
		g_tPrtModeHZ.WidthMagnification = GetChar;		//���ֿ�
		g_tPrtModeHZ.HeightMagnification = GetChar;		//���ָ�
	}
	else  //΢�����,�������Ŵ�
	{
		GetChar &= 0x0F;
		if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X) || (GetChar > MAX_RATIO_Y))
		{
			return;
		}
		else
		{
			g_tPrtModeHZ.WidthMagnification = GetChar;		//���ֿ�
			g_tPrtModeHZ.HeightMagnification = GetChar;		//���ָ�
		}
	}
}

/*******************************************************************************
* Function Name  : Command_1C58
* Description    : ���ú����������Ŵ���,
*	1C 58 n1 n2	΢������Ϊ���ú��ֺ���Ŵ�n1��,����Ŵ�n2��,
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C58(void)
{
	uint8_t	GetChar;
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_X))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.WidthMagnification	= GetChar;	//���ֺ�����
	}
	
	GetChar = ReadInBuffer();
	GetChar &= 0x0F;
	if ((GetChar == 0x00) || (GetChar > MAX_RATIO_Y))
	{
		;
	}
	else
	{
		g_tPrtModeHZ.HeightMagnification = GetChar;	//����������
	}
}

void Command_1C67(void)	//�����
{
}

/*******************************************************************************
* Function Name  : Command_1C70
* Description    : ��ӡԤ����NVλͼ
Hex 1C 70 n	m
1 �� n �� 255
0 �� m �� 3, 48 �� m �� 51
Prints NV bit image n using the process of  FS q and using the mode  specified by m.
[ ����]	 ����m ȷ����ģʽ��ӡλͼ��
	m					ģʽ		����ֱ��� (DPI)	����ֱ��� (DPI)
0, 48		����						203									203
1, 49		����						203									101
2, 50		����						101									203
3, 51		����,����			101									101
[Recommended Functions]
This function is supported only by some printer models and may not be supported by 
future models.It is recommended that NV graphics function 
(GS ( L GS 8 L : <Function 51> and <Function 64> ~ <Function 69>) be used because 
they offer the follow ing additional features: 
	Multiple logo data and mark  data can be specified (except for some models).
	Data can be controlled by key code.
	Redefining or deleting the same data is possible for each key code.
	Color can be specified  for the definition data.
	Data can be defined by raster format.
	The remaining capacity of the definition area can be confirmed.
[Notes]
�� This command is not effective when the NV bit image specified by n has not been defined.
�� In standard mode, this command is effective only when there is no data in the print 
	buffer and the printer is at the beginning of the line.
�� In page mode, the NV bit image is only stored in the print buffer and is not printed.
�� If the NV bit image exceeds one line of pr int area, the printe r does not print it.
�� The scales for width and height of NV bit images are specified by m. Therefore, in 
	page mode with 90�� or 270 �� clockwise-rotated NV bit image, the printer applies print 
	area and dot density from [width: direction of paper feed, height: perpendicular to 
	direction of paper feed].
�� This command is not affected by print modes (such as emphasized, underline, character size, 
	or 90�� rotated characters), except upside-down print mode.
	���ô�ӡ��Ч.������Ч.
�� This command executes paper feed for amount needed for printing the NV bit image regardless 
	of paper feed amount set by a paper feed setting command.
�� After printing the bit image, this command sets the print position to the beginning of the line.
�� When printing the NV bit image, selecting unidirectional print mode with ESC U enables 
	printing patterns in which the top and bottom parts are aligned vertically. 
�� The NV bit image is defined by FS q.
�� NV bit image is printed in the default dot density (dot density of vertical and horizontal
	direction in normal mode) defined by GS L <Function 49>.
�� Bit image data and print result are as follows:

d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
Y = yL + yH �� 256

* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	BIT_BMP_X_BYTE_MAX				(1023)		//����λͼX��������ֽ���
#define	BIT_BMP_Y_BYTE_MAX				(288)			//����λͼY��������ֽ���
#define	BIT_BMP_XH_MAX						(3)				//����λͼX��������ֽ�����8λ���ֵ
#define	BIT_BMP_YH_MAX						(1)				//����λͼY��������ֽ�����8λ���ֵ

//����λͼ�����ܵ��ֽ���
#define	BIT_BMP_MAX_BYTE					(BIT_BMP_X_BYTE_MAX * BIT_BMP_Y_BYTE_MAX * 8)

//����λͼ����ռ�õ����ռ�,�����ڻ��͵Ĵ洢�ռ�
#define	BIT_BMP_MAX_STORE_BYTE		(BIT_BMP_Y_BYTE_MAX * PRT_DOT_NUMBERS)

#define	MAX_BIT_BMP_NUM						(255)			//�����ܵ�����ͼƬ����Ŀ

/******* ����NV����λͼ���ͽṹ *************/
typedef struct
{
	uint8_t	ImageNum;		//�����ͼƬ��Ŀ
	
	//���ÿ��ͼƬX,Y����������ֽ���Ŀֵ,ÿ��ͼƬX������ǰ,Y�����ں�,xyL��ǰ,xyH�ں�;
	uint8_t	XYByteNum[MAX_BIT_BMP_NUM * 4];
}
NVFlASH_BMP_Struct;

void Command_1C70(void)
{
	
	uint8_t	FlashBitBmpNum;
	uint8_t	DataBuf[BYTES_OF_HEAT_ELEMENT];
	uint16_t High;
	uint16_t Width;
	uint16_t RealWidth;
	uint32_t SectorAddress;
	uint32_t i;
	
	FlashBitBmpNum = ReadInBuffer();	//ͼƬ�����к�
	if(FlashBitBmpNum>0)							//2016.08.30  1C70����n��0��8 �����ù�������λͼλ�ö�Ӧ
	{
		FlashBitBmpNum = FlashBitBmpNum-1;
	}	
	
	if (FlashBitBmpNum < MAX_DOWNLOAD_NVFLASH_BITMAP_NUM)	//�������Ϸ���Χ, ���򲻴���  7��ͼ
	{
		memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
		//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
		SectorAddress = BIT_BMP_DATA_BASE_ADDR + 0x10000 * FlashBitBmpNum;	//��ʼ�洢��ַ
		sFLASH_ReadBuffer(DataBuf, SectorAddress, 5);
		if (DataBuf[0] == FlashBitBmpNum)	//���ͼƬ�Ƿ��Ѿ�������
		{
			//Width = DataBuf[1] + 256 * DataBuf[2];
			//High = DataBuf[3] + 256 * DataBuf[4];
			Width	= (uint16_t)DataBuf[1] + (uint16_t)(((uint16_t)DataBuf[2]) << 8);	//��ȷ������ֽ���
			High	= (uint16_t)DataBuf[3] + (uint16_t)(((uint16_t)DataBuf[4]) << 8);	//�߶ȷ����ϵ���
			
			if (Width > (g_tSystemCtrl.LineWidth >> 3))
				RealWidth = (g_tSystemCtrl.LineWidth >> 3);		//ʵ�ʴ�ӡ�Ŀ���ֽ���
			else
				RealWidth = Width;
			
			//while (PRINT_PROCESS_STATUS_BUSY == GetPrintProcessStatus());	//�ȴ�֮ǰ��ӡ����
			
			SectorAddress += 5;
			for (i = 0; i < High; i++)				//��ӡһ����
			{
				memset(DataBuf, 0x00, BYTES_OF_HEAT_ELEMENT);
				//memset(DataBuf, 0x00, sizeof(DataBuf) / sizeof(DataBuf[0]));
				sFLASH_ReadBuffer(DataBuf, SectorAddress, RealWidth);
				SectorAddress += Width;
				DotLinePutDrvBuf( DataBuf );				//2016.06.16 ��һ�����ݵ�������ӡ������
			}
		}	//����ӡͼƬ�Ѿ������˴������
	}	//������Χ�Ϸ�����
	
// 	uint8_t	GetChar;
// 	uint8_t	cImageNum;	//����ӡ��λͼ���
// 	uint8_t	RatioX;
// 	uint8_t	RatioY;			//��������Ŵ���
// 	uint8_t	ratiox;
// 	uint8_t	ratioy;			//���ʱ��ѭ������
// 	uint8_t	TempBuf[MAX_RATIO_Y];
// 	
// 	uint16_t	SpaceDot;						//��ӡʱ�Ŀհ���
// 	uint16_t	sXByteNum;					//�����ֽ���
// 	uint16_t	sYByteNum;					//�����ֽ���
// 	uint16_t	MaxX_width;					//ʵ�ʴ����ӡ����
// 	uint16_t	x;
// 	
// 	uint32_t	iCount;							//ѭ������
// 	uint32_t	iAddress;						//λͼ�洢��ַ
// 	uint32_t	iColumnAddress;
// 	
// 	NVFlASH_BMP_Struct tNVFlashImage;	//NVλͼ���Ʋ�������
// 	
// 	uint8_t	* pbByteStart = NULL;
// 	uint8_t	* pbTemp = NULL;
// 	uint8_t	* pbColumn = NULL;
// 	
// 	//ֻ�ڱ�׼ģʽ���������ײ���Ч
// 	if (!((g_tSystemCtrl.PrtModeFlag == STD_MODE) && \
// 				(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)))
// 		return;
// 	
// 	cImageNum = ReadInBuffer();	//����ӡ��ͼƬ���
// 	GetChar = ReadInBuffer();		//��ӡģʽ
// 	if (((GetChar > 3) && (GetChar < 48)) || (GetChar > 51))	//m������Ч����
// 		return;
// 	
// 	if (cImageNum == 0)					//��������ȷ
// 		return;
// 	
// 	//��ȡ�洢����ͼƬ��Ŀ
// 	sFLASH_ReadBuffer(&tNVFlashImage.ImageNum, BIT_BMP_PARA_BASE_ADDR, 1);
// 	if (cImageNum > tNVFlashImage.ImageNum)	//����ӡ��ͼƬ��δ����
// 		return;
// 	
// 	RatioX = (GetChar & 0x01) + 1;					//��ȱ���,���Ϊ2
// 	RatioY = ((GetChar & 0x02) >> 1) + 1;		//�߶ȱ���,���Ϊ2
// 	
// 	//������ָ��ͼƬΪֹ������ͼƬ�Ĳ���
// 	pbByteStart = tNVFlashImage.XYByteNum;
// 	sFLASH_ReadBuffer(pbByteStart, (BIT_BMP_PARA_BASE_ADDR + 1), (cImageNum * 4));
// 	
// 	//���㱾����Ҫ��ӡ��ͼƬ�Ĵ洢�׵�ַ
// 	iAddress = BIT_BMP_DATA_BASE_ADDR;
// 	pbByteStart = tNVFlashImage.XYByteNum;
// 	for (iCount = 0; iCount < (cImageNum - 1); iCount++)
// 	{
// 		sXByteNum = *pbByteStart++;									//�����ֽ�����λ
// 		sXByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//�����ֽ�����λ
// 		pbByteStart++;
// 		sYByteNum = *pbByteStart++;									//�����ֽ�����λ
// 		sYByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//�����ֽ�����λ
// 		pbByteStart++;
// 		//���ͼƬ����ռ���ֽ����ó����δ���ӡͼƬ���׵�ַ
// 		iAddress += (((uint32_t)sXByteNum) * ((uint32_t)sYByteNum) * 8);
// 	}
// 	
// 	//���㱾����Ҫ��ӡ��ͼƬ��X,Y������ֽ���
// 	sXByteNum = *pbByteStart++;									//�����ֽ�����λ
// 	sXByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//�����ֽ�����λ
// 	pbByteStart++;
// 	sYByteNum = *pbByteStart++;									//�����ֽ�����λ
// 	sYByteNum += (uint16_t)(((uint16_t)(*pbByteStart)) << 8);	//�����ֽ�����λ
// 	
// 	x = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtLeftLimit;	//����ɴ�ӡ������
// 	if (((sXByteNum << 3) * RatioX) > x)
// 		MaxX_width = x / RatioX;
// 	else
// 		MaxX_width = (sXByteNum << 3);
// 	
// 	SpaceDot = g_tPrtCtrlData.PrtLeftLimit;
// 	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)			//�ַ�����ģʽ,����
// 		SpaceDot += ((x - MaxX_width * RatioX) >> 1);
// 	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//����
// 		SpaceDot += (x - MaxX_width * RatioX);
// 	
// 	//�����ӡ�����п�ʼȡ��,�����ӡ�����һ�п�ʼȡ��
// 	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
// 	{
// 		iAddress += (sYByteNum - 1);	//�Ӹ߶������һ�п�ʼ
// 	}
// 	
// 	//���ݸ߶��ֽ���ÿ�ζ�ȡ1�ֽڵ����ݴ�ӡ.ÿ�δ���һ�ֽ���.
// 	//���м���������ʼָ��
// 	pbByteStart = g_tLineEditBuf + SpaceDot + \
// 		(MAX_PRT_HEIGHT_BYTE - RatioY) * PRT_DOT_NUMBERS;
// 	for (iCount = 0; iCount < sYByteNum; iCount++)
// 	{
// 		iColumnAddress = iAddress;	//ÿһ�е����������ʼ��ַ
// 		//ָ��ǰ�༭���ݻ�������ʼ�ֽ���,ÿ����������1���ֽ�,�ı���ֵ
// 		pbColumn = pbByteStart;
// 		for (x = 0; x < MaxX_width; x++)	//������,�Ե�Ϊ��λ,��ֵ
// 		{
// 			sFLASH_ReadBuffer(&GetChar, iColumnAddress, 1);	//��ȡ1�ֽ�����
// 			if (GetChar)				//���ݲ�Ϊ0ʱȫ����
// 			{
// 				if (RatioY > 1)		//��Ҫ�Ŵ�
// 				{
// 					memset(TempBuf, 0x00, MAX_RATIO_Y);
// 					//memset(TempBuf, 0x00, sizeof(TempBuf) / sizeof(TempBuf[0]));
// 					ByteZoomCtrl(RatioY, GetChar, TempBuf);
// 				}
// 				else		//����Ŵ���Ϊ1
// 				{
// 					TempBuf[0] = GetChar;
// 				}
// 				pbTemp = pbColumn;
// 				for (ratiox = 0; ratiox < RatioX; ratiox++)
// 				{
// 					for (ratioy = 0; ratioy < RatioY; ratioy++)
// 					{
// 						*(pbTemp + ratioy * PRT_DOT_NUMBERS) = TempBuf[ratioy];
// 					}
// 					pbTemp++;
// 				}
// 			}	//һ����Ϊ0�ֽ�������,����Ϊ0ʱֱ���޸�ָ��
// 			iColumnAddress += sYByteNum;	//ָ����һ�е����ݵĴ洢��ַ
// 			pbColumn += RatioX;
// 		}		//��䴦��1�����
// 		
// 		if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)
// 			iAddress--;	//ָ����һ�����ݴ洢�׵�ַ,���ô�ӡʱ�����һ�е�ȡ��
// 		else
// 			iAddress++;	//ָ����һ�����ݴ洢�׵�ַ,�����ӡ������˳��ȡ��
// 		
// 		//�����ϴ�ӡ����
// 		g_tPrtCtrlData.PrtDataDotIndex += (SpaceDot + MaxX_width * RatioX);
// 		g_tPrtCtrlData.BitImageFlag = 1; 	//λͼ��ӡ��־
// 		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)((uint16_t)RatioY << 3);	//�����ܵ���
// // 		PrintOneLine();									//��ӡ�ñ༭������
// 		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
// 	}	//�����д������
}

void	WriteNVFlashImageParameter(uint8_t *pbBuf, uint32_t	iAddress, uint16_t iLen)
{
	sFLASH_EraseSector(iAddress);
	sFLASH_WriteBuffer(pbBuf, iAddress, iLen);
}

/*******************************************************************************
* Function Name  : Command_1C71
* Description    : Ԥ����NVλͼ
Hex 1C 71 n[xL xH yL yH d1...dk]1...[xL xH yL yH d1...dk]n
1 �� n �� 255
1 �� (xL + xH �� 256) �� 1023 (0 �� xL �� 255, 0 �� xH �� 3)
1 �� (yL + yH �� 256) �� 288  (0 �� yL �� 255, yH = 0, 1)
0 �� d �� 255
k = (xL + xH �� 256) �� (yL + yH �� 256) �� 8
The definition area is 256 KB
Defines the NV bit image in the NV graphics area.
 n specifies the number of defined NV bit images.
 xL,xH specifies (xL + xH �� 256) bytes in the horizontal direction for the NV 
			bit image you defined.
 yL,yH specifies (yL + yH �� 256) bytes in the vertical directi on for the NV 
			bit image you defined.
 d specifies the definition data fo r the NV bit image (column format).
 k indicates the number of the definition data. k is an explanation parameter;
			therefore it does not need to be transmitted.
��������ˮƽ�����ֽ���,��ֱ�����ֽ���,����ȡ����,���ֽ�����Ҫ*8.
[Recommended Functions]
	This function is supported only by some printer models and may not be 
	supported by future models. It is recommended that NV graphics function 
	( GS ( L  GS 8 L :  <Function 51> and <Function 64> ~ <Function 69>) be used 
	instead of FS q because the NV graphics  function offers t he following 
	additional features: 
	Multiple logo data and mark  data can be specified (except for some models).
	Data can be controlled by key code.
	Redefining or deleting is possible for each key code.
	Color can be specified  for the definition data.
	Data can be defined by raster format.
	The remaining capacity of the definition area can be confirmed.
	Continuous processing possible (without a software reset when a command has been processed).
[Notes]
	NV bit image means a bit image which is defined in a non-volatile memory. 
	The NV bit image defined is effective until the next NV bit image is defined.
�� In standard mode, this command is effective only when processed at the beginning of the line. 
�� If this command is processed while a macro is being defined, the printer cancels 
	macro definition and starts processing this command. At this time, the macro becomes undefined.
�� k bytes data of d1...dk is processed as a defined data of a NV bit image. 
	The defined data (d ) specifies a bit printed to 1 and not printed to 0. 
�� All NV bit images previously defined are canceled.
�� After processing this command, the printer executes a software reset. 
	Therefore, processing this command enables the printer to be in the correct 
	status when the power is turned on.
�� The limitations during processing of this command are as follows:
		Even if the PAPER FEED button is pressed, the printer does not feed paper.
		The real-time commands are not processed.
		Even if the ASB function is effective, the ASB status cannot be transmitted.
�� The NV bit image is printed by FS p.
�� Bit image data and print result are as follows:
d1	dY + 1	...		:				MSB
													LSB
d2	dY + 2	...		dk - 2	MSB
													LSB
:		:				...		dk - 1	MSB
													LSB
dY	dY * 2	...		dk			MSB
													LSB
Y = yL + yH �� 256
�� Data is written to the non-volatile memory by this command. 
	Note the following when using this command. 
	1.	Do not turn off the power or reset the printer from the interface 
			when this command is being executed.
	2.	The printer is BUSY when writing the data to the non-volatile memory. 
			In this case, be sure not to transmit data from the host because 
			the printer does not receive data.
	3.	Excessive use of this function may destroy the non-volatile memory. 
			As a guideline, do not use any combination of the following commands 
			more than 10 times per day for writing data to the non-volatile memory: 
			GS ( A (part of functions), GS ( C (part of functions), 
			GS ( E (part of functions), GS ( L  /  GS 8 L (part of functions),  
			GS ( M (part of functions),  GS g 0,  FS g 1 , FS q.

* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1C71(void)
{
	uint8_t	GetChar;
	uint8_t	cTotalImageNum;			//�����λͼ�ķ���
	uint8_t	cImageNumCount;			//ѭ������
	uint8_t	RealTimeEnable;			//�ر�ʵʱָ��
	uint8_t	KeyEnable;					//�رհ�������
	uint8_t	ASBEnable;					//�ر�ASB����
	uint16_t	sXByteNum;				//�����ֽ���
	uint16_t	sYByteNum;				//�����ֽ���
	uint32_t	iOneImageByteNum;	//һ��ͼ������ֽ���,���ڼ����������ֵ
	uint32_t	iStoreByteNum;		//ʵ��������Ĵ洢�ֽ���
	uint32_t	iCount;						//ѭ������
	uint32_t	iAddress;					//λͼ�洢��ַ
	uint32_t	iUsedStoreSpace;
	NVFlASH_BMP_Struct tNVFlashImage;	//NVλͼ���Ʋ�������
	uint8_t	* pbByte = NULL;

#ifdef	DBG_ZIKU_SPI_WRITE
	uint8_t	bBuf;
	uint8_t	bBufTemp[16];
	uint32_t	iDbgCount;
	uint32_t	iDbgAddress;
#endif
	
	//ֻ�ڱ�׼ģʽ���������ײ���Ч
	if (!((g_tSystemCtrl.PrtModeFlag == STD_MODE) && \
			(g_tPrtCtrlData.PrtDataDotIndex == g_tPrtCtrlData.PrtLeftLimit)))
		return;
	
	if (g_tMacro.DefinitionFlag)	//�궨����,�˳��궨��,�����
	{
		g_tMacro.Number = 0;				//���ַ���������
		g_tMacro.GetPointer = 0;
		g_tMacro.DefinitionFlag = 0;
		g_tMacro.SetFlag = 0;				//�����ı�־����
		g_tMacro.RunFlag = 0;
	}
	
	cTotalImageNum = ReadInBuffer();	//�����λͼ�ķ���
	if (cTotalImageNum == 0x00)
		return;
	
	//�ر��Զ��ش�,��ֹ����,��ֹʵʱָ��
	KeyEnable = g_tFeedButton.KeyEnable;
	g_tFeedButton.KeyEnable	= 1;
	RealTimeEnable = g_tRealTimeCmd.RealTimeEnable;
	g_tRealTimeCmd.RealTimeEnable = 0;
	ASBEnable = g_tError.ASBAllowFlag;
	g_tError.ASBAllowFlag = 0;
	
#if	1
	//������ǰ���������NVλͼ,�Կ�Ϊ��λ����,ÿ����64KB
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������64KΪ��λ�����ĵ�ַ
	for (iCount = 0; iCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_BLOCKSIZE); iCount++)
	{
		sFLASH_EraseBlock(iAddress);
		iAddress += sFLASH_SPI_BLOCKSIZE;
	}
#else
	//������ǰ���������NVλͼ,������Ϊ��λ����,ÿ������4KB
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������4KΪ��λ�����ĵ�ַ
	for (iCount = 0; iCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_SECTORSIZE); iCount++)
	{
		sFLASH_EraseSector(iAddress);
		iAddress += sFLASH_SPI_SECTORSIZE;
	}
#endif

#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_DATA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������64KΪ��λ�����ĵ�ַ
	//��ȡÿ����Ĵ洢�����ֽ����ݷ��͵�PC
	for (iDbgCount = 0; iDbgCount < (BIT_BMP_MAX_STORE_SPACE / sFLASH_SPI_BLOCKSIZE); iDbgCount++)
	{
		sFLASH_ReadBuffer(&bBuf, iDbgAddress, 1);
		UpLoadData(&bBuf, 1);
		iDbgAddress += sFLASH_SPI_BLOCKSIZE;
		//iDbgAddress += sFLASH_SPI_SECTORSIZE;
	}
#endif

	//�������Ʋ����洢����,������Ϊ��λ����,ÿ����4K
	iAddress = BIT_BMP_PARA_BASE_ADDR;
	sFLASH_EraseSector(iAddress);
	tNVFlashImage.ImageNum = 0;			//��տ��Ʋ���
#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_PARA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������64KΪ��λ�����ĵ�ַ
	for (iDbgCount = 0; iDbgCount < sizeof(tNVFlashImage); iDbgCount++)	//��ȡ�����������ݷ��͵�PC
	{
		sFLASH_ReadBuffer(&bBuf, iDbgAddress++, 1);
		UpLoadData(&bBuf, 1);
	}
#endif
	
	//����ͼƬ��Ŀ,���ͼƬ���д洢
	pbByte = tNVFlashImage.XYByteNum;
	iUsedStoreSpace = 0;
	iAddress = BIT_BMP_DATA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������64KΪ��λ�����ĵ�ַ
	for (cImageNumCount = 0; cImageNumCount < cTotalImageNum; cImageNumCount++)
	{
		//��ȡͼƬ�ĺ��������ֽ���
		sXByteNum = ReadInBuffer();									//�����ֽ�����λ
		sXByteNum += (uint16_t)(((uint16_t)ReadInBuffer()) << 8);	//�������ֽ���ֵ
		if ((sXByteNum < 1) || (sXByteNum > BIT_BMP_X_BYTE_MAX))	//��������Χ
			return;
		
		sYByteNum = ReadInBuffer();									//�����ֽ�����λ
		sYByteNum += (uint16_t)(((uint16_t)ReadInBuffer()) << 8);	//����߶��ֽ���ֵ
		if ((sYByteNum < 1) || (sYByteNum > BIT_BMP_Y_BYTE_MAX))	//��������Χ
			return;
		
		iOneImageByteNum = sXByteNum * sYByteNum;
		iOneImageByteNum <<= 3;									//1��ͼƬ��������,X*Y*8,����Ҫ��ȡ����������
		if (sXByteNum > (PRT_DOT_NUMBERS / 8))	//�˴����в������,��������ֽ�ֵ
			sXByteNum = (PRT_DOT_NUMBERS / 8);		//��ӡ�������,�������
		if (sYByteNum > BIT_BMP_Y_BYTE_MAX)			//��������ֽ�ֵ
			sYByteNum = BIT_BMP_Y_BYTE_MAX;
		
		//������Ԥ���洢�ռ估�ɴ�ӡ����,ֻ�洢��Ч�Ŀɴ�ӡ����,����������
		iStoreByteNum = sXByteNum * sYByteNum;
		iStoreByteNum <<= 3;
		
		//���ʣ��ռ��Ƿ���Է��±���ͼƬ
		iUsedStoreSpace += iStoreByteNum;	//��Ҫʹ�õ��ܿռ�
		if (iUsedStoreSpace > BIT_BMP_MAX_STORE_SPACE)
		{
			PrintString("No enough NV FLASH space to store.");
			PrintString("\n");
			break;
		}
		
		//���ֽڴ洢����
		for (iCount = 0; iCount < iStoreByteNum; iCount++)
		{
			GetChar = ReadInBuffer();
			sFLASH_WriteBuffer(&GetChar, iAddress, 1);
#ifdef	DBG_ZIKU_SPI_WRITE
			sFLASH_ReadBuffer(&bBuf, iAddress, 1);
			UpLoadData(&bBuf, 1);
#endif
			iAddress++;
		}
		
		//��ȡ���ܵĶ�������
		for (; iCount < iOneImageByteNum; iCount++)
		{
			ReadInBuffer();
		}
		
		//������һ��ͼƬ,���¿��Ʋ���ֵ
		tNVFlashImage.ImageNum++;
		*pbByte++ = (uint8_t)sXByteNum;
		*pbByte++ = (uint8_t)((sXByteNum & 0xFF00) >> 8);
		*pbByte++ = (uint8_t)sYByteNum;
		*pbByte++ = (uint8_t)((sYByteNum & 0xFF00) >> 8);
	}
	
	//����NVλͼ���Ʋ���
	iAddress = BIT_BMP_PARA_BASE_ADDR;
	GetChar = tNVFlashImage.ImageNum;
	sFLASH_WriteBuffer(&GetChar, iAddress++, 1);	//����ͼƬ��Ŀ
	pbByte = tNVFlashImage.XYByteNum;
	iStoreByteNum = tNVFlashImage.ImageNum * 4;
	sFLASH_WriteBuffer(pbByte, iAddress, iStoreByteNum);	//����ÿ��ͼƬ��X,Yֵ
#ifdef	DBG_ZIKU_SPI_WRITE
	iDbgAddress = BIT_BMP_PARA_BASE_ADDR;	//ÿ������׵�ַ,�׵�ַ��������64KΪ��λ�����ĵ�ַ
	pbByte = bBufTemp;
	for (iDbgCount = 0; iDbgCount < (iStoreByteNum + 1); iDbgCount++)	//��ȡ�����������ݷ��͵�PC
	{
		sFLASH_ReadBuffer(pbByte, iDbgAddress++, 1);
		UpLoadData(pbByte, 1);
	}
#endif
	
	//�ָ��ֳ�
	g_tFeedButton.KeyEnable = KeyEnable;
	g_tRealTimeCmd.RealTimeEnable = RealTimeEnable;
	g_tError.ASBAllowFlag = ASBEnable;
	//����Ҫִ��ϵͳ��λ
}

/*******************************************************************************
* Function Name  : EscCommand
* Description    : ESSC����Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FS_Command(void)
{
	uint8_t	GetChar;

	GetChar = ReadInBuffer();

	switch (GetChar)
	{
		case 0x21:
		{
			Command_1C21();		//���ú��ִ�ӡģʽ
			break;
		}
		case 0x26:
		{
			Command_1C26();		//���뺺��ģʽ
			break;
		}
		case 0x2D:
		{
			Command_1C2D();		//����/ȡ�������»���
			break;
		}
		case 0x2E:
		{
			Command_1C2E();		//�˳�����ģʽ
			break;
		}
		case 0x32:
		{
			Command_1C32();		//�Զ��庺��
			break;
		}
		case 0x3F:
		{
			Command_1C3F();		//ȡ���û��Զ��庺��
			break;
		}
		case 0x43:
		{
			Command_1C43();		//ѡ���ִ��뼯
			break;
		}
		case 0x50:
		{
			Command_1C50();		//��ӡԤ����λͼ
			break;
		}
		case 0x53:
		{
			Command_1C53();		//���ú������Ҽ��
			break;
		}
		case 0x55:
		{
			Command_1C55();		//΢������Ϊ���ú��ֺ���Ŵ�n��,
			break;
		}
		case 0x56:
		{
			Command_1C56();		//΢������Ϊ���ú�������Ŵ�n��,
			break;
		}
		case 0x57:
		{
			Command_1C57();		//ѡ��/ȡ�����ֱ�����ģʽ
			break;
		}
		case 0x58:
		{
			Command_1C58();		//΢������Ϊ���ú��ֺ���Ŵ�n1��,����Ŵ�n2��,
			break;
		}
		case 0x70:
		{
			Command_1C70();		//��ӡԤ����NVλͼ
			break;
		}
		case 0x71:
		{
			Command_1C71();		//����NVλͼ
			break;
		}
		
		default:
			break;
	}		 	
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
