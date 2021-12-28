/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2017-3-18
  * @brief   wifi��س���
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
#include	"wifi.h"
#include	"extgvar.h"
#include	"debug.h"

#include "stdio.h"
#include "stm32f10x.h"
#include "string.h"
#include "led.h"

u8 Wifi_Name[23] = {0};				  //STA/AP����
u8 Route_Name[23] = {0} ;
u8 Wifi_Mode[10]={0};
u8 Wifi_Version[20]={0};		    //ģ��汾
u8 Wifi_Port[20]={0};				    //Э��˿�
u8 Wifi_Protocol[12]={0};			  //����Э������
u8 Wifi_Cs[12]={0};					    //��������
// u8 Wifi_Server_IP[20]={0};				
u8 Wifi_DHCP[6]={0};   //STAģʽ�� DHCP�Ƿ�ʹ��

u8 Wifi_Ipaddr[20]={0};				  //AP/STAģʽ��IP��ַ
u8 Wifi_Mask[20]={0};				    //AP/STAģʽ����������
u8 Wifi_Gateway[20]={0};					//AP/STAģʽ�����ص�ַ
u8 Wifi_Auth[10]={0};				    //AP��֤ģʽ
// u8 Wifi_Encry[10]={0};				  //�����㷨
u8 Wifi_Password[40]={0};			  //����

u8 wifi_Mode_type = 0 ;
u16 wscan_len = 0 ;

/* Private function prototypes -----------------------------------------------*/
extern void DelayUs(uint32_t DelayNumber);
extern void UpLoadData(u8 *DataBuf, u32 Length);				//�ϴ�����
extern void PrintString( u8 *Str );


/* Private functions ---------------------------------------------------------*/
uint8_t WIFI_ReadCommand1111(uint8_t *CommandString,uint8_t *Buffer,uint8_t number);
uint8_t WIFI_ReadCommand(uint8_t *CommandString,uint8_t *Buffer);
uint8_t WIFI_Config_TranS(void);
uint8_t WIFI_TranS_Config(void);
void Wifi_SelfTest(void);
void Init_WiFi(void);


extern u8 set_flag ; 
extern TypeDef_StructInBuffer volatile g_tInBuf;  //2017.10.25

/* ---------------------------------------------------------*/


//��n�γ����ַ�C  �ĵ�ַ
char *mystrnchr( char*str,char c,int n)
{
	int i=0;
	while(*str)
	{
		if(*str== c)
		{i++;
			if(i==n)
			{
				return str;
			}
		}
		str++;
	}
	return 0;
}


uint32_t WiFiSerialKeyTimeout(u8 *key)
{
	static u16 Timeout = 0 ;
	while(1)
	{      
		if ( USART_GetFlagStatus(WIFI_COM, USART_FLAG_RXNE) != RESET)
		{
			*key = (uint8_t)WIFI_COM->DR;
			Timeout = 0 ;
			return 1;
		}
		else
		{
			Timeout++ ;
 			DelayUs(1) ; 
		}
		if(Timeout > 25000)
		{
			*key = 0 ;
			Timeout = 0 ;
			return 0 ;
		}
	}
}
uint32_t Read_A(u8 *key)
{
	static u16 Timeout = 0 ;
	while(1)
	{
		if ( USART_GetFlagStatus(WIFI_COM, USART_FLAG_RXNE) != RESET)
		{
			*key = (uint8_t)WIFI_COM->DR;
			Timeout = 0 ;
			return 1;
		}
		else
		{
			Timeout++ ;
			DelayUs(1000) ;     
		}
		if(Timeout > 1000)  
		{
			*key = 0 ;
			Timeout = 0 ;
			return 0 ;
		}
	}
}
/*******************************************************************************
* Function Name  : void USART_SendSrt(USART_TypeDef* USARTx, *u8 Str)
* Description    : ��ָ���Ĵ��ڷ���һ�ַ���
* Input          : USARTX�����ںţ�Srt:�ַ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
void WiFi_USART_SendStr(USART_TypeDef* USARTx, u8 *Str)
{
// //  	USART_ITConfig(USARTx, USART_IT_TC, DISABLE);
	while(1)
	{
		if( *Str)
		{				
			USART_SendData(USARTx, *Str);
			Str++;
		}
		else
			break;
	}
}
/*******************************************************************************
* Function Name  : uint32_t WIFI_SetCommand
* Description    : ��WIFIģ�鷢������ָ��
* Input          : CommandString��ָ���ַ���
* Output         : None
* Return         : =1 error ,=0 ok 
*******************************************************************************/
uint8_t WIFI_SetCommand(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t i,j=0,Status_flag = 0 ,Status = 0 ,ch;
   uint8_t DataBuf[100];
	WiFi_USART_SendStr(WIFI_COM,CommandString);
	WiFi_USART_SendStr(WIFI_COM,Buffer);
  USART_SendData(WIFI_COM,0x0D);
// 	USART_SendData(WIFI_COM,0x0A);
	
  for(i = 0 ; i < 100 ; i++)
	{
		WiFiSerialKeyTimeout(&ch);						//��ȡָ��ȷ��
		DataBuf[i] = ch ;
		if( DataBuf[i] == 0x0A)
		{
			j++ ; 
		}
		if(j == 1)
		{
			DataBuf[i + 1] = 0 ;
			break ;
		}
		if(DataBuf[2] == 0x4B &&DataBuf[1] ==0x4F &&DataBuf[0] ==0x2B)
		{
			Status_flag = 1 ;
		}
	}
	if(Status_flag)
		Status = 0;
	else
		Status = 1;
	return Status;
}
/*******************************************************************************
* Function Name  : uint32_t WIFI_ReadCommand
* Description    : ��WIFIģ�����
* Input          : CommandString��ָ���ַ�����Buffer�����صĲ����ַ���ָ��
* Output         : Buffer�����صĲ���
* Return         : =1 error ,=0 ok 
*******************************************************************************/
uint8_t WIFI_ReadCommand(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t Status =0 ,Status_flag = 0, i ,ch , j = 0;
	
	WiFi_USART_SendStr(WIFI_COM,CommandString);
	for(i = 0 ; i < 10 ; i++)
	{
			if(WiFiSerialKeyTimeout(&ch))
			{
				Buffer[0] = ch;
				break;
			}
	}
	if(i>= 10)
		return 1;
	
  for(i = 1 ; i < 100 ; i++)
	{
		WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
		Buffer[i] = ch;
		if( ch == 0x0A)
		{
			
			j++ ; 
		}
		if(j >= 1) 
			break ;
		if(Buffer[2] == 0x4B &&Buffer[1] ==0x4F &&Buffer[0] ==0x2B)//+OK 
		{
			Status_flag = 1 ;
		}
	}
	Buffer[i+1]=0;	
	if( Status_flag)
	{
		Status = 0;                         
	}
	else
	{
    Status = 1 ;
	}
	return Status;
}


uint8_t WIFI_AT_WSCAN(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t Status =0 , data[1000],Status_flag = 0,ch , j = 0,k=0 ;
	uint16_t i=0 ;
	WiFi_USART_SendStr(WIFI_COM,CommandString);
	for(i = 0 ; i < 1000 ; )
	{

 		WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
		Buffer[i] = ch;
		data[i] = ch;
		if( ch == 0x0A)
		{
			if( data[i-1] == 0x0D )
				j++ ;
			if(j >= 8)
				break ;
		}
		if(data[2] == 0x4B &&data[1] ==0x4F &&data[0] ==0x2B)
		{
			Status_flag = 1 ;
		}
		if(data[1] == 'E'&&data[0] =='+')
		{
			Status_flag= 0 ;
			break ;
		}
		if(data[i] == 0)
		{
			
				k++ ;
				if(k>100)
						break ;
		}
		else
			i++;
		
		
	}
	data[i+1]=0;
	wscan_len = i+1 ;
	if( Status_flag)
	{
		Status = 0;                         
	}
	else
	{
        Status = 1 ;
	}
	return Status;
}
uint8_t WIFI_AT_WSCAN1(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t Status =0 , data[1000],Status_flag = 0,ch , j = 0,k=0 ;
	uint16_t i=0 ;
	WiFi_USART_SendStr(WIFI_COM,CommandString);
	for(i = 0 ; i < 1000 ; i++)
	{
 		WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
		Buffer[i] = ch;
		data[i] = ch;
		if( ch == 0x0A)
		{
			if( data[i-1] == 0x0D )
				j++ ;
			if(j >= 3)
				break ;
		}
		if(data[i] == 0x6B &&data[i-1] ==0x6F &&data[i-2] ==0x2B)
		{
			Status_flag = 1 ;
		}
		if(data[i] == 'E'&&data[i-1] =='+')
		{
			Status_flag= 0 ;
			break ;
		}
        if(data[i] == 0)
        {
            i-- ;
            k++ ;
            if(k>100)
                break ;
        }
	}
	wscan_len = i+1 ;
	if( Status_flag)
	{
		Status = 0;                         
	}
	else
	{
        Status = 1 ;
	}
	return Status;
}
/*******************************************************************************
* Function Name  : uint32_t WIFI_TranS_Config()
* Description    : ��͸��������ģʽ
* Input          : 
* Output         : Status�����صĲ���
* Return         : =1 error ,=0 ok 
*******************************************************************************/
uint8_t WIFI_TranS_Config(void)
{
	uint8_t DataBuf[40]= {0};
	uint8_t Status =0 ,i=0, j=0 ,ch , *ptr ;
	for(i = 0 ; i < 1 ; i++)
	{
		WiFi_USART_SendStr(WIFI_COM,"+++"); 
		
		Read_A(&ch);						 	//��ȡָ��ȷ��
		DataBuf[0] =  ch ;
		if( strcmp(DataBuf,"a") == 0)
		{
			break ;
		}
		else
		{
			for(i = 0 ; i < 10 ; i++)
			{
				WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
				DataBuf[0] = ch;
				if( DataBuf[0] == 'a')
				{
					break ; 
				}
			}
		}
	}	
	
	if( strstr(DataBuf,"a"))
  {
    WiFi_USART_SendStr(WIFI_COM,"a"); 
		
		for(i = 0 ; i < 20 ; i++)
		{
			if(WiFiSerialKeyTimeout(&ch))
			{
				DataBuf[0] = ch;
				break;
			}
		}
		
		
		for(i=1; i < 10 ; i++)
		{
			WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
			DataBuf[i] = ch;
			if( DataBuf[i] == 0x0A)
			{
				j++ ; 
			}
			if(j >= 1)
				break ;
		}
		ptr = strstr(DataBuf,"+OK") ;
		if(ptr)
            Status = 0;
		else
				Status = 1 ;
  }                        
	else
     Status = 1 ;
	return Status;


// 	uint8_t DataBuf[40]= {0};
// 	uint8_t Status =0 ,i=0, j=0 ,ch , *ptr ;
// 	for(i = 0 ; i < 1 ; i++)
// 	{
// 		WiFi_USART_SendStr(WIFI_COM,"+++"); 
// 		
// // 		Read_A(&ch);						 	//��ȡָ��ȷ��
// // 		DataBuf[0] =  ch ;
// // 		if( strcmp(DataBuf,"a") == 0)
// // 		{
// // 			break ;
// // 		}
// // 		else
// 		{
// 			for(i = 0 ; i < 40 ; i++)
// 			{
// 				WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
// 				DataBuf[0] = ch;
// 				if( DataBuf[0] == 'a')
// 				{
// 					break ; 
// 				}
// 			}
// 		}
// 	}	
// 	if( strstr(DataBuf,"a"))
//   {
//     WiFi_USART_SendStr(WIFI_COM,"a"); 
// 		
// 		for(i = 0 ; i < 20 ; i++)
// 		{
// 			if(WiFiSerialKeyTimeout(&ch))
// 			{
// 				DataBuf[0] = ch;
// 				break;
// 			}
// 		}
// 		
// 		
// 		for(i=1; i < 30 ; i++)
// 		{
// 			WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
// 			DataBuf[i] = ch;
// 			if( DataBuf[i] == 0x0A)
// 			{
// 				j++ ; 
// 			}
// 			if(j >= 1)
// 				break ;
// 		}
// 		ptr = strstr(DataBuf,"+OK") ;
// 		if(ptr)
//             Status = 0;
// 		else
// 				Status = 1 ;
//   }                        
// 	else
//      Status = 1 ;
// 	return Status;
}
/*******************************************************************************
* Function Name  : uint32_t WIFI_Config_TranS()
* Description    : ��������ɷ���
* Input          : 
* Output         : Status�����صĲ���
* Return         : =1 error ,=0 ok 
*******************************************************************************/
uint8_t WIFI_Config_TranS(void)
{
	uint8_t DataBuf[255],ch ,number;
	uint8_t Status =0;

	WiFi_USART_SendStr(WIFI_COM,"AT+Z\r\n");
  for(number = 0 ; number < 7 ; number++)
	{
		WiFiSerialKeyTimeout(&ch);						 //��ȡָ��ȷ��
		DataBuf[number] = ch;
	}
	if( strstr(DataBuf,"AT+Z") == NULL)
	{
		Status = 1;                         
	}
	else
	{
        Status = 0 ;
	}
	return Status;
}
/*******************************************************************************
* Function Name  : void Init_WiFi(void)
* Description    : ��ʼ��WIFIģ�飬�ȶ�ģ�鴮�ڲ����ʣ������Ϊ115200������Ϊ��ģ��Ϊ�µ�ģ�飬
				           ����䰴Ĭ�Ϸ�ʽ�������á�				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_WiFi(void)
{
	
	uint8_t wifi_buff[100]={0} , Status,wifi_mode[5],wifi_name[23],wifi_version[20];    /*, Wif_Class[3] = {0x80,0x06,0x04}*/  //��ӡ���豸����040680
	u32	i=0,j=0; 	
	uint8_t *p = NULL;
	uint8_t IntWifiFlag = 0;
  uint8_t DataBuf[100]={0};
	USART_InitTypeDef USART_WiFi_InitStructure; 

	if(g_tInterface.WIFI_Type == 0)
	{
		return;
	}	
	
	//Դ����  2017.09.12
  
	RCC_APB1PeriphClockCmd(WIFI_RCC_APB1Periph_USART, ENABLE);		  //ʹ�ܴ���ʱ�� 2017.10.13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  
	
  if(g_tInterface.WIFI_Type )
  {	               
		Status =0;
// 		while( GPIO_ReadInputDataBit(WIFI_CON_PORT, WIFI_READY ) )
// 		{
// 			i++ ;
// 			DelayUs(1);
// 			if(i > 3000000)
// 			{
// 				g_tError.R_ErrorFlag |= 0x80;   //wifiģ�����
// 				return ;
// 			}
// 		}
// 		if( GPIO_ReadInputDataBit(WIFI_CON_PORT, WIFI_READY ) == 0 )
		if(1)
    {			
			DelayMs(2000);  //2020.12.07 
			USART_WiFi_InitStructure.USART_BaudRate = 230400;							          //���ò�����  460800�����ϻ�����������
			USART_WiFi_InitStructure.USART_Parity = USART_Parity_No;   	   				  //��У��
			USART_WiFi_InitStructure.USART_WordLength = USART_WordLength_8b;   			//8λ����
			USART_WiFi_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//������ա�����	
			USART_WiFi_InitStructure.USART_StopBits = USART_StopBits_1;		   				//ֹͣλΪ1λ
			USART_WiFi_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��ʹ��Ӳ��������	
      USART_ITConfig(WIFI_COM, USART_IT_RXNE, DISABLE);							//USART1�����ж�
			USART_Init(WIFI_COM, &USART_WiFi_InitStructure);			//��ʼ��USART1
			USART_Cmd(WIFI_COM, ENABLE);							//�ر�USART2�����ж�

			Status = WIFI_TranS_Config();                                      //��͸������>ָ��ģʽ

			if(Status == 1)                                        
			{
				DelayMs(500);
				USART_WiFi_InitStructure.USART_BaudRate = 115200;							        //���ò�����
				USART_WiFi_InitStructure.USART_Parity = USART_Parity_No;   	   				//��У��
				USART_WiFi_InitStructure.USART_WordLength = USART_WordLength_8b;   			//8λ����
				USART_WiFi_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//������ա�����	
				USART_WiFi_InitStructure.USART_StopBits = USART_StopBits_1;		   				//ֹͣλΪ1λ
				USART_WiFi_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��ʹ��Ӳ��������				
				USART_ITConfig(WIFI_COM, USART_IT_RXNE, DISABLE);							//USART1�����ж�
				USART_Init(WIFI_COM, &USART_WiFi_InitStructure);			//��ʼ��USART1
				USART_Cmd(WIFI_COM, ENABLE);							//�ر�USART2�����ж�
				
        Status = WIFI_TranS_Config();

      }   
            
			if(Status == 1)                                                            //2015.04.29                                  
			{
				DelayMs(500);
				USART_WiFi_InitStructure.USART_BaudRate = 460800;												//���ò�����
				USART_WiFi_InitStructure.USART_Parity = USART_Parity_No;   	   					//��У��
				USART_WiFi_InitStructure.USART_WordLength = USART_WordLength_8b;   			//8λ����
				USART_WiFi_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//������ա�����	
				USART_WiFi_InitStructure.USART_StopBits = USART_StopBits_1;		   				//ֹͣλΪ1λ
				USART_WiFi_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��ʹ��Ӳ��������				
				USART_ITConfig(WIFI_COM, USART_IT_RXNE, DISABLE);							//USART1�����ж�
				USART_Init(WIFI_COM, &USART_WiFi_InitStructure);			//��ʼ��USART1
				USART_Cmd(WIFI_COM, ENABLE);							//�ر�USART2�����ж�
				
        Status = WIFI_TranS_Config();

			}  

			Status = WIFI_ReadCommand("AT+UART\r", DataBuf);     //��ȡ������
			if(Status == 0)
			{
					if(strstr(DataBuf,"+OK=230400") == NULL)				//�������230400�����ģ���ʼ��	
					{
						IntWifiFlag = 1;   //Wifi�������ݲ���λ��־
						WIFI_SetCommand("AT+UART=","230400,8,NONE,1,CTSRTS");   //2018.04.13  115200��Ϊ460800  ����ڱ��������
					}
			}					
      DelayMs(5);
			
			Status = WIFI_ReadCommand("AT+FMVER\r", DataBuf);  //��ѯ�汾��
			if(Status == 0)
			{
				p =strchr(DataBuf, '=');
				
				strcpy(wifi_version, p+1) ;
				for(i = 0 ; i < 20 ; i ++)
				{
					Wifi_Version[i]=wifi_version[i];
					if(Wifi_Version[i] == 0x0D)
					{
						Wifi_Version[i] = 0x0A;
						i ++ ;
						Wifi_Version[i] = 0 ;
						
					}
				}
			}
			else 
			{
				Status = 1 ;
				Wifi_Version[0] = '\n' ;
				Wifi_Version[1] = 0 ;
			}
			DelayUs(1000);
			Status = WIFI_ReadCommand("AT+CON1\r", DataBuf);	//��ѯTCP��UDP����1�Ĳ���
			if(Status == 0)
			{
				p =strchr(DataBuf, '=');			
				strcpy(wifi_buff, p+1) ;
				for(i = 0 ; i < 10 ; i ++)
				{
					Wifi_Cs[i]=wifi_buff[i];
					if(Wifi_Cs[i] == 0x2C)
					{
// 						Wifi_Cs[i] = '\r' ;
						Wifi_Cs[i] = '\n' ;
						Wifi_Cs[i+1] = 0 ;
							break ;
					}
				}
				p =strchr(DataBuf, ',');
				strcpy(wifi_buff, p+1) ;
				for(i = 0 ; i < 10 ; i ++)
				{
					Wifi_Port[i]=wifi_buff[i];
					if(Wifi_Port[i] == 0x2C)
					{
// 						Wifi_Port[i] = '\r' ;
						Wifi_Port[i] = '\n' ;
						Wifi_Port[i+1] = 0 ;
							break ;
					}
				}
				
				if(strcmp(Wifi_Cs,"SERVER\n") == 0)
				{
					strcpy(Wifi_Protocol,"TCP\n");
				}
				else if(strcmp(Wifi_Cs,"CLIENT\n") == 0)
				{
					strcpy(Wifi_Protocol,"TCP\n");
				}
				else if(strcmp(Wifi_Cs,"BOARDCAST\n") == 0)
				{
					strcpy(Wifi_Protocol,"UDP\n");
				}
				else if(strcmp(Wifi_Cs,"UNICAST\n") == 0)
				{
					strcpy(Wifi_Protocol,"UDP\n");
				}
			}
			else 
			{
				Status = 1 ;
				Wifi_Protocol[0] = '\n' ;
				Wifi_Protocol[1] = 0 ;
				Wifi_Cs[0] = '\n' ;
				Wifi_Cs[1] = 0 ;
				Wifi_Port[0] = '\n' ;
				Wifi_Port[1] = 0 ;
			}
      Status = WIFI_ReadCommand("AT+WMODE\r", DataBuf);//��ѯWIFI����ģʽ

			if(Status == 0)
			{
					if(strstr(DataBuf,"STA") != NULL)
					{     
							wifi_Mode_type = 1 ;
							p =strchr(DataBuf, '=');
							strcpy(wifi_mode, p+1) ;
							for(i = 0 ; i < 7 ; i ++)
							{
								Wifi_Mode[i]=wifi_mode[i];
								if(Wifi_Mode[i] == 0x0D)
								{
									Wifi_Mode[i] = 0x0A ;
									i ++ ;
									Wifi_Mode[i] = 0 ;
									break ;
								}
							}

							Status = WIFI_ReadCommand("AT+DHCP\r", DataBuf);  //STAģʽ�£��Ƿ���DHCPģʽ
							if(Status == 0)
							{
								p =strchr(DataBuf, '=');
								strcpy(DataBuf, p+1) ;
								for(i = 0 ; i < 23 ; i ++)
								{
									Wifi_DHCP[i]=DataBuf[i];
									
									if(Wifi_DHCP[i] == 0x0D)
									{
// 										Route_Name[i] = '\r' ;
										Wifi_DHCP[i] = 0x0A ;
										Wifi_DHCP[i+1] = 0 ;
										break ;
									}
								}
							}
							else 
							{
								Status = 1 ;
								Wifi_DHCP[0] = '\n' ;
								Wifi_DHCP[1] = 0 ;
							}
							
							
							Status = WIFI_ReadCommand("AT+WSTA\r", DataBuf);  //��ѯ���ӵ�·����������
							if(Status == 0)
							{
								p =strchr(DataBuf, '=');
								strcpy(DataBuf, p+1) ;
								for(i = 0 ; i < 23 ; i ++)
								{
									Route_Name[i]=DataBuf[i];
									
									if(Route_Name[i] == 0x2C)
									{
// 										Route_Name[i] = '\r' ;
										Route_Name[i] = '\n' ;
										Route_Name[i+1] = 0 ;
										break ;
									}
								}
							}
							else 
							{
								Status = 1 ;
								Route_Name[0] = '\n' ;
								Route_Name[1] = 0 ;
							}
							Status = WIFI_ReadCommand("AT+IPCONFIG\r", DataBuf);	//IP��ַ��ѯ  mask��ַ

							if(Status == 0)
							{
								p =mystrnchr(DataBuf, ',',3);
								strcpy(wifi_buff, p+1) ;
	
									for(i = 0 ; i < 20 ; i ++)
									{
										Wifi_Ipaddr[i]=wifi_buff[i];
										if(Wifi_Ipaddr[i] == 0x2C)
										{
// 											Wifi_Ipaddr[i] = '\r' ;
											Wifi_Ipaddr[i] = '\n' ;
											Wifi_Ipaddr[i+1] = 0 ;
											break ;
										}
									}
									
								p =mystrnchr(DataBuf, ',',4);
								strcpy(wifi_buff, p+1) ;
								for(i=0; i <20 ; i ++)
								{
									Wifi_Mask[i]=wifi_buff[i];
									if(Wifi_Mask[i] == 0x2C)
									{
// 										Wifi_Mask[i] = '\r' ;
										Wifi_Mask[i] = '\n' ;
										Wifi_Mask[i+1] = 0 ;
										break ;
									}
								}
								p =strchr(wifi_buff, ',');
								strcpy(wifi_buff, p+1) ;
								for(i=0; i <20 ; i ++)
								{
									Wifi_Gateway[i]=wifi_buff[i];
									if(Wifi_Gateway[i] == 0x0D)
									{
// 										Wifi_Mask[i] = '\r' ;
										Wifi_Gateway[i] = 0x0A ;
										Wifi_Gateway[i+1] = 0 ;
										break ;
									}
								}
								
								
								
							}
							else 
							{
								Status = 1 ;
								Wifi_Ipaddr[0] = '\n' ;
								Wifi_Ipaddr[1] = 0 ;
								Wifi_Mask[0] = '\n' ;
								Wifi_Mask[1] = 0 ;
							}
          }  
// 				else 
// 				{
// 					Status = 1 ;
// 					Wifi_Mode[0] = '\r' ;
// 					Wifi_Mode[1] = '\n' ;
// 					Wifi_Mode[2] = 0 ;
// 				}
        else if(strstr(DataBuf,"AP") != NULL)   //2014.05.28
        {
					wifi_Mode_type = 0 ;
					p =strchr(DataBuf, '=');
          strcpy(wifi_buff, p+1) ;
					for(i = 0 ; i < 7 ; i ++)
					{
						Wifi_Mode[i]=wifi_buff[i];
						if(Wifi_Mode[i] == 0x0D)
						{
							Wifi_Mode[i] = 0X0a ;
							i ++ ;
							Wifi_Mode[i] = 0 ;
							break ;
						}
					}

					Status = WIFI_ReadCommand("AT+IPCONFIG\r", DataBuf);//��ѯIP��ַ������������
					if(Status == 0)
					{
						p =strchr(DataBuf, '=');
						strcpy(wifi_buff, p+1) ;
						for(i = 0 ; i < 20 ; i ++)
						{
							Wifi_Ipaddr[i]=wifi_buff[i];
							if(Wifi_Ipaddr[i] == 0x2C)
							{
								Wifi_Ipaddr[i] = '\n' ;
								Wifi_Ipaddr[i+1] = 0 ;
								break ;
							}
						}
						p =strchr(wifi_buff, ',');
						strcpy(wifi_buff, p+1) ;
						for(i = 0 ; i < 20 ; i ++)
						{
							Wifi_Mask[i]=wifi_buff[i];
							if(Wifi_Mask[i] == 0x2C)
							{
								Wifi_Mask[i] = '\n' ;
								i ++ ;
								Wifi_Mask[i] = 0 ;
								break ;
							}
						}
						p =strchr(wifi_buff, ',');
						strcpy(wifi_buff, p+1) ;
						for(i = 0 ; i < 20 ; i ++)
						{
							Wifi_Gateway[i]=wifi_buff[i];
							if(Wifi_Gateway[i] == 0x2C)
							{
								Wifi_Gateway[i] = '\n' ;
								i ++ ;
								Wifi_Gateway[i] = 0 ;
								break ;
							}
						}
						
						
         }
					else 
					{
						Status = 1 ;
						Wifi_Ipaddr[0] = '\n' ;
						Wifi_Ipaddr[1] = 0 ;
						Wifi_Mask[0] = '\n' ;
						Wifi_Mask[1] = 0 ;
						Wifi_Gateway[0] ='\n' ;
						Wifi_Gateway[1] = 0 ;
					}
					
					Status = WIFI_ReadCommand("AT+WAP\r", DataBuf);  //��ȡ����SSID ������
					if(Status == 0)
					{
						p =strchr(DataBuf, '=');
						strcpy(wifi_buff, p+1) ;
						for(i = 0 ; i < 23 ; i ++)
						{
							Wifi_Name[i]=wifi_buff[i];	//�õ�����SSID
							if(Wifi_Name[i] == 0x2C)
							{
								Wifi_Name[i] = '\n' ;
								Wifi_Name[i+1] = 0 ;
								break ;
							}
						}
						p =strchr(wifi_buff, ',');
							strcpy(wifi_buff, p+1) ;
							for(i = 0 ; i < 40 ; i ++)
							{
								Wifi_Password[i]=wifi_buff[i];
								if(Wifi_Password[i] == 0x0D)
								{
									Wifi_Password[i] = 0x0A;
									i ++ ;
									Wifi_Password[i] = 0 ;
									break ;
								}
							}
							if(i<=2)
								strcpy(Wifi_Auth,"OPEN\n");
							else
								strcpy(Wifi_Auth,"WPA\n");
          }
					else 
					{
						Status = 1 ;
						Wifi_Name[0] = '\n' ;
						Wifi_Name[1] = 0 ;
					}
        }  
				else 
				{
					Status = 1 ;
					Wifi_Mode[0] = '\n' ;
					Wifi_Mode[1] = 0 ;
				}                
      }

			if(IntWifiFlag == 1)    //��λwifiģ�� ����������Ϣ ��λģ��
			{
				DelayMs(10);
				CLI();
				CloseWiFICommand();
				SEI();
			}
			else
			{
				Status = WIFI_ReadCommand("AT+QUIT\r", DataBuf);   //�˳�ATָ��ģʽ
			}	
    } 
		USART_WiFi_InitStructure.USART_BaudRate = 230400;							          //���ò�����   2015.04.29
		USART_WiFi_InitStructure.USART_Parity = USART_Parity_No;   	   				  //��У��
		USART_WiFi_InitStructure.USART_WordLength = USART_WordLength_8b;   			//8λ����
		USART_WiFi_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//������ա�����	
		USART_WiFi_InitStructure.USART_StopBits = USART_StopBits_1;		   				//ֹͣλΪ1λ
		USART_WiFi_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��ʹ��Ӳ��������	
		USART_Init(WIFI_COM, &USART_WiFi_InitStructure);			//��ʼ��USART1
		USART_Cmd(WIFI_COM, ENABLE);							//�ر�USART2�����ж�
		USART_ITConfig(WIFI_COM, USART_IT_RXNE, ENABLE);		//��USART2�����ж�
  }
}

void Wifi_SelfTest(void)     //2017.10.25
{
	PrintString("Wifi Parameter\n" );

	PrintString("  Wifi Version:  	" );
	PrintString(Wifi_Version);
	if(wifi_Mode_type == 0)
	{
	/****��ӡWifi�豸����*****************/
		PrintString("  Wifi Name:     	");
		PrintString(Wifi_Name);
	}
	/****��ӡWifi����ģʽ*****************/
	PrintString("  Wifi Mode:     	");
	PrintString(Wifi_Mode);

	if(wifi_Mode_type == 0) //AP
	{
		/****��ӡAP״̬�µ�AP��ַ*****************/
		PrintString("  Wifi IP:  	     " );
		PrintString(Wifi_Ipaddr);
		/****��ӡAP״̬�µ���������*****************/
		PrintString("  Wifi Mask:     ");
		PrintString(Wifi_Mask);

		PrintString("Wifi Gateway   ");
		PrintString(Wifi_Gateway);
		/****��ӡAP״̬����֤ģʽ*****************/
		PrintString("  Wifi Auth:     ");
		PrintString(Wifi_Auth);

		/****��ӡAP״̬�¼����㷨ģʽ*****************/
// 		PrintString("  Wifi Encry:    ");
// 		PrintString(Wifi_Encry);
		if(Wifi_Auth[0] == 'W')
		{
			/****��ӡAP״̬������*****************/
			PrintString("  Wifi Password: ");
			PrintString(Wifi_Password);
		}
	}
	if(wifi_Mode_type == 1) //STA
	{
	//			Route_Name
        PrintString("  Route_Name:  	  ");
		PrintString(Route_Name);
				PrintString("  DHCP:  	  ");
		PrintString(Wifi_DHCP);
		/****��ӡSTA״̬�µ�IP��ַ*****************/
		PrintString("  Wifi IP:  	     ");
		PrintString(Wifi_Ipaddr);
		/****��ӡSTA״̬�µ���������*****************/
		PrintString("  Wifi Mask:     ");
		PrintString(Wifi_Mask);
		
		PrintString("Wifi Gateway:  ");
		PrintString(Wifi_Gateway);
	}
	/****��ӡWifi����Э��*****************/
	PrintString("  Network Pro:   ");
	PrintString(Wifi_Protocol);
	/****��ӡWifi����ģʽ*****************/
	PrintString("  Network Cs:  	  ");       
	PrintString(Wifi_Cs);
	/****��ӡWifi����Э��˿�*****************/
	PrintString("  Network Port:  	");
	PrintString(Wifi_Port);
}

/*******************************************************************************
* Function Name  : void WIFI_Config_Gpio(void)
* Description    : ����wifi�ܽ�
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void WIFI_Config_Gpio(void)
{  
	  u32	i;
		GPIO_InitTypeDef GPIO_InitStructure;  
	
	RCC_APB2PeriphClockCmd(WIFI_RCC_APB2Periph_GPIO, ENABLE);
	RCC_APB1PeriphClockCmd(WIFI_RCC_APB1Periph_USART, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);   //��ӳ�䴮��2
	
	
	
	
	
		GPIO_InitStructure.GPIO_Pin = WIFI_LINK ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
		GPIO_Init(WIFI_CON_PORT, &GPIO_InitStructure);	
      WIFI_ELINK_SET;
		GPIO_InitStructure.GPIO_Pin =	WIFI_RXD;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(WIFI_CON_PORT, &GPIO_InitStructure);				

		GPIO_InitStructure.GPIO_Pin = 	WIFI_TXD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   	 //�������
		GPIO_Init(WIFI_CON_PORT, &GPIO_InitStructure);

// 		GPIO_InitStructure.GPIO_Pin = WIFI_DTR ;				       //���ýӿڶ��� CTS
// 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			     //��������
// 		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 		GPIO_Init(WIFI_CON_PORT, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = WIFI_BUSY ;			   	//æ RTS
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
		GPIO_Init(WIFI_CON_PORT, &GPIO_InitStructure);	
       
    WIFI_BUSY_CLR;											//��æ	
		

    
}


//2017.10.25  ��������ָ��

/*******************************************************************************
* Function Name  : SetCommand_1B46
* Description    : ����Wifi����ģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B46(void)
{
	uint8_t DataBuf[40],Status;

	Read_NByte(1,DataBuf );
//	SystemDelay1mS(10);
 	DelayUs(10000) ;
	if(DataBuf[0] == 0)
	{

		CLI();
		Status = WIFI_SetCommand("AT+WMODE=","STA");
    SEI();       
// SystemDelay1mS(10);        
		if(Status == 0)
		{
			UpLoadData("OK\r\n", 4);	//�ϴ�����
			PrintString("Wifi Laddr:");	
			PrintString("STA");
			PrintString("\r\n");
		}
		else
			UpLoadData("ERROR\r\n", 7);	//�ϴ�����


	}
	else if(DataBuf[0] == 1)
	{
	
		CLI();
		Status = WIFI_SetCommand("AT+WMODE=","AP");
    SEI();
//         SystemDelay1mS(10);
 		if(Status == 0)
 		{
 			UpLoadData("OK\r\n", 4);	//�ϴ�����
 			if(g_tSysConfig.SysLanguage == 1)
 				PrintString("Wifiģʽ��");	
			else
				PrintString("Wifi Laddr:");	
			PrintString("AP");
			PrintString("\r\n");
		}
 		else 
		{
 			UpLoadData("ERROR\r\n", 7);	//�ϴ�����
			PrintString("ERROR");
			PrintString("\r\n");
		}
	}	
    
    GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}
/*******************************************************************************
* Function Name  : SetCommand_1B41
* Description    : ����Wifi����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B41(void)
{
	uint8_t DataBuf[40],Status;
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
    while(g_tInBuf.BytesNumber != 0);    //2017.10.25  �ȴ���������
    CLI();
	Status = WIFI_SetCommand("AT+WRMID=",DataBuf);
    SEI();
//     SystemDelay1mS(10);
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("Wifi���ƣ�");	
		else
			PrintString("Wifi Name:");	
		PrintString(DataBuf);
		PrintString("\r\n");	
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
	{
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����
	}
}
/*******************************************************************************
* Function Name  : SetCommand_1B42
* Description    : ����Wifi��STAģʽ�µ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B42(void)
{
	uint8_t DataBuf[60]={0},i,n,Status ,*p=NULL,wifi_buf[40]={0};
	uint32_t Number;

	n=ReadInBuffer();
//  	SystemDelay1mS(10);
	DelayUs(10000) ;
	if(n == 0 )
	{
        CLI();
		    Status = WIFI_SetCommand("AT+DHCP=","ON"); //DHCPʹ��
        SEI();
//         SystemDelay1mS(10);
        if(g_tSysConfig.SysLanguage == 1)
            PrintString("Wifi IP ��ַ��");	
        else
            PrintString("Wifi IP Laddr:");
        PrintString("DHCP");
	}
	else if(n == 1)
	{
		Number =ReadInBuffer();
		Read_NByte(Number,DataBuf );
// 		SystemDelay1mS(10);
		DelayUs(10000) ;
		WIFI_SetCommand("AT+DHCP=","OFF");
		DelayUs(10000) ;
         CLI();
		Status = WIFI_SetCommand("AT+IPCONFIG=",DataBuf);
        SEI();
		
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����

		p =strchr(DataBuf, ',');
		strcpy(DataBuf,(p+1));
		
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x2C)
			{
				wifi_buf[i] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("IP��ַ��");	
		else
			PrintString("Wifi IP:");	
		PrintString(wifi_buf);
		PrintString("\r\n");

		p =strchr(DataBuf, ',');
		strcpy(DataBuf,p+1);
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x2C)
			{
				wifi_buf[i] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("�������룺");	
		else
			PrintString("Wifi Mask:");	
		PrintString(wifi_buf);
		PrintString("\r\n");
		
		p =strchr(DataBuf, ',');
		strcpy(DataBuf,(p+1));
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x0D)
			{
				i++;
				wifi_buf[i+1] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("���ص�ַ��");	
		else
			PrintString("Wifi Gateway:");	
		PrintString(wifi_buf);
		PrintString("\r\n");
		
	}
		

	}
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����
//         if(n==1)
//         {

//         }
//         else 
//         {

//         }
        PrintString("\r\n");
        GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����
}
/*******************************************************************************
* Function Name  : SetCommand_1B43
* Description    : ����AP
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B43(void)
{
	uint8_t DataBuf[1000],statebuf= 0;
	uint16_t  i;
//    while(PrintBuf.IdxWR != PrintBuf.IdxRD);
    CLI();    
 	WIFI_AT_WSCAN("AT+WSCAN\r", DataBuf);
	statebuf = g_tSystemCtrl.PrtSourceIndex;
    SEI();
	
DelayUs(3000000);
	while(1)
	{
		
			for(i = 0 ; i < wscan_len ; i ++)
			{
				g_tSystemCtrl.PrtSourceIndex = statebuf;
				UpLoadData(DataBuf+i,1);	//�ϴ�����
				if(g_tSystemCtrl.PrtSourceIndex == USB)
				{	
					DelayUs(800);
// 						while(clear_busy==0) ;
				}	
				else if(g_tSystemCtrl.PrtSourceIndex == ETHERNET )
				{                   
					
				}
			}
			if(i >= wscan_len )
			{	
				break ;
			}
	}
	DelayUs(10000) ;	
}
/*******************************************************************************
* Function Name  : SetCommand_1B44
* Description    : ����·�ɣ�����·������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B44(void)
{
	uint8_t DataBuf[40],Status;
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;

     CLI();
	Status = WIFI_SetCommand("AT+WSTA=",DataBuf);
    SEI();
//     SystemDelay1mS(10);
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("·�����ƣ�");	
		else
			PrintString("Route Name:");	
		PrintString(DataBuf);
		PrintString("\r\n");
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����

}
/*******************************************************************************
* Function Name  : SetCommand_1B45
* Description    : ����·������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B45(void)
{
	uint8_t DataBuf[40],Status;
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;
    while(g_tInBuf.BytesNumber != 0);    //2017.10.25  �ȴ���������
     CLI();
	Status = WIFI_SetCommand("AT+WSKEY=",DataBuf);
    SEI();
//     SystemDelay1mS(10);
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����

		if(g_tSysConfig.SysLanguage == 1)
			PrintString("·�����룺");	
		else
			PrintString("Route Password:");	
		PrintString(DataBuf);
		PrintString("\r\n");
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��	
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����

}
/*******************************************************************************
* Function Name  : SetCommand_1B47
* Description    : ����Wifi������Э�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B47(void)
{
	uint8_t DataBuf[40], i,Status,wifi_buf[40],wifi_cs[10],wifi_port[10],*p=NULL;
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;
     CLI();
	Status = WIFI_SetCommand("AT+CON1=",DataBuf);
    SEI();
//     SystemDelay1mS(10);
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����

		for(i = 0 ; i < Number ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x2C)
			{
				wifi_buf[i] = 0 ;
					break ;
			}
		}
		
				if(strcmp(wifi_buf,"SERVER") == 0)
				{
					strcpy(Wifi_Protocol,"TCP\n");
					strcpy(wifi_cs,"SERVER\n");
				}
				else if(strcmp(wifi_buf,"CLIENT") == 0)
				{
					strcpy(Wifi_Protocol,"TCP\n");
					strcpy(wifi_cs,"CLIENT\n");
				}
				else if(strcmp(wifi_buf,"BOARDCAST") == 0)
				{
					strcpy(Wifi_Protocol,"UDP\n");
					strcpy(wifi_cs,"SERVER\n");
				}
				else if(strcmp(wifi_buf,"UNICAST") == 0)
				{
					strcpy(Wifi_Protocol,"UDP\n");
					strcpy(wifi_cs,"CLIENT\n");
				}
		
		
		
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("����Э�飺");	
		else
			PrintString("Network Pro:");	
		PrintString(Wifi_Protocol);
		PrintString("\r\n");


		if(g_tSysConfig.SysLanguage == 1)
			PrintString("����ģʽ��");	
		else
			PrintString("Network Cs:");	
		PrintString(wifi_cs);
		PrintString("\r\n");
	//    GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��

		p =strchr(DataBuf, ',');
		strcpy(DataBuf, p+1) ;
		for(i = 0 ; i < 10 ; i ++)
		{
			wifi_port[i]=DataBuf[i];
			if(wifi_port[i] == 0x2C)
			{
				wifi_port[i] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("Э��˿ڣ�");	
		else
			PrintString("Network Port:");	
		PrintString(wifi_port);
		PrintString("\r\n");
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����
}
/*******************************************************************************
* Function Name  : SetCommand_1B48
* Description    : ����AP�����ò���  ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B48(void)
{
	uint8_t DataBuf[50]={0},Buff[70]={0},i,Status,*p=NULL;
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;
  CLI();
	Status = WIFI_ReadCommand("AT+WAP\r", Buff);  //��ȡ����SSID ������
SEI();
	if(Status == 0)
	{
		p =strchr(Buff, ',');
		strcpy(Buff, p) ;  //�õ����루����������:,key
		for(i = 0 ; i < 23 ; i ++)
		{
			if(Buff[i] == 0x0D)
			{
				Buff[i] = 0 ;
				break ;
			}
		}
	}

	strcat(DataBuf,Buff);
	CLI();
	Status = WIFI_SetCommand("AT+WAP=",DataBuf);  //����SSID,KEY
    SEI();

	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����
		p =strchr(DataBuf, ',');
		*p = 0;

		if(g_tSysConfig.SysLanguage == 1)
			PrintString("Wifi���ƣ�");	
		else
			PrintString("Wifi Name:");	
		PrintString(DataBuf);
		PrintString("\r\n");
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����	
}
/*******************************************************************************
* Function Name  : SetCommand_1B49
* Description    : ����AP�ļ��ܲ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B49(void)
{
	uint8_t DataBuf[50]={0},Buff[70]={0},i,Status,wifi_buf[40],wifi_password[40] ,wifi_encry[10],*p=NULL;
	uint32_t Number;
	
	Number =ReadInBuffer(); //��ȡAP���볤��

	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;
	
	  CLI();
	Status = WIFI_ReadCommand("AT+WAP\r", Buff);  //��ȡ����SSID ������
	SEI();
	if(Status == 0)
	{
		p =strchr(Buff, '=');
		strcpy(Buff,(p+1)) ;
		p =strchr(Buff, ',');
		strcpy((p+1), DataBuf) ;  //������copy����ȡ���ַ�����

	}

  CLI();
	Status = WIFI_SetCommand("AT+WAP=",Buff);
  SEI();

	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����
		if(Number < 8)
		{
			if(g_tSysConfig.SysLanguage == 1)
				PrintString("Wifi������");	
			else
				PrintString("Wifi No Password");	
		}
		else
		{
			if(g_tSysConfig.SysLanguage == 1)
				PrintString("Wifi���룺");	
			else
				PrintString("Wifi Password:");	
			PrintString(DataBuf);
		}
		PrintString("\r\n");

		DelayUs(10000) ;
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����


}
/*******************************************************************************
* Function Name  : SetCommand_1B4A
* Description    : ����Wifi������Э�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1B4A(void)
{
	uint8_t DataBuf[60],i,Status ,*p=NULL,wifi_buf[40];
	uint32_t Number;
	
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
// 	SystemDelay1mS(10);
	DelayUs(10000) ;
    CLI(); 
	Status = WIFI_SetCommand("AT+IPCONFIG=",DataBuf);
     SEI();
//     SystemDelay1mS(10);
	if(Status == 0)
	{
		UpLoadData("OK\r\n", 4);	//�ϴ�����

		p =strchr(DataBuf, ',');
		strcpy(DataBuf,(p+1));
		
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x2C)
			{
				wifi_buf[i] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("IP��ַ��");	
		else
			PrintString("Wifi IP:");	
		PrintString(wifi_buf);
		PrintString("\r\n");
	//    GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��	

		p =strchr(DataBuf, ',');
		strcpy(DataBuf,p+1);
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x2C)
			{
				wifi_buf[i] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("�������룺");	
		else
			PrintString("Wifi Mask:");	
		PrintString(wifi_buf);
		PrintString("\r\n");
		
		p =strchr(DataBuf, ',');
		strcpy(DataBuf,(p+1));
		for(i = 0 ; i < 20 ; i ++)
		{
			wifi_buf[i]=DataBuf[i];
			if(wifi_buf[i] == 0x0D)
			{
				i++;
				wifi_buf[i+1] = 0 ;
					break ;
			}
		}
		if(g_tSysConfig.SysLanguage == 1)
			PrintString("���ص�ַ��");	
		else
			PrintString("Wifi Gateway:");	
		PrintString(wifi_buf);
		PrintString("\r\n");
		
		GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
	}
	else 
		UpLoadData("ERROR\r\n", 7);	//�ϴ�����
}

/*******************************************************************************
* Function Name  : void SetWiFICommand(u8 FCommand,u8 SCommand)
* Description    : ����wifi����
* Input          : Command������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
void SetWiFICommand(u8 FCommand,u8 SCommand)
{
    u8 GetChar,CmdData[40],i;
    
    if( FCommand == 0x1b )
    { 
				switch( SCommand )
				{	
					case 0x41:			
					{
						SetCommand_1B41();		//�����豸����
						
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x42:			
					{
						SetCommand_1B42();		//����STA�µĹ�������
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x43:			
					{

						SetCommand_1B43();		//����AP
         Set_BTWFflg = 1 ;
						break;
					}
					case 0x44:			
					{
						SetCommand_1B44();		//����·��
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x45:			
					{
						SetCommand_1B45();		//����·������
            Set_BTWFflg = 1 ;
						break;
					}						
					case 0x46:			
					{
						SetCommand_1B46();		//����WIFI����ģʽ
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x47:			
					{
						SetCommand_1B47();		//��������Э�����
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x48:			
					{
						SetCommand_1B48();		//����WIFI��APģʽ�µ����ò���
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x49:			
					{
						SetCommand_1B49();		//����WIFI��APģʽ�µļ��ܲ���
            Set_BTWFflg = 1 ;
						break;
					}
					case 0x4A:			
					{
						SetCommand_1B4A();		//����WIFI��APģʽ�µ��������
            Set_BTWFflg = 1 ;
						break;
					}
					default:
						break;
				}
		}
		else if(FCommand ==0x1F)               //2017.10.25
	  {	 
		  if(SCommand == 0x5A)
			{
				  GetChar =ReadInBuffer();		     //2016.08.13  ��ȡ�ڶ�λ����
					switch( GetChar )
					{
						case 0x02:
						{
							SetCommand_1B42();		//����STA�µĹ�������
							Set_BTWFflg = 1;
							break;
						}
						case 0x03:
						{
							SetCommand_1B43();		//����AP
							Set_BTWFflg = 1 ;
							break;
						}
						case 0x04:
						{
							SetCommand_1B44();	//�������ӵ�·���������ƺ�����
							Set_BTWFflg = 1;
							break;
						}
						case 0x06:
						{
							SetCommand_1B46();	//����WIFI����ģʽ
							Set_BTWFflg = 1;
							break;
						}
						case 0x07:
						{
							SetCommand_1B47();		//��������Э�����
							Set_BTWFflg = 1;
							break;
						}
						case 0x08:
						{
							SetCommand_1B48();	//����APģʽ�� wifi����
							Set_BTWFflg = 1;
							break;
						}
						case 0x09:
						{
							SetCommand_1B49();  //����APģʽ�� wifi����
							Set_BTWFflg = 1;
							break;
						}
						case 0x0A:
						{
							SetCommand_1B4A();		//����WIFI��APģʽ�µ��������
							Set_BTWFflg = 1;
							break;
						}
						default:
							break;
					}
			}
    }
}		

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
