/******************** (C) COPYRIGHT 2010 Spirit ********************
* File Name          : ������.c
* Author             : Spirit R&D Team
* Version            : �汾
* Date               : ����
* Description        : ����
********************************************************************************
*����˵�� 
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32_eth.h"
//#include "ExtGlobalVariable.h"
#include "ETH_config.h"

#include "uip_timer.h"
#include "uip_arp.h"
#include "DefConstant.h"
#include "extgvar.h"
#include "cutter.h"
//#include "gvar.h"
extern uint8_t	g_bSleepModeFlag;					   				//�������߱�־ 
extern volatile uint8_t	g_bRoutinFlag;						//T2�жϱ�־ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUF        ((struct uip_eth_hdr *)uip_buf)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct timer periodic_timer, arp_timer;
uint8_t IP_ChangeFlag =0;			//������IP��ַ
uint8_t emac_rcv_buf[UIP_BUFSIZE]; //��̫�����ջ�����

uint8_t *uip_buf;
uint8_t ETH_ConnectState=0;
volatile uint32_t SYS_TICK=0 ;

/* Private function prototypes -----------------------------------------------*/
void TransmitPacket(void);
void LoadNetworkAddr(void);
void ETH_SelfTest(void);
void ReadNetworkAddr ( uip_ipaddr_t *ipaddr, struct uip_eth_addr *macaddr);
void uIP_Pkt_Handle(void);
/* Extern  variables ---------------------------------------------------------*/
extern uint8_t Eth_ClearBusy;//08
#define ETH_TX_DATA_SIZE 256  //08
uint8_t ETH_Tx_Buffer[ETH_TX_DATA_SIZE]; 
APP_Tx_Struct ETH_Tx={ETH_TX_DATA_SIZE,0,0,0,0,ETH_Tx_Buffer};
extern ETH_InitTypeDef ETH_InitStructure;

/* Extern function prototypes ------------------------------------------------*/
extern void ArpRequest(void);
extern void http_server(void);
extern void PrinterDataServer(void );
extern void UDPPrinterDataServer(void);
extern void PrintString(uint8_t *Str);
extern void WriterCPUFlash(uint32_t Addr, uint32_t *Buf, uint32_t Len,uint8_t Flag);
extern void Init_ETH(void);
extern void System_Upgrade(uint8_t Interface);
extern uint32_t PHY_SpeedModeConfig(ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress);
extern void SystemDelay1mS(uint32_t DelayNumber);
extern void ETH_HardwareConfig(void);
extern void SystemDelay1mS(uint32_t DelayNumber);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : ETH_SelfTest(void)
* Description    : ��ӡ����ҳ
* Input          : flag�����Է�ʽ,=0 ͨ����ҳ����,=1 ͨ����������, =2����Ĭ��IP��
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_SelfTest(void)
{
	struct uip_eth_addr macaddr;
	uip_ipaddr_t ipaddr[3];
	char tmp[64];

  ReadNetworkAddr(ipaddr, &macaddr);
	
	if(g_tSysConfig.DHCPEnable)
	{
		ipaddr[0][0] = uip_hostaddr[0];  
		ipaddr[0][1] = uip_hostaddr[1]; 

		ipaddr[1][0] = uip_netmask[0]; 
		ipaddr[1][1] = uip_netmask[1]; 

		ipaddr[2][0] = uip_draddr[0]; 
		ipaddr[2][1] = uip_draddr[1]; 
	}
	
// 	PrintString(" Ethernet Parameter:\n");
// 	
// 	PrintString(" Server port:\n");
// 	sprintf(tmp,"  Print:       %d\n",PRINT_PORT);
// 	PrintString((uint8_t*)tmp);
// 	sprintf(tmp,"  Http         %d\n",HTTP_PORT);
// 	PrintString((uint8_t*)tmp);

	PrintString(" Network Setting:\n");
	sprintf(tmp,"  MAC Address: %02x-%02x-%02x-%02x-%02x-%02x\n",
			macaddr.addr[0], macaddr.addr[1],
			macaddr.addr[2], macaddr.addr[3],
			macaddr.addr[4], macaddr.addr[5]);
	PrintString((uint8_t*)tmp);
	
	sprintf(tmp,"  IP Address:  %d.%d.%d.%d\n",
			(HTONS(ipaddr[0][0]) >>8) &0xff, HTONS(ipaddr[0][0]) &0xff,
			(HTONS(ipaddr[0][1]) >>8) &0xff, HTONS(ipaddr[0][1]) &0xff);
	PrintString((uint8_t*)tmp);

	sprintf(tmp,"  SubneMask:   %d.%d.%d.%d\n",
			(HTONS(ipaddr[1][0]) >>8) &0xff, HTONS(ipaddr[1][0]) &0xff,
			(HTONS(ipaddr[1][1]) >>8) &0xff, HTONS(ipaddr[1][1]) &0xff);
	PrintString((uint8_t*)tmp);

	sprintf(tmp,"  Gateway:     %d.%d.%d.%d\n\n",
			(HTONS(ipaddr[2][0]) >>8) &0xff, HTONS(ipaddr[2][0]) &0xff,
			(HTONS(ipaddr[2][1]) >>8) &0xff, HTONS(ipaddr[2][1]) &0xff);
	PrintString((uint8_t*)tmp);
	GoDotLine(CUT_OFFSETNUM);		//�ߵ���ֽλ��  2017.02.07 
	DriveCutPaper(CUT_TYPE_PART);
}

/*******************************************************************************
* Function Name  : WriteNetworkAddr( uip_ipaddr_t *ipaddr ,struct uip_eth_addr macaddr)
* Description    : �����ַд��FLASH��
* Input          : ipaddr��IP��ַ��ص�����ָ�룬macaddr��MAC��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void WriteNetworkAddr ( uip_ipaddr_t *ipaddr, struct uip_eth_addr macaddr)
{
	uint32_t config[5];
	
	config[0] = HTONS(ipaddr[0][0])<< 16 | HTONS(ipaddr[0][1]);
	config[1] = HTONS(ipaddr[1][0])<< 16 | HTONS(ipaddr[1][1]);
	config[2] = HTONS(ipaddr[2][0])<< 16 | HTONS(ipaddr[2][1]);
	config[3] =macaddr.addr[0]|(macaddr.addr[1]<<8)|(macaddr.addr[2]<<16)|(macaddr.addr[3]<<24);
	config[4] =macaddr.addr[4]|(macaddr.addr[5]<<8);
	
	//����д��Flash��
	WriterCPUFlash(ETH_FLASH_ADDR, config, 5, 2);
}
/*******************************************************************************
* Function Name  :ReadNetworkAddr( uip_ipaddr_t *ipaddr, struct uip_eth_addr macaddr)
* Description    : ��FLASH�ж�ȡ�����ַ
* Input          : ipaddr��IP��ַ��ص�����ָ�룬macaddr��MAC��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void ReadNetworkAddr(uip_ipaddr_t *ipaddr, struct uip_eth_addr *macaddr)
{
	uint32_t ultmp;

  ultmp = ReadFlash((EMAC_FLASH_ADDR));
	macaddr->addr[0] =ultmp;
	macaddr->addr[1] =ultmp>>8;
	macaddr->addr[2] =ultmp>>16;
	macaddr->addr[3] =ultmp>>24;
	ultmp =ReadFlash((EMAC_FLASH_ADDR +4));
	macaddr->addr[4] =ultmp;
	macaddr->addr[5] =ultmp>>8;
	
	ultmp =ReadFlash(IP_FLASH_ADDR);
	ipaddr[0][0] = HTONS(ultmp >>16); 
	ipaddr[0][1] = HTONS(ultmp); 

	ultmp =ReadFlash(MASK_FLASH_ADDR);	
	ipaddr[1][0] = HTONS(ultmp >>16 ); 
	ipaddr[1][1] = HTONS(ultmp ); 

	ultmp =ReadFlash(GATE_FLASH_ADDR);
	ipaddr[2][0] = HTONS(ultmp >>16 ); 
	ipaddr[2][1] = HTONS(ultmp); 
	
}

/*******************************************************************************
* Function Name  : InitNetworkAddr(void)
* Description    : ����ΪĬ��IP��ַ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitNetworkAddr(void)
{
	struct uip_eth_addr macaddr;
	uip_ipaddr_t ipaddr[3];			//IP��ַ��ʽ����
	uint32_t ultmp;
  if(g_tSysConfig.DHCPEnable)	//2017.02.07  DHCPģʽ��IPΪ0
	{
		uip_ipaddr(&ipaddr[0], 0,0,0,0);
	}
	else
	{
		uip_ipaddr(&ipaddr[0], 192,168,1,211);
	}		
	uip_ipaddr(&ipaddr[1], 255,255,255,0);
	uip_ipaddr(&ipaddr[2], 192,168,1,1);

	macaddr.addr[0]=0x00;
	macaddr.addr[1]=0x2e;
	ultmp = ReadFlash(0x1FFFF7E8);		//��ȡMCU��ID��ΪMAC��ַ
	macaddr.addr[2]=(uint8_t)(ultmp >>8 );
	macaddr.addr[3]=(uint8_t)(ultmp );
	macaddr.addr[4]=(uint8_t)(ultmp >>24);
	macaddr.addr[5]=(uint8_t)(ultmp >>16);

	ultmp = ReadFlash(0x1FFFF7EC);		//��ȡMCU��ID��ΪMAC��ַ
	macaddr.addr[2]+=(uint8_t)(ultmp >>8 );
	macaddr.addr[3]+=(uint8_t)(ultmp );
	macaddr.addr[4]+=(uint8_t)(ultmp >>24);
	macaddr.addr[5]+=(uint8_t)(ultmp >>16);
	
	ultmp = ReadFlash(0x1FFFF7F0);		//��ȡMCU��ID��ΪMAC��ַ
	macaddr.addr[2]+=(uint8_t)(ultmp >>8 );
	macaddr.addr[3]+=(uint8_t)(ultmp );
	macaddr.addr[4]+=(uint8_t)(ultmp >>24);
	macaddr.addr[5]+=(uint8_t)(ultmp >>16);

	//�������µ�ǰIP��MAC
	uip_setethaddr(macaddr);
 	uip_sethostaddr(&ipaddr[0]);
	uip_setnetmask(&ipaddr[1]);
	uip_setdraddr(&ipaddr[2]);
	
	WriteNetworkAddr(ipaddr, macaddr);
	
}
/*******************************************************************************
* Function Name  : LoadNetworkAddr(void)
* Description    : ��FLASH�ж��������ַ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LoadNetworkAddr(void)
{
	struct uip_eth_addr macaddr;
	uip_ipaddr_t ipaddr[3];			//IP��ַ��ʽ����
	uint32_t ultmp, ultmp1;
	
	ultmp =ReadFlash(IP_FLASH_ADDR);
	ultmp1 = ReadFlash((EMAC_FLASH_ADDR));
	
	//IP��MAC��ַ�Ƿ��Ѿ����ù�,û��������ΪĬ��IP
	if( ultmp == 0xffffffff || ultmp ==0 || (ultmp1 &0x000000ff) == 0xff || (g_tSysConfig.DHCPEnable==1))    //2017.02.07  DHCPģʽ�� ��Ҫ��IP��ַ��Ϊ0д������� Ϊ�˷����ж�Ϊ��0��Ϊ��ȡ����ip��ַ
	{
		InitNetworkAddr();	
	}		
		
  ReadNetworkAddr(ipaddr, &macaddr);

	uip_setethaddr(macaddr);
	uip_sethostaddr(&ipaddr[0]);
	uip_setnetmask(&ipaddr[1]);
	uip_setdraddr(&ipaddr[2]);
    
	// Program the hardware with it's MAC address (for filtering).
	ETH_MACAddressConfig(ETH_MAC_Address0, (uint8_t*)macaddr.addr);
}
void DHCP_Init(void)
{
	static u8 macaddr[6]; 	
	uint32_t ultmp;
	
	ultmp = ReadFlash((EMAC_FLASH_ADDR));
	macaddr[0] =ultmp;
	macaddr[1] =ultmp>>8;
	macaddr[2] =ultmp>>16;
	macaddr[3] =ultmp>>24;
	ultmp =ReadFlash((EMAC_FLASH_ADDR +4));
	macaddr[4] =ultmp;
	macaddr[5] =ultmp>>8;	

	dhcpc_init(&macaddr, 6);	
	dhcpc_request();
}
/*******************************************************************************
* Function Name  : ETH_main.
* Description    : ETH Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void uIPMain(void)
{
	//��̫��Ӳ������
	ETH_HardwareConfig();

	//��ʼ����̫��Ӳ��
	Init_ETH();
	
	//����IP��ַ��
	LoadNetworkAddr();

	//��ʼ��Arp��
	uip_arp_init();
	
	//��ʼ����ʱ��
	timer_set(&periodic_timer, CLOCK_SECOND / 2);	//0.5s
	timer_set(&arp_timer, CLOCK_SECOND * 10); 		//10s
	
	//��ʼ��uipЭ��ջ
	uip_init();
	
	//���������˿�
	uip_listen(HTONS(HTTP_PORT));
    uip_listen(HTONS(PRINT_PORT));
    uip_listen(HTONS(STATUS_PORT));
    
	//������̫�����ջ�������ַ
	uip_buf = emac_rcv_buf;
}

/*******************************************************************************
* Function Name  : GetETH_ConnectState.
* Description    : ��ѯ��̫������״̬.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void GetETH_ConnectState(void)
{
	static uint16_t Cntr = 0;

	//��ѯ�Ƿ��Ѿ�������
	if(ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR) & PHY_Linked_Status) 
	{
		//��һ������
		if(ETH_ConnectState ==0)
		{
			if(PHY_SpeedModeConfig(&ETH_InitStructure, PHY_ADDRESS))	//��ʼ��PHY�ӿ�
			{
				//SystemDelay1mS(5);						//��ʱ
				DelayMs(5);											//��ʱ 2016.07.21
				ETH_ConnectState=1;
			}
		}
	}
	else
	{
		Cntr++;
		if(Cntr > 100)
		{
			ETH_ConnectState =0;
			Cntr =0;
		}
	}
}

/*******************************************************************************
* Function Name  : uIP_Pkt_Handle(void).
* Description    : ��̫���жϴ���
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void uIP_Pkt_Handle(void)
{
		//��̫�����ݽ���
		uip_len = ETH_HandleRxPkt(uip_buf);
		
		if(uip_len > 0)
		{
			if(BUF->type == htons(UIP_ETHTYPE_IP))
			{
				uip_arp_ipin();
				uip_input();
				if(uip_len > 0)
				{
					NetReloadCounter = 0;       //2018.12.10  �����ӻ���յ����� ����
					uip_arp_out();
					TransmitPacket();
				}
			}
			else if(BUF->type == htons(UIP_ETHTYPE_ARP))
			{
				uip_arp_arpin();
				if(uip_len > 0)
				{
					TransmitPacket();
				}
			}
		}
}

/*******************************************************************************
* Function Name  : eth_event(void).
* Description    : ��̫���ӿڽ������ݴ���
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void eth_event(void)
{
	static uint8_t ETH_ConnectFlag = 0;
	static uint16_t Cntr = 0;
	uint32_t i;
	struct uip_eth_addr macaddr;	//MAC��ַ��ʽ����
	uip_ipaddr_t ipaddr[3];			//IP��ַ��ʽ����
	
	if(ETH_ConnectState)
	{
		//ÿ3�β�ѯһ�Σ�����ʱÿ�β�ѯ
		if( ++Cntr == 3 || g_bSleepModeFlag !=0)	
		{
			Cntr=0;

			//PHY��һ������,��������ARP����
			if(ETH_ConnectFlag ==0)
			{
				//���¿�ʼ��ʱ��
				timer_restart(&periodic_timer);
				timer_restart(&arp_timer);

				ETH_ConnectFlag=1;

				if(g_tSysConfig.DHCPEnable)
				{
					DHCP_Init();					//����DHCP,����discover	
				}
				else
				{	
					ArpRequest();
					TransmitPacket();
				}
				return;
			}
		
// 			//��̫�����ݽ���
// 			uip_len = ETH_HandleRxPkt(uip_buf);
// 			
// 			if(uip_len > 0)
// 			{
// 				if(BUF->type == htons(UIP_ETHTYPE_IP))
// 				{
// 					uip_arp_ipin();
// 					uip_input();
// 					if(uip_len > 0)
// 					{
// 						uip_arp_out();
// 						TransmitPacket();
// 					}
// 				}
// 				else if(BUF->type == htons(UIP_ETHTYPE_ARP))
// 				{
// 					uip_arp_arpin();
// 					if(uip_len > 0)
// 					{
// 						TransmitPacket();
// 					}
// 				}
// 			}

			//��ʱ��ѯ����
			if( timer_expired(&periodic_timer))		//��ѯʱ�䵽
			{
				timer_reset(&periodic_timer);
				for(i = 0; i < UIP_CONNS; i++)
				{
					uip_periodic(i);
					
					//�����������Ҫ���ͣ�����
					if(uip_len > 0)
					{
						uip_arp_out();
						TransmitPacket();
					}
				}
					
				#if UIP_UDP
				for(i = 0; i < UIP_UDP_CONNS; i++)
				{
					uip_udp_periodic(i);
					if(uip_len > 0)
					{
						uip_arp_out();
						TransmitPacket();
					}
				}
				#endif /* UIP_UDP */
				
				/* Call the ARP timer function every 10 seconds. */
				if(timer_expired(&arp_timer))
				{
						timer_reset(&arp_timer);
						uip_arp_timer();
				}
                
			}
			//
			if( Eth_ClearBusy ==1 ||										//��Ҫ��æ
				   ( ETH_Tx.ptr_in != ETH_Tx.ptr_out && ETH_Tx.State ==0 ))	//��������Ҫ����
			{
				struct uip_conn *conn;	
				for(i = 0; i < UIP_CONNS; i++)
				{
					conn = &uip_conns[i]; 
					uip_poll_conn(conn);
					if(uip_len > 0)
					{
						uip_arp_out();
						TransmitPacket();
					}
				}
			}
     
      //�޸���IP��ַ,��������IP��ַ
			if(IP_ChangeFlag ==1)		//�޸���IP		
			{
					LoadNetworkAddr();
					IP_ChangeFlag=0;
					ArpRequest();
					TransmitPacket();
			}
			else if(IP_ChangeFlag ==2)	//��������ģʽ		
			{	
					IP_ChangeFlag =0;
					if(g_tSysConfig.DHCPEnable)		  //���ʹ��DHCP,����ǰIPд��FLASH
					{
						ReadNetworkAddr(ipaddr, &macaddr);//����IP����
						ipaddr[0][0] = uip_hostaddr[0];
						ipaddr[0][1] = uip_hostaddr[1];
						ipaddr[1][0] = uip_netmask[0];
						ipaddr[1][1] = uip_netmask[1];
						ipaddr[2][0] = uip_draddr[0];
						ipaddr[2][1] = uip_draddr[1];					
						WriteNetworkAddr(ipaddr, macaddr);
					}				
					System_Upgrade(1);
			}
			
		}
	}
	else
	{
		ETH_ConnectFlag =0;
	}
}

/*******************************************************************************
* Function Name  : clock_time(void)
* Description    : ����ϵͳʱ��
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
clock_time_t clock_time(void)
{
    return (SYS_TICK);
}

/*******************************************************************************
* Function Name  : TransmitPacket(void)
* Description    : ͨ����̫���������ݣ�
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TransmitPacket(void)
{
	while(ETH_HandleTxPkt(uip_buf,uip_len) ==0);
}

/*******************************************************************************
* Function Name  : my_appcall(void)
* Description    : TCPӦ�ó���
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void my_appcall(void)
{
	g_bRoutinFlag = 1;			//�жϱ�־���˳�����ʱ�� 
	switch(uip_conn->lport)
	{
		case HTONS(PRINT_PORT):
		case HTONS(STATUS_PORT):
		{
			PrinterDataServer();
			break;
		}
/*
		case HTONS(STATUS_PORT):
		{
			PrinterDataServer();
			break;
		}
*/	
		case HTONS(HTTP_PORT):
		{	
			http_server();
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : my_udp_appcall(void)
* Description    : UDPӦ�ó���
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void my_udp_appcall(void)
{
	g_bRoutinFlag = 1;			//�жϱ�־���˳�����ʱ�� 
	switch(uip_udp_conn->lport)
	{
		case HTONS(3289):
		{
			UDPPrinterDataServer();
			break;
		}
		case HTONS(67):
		{
			dhcpc_appcall();
			break;
		}
		case HTONS(68):
		{
			dhcpc_appcall();
			break;
		}
	}
}

