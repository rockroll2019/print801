
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
//#include "ExtGlobalVariable.h"
#include "ETH_config.h"
#include "uip_arp.h"
#include "DefConstant.h"
#include "extgvar.h"

extern TypeDef_StructSysConfig	SysConfigStruct;
extern TypeDef_StructSystemCtrl	g_tSystemCtrl;
extern TypeDef_StructError				g_tError;

/******* �������뻺�������� *************/
// InBuffer_Struct   InBuffer;

uint8_t NetOutBuf[UIP_BUFSIZE];		//��̫�����������
extern APP_Tx_Struct ETH_Tx;
extern uint8_t Eth_ClearBusy;

void SendData( struct Port_state *hhs, uint8_t *outbuf);

extern uint8_t *StartToHostData(APP_Tx_Struct *Tx_Buf, uint16_t *Tx_length);
extern uint8_t PutNetCharToBuffer(uint8_t *Bufer, uint16_t Length, uint8_t Port);
extern void UpLoadData(uint8_t *DataBuf, uint32_t Length);

/*******************************************************************************
* Function Name  : PrinterDataServer( void )
* Description    : ��ӡ�����ݶ˿ڷ��񣬽��յ������ʹ�ӡ���뻺��������ӡ�����ص����ݷ��͵�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PrinterDataServer( void )
{
	struct uip_conn *conn;
	struct Port_state *hs;
	
	uint8_t *outbuf;
	uint16_t i;

  outbuf =NetOutBuf;
	
	// Pick out the application state from the uip_conn structure.
	hs = (struct Port_state *)&(uip_conn->appstate);

	if(uip_connected())			//���ӳɹ�
  {
		hs->datalen = 0;
		hs->dataptr = 0;
		hs->count =0;
		hs->state =0;

		//�����Ƿ�����ͬ�˿ڵ������������ϣ�
		for(i = 0; i < UIP_CONNS; ++i)
		{
			conn = &uip_conns[i];
			if((conn->lport == uip_conn->lport) && ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED))
				hs->state++;		//�ȴ�����
		}

		//������
		if( hs->state >1 || g_tInBuf.BufFullFlag == 1)		//2016.07.08
		{	
			uip_stop();
		}	
				//2018.12.10 ��������æ ��ֹ��������
		if(g_tInBuf.BufFullFlag ==0)
		{
			Eth_ClearBusy =0;    //2019.05.21
		}
		return;
	}
	
	if(uip_newdata())		//��������
  {
        //���յ�����Ч���ݣ�������ջ�����
		i=0;
		if(uip_conn->lport == htons(STATUS_PORT))
		{
			//״̬�˿ڣ������Զ�����״̬
			if( *(uint8_t*)uip_appdata ==0x1b && *((uint8_t*)uip_appdata +1)==0x76)
			{
				// �޸�Ϊ4000�˿�Ҳ�ɽ�����������
				g_tSystemCtrl.PrtSourceIndex =  ETHERNET;			//��������Դ 
				UpLoadData(g_tError.ASBStatus, 4);
				i=2;
			}
		}
        
    // �޸�Ϊ4000�˿�Ҳ�ɽ�����������
    if(PutNetCharToBuffer((uint8_t *)uip_appdata +i, uip_datalen()-i, ETHERNET))	//�������� 
		{
			//������ջ�������������ֹͣ
			uip_stop();
		}
		SendData(hs, outbuf);	//�Ƿ�����Ҫ���͵�����

		hs->count=0;
	}
	else if(uip_acked())
	{
		//�Ƿ���������Ҫ����
//		if(uip_conn->lport == htons(PRINT_PORT))	// ȥ���Զ˿ڵ�����
		{
			SendData(hs, outbuf);				//�Ƿ�����Ҫ���͵�����
		}

		hs->count =0;

		if(uip_stopped(uip_conn) && Eth_ClearBusy ==1)
		{
			uip_restart();
			Eth_ClearBusy =0;
		}

	}
    else if(uip_poll())				//��ѯ�¼�
    {
		if( uip_stopped(uip_conn) )	//�Ƿ������
		{
			//�����Ƿ��п��õ���ͬ�˿ڵ�����
			for(i = 0; i < UIP_CONNS; ++i)
			{
				conn = &uip_conns[i];
				//������ͬ�˿ڣ��ڵ�ǰ����֮ǰ�����ϵ�����
				if( conn->lport == uip_conn->lport )	//��ͬ�˿�
				{
					if((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED 	//������
						&& conn->appstate.state < hs->state)					//
						break;
				}
			}
			
			//����п������ӣ����õ�ǰ����Ϊ����״̬
			if(i == UIP_CONNS && g_tInBuf.BufFullFlag ==0)  //2016.07.08
			{
				uip_restart();
				Eth_ClearBusy =0;
			}
		}

		if( !(uip_stopped(uip_conn)))	//û�й���
		{
			//�Ƿ���������Ҫ����
//			if(uip_conn->lport == htons(PRINT_PORT))	// ȥ���Զ˿ڵ�����
			{
				SendData(hs, outbuf);		//�Ƿ�����Ҫ���͵�����
			}

			//������еȴ�ʱ��
			hs->count++;
			if(hs->count > UIP_WAITTIMES)
			{
				uip_close();
			}
		}
	}
    else if(uip_rexmit())						//�ط��¼�
    {
		uip_send(hs->dataptr, hs->datalen);		//�������ݵ�����
	}
    else if(uip_closed() || uip_aborted()|| uip_timedout())	//���ӹر�
	{
		//�Թ����������������
		for(i = 0; i < UIP_CONNS; ++i)
		{
			conn = &uip_conns[i];
			if(conn->appstate.state > hs->state)
				conn->appstate.state--;
		}
		hs->state =0;
	}
}
/****************************/
void SendData( struct Port_state *hhs, uint8_t *outbuf)
{
	struct Port_state *hs;
	uint32_t len;
	
	if(ETH_Tx.ptr_out ==ETH_Tx.ptr_in)
		return;

	len =0;
	hs =hhs;
	hs->dataptr =outbuf;
	hs->datalen =uip_mss();

	while(1)
	{
		if(ETH_Tx.ptr_out ==ETH_Tx.ptr_in)
			break;
		else
		{
			*outbuf++ = ETH_Tx.Buffer[ETH_Tx.ptr_out];
			ETH_Tx.ptr_out++;
			if(ETH_Tx.ptr_out ==ETH_Tx.size)
				ETH_Tx.ptr_out=0;
			len++;
			if(len ==hs->datalen)
				break;
		}
	}
	hs->datalen = len;
	uip_send(hs->dataptr, hs->datalen);		//�������ݵ�����
}
/************************/
void SendCommand(uint8_t *Buf, uint16_t length)
{
	if(Buf[5]==0x43 || Buf[5]==0x49 || Buf[5]==0x50 || Buf[5]==0x51 || Buf[5]==0x53)
		Buf[5] +=0x20;
	else
		Buf[5] =0x2a;

	Buf[12] =(uint8_t)(length>>8);
	Buf[13] =(uint8_t)length;

	uip_send(Buf, length+14);		//�������ݵ�����
}
/*****************************/
void UDPPrinterDataServer(void)
{
	
	uint8_t c,i;
	uint8_t *outbuf;
	uint16_t len,Dlen;
	char tmp[16];
	uint32_t ultmp;
	
  outbuf =NetOutBuf;

	if(uip_newdata())		//��������
    {
        //���յ�����Ч���ݣ�������ջ�����
		
		len =uip_datalen();
		memcpy((void*)outbuf, uip_appdata, len);
		
		if(len >=14 )		//Э���ͷ���ȣ�14
		{
			Dlen = outbuf[12]*256 + outbuf[13];	//��Ч���ݳ���
			if(len != Dlen+14 )
				return;
			if(strncmp((void*)outbuf,"EPSON",5) !=0)
				return;
			if(outbuf[5] <0x41 || outbuf[5]==0x4e || outbuf[5] >0x5a)
				return;
			if(outbuf[5]==0x43 || outbuf[5]==0x49 || outbuf[5]==0x50 || outbuf[5]==0x51 || outbuf[5]==0x53)
			{
				tmp[0]=0x03;
				tmp[1]=0x00;
				tmp[2]=0x00;
				Dlen =0;
				c=(outbuf[9]);
				switch(c)
				{
					case 0x00:
					{
						if(outbuf[5]==0x49 || outbuf[5]==0x53)	//�޷���
							return;
						if((outbuf[5]==0x50 || outbuf[5]==0x51)&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							//����1D4901��02��03�ķ�������
							Dlen =5;
							outbuf[14]=0x00;
							outbuf[15]=0x05;
							outbuf[16]=0x00;
							outbuf[17]=MODEL_ID;
							outbuf[18]=0x00;

							if(g_tSysConfig.SysLanguage >0)
								outbuf[18] |= 0x01;
							if(g_tSysConfig.CutType)
								outbuf[18] |= 0x02;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
					case 0x10:
					{
						if((outbuf[5]==0x51 )&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							//�����Զ�����״̬
							Dlen =5;
							outbuf[14]=0x00;
							for(i=0; i<4; i++)
								*(outbuf+15+i) =g_tError.ASBStatus[i];

							for(i=0; i<8; i++)
								*(outbuf+19+i)=0xff;
							Dlen +=i;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
					case 0x12:
					case 0x13:
					case 0x16:
					{
						if((outbuf[5]==0x43 )&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							Dlen =1;
							outbuf[14]=0x00;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
						case 0x14:
					{
						if((outbuf[5]==0x51 )&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							Dlen =8;
							outbuf[14]=0x00;
							outbuf[15]=0x00;
							outbuf[16]=0x00;
							outbuf[17]=0x00;
							outbuf[18]=0x00;
							outbuf[19]=0x00;
							outbuf[20]=0x40;
							outbuf[21]=0x00;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
					case 0x15:
					{
						if((outbuf[5]==0x43 || outbuf[5]==0x51 )&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							Dlen =1;
							outbuf[14]=0x00;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
					case 0x17:
					{
						if((outbuf[5]==0x51 )&& (strncmp((void*)&outbuf[6],tmp,3) ==0))
						{
							Dlen =4;
							outbuf[14]=0x00;
							outbuf[15]=0x00;
							outbuf[16]=0x00;
							outbuf[17]=0x00;
						}
						else
						{
							outbuf[10]=0xff;
							outbuf[11]=0xff;
						}
						break;
					}
					default:
					{
						outbuf[10]=0xff;
						outbuf[11]=0xff;
						break;
					}

				}
			}
			else
			{
				Dlen =0;
				outbuf[10]=0xff;
				outbuf[11]=0xff;
			}
			SendCommand(outbuf, Dlen);
		}	
		if(len == 13 )		//Э���ͷ���ȣ�13   SPRT-IP	SPRT-MAC SPRT-MODE SPRT-NAME
		{
			Dlen = outbuf[11]*256 + outbuf[12];	//��Ч���ݳ���
			if(Dlen != 13 )
				return;
			if(strncmp((void*)outbuf,"SPRT",4) !=0)
				return;
			if(outbuf[4] != '-')
				return;
								
		  if(outbuf[5] == 'I' && outbuf[6] == 'n' && outbuf[7] == 'f'  && outbuf[8] == 'o'  && outbuf[9] == 'r')
			{
				ultmp = ReadFlash((EMAC_FLASH_ADDR));
				outbuf[0] =ultmp;
				outbuf[1] =ultmp>>8;
				outbuf[2] =ultmp>>16;
				outbuf[3] =ultmp>>24;
				ultmp =ReadFlash((EMAC_FLASH_ADDR +4));
				outbuf[4] =ultmp;
				outbuf[5] =ultmp>>8;	
				
				sprintf((char *)outbuf,"%02x-%02x-%02x-%02x-%02x-%02x\n%s\n%s",
				outbuf[0], outbuf[1],
				outbuf[2], outbuf[3],
				outbuf[4], outbuf[5],
				SysConfigStruct.Model,
				SysConfigStruct.Manufacturer);
				
				Dlen = strlen((char *)outbuf);	
				
			}
			uip_send(outbuf, Dlen);		//�������ݵ�����
		}
	}
	else if(uip_poll())				//��ѯ�¼�
	{
	}
}
