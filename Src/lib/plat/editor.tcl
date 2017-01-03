#################################################################
# File       : editor.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2004-08-20
# Description: Script.NET editor encap script
#################################################################

package require Itcl

#-------------------------------------------------------------
# TEditor class define
#-------------------------------------------------------------
if {[itcl::find classes TEditor] != "TEditor"} {
::itcl::class  TEditor {
	constructor { {fname ""} } {};
	destructor {};
	
	### data member ###
	private variable _filename;		#�ļ���
	private variable _owm_id;		#ģ��ID
	
	### public methods ###
	public method  Open { {fname ""} };	#���ļ�
	public method  Attach { {no -1} };	#�����ļ�����
	public method  GetOpenCount {};		#��ȡ�򿪵��ļ���
	public method  GetOpenFile { no };	#��ȡ�򿪵��ļ���
	public method  SetReadOnly { {bReadOnly 1} };#����ֻ������
	public method  SetCurrentLine { lineno };#���õ�ǰ��
	public method  SetCurrentExecLine { lineno };#���õ�ǰִ����
	public method  SearchLine { line };	#������,������Ϊ��ǰ��
	public method  GetBreakList {};		#��ȡ�ϵ��б�
	public method  AddBreakList { lsbreak };#���Ӷϵ��б�
	public method  ClearBreakList {};		#��նϵ��б�
	public method  GetLineCount {};		#��ȡ����
	public method  GetLine { lineno };	#��ȡ��
	public method  SetLine { lineno text };	#������
	public method  GetSelectLine { startline endline };	#��ȡ��ǰѡ�����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TEditor::constructor { {fname "" } } {
	set _filename $fname
	set _owm_id "org.owm.edit"
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
::itcl::body TEditor::destructor {} {
}

#-------------------------------------------------------------
#	���ļ�
#	����:
#	fname	: �ļ���,Ϊ�ձ�ʾ�ñ�����ļ���
#-------------------------------------------------------------
::itcl::body TEditor::Open { {fname ""} } {
	if {$fname != ""} {
		set _filename $fname
	}
	
	return [owm -sendcmd $_owm_id 102 "<string>$_filename" 0]
}

#-------------------------------------------------------------
#	�����ļ�����
#	����:
#	no	: �ļ����,Ϊ-1��ʾ��������ǰ������ļ�
#-------------------------------------------------------------
::itcl::body TEditor::Attach { {no -1} } {
	if {$no == -1} {
		set no -2
	}
	
	set _filename [GetOpenFile $no]
}

#-------------------------------------------------------------
#	��ȡ�򿪵��ļ���
#	����:
#-------------------------------------------------------------
::itcl::body TEditor::GetOpenCount {} {
	return [owm -sendcmd $_owm_id 105 -1 0]
}

#-------------------------------------------------------------
#	��ȡ�򿪵��ļ���
#	����:
#	no	: �ļ����,Ϊ-1��ʾ��ȡ�ļ���,-2��ʾ��ȡ��ǰ�ļ���
#-------------------------------------------------------------
::itcl::body TEditor::GetOpenFile { no } {
	return [owm -sendcmd $_owm_id 105 $no 0 "<string>"]
}

#-------------------------------------------------------------
#	����ֻ������
#	����:
#	bReadOnly	: ֻ������,1-ֻ��,0-��д,ȱʡΪֻ��
#-------------------------------------------------------------
::itcl::body TEditor::SetReadOnly { {bReadOnly 1} } {
	owm -sendcmd $_owm_id 104 "<string>$_filename" $bReadOnly
}

#-------------------------------------------------------------
#	���õ�ǰ��
#	����:
#	lineno	: �к�
#-------------------------------------------------------------
::itcl::body TEditor::SetCurrentLine { lineno } {
	owm -sendcmd $_owm_id 111 "<string>$_filename" $lineno
}

#-------------------------------------------------------------
#	���õ�ǰִ����
#	����:
#	lineno	: �к�
#-------------------------------------------------------------
::itcl::body TEditor::SetCurrentExecLine { lineno } {
	owm -sendcmd $_owm_id 112 "<string>$_filename" $lineno
}

#-------------------------------------------------------------
#	������,������Ϊ��ǰ��
#	����:
#	line	: ������
#-------------------------------------------------------------
::itcl::body TEditor::SearchLine { line } {
	owm -sendcmd $_owm_id 113 "<string>$_filename" "<string>$line"
}

#-------------------------------------------------------------
#	��ȡ�ϵ��б�
#-------------------------------------------------------------
::itcl::body TEditor::GetBreakList {} {
	owm -sendcmd $_owm_id 115 "<string>$_filename" "<uintarray>lsbreak"
	return $lsbreak
}

#-------------------------------------------------------------
#	���Ӷϵ��б�
#-------------------------------------------------------------
::itcl::body TEditor::AddBreakList { lsbreak } {
	owm -sendcmd $_owm_id 124 "<string>$_filename" "<uintarray>lsbreak"
}

#-------------------------------------------------------------
#	��նϵ��б�
#-------------------------------------------------------------
::itcl::body TEditor::ClearBreakList {} {
	owm -sendcmd $_owm_id 125 "<string>$_filename" 0
}

#-------------------------------------------------------------
#	��ȡ����
#-------------------------------------------------------------
::itcl::body TEditor::GetLineCount {} {
	return [owm -sendcmd $_owm_id 117 "<string>$_filename" 0]
}

#-------------------------------------------------------------
#	��ȡ��
#-------------------------------------------------------------
::itcl::body TEditor::GetLine { lineno } {
	return [owm -sendcmd $_owm_id 118 "<string>$_filename" $lineno "<string>"]
}

#-------------------------------------------------------------
#	������
#-------------------------------------------------------------
::itcl::body TEditor::SetLine { lineno text } {
	return [owm -sendcmd $_owm_id 119 "<string>$_filename" "<string><$lineno>$text"]
}

#-------------------------------------------------------------
#	��ȡ��ǰѡ�����
#-------------------------------------------------------------
::itcl::body TEditor::GetSelectLine { startline endline } {
	upvar $startline _startline
	upvar $endline _endline
	set linecount [owm -sendcmd $_owm_id 120 "<string>$_filename" "<&int>_startline"]
	set _endline [expr $_startline + $linecount -1]
}
