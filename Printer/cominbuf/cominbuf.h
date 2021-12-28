/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-5-14
  * @brief   ͨ�Ž��ջ�������صĳ���.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMINBUF_H
#define COMINBUF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "debug.h"

/* Macro Definition ----------------------------------------------------------*/
#ifdef	EXT_SRAM_ENABLE
	#define MAX_BYTES_OF_IN_BUF				(64*1024)			//���ͨ�Ž��ջ�������С
	#define	MAX_RSV_BYTES_OF_IN_BUF		(256)					//��������󻺳���
	#define	MIN_RSV_BYTES_OF_IN_BUF		(128)					//��������С������
#else
	#define MAX_BYTES_OF_IN_BUF				(6*1024)			//���ͨ�Ž��ջ�������С 2016.07.29  8K��Ϊ6K
	#define	MAX_RSV_BYTES_OF_IN_BUF		(1500+256)		//��������󻺳��� 2016.06.08 1500+256
	#define	MIN_RSV_BYTES_OF_IN_BUF		(1500)			  //��������С������   8*1024-512 2016.07.07 1500
#endif

#define	COM_IN_BUF_NOT_FULL					(0)						//������δ��
#define	COM_IN_BUF_FULL							(1)						//����������

/******* �������뻺�������� *************/
typedef struct 
{   
  volatile uint32_t	BytesSize;		    	//���뻺�����ֽ��� 2016
	volatile uint32_t	BytesNumber;			//���뻺�����ֽ���
	volatile uint32_t	PutPosition;			//���뻺��������λ��
	volatile uint32_t	GetPosition;			//���뻺����ȡ��λ��
	volatile uint32_t	PE_BytesNumber;		//��ǰ�����������ֽ���
	volatile uint32_t	PE_GetPosition;		//��ǰ����ȡ��ָ��
	volatile uint8_t	BufFullFlag;			//����������־
					 uint8_t * Buf;							//ͨ�Ž��ջ���
}TypeDef_StructInBuffer;


extern void RealTimeCommand(void);
/* Funtion Declare -----------------------------------------------------------*/
void	SetComInBufBusy(void);
void	ClearComInBufBusy(void);
void	EnableReceiveInterrupt(void);
void	DisableReceiveInterrupt(void);
void	ReceiveInterrupt(FunctionalState NewState);
void	InitInBuf(void);
uint32_t	GetInBufBytesNumber(void);
void	SetActiveComInBusy(void);
void	ClearActiveComInBusy(void);
void	EnableActiveReceiveInterrupt(void);
void	DisableActiveReceiveInterrupt(void);


#ifdef	NO_PAPER_RE_PRINT_ENABLE
void	ClearInBuf(void);
void	PaperOutDetect(void);
#endif

void	PutCharToInBuffer(uint8_t Data);		//�����ջ���������
uint8_t		GetCharFromInBuffer(void);		//�ӽ��ջ���������
void	DecInBufferGetPosition(void);	//��д��������ֻ���Ķ���λ��ֵ
uint8_t		ReadInBuffer(void);						//�ӽ��ջ���������
void	DecInBufferGetPoint(void);				//��д��������ֻ���Ķ���λ��ֵ
static void FillRealTimeCommand(uint8_t Data);

#ifdef __cplusplus
}
#endif

#endif /* COMINBUF_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
