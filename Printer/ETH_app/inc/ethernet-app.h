#ifndef __PRINT_H
#define __PRINT_H
 
//���崮�ڽ��ջ������ṹ
#define UART_BUF_MAX 2048			//���������ֵ
typedef struct
{
	int in ; 						//�����ַ
	int out ;						//������ַ
	u8_t 	buf[ UART_BUF_MAX ] ;		//���ջ�����
}UARTBUF;

//�����ӡ����������ṹ
#define PRINT_BUF_MAX	10*1024		//�����������С
typedef struct TPRINT {
	int in ; 						//�����ַ
	int out ;						//������ַ
	int n ;							//��������ݸ���
	char buf [PRINT_BUF_MAX ] ;		//������
}PRINT;

//����SPRTЭ��ͷ�ṹ
typedef struct {
//	u8_t		bodylenH;				//��������
//	u8_t		bodylenL;
	u8_t		CONSTIDH;				//��ͷ
	u8_t		CONSTIDL;				//
	u8_t		packetidH;				//�����
	u8_t		packetidL;
	u8_t		packetlenH;				//������
	u8_t		packetlenL;
	u8_t		chksumH;			  	//��У���
	u8_t		chksumL;
	u8_t		status;
	u8_t		nextpktidH;			   //�´ΰ����
	u8_t		nextpktidL;
	u8_t		dat;
}USER_HEADER;

void print_char(char ch);
char print_get ( char *ch );
void print_normal(void);
void print_init ( void );
char print_puts ( u8_t *buf );
u8_t print_putn( u8_t *buf, u16_t len);
void print_test( char flag );
void printer_server ( void );
void get_dat(void);
u8_t	chk_bsy(void);
u8_t	chk_err(void);

#endif

