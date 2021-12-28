/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   BLUETOOTH��س���
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
#include	"bluetooth.h"
#include	"extgvar.h"
#include	"debug.h"
#include  "string.h"
#include  "maintain.h"
/* Private typedef -----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern FLASH_Status Flash_Inside_BufWrite(const u8* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
extern void Flash_Inside_ReadByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

//2.0 4.0˫ģ����ģ��
u8	*I482E_DefaultParameter[] ={        
 								//"AT+BIND=0",				    //��ַ�󶨣���0����

 								//"AT+CLEARADDR",				 //����󶨵�ַ
 								//"AT+ROLE=0",				   //�豸��ɫ������ģʽ����0��ģʽ
								"AT+SSP=0",				       //�Ƿ��Ȩ����0��Ȩ�
								"AT+NAME=T801_BT_Printer",	//����  
 								"AT+CLASS=040680",	        //����
								"AT+PIN=1234",			        //����  
								"AT+BAUD=115200",			      //������ 2016.09.23 ��Ϊ115200
	              "AT+BTMODE=1",			        //2016.09.14 ���þ�Ĭģʽ
	              "AT+MULTICONN=0",          //2020.05.28  �رն�����
	              "AT+IDLE=10000",				 //ivt��BR2141es ��������Ϊ0ȡ������
                                
  };

//���3.0BTģ��
// u8	*BC04_DefaultParameter[] ={
// 								"AT+BIND=0",				   //��ַ�󶨣���0����

// 								"AT+CLEARADDR",				 //����󶨵�ַ
// 								"AT+ROLE=0",				   //�豸��ɫ������ģʽ����0��ģʽ
// 								"AT+AUTH=1",				   //�Ƿ��Ȩ����1��Ȩ

// 								"AT+NAME=T801_BT_Printer",	   //����   
// 								"AT+CLASS=040680",			       //����
// 								"AT+PASSWORD=1234",			       //����
// 								"AT+BAUD=230400",			         //������
// 	              "AT+LINKPLCY=1,6,6,0,300",     //�����������
// 	              "AT+SNIFF=0,0,0,0",					   //����������� 
// 	};

// u8	*JOBT4_DefaultParameter[] ={
// 								"AT+BIND=0",				   //��ַ�󶨣���0����

// 								"AT+CLEARADDR",				 //����󶨵�ַ
// 								"AT+ROLE=0",				   //�豸��ɫ������ģʽ����0��ģʽ
// 								"AT+AUTH=1",				   //�Ƿ��Ȩ����1��Ȩ

// 								"AT+NAME=T801_BT_Printer",	   //����   
// 								//"AT+CLASS=040680",			     //����
// 								"AT+PASSWORD=123456",			     //����
// 								"AT+BAUD=115200",			         //������ 115200
// 	              "AT+LINKPLCY=1,6,6,0,300",     //�����������
// 	};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*Extern Private functions ---------------------------------------------------------*/
extern void DelayUs(u32 DelayNumber);
extern u8	ReadInBuffer( void );
extern void SPI_FLASH_BlockErase(u32 SectorAddr); 
extern void UpLoadData(u8 *DataBuf, u32 Length);				//�ϴ�����
extern void PrintString( u8 *Str );
extern void GoDotLine(u32 DotLineNumber);
/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param key: The key pressed
  * @retval : 1: Correct
  *   0: Error
  */
	
/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ�����ڿ��ƹܽ�
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitBluetoothPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/************* �����������ڶ˿� ****************************/
	RCC_APB2PeriphClockCmd(BT_RCC_APB2Periph_GPIO, ENABLE);
	RCC_APB1PeriphClockCmd(BT_RCC_APB1Periph_USART, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);   //��ӳ�䴮��2
	
	GPIO_InitStructure.GPIO_Pin		=	BT_UART_RX;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//���ڸ�������  
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		=	BT_UART_TX;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;		   //���ڸ������
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  BT_UART_CTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  			
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= BT_UART_RTS;	    	   //��������æ
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
  GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	GPIO_SetBits(BT_UART_PORT, BT_UART_RTS );							 //����Ϊæ
	
	GPIO_InitStructure.GPIO_Pin =  BT_WIFI_TYPE ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //��ʼ��Ϊ����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin =  BT_LINK ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  		
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = BT_RST;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
  GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);	
	GPIO_ResetBits(BT_UART_PORT, BT_RST );							  //���õ�
	
	GPIO_InitStructure.GPIO_Pin = BT_SET ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(BT_UART_PORT, BT_SET );								//���õ�
}

/*******************************************************************************
* Function Name  : ������
* Description    : ��ʼ����������
* Input          : �������
* Output         : �������
* Return         : ���ز���
*******************************************************************************/
void	InitBTUsart(void)
{	
	RCC_APB1PeriphClockCmd(BT_RCC_APB1Periph_USART, ENABLE);		  //ʹ�ܴ���ʱ��

	USART_BT_InitStructure.USART_BaudRate = 115200;               
	USART_BT_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_BT_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_BT_InitStructure.USART_Parity = USART_Parity_No;
	USART_BT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_BT_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(BT_UART, &USART_BT_InitStructure);
	USART_Cmd(BT_UART, ENABLE);	  
	USART_ITConfig(BT_UART, USART_IT_RXNE, DISABLE);		          //�ر�BT��USART�����ж�DISABLE
	USART_ClearFlag(BT_UART,USART_FLAG_TC); 	
	
}


uint32_t SerialKeyPressed(uint8_t *key)
{
	static u16 Timeout=0;
	while(1)
	{  
			if ( USART_GetFlagStatus(BT_UART, USART_FLAG_RXNE) != RESET)
			{
					*key = (uint8_t)BT_UART->DR;	
					Timeout = 0;   
					return 1;
			}
			else
			{
					Timeout++;
					DelayUs(1);          
			}
			if(Timeout >25000)
			{
					*key = 0;
					Timeout = 0;
					return 0;
			}
	}
}

uint32_t SerialKeyTimeout(uint8_t *key)
{
    static u16 Timeout=0;
    while(1)
    {  
        if ( USART_GetFlagStatus(BT_UART, USART_FLAG_RXNE) != RESET)
        {
            *key = (uint8_t)BT_UART->DR;	
            Timeout = 0;   
            return 1;
        }
        else
        {
            Timeout++;
            DelayUs(1);          
        }
        if(Timeout >25000)
        {
            *key = 0;
            Timeout = 0;
					  BtTimeOut =1;   //2016.09.14  ��ʱ
            return 1;
        }
    }
}

/*******************************************************************************
* Function Name  : void ReadSrt(u8 *Str)
* Description    : �����뻺������ȡ��0A�������ַ���,�ַ�����󳤶�Ϊ255�ַ�
* Input          : Srt:�ַ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
void ReadSrt( u8 *Str )
{
	u8	ch;
	u32 i;

	while(!SerialKeyTimeout(&ch));			 //�ȴ����յ��ַ�
	if(ch ==0x0d)							 //ȥ������ʼ�ġ�0D��0A��
	{
		while(!SerialKeyTimeout(&ch));
		while(!SerialKeyTimeout(&ch));
	}
	Str[0]=ch;
	for(i=1; i< 45;i++)
	{
		while(!SerialKeyTimeout(&ch));
 		Str[i]=ch;
		if(Str[i] == 0x0a)
		{
			i++;
			break;
		}
	}
	Str[i]=0;	
}
/*******************************************************************************
* Function Name  : void USART_SendSrt(USART_TypeDef* USARTx, *u8 Str)
* Description    : ��ָ���Ĵ��ڷ���һ�ַ���
* Input          : USARTX�����ںţ�Srt:�ַ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
void USART_SendStr(USART_TypeDef* USARTx, u8 *Str)
{
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
* Function Name  : 	OpenSetupBluetooth
* Description    : ������������ģʽ
				           �������������ǰ����ȷ��������	
* Input          : None
* Output         : None
* Return         : 0 = �ɹ��� 1 = ʧ��
*******************************************************************************/
uint8_t OpenSetupBluetooth(void)
{
	uint8_t DataBuf[40];	

	USART_ITConfig(BT_UART, USART_IT_RXNE, DISABLE);		

	
	if(BluetoothStruct.Type == DMBT)                                     //˫ģģ������ģʽ�����ʹ̶�Ϊ230400
	{
		 USART_BT_InitStructure.USART_BaudRate = 115200;						//2016.09.23
	}
	USART_Init(BT_UART, &USART_BT_InitStructure);
		

	

	
	if(BluetoothStruct.Type == DMBT) 				//˫ģI482E
	{
		BT_RST_LOW;
		SysDelay1mS(20);     						//�õ�IO����Ҫ��λģ���������ģʽ
		BT_RST_HIGH; 
		SysDelay1mS(2000);
	}
  else                                                //����ģ��
	{
		BT_RST_LOW;
 		SysDelay1mS(20);     						//�õ�IO����Ҫ��λģ���������ģʽ
 		BT_RST_HIGH; 
    SysDelay1mS(3000);
	}
	return 0;											   
}

/*******************************************************************************
* Function Name  : 	CloseSetupBluetooth
* Description    : �˳���������ģʽ
* Input          : None
* Output         : None
* Return         : 0 = �ɹ��� 1 = ʧ��
*******************************************************************************/
uint8_t CloseSetupBluetooth(void)
{

	
	 if(BluetoothStruct.Type == DMBT)		//2016.09.21
	 {
		  if(BtParaReloadFlag==1)
			{
        BtParaReloadFlag = 0;							
				BT_RST_LOW;
		    SysDelay1mS(20);     						//�õ�IO����Ҫ��λģ���������ģʽ
		    BT_RST_HIGH; 
		    SysDelay1mS(1000);
			}	
			USART_BT_InitStructure.USART_BaudRate = 115200;
	 }
	 else
	 {
			USART_BT_InitStructure.USART_BaudRate = 230400;
   }	 
	 USART_Init(BT_UART, &USART_BT_InitStructure); 			  //��������
	 USART_ITConfig(BT_UART, USART_IT_RXNE, ENABLE);		  //��USART1�����ж�
	 return 0;										
}


/*******************************************************************************
* Function Name  : uint32_t BC04_ReadCommand
* Description    : ������ģ�����.����ľ�ʽ������ʽ�����������
* Input          : CommandString��ָ���ַ�����Buffer�����صĲ����ַ���ָ��
* Output         : Buffer�����صĲ���
* Return         : =1 error ,=0 ok    2014.06.18
*******************************************************************************/
uint8_t BC04_ReadCommand(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t DataBuf[400];
	uint8_t Status =0;

	USART_SendStr(BT_UART,CommandString);

	ReadSrt(Buffer);						 //��ȡָ��ȷ��

	if(strcmp(Buffer,"ERROR\r\n") == 0)
	{
		Status = 1;
    g_tError.UR_ErrorFlag |= 0x10;
	}
	else
	{
		if(strcmp(Buffer,"OK\r\n") == 0)
			ReadSrt(Buffer);			//OK��ǰ,��ȡ���صĲ���
		else
			ReadSrt(DataBuf);			//OK�ں󣬶�ȡ���ص�OK
	}
	return Status;
}
/*******************************************************************************
* Function Name  : uint32_t BC04_SetCommand
* Description    : ������ģ�鷢������ָ����ݽ��ģ��
* Input          : CommandString��ָ���ַ���
* Output         : None
* Return         : =1 error ,=0 ok   2014.06.18
*******************************************************************************/
uint8_t BC04_SetCommand(uint8_t *CommandString)
{
	uint8_t DataBuf[40];

	USART_SendStr(BT_UART,CommandString);
  USART_SendStr(BT_UART,"\r\n");
    
	ReadSrt(DataBuf);						//��ȡָ��ȷ��
	if(strcmp(DataBuf,"ERROR\r\n") == 0)
  {
    g_tError.UR_ErrorFlag |= 0x10;
		return 1;
  }
	else
  {
		return 0;
  }
}
/*******************************************************************************
* Function Name  : void Judge_BT_BM57(void)
* Description    : ͨ���ź��ߵ�ƽ�ж�����ģ���ͺ�
				           BluetoothStruct.Type 1,JOBT4 4.0ģ�飻2,GCJO 3.0ģ�飻3,DMBT,I482ES˫ģģ�飻4,DLBT,һ�϶�BTģ��	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Judge_BT_Type(void)
{
	uint32_t	i;
	uint8_t  Temp;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	for(i=0;i<80;i++)				   			//�ȴ����ģ�鸴λ���
	{
		DelayMs(20);								  //�˴�δ���δ�ʱ�� ����SysDelay1mS����
		if(!READ_BT_CTS)						  //����н��ģ�飬��ģ�鸴λ��BT_CTS�ź�Ϊ��  ��ʼ��ʱΪ��������
		{	
			break;
		}
	}
  if(!READ_BT_CTS)								      //CTSΪ�͵�ƽ			
	{				
    g_tInterface.BT_Type = 1;			      //������ģ��	
	}
	else										              //
	{	
    g_tInterface.BT_Type = 0;
	}
}
/*******************************************************************************
* Function Name  : void BTDMReadSrt(void)
* Description    : ˫ģģ��ͨ�Ŷ�ȡ�ַ���			            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 BTDMReadSrt( u8 *Str )         
{
	u8	ch;
	u32 i,j=0;

	while(!SerialKeyTimeout(&ch))			 //�ȴ����յ��ַ�
  {
		j++;
		if(j>=100)
			return 1;
  }
	if(ch ==0x0d)							        //ȥ������ʼ�ġ�0D��0A��
	{
		while(!SerialKeyTimeout(&ch));
		while(!SerialKeyTimeout(&ch));
	}
	
	Str[0]=ch;
	for(i=1; i< 35;i++)
	{
		while(!SerialKeyTimeout(&ch));
 		Str[i]=ch;
		if(Str[i] == 0x0a)
		{
			i++;
			break;
		}
	}
	Str[i]=0;	 
	if(BtTimeOut==1)  //2016.09.14 ������ʱ����
	{	
		BtTimeOut =0;
		return 1;
	}	
  else
	{	
		return 0;
	}	
	
}
/*******************************************************************************
* Function Name  : void I482_ReadCommand(void)
* Description    : ˫ģģ���ȡָ��
				            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t I482_ReadCommand(uint8_t *CommandString,uint8_t *Buffer)  
{
	uint8_t DataBuf[400];
	uint8_t Status =0;

	USART_SendStr(BT_UART,CommandString);

	Status = BTDMReadSrt(Buffer);						 //��ȡָ��ȷ��
  if(Status == 1)
    return Status;

	if(strcmp(Buffer,"ERROR\r\n") == 0)
	{
		Status = 1;
    g_tError.UR_ErrorFlag |= 0x10;
	}
	else
	{
		if(strcmp(Buffer,"OK\r\n") == 0)
			ReadSrt(Buffer);			//OK��ǰ,��ȡ���صĲ���
		else
			ReadSrt(DataBuf);			//OK�ں󣬶�ȡ���ص�OK
	}
	return Status;
}
/*******************************************************************************
* Function Name  : void I482_SetCommand(void)
* Description    : ˫ģģ������ָ��			            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t I482_SetCommand(uint8_t *CommandString) 
{
	uint8_t DataBuf[40];

	USART_SendStr(BT_UART,CommandString);
	USART_SendStr(BT_UART,"\r");
    
	ReadSrt(DataBuf);						//��ȡָ��ȷ��
	if(strcmp(DataBuf,"ERROR\r\n") == 0)
	{
			g_tError.UR_ErrorFlag |= 0x10;
			return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : u8 InitJOBT(void)
* Description    : ����ģ�飬�ȶ�ģ�鴮�ڲ����ʣ������Ϊ230400������Ϊ��
*                  ģ��Ϊ�µ�ģ�飬����䰴Ĭ�Ϸ�ʽ�������á�				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
// u8 InitJOBT(void)
// {
// 	u8 	DataBuf[40], *p, BT_Status=0, Status;  //��ӡ���豸����040680
// 	u32	i;	
//   
// 	for(i=0; i<3; i++)
// 	{
// 		BT_Status = BC04_ReadCommand("AT+BAUD?\r\n", DataBuf);
// 		if(BT_Status == 0)	 						//��ѯ����ģ��Ĳ�����
// 			break;
// 	}
// 	if(BT_Status ==0)		  						//���ش���
// 	{
// 		if(BluetoothStruct.Type == GCJO)
// 		{	
// 			if((strcmp(DataBuf,"+BAUD:230400\r\n") != 0) || BluetoothStruct.reInitFlag)	//�������230400�����ģ���ʼ��
// 			{
// 				for (i=0;i<10;i++)
// 				{
// 					BT_Status = BC04_SetCommand( &BC04_DefaultParameter[i][0]);//�������ģ��ָ��
// 					if(BT_Status == 1)		
// 						break;							//�����˳�
// 				}
// 			}
//   	}
// 		else if(BluetoothStruct.Type == JOBT4)
// 		{	
// 			if((strcmp(DataBuf,"+BAUD:115200\r\n") != 0) || BluetoothStruct.reInitFlag)	//�������115200�����ģ���ʼ��
// 			{
// 				for (i=0;i<8;i++)
// 				{
// 					BT_Status = BC04_SetCommand( &JOBT4_DefaultParameter[i][0]);//�������ģ��ָ��
// 					if(BT_Status == 1)		
// 						break;							//�����˳�
// 				}
// 			}
//   	}
// 	}  
//         
// 	if(BT_Status ==0)
// 	{
// 		Status = BC04_ReadCommand( "AT+LADDR?\r\n", DataBuf);	 //��ѯ����ģ��ĵ�ַ
// 		if(Status == 0)
// 		{
// 			for(i=0; i<6;i++)
// 			{
// 				BluetoothStruct.Laddr[3*i]= DataBuf[7+2*i];
// 				BluetoothStruct.Laddr[3*i+1]= DataBuf[7+2*i+1];
// 				BluetoothStruct.Laddr[3*i+2]= ':';
// 			}
// 			BluetoothStruct.Laddr[17] = 0;
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 			BluetoothStruct.Laddr[0] = 0;
// 		}
// 		Status = BC04_ReadCommand( "AT+VERSION?\r\n", DataBuf );	 //��ѯ����ģ��İ汾
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, 'B');
// 			if(p==0)
// 			{
// 				p =strchr(DataBuf, ':');
// 				strcpy(BluetoothStruct.Version, p+1);
//         p=strchr(BluetoothStruct.Version,0x0a);
//         if(p!=0)
// 				{	
//           *p=0;
//         }             
// 			}
// 			else
//       {
// 				strcpy(BluetoothStruct.Version, p);
//         p=strchr(BluetoothStruct.Version,0x0a);
//         if(p!=0)
// 				{	
//           *p=0;
// 				}	
//       }                
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 			BluetoothStruct.Version[0] = 0;
// 		}
// 		Status = BC04_ReadCommand( "AT+PASSWORD?\r\n", DataBuf );	 //��ѯ����ģ�������
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			strcpy(BluetoothStruct.Password, p+1);               
// 			p=strchr(BluetoothStruct.Password,0x0a);
// 			if(p!=0)
// 			{
// 				*p=0;
// 			}            
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 			BluetoothStruct.Password[0] = 0;
// 		}
// 		Status = BC04_ReadCommand( "AT+BIND?\r\n", DataBuf );	 //��ѯ����ģ��İ�״̬
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			p =strchr(p+1, '0');   //����鲻����Ӧ�ַ�����NULL   ���0��ʾ����ϵ�ַ��1�󶨵�ַ            
// 			if(p == NULL)          //1  �󶨵�ַ               
// 			{
// 					BluetoothStruct.Bind = 1;
// 			}
// 			else                   //������0����ʾ���󶨵�ַ
// 			{
// 					BluetoothStruct.Bind = 0;
// 			}            
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 		}
// 		Status = BC04_ReadCommand( "AT+AUTH?\r\n", DataBuf );	 //��ѯ����ģ��ļ�Ȩ״̬
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			p =strchr(p, '0');   //����鲻����Ӧ�ַ�����NULL
// 			p =strchr(DataBuf, '0');
// 			if(p == NULL)          //������1����ʾ��Ҫ��Ȩ  
// 			{
// 					BluetoothStruct.Auth = 1;
// 			}
// 			else                   //������0����ʾ����Ҫ��Ȩ
// 			{
// 					BluetoothStruct.Auth = 0;
// 			}
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 		}
// 		Status = BC04_ReadCommand( "AT+NAME?\r\n", DataBuf );	 //��ѯ����ģ�������
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			strcpy(BluetoothStruct.Name, p+1);
// 			p=strchr(BluetoothStruct.Name,0x0a);
// 			if(p!=0)
// 			{
// 					*p=0;
// 			}
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 			BluetoothStruct.Name[0] = 0;
// 		}
//     
//     if(BluetoothStruct.Type == GCJO)    
// 		{	
// 			Status = BC04_ReadCommand( "AT+CLASS?\r\n", DataBuf );	 //��ѯ����ģ��İ�״̬
// 			if(Status == 0)
// 			{
// 				p =strchr(DataBuf, ':');
// 				strcpy(BluetoothStruct.Class, p+1);
// 				p=strchr(BluetoothStruct.Class,0x0a);
// 				if(p!=0)
// 				{
// 						*p=0;
// 				}
// 			}
// 			else
// 			{
// 				BT_Status =1;
// 				BluetoothStruct.Class[0] = 0;
// 			}
// 		}
// 	}
//     return BT_Status;
// }


/*******************************************************************************
* Function Name  : u8 InitI482EBT(void)
* Description    : ��ʼ��˫ģģ�� �ȶ�ģ�鴮�ڲ����ʣ������Ϊ230400������Ϊ��
*                 ģ��Ϊ�µ�ģ�飬����䰴Ĭ�Ϸ�ʽ�������á�				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 InitI482EBT(void)    
{
	u8 	DataBuf[50], *p, BT_Status=0, Status;    //��ӡ���豸����040680
	u32	i;	  	
	u8  NameFlag=0;
  u8  BtNameStr[30]= {"AT+NAME=T801_BT_"};

	BT_Status = I482_ReadCommand("AT+BAUD?\r", DataBuf);
	if(BT_Status==1)
	{
		USART_BT_InitStructure.USART_BaudRate = 921600;              //2017.03.20  230400 ��Ϊ921600 ����һ�϶�ģ��
		USART_Init(BT_UART, &USART_BT_InitStructure); 	
  }      

  if((BT_Status== 1) || BluetoothStruct.reInitFlag)	             //�������230400�����ģ���ʼ��
	{
		NameFlag =1;
		for (i=0;i<8;i++)
		{
			BT_Status = I482_SetCommand( &I482E_DefaultParameter[i][0]);//�������ģ��ָ��
			DelayMs(30);
			if(BT_Status == 1)		
				break;							                                      //�����˳�
		}
		BtParaReloadFlag = 1;																				//2016.09.23
		SysDelay1mS(1500);		                                      //2018.02.28 ������ʱ �ò�������д������ģ��
	}  
	if(BT_Status ==0)
	{
		
    Status = BC04_ReadCommand( "AT+LBDADDR?\r", DataBuf);	       //��ѯ����ģ��ĵ�ַ
		if(Status == 0)
		{		
			for(i=0; i<6;i++)
			{
				BluetoothStruct.Laddr[3*i]= DataBuf[9+2*i];
				BluetoothStruct.Laddr[3*i+1]= DataBuf[9+2*i+1];
				BluetoothStruct.Laddr[3*i+2]= ':';
			}
			BluetoothStruct.Laddr[17] = 0;
			//2018.12.19
			BtNameStr[16]= BluetoothStruct.Laddr[12];
			BtNameStr[17]= BluetoothStruct.Laddr[13];
			BtNameStr[18]= BluetoothStruct.Laddr[15];
			BtNameStr[19]= BluetoothStruct.Laddr[16];
			BtNameStr[20]= 0x00;
			
			if(NameFlag==1)   //2018.12.19  ��������д��ģ��
			{
				Status = I482_SetCommand(BtNameStr);
				SystemDelay1mS(1500);		        //2018.12.10 ��ʱ �ǵ�ģ��������������ȷд��
			}	
			NameFlag=0;
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Laddr[0] = 0;
		}	

		Status = BC04_ReadCommand( "AT+PIN?\r", DataBuf );	 //��ѯ����ģ�������   
		if(Status == 0)
		{
			p =strchr(DataBuf, ':');
			strcpy(BluetoothStruct.Password, p+1);               
      p=strchr(BluetoothStruct.Password,0x0a);
      if(p!=0)
      {
        *p=0;
      }            
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Password[0] = 0;
		}

		Status = BC04_ReadCommand( "AT+NAME?\r", DataBuf );	 //��ѯ����ģ��İ�״̬
		if(Status == 0)
		{
			p =strchr(DataBuf, ':');
			strcpy(BluetoothStruct.Name, p+1);
      p=strchr(BluetoothStruct.Name,0x0a);
      if(p!=0)
      {
         *p=0;
      }
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Name[0] = 0;
		}

	  Status = BC04_ReadCommand( "AT+SSP?\r", DataBuf );	 //��ѯ����ģ��ļ�Ȩ״̬
		if(Status == 0)
		{
			p =strchr(DataBuf, ':');
			p =strchr(p, '0');   //����鲻����Ӧ�ַ�����NULL
      p =strchr(DataBuf, '0');
      if(p == NULL)          //������1����ʾ��Ҫ��Ȩ  
      {
         BluetoothStruct.Auth = 0;
      }
      else                   //������0����ʾ����Ҫ��Ȩ
      {
         BluetoothStruct.Auth = 1;
      }
		}
		else
		{
			BT_Status =1;
		}
		
		Status = BC04_ReadCommand( "AT+GVER?\r", DataBuf );	 //��ѯ����ģ��İ汾
		if(Status == 0)
		{
			p =strchr(DataBuf, 'B');
			if(p==0)
			{
				p =strchr(DataBuf, 'I');    //2017.03.21  ����i484Eģ��
				strcpy(BluetoothStruct.Version, p);
        p=strchr(BluetoothStruct.Version,0x0a);
        if(p!=0)
				{	
          *p=0;
        }             
			}
			else
      {
				strcpy(BluetoothStruct.Version, p);
        p=strchr(BluetoothStruct.Version,0x0a);
        if(p!=0)
				{	
          *p=0;
				}	
      }                
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Version[0] = 0;
		}
    Status = BC04_ReadCommand( "AT+CLASS?\r", DataBuf );	 //��ѯ����ģ��İ�״̬   
		if(Status == 0)
		{
			p =strchr(DataBuf, ':');
			strcpy(BluetoothStruct.Class, p+1);
      p=strchr(BluetoothStruct.Class,0x0a);
      if(p!=0)
      {
         *p=0;
      }
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Class[0] = 0;
		}
		
// 		Status = BC04_ReadCommand( "AT+BTMODE?\r", DataBuf );
	}
    return BT_Status;
}

/*******************************************************************************
* Function Name  : void Init_BT(void)
* Description    : ��ʼ������ģ�飬�ȶ�ģ�鴮�ڲ����ʣ������Ϊ115200������Ϊ��ģ��Ϊ�µ�ģ�飬
				           ����䰴Ĭ�Ϸ�ʽ�������á�				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


void Init_BT (void)
{
	uint8_t 	BT_Status=0;
	uint8_t   Temp;
	uint8_t   i;
	uint8_t 	Len,Temp1;    
	volatile FLASH_Status FLASHStatus;     //2016.10.13
	

	
	//2016.10.13  ��ȡ�ڲ�flash�����ڶ�ҳ������״̬ 
	Len=1;
	Temp1 = 0;     //Temp=1����Ҫ��дBtOnStatus��flash
 	Flash_Inside_ReadByte(&BtOnStatus,BtStatus_FLASH_ADDR, Len);
	
	if(BtOnStatus!=0xAA)				//���״̬Ϊδ������ģ��
	{
		if(g_tInterface.BT_Type == 1)        //���ڲ�������
		{
			BtOnStatus = 0xAA;
			Temp1 = 1;																 //��BtOnStatus״̬д��flash
			BluetoothStruct.reInitFlag = 1;          //��Ҫ��д��������
		}	
    else if(BtOnStatus!=0x55)
    {
			BtOnStatus = 0x55;
			Temp1 = 1;	
		}			
	}	
	else if(BtOnStatus==0xAA)										//״̬Ϊ֮ǰ������
	{
		if(g_tInterface.BT_Type != 1)        //����û������
		{
			BtOnStatus = 0x55;
			Temp1 = 1;																 //��BtOnStatus״̬д��flash
		}	
	}	
		
	if(Temp1 == 1)			//����״̬�иı�  ��дд��flash
	{	
		//���հ���д���ڲ�FLASH
		FLASH_Unlock();     //�Ƚ��Flash��
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);    //���־λ
		FLASHStatus = FLASH_ErasePage(BtStatus_FLASH_ADDR);  //ҳ����
		if (FLASHStatus == FLASH_COMPLETE)                      
		//���ҳ�����ɹ�,��ʼд��,����ֱ���˳�
		{   //��̲�У��,ÿ��д��16λ,���Ͳ��������ֽڼ���
 				FLASHStatus = Flash_Inside_BufWrite(&BtOnStatus, BtStatus_FLASH_ADDR, Len);
		}
		FLASH_Lock();  
		DelayUs(60000);
	}		
	
	if(g_tInterface.BT_Type == 0)	
	{
		return;
	}	
	DelayUs(250000);
	BluetoothStruct.Type = DMBT;	  //2016.07.22 �̶����2.0 GCJO  JOBT4 DMBT˫ģ
	
 	InitBluetoothPort();	
	InitBTUsart();
	
// 	BT_RST_HIGH;									  //��λ����ģ��
	BT_BUSY_CLR;				            //������ģ��ӿ�æ 		
	

  if(g_tInterface.BT_Type == 1)
	{	
    OpenSetupBluetooth();				  //�������÷�ʽ
  }
    
  switch(BluetoothStruct.Type)
	{		

		//���ģ��
// 		case JOBT4:
// 		case GCJO:
// 				BT_Status = InitJOBT(); 
// 				break;
		case DMBT:
        BT_Status = InitI482EBT(); 
		    break;
		default: 
				break;
	}
       	
   if(g_tInterface.BT_Type == 1)
	 { 
      CloseSetupBluetooth();					//�˳����÷�ʽ
   }
	 BluetoothStruct.InitFlag = 1;	

	if( BT_Status  )		 				//ģ��ͨѶ����
	{
		while(1)
		{
			SysDelay1mS(400);
		}
	}
}

/*******************************************************************************
* Function Name  : Read_NByte(uint8_t Length, uint8_t *DataBuf)
* Description    : ����ָ���ֽ���������ַ���
* Input          : Length���ֽ�����DataBuf���ַ�����ַ
* Output         : None
* Return         : None
*******************************************************************************/
void Read_NByte(uint32_t Length, uint8_t *DataBuf)
{
	uint32_t i;
	for(i=0; i<Length; i++)
		DataBuf[i] = ReadInBuffer();
	DataBuf[i] =0;
}

/*******************************************************************************
* Function Name  : uint8_t Judge_BT_Link(void)
* Description    : �ж������Ƿ����ӣ�����1��ʾ���ӣ�0��ʾδ����
				   BM57��GC04/BC04������ָʾ��ƽ�෴��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t Judge_BT_Link(void)
{
	u16  Count,Temp;
	static u16	BTCount=0,BTCount1=0;			//����ʱ��
	static u8   Flag = 0;
	static u8   FLAG_BT_LINK = 0;
	
	Count = 500;   //����ʱ����ʱ�� δ��
	if(BluetoothStruct.InitFlag == 0)	 			//������ʼ��δ��ɣ�����˸����ָʾ��
	{
		return 0;	
	}

	
	if(BluetoothStruct.Type == DMBT)  //˫ģ���� I482E
	{  
		if(READ_BT_LINK)
		{
			BTCount1++;
		}
		else
		{
			BTCount=BTCount1;
			BTCount1=0;
			if(BTCount>=400)                   //�����ź�  
			{
				//��ʾ��������
				FLAG_BT_LINK=1;
			}
			else if(BTCount>60 &&BTCount<200)  //�Ͽ��ź� 
			{
				FLAG_BT_LINK=0;
			}
		}     
		return FLAG_BT_LINK;
	}
	else
	{	
		return 0;
	}	
}

/*******************************************************************************
                                ��������ָ���
*******************************************************************************/

/*******************************************************************************
* Function Name  : SetCommand_1F5908
* Description    : ��������󶨵�ַ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5908( void )
{
	u8 ErrInfor;
	
	ErrInfor = 0xFE;
    
	if( BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)
			USART_SendStr(BT_UART,"AT+CLEARADDR\r\n");	
	else
			UpLoadData(&ErrInfor, 1);	//�ϴ�����
}

/*******************************************************************************
* Function Name  : SetCommand_1F5909
* Description    : ���ö�����  2020.05.28
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5909( void )
{
	uint8_t DataBuf[10],Status,Bind,Getchar;
	
	Status =1;
	
  if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4 || BluetoothStruct.Type == DMBT)
	{    
    Getchar =ReadInBuffer();	
    if(Getchar == 1)
    {
			DataBuf[0]=0x31;
    }			
		else
		{
			DataBuf[0]=0x30;
    }	
		DataBuf[1]=0;
		USART_SendStr(BT_UART,"AT+MULTICONN=");
		Status = BC04_SetCommand(DataBuf);	    
	}
  if(Status == 0)
  {
		 UpLoadData("OK", 2);	//�ϴ�����
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("���ö����ӣ�");	
		 else
				 PrintString("Set Done:");	
		 if(Getchar==1)
		 {
				PrintString("ON");
     } 
		 else
		 {
				PrintString("OFF");
     }	 
		 PrintString("\r\n");
  }
  else
  {
		 UpLoadData("ERROR", 5);	//�ϴ�����
		 PrintString("Set Failed\r\n");	
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ�� 
}
/*******************************************************************************
* Function Name  : SetCommand_1F5901
* Description    : �����������ڲ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5901( void )
{
    u8 BpsTemp;    
    BpsTemp = 0xFE;
    
    UpLoadData(&BpsTemp, 1);	//�ϴ�����				
}
/*******************************************************************************
* Function Name  : SetCommand_1F5902
* Description    : ������������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5902( void )
{
	uint8_t DataBuf[40],parameter,Status;
	uint32_t Number;
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
  
	if(Number < 4)
	{
		UpLoadData("ERROR", 5);	//�ϴ�����
		return;
	}
		
	if( BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)           //��������Ϊ4~8λ
	{
		if(Number > 8)
		{            
			Number = 8;
			DataBuf[Number] = 0;	
		}      		    		 
		USART_SendStr(BT_UART,"AT+PASSWORD=");
		Status = BC04_SetCommand(DataBuf);	      
	}
	else if( BluetoothStruct.Type == DMBT)            
	{
		if(Number > 8)
		{            
		  Number = 8;
			DataBuf[Number] = 0;	
		}      		    		 
		USART_SendStr(BT_UART,"AT+PIN=");
		Status = I482_SetCommand(DataBuf);	      
	}
  if(Status == 0)
  {
		 UpLoadData("OK", 2);	//�ϴ�����
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("�������룺");	
		 else
				 PrintString("Bluetooth Password:");	
		 PrintString(DataBuf);
		 PrintString("\r\n");
  }
  else
  {
		 UpLoadData("ERROR", 5);	//�ϴ�����
		 PrintString("Set Bluetooth Password Failed\r\n");
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}
/*******************************************************************************
* Function Name  : SetCommand_1F5903
* Description    : �������ӽ�ɫ 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5903( void )
{
	uint8_t DataBuf[40],Status,Role;  
	u8 ErrInfor;
	
	ErrInfor = 0xFE;     
  if(BluetoothStruct.Type == DMBT)    //��֧����ģʽ
	{
		UpLoadData(&ErrInfor, 1);	         //�ϴ�����
	  return;
  }    
	Read_NByte(1,DataBuf );
	DataBuf[0] |= 0x30;
	Role = DataBuf[0];
	if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)
	{     
			USART_SendStr(BT_UART,"AT+ROLE=");
			Status = BC04_SetCommand(DataBuf);	  
	}
	else
	{

	}	
	if(Status == 0)
	{
			UpLoadData("OK", 2);	//�ϴ�����
			if(g_tSysConfig.SysLanguage == 1)
			{
				PrintString("����ģʽ��");	
				if(Role == 0x30)
						PrintString("��");
				else
						PrintString("��");
			}
			else
			{
				PrintString("Bluetooth Role:");
				if(Role == 0x30)
						PrintString("Slave");
				else
						PrintString("Host");
			}           
			PrintString("\r\n");
	}
	else
	{
			UpLoadData("ERROR", 5);	//�ϴ�����
			PrintString("Set Bluetooth Role Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}

/*******************************************************************************
* Function Name  : SetCommand_1F5904
* Description    : ���õ�ַ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5904( void )
{
	uint8_t DataBuf[40],Status,Bind,ErrInfor;
    
  ErrInfor = 0xFE; 
  if( BluetoothStruct.Type == DMBT)
	{
		UpLoadData(&ErrInfor, 1);    	//�ϴ����� ��֧��
		return;
	}  
	Read_NByte(1,DataBuf );
	DataBuf[0] |= 0X30;	
	Bind = DataBuf[0];
	if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)
	{     
		USART_SendStr(BT_UART,"AT+BIND=");
		Status = BC04_SetCommand(DataBuf);
		if(Status == 0)
		{
			 Status = BC04_SetCommand("AT+CLEARADDR");	
		}    
	}
	else
	{		
	}
	if(Status == 0)
	{
		UpLoadData("OK", 2);	//�ϴ�����
		if(g_tSysConfig.SysLanguage == 1)
		{
			PrintString("������ģʽ��");	
			if(Bind == 0x30)
					PrintString("��ַ����");
			else
					PrintString("��ַ��");
		}
		else
		{
			PrintString("Bluetooth Bind:");
			if(Bind == 0x30)
					PrintString("Address is not binding");
			else
					PrintString("Address binding");
		}           
		PrintString("\r\n");
	}
	else
	{
		UpLoadData("ERROR", 5);	//�ϴ�����
		PrintString("Set Bluetooth Bind Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}
/*******************************************************************************
* Function Name  : SetCommand_1F5905
* Description    : ���������豸����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5905( void )
{
	uint8_t DataBuf[40],parameter,Status;
	uint32_t Number;
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
  if(Number > 16)                                             
	{
		Number = 16;
    DataBuf[Number] = 0;		    
	}
	if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4 || BluetoothStruct.Type == DMBT)
	{      
		USART_SendStr(BT_UART,"AT+NAME=");
		Status = BC04_SetCommand(DataBuf);	    
	}
  if(Status == 0)
  {
		 UpLoadData("OK", 2);	//�ϴ�����
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("�������ƣ�");	
		 else
				 PrintString("Bluetooth Name:");	
		 PrintString(DataBuf);
		 PrintString("\r\n");
  }
  else
  {
		 UpLoadData("ERROR", 5);	//�ϴ�����
		 PrintString("Set Bluetooth Name Failed\r\n");	
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ�� 
}
/*******************************************************************************
* Function Name  : SetCommand_1F5906
* Description    : ���������豸����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5906( void )
{
	uint8_t DataBuf[10],DataBuf1[10],BT_Class[3],Status,i,ErrInfor;
	uint32_t Number;

  ErrInfor = 0xFE;
           
	Number =ReadInBuffer();
	Read_NByte(Number,DataBuf );
  if(Number != 6)
	{
		UpLoadData("ERROR", 5);	//�ϴ�����
		return;		    
	}
  if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4 || BluetoothStruct.Type == DMBT)
	{
		if(Number != 6)
		{
			UpLoadData("ERROR", 5);     	//�ϴ�����
		  return;		    
		}      
		USART_SendStr(BT_UART,"AT+CLASS=");
		Status = BC04_SetCommand(DataBuf);    
	}
	if(Status == 0)
	{
			UpLoadData("OK", 2);	        //�ϴ�����
			if(g_tSysConfig.SysLanguage == 1)
					PrintString("�����豸���ͣ�");	
			else
					PrintString("Bluetooth Class:");	
			PrintString(DataBuf);
			PrintString("\r\n");
	}
	else
	{
			UpLoadData("ERROR", 5);	    //�ϴ�����
			PrintString("Set Bluetooth Class Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}		
/*******************************************************************************
* Function Name  : SetCommand_1F5907
* Description    : ����������������ʹ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5907( void )
{
	uint8_t DataBuf[10],DataBuf1[10],Status;

	Read_NByte(1,DataBuf );
	DataBuf[0] |= 0x30;	
  DataBuf1[0] = DataBuf[0];
  if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)
  {
		USART_SendStr(BT_UART,"AT+AUTH=");
		Status = BC04_SetCommand(DataBuf);	
		DataBuf1[0] = DataBuf[0];
		DataBuf1[1] = '\0';   
  }
	else if(BluetoothStruct.Type == DMBT)   
	{
		if(DataBuf[0] == 0x30)	
			DataBuf[0] = 0x31;
		else if(DataBuf[0] == 0x31)
			DataBuf[0]= 0x30;
		USART_SendStr(BT_UART,"AT+SSP=");
		Status = BC04_SetCommand(DataBuf);
		if(DataBuf[0] == 0x30)	
			DataBuf[0] = 0x31;
		else if(DataBuf[0] == 0x31)
			DataBuf[0]= 0x30;			
		DataBuf1[0] = DataBuf[0];
		DataBuf1[1] = '\0';   
	}
  if(Status == 0)
  {
		 UpLoadData("OK", 2);	//�ϴ�����
		 if(g_tSysConfig.SysLanguage == 1)
		 {
				 PrintString("��������ʹ�ܣ�");
				 if(DataBuf1[0] == 0x30 )
						 PrintString("��");
				 else
						 PrintString("��");
		 }
		 else
		 {
				 PrintString("Bluetooth Auth:");	
				 if(DataBuf1[0] == 0x30 )
						 PrintString("No");
				 else
						 PrintString("Yes");    
		 }
		 PrintString("\r\n");
  }
  else
  {
	  UpLoadData("ERROR", 5);	//�ϴ�����
	  PrintString("Set Bluetooth Auth Failed\r\n");
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//�ߵ�˺ֽλ��
}
/*******************************************************************************
* Function Name  : Command_1D1F
* Description    : ��������У���ַ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Command_1D1F(void)
{

	u32	CheckSum=0,Temp;
	u8	GetChar,Data[5];		//
	u8	Ch,*p,i=0;

	GetChar = ReadInBuffer();
	if(GetChar==0x00 ||GetChar==0x30)  	   //�����ӿ�
	{
		p= BluetoothStruct.Laddr;
	}
	else
	{
		p=&i;							  //��ָ��
	}
	while( *p )							  //�������У���
	{
		Ch = *p;
		if(Ch >=0x20 && Ch !=0x3A)
		{
	  		if( Ch>= 0x61)
				Ch -=0x20;
			CheckSum +=	Ch << 8*(i%4);
			i++;
		}
		p++;
	}	
	CheckSum ^=0x69505254;			//"iPRT"
	for(i=0; i<4; i++)					//��32λת��Ϊ4��8λ���ݣ��Է��㷢��
	{
		Data[i] = CheckSum >> (3-i)*8;
	}

	UpLoadData(Data, 4);					//�ϴ�����								
}
/*******************************************************************************
* Function Name  : void SetBluetooth(u8 FCommand,u8 SCommand)
* Description    : ������������
* Input          : Command������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
void SetBluetooth(u8 FCommand,u8 SCommand)
{
    u8 GetChar,CmdData[40],i;
    
    if( FCommand == 0x1b )
    {    
        switch( SCommand )
        {
//             case 0x0d:
//             {
//                 SetCommand_1F5908();	//����󶨵�ַ
// 								Set_BTWFflg = 1;
//                 break;
//             }
            case 0x0f:
            {
                SetCommand_1F5901();	   //���ò�����
                break;
            }
            case 0x10:			
            {
                SetCommand_1F5902();		//��������
							Set_BTWFflg = 1;
                break;
            }
            case 0x11:			
            {
                SetCommand_1F5903();		//�������ӽ�ɫ
							Set_BTWFflg = 1;
                break;
            }
            case 0x12:			
            {
                SetCommand_1F5904();		//���õ�ַ��
							Set_BTWFflg = 1;
                break;
            }
            case 0x13:			
            {
                SetCommand_1F5905();		//�����豸����
							 Set_BTWFflg = 1;
                break;
            }
            case 0x14:			
            {
                SetCommand_1F5906();		//�����豸����
							Set_BTWFflg = 1;
                break;
            }
            case 0x27:			
            {
                SetCommand_1F5907();		//������������ʹ��
							Set_BTWFflg = 1;
                break;
            }
            default:                
                break;
        }
   }
	 else if(FCommand ==0x1F)
	 {	 
		  if(SCommand == 0x59)
			{	
				  GetChar =ReadInBuffer();		     //2016.08.13  ��ȡ�ڶ�λ����
					switch( GetChar )
					{						
							case 0x01:
							{
									SetCommand_1F5901();	   //���ò�����
									break;
							}
							case 0x02:			
							{
									SetCommand_1F5902();		//��������
								  Set_BTWFflg = 1;
									break;
							}
							case 0x03:			
							{
									SetCommand_1F5903();		//�������ӽ�ɫ
								Set_BTWFflg = 1;
									break;
							}
							case 0x04:			
							{
									SetCommand_1F5904();		//���õ�ַ��
								Set_BTWFflg = 1;
									break;
							}
							case 0x05:			
							{
									SetCommand_1F5905();		//�����豸����
								Set_BTWFflg = 1;
									break;
							}
							case 0x06:			
							{
									SetCommand_1F5906();		//�����豸����
								Set_BTWFflg = 1;
									break;
							}
							case 0x07:			
							{
									SetCommand_1F5907();		//������������ʹ��
								Set_BTWFflg = 1;
									break;
							}
							case 0x08:
							{
									SetCommand_1F5908();	//�������ģʽ�󶨵�ַ
									break;
							}
							case 0x09:
							{
									SetCommand_1F5909();	//���ö�����
									break;
							}
							default:                
									break;
					}
		  }
			else
			{
		
			}	
	 }	 
   else if(FCommand =='A' && (BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4))	  //Ԥ������ATָ��
	 {
			CmdData[0]=	FCommand;
			CmdData[1]=	SCommand;
			for(i=1; i<36; i++)
			{
				CmdData[i] = ReadInBuffer();
				if(CmdData[i] ==0x0a)
				{
					CmdData[i+1] =0;
					break;
				}
			}
			if(i<36)
			{
				if(strchr(CmdData,'?'))		
				{
					BC04_ReadCommand(CmdData,CmdData);		//������ָ��
					UpLoadData(CmdData, strlen(CmdData));	//�ϴ�����
				}
				else
				{
					BC04_SetCommand(CmdData);	//����ָ��
				}
			}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
