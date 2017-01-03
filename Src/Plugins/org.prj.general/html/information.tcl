#################################################################
# File       : information.tcl									
# Author     : blueant											
# Version    : 4.0												
# Date       : 2015-01-18									
# Description: project information page script					
#################################################################

package require TclFace

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
if {[itcl::find classes TLFAppPage] != "TLFAppPage"} {
class  TLFAppPage {
	inherit TLFPage
	
	constructor {} {};
	
	### data member ###
	protected variable tableTest;
	
	### public methods ###
	public method  ProcessFormValue {name value};	# ����Form���
	public method  OnCreate {};						# ����ҳ��
	public method  OnEndAction {};					# ����ִ�����Ĳ���
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TLFAppPage::constructor {} {
	chain;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
body TLFAppPage::ProcessFormValue {name value} {
	if {[chain $name $value] == true} {
		return true
	}
	switch $name {
		project-act	{
			if {$value == "close"} {
				owm -sendcmd -1 75 0 0
			}
		}
	}
}

#-------------------------------------------------------------
#	����ҳ��
#-------------------------------------------------------------
body TLFAppPage::OnCreate {} {

	#set vciprj [vci_exec plat GetObjectByInstanceName "###project###"]
	#set tclvciprj [vci_info -gettclvciobj "org.prj.general" $vciprj]
	set project_name [pproject name]
	set project_desc [pproject desc]
	set project_file [pproject file]
	set project_path [pproject path]
	#vci_exec $tclvciprj GetActiveProject project_name
	#vci_exec $tclvciprj GetProjectInfo $project_name project_desc project_file project_path
	#if {$project_path == ""} {
	#	return
	#}
	
	# ������Ϣ����
	LangStringEC STR_PRJ_NAME "Project - " "���� - "
	LangStringEC STR_PRJ_DESC "Description : " "���� : "
	LangStringEC STR_PRJ_PATH "Path : " "·�� : "
	# ������������
	set tableIntro [CreateObject TLFTable]
	$tableIntro SetHaveTitle false
	$tableIntro SetGridColor #FFFFFF
	$tableIntro AddColumn ""
	$tableIntro AddColumn "" "" "" "" "100%"
	$tableIntro AddLine "<img src='[file dirname [info script]]/image/test.jpg' border=0>" ""
	set tableIntroText [$tableIntro AppendItemObject 0 1 TLFTable]
	$tableIntroText SetHaveTitle false
	$tableIntroText SetGridColor #FFFFFF
	$tableIntroText AddColumn ""
	$tableIntroText AddLine "<b>$STR_PRJ_NAME[varconvert strtoutf8 $project_name]</b>"
	$tableIntroText AddLine "$STR_PRJ_DESC[varconvert strtoutf8 $project_desc]"
	$tableIntroText AddLine "$STR_PRJ_PATH[varconvert strtoutf8 $project_path]"
	$tableIntroText SetOneColumnStyle 0 "" #000080
	
	set tableTest [CreateObject TLFTable]
	LangStringEC STR_COL1 "Test Item" "������"
	$tableTest AddColumn $STR_COL1
	LangStringEC STR_COL2 "Test Result" "���Խ��"
	$tableTest AddColumn $STR_COL2
	
	# ���̲�������
	LangString STR_COMPILE_PROJECT chinese "���빤��"
	LangString STR_COMPILE_PROJECT english "Build Project"
	LangString STR_SAVE_PROJECT chinese "���湤��"
	LangString STR_SAVE_PROJECT english "Save Project"
	LangString STR_CLOSE_PROJECT chinese "�رչ���"
	LangString STR_CLOSE_PROJECT english "Close Project"
	set objText [CreateObject TLFText "\[ <b><a href='tcl:<file>[info script]?project-act=compile'>$STR_COMPILE_PROJECT</a> | <a href='tcl:<file>[info script]?project-act=save'>$STR_SAVE_PROJECT</a> | <a href='tcl:<file>[info script]?project-act=close'>$STR_CLOSE_PROJECT</a></b> \]"]
	$objText SetColor #000040

}

#-------------------------------------------------------------
#	����ִ�����Ĳ���
#-------------------------------------------------------------
body TLFAppPage::OnEndAction {} {

	set STR_TEST_1 "������1"
	set res ""
	if {$res == -1} {set res 2}
	switch $res {		
		0		{set line "<img src='[file dirname [info script]]/image/error.gif' border=0>"}
		1		{set line "<img src='[file dirname [info script]]/image/pass.jpg' border=0>"}
		default	{set line "<img src='[file dirname [info script]]/image/question.gif' border=0>"}
	}
	$tableTest AddLine2 [list $STR_TEST_1 "" "tcl:<file>[info script]?test=1"] [list $line]
	set STR_TEST_2 "������2"
	set res ""
	if {$res == -1} {set res 2}
	switch $res {		
		0		{set line "<img src='[file dirname [info script]]/image/error.gif' border=0>"}
		1		{set line "<img src='[file dirname [info script]]/image/pass.jpg' border=0>"}
		default	{set line "<img src='[file dirname [info script]]/image/question.gif' border=0>"}
	}
	$tableTest AddLine2 [list $STR_TEST_2 "" "tcl:<file>[info script]?test=2"] [list $line]
	set STR_TEST_3 "������3"
	set res ""
	if {$res == -1} {set res 2}
	switch $res {		
		0		{set line "<img src='[file dirname [info script]]/image/error.gif' border=0>"}
		1		{set line "<img src='[file dirname [info script]]/image/pass.jpg' border=0>"}
		default	{set line "<img src='[file dirname [info script]]/image/question.gif' border=0>"}
	}
	$tableTest AddLine2 [list $STR_TEST_3 "" "tcl:<file>[info script]?test=3"] [list $line]

}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "Script.NET������Ϣ"
LangString STR_PAGE_TITLE english "General Project Information"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]
#delete object $page;
