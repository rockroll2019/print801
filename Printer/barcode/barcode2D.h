/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   ��ά������ز�������
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
  * <h2><center>&copy; COPYRIGHT 2012</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BARCODE2D_H
#define BARCODE2D_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	<string.h>
#include	"stm32f10x.h"
#include	"ean.h"

/* Macro Definition ----------------------------------------------------------*/
//��ά���������



//QR��  ��ؽṹ����
#define	mm				8
#define	nn				255
#define	maxgen			20		   //QRCoder 	����������ʽ����
#define	maxdm			30		   //DataMatrix ���ͼ�δ�С����

#define	MODENUM			0x00
#define	MODENUMCHR		0x01
#define	MODEBYTE		0x02
#define	MODECHN			0x03
#define	MAXVER			13

typedef struct _Pixel
{
	u16		x;
	u16		y;
} Pixel, *pPixel;

typedef struct _Rangle{
	Pixel		p1;
	Pixel		p2;
}Rangle, *pRangle;

typedef	struct _MovePix
{
	Pixel		p0;
	u8		up;
	u8		left;
}MovePix, *pMovePix;

typedef	struct _Blocks
{
	u8		blocks;
	u8		CodeSize;
	u8		CorSize;
}Blocks, pBlocks;

typedef	struct _CorFeat
{
	u16		aCodesize;		//����������
	u16		aCorSize;		//�ܾ���������
	u8		Codesize;		//ÿ����������	
	u8		CorSize;		//ÿ�����������
	u8		blocks0;		//������1����
	u8		blocks1;		//������2����
}CorFeat, *pCorFeat;

typedef struct _QRFeat
{
	u16		Size;				//���汾���ɵ��ֽ���
	u8		chkpos[7];			//
	CorFeat	Cors[4];			//
} QRFeat, *pQRFeat;

typedef	struct _MatrixFeat
{
	u8		mHeight;
	u8		mwidth;
	u8		blocks;
	u8		xcount, ycount;
	u8		subheight;
	u8		subwidth;
	CorFeat	Cor;
	
}MatrixFeat, *pMatrixFeat;

u8		const	GraphPos[]	={0xFE, 0x82, 0xBA, 0xBA, 0xBA, 0x82, 0xFE, 0x00};
u8		const	GraphAlg[]	={0xF8, 0x88, 0xA8, 0x88, 0xF8};
u8		const	pp[9] 		={1, 0, 1, 1, 1, 0, 0, 0, 1};
u8		const	ppdm[9] 	={1, 0, 1, 1, 0, 1, 0, 0, 1};

u8		verid, corid, width, bytes, orploy;
u8		*pCanvas, *alpha_to, *index_of, *gg;
u8		*pCodeBuf, *pMask, *pRS, *pRS1, *pTMP;
u16		datalen;
int 	nrow, ncol;

MatrixFeat MatrixCapDM;						//DataMatrix �����ṹ
QRFeat	   VerCapVerid;						//QRCode�汾�����ṹ

#ifdef EXTFLASH

	#define MatrixCap_ADDR		(0x1f6500+0x1dc2)				//DataMatrix����������ⲿFLASH�е��׵�ַ
	#define VerCap_ADDR			(0x1f6500+0x1fa2)				//RQcode	����������ⲿFLASH�е��׵�ַ
	#define g_poly_ADDR			(0x1f6500+0x2632)				//QRcode	��������ʽ���ⲿFLASH�е��׵�ַ

#else

MatrixFeat	const	MatrixCap[maxdm] = 
{
	{
		10,		10,		1,		1,		1,		10,		10,	   //�ܸߡ��ܿ����ܿ�����X���������Y�����������ߡ����
		{3,		5,		3,		5,		1,		0}			   //�������ܾ���������������������������顢����
	},
	{
		8,		18,		1,		1,		1,		8,		18,
		{5,		7,		5,		7,		1,		0}
	},
	{
		12,		12,		1,		1,		1,		12,		12,
		{5,		7,		5,		7,		1,		0}
	},
	{
		14,		14,		1,		1,		1,		14,		14,
		{8,		10,		8,		10,		1,		0}
	},
	{
		8,		32,		2,		2,		1,		8,		16,
		{10,	11,		10,		11,		1,		0}
	},
	{
		16,		16,		1,		1,		1,		16,		16,
		{12,	12,		12,		12,		1,		0}
	},
	{
		12,		26,		1,		1,		1,		12,		26,
		{16,	14,		16,		14,		1,		0}
	},
	{
		18,		18,		1,		1,		1,		18,		18,
		{18,	14,		18,		14,		1,		0}
	},
	{
		12,		36,		2,		2,		1,		12,		18,
		{22,	18,		22,		18,		1,		0}
	},
	{
		20,		20,		1,		1,		1,		20,		20,
		{22,	18,		22,		18,		1,		0}
	},
	{
		22,		22,		1,		1,		1,		22,		22,
		{30,	20,		30,		20,		1,		0}
	},
	{
		16,		36,		2,		2,		1,		16,		18,
		{32,	24,		32,		24,		1,		0}
	},
	{
		24,		24,		1,		1,		1,		24,		24,
		{36,	24,		36,		24,		1,		0}
	},
	{
		26,		26,		1,		1,		1,		26,		26,
		{44,	28,		44,		28,		1,		0}
	},
	{
		16,		48,		2,		2,		1,		16,		24,
		{49,	28,		49,		28,		1,		0}
	},
	{
		32,		32,		4,		2,		2,		16,		16,
		{62,	36,		62,		36,		1,		0}
	},
	{
		36,		36,		4,		2,		2,		18,		18,
		{86,	42,		86,		42,		1,		0}
	},
	{
		40,		40,		4,		2,		2,		20,		20,
		{114,	48,		114,	48,		1,		0}
	},
	{
		44,		44,		4,		2,		2,		22,		22,
		{144,	56,		144,	56,		1,		0}
	},
	{
		48,		48,		4,		2,		2,		24,		24,
		{174,	68,		174,	68,		1,		0}
	},
	{
		52,		52,		4,		2,		2,		26,		26,
		{204,	84,		102,	42,		2,		0}
	},
	{
		64,		64,		16,		4,		4,		16,		16,
		{280,	112,	140,	56,		2,		0}
	},
	{
		72,		72,		16,		4,		4,		18,		18,
		{368,	144,	92,		36,		4,		0}
	},
	{
		80,		80,		16,		4,		4,		20,		20,
		{456,	192,	114,	48,		4,		0}
	},
	{
		88,		88,		16,		4,		4,		22,		22,
		{576,	224,	144,	56,		4,		0}
	},
	{
		96,		96,		16,		4,		4,		24,		24,
		{696,	272,	174,	68,		4,		0}
	},
	{
		104,	104,	16,		4,		4,		26,		26,
		{816,	336,	136,	56,		6,		0}
	},							
	{
		120,	120,	36,		6,		6,		20,		20,
		{1050,	408,	175,	68,		6,		0}
	},
	{
		132,	132,	36,		6,		6,		22,		22,
		{1304,	496,	163,	62,		8,		0}
	},
	{
		144,	144,	36,		6,		6,		24,		24,
		{1558,	620,	156,	62,		10,		0}
	}
};

QRFeat	const	VerCap[40] =				//QRCode���汾��Ӧ�Ĳ���
{
	{
	26,		//1													//�汾�ֽ���
		{
			0,		0,		0,		0,		0,		0,		0	//У��ͼ������ģ�����/������ֵ
		},
		{
			{19,	7,		19,		7,		1,		0},			//L����������������������������������������������������1������������2����
			{16,	10,		16,		10,		1,		0},			//M
			{13,	13,		13,		13,		1,		0},			//Q
			{9,		17,		9,		17,		1,		0}			//H
		}
	},
	{
	44,		//2
		{
			6,		18,		0,		0,		0,		0,		0
		},
		{
			{34,	10,		34,		10,		1,		0},
			{28,	16,		28,		16,		1,		0},
			{22,	22,		22,		22,		1,		0},
			{16,	28,		16,		28,		1,		0}
		}
	},
	{
	70,		///3
		{
			6,		22,		0,		0,		0,		0,		0
		},
		{
			{55,	15,		55,		15,		1,		0},
			{44,	26,		44,		16,		1,		0},
			{34,	36,		17,		18,		2,		0},
			{26,	44,		13,		22,		2,		0}
		}
	},
	{
	100,		///4
		{
			6,		26,		0,		0,		0,		0,		0
		},
		{
			{80,	20,		80,		20,		1,		0},
			{64,	36,		32,		18,		2,		0},
			{48,	52,		24,		26,		2,		0},
			{36,	64,		9,		16,		4,		0}
		}
	},
	{
	134,		///5
		{
			6,		30,		0,		0,		0,		0,		0
		},
		{
			{108,	26,		108,	26,		1,		0},
			{86,	48,		43,		24,		2,		0},
			{62,	72,		15,		18,		2,		2},
			{46,	88,		11,		22,		2,		2}
		}
	},
	{
	172,		///6
		{
			6,		34,		0,		0,		0,		0,		0
		},
		{
			{136,	36,		68,		18,		2,		0},
			{108,	64,		27,		16,		4,		0},
			{76,	96,		19,		24,		4,		0},
			{60,	112,	15,		28,		4,		0}
		}
	},
	{
	196,		///7
		{
			6,		22,		38,		0,		0,		0,		0
		},
		{
			{156,	40,		78,		20,		2,		0},
			{124,	72,		31,		18,		4,		0},
			{88,	108,	14,		18,		2,		4},
			{66,	130,	13,		26,		4,		1}
		}
	},
	{
	242,		///8
		{
			6,		24,		42,		0,		0,		0,		0
		},
		{
			{194,	48,		97,		24,		2,		0},
			{154,	88,		38,		22,		2,		2},
			{110,	132,	18,		22,		4,		2},
			{86,	156,	14,		26,		4,		2}
		}
	},
	{
	292,		///9
		{
			6,		26,		46,		0,		0,		0,		0
		},
		{
			{232,	60,		116,	30,		2,		0},
			{182,	110,	36,		22,		3,		2},
			{132,	160,	16,		20,		4,		4},
			{100,	192,	12,		24,		4,		4}
		}
	},
	{
	346,		///10
		{
			6,		28,		50,		0,		0,		0,		0
		},
		{
			{274,	72,		68,		18,		2,		2},
			{216,	130,	43,		26,		4,		1},
			{154,	192,	19,		24,		6,		2},
			{122,	224,	15,		28,		6,		2}
		}
	},
	{
	404,		///11
		{
			6,		30,		54,		0,		0,		0,		0
		},
		{
			{324,	80,		81,		20,		4,		0},
			{254,	150,	50,		30,		1,		4},
			{180,	224,	22,		28,		4,		4},
			{140,	264,	12,		24,		3,		8}
		}
	},
	{
	466,		///12
		{
			6,		32,		58,		0,		0,		0,		0
		},
		{
			{370,	96,		92,		24,		2,		2},
			{290,	176,	36,		22,		6,		2},
			{206,	260,	20,		26,		4,		6},
			{158,	308,	14,		28,		7,		4}
		}
	},
	{
	532,		///13
		{
			6,		34,		62,		0,		0,		0,		0
		},
		{
			{428,	104,	107,	26,		4,		0},
			{334,	198,	37,		22,		8,		1},
			{244,	288,	20,		24,		8,		4},
			{180,	352,	11,		22,		12,		4}
		}
	},
	{
	581,		///14
		{
			6,		26,		46,		66,		0,		0,		0
		},
		{
			{461,	120,	115,	30,		3,		1},
			{365,	216,	40,		24,		4,		5},
			{261,	320,	16,		20,		11,		5},
			{197,	384,	12,		24,		11,		5}
		}
	},
	{
	655,		///15
		{
			6,		26,		48,		70,		0,		0,		0
		},
		{
			{523,	132,	87,		22,		5,		1},
			{415,	240,	41,		24,		5,		5},
			{295,	360,	24,		30,		5,		7},
			{223,	432,	12,		24,		11,		7}
		}
	},
	{
	733,		///16
		{
			6,		26,		50,		74,		0,		0,		0
		},
		{
			{589,	144,	98,		24,		5,		1},
			{453,	280,	45,		28,		7,		3},
			{325,	408,	19,		24,		15,		2},
			{253,	480,	15,		30,		3,		13}
		}
	},
	{
	815,		///17
		{
			6,		30,		54,		78,		0,		0,		0
		},
		{
			{647,	168,	107,	28,		1,		5},
			{507,	308,	46,		28,		10,		1},
			{367,	448,	22,		28,		1,		15},
			{283,	532,	14,		28,		2,		17}
		}
	},
	{
	901,		///18
		{
			6,		30,		56,		82,		0,		0,		0
		},
		{
			{721,	180,	120,	30,		5,		1},
			{563,	338,	43,		26,		9,		4},
			{397,	504,	22,		28,		17,		1},
			{313,	588,	14,		28,		2,		19}
		}
	},
	{
	991,		///19
		{
			6,		30,		58,		86,		0,		0,		0
		},
		{
			{795,	196,	113,	28,		3,		4},
			{627,	364,	44,		26,		3,		11},
			{445,	546,	21,		26,		17,		4},
			{341,	650,	13,		26,		9,		16}
		}
	},
	{
	1085,	///20
		{
			6,		34,		62,		90,		0,		0,		0
		},
		{
			{861,	224,	107,	28,		3,		5},
			{669,	416,	41,		26,		3,		13},
			{485,	600,	24,		30,		15,		5},
			{385,	700,	15,		28,		15,		10}
		}
	},
	{
	1156,	///21
		{
			6,		28,		50,		72,		94,		0,		0
		},
		{
			{932,	224,	116,	28,		4,		4},
			{714,	442,	42,		26,		17,		0},
			{512,	644,	22,		28,		17,		6},
			{406,	750,	16,		30,		19,		6}
		}
	},
	{
	1258,	///22
		{
			6,		26,		50,		74,		98,		0,		0
		},
		{
			{1006,	252,	111,	28,		2,		7},
			{782,	476,	46,		28,		17,		0},
			{568,	690,	24,		30,		7,		16},
			{442,	816,	13,		24,		34,		0}
		}
	},
	{
	1364,	///23
		{
			6,		30,		54,		78,		102,		0,		0
		},
		{
			{1094,	270,	121,	30,		4,		5},
			{860,	504,	47,		28,		4,		14},
			{614,	750,	24,		30,		11,		14},
			{464,	900,	15,		30,		16,		14}
		}
	},
	{
	1474,	///24
		{
			6,		28,		54,		80,		106,		0,		0
		},
		{
			{1174,	300,	117,	30,		6,		4},
			{914,	560,	45,		28,		6,		14},
			{664,	810,	24,		30,		11,		16},
			{514,	960,	16,		30,		30,		2}
		}
	},
	{
	1588,	///25
		{
			6,		32,		58,		84,		110,		0,		0
		},
		{
			{1276,	312,	106,	26,		8,		4},
			{1000,	588,	47,		28,		8,		13},
			{718,	870,	24,		30,		7,		22},
			{538,	1050,	15,		30,		22,		13}
		}
	},
	{
	1706,	///26
		{
			6,		30,		58,		86,		114,		0,		0
		},
		{
			{1370,	336,	114,	28,		10,		2},
			{1062,	644,	46,		28,		19,		4},
			{754,	952,	22,		28,		28,		6},
			{596,	1110,	16,		30,		33,		4}
		}
	},
	{
	1828,	///27
		{
			6,		34,		62,		90,		118,		0,		0
		},
		{
			{1468,	360,	122,	30,		8,		4},
			{1128,	700,	45,		28,		22,		3},
			{808,	1020,	23,		30,		8,		26},
			{628,	1200,	15,		30,		12,		28}
		}
	},
	{
	1921,	///28
		{
			6,		26,		50,		74,		98,		122,		0
		},
		{
			{1531,	390,	117,	30,		3,		10},
			{1193,	728,	45,		28,		3,		23},
			{871,	1050,	24,		30,		4,		31},
			{661,	1260,	15,		30,		11,		31}
		}
	},
	{
	2051,	///29
		{
			6,		30,		54,		78,		102,		126,		0
		},
		{
			{1631,	420,	116,	30,		7,		7},
			{1267,	784,	45,		28,		21,		7},
			{911,	1140,	23,		30,		1,		37},
			{701,	1350,	15,		30,		19,		26}
		}
	},
	{
	2185,	///30
		{
			6,		26,		52,		78,		104,		130,		0
		},
		{
			{1735,	450,	115,	30,		5,		10},
			{1373,	812,	47,		28,		19,		10},
			{985,	1200,	24,		30,		15,		25},
			{745,	1440,	15,		30,		23,		25}
		}
	},
	{
	2323,	///31
		{
			6,		30,		56,		82,		108,		134,		0
		},
		{
			{1843,	480,	115,	30,		13,		3},
			{1455,	868,	46,		28,		2,		29},
			{1033,	1290,	24,		30,		42,		1},
			{793,	1530,	15,		30,		23,		28}
		}
	},
	{
	2465,	///32
		{
			6,		34,		60,		86,		112,		138,		0
		},
		{
			{1955,	510,	115,	30,		17,		0},
			{1541,	924,	46,		28,		10,		23},
			{1115,	1350,	24,		30,		10,		35},
			{845,	1620,	15,		30,		19,		35}
		}
	},
	{
	2611,	///33
		{
			6,		30,		58,		86,		114,		142,		0
		},
		{
			{2071,	540,	115,	30,		17,		1},
			{1631,	980,	46,		28,		14,		21},
			{1171,	1440,	24,		30,		29,		19},
			{901,	1710,	15,		30,		11,		46}
		}
	},
	{
	2761,	///34
		{
			6,		34,		62,		90,		118,		146,		0
		},
		{
			{2191,	570,	115,	30,		13,		6},
			{1725,	1036,	46,		28,		14,		23},
			{1231,	1530,	24,		30,		44,		7},
			{961,	1800,	16,		30,		59,		1}
		}
	},
	{
	2876,	///35
		{
			6,		30,		54,		78,		102,		126,		150
		},
		{
			{2306,	570,	121,	30,		12,		7},
			{1812,	1064,	47,		28,		12,		26},
			{1286,	1590,	24,		30,		39,		14},
			{986,	1890,	15,		30,		22,		41}
		}
	},
	{
	3034,	///36
		{
			6,		24,		50,		76,		102,		128,		154
		},
		{
			{2434,	600,	121,	30,		6,		14},
			{1914,	1120,	47,		28,		6,		34},
			{1354,	1680,	24,		30,		46,		10},
			{1054,	1980,	15,		30,		2,		64}
		}
	},
	{
	3196,	///37
		{
			6,		28,		54,		80,		106,		132,		158
		},
		{
			{2566,	630,	122,	30,		17,		4},
			{1992,	1204,	46,		28,		29,		14},
			{1426,	1770,	24,		30,		49,		10},
			{1096,	2100,	15,		30,		24,		46}
		}
	},
	{
	3362,	///38
		{
			6,		32,		58,		84,		110,		136,		162
		},
		{
			{2702,	660,	122,	30,		4,		18},
			{2102,	1260,	46,		28,		13,		32},
			{1502,	1860,	24,		30,		48,		14},
			{1142,	2220,	15,		30,		42,		32}
		}
	},
	{
	3532,	///39
		{
			6,		26,		54,		82,		110,		138,		166
		},
		{
			{2812,	720,	117,	30,		20,		4},
			{2216,	1316,	47,		28,		40,		7},
			{1582,	1950,	24,		30,		43,		22},
			{1222,	2310,	15,		30,		10,		67}
		}
	},
	{
	3706,	///40
		{
			6,		30,		58,		86,		114,		142,		170
		},
		{
			{2956,	750,	118,	30,		19,		6},
			{2334,	1372,	47,		28,		18,		31},
			{1666,	2040,	24,		30,		34,		34},
			{1276,	2430,	15,		30,		20,		61}
		}
	}
};

u8		const	g_poly[maxgen][70]=			 //RQ CODE�ľ�����������Ӧ�ľ�������ʽ
{
{5, 0, 114, 61, 75, 29, 156},
{7, 0, 87, 229, 146, 149, 238, 102, 21},
{10, 0, 251, 67, 46, 61, 118, 70, 64, 94, 32, 45},
{13, 0, 74, 152, 176, 100, 86, 100, 106, 104, 130, 218, 206, 140, 78},
{15, 0, 8, 183, 61, 91, 202, 37, 51, 58, 58, 237, 140, 124, 5, 99, 105},
{16, 0, 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120},
{17, 0, 43, 139, 206, 78, 43, 239, 123, 206, 214, 147, 24, 99, 150, 39, 243, 163, 136},
{18, 0, 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98, 96, 153},
{20, 0, 17, 60, 79, 50, 61, 163, 26, 187, 202, 180, 221, 225, 83, 239, 156, 164, 212, 212, 188, 190},
{22, 0, 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80, 219, 134, 160, 105, 165, 231},
{24, 0, 229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218, 111, 0, 117, 232, 87, 96, 227, 21},
{26, 0, 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21, 245, 142, 13, 102, 48, 227, 153, 145, 218, 70},
{28, 0, 168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201, 21, 43, 245, 87, 42, 195, 212, 119, 242, 37, 9, 123},
{30, 0, 41, 173, 145, 152, 216, 31, 179, 182, 50, 48, 110, 86, 239, 96, 222, 125, 42, 173, 226, 193, 224, 130, 156, 37, 251, 216, 238, 40, 192, 180}, 
{36, 0, 200, 183, 98, 16, 172, 31, 246, 234, 60, 152, 115, 24, 167, 152, 113, 248, 238, 107, 18, 63, 
	218, 37, 87, 210, 105, 177, 120, 74, 121, 196, 117, 251, 113, 233, 30, 120},
{42, 0, 250, 103, 221, 230, 25, 18, 137, 231, 33, 3, 58, 242, 221, 191, 110, 84, 230, 8, 188, 106, 
	96, 147, 15, 131, 139, 34, 101, 223, 39, 101, 213, 199, 237, 254, 201, 123, 171, 162, 194, 117, 50, 96},
{48, 0, 228, 25, 196, 130, 211, 146, 60, 24, 251, 90, 39, 102, 240, 61, 178, 63, 46, 123, 115, 18, 221, 111, 135, 160, 182, 205, 107, 
	206, 95, 150, 120, 184, 91, 21, 247, 156, 140, 238, 191, 11, 94, 227, 84, 50, 163, 39, 34, 108},
{56, 0, 106, 120, 107, 157, 164, 216, 112, 116, 2, 91, 248, 163, 36, 201, 202, 229, 6, 144, 254, 155, 135, 208, 170, 209, 12, 139, 127, 142, 182, 249,
	177, 174, 190, 28, 10, 85, 239, 184, 101, 124, 152, 206, 96, 23, 163, 61, 27, 196, 247, 151, 154, 202, 207, 20, 61, 10},
{62, 0, 65, 202, 113, 98, 71, 223, 248, 118, 214, 94, 51, 122, 37, 23, 2, 228, 58, 121, 7, 105, 135, 78, 243, 118, 70, 76, 223, 89, 72, 50, 70, 
	111, 194, 17, 212, 126, 181, 35, 221, 117, 235, 11, 229, 149, 147, 123, 213, 40, 115, 6, 200, 100, 26, 246, 182, 218, 127, 215, 36, 186, 110, 106},
{68, 0, 247, 159, 223, 33, 224, 93, 77, 70, 90, 160, 32, 254, 43, 150, 84, 101, 190, 205, 133, 52, 60, 202, 165, 220, 203, 151, 93, 84, 15, 84, 253, 173, 160,
	89, 227, 52, 199, 97, 95, 231, 52, 177, 41, 125, 137, 241, 166, 225, 118, 2, 54, 32, 82, 215, 175, 198, 43, 238, 235, 27, 101, 184, 127, 3, 5, 8, 163, 238}
};

#endif

void generate_gf(int codelen);
unsigned char rs_mul(unsigned char poly1, unsigned char poly2);
void gen_poly(int codelen, int datlen);
void encode_rs(unsigned char *cor, int codelen, int datlen);
u32 bch_code(u32 dat, u32 gx, u8 codelen, u8 datlen);
void put_pixel(u8 *ptr, u16 x, u16 y, u16 dots, u8 value);
void put_array(u8 *des, u8 *src, u8 xlen, u8 ylen, u16 maxdot, u16 x0, u16 y0);
void put_pos_find();
void put_pos_align();
u16 press_byte_head(u8 *des, u16 srclen);
u16	press_byte(u8 *des, u8 *src, u16 srclen);
u8	dot_in_pos(u16 x, u16 y);
void	mov_one_pix(pMovePix pPix, u8 maxdot);
void Put_Byte(u8 dat, pMovePix pix);
void put_dat(u8 *ptr);
u8 get_dot(u8 *ptr, u16 bitH, u16 bitV, u16 bitA);
void	mask_dat(u8 *tmap, u8 ref);
void put_fmtinfo(u8 mskno);
void put_verinfo(void);
u32 count_score(u8 *tmap);
u8 do_mask();
void proc_data(u8 *dat, u8 datlen);
u8	QR_Ver(u8 Version, u16 len, u8 corlvl);
void Get_Code(u8 *pChar, u16 CharLen);
void sht_dat(void);
u32 QR_Print(u8 *pBuf, u8 Version, u8 CorID, u16 CharLen);
void put_a_finder(u8 *ptr, u8 vWidth, u8 vHeight, u8 xOrg, u8 yOrg, u8 dots);
void put_finder(u8 *ptr);
u8	CalculateRamBuffer(void);
u8	sel_matrx_module(u8 vHeight, u8 vWidth, u16 charlen);
u8 getdot(int row, int col, u8 id);
void putdot(int row, int col, u8 id, u8 values);
void module(int row, int col, u8 chr, u8 bit);
void utah(int row, int col, u8 chr);
void corner1(u8 chr);
void corner2(u8 chr);
void corner3(u8 chr);
void corner4(u8 chr);
void ecc200(void);
void gen_ploy_dm(u8 codelen, u8 datlen);
void encode_rs_dm(u8 *cor, u8 codelen, u8 datlen);
u16 Press_Dat(u8 *pChar, u16 datlen);
void Build_Code_Word(u8 *pCHar, u16 datlen);
u32 Matrix_Print(u8 *pBuf, u8 vHeight, u8 vWidth, u16 charlen);

     

#endif /* BARCODE_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/