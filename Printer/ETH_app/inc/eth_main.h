#ifndef	__MAIN_H
#define	__MAIN_H

extern void my_appcall(void);
extern void my_udp_appcall(void);

struct Port_state{
  u16_t datalen;			  //�������ݳ���
  u16_t olddatalen;			//���ڷ������ݳ���,�ط�ʱ��
  u16_t count;				  //���Ӽ�����
  u8_t state;				    //�ȴ�״̬
  u8_t *dataptr;			  //�������ݵ�ַ
  
};

#endif

