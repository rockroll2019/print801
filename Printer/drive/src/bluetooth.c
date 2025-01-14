/**
  ******************************************************************************
  * @file    x.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2016-3-18
  * @brief   BLUETOOTH相关程序。
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

//2.0 4.0双模蓝牙模块
u8	*I482E_DefaultParameter[] ={        
 								//"AT+BIND=0",				    //地址绑定，＝0不绑定

 								//"AT+CLEARADDR",				 //清除绑定地址
 								//"AT+ROLE=0",				   //设备角色（主从模式），0从模式
								"AT+SSP=0",				       //是否鉴权，＝0鉴权�
								"AT+NAME=T801_BT_Printer",	//名称  
 								"AT+CLASS=040680",	        //类型
								"AT+PIN=1234",			        //密码  
								"AT+BAUD=115200",			      //波特率 2016.09.23 改为115200
	              "AT+BTMODE=1",			        //2016.09.14 设置静默模式
	              "AT+MULTICONN=0",          //2020.05.28  关闭多连接
	              "AT+IDLE=10000",				 //ivt的BR2141es 不能设置为0取消休眠
                                
  };

//金瓯3.0BT模块
// u8	*BC04_DefaultParameter[] ={
// 								"AT+BIND=0",				   //地址绑定，＝0不绑定

// 								"AT+CLEARADDR",				 //清除绑定地址
// 								"AT+ROLE=0",				   //设备角色（主从模式），0从模式
// 								"AT+AUTH=1",				   //是否鉴权，＝1鉴权

// 								"AT+NAME=T801_BT_Printer",	   //名称   
// 								"AT+CLASS=040680",			       //类型
// 								"AT+PASSWORD=1234",			       //密码
// 								"AT+BAUD=230400",			         //波特率
// 	              "AT+LINKPLCY=1,6,6,0,300",     //提高蓝牙传输
// 	              "AT+SNIFF=0,0,0,0",					   //提高蓝牙传输 
// 	};

// u8	*JOBT4_DefaultParameter[] ={
// 								"AT+BIND=0",				   //地址绑定，＝0不绑定

// 								"AT+CLEARADDR",				 //清除绑定地址
// 								"AT+ROLE=0",				   //设备角色（主从模式），0从模式
// 								"AT+AUTH=1",				   //是否鉴权，＝1鉴权

// 								"AT+NAME=T801_BT_Printer",	   //名称   
// 								//"AT+CLASS=040680",			     //类型
// 								"AT+PASSWORD=123456",			     //密码
// 								"AT+BAUD=115200",			         //波特率 115200
// 	              "AT+LINKPLCY=1,6,6,0,300",     //提高蓝牙传输
// 	};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*Extern Private functions ---------------------------------------------------------*/
extern void DelayUs(u32 DelayNumber);
extern u8	ReadInBuffer( void );
extern void SPI_FLASH_BlockErase(u32 SectorAddr); 
extern void UpLoadData(u8 *DataBuf, u32 Length);				//上传数据
extern void PrintString( u8 *Str );
extern void GoDotLine(u32 DotLineNumber);
/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param key: The key pressed
  * @retval : 1: Correct
  *   0: Error
  */
	
/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化串口控制管脚
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitBluetoothPort(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/************* 设置蓝牙串口端口 ****************************/
	RCC_APB2PeriphClockCmd(BT_RCC_APB2Periph_GPIO, ENABLE);
	RCC_APB1PeriphClockCmd(BT_RCC_APB1Periph_USART, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);   //重映射串口2
	
	GPIO_InitStructure.GPIO_Pin		=	BT_UART_RX;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;	//串口复用输入  
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		=	BT_UART_TX;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;		   //串口复用输出
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  BT_UART_CTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  			
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= BT_UART_RTS;	    	   //对蓝牙置忙
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
  GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	GPIO_SetBits(BT_UART_PORT, BT_UART_RTS );							 //设置为忙
	
	GPIO_InitStructure.GPIO_Pin =  BT_WIFI_TYPE ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //初始化为浮空
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
	GPIO_ResetBits(BT_UART_PORT, BT_RST );							  //设置低
	
	GPIO_InitStructure.GPIO_Pin = BT_SET ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BT_UART_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(BT_UART_PORT, BT_SET );								//设置低
}

/*******************************************************************************
* Function Name  : 函数名
* Description    : 初始化蓝牙串口
* Input          : 输入参数
* Output         : 输出参数
* Return         : 返回参数
*******************************************************************************/
void	InitBTUsart(void)
{	
	RCC_APB1PeriphClockCmd(BT_RCC_APB1Periph_USART, ENABLE);		  //使能串口时钟

	USART_BT_InitStructure.USART_BaudRate = 115200;               
	USART_BT_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_BT_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_BT_InitStructure.USART_Parity = USART_Parity_No;
	USART_BT_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_BT_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(BT_UART, &USART_BT_InitStructure);
	USART_Cmd(BT_UART, ENABLE);	  
	USART_ITConfig(BT_UART, USART_IT_RXNE, DISABLE);		          //关闭BT—USART接收中断DISABLE
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
					  BtTimeOut =1;   //2016.09.14  超时
            return 1;
        }
    }
}

/*******************************************************************************
* Function Name  : void ReadSrt(u8 *Str)
* Description    : 从输入缓冲区读取以0A结束的字符串,字符串最大长度为255字符
* Input          : Srt:字符串地址
* Output         : None
* Return         : None
*******************************************************************************/
void ReadSrt( u8 *Str )
{
	u8	ch;
	u32 i;

	while(!SerialKeyTimeout(&ch));			 //等待接收到字符
	if(ch ==0x0d)							 //去除掉开始的“0D、0A”
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
* Description    : 从指定的串口发送一字符串
* Input          : USARTX，串口号，Srt:字符串地址
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
* Description    : 进入蓝牙设置模式
				           调用这个函数的前提是确保有蓝牙	
* Input          : None
* Output         : None
* Return         : 0 = 成功， 1 = 失败
*******************************************************************************/
uint8_t OpenSetupBluetooth(void)
{
	uint8_t DataBuf[40];	

	USART_ITConfig(BT_UART, USART_IT_RXNE, DISABLE);		

	
	if(BluetoothStruct.Type == DMBT)                                     //双模模块设置模式波特率固定为230400
	{
		 USART_BT_InitStructure.USART_BaudRate = 115200;						//2016.09.23
	}
	USART_Init(BT_UART, &USART_BT_InitStructure);
		

	

	
	if(BluetoothStruct.Type == DMBT) 				//双模I482E
	{
		BT_RST_LOW;
		SysDelay1mS(20);     						//置低IO后需要复位模块进入设置模式
		BT_RST_HIGH; 
		SysDelay1mS(2000);
	}
  else                                                //其他模块
	{
		BT_RST_LOW;
 		SysDelay1mS(20);     						//置低IO后需要复位模块进入设置模式
 		BT_RST_HIGH; 
    SysDelay1mS(3000);
	}
	return 0;											   
}

/*******************************************************************************
* Function Name  : 	CloseSetupBluetooth
* Description    : 退出蓝牙设置模式
* Input          : None
* Output         : None
* Return         : 0 = 成功， 1 = 失败
*******************************************************************************/
uint8_t CloseSetupBluetooth(void)
{

	
	 if(BluetoothStruct.Type == DMBT)		//2016.09.21
	 {
		  if(BtParaReloadFlag==1)
			{
        BtParaReloadFlag = 0;							
				BT_RST_LOW;
		    SysDelay1mS(20);     						//置低IO后需要复位模块进入设置模式
		    BT_RST_HIGH; 
		    SysDelay1mS(1000);
			}	
			USART_BT_InitStructure.USART_BaudRate = 115200;
	 }
	 else
	 {
			USART_BT_InitStructure.USART_BaudRate = 230400;
   }	 
	 USART_Init(BT_UART, &USART_BT_InitStructure); 			  //串口设置
	 USART_ITConfig(BT_UART, USART_IT_RXNE, ENABLE);		  //打开USART1接收中断
	 return 0;										
}


/*******************************************************************************
* Function Name  : uint32_t BC04_ReadCommand
* Description    : 读蓝牙模块参数.引入的旧式工作方式，后续需调整
* Input          : CommandString：指令字符串，Buffer：返回的参数字符串指针
* Output         : Buffer：返回的参数
* Return         : =1 error ,=0 ok    2014.06.18
*******************************************************************************/
uint8_t BC04_ReadCommand(uint8_t *CommandString,uint8_t *Buffer)
{
	uint8_t DataBuf[400];
	uint8_t Status =0;

	USART_SendStr(BT_UART,CommandString);

	ReadSrt(Buffer);						 //读取指令确认

	if(strcmp(Buffer,"ERROR\r\n") == 0)
	{
		Status = 1;
    g_tError.UR_ErrorFlag |= 0x10;
	}
	else
	{
		if(strcmp(Buffer,"OK\r\n") == 0)
			ReadSrt(Buffer);			//OK在前,读取返回的参数
		else
			ReadSrt(DataBuf);			//OK在后，读取返回的OK
	}
	return Status;
}
/*******************************************************************************
* Function Name  : uint32_t BC04_SetCommand
* Description    : 给蓝牙模块发送设置指令，兼容金瓯模块
* Input          : CommandString：指令字符串
* Output         : None
* Return         : =1 error ,=0 ok   2014.06.18
*******************************************************************************/
uint8_t BC04_SetCommand(uint8_t *CommandString)
{
	uint8_t DataBuf[40];

	USART_SendStr(BT_UART,CommandString);
  USART_SendStr(BT_UART,"\r\n");
    
	ReadSrt(DataBuf);						//读取指令确认
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
* Description    : 通过信号线电平判断蓝牙模块型号
				           BluetoothStruct.Type 1,JOBT4 4.0模块；2,GCJO 3.0模块；3,DMBT,I482ES双模模块；4,DLBT,一拖多BT模块	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Judge_BT_Type(void)
{
	uint32_t	i;
	uint8_t  Temp;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	for(i=0;i<80;i++)				   			//等待金瓯模块复位完成
	{
		DelayMs(20);								  //此处未开滴答定时器 不用SysDelay1mS函数
		if(!READ_BT_CTS)						  //如果有金瓯模块，则模块复位后BT_CTS信号为低  初始化时为上拉输入
		{	
			break;
		}
	}
  if(!READ_BT_CTS)								      //CTS为低电平			
	{				
    g_tInterface.BT_Type = 1;			      //有蓝牙模块	
	}
	else										              //
	{	
    g_tInterface.BT_Type = 0;
	}
}
/*******************************************************************************
* Function Name  : void BTDMReadSrt(void)
* Description    : 双模模块通信读取字符串			            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 BTDMReadSrt( u8 *Str )         
{
	u8	ch;
	u32 i,j=0;

	while(!SerialKeyTimeout(&ch))			 //等待接收到字符
  {
		j++;
		if(j>=100)
			return 1;
  }
	if(ch ==0x0d)							        //去除掉开始的“0D、0A”
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
	if(BtTimeOut==1)  //2016.09.14 蓝牙超时处理
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
* Description    : 双模模块读取指令
				            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t I482_ReadCommand(uint8_t *CommandString,uint8_t *Buffer)  
{
	uint8_t DataBuf[400];
	uint8_t Status =0;

	USART_SendStr(BT_UART,CommandString);

	Status = BTDMReadSrt(Buffer);						 //读取指令确认
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
			ReadSrt(Buffer);			//OK在前,读取返回的参数
		else
			ReadSrt(DataBuf);			//OK在后，读取返回的OK
	}
	return Status;
}
/*******************************************************************************
* Function Name  : void I482_SetCommand(void)
* Description    : 双模模块设置指令			            	
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t I482_SetCommand(uint8_t *CommandString) 
{
	uint8_t DataBuf[40];

	USART_SendStr(BT_UART,CommandString);
	USART_SendStr(BT_UART,"\r");
    
	ReadSrt(DataBuf);						//读取指令确认
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
* Description    : 蓝牙模块，先读模块串口波特率，如果不为230400，则认为该
*                  模块为新的模块，则对其按默认方式进行设置。				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
// u8 InitJOBT(void)
// {
// 	u8 	DataBuf[40], *p, BT_Status=0, Status;  //打印机设备类型040680
// 	u32	i;	
//   
// 	for(i=0; i<3; i++)
// 	{
// 		BT_Status = BC04_ReadCommand("AT+BAUD?\r\n", DataBuf);
// 		if(BT_Status == 0)	 						//查询蓝牙模块的波特率
// 			break;
// 	}
// 	if(BT_Status ==0)		  						//返回错误
// 	{
// 		if(BluetoothStruct.Type == GCJO)
// 		{	
// 			if((strcmp(DataBuf,"+BAUD:230400\r\n") != 0) || BluetoothStruct.reInitFlag)	//如果不是230400，则对模块初始化
// 			{
// 				for (i=0;i<10;i++)
// 				{
// 					BT_Status = BC04_SetCommand( &BC04_DefaultParameter[i][0]);//输出蓝牙模块指令
// 					if(BT_Status == 1)		
// 						break;							//出错退出
// 				}
// 			}
//   	}
// 		else if(BluetoothStruct.Type == JOBT4)
// 		{	
// 			if((strcmp(DataBuf,"+BAUD:115200\r\n") != 0) || BluetoothStruct.reInitFlag)	//如果不是115200，则对模块初始化
// 			{
// 				for (i=0;i<8;i++)
// 				{
// 					BT_Status = BC04_SetCommand( &JOBT4_DefaultParameter[i][0]);//输出蓝牙模块指令
// 					if(BT_Status == 1)		
// 						break;							//出错退出
// 				}
// 			}
//   	}
// 	}  
//         
// 	if(BT_Status ==0)
// 	{
// 		Status = BC04_ReadCommand( "AT+LADDR?\r\n", DataBuf);	 //查询蓝牙模块的地址
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
// 		Status = BC04_ReadCommand( "AT+VERSION?\r\n", DataBuf );	 //查询蓝牙模块的版本
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
// 		Status = BC04_ReadCommand( "AT+PASSWORD?\r\n", DataBuf );	 //查询蓝牙模块的密码
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
// 		Status = BC04_ReadCommand( "AT+BIND?\r\n", DataBuf );	 //查询蓝牙模块的绑定状态
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			p =strchr(p+1, '0');   //如果查不到对应字符返回NULL   金瓯0表示不邦迪地址，1绑定地址            
// 			if(p == NULL)          //1  绑定地址               
// 			{
// 					BluetoothStruct.Bind = 1;
// 			}
// 			else                   //参数‘0’表示不绑定地址
// 			{
// 					BluetoothStruct.Bind = 0;
// 			}            
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 		}
// 		Status = BC04_ReadCommand( "AT+AUTH?\r\n", DataBuf );	 //查询蓝牙模块的鉴权状态
// 		if(Status == 0)
// 		{
// 			p =strchr(DataBuf, ':');
// 			p =strchr(p, '0');   //如果查不到对应字符返回NULL
// 			p =strchr(DataBuf, '0');
// 			if(p == NULL)          //参数‘1’表示需要鉴权  
// 			{
// 					BluetoothStruct.Auth = 1;
// 			}
// 			else                   //参数‘0’表示不需要鉴权
// 			{
// 					BluetoothStruct.Auth = 0;
// 			}
// 		}
// 		else
// 		{
// 			BT_Status =1;
// 		}
// 		Status = BC04_ReadCommand( "AT+NAME?\r\n", DataBuf );	 //查询蓝牙模块的名称
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
// 			Status = BC04_ReadCommand( "AT+CLASS?\r\n", DataBuf );	 //查询蓝牙模块的绑定状态
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
* Description    : 初始化双模模块 先读模块串口波特率，如果不为230400，则认为该
*                 模块为新的模块，则对其按默认方式进行设置。				
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 InitI482EBT(void)    
{
	u8 	DataBuf[50], *p, BT_Status=0, Status;    //打印机设备类型040680
	u32	i;	  	
	u8  NameFlag=0;
  u8  BtNameStr[30]= {"AT+NAME=T801_BT_"};

	BT_Status = I482_ReadCommand("AT+BAUD?\r", DataBuf);
	if(BT_Status==1)
	{
		USART_BT_InitStructure.USART_BaudRate = 921600;              //2017.03.20  230400 改为921600 兼容一拖多模块
		USART_Init(BT_UART, &USART_BT_InitStructure); 	
  }      

  if((BT_Status== 1) || BluetoothStruct.reInitFlag)	             //如果不是230400，则对模块初始化
	{
		NameFlag =1;
		for (i=0;i<8;i++)
		{
			BT_Status = I482_SetCommand( &I482E_DefaultParameter[i][0]);//输出蓝牙模块指令
			DelayMs(30);
			if(BT_Status == 1)		
				break;							                                      //出错退出
		}
		BtParaReloadFlag = 1;																				//2016.09.23
		SysDelay1mS(1500);		                                      //2018.02.28 增加延时 让参数正常写入蓝牙模块
	}  
	if(BT_Status ==0)
	{
		
    Status = BC04_ReadCommand( "AT+LBDADDR?\r", DataBuf);	       //查询蓝牙模块的地址
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
			
			if(NameFlag==1)   //2018.12.19  蓝牙名称写入模块
			{
				Status = I482_SetCommand(BtNameStr);
				SystemDelay1mS(1500);		        //2018.12.10 延时 是的模块内蓝牙名称正确写入
			}	
			NameFlag=0;
		}
		else
		{
			BT_Status =1;
			BluetoothStruct.Laddr[0] = 0;
		}	

		Status = BC04_ReadCommand( "AT+PIN?\r", DataBuf );	 //查询蓝牙模块的密码   
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

		Status = BC04_ReadCommand( "AT+NAME?\r", DataBuf );	 //查询蓝牙模块的绑定状态
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

	  Status = BC04_ReadCommand( "AT+SSP?\r", DataBuf );	 //查询蓝牙模块的鉴权状态
		if(Status == 0)
		{
			p =strchr(DataBuf, ':');
			p =strchr(p, '0');   //如果查不到对应字符返回NULL
      p =strchr(DataBuf, '0');
      if(p == NULL)          //参数‘1’表示需要鉴权  
      {
         BluetoothStruct.Auth = 0;
      }
      else                   //参数‘0’表示不需要鉴权
      {
         BluetoothStruct.Auth = 1;
      }
		}
		else
		{
			BT_Status =1;
		}
		
		Status = BC04_ReadCommand( "AT+GVER?\r", DataBuf );	 //查询蓝牙模块的版本
		if(Status == 0)
		{
			p =strchr(DataBuf, 'B');
			if(p==0)
			{
				p =strchr(DataBuf, 'I');    //2017.03.21  兼容i484E模块
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
    Status = BC04_ReadCommand( "AT+CLASS?\r", DataBuf );	 //查询蓝牙模块的绑定状态   
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
* Description    : 初始化蓝牙模块，先读模块串口波特率，如果不为115200，则认为该模块为新的模块，
				           则对其按默认方式进行设置。				
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
	

	
	//2016.10.13  读取内部flash倒数第二页内蓝牙状态 
	Len=1;
	Temp1 = 0;     //Temp=1则需要重写BtOnStatus到flash
 	Flash_Inside_ReadByte(&BtOnStatus,BtStatus_FLASH_ADDR, Len);
	
	if(BtOnStatus!=0xAA)				//存的状态为未接蓝牙模块
	{
		if(g_tInterface.BT_Type == 1)        //现在插了蓝牙
		{
			BtOnStatus = 0xAA;
			Temp1 = 1;																 //把BtOnStatus状态写入flash
			BluetoothStruct.reInitFlag = 1;          //需要重写蓝牙参数
		}	
    else if(BtOnStatus!=0x55)
    {
			BtOnStatus = 0x55;
			Temp1 = 1;	
		}			
	}	
	else if(BtOnStatus==0xAA)										//状态为之前有蓝牙
	{
		if(g_tInterface.BT_Type != 1)        //现在没有蓝牙
		{
			BtOnStatus = 0x55;
			Temp1 = 1;																 //把BtOnStatus状态写入flash
		}	
	}	
		
	if(Temp1 == 1)			//蓝牙状态有改变  重写写入flash
	{	
		//按照半字写入内部FLASH
		FLASH_Unlock();     //先解除Flash锁
		FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);    //清标志位
		FLASHStatus = FLASH_ErasePage(BtStatus_FLASH_ADDR);  //页擦除
		if (FLASHStatus == FLASH_COMPLETE)                      
		//如果页擦除成功,开始写入,否则直接退出
		{   //编程并校验,每次写入16位,传送参数按照字节计算
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
	BluetoothStruct.Type = DMBT;	  //2016.07.22 固定金瓯2.0 GCJO  JOBT4 DMBT双模
	
 	InitBluetoothPort();	
	InitBTUsart();
	
// 	BT_RST_HIGH;									  //复位蓝牙模块
	BT_BUSY_CLR;				            //清蓝牙模块接口忙 		
	

  if(g_tInterface.BT_Type == 1)
	{	
    OpenSetupBluetooth();				  //进入设置方式
  }
    
  switch(BluetoothStruct.Type)
	{		

		//金瓯模块
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
      CloseSetupBluetooth();					//退出设置方式
   }
	 BluetoothStruct.InitFlag = 1;	

	if( BT_Status  )		 				//模块通讯错误
	{
		while(1)
		{
			SysDelay1mS(400);
		}
	}
}

/*******************************************************************************
* Function Name  : Read_NByte(uint8_t Length, uint8_t *DataBuf)
* Description    : 读入指令字节数据组成字符串
* Input          : Length：字节数，DataBuf：字符串地址
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
* Description    : 判断蓝牙是否链接，返回1表示链接，0表示未链接
				   BM57和GC04/BC04的链接指示电平相反。
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t Judge_BT_Link(void)
{
	u16  Count,Temp;
	static u16	BTCount=0,BTCount1=0;			//计数时间
	static u8   Flag = 0;
	static u8   FLAG_BT_LINK = 0;
	
	Count = 500;   //休眠时链接时间 未用
	if(BluetoothStruct.InitFlag == 0)	 			//蓝牙初始化未完成，不闪烁链接指示灯
	{
		return 0;	
	}

	
	if(BluetoothStruct.Type == DMBT)  //双模蓝牙 I482E
	{  
		if(READ_BT_LINK)
		{
			BTCount1++;
		}
		else
		{
			BTCount=BTCount1;
			BTCount1=0;
			if(BTCount>=400)                   //链接信号  
			{
				//表示链接上了
				FLAG_BT_LINK=1;
			}
			else if(BTCount>60 &&BTCount<200)  //断开信号 
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
                                蓝牙设置指令部分
*******************************************************************************/

/*******************************************************************************
* Function Name  : SetCommand_1F5908
* Description    : 清除蓝牙绑定地址
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
			UpLoadData(&ErrInfor, 1);	//上传数据
}

/*******************************************************************************
* Function Name  : SetCommand_1F5909
* Description    : 设置多连接  2020.05.28
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
		 UpLoadData("OK", 2);	//上传数据
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("设置多链接：");	
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
		 UpLoadData("ERROR", 5);	//上传数据
		 PrintString("Set Failed\r\n");	
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置 
}
/*******************************************************************************
* Function Name  : SetCommand_1F5901
* Description    : 设置蓝牙串口参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5901( void )
{
    u8 BpsTemp;    
    BpsTemp = 0xFE;
    
    UpLoadData(&BpsTemp, 1);	//上传数据				
}
/*******************************************************************************
* Function Name  : SetCommand_1F5902
* Description    : 设置蓝牙密码
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
		UpLoadData("ERROR", 5);	//上传数据
		return;
	}
		
	if( BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4)           //蓝牙密码为4~8位
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
		 UpLoadData("OK", 2);	//上传数据
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("蓝牙密码：");	
		 else
				 PrintString("Bluetooth Password:");	
		 PrintString(DataBuf);
		 PrintString("\r\n");
  }
  else
  {
		 UpLoadData("ERROR", 5);	//上传数据
		 PrintString("Set Bluetooth Password Failed\r\n");
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置
}
/*******************************************************************************
* Function Name  : SetCommand_1F5903
* Description    : 设置主从角色 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommand_1F5903( void )
{
	uint8_t DataBuf[40],Status,Role;  
	u8 ErrInfor;
	
	ErrInfor = 0xFE;     
  if(BluetoothStruct.Type == DMBT)    //不支持主模式
	{
		UpLoadData(&ErrInfor, 1);	         //上传数据
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
			UpLoadData("OK", 2);	//上传数据
			if(g_tSysConfig.SysLanguage == 1)
			{
				PrintString("蓝牙模式：");	
				if(Role == 0x30)
						PrintString("从");
				else
						PrintString("主");
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
			UpLoadData("ERROR", 5);	//上传数据
			PrintString("Set Bluetooth Role Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置
}

/*******************************************************************************
* Function Name  : SetCommand_1F5904
* Description    : 设置地址绑定
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
		UpLoadData(&ErrInfor, 1);    	//上传数据 不支持
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
		UpLoadData("OK", 2);	//上传数据
		if(g_tSysConfig.SysLanguage == 1)
		{
			PrintString("蓝牙绑定模式：");	
			if(Bind == 0x30)
					PrintString("地址不绑定");
			else
					PrintString("地址绑定");
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
		UpLoadData("ERROR", 5);	//上传数据
		PrintString("Set Bluetooth Bind Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置
}
/*******************************************************************************
* Function Name  : SetCommand_1F5905
* Description    : 设置蓝牙设备名称
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
		 UpLoadData("OK", 2);	//上传数据
		 if(g_tSysConfig.SysLanguage == 1)
				 PrintString("蓝牙名称：");	
		 else
				 PrintString("Bluetooth Name:");	
		 PrintString(DataBuf);
		 PrintString("\r\n");
  }
  else
  {
		 UpLoadData("ERROR", 5);	//上传数据
		 PrintString("Set Bluetooth Name Failed\r\n");	
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置 
}
/*******************************************************************************
* Function Name  : SetCommand_1F5906
* Description    : 设置蓝牙设备类型
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
		UpLoadData("ERROR", 5);	//上传数据
		return;		    
	}
  if(BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4 || BluetoothStruct.Type == DMBT)
	{
		if(Number != 6)
		{
			UpLoadData("ERROR", 5);     	//上传数据
		  return;		    
		}      
		USART_SendStr(BT_UART,"AT+CLASS=");
		Status = BC04_SetCommand(DataBuf);    
	}
	if(Status == 0)
	{
			UpLoadData("OK", 2);	        //上传数据
			if(g_tSysConfig.SysLanguage == 1)
					PrintString("蓝牙设备类型：");	
			else
					PrintString("Bluetooth Class:");	
			PrintString(DataBuf);
			PrintString("\r\n");
	}
	else
	{
			UpLoadData("ERROR", 5);	    //上传数据
			PrintString("Set Bluetooth Class Failed\r\n");
	}
	GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置
}		
/*******************************************************************************
* Function Name  : SetCommand_1F5907
* Description    : 设置设置蓝牙密码使能
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
		 UpLoadData("OK", 2);	//上传数据
		 if(g_tSysConfig.SysLanguage == 1)
		 {
				 PrintString("蓝牙密码使能：");
				 if(DataBuf1[0] == 0x30 )
						 PrintString("否");
				 else
						 PrintString("是");
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
	  UpLoadData("ERROR", 5);	//上传数据
	  PrintString("Set Bluetooth Auth Failed\r\n");
  }
  GoDotLine(CUT_OFFSET + NO_CUT_OFFSET);			//走到撕纸位置
}
/*******************************************************************************
* Function Name  : Command_1D1F
* Description    : 返回蓝牙校验地址
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
	if(GetChar==0x00 ||GetChar==0x30)  	   //蓝牙接口
	{
		p= BluetoothStruct.Laddr;
	}
	else
	{
		p=&i;							  //空指针
	}
	while( *p )							  //计算分组校验和
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
	for(i=0; i<4; i++)					//将32位转换为4个8位数据，以方便发送
	{
		Data[i] = CheckSum >> (3-i)*8;
	}

	UpLoadData(Data, 4);					//上传数据								
}
/*******************************************************************************
* Function Name  : void SetBluetooth(u8 FCommand,u8 SCommand)
* Description    : 设置蓝牙参数
* Input          : Command：设置指令
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
//                 SetCommand_1F5908();	//清除绑定地址
// 								Set_BTWFflg = 1;
//                 break;
//             }
            case 0x0f:
            {
                SetCommand_1F5901();	   //设置波特率
                break;
            }
            case 0x10:			
            {
                SetCommand_1F5902();		//设置密码
							Set_BTWFflg = 1;
                break;
            }
            case 0x11:			
            {
                SetCommand_1F5903();		//设置主从角色
							Set_BTWFflg = 1;
                break;
            }
            case 0x12:			
            {
                SetCommand_1F5904();		//设置地址绑定
							Set_BTWFflg = 1;
                break;
            }
            case 0x13:			
            {
                SetCommand_1F5905();		//设置设备名称
							 Set_BTWFflg = 1;
                break;
            }
            case 0x14:			
            {
                SetCommand_1F5906();		//设置设备类型
							Set_BTWFflg = 1;
                break;
            }
            case 0x27:			
            {
                SetCommand_1F5907();		//设置蓝牙密码使能
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
				  GetChar =ReadInBuffer();		     //2016.08.13  读取第二位参数
					switch( GetChar )
					{						
							case 0x01:
							{
									SetCommand_1F5901();	   //设置波特率
									break;
							}
							case 0x02:			
							{
									SetCommand_1F5902();		//设置密码
								  Set_BTWFflg = 1;
									break;
							}
							case 0x03:			
							{
									SetCommand_1F5903();		//设置主从角色
								Set_BTWFflg = 1;
									break;
							}
							case 0x04:			
							{
									SetCommand_1F5904();		//设置地址绑定
								Set_BTWFflg = 1;
									break;
							}
							case 0x05:			
							{
									SetCommand_1F5905();		//设置设备名称
								Set_BTWFflg = 1;
									break;
							}
							case 0x06:			
							{
									SetCommand_1F5906();		//设置设备类型
								Set_BTWFflg = 1;
									break;
							}
							case 0x07:			
							{
									SetCommand_1F5907();		//设置蓝牙密码使能
								Set_BTWFflg = 1;
									break;
							}
							case 0x08:
							{
									SetCommand_1F5908();	//清除蓝牙模式绑定地址
									break;
							}
							case 0x09:
							{
									SetCommand_1F5909();	//设置多连接
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
   else if(FCommand =='A' && (BluetoothStruct.Type == GCJO || BluetoothStruct.Type == JOBT4))	  //预留蓝牙AT指令
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
					BC04_ReadCommand(CmdData,CmdData);		//读参数指令
					UpLoadData(CmdData, strlen(CmdData));	//上传数据
				}
				else
				{
					BC04_SetCommand(CmdData);	//设置指令
				}
			}
	}
}

/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
