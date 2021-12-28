/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   EANһά������صĳ���.
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
	* ���ļ�����ı������з���GB12904-2008���ұ�׼,����ı��������а���ǰ����,
	*	��������������г���Ϊ13.���յ��Ĳ���������󲻳���13.
	* ǰ���벻������������ݷ���, ���������ַ���ʾ.
	*	ENA13���� ��ʼ�� + ������ݷ�6�� + �м����� + �Ҳ�����5�� + У���1�� + ������
	*	���е�ǰ���벻���������������, ���ǿ��Դ�ӡ����,1λ.
	* ����ǰ����ȷ���������ݵı��뷽ʽ.
	* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
	*		d13			d12	~	d7	 	d6	~	d2		d1
	*	ǰ�÷�		�������		�Ҳ�����		У���
	*	EAN13������ݸ���ǰ�÷�ȷ��������ݱ��뼯A����B
	*	EAN13�Ҳ����ݼ�У��������ñ��뼯C
	
	*	ENA8���� ��ʼ�� + ������ݷ�4�� + �м����� + �Ҳ�����3�� + У���1�� + ������
	*	EAN8������ݲ������ݱ��뼯A
	*	EAN8�Ҳ����ݼ�У��������ñ��뼯C
	
	*	ENA13У������㷽������:
	*	a)	�Ӵ���λ�����2��ʼ, ����ż��λ�����ִ������
	*	b)	��a)�ĺͳ���3
	*	c)	�Ӵ���λ�����3��ʼ, ��������λ�����ִ������
	*	d)	��b)��c)�Ľ�����
	*	e)	�ô��ڻ��ߵ���d)���ý����Ϊ10����С������������ȥd)���ý��,��ΪУ��ֵ
	
	*	UPC-A���뼴ǰ����Ϊ0��ENA13��.�乲��12λ�������.
	*	����λ�ı���������ȫ��EAN13��ͬ.��UPC-A��������ǰ����� 0 ����Ϊ��׼��EAN13����.
	* UPC-A�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
	*	d13(�޴�λ)			d12	~	d7	 	d6	~	d2		d1
	*	ǰ�÷�(�޴�λ)	�������		�Ҳ�����		У���
	*	UPC-AУ����ļ��㷽ʽ�൱��ǰ����Ϊ0��ENA13����.
	*	UPC-A���� ��ʼ�� + ������ݷ�6�� + �м����� + �Ҳ�����5�� + У���1�� + ������
	
	Ҳ���԰������·�������,Ч����ͬ�������㷨.
	�ӹ����뿪ʼ��������ȡ������UPC-A����������£�
	N1	N2	N3	N4	N5	N6	N7	N8	N9	N10	N11	C
	������֮���㲽�����£�
	C1 = N1+N3+N5+N7+N9+N11
	C2 = (N2+N4+N6+N8+N10)��3
	CC = (C1+C2)��ȡ��λ��
	C (�����) = 10 - CC�� (��ֵΪ10����ȡ0)
	
	*	UPC-E������8λ�������,�ǽ�ϵͳ�ַ�Ϊ0��UPC-12����ѹ������.
	*	UPC-E���ַ���Ϊ0,�м�Ϊ6λ����,���Ϊ1λУ����,
	*	UPC-E���� ��ʼ�� + ������ݷ�3�� + �Ҳ�����3�� + ������,���м�������У���
	
		UPC-A����ѹ����������:
											UPC-A����																					UPC-E����
						����ʶ�����						��Ʒ��Ŀ����		У����			��Ʒ��Ŀʶ�����			У����
		X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 0  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 1  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 2  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					
					0				X11 X10 3  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 ...0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 9  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					
					0				X11 X10 X9 1  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 ...0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 9  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					
					0				�����βX7 != 0   0  0  0  0	5       X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				�����βX7 != 0   0  0  0  0	...     X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				�����βX7 != 0   0  0  0  0	9       X1			0 X11 X10 X9 X8 X7 X2  	X1
		
	*	UPC-E�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
	*		d8	~		d7	~	d2	 				d1
	*	ϵͳ�ַ�0		��Ʒ��Ŀ����		У����
	*	У����Ϊ����ǰ��UPC-A�������õ���У����.
	*	UPC-E���м�ָ���.
	* UPC-E��ֹ����ͬ��EAN8, Ϊ010101.
	*	UPC-E���ݱ������У��λֵȷ��ѡ���ַ���A����B.��EAN8���벻ͬ.
	*	UPC-E��ϵͳ�ַ�X8��У����X1���������ַ���ʾ.
	* 
	*	�ܽ�:
	*	EAN13��������:ǰ�÷�1 + ������ݷ�6 + �Ҳ����ݷ�5 + У���1,�� 13 �ֽ�
	*	EAN13��ӡ��������:��ʼ��b101 + ������� + �м�ָ���b01010 + �Ҳ����� + У�� + ��ֹ��b101
	*	ǰ�÷�ֻ����ȷ��������ݷ�����ʱ�ı��뷽ʽ,�����뵽ʵ�ʵĴ�ӡ����������
	*	EAN13������ݸ���ǰ�÷�ȷ��������ݱ��뼯A����B,EAN13�Ҳ����ݼ�У��������ñ��뼯C
	
	*	UPC-A��������:������ݷ�6 + �Ҳ����ݷ�5 + У���1,�� 12 �ֽ�
	*	UPC-A��ӡ��������:��ʼ��b101 + ������� + �м�ָ���b01010 + �Ҳ����� + У�� + ��ֹ��b101
	*	��ǰ�÷�,�൱��ǰ�÷�Ϊ0��EAN13����.���뷽ʽ��ȫͬEAN13.
	
	*	EAN8��������:������ݷ�4 + �Ҳ����ݷ�3 + У���1,�� 8 �ֽ�
	*	EAN8��ӡ��������:��ʼ��b101 + ������� + �м�ָ���b01010 + �Ҳ����� + У�� + ��ֹ��b101
	*	������ݷ�����ʱ���ñ��뼯A,�Ҳ����ݼ�У���ñ��뼯C
	
	*	UPC-E��������:ϵͳ�ַ�1 + ���ݷ�6 + У���1,�� 8 �ֽ�
	*	UPC-E��ӡ��������:��ʼ��b101 + ����6 + ��ֹ��b010101
	*	ǰ�÷�Ϊ0,���ǲ������ڿɴ�ӡ�ı���֮��,У���Ϊѹ��ǰ��UPC-A��У���,�������ڿɴ�ӡ������.
	
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include	"ean.h"
#include	"defconstant.h"

/*  */
/********** һά�������ȫ�ֱ��� ***********/
extern	TypeDef_StructBarCtrl	g_tBarCodeCtrl;
extern	TypeDef_StructPrtCtrl	g_tPrtCtrlData;
extern	uint8_t	g_tUnderlineBuf[PRT_DOT_NUMBERS];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**********EAN13 A�ַ������ֱ���ģʽ *******/
/* ������ÿһ����Ա���ηֱ���������ַ�0~9�������������,����EAN-��������A */
const uint8_t EANContentCodeType_A[] = 
{
	0x0D, 0x19, 0x13, 0x3D, 0x23, 0x31, 0x2F, 0x3B, 0x37, 0x0B
};

/**********EAN13 B�ַ������ֱ���ģʽ *******/
/* ������ÿһ����Ա���ηֱ���������ַ�0~9�������������,����EAN-��������B */
const uint8_t EANContentCodeType_B[] = 
{
	0x27, 0x33, 0x1B, 0x21, 0x1D, 0x39, 0x05, 0x11, 0x09, 0x17
};

/**********EAN13 C�ַ������ֱ���ģʽ *******/
/* ������ÿһ����Ա���ηֱ���������ַ�0~9�������������,����EAN-��������,
	��������ͬ��UPC-A�����ϱ��� */
const uint8_t EANContentCodeType_C[] = 
{
	0x72, 0x66, 0x6C, 0x42, 0x5C, 0x4E, 0x50, 0x44, 0x48, 0x74
};

/**********EAN13 ����ǰ�÷�ȷ��������ַ�ѡ���ַ��� *******/
/*****************************************************************************
*EAN ����ȷ����������ַ����Ӽ�,"0"��ʾA�Ӽ�,"1"��ʾB�Ӽ� 
*��������ÿ�����ݵ�ֵ��D0��D5�ֱ��ʾEAN�������д��ҵ���ĵ�7��12λ(�������)
*���ݹ������ȷ�������Ϸ��ı��뷽ʽ,������ֵ�ĵ�6λ��Ч
******************************************************************************/
const uint8_t EANLeftCharCodeTypeSel[] =
{
	0x00, 0x0B, 0x0D, 0x0E, 0x13, 0x19, 0x1C, 0x15, 0x16, 0x1A
};

const uint8_t	UPCECharCodeTypeSel[] =
{
	0x38, 0x34, 0x32, 0x31, 0x2C, 0x26, 0x23, 0x2A, 0x29, 0x25 
};


/* EAN��ʼ������, 101 */
const uint8_t	EANStartCode = 0x05;

/* EAN����������, 101 */
const uint8_t	EANStopCode = 0x05;

/* EAN�м������, 01010 */
const uint8_t	EANMiddleCode = 0x0A;

/* UPCE��ʼ������, 101 */
const uint8_t	UPCStartCode = 0x05;

/* UPCE����������, 010101 */
const uint8_t	UPCEStopCode = 0x15;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
*******************************************************************************/
/*******************************************************************************
* Function Name  : ErrorStatus CheckEANUPCCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen)
* Description    : ���ָ�����ȵ�EAN_UPC�����Ƿ�Ϸ�
* Input          : �������:�����ַ���, ���볤��
* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*		d13			d12	~	d7	 	d6	~	d2		d1
*	ǰ�÷�		�������		�Ҳ�����		У���
*	ʵ�������������������:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	ǰ�÷�		�������		�Ҳ�����		У���
* Output         : �������:״ֵ̬
*******************************************************************************/
ErrorStatus	CheckEANUPCCodeValid(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	ErrorStatus	Result;
	uint8_t	i;
	
	if (bCodeLen)
	{
		Result = SUCCESS;
		for (i = 0; i < bCodeLen; i++)
		{
			if (!IS_EAN_UPC_CODE( bBuf[i] ))
			{	
				Result = ERROR;
				break;
			}
		}
	}
	else
	{
		Result = ERROR;
	}
	
	return	(Result);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ָ���ַ�����EAN13��У����
* Input          : �������:�Ѿ���׼���˵�EAN13�ַ�����,��12����13�ֽڳ���
* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*		d13			d12	~	d7	 	d6	~	d2		d1
*	ǰ�÷�		�������		�Ҳ�����		У���
*	ʵ�������������������:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	ǰ�÷�		�������		�Ҳ�����		У���
* Output         : �������:������У����, ��Χ'0'~'9'
*******************************************************************************/
uint8_t	CalEAN13CheckCode(uint8_t	bBuf[])
{
	uint8_t	bResult;
	uint16_t	OddCodeSum;			/* ������ű����,���մ��ҵ�������� */
	uint16_t	EvenCodeSum;		/* ż����ű����,���մ��ҵ�������� */
	uint16_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	OddCodeSum = 0;
	EvenCodeSum = 0;
	pbBuf = bBuf;
	for (nCount = 0; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		if (nCount % 2)
		{
			EvenCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* ����У���ַ�������ż��λ���ַ��� */
		}
		else
		{
			OddCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* ����У���ַ�����������λ���ַ��� */
		}
		pbBuf++;
	}
	pbBuf = NULL;
	
	EvenCodeSum *= 3;						/* �ò�С��ż��*3+�����͵�10���������������Ӻ�ֵ*/
	EvenCodeSum += OddCodeSum;
	bResult = EvenCodeSum % 10;
	if (bResult != 0)
	{
		bResult = 10 - bResult;
	}
	bResult += EAN_UPC_CODE_MIN;
	
	return	(bResult);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��У����ŵ�ָ���ַ�����EAN13��У����λ��
* Input          : �������:�Ѿ���׼���˵�EAN13�ַ�����,��12����13�ֽڳ���
* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*		d13			d12	~	d7	 	d6	~	d2		d1
*	ǰ�÷�		�������		�Ҳ�����		У���
*	ʵ�������������������:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	ǰ�÷�		�������		�Ҳ�����		У���
* Output         : �������:
*******************************************************************************/
void	SetEAN13CheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ EAN13_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ EAN13_DATA_MAX_LEN ] = '\0';	 //�����ַ���������
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȡ��ָ���ַ�����EAN13��У����
* Input          : �������:�Ѿ���׼���˵�EAN13�ַ�����,��12����13�ֽڳ���
* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*		d13			d12	~	d7	 	d6	~	d2		d1
*	ǰ�÷�		�������		�Ҳ�����		У���
*	ʵ�������������������:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	ǰ�÷�		�������		�Ҳ�����		У���
* Output         : �������:У����
*******************************************************************************/
uint8_t	GetEAN13CheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ EAN13_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ EAN13_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ָ���ַ�����EAN8��У����
* Input          : �������:�Ѿ���׼���˵�EAN8�ַ�����,��7����8�ֽڳ���
* EAN8�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	~	d5	 	d4	~	d2		d1
*	�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d1	~	d4	 	d5	~	d7		d8
*	�������		�Ҳ�����		У���
* Output         : �������:������У����, ��Χ'0'~'9'
*******************************************************************************/
uint8_t	CalEAN8CheckCode(uint8_t	bBuf[])
{
	uint8_t	bResult;
	uint16_t	OddCodeSum;			/* ������ű����,���մ��ҵ�������� */
	uint16_t	EvenCodeSum;		/* ż����ű����,���մ��ҵ�������� */
	uint16_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	OddCodeSum = 0;
	EvenCodeSum = 0;
	pbBuf = bBuf;
	for (nCount = 0; nCount < (EAN8_DATA_MAX_LEN - 1); nCount++)
	{
		if (nCount % 2)
		{
			OddCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* ����У���ַ�����������λ���ַ��� */
		}
		else
		{
			EvenCodeSum += (*pbBuf - EAN_UPC_CODE_MIN);	/* ����У���ַ�������ż��λ���ַ��� */
		}
		pbBuf++;
	}
	pbBuf = NULL;
	
	EvenCodeSum *= 3;						/* �ò�С��ż��*3+�����͵�10���������������Ӻ�ֵ*/
	EvenCodeSum += OddCodeSum;
	bResult = EvenCodeSum % 10;
	if (bResult != 0)
	{
		bResult = 10 - bResult;
	}
	bResult += EAN_UPC_CODE_MIN;
	
	return	(bResult);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��У����ŵ�ָ���ַ�����EAN8��У����λ��
* Input          : �������:�Ѿ���׼���˵�EAN8�ַ�����,��7����8�ֽڳ���
* EAN8�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	~	d5	 	d4	~	d2		d1
*	�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d1	~	d4	 	d5	~	d7		d8
*	�������		�Ҳ�����		У���

* Output         : �������:
*******************************************************************************/
void	SetEAN8CheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ EAN8_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ EAN8_DATA_MAX_LEN ] = '\0';	 //�����ַ���������
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȡ��ָ���ַ�����EAN8��У����
* Input          : �������:�Ѿ���׼���˵�EAN8�ַ�����,��7����8�ֽڳ���
* EAN8�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	~	d5	 	d4	~	d2		d1
*	�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d1	~	d4	 	d5	~	d7		d8
*	�������		�Ҳ�����		У���
* Output         : �������:У����
*******************************************************************************/
uint8_t	GetEAN8CheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ EAN8_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ EAN8_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����ָ���������༰�������ݳ��ȼ���ʣ���ӡ�ռ��Ƿ��ܹ���������
* Input          : 
* Output         : �������:���ܷ��·���0, �ܹ����·���1
*******************************************************************************/
uint8_t	CheckPrtSpace(uint8_t bType, uint8_t bCodeLen)
{
	uint16_t	nPrtWidth;					/* ��ӡ���, ���� */
	uint8_t		Result;
	
	if (bType == TYPE_EAN13)
	{
		nPrtWidth = \
			EAN_CODE_CONTENT_VALID_BIT_LEN * (EAN13_DATA_MAX_LEN - 1) + \
			EAN_START_VALID_BIT_LEN + \
			EAN_MIDDLE_VALID_BIT_LEN + \
			EAN_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* ��ӡ�������Ų���, ����ӡ */
		else
			Result = 1;
	}
	else if (bType == TYPE_EAN8)
	{
		nPrtWidth = \
			EAN_CODE_CONTENT_VALID_BIT_LEN * (EAN8_DATA_MAX_LEN - 1) + \
			EAN_START_VALID_BIT_LEN + \
			EAN_MIDDLE_VALID_BIT_LEN + \
			EAN_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* ��ӡ�������Ų���, ����ӡ */
		else
			Result = 1;
	}
	else if (bType == TYPE_UPCE)
	{
		nPrtWidth = \
			UPC_CODE_CONTENT_VALID_BIT_LEN * (UPCE_DATA_MAX_LEN - 2) + \
			UPC_START_VALID_BIT_LEN + \
			UPCE_STOP_VALID_BIT_LEN;
		nPrtWidth	*= g_tBarCodeCtrl.BarCodeThinWidth;
		if (nPrtWidth > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex))
			Result = 0;			/* ��ӡ�������Ų���, ����ӡ */
		else
			Result = 1;
	}
	else
	{
		Result = 0;		/* δ֪���뷽ʽ */
	}
	
	return	Result;
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����׼EAN13�ַ�����ת��Ϊ�ɴ�ӡ��������
* Input          : �������:�Ѿ���׼���˵�EAN13�ַ�����,��13�ֽڳ���
*	ת������洢���������bPrtBuf, ÿ�����ȵ��ӦbPrtBuf[]��һ���ֽ�
*	����ΪFF��ʾ��ӡΪ��,����Ϊ0��ʾ��ӡΪ�հ�.
* EAN13�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*		d13			d12	~	d7	 	d6	~	d2		d1
*	ǰ�÷�		�������		�Ҳ�����		У���
*	ʵ�������������������:
*		d0			d1	~	d6	 	d7	~	d11		d12
*	ǰ�÷�		�������		�Ҳ�����		У���
* ǰ���벻������������ݷ���, ���������ַ���ʾ.
* ����ǰ����ȷ���������ݵı��뷽ʽ.
*	EAN13������ݸ���ǰ�÷�ȷ��������ݱ��뼯A����B
*	EAN13�Ҳ����ݼ�У��������ñ��뼯C
*	ENA13���� ��ʼ�� + ������ݷ�6�� + �м����� + �Ҳ�����5�� + У���1�� + ������
*	����ʱ�ȸ���ǰ�÷��ж�������ݵı��뷽ʽ, �����������, �������������ֵ
* Output         : �������:�ܿɴ�ӡ�������Ŀ��, �Ե�Ϊ��λ����, 
*	�����ӡ�������ռ�Ų��»��߳��ִ���, ����0
*	��Ҫ�������붨���ȼ������͵������ֵ
*******************************************************************************/
uint16_t	EAN13ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	uint8_t	bCodeTypeSel;					/* ������ݱ������༯ѡ��*/
	/* �ɴ�ӡ����ֵ��ȡ��, ���д������ */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* ��ӡ���, ���� */
	uint16_t	nPrtWidth;					/* ��ӡ���, ���� */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* ���жϵ�ǰ�ɴ�ӡ�����Ƿ��ܹ����±��δ����ı����ӡ���� */
	/* ���봦��, ��������ڴ���� */
	if (CheckPrtSpace(TYPE_EAN13, EAN13_DATA_MAX_LEN) == 0)
		return 0;			/* ��ӡ�������Ų���, ����ӡ */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	pbSrcBuf = bBuf;
	assert_param(IS_EAN13_PRE_CODE( *pbSrcBuf ));
	bCodeTypeSel = EANLeftCharCodeTypeSel[ *pbSrcBuf - EAN13_PRE_CODE_MIN ];
	pbSrcBuf++;
	
	/* ���������������ȡ����Ӧ�Ŀɴ�ӡ���ݱ���ֵ */
	for (nCount = 0; nCount < EAN13_LEFT_DATA_LEN; nCount++)
	{
		if ((bCodeTypeSel & (1 << (EAN13_LEFT_DATA_LEN - 1 - nCount))))
		{
			pbCodeContentBuf = EANContentCodeType_B;	/* �ӱ����ַ���B��ѡ�� */
		}
		else
		{
			pbCodeContentBuf = EANContentCodeType_A;	/* �ӱ����ַ���A��ѡ�� */
		}
		/* ����ǰ�ַ��Ŀɴ�ӡ���ݱ���ֵȡ�� */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	/* ������ʣ���Ҳ༰У��������ȡ����Ӧ�Ŀɴ�ӡ���ݱ���ֵ */
	pbCodeContentBuf = EANContentCodeType_C;	/* �ӱ����ַ���C��ѡ�� */
	for (; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* ���濪ʼ���ɴ�ӡ�������� */
	nPrtWidth = 0;								/* ͳ��������ӡ���� */
	pbDstBuf = bPrtBuf;
	/* ���������ʼ�� b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStartCode, \
																EAN_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* ������������ */
	/* ÿ�����ݵ��������ռ��7λ,����ǰ�÷���ֵ��A,B�������뼯��ѡ�� */
	/* �������λ0��Ч,����7λ��Ч. ���մ�MSB->LSB��˳�����*/
	/* ǰ�÷��ĵ�6λֵ��MSB->LSB���ζ�Ӧ6��������� */
	for (nCount = 0; nCount < EAN13_LEFT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* ����м�ָ��� */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANMiddleCode, \
																EAN_MIDDLE_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* ���ʣ���ַ���У��� */
	for (; nCount < (EAN13_DATA_MAX_LEN - 1); nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* ��������ֹ�� */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStopCode, \
																EAN_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����׼EAN8�ַ�����ת��Ϊ�ɴ�ӡ��������
* Input          : �������:�Ѿ���׼���˵�EAN8�ַ�����,��8�ֽڳ���
*	ת������洢���������bPrtBuf, ÿ�����ȵ��ӦbPrtBuf[]��һ���ֽ�
*	����ΪFF��ʾ��ӡΪ��,����Ϊ0��ʾ��ӡΪ�հ�.
* EAN8�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	~	d5	 	d4	~	d2		d1
*	�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d1	~	d4	 	d5	~	d7		d8
*	�������		�Ҳ�����		У���
* ��ǰ����.
*	EAN8������ݲ��ñ��뼯A
*	EAN8�Ҳ����ݼ�У��������ñ��뼯C
*	ENA8���� ��ʼ�� + ������ݷ�4�� + �м����� + �Ҳ�����3�� + У���1�� + ������
* Output         : �������:�ܿɴ�ӡ�������Ŀ��, �Ե�Ϊ��λ����, 
*	�����ӡ�������ռ�Ų��»��߳��ִ���, ����0
*	��Ҫ�������붨���ȼ������͵������ֵ
*******************************************************************************/
uint16_t	EAN8ChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	/* �ɴ�ӡ����ֵ��ȡ��, ���д������ */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* ��ӡ���, ���� */
	uint16_t	nPrtWidth;					/* ��ӡ���, ���� */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* ���жϵ�ǰ�ɴ�ӡ�����Ƿ��ܹ����±��δ����ı����ӡ���� */
	/* ���봦��, ��������ڴ���� */
	if (CheckPrtSpace(TYPE_EAN8, EAN8_DATA_MAX_LEN) == 0)
		return 0;			/* ��ӡ�������Ų���, ����ӡ */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	pbSrcBuf = bBuf;
	/* ���������������ȡ����Ӧ�Ŀɴ�ӡ���ݱ���ֵ */
	pbCodeContentBuf = EANContentCodeType_A;	/* �ӱ����ַ���A��ѡ�� */
	for (nCount = 0; nCount < EAN8_LEFT_DATA_LEN; nCount++)
	{
		/* ����ǰ�ַ��Ŀɴ�ӡ���ݱ���ֵȡ�� */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	/* ������ʣ���Ҳ༰У��������ȡ����Ӧ�Ŀɴ�ӡ���ݱ���ֵ */
	pbCodeContentBuf = EANContentCodeType_C;	/* �ӱ����ַ���C��ѡ�� */
	for (; nCount < EAN8_DATA_MAX_LEN; nCount++)
	{
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* ���濪ʼ���ɴ�ӡ�������� */
	nPrtWidth = 0;								/* ͳ��������ӡ���� */
	pbDstBuf = bPrtBuf;
	/* ���������ʼ�� b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStartCode, \
																EAN_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* ������������ */
	/* ÿ�����ݵ��������ռ��7λ,����ǰ�÷���ֵ��A,B�������뼯��ѡ�� */
	/* �������λ0��Ч,����7λ��Ч. ���մ�MSB->LSB��˳�����*/
	/* ǰ�÷��ĵ�6λֵ��MSB->LSB���ζ�Ӧ6��������� */
	for (nCount = 0; nCount < EAN8_LEFT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* ����м�ָ��� */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANMiddleCode, \
																EAN_MIDDLE_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* ���ʣ���ַ���У��� */
	for (; nCount < EAN8_DATA_MAX_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	EAN_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* ��������ֹ�� */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																EANStopCode, \
																EAN_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

uint8_t	GetPrtDataFromCodeContentBuf(const uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE( bCode ));
	
	return	(bBuf[ bCode - EAN_UPC_CODE_MIN]);
}

/*******************************************************************************
* Function Name  : 
* Description    : ��ָ���Ļ�����д��һ��λ, �ظ�д��Ĵ����������������ȷ��
* Input          : 
* Output         : 
*******************************************************************************/
void	SetOneBitToPrtBuf(uint8_t	bBuf[], uint8_t	bCode, uint32_t	nWidth)
{
	uint8_t	* pbBuf = NULL;
	
	pbBuf = bBuf;
	while (nWidth)
	{
		*pbBuf = bCode;
		pbBuf++;
		nWidth--;
	}
	pbBuf = NULL;
}

/*******************************************************************************
* Function Name  : 
* Description    : 	��ָ���Ļ�����д��һ���ֽ�, д��������������������ȷ��
* Input          : 	bCodeָ��д������, 
										bBitNumָ��Ҫд���λ��,���8,��С1,
*										λ����LSB��ʼ����, ����д��ʱ���մ�MSB->LSB����д��.
*										���λ������8, ����Щ��λ�����ݲ�д��.
*										nWidth��ʾ���ݵ�ÿ��λ���Ĵ���.
* Output         :	����д������ռ�õ��ܵ������ֵ.
*******************************************************************************/
uint32_t	SetOneByteToPrtBuf(uint8_t bBuf[], uint8_t bCode, uint8_t bBitNum, uint32_t nWidth)
{
	uint8_t	i;
	uint8_t	bFillData;
	uint32_t	nCount;				/* д������ռ�õĵ������ֵ */
	uint8_t	* pbBuf = NULL;
	
	if (bBitNum == 0)
		return	(0);
	
	nCount = 0;
	pbBuf = bBuf;
	for (i = 0; i < bBitNum; i++)
	{
		if ((bCode & (1 << (bBitNum - 1 - i))))
		{
			bFillData = 0xFF;
		}
		else
		{
			bFillData = 0x0;
		}
		SetOneBitToPrtBuf(pbBuf, bFillData, nWidth);
		pbBuf += nWidth;
		nCount += nWidth;
	}
	pbBuf = NULL;
	
	return	(nCount);
}

/*******************************************************************************
* Function Name  : 
* Description    : ����ָ�����ݳ��Ƚ�ean13�����ݱ���ɿɴ�ӡ����
* Input          : bBufΪ������EAN13��������, ����ǰ�÷�, ���ǲ�һ������У���
*										bCodeLenΪ�����������еĳ���,12����13.
*										�������Ϊ12, �Զ����У��λ; ����ֱ��ʹ��У��λ�������
* Output         : ����д��󷵻�0, ���򷵻�ʵ�ʱ༭����������
*******************************************************************************/
uint16_t	FormatCodeEAN13(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint16_t Width;
	
	if (bCodeLen == 12)		/* ��Ҫ�Զ����У���ַ� */
	{
		Temp = CalEAN13CheckCode(bBuf);
		SetEAN13CheckCode(bBuf, Temp);
		Width = EAN13ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 13)	/* ֱ��ʹ�ý��յ���У���ַ������ټ����� */
	{
		Width = EAN13ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : ����ָ�����ݳ��Ƚ�UPCA�����ݱ���ɿɴ�ӡ����. UPCA�൱��
*										ǰ�÷�0��EAN13.����ֻ��Ҫ���ǰ�÷�0,Ȼ����EAN13������.
* Input          : bBufΪ������UPCA��������, ���ǲ�����ǰ�÷�, ��һ������У���
*										bCodeLenΪ�����������еĳ���,11����12.
*										�������Ϊ11, �Զ����У��λ; ����ֱ��ʹ��У��λ�������
* Output         : ����д��󷵻�0, ���򷵻�ʵ�ʱ༭����������
*******************************************************************************/
uint16_t	FormatCodeUPCA(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint8_t	bEAN13[EAN13_DATA_MAX_LEN + 1];	/* ��Ҫ���������, ��ֹ�±�Խ�� */
	uint16_t Width;
	uint8_t	* pbBuf = NULL;
	uint8_t	* pbEAN = NULL;
	
	if (bCodeLen > UPCA_DATA_MAX_LEN)	/* ָ�����ݳ���,�������� */
		return	0;
	
	pbBuf = bBuf;
	pbEAN = bEAN13;
	*pbEAN++ = EAN13_PRE_CODE_MIN;		/* ���ǰ�÷�0 */
	for (Temp = 0; Temp < bCodeLen; Temp++)		/* ������������ */
		*pbEAN++ = *pbBuf++;
	
	if (bCodeLen == 11)		/* ��Ҫ�Զ����У���ַ� */
	{
		Temp = CalEAN13CheckCode(bEAN13);
		SetEAN13CheckCode(bEAN13, Temp);
		Width = EAN13ChangeToPrintData(bEAN13, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
		/* ����ϴ�ӡ�ַ���У����,�����ӡ�ַ���У���� */
		pbBuf = bBuf;
		pbBuf += (UPCA_DATA_MAX_LEN - 1);
		*pbBuf++ = Temp;
		*pbBuf = '\0';		/* ��Ҫ��������� */
	}
	else if (bCodeLen == 12)	/* ֱ��ʹ�ý��յ���У���ַ������ټ����� */
	{
		Width = EAN13ChangeToPrintData(bEAN13, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : ����ָ�����ݳ��Ƚ�ean8�����ݱ���ɿɴ�ӡ����
* Input          : bBufΪ������EAN8��������, ��ǰ�÷�, ���ǲ�һ������У���
*										bCodeLenΪ�����������еĳ���,7����8.
*										�������Ϊ7, �Զ����У��λ; ����ֱ��ʹ��У��λ�������
* Output         : ����д��󷵻�0, ���򷵻�ʵ�ʱ༭����������
*******************************************************************************/
uint16_t	FormatCodeEAN8(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint16_t Width;
	
	if (bCodeLen == 7)		/* ��Ҫ�Զ����У���ַ� */
	{
		Temp = CalEAN8CheckCode(bBuf);
		SetEAN8CheckCode(bBuf, Temp);
		Width = EAN8ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 8)	/* ֱ��ʹ�ý��յ���У���ַ������ټ����� */
	{
		Width = EAN8ChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}

/*******************************************************************************
* Function Name  : 
* Description    : ��UPCE�ı������и�ԭΪUPCA����������������У����
* Input          : UPC�ı�������, ��У����
* Output         : ��ԭΪUPCA�ı�������, ע��UPE�������еĳ���Ҫ�㹻����UPCA����
*										����ֵ������У����, У������Ҫ����������õ�
	*	UPC-E������8λ�������,�ǽ�ϵͳ�ַ�Ϊ0��UPC-12����ѹ������.
	*	UPC-E���ַ���Ϊ0,�м�Ϊ6λ����,���Ϊ1λУ����,
	*	UPC-E���� ��ʼ�� + ������ݷ�3�� + �Ҳ�����3�� + ������,���м�������У���
	
		UPC-A����ѹ����������:
											UPC-A����																					UPC-E����
						����ʶ�����						��Ʒ��Ŀ����		У����			��Ʒ��Ŀʶ�����			У����
		X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 0  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 1  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					0				X11 X10 2  0  0   0  0  X4 X3 X2			X1			0 X11 X10 X4 X3 X2 X9 	X1
					
					0				X11 X10 3  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 ...0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					0				X11 X10 9  0  0   0  0  0  X3 X2			X1			0 X11 X10 X9 X3 X2 3  	X1
					
					0				X11 X10 X9 1  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 ...0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					0				X11 X10 X9 9  0   0  0  0  0  X2			X1			0 X11 X10 X9 X8 X2 4  	X1
					
					0				�����βX7 != 0   0  0  0  0	5       X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				�����βX7 != 0   0  0  0  0	...     X1			0 X11 X10 X9 X8 X7 X2  	X1
					0				�����βX7 != 0   0  0  0  0	9       X1			0 X11 X10 X9 X8 X7 X2  	X1
		
	*	UPC-E�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
	*		d8	~		d7	~	d2	 				d1
	*	ϵͳ�ַ�0		��Ʒ��Ŀ����		У����
	*	У����Ϊ����ǰ��UPC-A�������õ���У����.
	*	UPC-E���м�ָ���.
	* UPC-E��ֹ����ͬ��EAN8, Ϊ010101.
	*	UPC-E���ݱ������У��λֵȷ��ѡ���ַ���A����B.��EAN8���벻ͬ.
	*	UPC-E��ϵͳ�ַ�X8��У����X1���������ַ���ʾ.
	UPC-E�ָ�ΪUPC-A���㷨:
	UPCE������
	d8 d7 d6 d5 d4 d3 d2 d1
*******************************************************************************/
void	UPCERestoreUPCA(uint8_t	bBuf[])
{
	uint8_t	bTemp;
	uint8_t	bUPCA[EAN13_DATA_MAX_LEN];
	uint32_t	nCount;
	uint8_t	* pbBuf = NULL;
	
	pbBuf = bBuf;
	pbBuf += (UPCE_DATA_MAX_LEN - 2);
	bTemp = *pbBuf;		/* �õ�d2 */
	
	/* �Ȱ���UPCA����˳������� bUPCA[] */
	if ((bTemp == '0') || (bTemp == '1') || (bTemp == '2'))
	{
	/*
									UPC-A����																					UPC-E����
				����ʶ�����						��Ʒ��Ŀ����		У����		��Ʒ��Ŀʶ�����		У����
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2	d1
	X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 0  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 1  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
				0				X11 X10 2  0  0   0  0  X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9 X1
	*/
		for (nCount = 0; nCount < 3; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		bUPCA[3] = bTemp;
		for (nCount = 4; nCount < 8; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 3;
		for (nCount = 8; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else if (bTemp == '3')
	{
	/*
									UPC-A����																					UPC-E����
				����ʶ�����						��Ʒ��Ŀ����		У����		��Ʒ��Ŀʶ�����			У����
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				X11 X10 3  0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
				0				X11 X10 ...0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
				0				X11 X10 9  0  0   0  0  0  X3 X2	X1			0 X11 X10 X9 X3 X2 3  	X1
	*/
		for (nCount = 0; nCount < 4; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 4; nCount < 9; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 4;
		for (nCount = 9; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else if (bTemp == '4')
	{
		/*
									UPC-A����																					UPC-E����
				����ʶ�����						��Ʒ��Ŀ����		У����		��Ʒ��Ŀʶ�����			У����
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				X11 X10 X9 1  0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
				0				X11 X10 X9 ...0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
				0				X11 X10 X9 9  0   0  0  0  0  X2	X1			0 X11 X10 X9 X8 X2 4  	X1
	*/
		for (nCount = 0; nCount < 5; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 5; nCount < 10; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 5;
		for (nCount = 10; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	else
	{
		/*
									UPC-A����																					UPC-E����
				����ʶ�����						��Ʒ��Ŀ����		У����		��Ʒ��Ŀʶ�����			У����
					d12		d11	d10	d9 d8	d7	d6 d5 d4 d3 d2	d1			d8 d7 d6	d5 d4 d3 d2		d1
	X12(ϵͳ�ַ�)	X11 X10 X9 X8 X7	X6 X5 X4 X3 X2	X1			0 X11 X10 X4 X3 X2 X9		X1
				0				�����βX7 != 0   0  0  0  0	5   X1			0 X11 X10 X9 X8 X7 X2  	X1
				0				�����βX7 != 0   0  0  0  0	... X1			0 X11 X10 X9 X8 X7 X2  	X1
				0				�����βX7 != 0   0  0  0  0	9   X1			0 X11 X10 X9 X8 X7 X2  	X1
	*/
		for (nCount = 0; nCount < 6; nCount++)
		{
			bUPCA[nCount] = bBuf[nCount];
		}
		for (nCount = 6; nCount < 10; nCount++)
		{
			bUPCA[nCount] = '0';
		}
		pbBuf = bBuf;
		pbBuf += 6;
		for (nCount = 10; nCount < 11; nCount++)
		{
			bUPCA[nCount] = *pbBuf;
			pbBuf++;
		}
	}
	pbBuf = NULL;
	
	/* �ٽ��䷵��,���ص���UPCA,������У���� */
	for (nCount = 0; nCount < UPCA_DATA_MAX_LEN; nCount++)
	{
		bBuf[nCount] = bUPCA[nCount];
	}
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��У����ŵ�ָ���ַ�����UPCE��У����λ��
* Input          : �������:�Ѿ���׼���˵�UPCE�ַ�����,��7����8�ֽڳ���
* UPCE�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		�������		�Ҳ�����		У���
* Output         : �������:
*******************************************************************************/
void	SetUPCECheckCode(uint8_t	bBuf[], uint8_t	bCode)
{
	assert_param(IS_EAN_UPC_CODE(bCode));
	bBuf[ UPCE_DATA_MAX_LEN - 1 ] = bCode;
	bBuf[ UPCE_DATA_MAX_LEN ] = '\0';	 //�����ַ���������
}

/*******************************************************************************
* Function Name  : ������
* Description    : ȡ��ָ���ַ�����UPCE��У����
* Input          : �������:�Ѿ���׼���˵�UPCE�ַ�����,��7����8�ֽڳ���
* UPCE�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		�������		�Ҳ�����		У���
* Output         : �������:У����
*******************************************************************************/
uint8_t	GetUPCECheckCode(uint8_t	bBuf[])
{
	assert_param(IS_EAN_UPC_CODE(bBuf[ UPCE_DATA_MAX_LEN - 1 ]));
	
	return	(bBuf[ UPCE_DATA_MAX_LEN - 1 ]);
}

/*******************************************************************************
* Function Name  : ������
* Description    : ����׼UPCE�ַ�����ת��Ϊ�ɴ�ӡ��������
* Input          : �������:�Ѿ���׼���˵�UPCE�ַ�����,��8�ֽڳ���
*	ת������洢���������bPrtBuf, ÿ�����ȵ��ӦbPrtBuf[]��һ���ֽ�
*	����ΪFF��ʾ��ӡΪ��,����Ϊ0��ʾ��ӡΪ�հ�.
* UPCE�������и���λ�õ�λ����������:����У��������,���ҵ����˳���
*	d8	d7	~	d5	 	d4	~	d2		d1
*	0		�������		�Ҳ�����		У���
*	ʵ�������������������:
*	d0	d1	~	d3	 	d4	~	d6		d7
*	0		�������		�Ҳ�����		У���
* ǰ����, У���벻���������ݷ���, ���������ַ���ʾ.
* ����У����ȷ���������ݵı��뷽ʽ,���ݱ��뼯A����B.
*	UPCE���� ��ʼ�� + ����6�� + ������
* Output         : �������:�ܿɴ�ӡ�������Ŀ��, �Ե�Ϊ��λ����, 
*	�����ӡ�������ռ�Ų��»��߳��ִ���, ����0
*	��Ҫ�������붨���ȼ������͵������ֵ
*******************************************************************************/
uint16_t	UPCEChangeToPrintData(uint8_t	bBuf[], uint8_t	bPrtBuf[])
{
	uint8_t	bCodeTypeSel;					/* ���ݱ������༯ѡ��*/
	/* �ɴ�ӡ����ֵ��ȡ��, ���д������ */
	uint8_t	bContentBuf[EAN13_DATA_MAX_LEN];
	uint16_t	nCount;
	uint16_t	nWidth;							/* ��ӡ���, ���� */
	uint16_t	nPrtWidth;					/* ��ӡ���, ���� */
	uint8_t	* pbSrcBuf = NULL;
	uint8_t	* pbDstBuf = NULL;
	const uint8_t	* pbCodeContentBuf = NULL;
	
	/* ���жϵ�ǰ�ɴ�ӡ�����Ƿ��ܹ����±��δ����ı����ӡ���� */
	/* ���봦��, ��������ڴ���� */
	if (CheckPrtSpace(TYPE_UPCE, UPCE_DATA_MAX_LEN) == 0)
		return 0;			/* ��ӡ�������Ų���, ����ӡ */
	
	memset(bContentBuf, sizeof(bContentBuf) / sizeof(bContentBuf[0]), 0x00);
	bCodeTypeSel = UPCECharCodeTypeSel[ GetUPCECheckCode(bBuf) - EAN_UPC_CODE_MIN ];
	
	/* ������������ȡ����Ӧ�Ŀɴ�ӡ���ݱ���ֵ */
	pbSrcBuf = bBuf;
	assert_param(IS_UPCE_PRE_CODE( *pbSrcBuf ));
	pbSrcBuf++;		/* ������ǰ�÷� */
	for (nCount = 0; nCount < UPCE_PRT_DATA_LEN; nCount++)
	{
		if ((bCodeTypeSel & (1 << (UPCE_PRT_DATA_LEN - 1 - nCount))))
		{
			pbCodeContentBuf = EANContentCodeType_B;	/* �ӱ����ַ���B��ѡ�� */
		}
		else
		{
			pbCodeContentBuf = EANContentCodeType_A;	/* �ӱ����ַ���A��ѡ�� */
		}
		/* ����ǰ�ַ��Ŀɴ�ӡ���ݱ���ֵȡ�� */
		bContentBuf[nCount] = GetPrtDataFromCodeContentBuf(pbCodeContentBuf, *pbSrcBuf);
		pbSrcBuf++;
	}
	
	/* ���濪ʼ���ɴ�ӡ�������� */
	nPrtWidth = 0;								/* ͳ��������ӡ���� */
	pbDstBuf = bPrtBuf;
	/* ���������ʼ�� b101 */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																UPCStartCode, \
																UPC_START_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	pbDstBuf += nWidth;
	
	/* ��������� */
	/* ÿ�����ݵ��������ռ��7λ,����У�����ֵ��A,B�������뼯��ѡ�� */
	/* �������λ0��Ч,����7λ��Ч. ���մ�MSB->LSB��˳�����*/
	for (nCount = 0; nCount < UPCE_PRT_DATA_LEN; nCount++)
	{
		nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																	bContentBuf[nCount], \
																	UPC_CODE_CONTENT_VALID_BIT_LEN, \
																	g_tBarCodeCtrl.BarCodeThinWidth);
		nPrtWidth += nWidth;
		pbDstBuf += nWidth;
	}
	
	/* ��������ֹ�� */
	nWidth = SetOneByteToPrtBuf(	pbDstBuf, \
																UPCEStopCode, \
																UPCE_STOP_VALID_BIT_LEN, \
																g_tBarCodeCtrl.BarCodeThinWidth);
	nPrtWidth += nWidth;
	
	pbSrcBuf = NULL;
	pbDstBuf = NULL;
	pbCodeContentBuf = NULL;
	
	return	(nPrtWidth);
}

/*******************************************************************************
* Function Name  : 
* Description    : ����ָ�����ݳ��Ƚ�UPCE�����ݱ���ɿɴ�ӡ����. UPCE�൱��ѹ��
*										ǰ�÷�0��UPCA.������Ҫ���ݲ�ͬ�������лָ�Ϊԭѹ��ǰ��UPC-A,
*										��������Ҫ����У����,Ȼ���ٴ���.
* Input          :  bBufΪUPCE��������, ���ǲ�һ������ǰ�÷�, ��һ������У���
*										bCodeLenΪ�����������еĳ���,6,7,8,11����12.
*										�������Ϊ6, ���Զ����ǰ�÷�0,
*										�������Ϊ7,8,11����12, ǰ�÷�����Ϊ����Ϊ0,
*										�������Ϊ6,7,11, �Զ����У��λ,
*										�������Ϊ8����12, ֱ��ʹ��У��λ�������
* Output         : ����д��󷵻�0, ���򷵻�ʵ�ʱ༭����������
*******************************************************************************/
uint16_t	FormatCodeUPCE(uint8_t	bBuf[], uint8_t	bCodeLen)
{
	uint8_t	Temp;
	uint8_t	bEAN13[EAN13_DATA_MAX_LEN + 1];	/* ��Ҫ���������, ��ֹ�±�Խ�� */
	uint8_t	bUPCA[EAN13_DATA_MAX_LEN + 1];	/* ��Ҫ���������, ��ֹ�±�Խ�� */
	uint16_t Width;
	uint8_t	* pbBuf = NULL;
	uint8_t	* pbEAN = NULL;
	
	/* �ȼ������������Ƿ�Ϸ� */
	if (bCodeLen > UPCA_DATA_MAX_LEN)	/* ָ�����ݳ���,�������� */
		return	0;
	
	if ((bCodeLen == 7) || (bCodeLen == 8) || (bCodeLen == 11) || (bCodeLen == 12))
	{	/* �������Ϊ7,8,11����12, ǰ�÷�����Ϊ����Ϊ0 */
		if (bBuf[0] != UPCE_DEFAULT_PRE_CODE)
			return	0;
	}
	if (bCodeLen == 6)
	{
		/* �������Ϊ6, ���Զ����ǰ�÷�0 */
		pbBuf = bBuf;
		pbEAN = bUPCA;
		*pbEAN++ = UPCE_DEFAULT_PRE_CODE;		/* ���ǰ�÷�0 */
		for (Temp = 0; Temp < bCodeLen; Temp++)		/* ������������ */
		{
			*pbEAN++ = *pbBuf++;
		}
		bCodeLen += 1;	/* ���ݳ��ȼ�1 */
		
		/* �޸Ķ�Ӧ������������ */
		pbBuf = bBuf;
		pbEAN = bUPCA;
		for (Temp = 0; Temp < bCodeLen; Temp++)
		{
			*pbBuf++ = *pbEAN++;
		}
	}
	
	if ((bCodeLen == 11) || (bCodeLen == 12))	/* ��ΪUPCA��ӡ, �Զ�����У���� */
	{
		Width = FormatCodeUPCA(bBuf, bCodeLen);
	}
	else if (bCodeLen == 7)	/* ��ΪUPCE��ӡ, �����У���ַ� */
	{
		pbBuf = bBuf;
		pbEAN = bUPCA;
		for (Temp = 0; Temp < bCodeLen; Temp++)		/* ������������ */
		{
			*pbEAN++ = *pbBuf++;
		}
		UPCERestoreUPCA(bUPCA);	/* �Ȼָ�Ϊԭѹ��ǰUPCA�������� */
		
		pbBuf = bUPCA;
		pbEAN = bEAN13;
		*pbEAN++ = EAN13_PRE_CODE_MIN;		/* ���ǰ�÷�0 */
		for (Temp = 0; Temp < UPCA_DATA_MAX_LEN; Temp++)	/* ������������ */
			*pbEAN++ = *pbBuf++;
		Temp = CalEAN13CheckCode(bEAN13);	/* ����У���ַ� */
		SetUPCECheckCode(bBuf, Temp);			/* ����UPC-EУ���ַ�,�����ַ��������� */
		/* ����У���ַ�ѡ���������ݱ��뼯������ */
		Width = UPCEChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else if (bCodeLen == 8)	/* ��ΪUPCE��ӡ, ֱ��ʹ�ý��յ���У���ַ������ټ����� */
	{
		/* ����У���ַ�ѡ���������ݱ��뼯������ */
		Width = UPCEChangeToPrintData(bBuf, g_tUnderlineBuf + g_tPrtCtrlData.PrtDataDotIndex);
	}
	else
	{
		Width = 0;
	}
	
	return	(Width);
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
