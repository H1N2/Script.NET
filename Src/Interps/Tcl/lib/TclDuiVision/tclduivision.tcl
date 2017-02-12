#################################################################
# File       : TclDuiVision.tcl
# Author     : Blueant
# Version    : 1.0.20170128
# Date       : 2017-01-28
# Description: Tcl DuiVision script library
#################################################################

package provide TclDuiVision 1.0

package require Itcl

catch {namespace import itcl::*}

# DuiVision����ض���
namespace eval DUIVISION {
# ��Ϣ��������
set MSG_BUTTON_DOWN				1	;#��������ڿؼ�����
set MSG_BUTTON_UP				2	;#��������ڿؼ��ſ�
set MSG_BUTTON_DBLCLK			3	;#����ڿؼ�˫��
set MSG_BUTTON_CHECK			4	;#������Ϣ
set MSG_SCROLL_CHANGE			5	;#������λ�ñ���¼�
set MSG_CONTROL_BUTTON			6	;#�ؼ��ڵİ�ť����¼�
set MSG_MOUSE_MOVE				7	;#����ƶ��¼�
set MSG_MOUSE_LEAVE				8	;#����뿪�¼�
set MSG_MOUSE_LDOWN				9	;#�����������¼�
set MSG_MOUSE_LUP				10	;#�������ſ��¼�
set MSG_MOUSE_RDOWN				11	;#����Ҽ������¼�
set MSG_MOUSE_RUP				12	;#����Ҽ��ſ��¼�
set MSG_KEY_DOWN				13	;#���̰����¼�
set MSG_CONTROL_EVENT			14	;#�ؼ����Զ����¼�
set MSG_MOUSE_RDBLCLK			15	;#����Ҽ�˫��
set MSG_CONTROL_SELECT			16	;#�ؼ��ڵ�ѡ���¼�
set MSG_CONTROL_DELETE			17	;#�ؼ��ڵ�ɾ���¼�
set MSG_DROP_FILE				18	;#��ק�ļ��¼�
set MSG_FOCUS_CHANGE			19	;#�ؼ��Ľ���״̬����¼�
set MSG_KEY_UP					20	;#���̷ſ��¼�
}

#-------------------------------------------------------------
# TDuiHandler class define
#-------------------------------------------------------------
if {[itcl::find classes TDuiHandler] != "TDuiHandler"} {
class  TDuiHandler {
	constructor {} {};
	destructor {};
	
	### data member ###
	protected variable _logLevel;		# ��־����
	protected variable _objDuiObject;	# Handler������DuiVision����ָ��
	protected variable _objHandler;		# C++�¼��������ָ��
	protected variable _objTDuiHandler;	# Tcl�¼����������
	protected variable _xmlTemplate;	# XML�ļ���������
	protected variable _lsTimer;		# ������DUI��ʱ���б�
	protected variable _lsMsgMethod;	# DUI��Ϣ��Handler������ӳ���
	
	### public methods ###
	public method  GetClassName {};		# ��ȡ����
	public method  SetDuiObject {objDuiObject};	# ����DuiVision����ָ��
	public method  SetLogLevel {logLevel};		# ������־����
	public method  StartTimer {timerName time};	# ������ʱ��
	public method  StopTimer {timerName};		# ֹͣ��ʱ��
	public method  MsgMethod {name msg method};	# ���DUI��Ϣ��ӳ�䷽������
	public method  OnInit {};			# �¼���������ʼ������,C++����
	public method  OnMessage {name msg wParam lParam};# DUI��Ϣ������
	public method  OnTimer {name};		# ��ʱ��������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TDuiHandler::constructor {} {
	chain;
	
	set _logLevel "INFO"
	set _objDuiObject ""
	set _xmlTemplate ""
	set _lsTimer {}
	set _lsMsgMethod {}
	
	# ��ȡiTcl�¼����������,����������ȡ��һ��������
	set lsObjTDuiHandler [itcl::find objects -class [GetClassName]]
	if {[llength $lsObjTDuiHandler] > 0} {
		set _objTDuiHandler [lindex $lsObjTDuiHandler 0]
	}
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
body TDuiHandler::destructor {} {
	foreach timer $_lsTimer {
		dui::timer stop $timer
	}
}

#-------------------------------------------------------------
# ��ȡ����
#-------------------------------------------------------------
body TDuiHandler::GetClassName {} {
	set classname [$this info class];
	if {[string first "::" $classname] == 0} {
		set classname [string range $classname 2 end];
	}
	return $classname;
}

#-------------------------------------------------------------
# ����DuiVision����ָ��
#-------------------------------------------------------------
body TDuiHandler::SetDuiObject {objDuiObject} {
	set _objDuiObject $objDuiObject
}

#-------------------------------------------------------------
#	������־����
#-------------------------------------------------------------
body TDuiHandler::SetLogLevel {logLevel} {
	set _logLevel $logLevel
}

#-------------------------------------------------------------
#	������ʱ��
#-------------------------------------------------------------
body TDuiHandler::StartTimer {timerName time} {
	dui::timer add $timerName $time
	lappend _lsTimer $timerName
}

#-------------------------------------------------------------
#	ֹͣ��ʱ��
#-------------------------------------------------------------
body TDuiHandler::StopTimer {timerName} {
	dui::timer stop $timerName
}

#-------------------------------------------------------------
#	���DUI��Ϣ��ӳ�䷽������
#-------------------------------------------------------------
body TDuiHandler::MsgMethod {name msg method} {
	lappend _lsMsgMethod [list $name $msg $method]
}

#-------------------------------------------------------------
#	��ʼ��
#-------------------------------------------------------------
body TDuiHandler::OnInit {} {
}

#-------------------------------------------------------------
#	DUI��Ϣ����
#-------------------------------------------------------------
body TDuiHandler::OnMessage {name msg wParam lParam} {
	if {$_logLevel == "DEBUG"} {
		puts "TDuiHandler::OnMessage $name $msg $wParam $lParam"
	}
	log -level DEBUG "TDuiHandler::OnMessage $name $msg $wParam $lParam"
	foreach msgMethod $_lsMsgMethod {
		set _name [lindex $msgMethod 0]
		set _msg [lindex $msgMethod 1]
		set _method [lindex $msgMethod 2]
		if {($_name == $name) && ($_msg == $msg) && ($_method != "")} {
			$_objTDuiHandler $_method $name $msg $wParam $lParam
		}
	}
}

#-------------------------------------------------------------
#	DUI��ʱ��������
#-------------------------------------------------------------
body TDuiHandler::OnTimer {name} {
}

#-------------------------------------------------------------
# TDuiDialogHandler class define
#-------------------------------------------------------------
if {[itcl::find classes TDuiDialogHandler] != "TDuiDialogHandler"} {
class  TDuiDialogHandler {
	inherit TDuiHandler
	
	constructor {{xmlTemplate ""}} {};
	destructor {};
	
	### data member ###
	
	### public methods ###
	public method  GetControl {name};	# ��ȡ�Ի����ָ���ؼ�ָ��
	public method  ShowDialog {};	# ��ʾ�Ի���
	public method  DoOK {};			# �رնԻ���-OK
	public method  DoCancel {};		# �رնԻ���-Cancel
	public method  DoClose {};		# �رնԻ���-Close
	public method  DoYes {};		# �رնԻ���-Yes
	public method  DoNo {};			# �رնԻ���-No
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TDuiDialogHandler::constructor {{xmlTemplate ""}} {
	chain;
	set _xmlTemplate $xmlTemplate
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
body TDuiDialogHandler::destructor {} {
	chain;
	dui::dialog remove $_objDuiObject
}

#-------------------------------------------------------------
#	��ȡ�Ի����ָ���ؼ�ָ��
#-------------------------------------------------------------
body TDuiDialogHandler::GetControl {name} {
	return [dui::control getcontrol $name -parentDialog $_objDuiObject]
}

#-------------------------------------------------------------
#	��ʾ�Ի���
#-------------------------------------------------------------
body TDuiDialogHandler::ShowDialog {} {
	dui::dialog create $_xmlTemplate -handlerName $_objTDuiHandler
}

#-------------------------------------------------------------
#	�رնԻ���-OK
#-------------------------------------------------------------
body TDuiDialogHandler::DoOK {} {
	dui::dialog close $_objDuiObject -ok
}

#-------------------------------------------------------------
#	�رնԻ���-Cancel
#-------------------------------------------------------------
body TDuiDialogHandler::DoCancel {} {
	dui::dialog close $_objDuiObject -cancel
}

#-------------------------------------------------------------
#	�رնԻ���-Close
#-------------------------------------------------------------
body TDuiDialogHandler::DoClose {} {
	dui::dialog close $_objDuiObject -close
}

#-------------------------------------------------------------
#	�رնԻ���-Yes
#-------------------------------------------------------------
body TDuiDialogHandler::DoYes {} {
	dui::dialog close $_objDuiObject -yes
}

#-------------------------------------------------------------
#	�رնԻ���-No
#-------------------------------------------------------------
body TDuiDialogHandler::DoNo {} {
	dui::dialog close $_objDuiObject -no
}