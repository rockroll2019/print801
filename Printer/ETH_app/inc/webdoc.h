//-----------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net WEBDOC.C
//
// This module contains web pages and headers
//-----------------------------------------------------------------------------
//#include "includes.h"

//#ifdef WEB_SERVER
// This is the header for web text pages. LENGTH is the body
// length, needed because in HTTP 1.1 we leave connection open
// Includes control statements to prevent browser from caching page
#ifndef	__WEBSVR
#define __WEBSVR

const char html_header[] ={
	"HTTP/1.1 200 OK\r\n"
	"Cache-control: no-cache\r\n"
	"Connection: Keep-Alive\r\n"
	"Content-Length: TAG:LEN1\r\n"
	"Content-Type: text/html\r\n\r\n"
};

const char language_htm_ch[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:����;font-size:26pt;width:60%%><BR><BR><B>��ӡ��</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>����ѡ��</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>���ļ���"
  "<option value=2>���ķ���</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='�� ��'></TD></TR><TR><TD align=middle><FONT>ѡ�����Բ��ύ��������ý��棡</TD></TR></TR></BODY></HTML>"};

const char menu_htm_ch[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>����IP<BR><BR><A href='sp' target=bf>���ڲ���<BR><BR>"
	"<A href='usb' target=bf>USB ����<BR><BR><A href='print' target=bf>��ӡ������<BR><BR>"
	"<A href='prtset' target=bf>���ò���<BR><BR>"   //2017.10.11
	"<A href='symbol' target=bf>����������<BR><BR>"
	"<A href='upload' target=bf>�̼�����<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};
	
const char port_htm_ch[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>����IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>����</TD><TD align='middle'>ֵ</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;��ӡ�˿�<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP��ַ<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;��������<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;����<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC��ַ<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>�ر�<option value=1>��</select></TD></TR>"     //2017.02.07 ����DHCPѡ��
	"<TR align=center><TD colspan=2><FONT color=#ff0000>ע��:�޸�IP�������µ�¼</TD></TR>"
  "<TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='�� ��' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_ch[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //ȱֽ�ش�ӡ                             
	"document.tk.buzzer_control.value=buzzercontrol;"   //������
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>��ӡ������</TD></TR><TR><TD><FORM name=tk action=printset method=POST><TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD>&nbsp;��ӡ���ͺ�</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"
	"<TR><TD>&nbsp;��ӡ�ٶ�</TD><TD>&nbsp;<select name='print_speed'><option value=0>250mm/s<option value=1>250mm/s<option value=2>250mm/s</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡŨ��</TD><TD>&nbsp;<select name='print_colour'><option value=0>ǳɫ<option value=1>�ж�<option value=2>��ɫ</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ���</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ����</TD><TD>&nbsp;<select name='print_direction'><option value=0>����<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ����</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;�е�����</TD><TD>&nbsp;<select name='cut_control'><option value=0>�ر�<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;ȱֽ�ش�ӡ</TD><TD>&nbsp;<select name='pne_print'><option value=0>�ر�<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;������ģʽ</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>�ر�<option value=1>��������<option value=2>�е�����<option value=3>������е�����</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char prtset_htm_ch[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"   //2016.10.10 ����δȡ������	
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>���ò���</TD></TR><TR><TD><FORM name=tk action=prtset method=POST><TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"   //2017.10.12 ���ӵ���һ��Ŀ ���е�printset��Ϊprtset
	"<TR><TD>&nbsp;�򵥷���������</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>�ر�<option value=1>δȡ������<option value=2>��������</select></TD></TR>"			//2016.10.10 ����δȡ������
	"<TR><TD>&nbsp;��������</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>��<option value=1>��<option value=2>��<option value=3>��</select></TD></TR>"	
	"<TR><TD>&nbsp;�ڱ���</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>�ر�<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;��ֽ������</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>�Ͷ�<option value=1>�ж�<option value=2>�߶�</select></TD></TR>"
	"<TR><TD>&nbsp;�س�ָ��</TD><TD>&nbsp;<select name='enter_command'><option value=0>�ر�<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;ָ�</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>MP<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;����DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>�ر�<option value=1>����</select></TD></TR>"        //2017.02.07  ��������DHCP
	"<TR><TD>&nbsp;Ĭ�ϴ���ҳ</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char soft_htm_ch[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>������Ϣ</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD align=middle>��ǰ�汾</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>ѡ���ļ�</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>ע��:�����в����˳�</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

//2016.07.14  ���ӹ̼�����
const char upload_htm_ch[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>��ӡ���̼�����<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD align=middle>��ǰ�汾</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>ע��:�����в����˳�</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='��������'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};


const char sp_htm_ch[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>���ڲ���</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR><TR><TD align=middle>������</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>����λ��</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 λ"
	"<option value=1>7 λ</select></TD></TR><TR><TD align=middle>У��</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>��У��<option value=1>��У��<option value=3>żУ��</select></TD></TR>"
	"<TR><TD align=middle>����</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char start_htm_ch[]={"<HTML><HEAD><TITLE>start</TITLE><META http-equiv=Content-Type content=\\text/html; charset=gb2312\\></HEAD>"
"<frameset cols=129,*><frame name=af marginWidth=4 marginHeight=0 src='menu'><frame name=bf noresize src='port'></FRAMESET></HTML>"};

const char symbol_htm_ch[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>����������</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>����</TD><TD align=middle>ֵ</TD></TR>"
  "<TR><TD align=middle rowspan=1>����������</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>ע��:���20��Ӣ���ַ�</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='�� ��' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_ch[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR><TR><TD>&nbsp;USB���к�</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR><TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>ʹ��<option value=1>��ֹ</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "���ڲ������óɹ��������²������µ�¼��</TD></TR></DIV></BODY></HTML>"};

const char error_htm_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>���ڲ�������ʧ�ܣ����������ã�</TD></TR></DIV></BODY></HTML>"};

const char ok_w_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "�������óɹ���</TD></TR></DIV></BODY></HTML>"};

const char ok_s_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "��ȴ���ӡ��������ģʽ��ˢ��ҳ�棡</TD></TR></DIV></BODY></HTML>"};

const char language_htm_e[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:����;font-size:26pt;width:60%%><BR><BR><B>Printer</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>Language Choose</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>Chinese"
  "<option value=2>Chinese-bi5</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='Submit'></TD></TR><TR><TD align=middle><FONT>Into the Set interface after Select the language and submit !</TD></TR></TR></BODY></HTML>"};

const char menu_htm_e[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>Net IP<BR><BR><A href='sp' target=bf>Usart Parameter<BR><BR>"
	"<A href='usb' target=bf>USB Parameter<BR><BR><A href='print' target=bf>Printer Parameter<BR><BR>"
	"<A href='prtset' target=bf>Set Parameter<BR><BR>"    //2017.10.11
	"<A href='symbol' target=bf>Company Content<BR><BR>"
	"<A href='upload' target=bf>UpLoad Firmware<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};  //2016.07.14
	
const char port_htm_e[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>Net IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>Content</TD><TD align='middle'>Value</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Printer Port<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Sub Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Gate Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>OFF<option value=1>ON</select></TD></TR>"   //2017.02.07
	"<TR align=center><TD colspan=2><FONT color=#ff0000>Please re-login after changing the IP !</TD></TR>"
  "<TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='Reload' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_e[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //ȱֽ�ش�ӡ
	"document.tk.buzzer_control.value=buzzercontrol;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=369 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>Printer Parameter</TD></TR><TR><TD><FORM name=tk action=printset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD>&nbsp;Printer model</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"
	"<TR><TD>&nbsp;Print Speed</TD><TD>&nbsp;<select name='print_speed'>"
	"<option value=0>250mm/s<option value=1>250mm/s<option value=2>250mm/s</select></TD></TR><TR><TD>&nbsp;Print Density</TD><TD>&nbsp;<select name='print_colour'><option value=0>Light<option value=1>Medium<option value=2>High</select></TD></TR>"
	"<TR><TD>&nbsp;Print Width</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;Print Direction</TD><TD>&nbsp;<select name='print_direction'><option value=0>Normal<option value=1>Reverse</select></TD></TR>"
	"<TR><TD>&nbsp;Print Font</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;Auto Cut</TD><TD>&nbsp;<select name='cut_control'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;ChangePaperRePrint</TD><TD>&nbsp;<select name='pne_print'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;Buzzer Enable</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>Off<option value=1>Error On<option value=2>Cut On<option value=3>Error&Cut On</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char prtset_htm_e[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"    //���2017.02.07�DHCP document.tk.dhcp_flag.value=dhcpflag;
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=369 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>Printer Parameter</TD></TR><TR><TD><FORM name=tk action=prtset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD>&nbsp;Bill Buzzer Alarm</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>Off<option value=1>TakeBill On<option value=2>PrintBill On</select></TD></TR>"
	"<TR><TD>&nbsp;Speaker Volume</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>Low<option value=1>Medium<option value=2>Hight<option value=3>OFF</select></TD></TR>"
	"<TR><TD>&nbsp;Black Mark</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;Paper Sensor</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>Light<option value=1>Medium"
	"<option value=2>High</select></TD></TR><TR><TD>&nbsp;CR_Enable</TD><TD>&nbsp;<select name='enter_command'><option value=0>OFF<option value=1>ON</select></TD></TR>"
	"<TR><TD>&nbsp;Command Set</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>MP<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;Net DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>OFF<option value=1>ON</select></TD></TR>"        //2017.02.07  ��������DHCP
	"<TR><TD>&nbsp;Code Page</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
		
const char soft_htm_e[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Soft Information</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD align=middle>Soft Ver</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>Choose File</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>Attention:Can't exit when upgrading !</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

//2016.07.14  ���ӹ̼�����
const char upload_htm_e[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>Upload Firmware<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD align=middle>Soft Ver</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>Attention:Can't exit when upgrading !</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='Goto Upload'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};


const char sp_htm_e[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Usart Parameter</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR><TR><TD align=middle>Baud Rate</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>Data bit</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 bit"
	"<option value=1>7 bit</select></TD></TR><TR><TD align=middle>Parity</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>None<option value=1>Odd<option value=3>Even</select></TD></TR>"
	"<TR><TD align=middle>Handshaking</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char symbol_htm_e[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Company Content</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>Content</TD><TD align=middle>Value</TD></TR>"
  "<TR><TD align=middle rowspan=1>Company Content</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>Up to 20 characters !</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='Reload' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_e[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR><TR><TD>&nbsp;USB Serial Number</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR><TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>Enable<option value=1>Disable</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Network parameters setting is successful, please re-login with new parameters !</TD></TR></DIV></BODY></HTML>"};

const char error_htm_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>Network parameters setting failed, please reset !</TD></TR></DIV></BODY></HTML>"};

const char ok_w_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Parameter is set successfully !</TD></TR></DIV></BODY></HTML>"};

const char ok_s_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Please wait for Upgrade Mode and Refresh!</TD></TR></DIV></BODY></HTML>"};

const char language_htm_b[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:����;font-size:26pt;width:60%%><BR><BR><B>��ӡ�C</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>�Z���x��</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>���ĺ��w"
  "<option value=2>���ķ��w</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='�� ��'></TD></TR><TR><TD align=middle><FONT>�x���Z�Բ��ύ���M���O�ý��棡</TD></TR></TR></BODY></HTML>"};

const char menu_htm_b[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>�W��IP<BR><BR><A href='sp' target=bf>���څ���<BR><BR>"
	"<A href='usb' target=bf>USB ����<BR><BR><A href='print' target=bf>��ӡ�C����<BR><BR>"
	"<A href='prtset' target=bf>�O�Å���<BR><BR>"      //2017.10.11
	"<A href='symbol' target=bf>�u��������<BR><BR>"
	"<A href='upload' target=bf>�̼�����<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};  //2016.07.14 ���ӹ̼�����ѡ�� upload
	
const char port_htm_b[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>�W��IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>����</TD><TD align='middle'>ֵ</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;��ӡ�˿�<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP��ַ<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;�ӾW�ڴa<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;�W�P<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC��ַ<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"    //2017.02.07 ����DHCPѡ��
	"<TR align=center><TD colspan=2><FONT color=#ff0000>ע��:�޸�IP��Ո���µ��</TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='�� ��' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_b[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //ȱֽ�ش�ӡ
	"document.tk.buzzer_control.value=buzzercontrol;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>��ӡ�C����</TD></TR><TR><TD><FORM name=tk action=printset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD>&nbsp;��ӡ�C��̖</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"	
	"<TR><TD>&nbsp;��ӡ�ٶ�</TD><TD>&nbsp;<select name='print_speed'><option value=0>250mm/s<option value=1>150mm/s<option value=2>250mm/s</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ���</TD><TD>&nbsp;<select name='print_colour'><option value=0>�\ɫ<option value=1>�ж�<option value=2>��ɫ</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ����</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ����</TD><TD>&nbsp;<select name='print_direction'><option value=0>����<option value=1>����</select></TD></TR>"
	"<TR><TD>&nbsp;��ӡ���w</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;�е�����</TD><TD>&nbsp;<select name='cut_control'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"
	"<TR><TD>&nbsp;ȱ���ش�ӡ</TD><TD>&nbsp;<select name='pne_print'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"
	"<TR><TD>&nbsp;���Q��ģʽ</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>�P�]<option value=1>�e�`���S<option value=2>�е����S<option value=3>�e�`���е����S</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char prtset_htm_b[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>�O�Å���</TD></TR><TR><TD><FORM name=tk action=prtset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD>&nbsp;��η��Q����</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>�P�]<option value=1>δȡ�Έ�<option value=2>��Έ�</select></TD></TR>"
	"<TR><TD>&nbsp;�Z������</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>��<option value=1>��<option value=2>��<option value=3>�P</select></TD></TR>"
	"<TR><TD>&nbsp;�ژ˙z�y</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"
	"<TR><TD>&nbsp;�y���`����</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>�Ͷ�<option value=1>�ж�<option value=2>�߶�</select></TD></TR>"
	"<TR><TD>&nbsp;��܇ָ��</TD><TD>&nbsp;<select name='enter_command'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"
	"<TR><TD>&nbsp;ָ�</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>WH<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;�W��DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>�P�]<option value=1>�_��</select></TD></TR>"        //2017.02.07  ��������DHCP
	"<TR><TD>&nbsp;Ĭ�J���a�</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
		
const char soft_htm_b[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>������Ϣ</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD align=middle>��ǰ�汾</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>�x���ļ�</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>ע��:�����в����˳�</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
//2016.07.14  ���ӹ̼�����
const char upload_htm_b[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>��ӡ�C�̼�����<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD align=middle>��ǰ�汾</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>ע��:�����в����˳�</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='�M������'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char sp_htm_b[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>���څ���</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR><TR><TD align=middle>������</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>����λ��</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 bit"
	"<option value=1>7 bit</select></TD></TR><TR><TD align=middle>У�</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>�oУ�<option value=1>��У�<option value=3>żУ�</select></TD></TR>"
	"<TR><TD align=middle>����</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char symbol_htm_b[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>�u��������</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>����</TD><TD align=middle>ֵ</TD></TR>"
  "<TR><TD align=middle rowspan=1>�u��������</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>ע��:���20��Ӣ���ַ�</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='�� ��' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_b[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>����</TD><TD align=middle>ֵ</TD></TR>"
	"<TR><TD>&nbsp;USB����̖</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR>"
	"<TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>ʹ��<option value=1>��ֹ</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='�� ��' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='�� ��'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "�W�څ����O�óɹ���Ո���������µ�䛣�</TD></TR></DIV></BODY></HTML>"};

const char error_htm_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>���څ�������ʧ����Ո�����O�ã�</TD></TR></DIV></BODY></HTML>"};

const char ok_w_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "�����O�óɹ���</TD></TR></DIV></BODY></HTML>"};

const char ok_s_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Ո�ȴ��M������ģʽ��ˢ����棡</TD></TR></DIV></BODY></HTML>"};
#endif

