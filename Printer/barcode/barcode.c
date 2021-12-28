/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   һά������صĳ���.
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
#include	"barcode.h"
#include	"extgvar.h"		//ȫ���ⲿ��������
#include	"timconfig.h"
#include	"feedpaper.h"
#include	"print.h"
#include	"heat.h"
#include	"charmaskbuf.h"
#include	"ean.h"

extern TypeDef_StructHeatCtrl	g_tHeatCtrl;
//extern TypeDef_StructLinePrtBuf	g_tLinePrtBuf;

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
/*****************************
*EAN ����ȷ����������ַ����Ӽ�,"0"��ʾA�Ӽ�,"1"��ʾB�Ӽ� 
*��D0��D5�ֱ��ʾ�ڴ��ҵ����7��12λ
*���ݹ������ͷһλȷ�������Ϸ��ı��뷽ʽ,������ֵ�ĵ�6λ��Ч
*******************************/
const uint8_t EANCharSel[] =
{
	0x00,0x0B,0x0D,0x0E,0x13,0x19,0x1C,0x15,0x16,0x1A,
};

/***����ȷ�������ַ����Ӽ�,"0"��ʾA�Ӽ�,"1"��ʾB�Ӽ� 
*��D0��D5�ֱ��ʾ���ҵ���ĵ�2��7λ
*******************************/
const uint8_t UPCCharSel[] =
{
	0x38,0x34,0x32,0x31,0x2C,0x26,0x23,0x2A,0x29,0x25 
};

/***����ȷ��UPC-E��6λ�����ַ����Ӽ�,"0"��ʾA�Ӽ�,"1"��ʾB�Ӽ� 
*��D5��D0�ֱ��ʾ�����ҵ�6λ�������5��0λ,���뷽ʽͬUPC-A
*******************************/
const uint8_t UPCECharSel[] =
{
	0x38,0x34,0x32,0x31,0x2C,0x26,0x23,0x2A,0x29,0x25 
};

/**********EAN13 A�ַ�����ģʽ *******/
/*ENA-��������A*/
const uint8_t EAN13_A[] = 
{
	0x0d,0x19,0x13,0x3d,0x23,0x31,0x2f,0x3b,0x37,0x0b
};

/**********EAN13 B�ַ�����ģʽ *******/
/*ENA-��������B*/
const uint8_t EAN13_B[] = 
{
	0x27,0x33,0x1b,0x21,0x1d,0x39,0x05,0x11,0x09,0x17
};

/**********EAN13 C�ַ�����ģʽ *******/
/*ENA-��������,��������ͬ��UPC-A�����ϱ���*/
const uint8_t EAN13_C[] = 
{
	0x72,0x66,0x6c,0x42,0x5c,0x4e,0x50,0x44,0x48,0x74
};

/* ENA��ʼ������ */
const uint8_t	ENAStartCode = 0x05;

/* ENA���������� */
const uint8_t	ENAStopCode = 0x05;

/* ENA�м������ */
const uint8_t	ENADivideCode = 0x0A;

/********* Code39�ַ��� ********************/
const uint8_t *codebuf39 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%*";

/***********CODE39 ������ģ***************************
*��ģ���ݵ�10λ��Чʼ,ÿλ�����ʾ������,��10λ(�����ָ���),
*0��ʾխ��,1��ʾ����.�ָ���Ϊһխ��
*************************************************************/
const uint16_t Code39buf[] = 
{
	0x068,0x242,0x0C2,0x2C0,0x062,0x260,0x0E0,0x04A,0x248,0x0C8,	//0��9
	0x212,0x092,0x290,0x032,0x230,0x0B0,0x01A,0x218,0x098,0x038,	//A��J
	0x206,0x086,0x284,0x026,0x224,0x0A4,0x00E,0x20C,0x08C,0x02C,	//K��T
	0x302,0x182,0x380,0x122,0x320,0x1A0,							//U~Z
	0x10A,0x308,0x188,0x150,0x144,0x114,0x054,				//-. $/+%*
	0x128,															//��ʼ��������
};

/***********ITF ������ģ*************************************
*��ģ���ݵ�5λ��Ч��0��ʾխ����1��ʾ������
*************************************************************/
const uint8_t CodeItf[] =
{
	0x06,0x11,0x09,0x18,0x05,0x14,0x0c,0x03,0x12,0x0a,
};

/*********** CODABAR �ַ���********************/
const uint8_t * codabar = "0123456789$-:/.+ABCD";

/*********** CODABAR ������ģ***********************************
*��ģ���ݴ����λ��ʼ,ÿλ�����ʾ������,��8λ(�����ָ���),
*0��ʾխ��,1��ʾ����.
*************************************************************/
const uint8_t Codabarbuf[] =
{
	0x07,0x0d,0x13,0xc1,0x25,0x85,0x43,0x49,0x61,0x91,
	0x31,0x19,0x8b,0xa3,0xa9,0x2b,0x35,0x53,0x17,0x1d,
};

/************ Code93 �ַ��� ********************/
const uint8_t * codebuf93 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%";

/*********** CODE93 ������ģ***********************************
*��ģ���ݵ�9λ(������10λ)��Ч,0��ʾ��,1��ʾ��.��Ϊխ����
*�ö��������0��1����ʾ����
*************************************************************/
const uint16_t code93buf[] =
{
	0x114,0x148,0x144,0x142,0x128,0x124,0x122,0x150,0x112,0x10A,	//0��9				 
	0x1A8,0x1A4,0x1A2,0x194,0x192,0x18A,0x168,0x164,0x162,0x134,	//A��J				
	0x11A,0x158,0x14C,0x146,0x12C,0x116,0x1B4,0x1B2,0x1AC,0x1A6,	//K~T			
	0x196,0x19A,0x16C,0x166,0x136,0x13A,		 		//U~Z
	0x12E,0x1D4,0x1D2,0x1CA,0x16E,0x176,0x1AE,	//-. $/+%
	0x126,0x1DA,0x1D6,0x132,	//ת����($)(%)(/)(+)
	0x15E,0x2BD,							//��ʼ����������														
};

/*********Code128 ������ģ *****
*��ģ���ݵ�11λ��������13λ����Ч,ÿλ�����ʾ������,0��ʾխ��,1��ʾ����.
***************************/
const uint16_t CODE128[]=
{
	0x6CC,0x66C,0x666,0x498,0x48C,
	0x44C,0x4C8,0x4C4,0x464,0x648,
	0x644,0x624,0x59C,0x4DC,0x4CE,
	0x5CC,0x4EC,0x4E6,0x672,0x65C,
	0x64E,0x6E4,0x674,0x76E,0x74C,
	0x72C,0x726,0x764,0x734,0x732,
	0x6D8,0x6C6,0x636,0x518,0x458,
	0x446,0x588,0x468,0x462,0x688,
	0x628,0x622,0x5B8,0x58E,0x46E,
	0x5D8,0x5C6,0x476,0x776,0x68E,
	0x62E,0x6E8,0x6E2,0x6EE,0x758,
	0x746,0x716,0x768,0x762,0x71A,
	0x77A,0x642,0x78A,0x530,0x50C,
	0x4B0,0x486,0x42C,0x426,0x590,
	0x584,0x4D0,0x4C2,0x434,0x432,
	0x612,0x650,0x7BA,0x614,0x47A,
	0x53C,0x4BC,0x49E,0x5E4,0x4F4,
	0x4F2,0x7A4,0x794,0x792,0x6DE,
	0x6F6,0x7B6,0x578,0x51E,0x45E,
	0x5E8,0x5E2,0x7A8,0x7A2,0x5DE,
	0x5EE,0x75E,0x7AE,0x684,0x690,
	0x69C,0x18EB,
};

//		0		1		2		3		4		5		6		7		8		9		10	11
enum {M6, M5, M4, M3, M2, M1, S5, S4, S3, S2, S1, CD};

/*******************************************************************************
* Function Name  : EanCheckCode(uint8_t *Inbuf, uint8_t length)
* Description    : ����Inbuf��ָ�����ȵ�У����,����У�������Inbuf��
* Input          : inbuf����������ָ�룬length�����ݳ���
* Output         :
* Return		 : 	
*******************************************************************************/
void	EanCheckCode(uint8_t * Inbuf, uint8_t Length)
{
	uint8_t	i;
	uint16_t	lsum;
	uint16_t	rsum;
	uint16_t	tsum;
	
	lsum = 0;
	rsum = 0;
	//����У���,Inbuf[0]���������У���
	for (i = 1; i <= Length; i++)
	{
		if (i % 2)
		{
			lsum += (*(Inbuf + Length - i) - '0');		//ż��λ�Ӻ�,��Ҫת��Ϊ0~9�������
		}
		else
		{
			rsum += (*(Inbuf + Length - i) - '0');		//����λ�Ӻ�,��Ҫת��Ϊ0~9�������
		}
	}
	
	tsum = 3 * lsum + rsum;
	i--;
	*(Inbuf + i) = ((10 - tsum % 10) % 10) + '0';	//���tsum�ĸ�λ��Ϊ0,��10-tsum��λ��=10,��ʱȡֵΪ0
	i++;
	*(Inbuf + i) = '\0';
}

/*******************************************************************************
* Function Name  : FormatOne(uint8_t *Dest, uint16_t Data , uint8_t Blength)
* Description    : ��1��ֻ��խ����������ŵ���ģ���ݷ���ָ���Ļ�������
* Input          : Dest�����ݴ���ָ�룬Data����ģ���ݣ�Blength����Ч����λ����
* Output         : 
* Return		 : 	
*******************************************************************************/
uint16_t FormatOne(uint8_t * Dest, uint16_t Data ,uint8_t Blength)
{
	uint8_t	i, j, Temp;
	uint16_t n = 0;
	
	//���ʱ��ָ�������λ��ʼ�����λ����
	for (i = 1; i <= Blength; i++)
	{
		if (Data & (uint16_t)(1 << (Blength - i)))
			Temp = 0xFF;
		else
			Temp = 0x00;
		for (j = 0; j < g_tBarCodeCtrl.BarCodeThinWidth; j++)
			Dest[n++] = Temp;
	}
	
	return n;
}

/*******************************************************************************
* Function Name  : EnaFillToBuf(uint8_t *Inbuf, uint8_t length,uint8_t PreCode)
* Description    : Inbuf�������õ�Ϊ��������,�����g_tUnderlineBuf��
* Input          : inbuf����������ָ��,
*						length�����ݳ���
*				   	PreCode��ǰ����,=0ʱ��ʾ�������ͼ��ô�
*						Tpye:��������,=1,UPC_E;=0,EAN-13,EAN-8,UPC-A
* Output         : 
* Return		 : 	
*******************************************************************************/
uint16_t EnaFillToBuf(uint8_t * inbuf, uint8_t Length, uint8_t PreCode, uint8_t Tpye)
{
	uint8_t i, Temp;
	uint8_t	outbuf[13];
	uint8_t	* p = NULL;
	uint16_t j;
	
	if (Tpye)		//UPC-E
	{
		Temp = UPCCharSel[PreCode];
		//���������ܿ��,����010101,�һ���101,���м���
		j = (7 * Length + 3 + 6) * g_tBarCodeCtrl.BarCodeThinWidth;
	}
	else				//EAN
	{
		Temp = EANCharSel[PreCode];		//�������������AB�������
		//���������ܿ��,���һ��߸�3,�м���5,������7
		j = (7 * Length + 3 + 5 + 3) * g_tBarCodeCtrl.BarCodeThinWidth;
	}
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
		return 0;		//������ӡ����,���ܷ�������������,����0
	
	//for (i = 0; i < (Length / 2); i++)	//���������,���7λ
	for (i = 0; i < (Length >> 1); i++)	//���������,���7λ
	{
		if ((uint8_t)(Temp << i) & 0x20)	//D5��ʾ�ַ���ѡ��,����Ϊ00xx xxxx,λֵΪ0��ʾA���ͱ���,λֵΪ1��ʾB���ͱ���
		{
			outbuf[i] = EAN13_B[*(inbuf + i) - 0x30];	//���յ�������ΪASCII��ֵ0~9,��Ҫת��������ֵ0~9
		}
		else
		{
			outbuf[i] = EAN13_A[*(inbuf + i) - 0x30];
		}
	}
	if (Tpye == 0)		//EAN
	{
		for (; i < Length; i++)		//ʣ���Ҳ�������
		{
		  outbuf[i] = EAN13_C[*(inbuf + i) - 0x30];
		}
	}
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	j = FormatOne(p, 0x05, 0x03);	//��ʼ��,����101,����3,����ֵj��ʾ������ֵ
	p += j;		//׼�������������
	
	//for (i = 0; i < (Length / 2); i++)
	for (i = 0; i < (Length >> 1); i++)
	{
		j = FormatOne(p, outbuf[i], 0x07);	//�����������,ÿ���볤��7,
		p += j;
	}
	
	if (Tpye == 0)	//����ENA���,UPC_Eû���м�ָ������Ҳ�����
	{
		j = FormatOne(p, 0x0A, 0x05);	//���ENA�м�ָ���,01010,����5
		p += j;
		for ( ; i < Length; i++)
		{
			j = FormatOne(p, outbuf[i], 0x07);	//�����������,ÿ���볤��7,
			p += j;
		}
		j = FormatOne(p, 0x05, 0x03);	//��������,�һ���101,����3
		p += j;
	}
	else
	{
		j = FormatOne(p, 0x15, 0x06);	//������,UPC_E�Ľ�������ͬ
		p += j;
	}
	//������pָ��Ϊ���һ������ͼ�ε���һ��λ��
	j = p - g_tUnderlineBuf;	//j = ԭg_tPrtCtrlData.PrtDataDotIndex + �����������ߵĳ���
	
	return j;		//����ֵΪ��������Ҫ��ӡ���»��ߵ�����
}

/*******************************************************************************
* Function Name  : FormatCodeUpc_A(uint8_t *inbuf)
* Description    : ��inbuf���������������ת��ΪUPC_A��������,���������g_tUnderlineBuf��.
* Input          : inbuf��������������
* Output         : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
*******************************************************************************/
uint16_t FormatCodeUpc_A(uint8_t * inbuf)
{
 	EanCheckCode(inbuf, 11);	//����Inbuf��ָ�����ȵ�У����,����У�������Inbuf��
 	return (EnaFillToBuf((inbuf), 12, 0, 0));
}

/*******************************************************************************
* Function Name  : FormatCodeUpc_E(uint8_t *inbuf)
* Description    : ��inbuf���������������ת��ΪUPC_A��������,���������g_tUnderlineBuf��.
* Input          : inbuf��������������
* Output         : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
* UPC-E��UPC-A��ļ���ʽ������뷽ʽ�ǽ�UPC-A������ѹ���ɶ��룬�Է���ʹ�ã�
*	����������ʽ�뾭��UPC-A����ת����UPC-E��6λ���������һ�����ɣ����м��ߡ�
*	6λ�����������Ϊ3��3ż�������з���ȡ��춼�����ֵ
*	UPC-E��ֻ��춹�����Ϊ0����Ʒ��
*******************************************************************************/
#if	0   //2016.07.02
//��Ϊ�޸ĺ�ĳ���,����8λ��������
uint16_t FormatCodeUpc_E(uint8_t * inbuf)
{	
	uint8_t i;	
	uint8_t	cCheckValue;
	uint8_t rule;
	uint8_t	outbuf[13];	
	uint8_t	* p = NULL;
	uint16_t j;
	
	if (*inbuf != '0')	//��λ������0��ʾ����������
		return 0;     // illegal; can't compress
	
	cCheckValue = *(inbuf + 7) - '0';	//������������,ֱ�Ӷ�ȡ,������ֵ����������6λ������
	rule = UPCCharSel[cCheckValue];		//6λ������ı��뷽ʽ����
	
	//���������ܿ��,����101,�һ���010101,���м���,6λ����,ÿλ����ռ��7λ
	j = (7 * 6 + 3 + 6) * g_tBarCodeCtrl.BarCodeThinWidth;
	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
		return 0;		//������ӡ����,���ܷ�������������,����0
		
	for (i = 0; i < 6; i++)	//6��������
	{
		if ((uint8_t)(rule << i) & 0x20)	//D5��ʾ�ַ���ѡ��,����Ϊ00xx xxxx,λֵΪ0��ʾA���ͱ���,λֵΪ1��ʾB���ͱ���
		{
			outbuf[i + 1] = EAN13_B[*(inbuf + 1 + i) - 0x30];	//���յ�������ΪASCII��ֵ0~9,��Ҫת��������ֵ0~9
		}
		else
		{
			outbuf[i + 1] = EAN13_A[*(inbuf + 1 + i) - 0x30];
		}
	}
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;	

	j = FormatOne(p, 0x05, 0x03);	//��ʼ��,����UPC_E��������UPC-A����ͬ,101,����3,����ֵj��ʾ������ֵ
	p += j;		//׼�����������
	
	for (i = 0; i < 6; i++)
	{
		j = FormatOne(p, outbuf[i + 1], 0x07);	//���������,ÿ���볤��7,
		p += j;
	}
	
	j = FormatOne(p, 0x15, 0x06);		//������,�һ���010101,����6,����ֵj��ʾ������ֵ
	p += j;
	
	j = p - g_tUnderlineBuf;	//j = ԭg_tPrtCtrlData.PrtDataDotIndex + �����������ߵĳ���
	
	return j;		//����ֵΪ��������Ҫ��ӡ���»��ߵ�����
}
#else
//��Ϊԭ����,����11����12λ��������
uint16_t FormatCodeUpc_E(uint8_t * inbuf)
{
	uint8_t rule, i;
	uint8_t	outbuf[13];
	
	// general check; m6, s5, and s4 must always be 0
	if ((inbuf[M6] | inbuf[S5] | inbuf[S4]) != '0')
		// mfr# not of form 0mmmmmm and/or sku not of form 00sss
		return 0;     // illegal; can't compress
	
	// else calculate the Rule key:
	/* ����У��� */
 	EanCheckCode(inbuf,11);

	/* ��ʼѹ������ */
	rule = 0;
	// if m1 is not 0 ...
	if ( inbuf[M1] != '0' )
	{ // ... then s1 must be 5..9, and s2, s3 must both be 0
		if  ( (inbuf[S1] >= '5') && ( (inbuf[S2] | inbuf[S3]) == '0' ) )
			rule = 4;
		// else not valid
	}
	else // m1 is 0, so ...
	{   // ... if m2 is not 0, ...
		if ( inbuf[M2] != '0' )
		{   // ... then s3 must also be 0
				if ( inbuf[S3] == '0' )
						rule = 3;
				// else not valid
		}
		else // m2, m1 are 0, so ...
		{    // ... if m3 is > 2 ...
				if ( inbuf[M3] >= '3' )
						rule = 2;
				else
						rule = 1;
		}
	}
	// quit here if there's no valid rule key
	if (rule == 0)
		return 0;     // illegal; can't compress

	i = 0;
	outbuf[i++] = inbuf[M6];  // always 0
	outbuf[i++] = inbuf[M5];
	outbuf[i++] = inbuf[M4];

	switch (rule)
	{
		case 1:     // Rule E.1: mfr# = 0mmx00 (x = 0..2), sku = 00sss
			outbuf[i++] = inbuf[S3];
			outbuf[i++] = inbuf[S2];
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = inbuf[M3];	
			outbuf[i++] = inbuf[CD];
			break;
		case 2:     // Rule E.2: mfr# = 0mmx00 (x = 3..9), sku = 000ss
			outbuf[i++] = inbuf[M3];	
			outbuf[i++] = inbuf[S2];
			outbuf[i++] = inbuf[S1];
			outbuf[i++] = '3';	
			outbuf[i++] = inbuf[CD];
			break;
		case 3:     // Rule E.3: mfr# = 0mmmm0, sku = 0000s
			outbuf[i++] = inbuf[M3];
			outbuf[i++] = inbuf[M2];	
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = '4';	
			outbuf[i++] = inbuf[CD];
			break;
		case 4:     // Rule E.4: mfr# = 0mmmmx (x = 1..9), sku = 0000y (y = 5..9)
			outbuf[i++] = inbuf[M3];
			outbuf[i++] = inbuf[M2];
			outbuf[i++] = inbuf[M1];	
			outbuf[i++] = inbuf[S1];	
			outbuf[i++] = inbuf[CD];
			break;
	}
	outbuf[i++] = '\0';
	strncpy(inbuf,outbuf,9);		//����HRI�ַ�

 	return (EnaFillToBuf((outbuf + 1), 2*6, (*(outbuf + 7) - '0'), 1));		//2*6Ϊ���Ӻ����з���ʵ��,
}
#endif

/*******************************************************************************
* Function Name  : FormatCodeEan13(uint8_t *inbuf)
* Description    : ��inbuf���������������ת��ΪEan13��������,���������g_tUnderlineBuf��.
* Input          : inbuf��������������
* Output         : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
* ENA-13�Ĺ�����ĵ�һλ������ȷ������������뷽ʽ��,�������������,�������������ֵ�������
*	������������Ϊ6λ,����������������2λ+���̴���4λ.��������5λ,��Ӽ����1λ.
*	���Դ�ӡ�����������м������Ϊ6λ,�Ҳ�Ϊ6λ��12λ,
*	���ǽ��յ������ݰ���������ĵ�һλ,������13λ.
*	������������λ,��ʱ���յ�����12λ,���м�������.
*	����ʱ��Ҫ���ݹ������һλȷ����������Ľ��뷽ʽ,���Թ������һλ���ݱز�����.
*	����������ʱ����Ҫ���������һλ.��ӡHRIʱ��Ҫ��ӡ�����յ���ȫ������.
*	EAN-8�빲8λ��,����������2λ,��Ʒ����5λ,�������1λ.
*	EAN-8��������������������EAN-13����ͬ.
*	EAN-8�����������������2λ+��Ʒ����2λ,�������������Ʒ����3λ+�����1λ
*******************************************************************************/
uint16_t FormatCodeEan13(uint8_t * inbuf)
{
 	uint8_t Temp;

	EanCheckCode(inbuf, 12);	//����У���벢��ֵ
	Temp = (*inbuf -'0');			//ȡǰ���ַ�,����ǰ���ַ�ֵ����������������
	
	//����ֵΪEAN-13������ߵ�һ������,�����Ҵ���ĵ�һ��,�ǲ���������ű�ʾ��,
	//�书�ܽ���Ϊ��������ı����趨֮��
	//����ֵΪ��������Ҫ��ӡ���»��ߵ�����
 	return (EnaFillToBuf((inbuf + 1), 12, Temp, 0));
}

/*******************************************************************************
* Function Name : FormatCodeEan8(uint8_t *inbuf)
* Description   : ��inbuf���������������ת��ΪEan8��������,���������g_tUnderlineBuf��.
* Input         : inbuf��������������
* Output        : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
*******************************************************************************/
uint16_t FormatCodeEan8(uint8_t *inbuf)
{
	EanCheckCode(inbuf, 7);
	return (EnaFillToBuf(inbuf, 8, 0, 0));
}

/*******************************************************************************
* Function Name : FormatTwo(uint8_t *Dest, uint16_t Data , uint8_t Blength)
* Description   : ��1���п�խ����������ŵ���ģ���ݷ���ָ���Ļ�������
* Input         : Dest�����ݴ���ָ��,Data����ģ����,Blength����Ч����λ����
* Output        :
* Return		 : 	
*******************************************************************************/
uint16_t FormatTwo(uint8_t *Dest, uint16_t Data, uint8_t Blength)
{
	uint8_t i, j, Temp, Width;
	uint16_t n = 0;
	
	for (i = 1; i <= Blength; i++)
	{	
		if (i % 2)
			Temp = 0xFF;	//��,������Ŵ�����,��ʾ����λ�ַ����������ݷ��������,
		else
			Temp = 0x00;	//��,��ʾż��λ�ַ����������ݷ��ɿ����
		
		if (Data & (uint16_t)(1 << (Blength - i)))
		{
			Width = g_tBarCodeCtrl.BarCodeThickWidth;	//������
		}
		else
		{
			Width = g_tBarCodeCtrl.BarCodeThinWidth;		//խ����
		}
		for (j = 0; j < Width; j++)
			Dest[n++] = Temp;
	}
	
	return n;
}

/*******************************************************************************
* Function Name  : FormatCode39(uint8_t *inbuf)
* Description    : ��inbuf���������������ת��Ϊcode39��������,���������g_tUnderlineBuf��.
* Input          : inbuf��������������
* Output         : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
*******************************************************************************/
uint16_t FormatCode39(uint8_t *inbuf)
{
	uint8_t codebuf[255];
	uint8_t * p = '\0';
	uint16_t i, j, Number, Zimo, tsum;
	
	tsum = 0;
	
	/* �����Ƿ�����ֹ��,���������*/ 
	for (Number = 0; ; )
	{
		if ((inbuf[Number] == '*') || (inbuf[Number] == 0x00))
		{
			Number++;
			if (Number != 1)
				break;
		}
		else
		{
			p = strchr((const char *)codebuf39, inbuf[Number]);
			tsum += (p - codebuf39);		/* ����У��� */
			Number++;
		}
	}
	
	if (inbuf[0] != '*')
	{
		codebuf[0] = '*';			//���û����ʼ��,�����
		strncpy(&codebuf[1], (const char *)inbuf, Number);
		Number++;
	}
	else
		strncpy(&codebuf[0], (const char *)inbuf, Number);	//�п�ʼ������
	
	if (codebuf[Number - 1] != '*')
	{
		codebuf[Number - 1] = '*';		   //���������
	}
	
	strncpy(inbuf, (const char *)codebuf, Number);	   //����HRI
	*(inbuf + Number) = 0x00;
	
/* ����У��� ��ΪУ����ǿ�ѡ��,�ο�EPSON,WOOSIM�Ȼ���,ȥ��У��� */
/*
	osum = tsum%43;
	code[j-1]=*(codebuf39+osum);
	code[j]='*';
	j++;
*/
	j = Number * (((6 + 1)) * g_tBarCodeCtrl.BarCodeThinWidth + 3 * g_tBarCodeCtrl.BarCodeThickWidth);
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//������ӡ����,���ܷ�������������,����0
	
	//����ת��
	for (i = 0; i < Number; i++)
	{
		p = strchr((const char *)codebuf39, codebuf[i]);
		codebuf[i] = (p - codebuf39);
	}	
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < Number; i++)
	{
		Zimo = Code39buf[codebuf[i]];
		j = FormatTwo(p, Zimo, 10);
		p += j;
	}
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name: FormatItf25(uint8_t *inbuf)
* Description  : ��inbuf���������������ת��ΪTIF25��������,���������g_tUnderlineBuf��.
* Input        : inbuf��������������
* Output       : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
*******************************************************************************/
uint16_t FormatItf25(uint8_t *inbuf)
{
	uint8_t Zimo1, Zimo2;
	uint8_t * p = NULL;
	uint16_t i, j, m, n, Number;	
	
	Number = strlen((const char *)inbuf);
	
	if ((Number % 2) == 1)	//��������ֽ����Ƿ�Ϊż��
	{
		Number--;
		inbuf[Number] = 0x00;	//�����������Ϊż��,��ȥ�����һλ��
	}
	
/* ����У����,�ο�EPSON WOOSIM����,������У����,ȥ�� */

/*
	for (i=0;i<m;i++){
		if (i%2)
			oddsum +=*(inbuf+i)-'0';
		else
			evensum+=*(inbuf+i)-'0';

	}
	check = (oddsum+evensum*3)%10+'0';
	strcat(newinbuf,inbuf);
//	strncat(inbuf,&check,1);
	strncat(newinbuf,&check,1);
*/
	/*����25��ÿ��������5����Ԫ���,2��3խ;��ʼ��2խ��2խ��;��ֹ��1����1խ��1խ��*/
	j = (Number * 3 + 4 + 2) * g_tBarCodeCtrl.BarCodeThinWidth + 
			(Number * 2 + 1) * g_tBarCodeCtrl.BarCodeThickWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//������ӡ����,���ܷ�������������,����0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	j = FormatOne(p, 0x0a, 4);		//��ʼ��2խ��2խ��
	
	p += j;
	for (i = 0; i < Number; i++)
	{
		Zimo1 = CodeItf[inbuf[i] - 0x30];	//���ֽںϳɽ����1�ֽ�
		i++;
		Zimo2 = CodeItf[inbuf[i] - 0x30];	
		n = 0;
		for (m = 0; m < 5; m++)
		{
			if ((Zimo1 >> m) & 0x01)
				n |= (uint16_t)(1 << (2 * m + 1));
			if ((Zimo2 >> m) & 0x01)
				n |= (uint16_t)(1 << (2 * m));
		}
		j = FormatTwo(p, n, 10);	//һ�β���10��λ��,2������λ
		p += j;
	}
	
	j = FormatTwo(p, 0x04, 3);	//��ֹ��
	p += j;
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : FormatCodabar(uint8_t *inbuf)
* Description   : ��inbuf���������������ת��Ϊcodabar��������,���������g_tUnderlineBuf��.
* Input         : inbuf��������������
* Output        : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
*******************************************************************************/
uint16_t FormatCodabar(uint8_t *inbuf)
{
	uint16_t i, j, Number;
	uint8_t Zimo, codebuf[255];
	uint8_t	 * p = '\0';
	
	for (Number = 1; ; )
	{
		if (((inbuf[Number] >= 'A') && (inbuf[Number] <= 'D')) || (inbuf[Number] == 0x00))
		{
			Number++;
			break;
		}
		else
			Number++;
	}
	
	if ((inbuf[0] < 'A') || (inbuf[0] > 'D'))
	{
		codebuf[0] = 'A';					//���û����ʼ��,�����
		strncpy(&codebuf[1], (const char *)inbuf, Number);
		Number++;
	}
	else
		strncpy(&codebuf[0], (const char *)inbuf, Number);	//�п�ʼ������
	
	if ((codebuf[Number - 1] < 'A') || (codebuf[Number - 1] > 'D'))
	{
		codebuf[Number - 1] = 'B';		   			//���������
	}
	strncpy(inbuf, (const char *)codebuf, Number);				//����
	*(inbuf + Number) = 0x00;
	
	j = 0;
	for (i = 0; i < Number; i++)
	{									//���д���ת��
		p = strchr((const char *)codabar, codebuf[i]);
		codebuf[i] = (p - codabar);
		if (codebuf[i] > 12)
			j++;						//����3���������ַ�����
	}
	
	j = (Number * 5 - j) * g_tBarCodeCtrl.BarCodeThinWidth + \
			(Number * 3 + j) * g_tBarCodeCtrl.BarCodeThickWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//������ӡ����,���ܷ�������������,����0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < Number; i++)
	{
		Zimo = Codabarbuf[*(codebuf + i)];
		j = FormatTwo(p, Zimo, 8);
		p += j;
	}
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : FormatCode93(uint8_t *Inbuf, uint8_t Length)
* Description   : ��inbuf���������������ת��Ϊcode93��������,���������g_tUnderlineBuf��.
* Input         : inbuf��������������,Length�����ݳ���
* Output        : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0.
************************************************************************************/
uint16_t FormatCode93(uint8_t *inbuf, uint8_t Length)	 
{
	uint8_t m, n, Temp;
	uint8_t	* p = '\0';	
	uint8_t codebuf[PRT_DOT_NUMBERS / (2 * 9)], Hribuf[PRT_DOT_NUMBERS / (2 * 9)];
	uint16_t i, j, Number, osum, tsum, Zimo;
	
 	codebuf[0] = 47;	//������ʼ��
	i = 1;
	m = 0;
	for (Number = 0; Number < Length; Number++)
	{
		Temp = *(inbuf + Number);
		p = strchr((const char *)codebuf93, Temp);
		if ((p == 0x00) || (Temp == 0))	/* P����0,���ڻ����ַ�����,����д���ת��*/
		{
			n = Temp & 0x1F;
			if (Temp < 0x20)    /* 0x00=<vartemp<0x20 */
			{
				Hribuf[m] = 0xFE;	/*���ɴ�ӡ�ַ����� ,���ɴ�ӡ�ַ�����ǰ�ַ���0xfe����ʾ*/
				m++;
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 43;	/*���Ʒ�($) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*���Ʒ�(%) */
					i++;
					if (n == 0)
					{
						Temp = 'U';
					}
					else
					{
						Temp = n - 0x1B + 'A';
					}
				}
				Hribuf[m] = Temp;	/*HRI��ӡ�ַ�����*/
			}
			else if	(Temp < 0x40)	/* 0x20=<vartemp<0x40 */
			{
				Hribuf[m] = Temp;	/*HRI��ӡ�ַ����� */
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 45;	/*���Ʒ�(/) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*���Ʒ�(%) */
					i++;
					Temp = n - 0x1B + 'F';
			 	}
			}
			else if	(Temp < 0x60)	/* 0x40=<vartemp<0x60 */
			{
				Hribuf[m] = Temp;	/*HRI��ӡ�ַ����� */
				codebuf[i] = 44;	/*���Ʒ�(%) */
				i++;
				if (n == 0)
				{
					Temp = 'V';
				}
				else			  
				{
					Temp = n - 0x1B + 'K';
				}
			}
			else 	 	 /* 0x60=<vartemp<0x80 */
			{
				if (Temp == 0x7F)
				{
					Hribuf[m] = 0xFE;	/*���ɴ�ӡ�ַ����� */
					m++;
					Hribuf[m] = 'T';
				}
				else 
					Hribuf[m] = Temp;	/*HRI��ӡ�ַ����� */
				if ((n > 0) && (n < 0x1B))
				{
					codebuf[i] = 46;	/*���Ʒ�(+) */
					i++;
					Temp = n - 1 + 'A';
				}
				else
				{
					codebuf[i] = 44;	/*���Ʒ�(%) */
					i++;
					if (n == 0)
					{
						Temp = 'W';
					}
					else
					{
						Temp = n - 0x1B + 'P';
					}
				}
			}
			p = strchr((const char *)codebuf93, Temp);
		}
		else
			Hribuf[m] = Temp;	/*HRI��ӡ�ַ����� */
		m++;
		codebuf[i] = (p - codebuf93);
		i++;
	}
	
	*(Hribuf + m) = 0x00;
	m++;
	strncpy(inbuf, (const char *)Hribuf, m);	//�п�ʼ������
	
	/*ת��ΪCODE93�Ļ�����ֵ,�������һУ���� */
	tsum = 0;
	j = i - 1;
	for (i = 0; i < j; i++)
	{
		tsum += codebuf[j - i] * (i % 20 + 1);
	}
	tsum = tsum % 47;
	j++;
	codebuf[j] = tsum;
	
	/*�����=У���� C */
	tsum = 0;
	for (i = 0; i < j; i++)
		tsum += codebuf[j - i] * (i % 15 + 1);
	osum = tsum % 47;
	j++;
	codebuf[j] = osum;
	j++;
	codebuf[j] = 48;		//���������
	
	Number = j + 1;

	j = (Number * 9 + 1) * g_tBarCodeCtrl.BarCodeThinWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;														//������ӡ����,���ܷ�������������,����0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < (Number - 1); i++)
	{
		Zimo = code93buf[*(codebuf + i)];
		j = FormatOne(p, Zimo, 9);
		p += j;
	}
	Zimo = code93buf[*(codebuf + i)];	//������
	j = FormatOne(p, Zimo, 10);
	p += j;
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name  : FormatCode128(uint8_t *Inbuf, uint8_t Length)
* Description    : ��inbuf���������������ת��Ϊcode93��������,���������g_tUnderlineBuf��.
* Input          : inbuf��������������,Length�����ݳ���
* Output         : g_tUnderlineBuf������Ϊ��������,inbuf����ΪHRI�ַ���
* Return		 : ���ݸ�ʽ��ȷ��������. ����̫������ݳ���0
************************************************************************************/
uint16_t FormatCode128(uint8_t *Inbuf, uint8_t Length)
{
	uint8_t * p = NULL;
	uint16_t i, j, Zimo;
	
	j = (Length * 11 + 2) * g_tBarCodeCtrl.BarCodeThinWidth;
 	if (j > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))		
		return 0;		//������ӡ����,���ܷ�������������,����0
	
	p = g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex;
	for (i = 0; i < (Length - 1); i++)
	{
		Zimo = CODE128[*(Inbuf + i)];
		j = FormatOne(p, Zimo, 11);
		p += j;
	}
	
	Zimo = CODE128[*(Inbuf + i)];
	j = FormatOne(p, Zimo, 13);		//������
	p += j;	
	j = p - g_tUnderlineBuf;
	
	return j;
}

/*******************************************************************************
* Function Name : GetCode128Str(uint8_t *Codebuf, uint8_t *Hribuf, uint8_t Length,))
* Description   : ����ָ�����ȵ��ַ�,�ж��ַ��Ƿ�Ϊ��Ч�ַ�,��ת��Ϊ��Ӧ����������
* Input         : Codebuf:��������ת���������ַ����ָ��,Length����Ҫ�������ݳ���,Hirbuf��ת�����HIR���ָ��
* Output        : None
* Return        : �����ַ��Ƿ���0,���򷵻�ʵ�ʽ��յ��ַ�����
*******************************************************************************/
uint8_t GetCode128Str(uint8_t *Codebuf, uint8_t *Hribuf, uint8_t Length)
{
	uint8_t code, shift, Temp, TempCode;
	uint8_t	* p = NULL;
	uint8_t GetChar;
	uint16_t i, k, m, sum;
	
	p = Codebuf;
	k = 1;			//KУ��ͼ���Ȩ��
	code = 0;		//�ַ���
	shift = 0;	//ת���ַ���־
	sum	= 0;		//У���
	m = 0;			//ת����HRI����
	for (i = 0; i < Length; i++)
	{
		GetChar = ReadInBuffer();
		if ((i == 0) && (GetChar != '{'))	//���û���ַ���ѡ����˳�,�ַ���ѡ��ͨ��{A,{B,{Cʵ��
		{
			return 0;
		}
		if (GetChar == '{')		 //�ַ���ѡ�����
		{
			GetChar = ReadInBuffer();
			i++;
			switch (GetChar)
			{
				case 65:   			//CODE A 
				{
					if (code != 1)
					{
						code = 1;
						if (i == 1)
						{
							Temp = 103;	//У���
						}
						else
						{
							Temp = 101;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 66:            //CODE B 
				{
					if (code != 2)	   //
					{
						code = 2;
						if (i == 1)
						{
							Temp = 104;	 
						}
						else
						{
							Temp = 100;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 67:               //CODE C
				{
					if (code != 3)	   //
					{
						code = 3;
						if (i == 1)
						{
							Temp = 105;	 
						}
						else
						{
							Temp = 99;	 
						}
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 83:              //SHIFT 
				{
					if (code != 3)
					{
						Temp = 98;
						shift = 1;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 123:		   	 //�������ַ�����
				{
					if (code == 2)
					{
						Temp = (GetChar - 32);
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 49:              //FNC1 
				{
					Temp = 102;	 
					break;
				}
				case 50:              //FNC2 
				{
					if (code != 3)
					{
						Temp = 97;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 51:              //FNC3 
				{
					if (code != 3)
					{
						Temp = 96;	 
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 52:              //FNC4 
				{
					if (code == 1)
					{
						Temp = 101;	 
					}
					else if (code == 2)
					{
						Temp = 100;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				default:
					return 0;
			}
			sum += Temp * k;
			*(p++) = Temp;
			Hribuf[m++] = 0x20;	   //HRIΪ�ո��
			if (i > 1)
			{
			 	k++;
			}
		}	 // End  *(DST+i)==123
		else 
		{
			TempCode = code;
			if (shift)
			{
			 	if (code == 1)
					TempCode = 2;
				else if (code == 2)
					TempCode = 1;
				shift = 0;
			}
			switch (TempCode)
			{
				case 1:				//A�ַ������ַ�ת��Ϊ�������
				{
					if ((GetChar >= 32) && (GetChar <= 95))
					{
						Temp = GetChar - 32;
						Hribuf[m++] = GetChar;
					}
					else if (GetChar < 32)
					{
						Temp = GetChar + 64;
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 2:			//B�ַ������ַ�ת��Ϊ�������
				{
					if ((GetChar >= 32) && (GetChar <= 127))
					{
						Temp = GetChar - 32;
						Hribuf[m++] = GetChar;
					}
					else
					{
						return 0;
					}
					
					break;
				}
				case 3:			//C�ַ������ַ�ת��Ϊ�������
				{
					if (GetChar <= 99)
					{
						Temp = GetChar;
						Hribuf[m++] = GetChar / 10 + '0';
						Hribuf[m++] = GetChar % 10 + '0';
					}
					else
					{
						return 0;
					}
					
					break;
				}
			}
			sum += Temp * k;
			*(p++) = Temp;
			k++;
		}
	}
	*(p++) = sum % 103;
	k++;	//У��λ

	*(p++) = 106;	 //stop bit
	k++;		  	//ʵ�������ַ���

 	Hribuf[m++] = '\0';		//���������
	
	return k;
}

/*******************************************************************************
* Function Name  : void GetCodeBarStr(uint8_t *Inbuf, uint8_t Number, uint8_t Type)
* Description    : ����ָ���������ַ�,���ָ������0xFF,����յ�0x00,����
* Input          : Inbuf:�������ݴ��ָ��,Number����Ҫ�������ݳ���,Type����������
* Output         : None
* Return         : �����ַ��Ƿ���0,���򷵻�ʵ�ʽ��յ��ַ�����
*******************************************************************************/
uint8_t GetCodeBarStr(uint8_t *Inbuf, uint8_t Number, uint8_t Type)
{
	uint8_t GetChar,i;

	for (i = 0; i < Number; i++)
	{
		GetChar = ReadInBuffer();
		Inbuf[i] = GetChar;
		
		switch (Type)
		{
			case	0:		//UPC-A
			case	1:		//UPC-E
			case	2:		//ENA-13
			case	3:		//ENA-8
			case	5:		//ITF
			case	65:		//UPC-A
			case	66:		//UPC-E
			case	67:		//ENA-13
			case	68:		//ENA-8
			case	70:		//ITF
			{
				if (Inbuf[i] == 0)	//�����NUL,��ʾ���ݲ�������
					//return (i + 1);
					return i;
				else if ((Inbuf[i] < '0') || (Inbuf[i] > '9'))
					return 0;			//�Ƿ��ַ�����
				break;
			}
			case	4:		//CODE39
			case	69:		//CODE39
			{
				if ((Inbuf[i] == 0) || ((Inbuf[i] == '*') && (i != 0)))
					return (i + 1);
				else if (strchr((const char *)codebuf39, Inbuf[i]) == 0)
					return 0;			//�Ƿ��ַ�����
				break;
			}
			case	6:		     //CODEBAR
			case	71:		     //CODEBAR
			{
				if ((Inbuf[i] == 0) || ((Inbuf[i] >= 'A') && (Inbuf[i] <= 'D') && (i != 0)))
					return (i + 1);
				else if (strchr((const char *)codabar,Inbuf[i]) == 0)
					return 0;			//�Ƿ��ַ�����
				break;
			}
			case	72:		      //CODE93
			{
				if (Inbuf[i] >= 0x80)
					return 0;			//�Ƿ��ַ�����
				break;
			}
			default:
				break;
		}
	}
	
	Inbuf[i] = 0;			//���������
	
	return i;
}

/*******************************************************************************
* Function Name  : void PrintBar(uint8_t *HriStr, uint16_t Width)
* Description    : ��ӡ����
* Input          : *HriStr:HRI��ַ,��ſ��Ķ�����,NUL����
	Width:�����ܿ�,������g_tPrtCtrlData.PrtLeftLimit���ڵĴ�0����������ͼ�ο������
	�޸�Ϊֻ��������ͼ�ο��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBar(uint8_t * HriStr, uint16_t Width)
{
	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_NORMAL)
	{
		if (g_tBarCodeCtrl.HRIPosition)
		{
			if ( g_tBarCodeCtrl.HRIPosition & 0x01 )
			{	//���������Ҫ��ӡHRI
				PrintBarHRI(HriStr, Width);
			}
			
			PrintBarBitmap(HriStr, Width);
			
			if (g_tBarCodeCtrl.HRIPosition & 0x02)
			{	//���������Ҫ��ӡHRI
				PrintBarHRI(HriStr, Width);
			}
		}
		else	//�����ӡHRI
		{
			PrintBarBitmap(HriStr, Width);
		}
	}
	else
	{
		if (g_tBarCodeCtrl.HRIPosition)
		{
			if ( g_tBarCodeCtrl.HRIPosition & 0x02 )
			{	//���������Ҫ��ӡHRI
				PrintBarHRI(HriStr, Width);
			}
			
			PrintBarBitmap(HriStr, Width);
			
			if ( g_tBarCodeCtrl.HRIPosition & 0x01 )
			{	//���������Ҫ��ӡHRI
				PrintBarHRI(HriStr, Width);
			}
		}
		else	//�����ӡHRI
		{
			PrintBarBitmap(HriStr, Width);
		}
	}
	g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLeftLimit;   //2017.10.14
}

/*******************************************************************************
* Function Name  : void PrintBarHRI(uint8_t *HriStr, uint16_t Width)
* Description    : ��ӡ��������
* Input          : *HriStr:HRI��ַ,Width:�����ܿ�,���ȫ�ֱ���
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarHRI(uint8_t * HriStr, uint16_t Width)
{
	uint8_t	i;
	uint8_t Language;
	uint8_t SYS_FontSize;
	uint8_t Underline;
	uint8_t CharWidth;
	uint8_t CharHigh;
	uint8_t MaxRatioX;
	uint8_t MaxRatioY;
	uint8_t CodeLineHigh;
	uint8_t HZUnderline;
	uint8_t CharNot;
	uint8_t CharCycle90;
	uint16_t LeftSpace;
	uint16_t RightSpace;
	uint32_t temp1;                 //2017.10.14
	
	//��������
	temp1 = g_tPrtCtrlData.PrtDataDotIndex;       //2017.10.14
	Language			= g_tSysConfig.SysLanguage;			//����
	SYS_FontSize	= g_tSysConfig.SysFontSize;			//����
	HZUnderline		= g_tPrtModeHZ.Underline;				//�»���
	MaxRatioY			= g_tPrtModeChar.HeightMagnification;	//����Ŵ���
	MaxRatioX			= g_tPrtModeChar.WidthMagnification;	//����Ŵ���
	Underline			= g_tPrtModeChar.Underline;			//�»���
	CharWidth			=	g_tPrtModeChar.CharWidth;			//�ַ����
	CharHigh			= g_tPrtModeChar.CharHigh;			//�ַ��߶�
	LeftSpace			=	g_tPrtModeChar.LeftSpace;			//�ַ�����
	RightSpace		= g_tPrtModeChar.RightSpace;		//�ַ��Ҽ��
	CodeLineHigh	= g_tPrtCtrlData.CodeLineHigh;	//�м��
	CharNot				= g_tPrtCtrlData.CharNotFlag;		//���Դ�ӡ
	CharCycle90		= g_tPrtCtrlData.CharCycle90;		//��ת90��
	
	//���ò���
	g_tSysConfig.SysLanguage = LANGUAGE_ENGLISH;
	if (g_tBarCodeCtrl.HRIFont == ASCII_FONT_A)
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_A_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_A_HEIGHT;
	}
	else if (g_tBarCodeCtrl.HRIFont == ASCII_FONT_B)
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_B;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_B_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_B_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_B_HEIGHT;
	}
	else
	{
		g_tSysConfig.SysFontSize	= ASCII_FONT_A;
		g_tPrtModeChar.CharWidth	= ASCII_FONT_A_WIDTH;
		g_tPrtModeChar.CharHigh		= ASCII_FONT_A_HEIGHT;
		g_tPrtCtrlData.LineHigh		= ASCII_FONT_A_HEIGHT;
	}
	
	g_tPrtModeChar.Underline =	0;
	g_tPrtModeChar.HeightMagnification = 1;
	g_tPrtModeChar.WidthMagnification = 1;
	g_tPrtModeChar.LeftSpace = 0;
	i = strlen((const char *)HriStr);
	if ((Width / i) > g_tPrtModeChar.CharWidth)
		g_tPrtModeChar.RightSpace = (Width / i - g_tPrtModeChar.CharWidth);
	else
		g_tPrtModeChar.RightSpace = 0;
	g_tPrtModeHZ.Underline					= 0;
	g_tPrtCtrlData.CodeLineHigh 		= (g_tPrtModeChar.CharHigh + 8);
	g_tPrtCtrlData.DoUnderlineFlag	= 0;	//ȷ������ȴ�ӡHRI�ַ�,����������»��߻�����
	g_tPrtCtrlData.OneLineWidthFlag	= 0;
	g_tPrtCtrlData.MaxRatioY		= 1;			//�������������
	g_tPrtCtrlData.CharNotFlag	= 0;			//���Դ�ӡ
	g_tPrtCtrlData.CharCycle90	= 0;			//��ת90��
	PrintString(HriStr);
	PrintString("\n");
	#if	1
	//�ָ�����
	g_tPrtCtrlData.PrtDataDotIndex = temp1;       //2017.10.14
	g_tSysConfig.SysLanguage	= Language;				//����
	g_tSysConfig.SysFontSize	= SYS_FontSize;		//����
	g_tPrtModeHZ.Underline		= HZUnderline;
	g_tPrtModeChar.HeightMagnification	= MaxRatioY;			//����Ŵ���
	g_tPrtModeChar.WidthMagnification		= MaxRatioX;			//����Ŵ���
	g_tPrtModeChar.Underline						= Underline;			//�»���
	g_tPrtModeChar.LeftSpace						= LeftSpace;			//�ַ�����
	g_tPrtModeChar.RightSpace						= RightSpace;			//�ַ��Ҽ��
	g_tPrtModeChar.CharWidth						= CharWidth;			//�ַ����
	g_tPrtModeChar.CharHigh							= CharHigh;				//�ַ��߶�
	g_tPrtCtrlData.CodeLineHigh					= CodeLineHigh;		//�м��
	g_tPrtCtrlData.CharNotFlag					= CharNot;				//���Դ�ӡ
	g_tPrtCtrlData.CharCycle90					= CharCycle90;		//��ת90��
	#endif
}

#ifdef	BARCODE_PRT_AS_BITMAP_ENABLE
/*******************************************************************************
* Function Name  : void PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
* Description    : ��ӡ����λͼ,���»��߻��������ݸ�������߶���䵽��ӡ����������ӡ
* Input          : *HriStr:HRI��ַ,Width:�����ܿ�,���ȫ�ֱ���
*	�»��߻�������Ч����:g_tPrtCtrlData.PrtDataDotIndex��ʼ,��.PrtDataDotIndex + Width����.
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
{
	uint8_t	i;
	uint8_t LineHigh, CodeLineHigh;
	uint8_t cByteHigh, cMaxByte;
	uint16_t	j;
	uint8_t * psrc = NULL;
	uint8_t * pdst = NULL;
	uint8_t * ptemp = NULL;
	
	CodeLineHigh = g_tPrtCtrlData.CodeLineHigh;
	LineHigh = g_tPrtCtrlData.LineHigh;
	
	if (Width > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))	//���ⳬ�����ֵ
	{
		return;
	}
	
	cMaxByte = g_tBarCodeCtrl.BarCodeHigh >> 3;	//���������ֽ���
	if ((g_tBarCodeCtrl.BarCodeHigh % 8) > 4)		//��������
		cMaxByte += 1;
	
	if (cMaxByte >= MAX_PRT_HEIGHT_BYTE)	//��ֹ�߶ȹ��߳����ڴ滺����
	{
		cByteHigh = MAX_PRT_HEIGHT_BYTE;		//�ֳ����δ�ӡ
		//����׵�ַ,�������Ϊg_tUnderlineBuf�д�LeftLimit��ʼWidth��ȵ�ֵ,0xFF����0x00
		ptemp = g_tLineEditBuf + g_tPrtCtrlData.PrtDataDotIndex;		//��ǰ�༭�������հ���ʼλ��
		g_tPrtCtrlData.PrtDataDotIndex += Width;		//����������
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//����һ�еĲ��ֲ���ӡ
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//���ֽ������
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//���»��߻������ж�Ӧλ�õ�������䵽�༭������
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//�����ܵ���
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;				//��ͼ�η�ʽ��ӡ
// 		PrintOneLine();													//��ӡ��������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
		cByteHigh = cMaxByte - MAX_PRT_HEIGHT_BYTE;		//�ֳ����δ�ӡ
		//����׵�ַ,�������Ϊg_tUnderlineBuf�д�LeftLimit��ʼWidth��ȵ�ֵ,0xFF����0x00
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - cByteHigh) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		g_tPrtCtrlData.PrtDataDotIndex += Width;	//����������
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//����һ�еĲ��ֲ���ӡ
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//���ֽ������
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//���»��߻������ж�Ӧλ�õ�������䵽�༭������
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//�����ܵ���
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;				//��ͼ�η�ʽ��ӡ
		g_tPrtCtrlData.DoUnderlineFlag = 0;			//����»���
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS); //�建����
		memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
// 		PrintOneLine();													//��ӡ��������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	else
	{
		cByteHigh = cMaxByte;						//ֱ��һ�δ�ӡ���
		//����׵�ַ,�������Ϊg_tUnderlineBuf�д�LeftLimit��ʼWidth��ȵ�ֵ,0xFF����0x00
		ptemp = g_tLineEditBuf + (MAX_PRT_HEIGHT_BYTE - cByteHigh) * PRT_DOT_NUMBERS + g_tPrtCtrlData.PrtDataDotIndex;
		g_tPrtCtrlData.PrtDataDotIndex += Width;		//����������
		if (g_tPrtCtrlData.PrtDataDotIndex > g_tPrtCtrlData.PrtLength)	//����һ�еĲ��ֲ���ӡ
			g_tPrtCtrlData.PrtDataDotIndex = g_tPrtCtrlData.PrtLength;
		
		for (i = 0; i < cByteHigh; i++)		//���ֽ������
		{
			psrc = g_tUnderlineBuf + g_tPrtCtrlData.PrtLeftLimit;
			pdst = ptemp + i * PRT_DOT_NUMBERS;
			for (j = 0; j < Width; j++)			//���»��߻������ж�Ӧλ�õ�������䵽�༭������
			{
				*pdst++ = *psrc++;
			}
		}
		
		g_tPrtCtrlData.MaxPrtDotHigh = (uint16_t)cByteHigh;	//�����ܵ���
		g_tPrtCtrlData.MaxPrtDotHigh <<= 3;
		g_tPrtCtrlData.BitImageFlag = 1;					//��ͼ�η�ʽ��ӡ
		g_tPrtCtrlData.DoUnderlineFlag = 0;				//����»���		
		g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
		//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS); //�建����
		memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
    //PrintOneLine();													//��ӡ��������
		PrtOneLineToBuf(g_tPrtCtrlData.CodeLineHigh);
	}
	
	GoDotLine(8);	   //��ֽ����
	
	g_tPrtCtrlData.LineHigh 		= LineHigh;		//�ָ�����
	g_tPrtCtrlData.CodeLineHigh	= CodeLineHigh;
}		//ͼ�η�ʽ��ӡ�������

#else
extern	uint8_t	g_tbHeatElementBuf[];
static uint16_t	GetStartColumnIndex(void)
{
	uint16_t	ColumnIndex;	//�����м�������
	
	if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_LEFT)		//�����
	{
		ColumnIndex = g_tPrtCtrlData.PrtLeftLimit;
	}
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_MIDDLE)	//�м����
	{	// = ��Ч��ӡ���� - ��ӡ�����м���������(һ���д�ӡ��ӡ���������)
		ColumnIndex = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		ColumnIndex >>= 1;
		ColumnIndex += g_tPrtCtrlData.PrtLeftLimit;
	}
	else if (g_tPrtCtrlData.CodeAimMode == AIM_MODE_RIGHT)	//�Ҷ���
	{
		ColumnIndex = g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex;
		ColumnIndex += g_tPrtCtrlData.PrtLeftLimit;
	}
	
	return (ColumnIndex);
}

static void	FillCodebarBufToHeatElementBuf(uint8_t * bBuf, uint8_t Lines)
{
	if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_NORMAL)	//�����ӡ��ʼ
	{
		CodebarBufToHeatElementNormalDir(bBuf, Lines);
	}	//�����ӡ�������
	else if (g_tPrtCtrlData.LineDirection == PRINT_DIRECTION_REVERSE)	//�����ӡ��ʼ
	{
		CodebarBufToHeatElementReverseDir(bBuf, Lines);
	}	//�����ӡ�������
}

static void	CodebarBufToHeatElementNormalDir(uint8_t * bBuf, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0ʱһ���ֽ�������
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint16_t	CopyIndex;
	uint8_t	* pBuf = NULL;
	
	//memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	//���������Ŀհ�λ��������,��Ϊǰ���Ѿ�ȫ�������
	if (ColumnIndex % 8)				//ColumnIndex����8��������,ǰ����Ҫ�ν�
	{
		CurrentBit = 8 - (ColumnIndex % 8);	//��ǰ�ֽ����λ��λ��,��8��1,Ϊ0		
	}
	else
	{
		CurrentBit = 8;		//һ���ֽ������8λ
	}
	i = (ColumnIndex >> 3);	//׼�������ֽ�,���ֽڵĺ��治��Ĳ�����Ҫ����������в���
	CopyIndex = i;
	//����Ϊ����հײ���
	
	//����ʼ�д������Ҵ����ת���ַ�
	for (ColumnIndex = g_tPrtCtrlData.PrtLeftLimit; 
		ColumnIndex < g_tPrtCtrlData.PrtDataDotIndex; ColumnIndex++)
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ0~7
		if ((*bBuf & (0x80 >> (Lines & 0x07))))	//�����Ӧλ��Ϊ1
		{
			Buf[i] |= (uint8_t)(1 << (CurrentBit - 1));	//���һ�������ֽ�
		}
		CurrentBit--;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 0)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 8;			//���¿�ʼ׼�������һ���ֽ�,��MSB��ʼ���
			i++;								//ֻ�����ֽں��ִ��
		}
		bBuf++;								//׼�������һ���ֽ�
	}		//����ӡ�ֽڴ������
#if	0
	if (!CurrentBit)		//˵����ת��������һ���ֽڶ�û����ɵ�,��Ҫ����1��
		i++;
	if (i > BYTES_OF_HEAT_ELEMENT)		//��ֹ���
		i = BYTES_OF_HEAT_ELEMENT;
#endif
	
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//�������32λ�հ����ͼ�������Ϊ0������,�Ҳ�32λ�հ����Ѿ�����
	pBuf = g_tbHeatElementBuf;
	pBuf += (BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT + CopyIndex);
	for (; CopyIndex < BYTES_OF_HEAT_ELEMENT; CopyIndex++)
	{
		*pBuf++ = Buf[CopyIndex];		//�м�ʵ�ʴ�ӡ��
	}
}

static void	CodebarBufToHeatElementReverseDir(uint8_t * bBuf, uint8_t Lines)
{
	uint8_t	Buf[BYTES_OF_HEAT_ELEMENT];	//������ȴ�����
	uint8_t	CurrentBit;		//��ǰ�ֽ����λ��λ��,��8��1,Ϊ8ʱһ���ֽ�������
	uint16_t	ColumnIndex;	//�����м�������,������ʼֵ����հ�λ�ú��ֽ�λ
	uint16_t	i;
	uint8_t	* pBuf = NULL;
	
	//memset(Buf, 0x00, BYTES_OF_HEAT_ELEMENT);
	memset(Buf, 0x00, sizeof(Buf) / sizeof(Buf[0]));
	ColumnIndex = GetStartColumnIndex();	//���ݶ��뷽ʽ�õ���ʼ��ֵ
	i = g_tPrtCtrlData.PrtLength - ColumnIndex;	//ָ�����ת������Ȼ�������ʼλ��
	if (i % 8)
	{
		CurrentBit = 8 - (i % 8);	//���ܳ��ֲ���8λ������λ��,������������λ��ʼ�����
	}
	else
	{
		CurrentBit = 0;		//�����8�����������0��ʼ
	}
	
	i >>= 3;													//ת��Ϊ�ֽ�
	if (i >= BYTES_OF_HEAT_ELEMENT)		//����������һ���ֽ�
	{
		i = BYTES_OF_HEAT_ELEMENT - 1;	//��ֹ�±�Խ��
	}
	
	pBuf = Buf;
	pBuf += i;
	//��Ҫ�������(�ֽ�)��Ŀ
	for (i = g_tPrtCtrlData.PrtLeftLimit; i < g_tPrtCtrlData.PrtDataDotIndex; i++)
	{
		//�жϸ��ֽڶ�Ӧλ��ֵ�Ƿ�Ϊ1,Lines��Ҫת��Ϊ��Χ0~7
		if (*bBuf & (0x80 >> (Lines & 0x07)))	//�����Ӧλ��Ϊ1
		{
			*pBuf |= (uint8_t)(1 << CurrentBit);	//���һ�������ֽ�
		}
		CurrentBit++;					//�ֽ�λ��ÿ�β��۴����ֽڶ�Ӧλ�Ƿ�Ϊ1����Ҫ���д���
		
		if (CurrentBit == 8)	//�Ѿ�����һ����������ֽ�
		{
			CurrentBit = 0;			//���¿�ʼ׼�������һ���ֽ�,��LSB��ʼ���
			pBuf--;							//ֻ����һ���ֽں�ż���
		}
		bBuf++;								//׼�������һ�е��ֽ�,ÿ��ѭ��������
	}
	
	//���½���������䵽������Ȼ�����
	memset(g_tbHeatElementBuf, 0x00, BYTES_OF_ALL_ELEMENT);
	//memset(g_tbHeatElementBuf, 0x00, sizeof(g_tbHeatElementBuf) / sizeof(g_tbHeatElementBuf[0]));
	pBuf = g_tbHeatElementBuf;
	pBuf += BYTES_OF_LEFT_EMPTY_HEAT_ELEMENT;		//�������32λ�հ���,�Ҳ�32λ�հ����Ѿ�����
	for (i = 0; i < BYTES_OF_HEAT_ELEMENT; i++)
	{
		*pBuf++ = Buf[i];		//�м�ʵ�ʴ�ӡ��
	}
}

/*******************************************************************************
* Function Name  : void PrintBarBitmap(uint8_t *HriStr, uint16_t Width)
* Description    : ��ӡ����λͼ,���»��߻��������ݸ�������߶���Ϊѭ�����д�����ӡ
*	����ʽ:��ȷ����ӡ��ʼ�ͽ�����Χ;�پݴ˴��»��߻������ҳ�һ���е�����������䵽
*	���Ȼ�����;�ٸ�������߶�ѭ����ӡ����
*	�»��߻�������Ч����:g_tPrtCtrlData.PrtDataDotIndex��ʼ,��.PrtDataDotIndex + Width����.
* Input          : *HriStr:HRI��ַ,Width:�����ܿ�,���ȫ�ֱ���
* Output         : None
* Return         : None
*******************************************************************************/
void	PrintBarBitmap(uint8_t * HriStr, uint16_t Width)
{
	uint16_t	HeatTime;
	uint16_t	totalheatus;
	uint32_t	temp,temp1;
	uint32_t	i;
	uint8_t * psrc = NULL;
	
	WaitPreviousPrintFinish();	//�ȴ�������Ϊ�հ�
	WaitHeatFinish();						//�ȴ��������
	
	//���»��߻��������ݴӿ�ʼ��������ȡ���༭������
	temp1 = g_tPrtCtrlData.PrtDataDotIndex;
	g_tPrtCtrlData.PrtDataDotIndex += Width;		//����ֵ����������λ�� 2017.10.14
	//���»��߻�������ָ���߶ȵ�����ת��Ϊ������ȵĸ�ʽ
	//��ת���������浽ȫ�ּ������������
	psrc = g_tUnderlineBuf;          // + temp;		//���ʼ����Ч���ݿ�ʼ  2017.10.14 �����ǵ���߾���Ч
// 	FillCodebarBufToHeatElementBuf(psrc, 0x00);	//�������λ��ȡ,��0��7����һλ������
// 	SendHeatElementBufToCore();		//���ͼ������ݵ���о
// 	LatchHeatData();							//�����о����
// 	WaitFeedPaperFinish();				//�ȴ���ֽ���
	
	for (temp = 0; temp < g_tBarCodeCtrl.BarCodeHigh; temp++)	//��������߶�ѭ����ӡÿ����
	{
			MovDotLine( psrc, 0);
	}
	
	//�������,��һ�����,�ָ������ȫ�ֱ���
	GoDotLine(8);	   //��ֽ����
	
	g_tPrtCtrlData.PrtDataDotIndex = temp1;       //g_tPrtCtrlData.PrtLeftLimit;    //2017.10.14
	g_tPrtCtrlData.DoUnderlineFlag = 0;						//����»���
	g_tPrtCtrlData.UnderlineMaxPrtHigh = 0;
	//memset(g_tUnderlineBuf, 0x00, PRT_DOT_NUMBERS);	//�建����
	memset(g_tUnderlineBuf, 0x00, sizeof(g_tUnderlineBuf) / sizeof(g_tUnderlineBuf[0]));
}
#endif

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
