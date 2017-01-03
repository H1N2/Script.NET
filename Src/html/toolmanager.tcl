#################################################################
# File       : toolmanager.tcl									#
# Author     : Blueant Software									#
# Version    : 1.0												#
# Date       : 2008-03-30										#
# Description: Script.NET extend tools manager page script		#
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
	global platform_path platform_language

	# �������߹������
	set lsTool {}
	set toolcount [vci_exec plat GetInstalledTool lsTool]
	
	LangString STR_LINE1 chinese "һ����[llength $lsTool]�����ߣ�"
	LangString STR_LINE1 english "Total [llength $lsTool] Tools:"
	set objTitle [CreateObject TLFText "<b>$STR_LINE1</b>"]
	$objTitle SetColor #000040
	
	set tableTool [CreateObject TLFTable]
	LangStringEC STR_TOOL_NAME "Name" "����"
	$tableTool AddColumn $STR_TOOL_NAME
	LangStringEC STR_TOOL_DESC "Description" "����"
	$tableTool AddColumn $STR_TOOL_DESC
	LangStringEC STR_TOOL_VERSION "Version" "�汾"
	$tableTool AddColumn $STR_TOOL_VERSION
	LangStringEC STR_TOOL_PROVIDER "Provider" "������"
	$tableTool AddColumn $STR_TOOL_PROVIDER

	foreach objectDir [glob -nocomplain -directory "$platform_path/tools" -type d *] {
		if {([file exists "$objectDir/tool.xml"]) || ([file exists "$objectDir/tool_cn.xml"])} {
			set toolxml "$objectDir/tool.xml"
			if {($platform_language == "chinese") && ([file exist "$objectDir/tool_cn.xml"])} {
				set toolxml "$objectDir/tool_cn.xml"
			}
			set xml [xml_open file $toolxml]
			set nodeTool [xml_get $xml node ""]
			set id [xml_get $xml nodeAttr -node $nodeTool id]
			set name [xml_get $xml nodeAttr -node $nodeTool name]
			set version [xml_get $xml nodeAttr -node $nodeTool version]
			set provider [xml_get $xml nodeAttr -node $nodeTool "provider-name"]
			set nodeDesc [xml_get $xml node description]
			set desc [xml_get $xml nodeText -node $nodeDesc]
			
			$tableTool AddLine2 [list $name "" "tcl:vci_exec plat RunTool $id" "Run this tool"] [list $desc] [list $version] [list $provider]
			
			xml_close $xml
		}
	}
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "���߹���"
LangString STR_PAGE_TITLE english "Tool Manager"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]

delete object $page;
