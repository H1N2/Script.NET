#################################################################
# File       : extendtools.tcl									#
# Author     : Blueant Software									#
# Version    : 1.0												#
# Date       : 2007-06-10										#
# Description: Script.NET extend tools page script				#
#################################################################

package require TclFace
package require Mk4tcl

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
class  TLFAppPage {
	inherit TLFPage
	
	constructor {} {};
	
	### data member ###
	protected variable _lsSelectTool;	# ѡ��Ĺ���
	protected variable _CurrentToolIndex;# ��ǰѡ����
	protected variable _objInputToolName;
	protected variable _objInputToolDesc;
	protected variable _objInputToolLink;
	protected variable _objInputToolVersion;
	protected variable _objInputToolProvider;
	protected variable _objSelectToolLanguage;
	protected variable _winModify;
	
	### public methods ###
	public method  ProcessFormValue {name value};	# ����Form���
	public method  OnCreate {};
	public method  OnEndAction {};					# ����ִ�����Ĳ���
	public method  OnAddTool {};
	public method  OnRemoveTool {};
	public method  OnModifyTool {};
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TLFAppPage::constructor {} {
	chain;
	#SetEnableSerial true;
	SetFormAction;
	set _lsSelectTool {};
	set _CurrentToolIndex -1;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
body TLFAppPage::ProcessFormValue {name value} {
	if {[chain $name $value] == true} {
		return true
	}
	switch -glob $name {
		"#*"	{lappend _lsSelectTool [string range $name 1 end]}
		"index"	{set _CurrentToolIndex $value}
	}
}

#-------------------------------------------------------------
#	����ҳ��
#-------------------------------------------------------------
body TLFAppPage::OnCreate {} {
	
	# ��ӹ��ߴ���
	LangStringEC STR_WINTITLE_ADDTOOL "Add Tool" "��ӹ���"
	set win [CreateObject TLFWindow $STR_WINTITLE_ADDTOOL]
	$win SetFormAction;
	$win SetWindowPos 100 80
	#$win SetSize 300 200
	
	set objT [$win CreateObject TLFTable]
	$objT SetHaveTitle false
	$objT AddColumn ""
	$objT AddColumn ""
	LangStringEC STR_TOOL_NAME "Name" "����"
	$objT AddLine2 [list $STR_TOOL_NAME] [list [list TLFInput "" "" 220] object]
	set _objInputToolName [$objT GetItemObject 0 1]
	LangStringEC STR_TOOL_DESC "Description" "����"
	$objT AddLine2 [list $STR_TOOL_DESC] [list [list TLFInput "" "" 220] object]
	set _objInputToolDesc [$objT GetItemObject 1 1]
	LangStringEC STR_TOOL_LINK "Action Link" "��������"
	$objT AddLine2 [list $STR_TOOL_LINK] [list [list TLFInput "" "" 220] object]
	set _objInputToolLink [$objT GetItemObject 2 1]
	LangStringEC STR_TOOL_VERSION "Version" "�汾"
	$objT AddLine2 [list $STR_TOOL_VERSION] [list [list TLFInput "" "" 220] object]
	set _objInputToolVersion [$objT GetItemObject 3 1]
	LangStringEC STR_TOOL_PROVIDER "Provider" "������"
	$objT AddLine2 [list $STR_TOOL_PROVIDER] [list [list TLFInput "" "" 220] object]
	set _objInputToolProvider [$objT GetItemObject 4 1]
	LangStringEC STR_TOOL_LANGUAGE "Language" "����"
	$objT AddLine2 [list $STR_TOOL_LANGUAGE] [list [list TLFSelect 220] object]
	set _objSelectToolLanguage [$objT GetItemObject 5 1]
	LangStringEC STR_TOOL_LANGUAGE_ENGLISH "English" "Ӣ��"
	LangStringEC STR_TOOL_LANGUAGE_CHINESE "Chinese" "����"
	LangStringEC STR_TOOL_LANGUAGE_ALL "All Language" "��������"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_ENGLISH "english"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_CHINESE "chinese"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_ALL "all"
	#$_objSelectToolLanguage SetStyleType ss

	# �ύ
	LangStringEC STR_INFO_NAME "Submit" "�ύ"
	$objT AddLine2 [list $STR_INFO_NAME] [list [list TLFButton $STR_INFO_NAME] object]
	set objButtonSend [$objT GetItemObject 6 1]
	$objButtonSend SetStyle "width:58;height:20;padding:1px;border:0;font-size:12px;background-image:url(image/button.gif)"
	$objButtonSend SetAction "OnAddTool"
	$objT SetOneColumnStyle 0 #ffffd0

	# �޸Ĺ��ߴ���
	LangStringEC STR_WINTITLE_MODIFYTOOL "Modify Tool Information" "�޸Ĺ�����Ϣ"
	set _winModify [CreateObject TLFWindow $STR_WINTITLE_MODIFYTOOL]
	$_winModify SetFormAction;
	$_winModify SetWindowPos 100 80
	#$_winModify SetSize 300 200
	
	set objT [$_winModify CreateObject TLFTable]
	$objT SetHaveTitle false
	$objT AddColumn ""
	$objT AddColumn ""
	LangStringEC STR_TOOL_NAME "Name" "����"
	$objT AddLine2 [list $STR_TOOL_NAME] [list [list TLFInput "" "" 220] object]
	set _objInputToolName [$objT GetItemObject 0 1]
	LangStringEC STR_TOOL_DESC "Description" "����"
	$objT AddLine2 [list $STR_TOOL_DESC] [list [list TLFInput "" "" 220] object]
	set _objInputToolDesc [$objT GetItemObject 1 1]
	LangStringEC STR_TOOL_LINK "Action Link" "��������"
	$objT AddLine2 [list $STR_TOOL_LINK] [list [list TLFInput "" "" 220] object]
	set _objInputToolLink [$objT GetItemObject 2 1]
	LangStringEC STR_TOOL_VERSION "Version" "�汾"
	$objT AddLine2 [list $STR_TOOL_VERSION] [list [list TLFInput "" "" 220] object]
	set _objInputToolVersion [$objT GetItemObject 3 1]
	LangStringEC STR_TOOL_PROVIDER "Provider" "������"
	$objT AddLine2 [list $STR_TOOL_PROVIDER] [list [list TLFInput "" "" 220] object]
	set _objInputToolProvider [$objT GetItemObject 4 1]
	LangStringEC STR_TOOL_LANGUAGE "Language" "����"
	$objT AddLine2 [list $STR_TOOL_LANGUAGE] [list [list TLFSelect 220] object]
	set _objSelectToolLanguage [$objT GetItemObject 5 1]
	LangStringEC STR_TOOL_LANGUAGE_ENGLISH "English" "Ӣ��"
	LangStringEC STR_TOOL_LANGUAGE_CHINESE "Chinese" "����"
	LangStringEC STR_TOOL_LANGUAGE_ALL "All Language" "��������"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_ENGLISH "english"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_CHINESE "chinese"
	$_objSelectToolLanguage AddOption $STR_TOOL_LANGUAGE_ALL "all"

	# �ύ
	LangStringEC STR_INFO_NAME "Submit" "�ύ"
	$objT AddLine2 [list $STR_INFO_NAME] [list [list TLFButton $STR_INFO_NAME] object]
	set objButtonSend [$objT GetItemObject 6 1]
	$objButtonSend SetStyle "width:58;height:20;padding:1px;border:0;font-size:12px;background-image:url(image/button.gif)"
	$objButtonSend SetAction "OnModifyTool"
	$objT SetOneColumnStyle 0 #ffffd0

	# ��Ӻ�ɾ����ť	
	set _objTextButton [AppendText ""]
	LangStringEC STR_BUTTON_ADDTOOL "Add Tool" "��ӹ���"
	LangStringEC STR_BUTTON_REMOVETOOL "Remove Tool" "ɾ������"
	set objAddTool [$_objTextButton CreateObject TLFButton $STR_BUTTON_ADDTOOL]
	$objAddTool SetAction "OnAddTool"
	$objAddTool SetSubmitAction "Show()" "$win"
	set objRemoveTool [$_objTextButton CreateObject TLFButton $STR_BUTTON_REMOVETOOL]
	$objRemoveTool SetAction "OnRemoveTool"
}

#-------------------------------------------------------------
#	����ִ�����Ĳ���
#-------------------------------------------------------------
body TLFAppPage::OnEndAction {} {
	global platform_path platform_language

	# ������չ���߹������
	set mkfile "$platform_path/tools/tools.mk"
	
	# �����б�
	set tableTools [CreateObject TLFTable]
	$tableTools AddColumn "" "" "" "" 16
	LangStringEC STR_COL1 "Name" "������"
	$tableTools AddColumn $STR_COL1
	LangStringEC STR_COL2 "Description" "��������"
	$tableTools AddColumn $STR_COL2
	LangStringEC STR_COL3 "Version" "�汾"
	$tableTools AddColumn $STR_COL3
	LangStringEC STR_COL4 "Provider" "������"
	$tableTools AddColumn $STR_COL4
	LangStringEC STR_COL5 "Modify" "�޸�"
	#$tableTools AddColumn $STR_COL5

	catch {
		mk::file open db $mkfile -readonly
		set vw [mk::view layout db.tools]
		
		set index 0
		set showindex 0
		mk::loop c db.tools {
			set lsRecord [mk::get $c name link desc version provider language]
			#puts $lsRecord
			set itemLanguage [lindex $lsRecord 5]
			if {($itemLanguage == $platform_language) || ($itemLanguage == "all")} {
				set itemCheckBox "<input type='checkbox' name='#$index' value='ON'>"
				set itemDesc [lindex $lsRecord 2]
				set itemVersion [lindex $lsRecord 3]
				set itemProvider [lindex $lsRecord 4]
				$tableTools AddLine2 [list $itemCheckBox] \
								[list [lindex $lsRecord 0] "" [lindex $lsRecord 1]] \
								[list $itemDesc] \
								[list $itemVersion] \
								[list $itemProvider]
								#[list [list TLFButton $STR_COL5] object]
								#[list [list TLFLink "action=OnModifyTool&index=$index" $STR_COL5] object]
				
				set objModifyButton [$tableTools GetItemObject $showindex 5]
				$objModifyButton SetAction "OnModifyTool&index=$index"
				$objModifyButton SetSubmitAction "Show($index)" "$_winModify"
				
				incr showindex
			}
			incr index
		}
		
		mk::file close db
	}
}

#-------------------------------------------------------------
#	��ӹ���
#-------------------------------------------------------------
body TLFAppPage::OnAddTool {} {
	global platform_path

	set szToolName [$_objInputToolName GetInputText]
	set szToolDesc [$_objInputToolDesc GetInputText]
	set szToolLink [$_objInputToolLink GetInputText]
	regsub {PLAT:\\\\} $szToolLink {PLAT://} szToolLink
	set szToolVersion [$_objInputToolVersion GetInputText]
	set szToolProvider [$_objInputToolProvider GetInputText]
	set szToolLanguage [$_objSelectToolLanguage GetSelectValue]
	
	if {($szToolName == "") || ($szToolLink == "")} {
		return
	} 
	
	set mkfile "$platform_path/tools/tools.mk"
	
	catch {
		mk::file open db $mkfile
		mk::view layout db.tools {name link desc version provider language}
		mk::row append db.tools name $szToolName link $szToolLink desc $szToolDesc version $szToolVersion provider $szToolProvider language $szToolLanguage
		mk::file close db
	}
}

#-------------------------------------------------------------
#	ɾ������
#-------------------------------------------------------------
body TLFAppPage::OnRemoveTool {} {
	global platform_path

	set mkfile "$platform_path/tools/tools.mk"
	# ѡ���������,�����ʼɾ��
	set _lsSelectTool [lsort -integer -decreasing $_lsSelectTool]
	
	catch {
		mk::file open db $mkfile
		foreach index $_lsSelectTool {
			mk::row delete db.tools!$index
		}
		mk::file close db
	}
}

#-------------------------------------------------------------
#	�޸Ĺ���
#-------------------------------------------------------------
body TLFAppPage::OnModifyTool {} {
	global platform_path

	puts $_CurrentToolIndex
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "��չ����"
LangString STR_PAGE_TITLE english "Extend Tools"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]

delete object $page;
