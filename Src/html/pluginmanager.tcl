#################################################################
# File       : pluginmanager.tcl								#
# Author     : Blueant											#
# Version    : 1.0												#
# Date       : 2009-6-04										#
# Description: plugin manager page script						#
#################################################################

package require TclFace

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
class  TLFAppPage {
	inherit TLFPage
	
	constructor {} {};
	
	### data member ###
		
	### public methods ###
	public method  OnCreate {};						# ����ҳ��
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TLFAppPage::constructor {} {
	chain;
}

#-------------------------------------------------------------
#	����ҳ��
#-------------------------------------------------------------
body TLFAppPage::OnCreate {} {
	
	# ������Ϣ����
	LangStringEC STR_INFO_1 "Plugin Manager" "�������"
	LangString STR_INFO_2 english "Management VCI plugin, UI plugin, Interpreter plugin."
	LangString STR_INFO_2 chinese "����VCI�������������������������"
	# ������������
	set tableIntro [CreateObject TLFTable]
	$tableIntro SetHaveTitle false
	$tableIntro SetGridColor #FFFFFF
	$tableIntro SetItemStyle "vertical-align:top;"
	$tableIntro AddColumn ""
	$tableIntro AddColumn "" "" "" "" "100%"
	$tableIntro AddLine "<img src='[file dirname [info script]]/image/plugin.jpg' border=0>" ""
	set tableIntroText [$tableIntro AppendItemObject 0 1 TLFTable]
	$tableIntroText SetHaveTitle false
	$tableIntroText SetGridColor #FFFFFF
	$tableIntroText AddColumn ""
	$tableIntroText AddLine "<b>$STR_INFO_1</b>"
	$tableIntroText AddLine "$STR_INFO_2"
	$tableIntroText SetOneColumnStyle 0 "" #000080

	set tableIntro [CreateObject TLFTable]
	$tableIntro SetHaveTitle false
	$tableIntro SetGridColor #FFFFFF
	$tableIntro SetItemStyle "text-align:center;"
	$tableIntro AddColumn "" "" "" "" "33%"
	$tableIntro AddColumn "" "" "" "" "33%"
	$tableIntro AddColumn "" "" "" "" "33%"
	$tableIntro AddLine2 \
		[list "<img src='[file dirname [info script]]/image/vci.jpg' border=0>" "" "tcl:<file>PLAT:///html/vcimanager.tcl"] \
		[list "<img src='[file dirname [info script]]/image/owmbig.jpg' border=0>" "" "tcl:<file>PLAT:///html/owmmanager.tcl"] \
		[list "<img src='[file dirname [info script]]/image/interp.jpg' border=0>" "" "tcl:<file>PLAT:///html/interpmanager.tcl"]
	LangStringEC STR_MGR_VCI "VCI Plugin Manager" "VCI�������"
	LangStringEC STR_MGR_OWM "UI Plugin Manager" "�����������"
	LangStringEC STR_MGR_INTERP "Interpreter Plugin Manager" "����������"
	$tableIntro AddLine $STR_MGR_VCI $STR_MGR_OWM $STR_MGR_INTERP

}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "�������"
LangString STR_PAGE_TITLE english "Plugin Manager"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]
delete object $page;
