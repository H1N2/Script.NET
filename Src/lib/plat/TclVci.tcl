#################################################################
# File       : TclVci.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2001-02-04
# Description: Script.NET TclVci support script
#################################################################

#-------------------------------------------------------------
# TclVci�ṩ����������
#-------------------------------------------------------------
# vci_open name ?ver? ?sharename?
# ����  : ��һ��VCI�����װ
# ����  : name		- �����
#		  ver		- ����汾,ȱʡΪ���°汾
#		  sharename	- ������,����ṩ��ͨ��������ʽ������,
#					  �����Ϳ���ʵ�ֶ��ʵ����Ĺ���
# ����ֵ: �򿪵����ʵ���ľ��,�������,�򷵻ش�����Ϣ
#-------------------------------------------------------------
# vci_close handle
# ����  : �ر�һ��VCI���ʵ��
# ����  : handle	- ʵ���ľ��
# ����ֵ: ����ɹ�û�з���ֵ,���ʧ���򷵻ش�����Ϣ
# ˵��  : ���һ����װ�����˶��ʵ��,ֻ�����һ��ʵ���ر�
#		  ֮��,�˷�װ����������Ϣ�Ż���ȫж��,����ͨ�������
#		  device::func�����ѯĿǰ�������ڴ��еķ�װ��Ϣ
#-------------------------------------------------------------
# vci_info -operator devicename ?...?
# ����  : �����װ�����Ͳ�ѯ
# ����  : -operator	- �ӹ���,������õ��ӹ�������
#			1)listdef	- �к��������嵥(�б���ʽ)
#			2)listfunc	- �к������嵥(�б���ʽ)
#			3)listparam	- �к����Ĳ���,������һ������Ϊ
#						  ������,������г��˲�������ϸ��Ϣ
#			4)listdevice- ��Ŀǰ���ص����з�װ����
#			5)listobject ?devicename?
#						- ��ĳ�豸������ʵ�����,�����ָ��
#						  �豸,���������豸������ʵ�����
#			6)listinstalled ?device? ?verlist|ver?
#						- �а�װ�������Ϣ
#			7)getvcihandle tclvciobj
#						- ��ȡָ��TclVci�����VCI���
#			8)gettclvciobj vciid vcihandle
#						- ��ȡָ��VCI�����TclVci��װ������
#-------------------------------------------------------------
# vci_exec handle func parameters
# ����  : ִ���������ĺ���
# ����  : handle		- ʵ���ľ��
#		  func			- ������
#		  parameters	- ���в���,���Ϊ�������Ͳ���,�������
#						  һ��TCL������
# ����ֵ: ����ִ�еķ���ֵ
#-------------------------------------------------------------
# TclVci���ò������Ͷ�������
#-------------------------------------------------------------
#	����			ֵ		˵��
#	PTYPE_VOID		0		��
#	PTYPE_CHAR		1		�з��ŵ��ֽ�
#	PTYPE_BYTE		2		�޷��ŵ��ֽ�
#	PTYPE_SHORT		3		�з���˫�ֽ�
#	PTYPE_WORD		4		�޷���˫�ֽ�
#	PTYPE_INT		5		�з������ֽ�
#	PTYPE_UINT		6		�޷������ֽ�
#	PTYPE_POINTER	7		ָ��
#	PTYEP_FLOAT		8		����
#	PTYPE_STRING	9		�ַ���
#	PTYPE_CSTRING	0		CString����
#	PTYPE_UNKNOWN	-1		δ֪����
#-------------------------------------------------------------
# TclVci�������ݷ�ʽ����
#-------------------------------------------------------------
#	����			ֵ		˵��
#	PMODE_DIRECT	0		ֱ�Ӵ���
#	PMODE_REFERENCE	1		����&
#	PMODE_POINTER	2		ָ��*
#	PMODE_POINTER2	3		˫��ָ��**
#	PMODE_CONST		4		����,�����޸�const
#-------------------------------------------------------------

#-------------------------------------------------------------
# ʵ���ַ������б��ת��
#-------------------------------------------------------------
proc _plat_Tcl_Vci_StrToList {str} {
	set strlength [string length $str]
	set DList ""
	for {set i 0} {$i < $strlength} {incr i} {
		binary scan [string index $str $i] H2 X1
		set DList "$DList $X1"
	}
	return $DList
}

#-------------------------------------------------------------
# ʵ���б��ַ�����ת��
#-------------------------------------------------------------
proc _plat_Tcl_Vci_ListToStr {listA} {
	set strlength [llength $listA]

	set formatStr [string repeat "%c" $strlength]
	return [eval "format $formatStr $listA"]
}

#-------------------------------------------------------------
# ����ȱʡ��DEBUG_OUT�ص�����,ָ��ƽ̨�����
# ��������:
# nDeviceID		- ����ID
# nType			- ������Ϣ����
#					TEXT_RECEIVE=0x10
#					TEXT_SEND=0x20
#					TEXT_DEBUG=0x30
# nMode			- ������Ϣ��ʾ��ʽ
#					MODE_HEX=0x10
#					MODE_ASCII=0x20
#					MODE_USER=0x30
# lpData		- ����ĵ�����Ϣ
# nLen			- ������Ϣ����
#-------------------------------------------------------------
proc _plat_Tcl_Vci_DebugOut {nDeviceID nType nMode lpData nLen} {
	set strInfo [varconvert itos $lpData];
	if { [expr $nMode == 0x20] } {
		puts -nonewline "DEBUG:$strInfo";
	} elseif { [expr $nMode == 0x10] } {
		set index 0;
		while {$index < [string length $strInfo]} {
			if {$index == 0} {
				puts "DEBUG:[_plat_Tcl_Vci_StrToList [string range $strInfo $index [expr $index+17]]]";
			} else {
				puts "      [_plat_Tcl_Vci_StrToList [string range $strInfo $index [expr $index+17]]]";
			}
			incr index 18;
		}
	}
}
