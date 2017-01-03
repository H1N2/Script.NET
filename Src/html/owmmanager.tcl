#################################################################
# File       : owmmanager.tcl									#
# Author     : Blueant Software									#
# Version    : 1.0												#
# Date       : 2006-05-04										#
# Description: Script.NET owmmanager page script				#
#################################################################

package require TclFace

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
class  TLFAppPage {
	inherit TLFPage
	
	### public methods ###
	public method  OnCreate {};
}

#-------------------------------------------------------------
#	����ҳ��
#-------------------------------------------------------------
body TLFAppPage::OnCreate {} {

	# ����OWM�������
	
	set lsVci [vci_info -listinstalled]
	set lsOwm {}
	foreach vciId $lsVci {
		set vcitype [vci_exec plat GetVciType $vciId]
		if {$vcitype == "owm"} {
			lappend lsOwm $vciId
		}
	}
	
	LangString STR_LINE1 chinese "һ����[llength $lsOwm]����������"
	LangString STR_LINE1 english "Total [llength $lsOwm] UI plugins:"
	set objTitle [CreateObject TLFText "<b>$STR_LINE1</b>"]
	$objTitle SetColor #000040
	
	# OWM�б�
	set tableOwm [CreateObject TLFTable]
	#$tableOwm SetGridColor #000080
		#$tableOwm SetTitleColor #008040
	#$tableOwm SetTitleBgColor #a0f0b0
		#$tableOwm SetLineColor #000040
	#$tableOwm SetLineBgColor #f0f0a0
	LangStringEC STR_COL1 "Plugin Id" "���ID"
	$tableOwm AddColumn $STR_COL1
	LangStringEC STR_COL2 "Name" "ģ����"
	$tableOwm AddColumn $STR_COL2
	LangStringEC STR_COL3 "Version" "ģ��汾"
	$tableOwm AddColumn $STR_COL3
	LangStringEC STR_COL4 "Provider" "������"
	$tableOwm AddColumn $STR_COL4
	foreach owmId $lsOwm {
		set owmName [vci_exec plat GetVciName $owmId]
		set owmVersion [vci_exec plat GetVciVersion $owmId]
		set owmProvider [vci_exec plat GetVciProvider $owmId]
		$tableOwm AddLine2 [list $owmId "" "tcl:<file>PLAT:///html/owm.tcl?vci=$owmId"] [list $owmName] [list $owmVersion] [list $owmProvider]
	}
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "����������"
LangString STR_PAGE_TITLE english "UI Plugin Manager"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]

delete object $page;
