/******************** (C) COPYRIGHT 2010 Spirit ********************
* File Name          : Http.c
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
#include "Http.h"
#include "ETH_config.h"
#include "uip.h"
#include "DefConstant.h"
#include "webdoc.h"
#include "stm32f10x_conf.h"
#include "extgvar.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUF_APPDATA ((uint8_t *)uip_appdata)
/*UserID and Password definition*/

/* Private macro -------------------------------------------------------------*/
#define WEB_LANGUAGE 1
#define WEB_PORT     2
#define WEB_PRINT    3
#define WEB_PRTSET   4
#define WEB_SOFT     5
#define WEB_SP       6
#define WEB_SYMBOL   7
#define WEB_USB      8
#define WEB_UPLOAD   9

/* Private variables ---------------------------------------------------------*/
uint8_t webtemp; 
const char *language_htm;
const char *menu_htm;
const char *port_htm;
const char *print_htm;
const char *prtset_htm;             //2017.10.11  ��������ҳ��
const char *soft_htm;
const char *upload_htm;
const char *sp_htm;
const char *symbol_htm;
const char *usb_htm;
const char *ok_htm;
const char *error_htm;
const char *ok_w;
const char *ok_s;
/* Extern  variables ---------------------------------------------------------*/
extern uint8_t NetOutBuf[UIP_BUFSIZE];		//��̫�����������
extern uint8_t IP_ChangeFlag;				//IP��ַ�޸ı�־	
extern uint8_t Eth_ClearBusy;
/* Private function prototypes -----------------------------------------------*/

/* Extern function prototypes ------------------------------------------------*/
extern void WriteNetworkAddr(uip_ipaddr_t *ipaddr, struct uip_eth_addr macaddr);
extern void ReadNetworkAddr (uip_ipaddr_t *ipaddr, struct uip_eth_addr *macaddr);
extern void WriteSPI_FlashParameter(uint8_t *Buf, uint8_t Len);
extern uint8_t ReadSysParameter(TypeDef_StructSysConfig *Prt);

uint8_t	*ppp;
TypeDef_StructSysConfig	SysConfigStruct;

/* Private functions ---------------------------------------------------------*/
void LanguageSet(uint8_t language)
{
		if(language == 0)
		{
				language_htm = language_htm_e;
				menu_htm = menu_htm_e;
				port_htm = port_htm_e;
				print_htm = print_htm_e;
			  prtset_htm = prtset_htm_e;           //2017.10.11
				soft_htm = soft_htm_e;
			  upload_htm = upload_htm_e;	
				sp_htm = sp_htm_e;
				symbol_htm = symbol_htm_e;
				usb_htm = usb_htm_e;
				ok_htm = ok_htm_e;
				error_htm = error_htm_e;
				ok_w = ok_w_e;
				ok_s = ok_s_e;
		}	
		else if(language == 1)
		{
				language_htm = language_htm_ch;
				menu_htm = menu_htm_ch;
				port_htm = port_htm_ch;		  
				print_htm = print_htm_ch;
			  prtset_htm = prtset_htm_ch;           //2017.10.11
				soft_htm = soft_htm_ch;
			  upload_htm = upload_htm_ch;
				sp_htm = sp_htm_ch;
				symbol_htm = symbol_htm_ch;
				usb_htm = usb_htm_ch;
				ok_htm = ok_htm_ch;
				error_htm = error_htm_ch;
				ok_w = ok_w_ch;
				ok_s = ok_s_ch;
		}	
		else if(language == 2)
		{
				language_htm = language_htm_b;
				menu_htm = menu_htm_b;
				port_htm = port_htm_b;
				print_htm = print_htm_b;
			  prtset_htm = prtset_htm_b;           //2017.10.11
				soft_htm = soft_htm_b;
			  upload_htm = upload_htm_b;
				sp_htm = sp_htm_b;
				symbol_htm = symbol_htm_b;
				usb_htm = usb_htm_b;
				ok_htm = ok_htm_b;
				error_htm = error_htm_b;
				ok_w = ok_w_b;
				ok_s = ok_s_b;
		}
		else
		{
				language_htm = language_htm_ch;
				menu_htm = menu_htm_ch;
				port_htm = port_htm_ch;
				print_htm = print_htm_ch;
			  prtset_htm = prtset_htm_ch;           //2017.10.11
				soft_htm = soft_htm_ch;
			  upload_htm = upload_htm_ch;
				sp_htm = sp_htm_ch;
				symbol_htm = symbol_htm_ch;
				usb_htm = usb_htm_ch;
				ok_htm = ok_htm_ch;
				error_htm = error_htm_ch;
				ok_w = ok_w_ch;
				ok_s = ok_s_ch;
		}
}
/*******************************************************************************
* Function Name  : http_init(void)
* Description    : ��ʼ������ҳ�˿�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*****************************/
void http_init(void)
{
    uip_listen(HTONS(HTTP_PORT));
}

/*******************************************************************************
* Function Name  : filldata ( char *des , char *src)
* Description    : ���ƹ̶����ַ�����17�ֽڣ�
* Input          : des��Ŀ�ĵ�ַ��src��Դ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void filldata (char *des , char *src)
{
	uint8_t i;

	for (i=0; i < 17; i++)
	{
		if(*src)
			*des++ =*src++;
		else
			*des++ =0x20;
	}
}

/*******************************************************************************
* Function Name  : str_to_ip ( uip_ipaddr_t *ipaddr, char *str )
* Description    : IP�ַ���ת��Ϊʮ��������
* Input          : 
* Output         : None
* Return         : �����ַ��ʽ��ȷ���ء�1�������򷵻ء�0��
*******************************************************************************/
uint8_t str_to_ip ( uip_ipaddr_t *ipaddr, char *str ,uint8_t ipflag)
{
	uint8_t i =0;
	uint8_t ipbuf[4]={0,0,0,0};	
	uint16_t num =0;
	
	while((*str !='&') && (*str !=0))
	{
		if(*str >='0' && *str <='9')
		{
			num *=10;
			num += *str -'0';
			if(num >255)
				return 0;
		}
		else if(*str=='.')
		{
			ipbuf[i] =num;
			num =0;
			i++;
		}
		else
			return 0;
		str++;
	}
	
	ipbuf[i] =num;
	
	//IP��gwУ�鷶Χ
	if(ipflag)
	{
		if(ipbuf[3]==0 ||ipbuf[3]==255)		//
			return 0;
	}
	uip_ipaddr(ipaddr, ipbuf[0],ipbuf[1],ipbuf[2],ipbuf[3]);
	return 1;
}

/*******************************************************************************
* Function Name  : str_to_mac ( uint8_t *mac , char *str )
* Description    : MAC�ַ���ת��Ϊʮ��������
* Input          : mac��MAC��ַ��str���ַ���
* Output         : None
* Return         : ��ʽ��ȷ���ء�1�������򷵻ء�0��
*******************************************************************************/
uint8_t str_to_mac ( uint8_t *mac , char *str )
{
	uint8_t ch, cl, i=0;
	
	while(*str !='&' && *str !=0 && i<6)
	{
		ch =*str++;
	
		if(ch >='a' && ch <='f')
			ch =ch -'a' +10;
		else if(ch >='A' && ch <='F')
			ch =ch -'A' +10;
		else if(ch >='0' && ch <='9')
			ch -= '0';
		else
			return 0;
		ch <<=4;

		cl =*str++;
		if(cl >='a' && cl <='f')
			cl =cl -'a' +10;
		else if(cl >='A' && cl <='F')
			cl= cl -'A' +10;
		else if(cl>='0'&&cl<='9')
			cl -='0';
		else
			return 0;
		
		mac[i++] =ch|cl;
			str++;
	}
	if(i<6)
		return 0;
	else
		return 1;
}
/*******************************************************************************
* Function Name  : CreateHtmlVariable(char *name, char *value)
* Description    : ���ɱ���
* Input          : name:������,value:����ֵ
* Output         : None
* Return         : ���ر����ַ�������
*******************************************************************************/
uint16_t CreateAddr(char *buf, char *name, char *value)
{
    char *str, *p;
    uint16_t len, i;
    
    p = buf;
    
    str ="// value=";
    strncpy(p, str, 9);
    p +=9;

    str =(char *)value;
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;

    for(i= len; i<17; i++)
        *p++ =0x20;

    str =" name=";
    strncpy(p, str, 6 );
    p +=6;

    str =(char *)name;
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;

    return (p - buf);
}
/*******************************************************************************
* Function Name  : CreateHtmlVariable(char *name, char *value)
* Description    : ���ɱ���
* Input          : name:������,value:����ֵ
* Output         : None
* Return         : ���ر����ַ�������
*******************************************************************************/
uint16_t CreateHtmlVariable(char *buf, char *name, char *value)
{
    char *str, *p;
    uint16_t len;
    
    p = buf;
    
    str ="var ";
    strncpy(p, str, 4);
    p +=4;

    str =(char *)name;
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;
	  str ="='";			
    strncpy(p, str, 2);
    p +=2;

    str =(char *)value;
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;
	  str ="';";			
    strncpy(p, str, 2);
    p +=2;

    return (p - buf);
}

/*******************************************************************************
* Function Name  : CreateHtmlVartable(char *buf, char Lang)
* Description    : ������ҳ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t CreateHtmlVarTable(char *buf, u8_t Lang)
{
    TypeDef_StructSysConfig Prt;        //ϵͳ���� 

    uint16_t len=0;
    uint32_t ultmp, ultmp1;
	  char tmpbuf[20];		
    char *p, *str;
            
    //����ϵͳ����
    ReadSysParameter(&Prt);
    
    p =buf;

    str="<SCRIPT LANGUAGE='JavaScript'>";
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;

	  switch (webtemp)
		{
			case WEB_LANGUAGE:
				str = tmpbuf;
				sprintf(str,"var language=%d;",Prt.SysLanguage);
				len =strlen(str);
				strncpy(p, str, len);
				p +=len;
			
			  str = tmpbuf;
				ultmp =ReadFlash(EMAC_FLASH_ADDR);
				ultmp1 =ReadFlash((EMAC_FLASH_ADDR +4));
				sprintf(str,"%02x-%02x-%02x-%02x-%02x-%02x",
				ultmp &0xff, (ultmp>>8)&0xff, (ultmp>>16)&0xff, (ultmp>>24)&0xff,
				ultmp1 &0xff, (ultmp1>>8)&0xff);
				len =CreateHtmlVariable(p, "MacAddr", str);
				p +=len;
        len =CreateAddr(p, "\"MacAddress\">\r\n", str);  //2016.07.21 ����IP��Ϣ
				p +=len;		
			
				str = tmpbuf;
				ultmp =ReadFlash(IP_FLASH_ADDR);
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "IPAddr", str);
				p +=len;
				len =CreateAddr(p, "\"IpAddress\">\r\n", str);
        p +=len;
			
				str = tmpbuf;
				ultmp =ReadFlash(MASK_FLASH_ADDR);	
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "Netmask", str);
				p +=len;
				len =CreateAddr(p, "\"SubnetMask\">\r\n", str);
        p +=len;
		
				str = tmpbuf;
				ultmp =ReadFlash(GATE_FLASH_ADDR);
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "Gateway", str);
				p +=len;
				len =CreateAddr(p, "\"Gateway\">\r\n", str);
        p +=len;
				break;
			
			case WEB_PORT:
				str = tmpbuf;
				ultmp =ReadFlash(EMAC_FLASH_ADDR);
				ultmp1 =ReadFlash((EMAC_FLASH_ADDR +4));
				sprintf(str,"%02x-%02x-%02x-%02x-%02x-%02x",
				ultmp &0xff, (ultmp>>8)&0xff, (ultmp>>16)&0xff, (ultmp>>24)&0xff,
				ultmp1 &0xff, (ultmp1>>8)&0xff);
				len =CreateHtmlVariable(p, "MacAddr", str);
				p +=len;
        len =CreateAddr(p, "\"MacAddress\">\r\n", str);  //2016.07.21 ����IP��Ϣ
				p +=len;		
			
				str = tmpbuf;
				ultmp =ReadFlash(IP_FLASH_ADDR);
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "IPAddr", str);
				p +=len;
				len =CreateAddr(p, "\"IpAddress\">\r\n", str);
        p +=len;
			
				str = tmpbuf;
				ultmp =ReadFlash(MASK_FLASH_ADDR);	
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "Netmask", str);
				p +=len;
				len =CreateAddr(p, "\"SubnetMask\">\r\n", str);
        p +=len;
		
				str = tmpbuf;
				ultmp =ReadFlash(GATE_FLASH_ADDR);
				sprintf(str,"%d.%d.%d.%d",
				(ultmp >>24)&0xff, (ultmp>>16)&0xff, (ultmp>>8)&0xff, ultmp&0xff);
				len =CreateHtmlVariable(p, "Gateway", str);
				p +=len;
				len =CreateAddr(p, "\"Gateway\">\r\n", str);
        p +=len;
			break;
			
		case WEB_PRINT:
			len =CreateHtmlVariable(p, "printtype", (char *)Prt.Model);
      p +=len;		  
		  str = tmpbuf;
			sprintf(str,"var printspeed=%d;",Prt.MaxSpeed);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var printcolour=%d;",Prt.Density);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var printwide=%d;",Prt.PrintWidthSelect);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var printdirection=%d;",Prt.PrintDir);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var printfont=%d;",Prt.SysFontSize);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var cutcontrol=%d;",Prt.CutType);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var pneprint=%d;",Prt.PaperOutReprint);       //ȱֽ�ش�ӡ 2017.03.16
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var buzzercontrol=%d;",Prt.BuzzerEnable);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;			
			break;

		case WEB_PRTSET:  																												//2017.10.11
		  str = tmpbuf;		
			sprintf(str,"var billalarm=%d;",Prt.PaperTakeAwaySensorEnable);         //2016.10.10  ����δȡ������
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var speakervolume=%d;",Prt.SpeakerVolume);                  //2017.5.26  ������������
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var blackflagtest=%d;",Prt.BlackMarkEnable);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var papersensitivity=%d;",Prt.PaperSensorSensitive);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var entercommand=%d;",Prt.CREnable);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var commandassemblage=%d;",Prt.CommandSystem);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var dhcpflag=%d;",Prt.DHCPEnable);         //2017.02.07  DHCP
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			sprintf(str,"var normalcode=%d;",Prt.DefaultCodePage);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			break;
			
// 		case WEB_SOFT:
// 			ppp=FIRMWARE_STRING;			  //2016.07.14 VersionString ȥ������Ŀ
// 			len =CreateHtmlVariable(p, "softver", (char *)ppp);
//       p +=len;
// 			break;

		case WEB_UPLOAD:
			ppp=FIRMWARE_STRING;			  //2016.07.14 VersionString
			len =CreateHtmlVariable(p, "softver", (char *)ppp);
      p +=len;
			break;
		
		case WEB_SP:
      str = tmpbuf;
			sprintf(str,"var spbaud=%d;",(Prt.ComBaudRate & 0x07));
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			
			sprintf(str,"var spbit=%d;",((Prt.ComProtocol & 0x08)>>3));
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			
			sprintf(str,"var spcheck=%d;",(Prt.ComProtocol & 0x03));
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			
			sprintf(str,"var spcontrol=%d;",((Prt.ComProtocol & 0x04)>>2));
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			break;
		
		case WEB_SYMBOL:	
      len =CreateHtmlVariable(p, "FactorID", (char *)Prt.Manufacturer);
      p +=len;
			break;
		
		case WEB_USB:
			len =CreateHtmlVariable(p, "printid", (char *)Prt.SerialNumber);
      p +=len;
			str = tmpbuf;
			sprintf(str,"var usb_id=%d;",Prt.USBIDEnable);
			len =strlen(str);
			strncpy(p, str, len);
			p +=len;
			break;
		
		default:
			break;
		}
		
    str ="</SCRIPT>";//");\r\n//--></SCRIPT>\r\n";
    len =strlen(str);
    strncpy(p, str, len);
    p +=len;
    
    return (p - buf);
}
/*******************************************************************************
* Function Name  : GetHtmlString(uint8_t *Dest, uint8_t *Source)
* Description    : ����ҳ��ȡ�ַ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GetHtmlString(uint8_t *Dest, char *Source)
{
    while( *Source !='&')
    {
            *Dest++ = *Source++;
    }
    *Dest =0;
}
//------------------------------------------------------------------------
// This serves up either a HTML page, a JPEG image, or controls an 
// LED,	depending what it gets from the browser. The received header
// must contain the word "GET" or "POST" to be considered a valid request.
// With HTTP 1.1 where the connection is left open, the header I send
// should include content length. With HTTP 1.0 you can just close the
// connection after sending the page and the browser knows its done. 
//
// The HTTP protocol specification is at http://www.w3.org/Protocols/ 
//------------------------------------------------------------------------
/*******************************************************************************
* Function Name  : http_server( void )
* Description    : http������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void http_server(void)
{
	struct Port_state *hs;
	struct uip_eth_addr macaddr;	//MAC��ַ��ʽ����
	uip_ipaddr_t ipaddr[3];			//IP��ַ��ʽ����

	uint32_t len,len1=0;
	uint8_t	ErrState;
	uint8_t	Temp;
	
	char tmpbuf[8];				//
	char *inbuf;					//�������ݵ�ַ
	char *outbuf;					//������ݵ�ַ

	uint16_t num;
	
	TypeDef_StructSysConfig Prt;        //ϵͳ���� 
  uint8_t  *p;
  p= (uint8_t *)(&Prt);
	Temp = 0;
	webtemp=0;
	inbuf = (char *)BUF_APPDATA;
	*(inbuf+uip_len)=0;

	hs = (struct Port_state *)&(uip_conn->appstate);
	
	if(uip_connected())
  {
		hs->count =0;				      //�޸Ĺ�������־
		hs->state = 0;				    //���Ա�־
    hs->datalen = 0;			    //�������ݳ���
    hs->dataptr = 0;			    //�������ݵ�ַ
		hs->olddatalen =0;			  //�����͵����ݳ��ȣ������ط� 
    return;
  }
	else if(uip_newdata())			//���յ�����
  {  
		if (strstr(inbuf, "GET / HTTP") != NULL)  //2016.07.21 ��ȡIP������
		{
// 				ReadSysParameter(&Prt);
// 			  LanguageSet(Prt.SysLanguage);	
// 			  webtemp=WEB_PORT;
// 			  hs->dataptr = (uint8_t *)port_htm;
// 				hs->datalen = strlen(port_htm);
			
			  ReadSysParameter(&Prt);
			  LanguageSet(Prt.SysLanguage);	
			  webtemp=WEB_LANGUAGE;
			  hs->dataptr = (uint8_t *)language_htm;
				hs->datalen = strlen(language_htm);
		}
		else if (strstr(inbuf, "GET /favicon.ico HTTP") != NULL)  //2017.03.13 ���ӣ�XP��IE�������ʱ�ᷢ�ʹ�����
		{			
			  ReadSysParameter(&Prt);
			  LanguageSet(Prt.SysLanguage);	
			  webtemp=WEB_LANGUAGE;
			  hs->dataptr = (uint8_t *)language_htm;
				hs->datalen = strlen(language_htm);
		}		
		else if (strstr(inbuf, "GET /menu") != NULL)
		{
				hs->dataptr = (uint8_t *)menu_htm;
				hs->datalen = strlen(menu_htm);
		}
    else if (strstr(inbuf, "GET /port") != NULL)
		{
				webtemp=WEB_PORT;
			  hs->dataptr = (uint8_t *)port_htm;
				hs->datalen = strlen(port_htm);
    }
		else if (strstr(inbuf, "GET /print") != NULL)
		{
				webtemp=WEB_PRINT;
			  hs->dataptr = (uint8_t *)print_htm;
				hs->datalen = strlen(print_htm);
		}
    else if (strstr(inbuf, "GET /prtset") != NULL)    //2017.10.11
		{
				webtemp=WEB_PRTSET;
			  hs->dataptr = (uint8_t *)prtset_htm;
				hs->datalen = strlen(prtset_htm);	
		}
		else if (strstr(inbuf, "GET /upload") != NULL)		//2016.07.14
		{
				webtemp=WEB_UPLOAD;
			  hs->dataptr = (uint8_t *)upload_htm;
				hs->datalen = strlen(upload_htm);	
		}
		else if (strstr(inbuf, "GET /sp") != NULL)
		{
				webtemp=WEB_SP;
			  hs->dataptr = (uint8_t *)sp_htm;
				hs->datalen = strlen(sp_htm);
		}
		else if (strstr(inbuf, "GET /start") != NULL)
		{
			  hs->dataptr = (uint8_t *)start_htm_ch;
				hs->datalen = strlen(start_htm_ch);
		}
		else if (strstr(inbuf, "GET /symbol") != NULL)
		{
			  webtemp=WEB_SYMBOL;
			  hs->dataptr = (uint8_t *)symbol_htm;
				hs->datalen = strlen(symbol_htm);
		}
    else if (strstr(inbuf, "GET /usb") != NULL)
		{
				webtemp=WEB_USB;
			  hs->dataptr = (uint8_t *)usb_htm;
				hs->datalen = strlen(usb_htm);	
		}
		else if( (outbuf =strstr(inbuf, "Refresh")) != NULL )
		{
				return;
		}
		else if (strstr(inbuf, "POST /languageset") != NULL)
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_LANGUAGE;
			  
			  if( (outbuf =strstr(inbuf,"languageset=")) != NULL )
				{
						outbuf +=12;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;
						}
				}	
				Prt.SysLanguage = num;
			  
				WriteSPI_FlashParameter(p, sizeof( Prt ));
			  LanguageSet(Prt.SysLanguage);
				hs->dataptr = (uint8_t *)start_htm_ch;
				hs->datalen = strlen(start_htm_ch);
		}
    else if (strstr(inbuf, "POST /portset") != NULL)
		{
			  webtemp=WEB_PORT;
				ReadNetworkAddr(ipaddr, &macaddr);
				ErrState=0;
			
				if( (outbuf = strstr(inbuf,"IpAddress=")) != NULL )		
				{
						if( 0== str_to_ip(&ipaddr[0], outbuf +10, 1))
								ErrState =1;

				}
		
				if( (outbuf = strstr(inbuf,"SubnetMask=")) != NULL )
				{
						if( 0 ==str_to_ip(&ipaddr[1], outbuf +11, 0))
								ErrState =1;
				}
		
				if( (outbuf =strstr(inbuf,"Gateway=")) != NULL )
				{
						if( 0== str_to_ip(&ipaddr[2], outbuf +8, 1))
								ErrState =1;
						
				}			
  
				if( (outbuf =strstr(inbuf,"MacAddress=")) != NULL )
				{
						if( 0 ==str_to_mac(macaddr.addr, outbuf+11))
								ErrState =1;
				}
				
				if(ErrState ==0)
				{
						WriteNetworkAddr(ipaddr, macaddr);
						hs->count =1;
					
						hs->dataptr = (uint8_t *)ok_htm_ch;
						hs->datalen = strlen(ok_htm_ch);
				}
				else
				{
						hs->dataptr = (uint8_t *)error_htm;
						hs->datalen = strlen(error_htm);
				}				
		}
		else if (strstr(inbuf, "POST /printset") != NULL)
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_PRINT;
			
			  if( (outbuf =strstr(inbuf,"print_type=")) != NULL )
				{
						GetHtmlString(Prt.Model, outbuf +11);
				}
			
				if( (outbuf =strstr(inbuf,"print_speed=")) != NULL )
				{
						outbuf +=12;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;
						}
				}	
				Prt.MaxSpeed = num;
				outbuf++;
			
			  if( (outbuf =strstr(inbuf,"print_colour=")) != NULL )
				{
						outbuf +=13;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}	
				Prt.Density = num;
				outbuf++;

		    if( (outbuf =strstr(inbuf,"print_wide=")) != NULL )
				{
						outbuf +=11;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.PrintWidthSelect = num;
				outbuf++;
			 
				if( (outbuf =strstr(inbuf,"print_direction=")) != NULL )
				{
						outbuf +=16;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.PrintDir = num;
				outbuf++;
				
				if( (outbuf =strstr(inbuf,"print_font=")) != NULL )
				{
						outbuf +=11;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;
						}
				}	
				Prt.SysFontSize = num;
				outbuf++;
			
			  if( (outbuf =strstr(inbuf,"cut_control=")) != NULL )
				{
						outbuf +=12;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}	
				Prt.CutType = num;
				outbuf++;

				if( (outbuf =strstr(inbuf,"pne_print=")) != NULL )   //ȱֽ�ش�ӡ
				{
						outbuf +=10;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.PaperOutReprint = num;
				outbuf++;
				
		    if( (outbuf =strstr(inbuf,"buzzer_control=")) != NULL )
				{
						outbuf +=15;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.BuzzerEnable = num;
						  				
			  WriteSPI_FlashParameter(p, sizeof( Prt ));
			  hs->dataptr = (uint8_t *)ok_w;
				hs->datalen = strlen(ok_w);
				
				g_tSysConfig = Prt;					   //2016.10.10  �Ѹ��ĵı�����ֵ ʹ������������Ч
				if(Temp == 1)									 //2017.02.07  ���ÿ���DHCP ������ӡ��
				{
					NVIC_SystemReset();			  	 //��λ
				}	
		}
		else if (strstr(inbuf, "POST /prtset") != NULL)     //2017.10.11  ���ӵ�����ҳ���ڵ�������
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_PRTSET;
						  			 
				if( (outbuf =strstr(inbuf,"bill_alarm=")) != NULL )		//2016.10.10 ����δȡ������
				{
						outbuf +=11;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.PaperTakeAwaySensorEnable = num;
				outbuf++;

				if( (outbuf =strstr(inbuf,"speaker_volume=")) != NULL )		//2017.5.26������Z������
				{
						outbuf +=15;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.SpeakerVolume = num;
				outbuf++;

				if(Prt.SpeakerVolume == 0)
					Line_2A_WTN588H_Dly(SPEAKERLOW);
				else if(Prt.SpeakerVolume == 1)
					Line_2A_WTN588H_Dly(SPEAKERMIDDLE);
				else if(Prt.SpeakerVolume == 2)
					Line_2A_WTN588H_Dly(SPEAKERHIGHT);
				else if(Prt.SpeakerVolume == SPEAKECLOSED)
				{
					g_tSpeaker.SpeakCtrl = 0;  //�ر�����
				}
				
				if((g_tSpeaker.SpeakElecCtrl == 1)&&(Prt.SpeakerVolume<=2))
				{
					g_tSpeaker.SpeakCtrl = 1;
				}
				
				if( (outbuf =strstr(inbuf,"blackflag_test=")) != NULL )
				{
						outbuf +=15;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.BlackMarkEnable = num;
				outbuf++;
				
				if( (outbuf =strstr(inbuf,"paper_sensitivity=")) != NULL )
				{
						outbuf +=18;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;
						}
				}	
				Prt.PaperSensorSensitive = num;
				outbuf++;
			
			  if( (outbuf =strstr(inbuf,"enter_command=")) != NULL )
				{
						outbuf +=14;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}	
				Prt.CREnable = num;
				outbuf++;

		    if( (outbuf =strstr(inbuf,"command_assemblage=")) != NULL )
				{
						outbuf +=19;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.CommandSystem = num;
				outbuf++;
			 
				if( (outbuf =strstr(inbuf,"dhcp_flag=")) != NULL )		//2017.02.07 ����DHCP
				{
						outbuf +=10;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.DHCPEnable = num;
				if(Prt.DHCPEnable == 1)			//2017.02.07 �����ҳ���ÿ���DHCP������������ӡ�� �����ӡ��ȡ�ĵ�ַ��
				{
					Temp = 1;
				}	
				outbuf++;
				
				if( (outbuf =strstr(inbuf,"normal_code=")) != NULL )
				{
						outbuf +=12;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.DefaultCodePage = num;
			  				
			  WriteSPI_FlashParameter(p, sizeof( Prt ));
			  hs->dataptr = (uint8_t *)ok_w;
				hs->datalen = strlen(ok_w);
				
				g_tSysConfig = Prt;					   //2016.10.10  �Ѹ��ĵı�����ֵ ʹ������������Ч
				if(Temp == 1)									 //2017.02.07  ���ÿ���DHCP ������ӡ��
				{
					NVIC_SystemReset();			  	 //��λ
				}	
		}
//     else if (strstr(inbuf, "POST /softset") != NULL)  //2016.07.14  ȥ������Ŀ
// 		{
// 			  webtemp=WEB_SOFT;	
// 				hs->dataptr = (uint8_t *)ok_s;
// 				hs->datalen = strlen(ok_s);		
// 		}
		else if (strstr(inbuf, "POST /uploadset") != NULL)  //2016.07.14  ��������
		{
			  webtemp=WEB_UPLOAD;	
				hs->dataptr = (uint8_t *)ok_s;
				hs->datalen = strlen(ok_s);		
				hs->count =2;			
		}
		else if (strstr(inbuf, "POST /spset") != NULL)
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_SP;
			  
				if( (outbuf =strstr(inbuf,"sp_baud=")) != NULL )
				{
						outbuf +=8;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;
						}
				}	
				Prt.ComBaudRate =(Prt.ComBaudRate & 0xf8) | num;
				outbuf++;
			
			  if( (outbuf =strstr(inbuf,"sp_bit=")) != NULL )
				{
						outbuf +=7;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}	
				Prt.ComProtocol = num <<3;
				outbuf++;

		    if( (outbuf =strstr(inbuf,"sp_check=")) != NULL )
				{
						outbuf +=9;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.ComProtocol |= num;
				outbuf++;
			 
				if( (outbuf =strstr(inbuf,"sp_control=")) != NULL )
				{
						outbuf +=11;
						num =0;
						while( *outbuf !='&')
						{
								num *=10;
								num += *outbuf -'0';
								outbuf++;	
						}
				}
				Prt.ComProtocol |= num << 2;
			
			  WriteSPI_FlashParameter(p, sizeof( Prt ));
			  hs->dataptr = (uint8_t *)ok_w;
				hs->datalen = strlen(ok_w);
				
				g_tSysConfig = Prt;					   //2016.10.10  �Ѹ��ĵı�����ֵ ʹ������������Ч
		}
		else if (strstr(inbuf, "POST /symbolset") != NULL)  //��ʾlogo���ݴ�ӡ ������
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_SYMBOL;
			
				if( (outbuf =strstr(inbuf,"symbolset=")) != NULL )
				{
						GetHtmlString(Prt.Manufacturer, outbuf +10);
				}
				
				WriteSPI_FlashParameter(p, sizeof( Prt ));
				hs->dataptr = (uint8_t *)ok_w;
				hs->datalen = strlen(ok_w);
				
				g_tSysConfig = Prt;					   //2016.10.10  �Ѹ��ĵı�����ֵ ʹ������������Ч
		}
    else if (strstr(inbuf, "POST /usbset") != NULL)
		{
				ReadSysParameter(&Prt);
			  webtemp=WEB_USB;
			
	      if( (outbuf =strstr(inbuf,"print_id=")) != NULL )			
				{
						GetHtmlString(Prt.SerialNumber, outbuf +9);
				}		
			
				if( (outbuf =strstr(inbuf,"usb_id=")) != NULL )
				{
					outbuf +=7;
					num =0;
					while( *outbuf !='&')
					{
						num *=10;
						num += *outbuf -'0';
						outbuf++;
					}
				}					
				Prt.USBIDEnable = num;
				
				WriteSPI_FlashParameter(p, sizeof( Prt ));
				hs->dataptr = (uint8_t *)ok_w;
				hs->datalen = strlen(ok_w);	
				
				g_tSysConfig = Prt;					   //2016.10.10  �Ѹ��ĵı�����ֵ ʹ������������Ч
		}
		else if (strstr(inbuf, "POST /setnetwork.htm") != NULL)		//2016.07.21 ��������Ip��ַ�Ȳ���
		{
      //����IP����
			ReadNetworkAddr(ipaddr, &macaddr);
      if(SysConfigStruct.DHCPEnable)				//ʹ����DHCP����ʾ��ǰIP��ַ
			{
				ipaddr[0][0] = uip_hostaddr[0];
				ipaddr[0][1] = uip_hostaddr[1];
				ipaddr[1][0] = uip_netmask[0];
				ipaddr[1][1] = uip_netmask[1];
				ipaddr[2][0] = uip_draddr[0];
				ipaddr[2][1] = uip_draddr[1];	
			}      
      ErrState=0;
			//if ((outbuf =strstr(inbuf, "setup=")) !=NULL)		//2016.07.21 ����IP������
			{
				//����MAC
				if( (outbuf =strstr(inbuf,"MacAddress=")) != NULL )
				{
					if( 0 ==str_to_mac(macaddr.addr, outbuf+11))
						ErrState =1;
				}
				//����IP
				if( (outbuf = strstr(inbuf,"IpAddress=")) != NULL )		
				{
					if( 0== str_to_ip(&ipaddr[0], outbuf +10, 1))
						ErrState =1;

				}
				//��������
				if( (outbuf = strstr(inbuf,"SubnetMask=")) != NULL )
				{
					if( 0 ==str_to_ip(&ipaddr[1], outbuf +11, 0))
						ErrState =1;
				}
				//��������
				if( (outbuf =strstr(inbuf,"Gateway=")) != NULL )
				{
					if( 0== str_to_ip(&ipaddr[2], outbuf +8, 1))
						ErrState =1;
						
				}			

				if(ErrState ==0)
				{
					WriteNetworkAddr(ipaddr, macaddr);
					hs->count =1;
				
					hs->dataptr = (uint8_t *)ok_htm;
					hs->datalen = strlen(ok_htm);
				}
				else
				{
					hs->dataptr = (uint8_t *)error_htm;
					hs->datalen = strlen(error_htm);
				}
			}
		}
 		else
		{
			return;
		}

		outbuf = (char *)NetOutBuf;
		len = strlen(html_header);
		memcpy(outbuf, html_header, len);
		len1 = CreateHtmlVarTable(outbuf+len, hs->state);
		len +=len1;
		*(outbuf +len)=0;

		sprintf(tmpbuf, "%d", len1 + hs->datalen);
		if((inbuf =strstr(outbuf, "TAG:LEN1")) !=0)
		{
			len1=strlen(tmpbuf);
			strncpy(inbuf, tmpbuf, len1);
			memset(inbuf+len1, ' ', 8-len1);
		}
		uip_send(outbuf, len);
		hs->olddatalen =len;				
	}
	else if(uip_acked())					//����Ӧ��
	{
		if(hs->datalen ==0)					//�����ϴ�һ��û�з����������
		{
			uip_close();
			if(hs->count )					//�޸Ĺ�IP
			{
				IP_ChangeFlag =hs->count;
				hs->count =0;
			}
		}
		else
		{
			outbuf =(char *)NetOutBuf;
		
			len = uip_mss();					
			if(hs->datalen < len)
				len = hs->datalen;

			memcpy(outbuf, hs->dataptr, len);
			uip_send(outbuf, len );
			hs->olddatalen =len;			

			hs->dataptr +=len ;
			hs->datalen -=len ;
		}
	}
    else if(uip_poll())						//��ѯ
    {
			Eth_ClearBusy =0;
		}
    else if(uip_rexmit())					//�ط��¼�
    {
			outbuf =(char *)NetOutBuf;
			uip_send(outbuf, hs->olddatalen );	
		}
    else if(uip_closed()|| uip_aborted()|| uip_timedout())	//���ӹر�
		{

/*		if(hs->count )					//�޸Ĺ�IP������ģʽ
		{
			IP_ChangeFlag =hs->count;
			hs->count =0;
		}
*/
		}
}
