/**
  ******************************************************************************
  * @file    Barcode2D.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   ��ά������صĳ���.
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
//#define EXTSPIFLASH

#include "barcodepdf417.h"
#include "extgvar.h"		//ȫ���ⲿ��������

#define MAX_CHAR_LEN 1024			//����PDF417������ַ���

extern TypeDef_StructPrtCtrl			g_tPrtCtrlData;		//��ӡ����
/*******************************************************************************
* Function Name	: u16 *encode_text ( u16 *dst , u8 *src , int len )
* Description	: ׼����ʼ�µ�һ�в���
* Input			: 
* Output		: 
* Return		: 
*******************************************************************************/
u16 *encode_text ( u16 *dst , u8 *src , int len )
{
	char m_o,m_n;
	u8 ch;
	int total;
	u16 dat;

	if(pdf_mode != TEXT_MODE)
	{
		pdf_mode = TEXT_MODE;
		*dst++ = TEXT_MODE;
	}
	
	m_o=UPPER;
	total=0;
	while(len)
	{
		ch=*src++;
		//ת�����ݲ���ȡ����
		text_get(m_n,ch)

		if(m_o&m_n)//����������ģʽ�У������л���
			text_add(ch)
		else
		{	//��Ҫģʽ�л�
			//Ϊ�˸��õ�ѹ�����ٳ���ת�⸮������Ҫ�ο�����2-3���������ڵ���ģʽ
			//Ϊ�˼��ݲ�������ô��
			switch(m_o)
			{
				case UPPER:
					if(m_n&LOWER)
					{	//ת��Сд
						text_add(LL);
						m_o=LOWER;
					}
					else if(m_n&MIXED)
					{	//ת�ɻ��
						text_add(ML);
						m_o=MIXED;
					}
					else
					{	//��㲻��ֱ��ת��
						//text_add(PS);
						text_add(ML);
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case LOWER:
					if(m_n&UPPER)
					{	//����ֱ��ת��
						text_add(ML);
						text_add(AL);
						m_o=UPPER;
					}
					else if(m_n&MIXED)
					{	//ת�ɻ��
						text_add(ML);
						m_o=MIXED;
					}
					else
					{	//��㲻��ֱ��ת��
						text_add(ML);
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case MIXED:
					if(m_n&UPPER)
					{
						text_add(AL);
						m_o=UPPER;
					}
					else if(m_n&LOWER)
					{
						text_add(LL);
						m_o=LOWER;
					}
					else
					{	//���
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case PUNCT:
					if(m_n&UPPER)
					{	//���
						text_add(PAL);
						m_o=UPPER;
					}
					if(m_n&LOWER)
					{	//ת��Сд
						text_add(PAL);
						text_add(LL);
						m_o=LOWER;
					}
					if(m_n&MIXED)
					{	//ת�ɻ��
						text_add(PAL);
						text_add(ML);
						m_o=MIXED;
					}
					text_add(ch);
					break;
			}
		}
		len--;
		if(!len)
		{
			text_end();
			break;
		}
    }
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *ch10to900 ( u16 *dst , u8 *src , int len )
* Description	: ����ģʽ
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
u16 *ch10to900 ( u16 *dst , u8 *src , int len )
{
    u16* ret = dst;
    int last = len / 3;
    int ni, k;

    memset(ret, 0, (last + 1) * sizeof(u16));
    ret[last] = 1;
    for (ni = 0; ni < len; ++ni) {
        // multiply by 10
        for (k = last; k >= 0; --k)
            ret[k] *= 10;
        // add the digit
        ret[last] += src[ni] - '0';
        // propagate carry
        for (k = last; k > 0; --k) {
            ret[k - 1] += ret[k] / 900;
            ret[k] %= 900;
        }
    }
	return dst+last+1;
}
/*******************************************************************************
* Function Name	: u16 *encode_num ( u16 *dst , u8 *src , int len )
* Description	: ��Դ��ַ�����ַ�ѹ�������Ŀ�ĵ�ַ����
* Input			: dst��Ŀ�ĵ�ַ��src��Դ��ַ��len�����ݳ���
* Output		: �ı�Ŀ�ĵ�ַ����
* Return		: ����Ŀ�ĵ�ַ
*******************************************************************************/
u16 *encode_num ( u16 *dst , u8 *src , int len )
{
	pdf_mode=NUME_MODE;
	*dst++=NUME_MODE;
	
	while(len)
	{
		if(len <=44)
		{
	        dst = ch10to900(dst, src, len);
			return dst;
		}
		else
			dst = ch10to900(dst, src, 44);
		src +=44;
		len -=44;
    }
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *ch256to900 ( u16 *dst , u8 *src )
* Description	: �ֽ�ģʽ
* Input			: 
* Output		: None
* Return		: 
//����6�ֽ�ת5�ֽ�,�̶�
*******************************************************************************/
u16 *ch256to900 ( u16 *dst , u8 *src )
{
    u32 ret[5];
    int ni, k;

    memset(ret, 0, sizeof(ret));
    for (ni = 0; ni < 6; ++ni) {
        // multiply by 256
        for (k = 4; k >= 0; --k)
            ret[k] *= 256;
        // add the digit
        ret[4] += src[ni] ;
        // propagate carry
        for (k = 4; k > 0; --k) {
            ret[k - 1] += ret[k] / 900;
            ret[k] %= 900;
        }
    }
	for(ni=0;ni<5;ni++)
		*dst++=ret[ni];
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *encode_byte(u16 *dst,u8 *src,int len)
* Description	: 
* Input			: 
* Output		: None
* Return		: 
*******************************************************************************/
u16 *encode_byte(u16 *dst,u8 *src,int len)
{
	if(len%6)
	{
		pdf_mode=BYTE_MODE;
		*dst++=BYTE_MODE;
		while(len>=6)
		{
	        dst=ch256to900(dst, src);
			src+=6;
			len-=6;
		}
		while(len--)
			*dst++=*src++;
	}
	else
	{
		pdf_mode=BYTE_MODE_6;
		*dst++=BYTE_MODE_6;
		while(len>=6)
		{
	        dst=ch256to900(dst, src);
			len-=6;
			src+=6;
		}
	}
	return dst;
}
/*******************************************************************************
* Function Name	: void encode ( PDF417_EN *pdf , pdf417 *p )
* Description	: 
* Input			: 
* Output		: �ı�pdf�ṹ����
* Return		: None
*******************************************************************************/
void encode ( PDF417_EN *pdf , pdf417 *p )
{
	u8 *src;
	u16 *dst;
	u8 *tail;
	u8 *num_p;		//����13�����������ֵ�ͷָ��
	u8 *txt_p;		//����5���������ı���ͷָ��
	int num_n;
	int txt_n;
	int i;
	u8 ch;

	pdf_mode = TEXT_MODE;

	//�����������
	
	src = p->in;
	dst = pdf->buf +1;
	tail = src+ p->len;		
	num_p = txt_p = tail;	//����ͷָ��

	while(src < tail)
	{
		u8 *p;

		//����ѹ���̶Ƚ����������ݿ���
		//����Ѱ��������13�����ݵ�ͷ��λ��(�ڵ�ǰδ����λ�õ�ȫ�Ľ�������Ѱ)
		p =src;		//�ӵ�ǰ��ʼ����
		num_n =0;
		while( p < tail)
		{
			ch = *p;
			if(ch>='0'&&ch<='9')
				num_n++;	//��¼��������
			else
			{	//�������13����,����13��ͬʱnֵ��¼��Ч���ݳ���
				if(num_n>=13)
					break;
				num_n=0;
			}
			p++;
		}
		if(num_n >=13)
			num_p = p -num_n;	//������ʼλ��
		else
			num_p = tail;	//����ͷָ��

		//�м��������
		while(src < num_p)
		{
			//����Ѱ��������5�ֽ��ı�(�ڵ�ǰδ����λ�ú�ǰ����Ѱ�����ͷ��֮����Ѱ)
			p =src;
			txt_n =0;
			while(p <num_p)
			{
				ch=*p;
    	        if((ch>=' '&&ch<127)||ch=='\r'||ch=='\n'||ch=='\t')
					txt_n++;
				else
				{
					if(txt_n>=5)	//�������5����,����5��ͬʱnֵ��¼��Ч���ݳ���
						break;
					txt_n=0;
				}
				p++;
			}
			if(txt_n >=5)
				txt_p = p-txt_n;	//������ʼλ��
			else
				txt_p =num_p;

			//���ʣ�����ݰ��ֽ�ģʽ����(�ڵ�ǰδ����λ�ú�ǰ����Ѱ�����ͷ��֮����Ѱ)
			i= txt_p - src;
			if(i)
			{
				dst = encode_byte(dst, src, i);
				src +=i;
			}

			if(txt_n>=5)
			{
				dst =encode_text(dst, txt_p, txt_n);
				src = txt_p + txt_n;
			}
		}

		if(num_n >=13)
		{
			dst = encode_num(dst, num_p, num_n);
			src = num_p+num_n;
		}
	}
	pdf->len = dst- pdf->buf;		//ѹ��������
}
//(17*(wide+4)):(high*3)=17:9	w=sqrt((num+12)/3)-2;
//					  	1,	2, 3, 4, 5, 6,   7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18
//const u16 WIDE_TBL[30]={15,36,63,96,135,180,231,288,351,420,495,576,663,756,855,960,1071,0};
/*******************************************************************************
* Function Name	: struct_make ( PDF417_EN *pdf , pdf417 *p )
* Description	: �����Ű�
* Input			: 
* Output		: None
* Return		: ��������0�����󷵻�1
*******************************************************************************/
u8 struct_make ( PDF417_EN *pdf , pdf417 *p )
{
	int len;	//����
	int max;	//�������ʾ����
	int i, j;

	//������������ݳ�ʼ��
#ifdef EXTFLASH

	pdf->err = 0;
	if(p->level >8)			 //����ȼ�����
		p->level =0;
#else
	u16 *ERROR_LEVEL_ADDR[9] = {(u16 *)ERROR_LEVEL0, (u16 *)ERROR_LEVEL1, (u16 *)ERROR_LEVEL2, (u16 *)ERROR_LEVEL3,
							  	(u16 *)ERROR_LEVEL4, (u16 *)ERROR_LEVEL5, (u16 *)ERROR_LEVEL6, (u16 *)ERROR_LEVEL7,
							  	(u16 *)ERROR_LEVEL8};
	if(p->level >8)
		p->level =0;

	pdf->err = ERROR_LEVEL_ADDR[p->level];	//�������ʽ��ַ
#endif
	pdf->err_l = p->level;			//����ȼ�
	pdf->err_n = 2 << p->level;		//�����ֳ���


	//ȷ�Ͼ��ο��
	//1��ȷ�ϴ�ӡ��Ŀǰ������������
	max = p->max / PDF417_MODE_WIDE;	//ÿ���ֿ�ռ��17������Ϊ18��
	if(!(p-> max%PDF417_MODE_WIDE))
	{	//�����������ڽ���Ϊ18����Ҫ��ȥ
		if(max)
			max--;
	}
	
	if(max <5 )
	{						//���̫С�޷����
//		pdf->wide =0;
		return 1;
	}
	
	max -=4;//��ȥ��ʼ����ָ����ָ�������������Ч���ݿ�
	
	len = pdf->len + pdf->err_n;		//�ܵ���������ݳ���

	pdf->wide =max;						//����ֽ���

	//����߶�,������
	pdf->high = len/max;
	if(len % max )
		pdf->high++;
	
	if(pdf->high >PDF417_HIGH_MAX)
	{									//�߶�̫���޷����
//		pdf->high =0;
		return 1;
	}

/**** 2012.9.6 ���뻺���������� ****************/	
	//������Ҫ����������,����������򷵻ش���
	i =(((pdf->wide +4)*PDF417_MODE_WIDE+1)+7)/8;	//�����ֽ���
	len = (MAX_CHAR_LEN) + ( pdf->len + pdf->err_n )*(sizeof(u16)) + i* pdf->high;  
	if(len > BARCODE2D_SIZE)
		return 1;
/**** 2012.9.6 ****************/	

	len= max * pdf->high;			//�ڴ˿���¿����������ݳ���
	i = len - pdf->err_n;			//������Ч���ݳ���
	*pdf->buf =i;					//�������ݳ���
	len =i - pdf->len;				//��Ҫ�������ݳ���
	j = pdf->len;					//
	pdf->len =i;					//�������ݳ���

		
	while(len--)
		pdf->buf[j++] = TEXT_MODE;	//�������
	return 0;
}
/*******************************************************************************
* Function Name	: correct_append ( PDF417_EN *pdf )
* Description	: ���ɾ������ݲ�ֱ����ӵ�����β
* Input			: �������ݽṹ
* Output		: None
* Return		: None
*******************************************************************************/
void correct_append ( PDF417_EN *pdf )
{
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;
	int err_n;
	u16 *p;
	u16 *buf;
	int len;
	int i, e;
	u16 *err;				//err���뱣�������ڶ�ά�����������ڲ�FLASH��

	#ifdef EXTFLASH
		u32	Flashaddr;
		u8	Data[2];
		u16 ERROR_LEVEL_ADDR[9]={0,4,12,28,60,124,252,508,1020};	//�������ʽ�׵�ַ���������ⲿ��
	#endif
	
	//ȷ�϶���ʽ
	buf = pdf->buf;							//�����ַ
	len = pdf->len;							//���ݳ���
	p = buf + len;							//ָ������λ��
	err = pdf->err;							//�������ʽ��ַ
	err_n = pdf->err_n;						//�����ֳ���	
	memset(p, 0, err_n * (sizeof(u16)+1));	//��ʼ����������
	
	for(i=0; i<len; i++)
	{
		t1 = buf[i] + p[0];
		for(e =0; e<err_n; e++)
		{
			#ifdef EXTFLASH
			 	Flashaddr =PDF417_ERROR_LEVEL_ADDR + (ERROR_LEVEL_ADDR[	pdf->err_l] + 2*(err_n - e-1));
				SPI_FLASH_BufferRead(Data, Flashaddr , 2 );
				t2=(t1 * (*(u16*)Data)) %MOD;
			#else
				t2=(t1 * err[err_n - e -1]) %MOD;
			#endif

			t3=MOD - t2;
			p[e] = (p[e+1] + t3) %MOD;
		}
	}
	for (i = 0; i < err_n; i++)
		p[i] = (MOD - p[i]) % MOD;
}

int bits;

void bits_begin ( void )
{
	bits=0;
}
/*******************************************************************************
* Function Name	: void bits_add ( u8 *buf , u16 dat )
* Description	: 
* Input			: 
* Output		: 
* Return		: None
*******************************************************************************/
void bits_add ( u8 *buf , u16 dat )
{
	int i;
	int bit;
	u8 *p;
	
	p=&buf[bits>>3];
	bit=bits&0x07;
	i=dat|0x10000;

	//17λ����ʼ��Ҫ��䵽3���ֽ���
	*p|=i>>(17-8+bit);
	p++;
	*p++=i>>(17-16+bit);
	*p=i<<(7-bit);
	bits+=17;
}

/*******************************************************************************
* Function Name	: int bits_end ( u8 *buf )
* Description	: 
* Input			: 
* Output		: 
* Return		: 
*******************************************************************************/
int bits_end ( u8 *buf )
{
	int bit;
	u8 *p;
	
	p = &buf[bits >> 3];
	bit = bits&0x07;
	*p |= 1<<(7-bit);
	bits++;

	return bits;
}
/*******************************************************************************
* Function Name	: bitmap_make ( pdf417 *p , PDF417_EN *pdf )
* Description	: ������ת����λͼ
* Input			: 
* Output		: 
* Return		: None
*******************************************************************************/
void bitmap_make ( pdf417 *p , PDF417_EN *pdf )
{
	int w,h;
	u16 ch;
	u8 *dst;
	u16 *src;
	
#ifdef EXTFLASH
	u32	ClusterAddress, FlashAddress;
	u8  Data[2];
#else
	u16 *tbl;		
#endif

	src = pdf->buf;
	dst = p->out;
	
	for(h=0; h < pdf->high; h++)
	{
		bits_begin();

		//add head
		bits_add(dst, PDF417_START_FLAG);
#ifdef EXTFLASH
		ClusterAddress = PDF417_CLUSTERS_ADDR + 2*929*(h %3);	//��ȡ����׵�ַ
#else
		{
			tbl=(u16 *)CLUSTERS[ h % 3];	//��ȡ���
		}
#endif

		//add L
		switch(h %3)
		{
			case 0: 
				ch= (pdf->high- 1)/3;
				break;
			case 1:
				ch= pdf->err_l *3+((pdf->high -1) %3);
				break;
			default:
				ch= pdf->wide -1;
			break;
		}

		ch= 30*(h/3) + ch;

#ifdef EXTFLASH
		FlashAddress = ClusterAddress + 2*ch;					//��ȡ���(���Ϊ2�ֽ�)
		SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
		bits_add(dst,*(u16*)Data);
#else
		bits_add(dst, tbl[ch]);
#endif
		//add data
		for(w=0; w < pdf->wide; w++)
		{
			#ifdef EXTFLASH
					FlashAddress = ClusterAddress + 2*(*src++);	//��ȡ���
					SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
					bits_add(dst, *(u16*)Data);
			#else
					bits_add(dst, tbl[*src++]);
			#endif
		}
		//add R
		switch(h%3)
		{
			case 0:
				ch =pdf->wide -1;
				break;
			case 1:
				ch =(pdf->high -1)/3;
				break;
			default:
				ch =pdf->err_l*3 + ((pdf->high -1)%3);
				break;
		}
		ch =30*(h/3)+ch;

#ifdef EXTFLASH
		FlashAddress = ClusterAddress + 2*ch;	//��ȡ���
		SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
		bits_add(dst,*(u16*)Data);
#else
		bits_add(dst,tbl[ch]);
#endif
		//add tail
		bits_add(dst,PDF417_STOP_FLAG);
		ch = bits_end(dst);
		dst +=((ch-1)>>3)+1;
	}
	p->wide = ch;		//������ȣ����ռ�ÿռ����ֽڶ���
	p->high = pdf->high;
}
/*******************************************************************************
* Function Name	: pdf417_encode ( pdf417 *p )
* Description	: ��ɱ��롢λͼ����
* Input			: p������ṹ
* Output		: Ӱ��ṹp������
* Return		: ��������0�����󷵻�1
*******************************************************************************/
u8 pdf417_encode ( pdf417 *p )
{
	encode(&pdf_en, p);			//����
	
	if(struct_make(&pdf_en, p))	//�Ű�
		return 1;				//�д����򷵻� 2012.9.6
	
	correct_append(&pdf_en);	//��Ӿ�����
	p->out = (u8*)(pdf_en.buf + pdf_en.len + pdf_en.err_n);		//λͼ���λ�ã��ֽ������λ��ʾͼ�ε�����ߣ�
	bitmap_make(p, &pdf_en);	//����λͼ
	
	return 0;
}
/*******************************************************************************
* Function Name	: u32 Pdf417_Print(u8 *pBuf, u8 Width, u8 CorID, u16 CharLen)
* Description	: ����PDF417����
* Input			: pBuf�����ݻ�������Width��ÿ���ַ�����CorID������ȼ���CharLen�����ݳ���
* Output		: None
* Return		: ��Ч�ַ�������16λΪ�����������16λΪ�����ֽ�����0x0000����ʾ����
*******************************************************************************/
u32 Pdf417_Print(u8 *pBuf, u8 Width, u8 CorID, u16 CharLen)
{
	pdf417 pdf;
	u8	*dst,  row, col,  value, *pT0, *pT1, i, j;
	u16	height, width, bitsw;						//2012.7.12,ԭbitswΪu8
	u32 res;
	
	if(CharLen > MAX_CHAR_LEN)		//����ַ����ݳ��������ش���
		return 0x0000;
	
	dst = pBuf + MAX_CHAR_LEN;		//

	memset(dst, 0x00, MAX_CHAR_LEN);
	
	pdf.in  = pBuf;					//�����ַ
	pdf.len = CharLen;				//���ݳ���
	pdf.wide = Width;				//ÿ���ַ���
	if(pdf.wide > PDF417_WIDE_MAX) 	//����һ���ַ���
		pdf.wide = PDF417_WIDE_MAX;

	pdf.max= (pdf.wide + 4) * PDF417_MODE_WIDE+1;	//���������(������Ϊ18���)�����������������ҽ�����

	if(pdf.max > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex )/2)	//����̶�Ϊ2��	,��������ַ��������˴�ӡ���п��Զ�����Ϊ����ַ���
		pdf.max= (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex )/2;	//2012.6.27 �����������Ϊʵ���趨���
	
	pdf.level=CorID;				//����ȼ�

	pdf_en.buf = (u16*)dst;
	
	if(pdf417_encode(&pdf))			//��ͼ��ת���� 
		return 0x0000;				//�д����򷵻� 2012.9.6

	dst = (u8*)(pdf_en.buf + pdf_en.len + pdf_en.err_n);	//λͼ���λ�ã��ֽ������λ��ʾͼ�ε�����ߣ�

	width = pdf.wide;
	height = pdf.high/8;
	if ((pdf.high % 8) != 0)
		height++;

	res = (width <<16 ) + height;

	width = width / 8;
	if ((pdf.wide % 8) != 0)
		width ++;

	//Ϊ�˱�֤����Ϊ���ֽ������ת����ȷ
	memset(dst + width * pdf.high, 0x00, width*8);	//2012.12.10 �����ַ����
	
	//��������ת��Ϊ�������У����Ϊ���λ
	pT0 = pBuf;
	for (row = 0; row < height; row++)
	{
		bitsw = 0;
	
		pT1 = dst + row * width * 8;
		for (col = 0; col < width; col++)
		{
			for (i = 0; i < 8; i++)
			{
				bitsw += 1;
				value = 0;
				
				for (j = 0; j < 8; j++)
					value |= ((*(pT1 + width * j)<< i) & 0x80)>>j;
				*pT0++ =value;
				
				if (bitsw >= pdf.wide)
					break;
			}
			if (bitsw >= pdf.wide)
				break;
			pT1++;
		}
	}
	return	 res;
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
