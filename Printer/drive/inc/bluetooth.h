/**
******************************************************************************
  * @file    x.h
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   usart��صĳ���.
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
  * <h2><center>&copy; COPYRIGHT 2016 </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include	"stm32f10x.h"
	 
/* Funtion Declare -----------------------------------------------------------*/
void Init_BT (void);
void InitBluetoothPort(void);
void InitBTUsart(void);
void Read_NByte(uint32_t Length, uint8_t *DataBuf);
uint8_t Judge_BT_Link(void);
void SetBluetooth(u8 FCommand,u8 SCommand);
uint8_t OpenSetupBluetooth(void);
uint8_t CloseSetupBluetooth(void);
/* Macro Definition ----------------------------------------------------------*/
/******* ����ģ������ *************/
//1,JOBT4 4.0ģ�飻2,GCJO 3.0ģ�飻3,DMBT,I482ES˫ģģ�飻4,DLBT,һ�϶�BTģ��
enum {JOBT4=1,GCJO=2,DMBT=3,DLBT=4};
 
#define BT_STR_LEN_NAME       30        //����ģ�������ַ�������󳤶� 2017.03.21  23��Ϊ30
#define BT_STR_LEN_ADDR       20        //����ģ���ַ�ַ�������󳤶�
#define BT_STR_LEN_PWD        20        //����ģ�������ַ�������󳤶�
#define BT_STR_LEN_VERSION    50        //����ģ��汾�ַ�������󳤶� 2017.03.21  20��Ϊ30
#define BT_STR_LEN_CLASS      10        //�����豸������󳤶�

//�����������ô���
#define BT_UART         USART2               //��������Ϊ����3
#define BT_RCC_APB1Periph_USART	RCC_APB1Periph_USART2			//ʱ��
#define BT_UART_IRQn    USART2_IRQn					//�ж�Դ	
//��������IO�˿�����
#define BT_UART_PORT    GPIOD               //��������Ϊ����3
#define BT_RCC_APB2Periph_GPIO	RCC_APB2Periph_GPIOD
#define BT_TYPE_PORT           GPIOD              //��������Ϊ����3
#define	BT_WIFI_TYPE		GPIO_Pin_15     //��������wifiʶ��  T801M
#define	BT_SET		      GPIO_Pin_1     //��������ģʽ����
#define	BT_UART_TX		  GPIO_Pin_5    //���ڷ���
#define	BT_UART_RX		  GPIO_Pin_6    //���ڽ���
#define	BT_LINK		      GPIO_Pin_4	   //��������״̬	 
#define	BT_UART_CTS		  GPIO_Pin_2    //���ڽ���æ
#define	BT_UART_RTS		  GPIO_Pin_3    //���ڷ���æ
#define	BT_RST		      GPIO_Pin_7	   //����ģ�鸴λ����


/****** �����������õĿ���****/
#define	BT_SET_LOW		GPIO_ResetBits(BT_UART_PORT,BT_SET)
#define	BT_SET_HIGH		GPIO_SetBits(BT_UART_PORT,BT_SET)
/****** ����������λ�Ŀ���****/
#define	BT_RST_LOW		GPIO_ResetBits(BT_UART_PORT,BT_RST)
#define	BT_RST_HIGH		GPIO_SetBits(BT_UART_PORT,BT_RST)
/****** ��������RTS��æ���Ŀ���****/
#define	BT_BUSY_CLR		GPIO_ResetBits(BT_UART_PORT, BT_UART_RTS)			//������ģ����æ
#define	BT_BUSY_SET		GPIO_SetBits(BT_UART_PORT, BT_UART_RTS)		//������ģ����æ

/****** ������������״̬****/
#define READ_BT_LINK		GPIO_ReadInputDataBit(BT_UART_PORT,BT_LINK)
#define READ_BT_CTS			GPIO_ReadInputDataBit(BT_UART_PORT,BT_UART_CTS)
#define READ_BT_WIFI		GPIO_ReadInputDataBit(BT_UART_PORT,BT_WIFI_TYPE)


/******* ��������ģ�����Ϣ�ṹ *************/
typedef struct
{
    u8 Type;                        //����ģ�����ͣ�0 BTM162����1 ���BCO4	  =2 BM57(SPP03)  =3 BM57(SPP05)    
    u8 InitFlag;                    //������ʼ����ɱ�־
    u8 reInitFlag;                  //����ģ�����³�ʼ����־
    u8 Bind;                        //����ģ���״̬
    u8 Auth;			                  //����ģ������ʹ��
    u8 Name[BT_STR_LEN_NAME];	      //����ģ������
    u8 Laddr[BT_STR_LEN_ADDR];	    //����ģ���ַ
    u8 Password[BT_STR_LEN_PWD];	  //����ģ������
    u8 Version[BT_STR_LEN_VERSION]; //����ģ��汾    
    u8 Class[BT_STR_LEN_CLASS];     //�����豸���� 
}Bluetooth_Struct; 


#ifdef __cplusplus
}
#endif

#endif /* USART_H������� */

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
