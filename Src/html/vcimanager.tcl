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

	# ����VCI�������
	
	set lsvci [vci_info -listinstalled]
	set lsVci {}
	foreach vciId $lsvci {
		set vcitype [vci_exec plat GetVciType $vciId]
		if {($vcitype != "owm") && ($vcitype != "interp")} {
			lappend lsVci $vciId
		}
	}
	
	LangString STR_LINE1 chinese "һ����[llength $lsVci]��VCI�����"
	LangString STR_LINE1 english "Total [llength $lsVci] VCI Components:"
	set objTitle [CreateObject TLFText "<b>$STR_LINE1</b>"]
	$objTitle SetColor #000040
	
	set tableVci [CreateObject TLFTable]
	#$tableVci SetGridColor #000080
		#$tableVci SetTitleColor #008040
	#$tableVci SetTitleBgColor #a0f0b0
		#$tableVci SetLineColor #000040
	#$tableVci SetLineBgColor #f0f0a0
	LangStringEC STR_COL1 "Vci Id" "���ID"
	$tableVci AddColumn $STR_COL1
	LangStringEC STR_COL2 "Name" "�����"
	$tableVci AddColumn $STR_COL2
	LangStringEC STR_COL3 "Version" "����汾"
	$tableVci AddColumn $STR_COL3
	LangStringEC STR_COL4 "Provider" "������"
	$tableVci AddColumn $STR_COL4
	LangStringEC STR_COL5 "Instances" "ʵ����"
	$tableVci AddColumn $STR_COL5
	foreach vciId $lsVci {
		set vciName [vci_exec plat GetVciName $vciId]
		set vciVersion [vci_exec plat GetVciVersion $vciId]
		set vciProvider [vci_exec plat GetVciProvider $vciId]
		set lsInstance {}
		catch {set lsInstance [vci_info -listobject $vciId]}
		set instanceNumber [llength $lsInstance]
		$tableVci AddLine2 [list $vciId "" "tcl:<file>PLAT:///html/vci.tcl?vci=$vciId&version=$vciVersion"] [list $vciName] [list $vciVersion] [list $vciProvider] [list $instanceNumber]
	}
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "VCI�������"
LangString STR_PAGE_TITLE english "VCI Manager"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]
delete object $page;
