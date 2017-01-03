#################################################################
# Script.NET VCI�������ҳ��ű�
# ����:blueant
# �汾:2.0
# ����:2006-06-25
#################################################################

package require TclFace

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
class  TLFAppPage {
	inherit TLFPage
	
	constructor {} {};
	
	### data member ###
	protected variable _objTextInfoInstance;
	protected variable _tableInstance;
	protected variable _vciId;		# ���ID
	protected variable _vciVersion;	# ����汾
	protected variable _CurrentObjHandle;
	protected variable _CloseObjHandle;
	protected variable _CurrentFunction;
	
	### public methods ###
	public method  ProcessFormValue {name value};	# ����Form���
	public method  OnInitialUpdate {};				# ҳ���ʼ��
	public method  OnEndAction {};					# ����ִ�����Ĳ���
	public method  CreateVciObject {};				# ���������ʵ��
	public method  CloseInstance {};				# �ر����ʵ��
	public method  ExecFunction {};					# ִ�к���
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TLFAppPage::constructor {} {
	chain;
	#SetFormAction;
	set _vciId "";
	set _vciVersion "";
	set _CurrentObjHandle "";
	set _CloseObjHandle "";
	set _CurrentFunction "";
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
body TLFAppPage::ProcessFormValue {name value} {
	if {[chain $name $value] == true} {
		return true
	}
	switch $name {
		vci		{set _vciId $value}
		version	{set _vciVersion $value}
		handle	{set _CurrentObjHandle $value}
		closehandle {set _CloseObjHandle $value}
		function {set _CurrentFunction $value}
	}
}

#-------------------------------------------------------------
#	ҳ���ʼ��
#-------------------------------------------------------------
body TLFAppPage::OnInitialUpdate {} {
	global platform_path
	
	# ��������ɾ��,����Ϊ��
	if {$_CloseObjHandle == $_CurrentObjHandle} {
		set _CurrentObjHandle ""
	}
	
	# �����,��ѯ�ӿ�����,Ȼ��ر����(����д򿪵�ʵ��,��ֱ��ʹ��)
	set openvci false
	if {[catch {set lsVci [vci_info -listobject $_vciId]}] == 1} {
		set obj [vci_open $_vciId "" $_vciVersion];
		set openvci true
	}
	set lstDef [vci_info -listdef $_vciId];
	set lsFunc [vci_info -listfunc $_vciId];
	set lsVciInfo [vci_info -listinstalled $_vciId $_vciVersion];
	
	# ����JavaScript cookie,RightEdge��
	AppendJavaScript "Cookie" class
	AppendJavaScript "RightEdge" class
	
	LangString STR_WIN_TITLE chinese "ִ�к���"
	LangString STR_WIN_TITLE english "Execute Function"
	set winFunction [CreateObject TLFWindow $STR_WIN_TITLE]
	$winFunction SetSize 350 200
	LangString STR_MSG_1 chinese "�����뺯�� "
	LangString STR_MSG_1 english "Please input function "
	LangString STR_MSG_2 chinese " ������"
	LangString STR_MSG_2 english " parameters:"
	LangString STR_MSG_3 chinese " ����"
	LangString STR_MSG_3 english " Parameters"
	LangString STR_MSG_4 chinese "ִ�к���"
	LangString STR_MSG_4 english "Execute Function"
	set vciExecFunction "\
		function vciExecFunction(funcIndex, funcName, paramNum) {\
			[$winFunction GetObjId].Content = \"<FORM name='[$winFunction GetFullId]' action='tcl:<form><file>$platform_path/html/vci.tcl' onsubmit='processFormSubmit(this)' method=POST>\" + \
				\"<input type=hidden name='vci' value='$_vciId'>\" + \
				\"<input type=hidden name='version' value='$_vciVersion'>\" + \
				\"<input type=hidden name='handle' value='$_CurrentObjHandle'>\" + \
				\"<input type=hidden name='function' value='\" + funcName + \"'>\" + \
				\"<input type=hidden name='action' value='ExecFunction'>\" + \
				\"<font color=#800000><b>$STR_MSG_1\" + \
				funcName + \"$STR_MSG_2<b></font></br>\"; \
			[$winFunction GetObjId].Content += \"<table width='100%' cellSpacing=1 cellPadding=3 bgColor=#7195c4 border=0>\"; \
			for(var i=0; i<paramNum; i++) \
			{ \
				[$winFunction GetObjId].Content += \"<tr><td class=text_all bgColor=#f0f2f6>$STR_MSG_3\" + i + \"</td><td class=text_all bgColor=#f0f2f6>\"; \
				[$winFunction GetObjId].Content += \"<input type=text size=40 name=param\"+i+\"></td></tr>\"; \
			} \
			[$winFunction GetObjId].Content += \"</table>\"; \
			[$winFunction GetObjId].Content += \"<input type=submit name='button_submit' value='$STR_MSG_4'>\"; \
			[$winFunction GetObjId].Content += \"</FORM>\"; \
			[$winFunction Show] \
			new DronFw.Class.RightEdge(\"[$winFunction GetObjId]\"); \
		}\
		"
	if {$_CurrentObjHandle == ""} {
		set vciExecFunction "\
			function vciExecFunction(funcIndex, funcName, paramNum) {\
			}\
			"
	}
	AppendJavaScript "$vciExecFunction" body
	
	LangString STR_INFO chinese "����ӿ� $_vciId\($_vciVersion\) ������ \[<a href=\"tcl:<file>PLAT:///html/vcimanager.tcl\">����б�</a>\]"
	LangString STR_INFO english "Component $_vciId\($_vciVersion\) Decalare: \[<a href=\"tcl:<file>PLAT:///html/vcimanager.tcl\">Component List</a>\]"
	set objTextInfo [AppendText "<b>${STR_INFO}</b>"]
	
	# ���������б�����
	set tableFunction [CreateObject TLFTable]
	$tableFunction SetLineColor #000040
	LangStringEC STR_COL1 "Serial No" "���"
	$tableFunction AddColumn $STR_COL1
	LangStringEC STR_COL2 "Function Declare" "��������"
	$tableFunction AddColumn $STR_COL2
	if { [catch {
		#set lsFunc [vci_info -listfunc $_vciId]
		set index 1
		foreach func $lstDef {
			# ��һ������Ϊ��������,����ʾ
			if {$index != 1} {
				#$tableFunction AddLine2 [expr $index-1] [list $func "" "tcl:<file>PLAT:///html/vci.tcl?vci=$_vciId&version=$_vciVersion"]
				# �������б�
				set funcname [lindex $lsFunc [expr $index-1]]
				set lsParam [vci_info -listparam $_vciId $funcname]
				$tableFunction AddLine2 [expr $index-1]\
					[list $func "" "javascript:vciExecFunction([expr $index-1], \"$funcname\", [llength $lsParam]);"]
			}
			incr index
		}
	} errinfo ] } {
		puts $errinfo
	}
	# �򿪵���������б�
	set _objTextInfoInstance [AppendText ""]
	
	set _tableInstance [CreateObject TLFTable]
	$_tableInstance SetLineColor #000040
	LangStringEC STR_COL1 "Serial No" "���"
	$_tableInstance AddColumn $STR_COL1
	LangStringEC STR_COL2 "Instance Handle" "ʵ�����"
	$_tableInstance AddColumn $STR_COL2
	LangStringEC STR_COL3 "Instance Name" "ʵ����"
	$_tableInstance AddColumn $STR_COL3
	LangStringEC STR_COL4 "Operate" "����"
	$_tableInstance AddColumn $STR_COL4
	
	# ���ע����Ϣ
	LangString STR_INFO chinese "���ע����Ϣ��"
	LangString STR_INFO english "Component Information:"
	set objTextInfo [AppendText "<b>${STR_INFO}</b>"]
	
	set tableInfo [CreateObject TLFTable]
	$tableInfo SetHaveTitle false
	$tableInfo AddColumn ""
	$tableInfo AddColumn ""
	
	catch {
		# �����
		LangString STR_INFO_NAME chinese "�����"
		LangString STR_INFO_NAME english "Component Name"
		$tableInfo AddLine2 [list $STR_INFO_NAME] [list $_vciId]
		# �汾
		LangString STR_INFO_NAME chinese "����汾"
		LangString STR_INFO_NAME english "Component Version"
		$tableInfo AddLine2 [list $STR_INFO_NAME] [list [lindex $lsVciInfo 0]]
		# ����
		LangString STR_INFO_NAME chinese "����"
		LangString STR_INFO_NAME english "Author"
		$tableInfo AddLine2 [list $STR_INFO_NAME] [list [lindex $lsVciInfo 2]]
		# ��������
		#LangString STR_INFO_NAME chinese "��������"
		#LangString STR_INFO_NAME english "Create Date"
		#$tableInfo AddLine2 [list $STR_INFO_NAME] ""
		# ˵��
		LangString STR_INFO_NAME chinese "˵��"
		LangString STR_INFO_NAME english "Description"
		$tableInfo AddLine2 [list $STR_INFO_NAME] [list [lindex $lsVciInfo 5] "" [lindex $lsVciInfo 5]]
		# ͷ�ļ�
		LangString STR_INFO_NAME chinese "ͷ�ļ�"
		LangString STR_INFO_NAME english "Header File"
		set headlink [lindex $lsVciInfo 4]
		regsub -all "/" $headlink "\\" headlink;
		$tableInfo AddLine2 [list $STR_INFO_NAME] [list [lindex $lsVciInfo 4] "" "pfe:open $headlink"]
		# �ֲ�
		#LangString STR_INFO_NAME chinese "�ֲ�"
		#LangString STR_INFO_NAME english "Manual File"
		#$tableInfo AddLine2 [list $STR_INFO_NAME] [list [lindex $lsVciInfo 5] "" [lindex $lsVciInfo 5]]
	}
	
	if {$openvci == "true"} {
		vci_close $obj;
	}
}

#-------------------------------------------------------------
#	����ִ�����Ĳ���
#-------------------------------------------------------------
body TLFAppPage::OnEndAction {} {
	# ˢ�¾���б�������Ϣ
	if { [ catch {
		set lsHandle [vci_info -listobject $_vciId];
	} ] } {
		set lsHandle {};
	}
	set HandleCount [llength $lsHandle];
	
	LangString STR_INFO chinese "��� $_vciId\($_vciVersion\) �Ѿ��򿪵�ʵ�����Ϊ $HandleCount�� <a href=\"tcl:<file>PLAT:///html/vci.tcl?action=CreateVciObject&vci=$_vciId&version=$_vciVersion&handle=$_CurrentObjHandle\">\[���������ʵ��\]</a>"
	LangString STR_INFO english "Component $_vciId\($_vciVersion\) have opened $HandleCount instances. <a href=\"tcl:<file>PLAT:///html/vci.tcl?action=CreateVciObject&vci=$_vciId&version=$_vciVersion&handle=$_CurrentObjHandle\">\[Create new component instance\]</a>"
	$_objTextInfoInstance SetText "<b>${STR_INFO}</b>"

	# ˢ�����ʵ����
	catch {
		set index 1
		foreach handle $lsHandle {
			#if {($_CurrentObjHandle == "") && ($index == 1)} {
				# �����ǰʵ��Ϊ��,��ѵ�һ��ʵ������Ϊ��ǰʵ��
			#	set _CurrentObjHandle $handle
			#} 
			
			if {$handle == $_CurrentObjHandle} {
				# ����Ϊ��ǰ�������
				set serial [list "$index<img border=\"0\" src=\"image/point.gif\">"]
			} else {
				set serial [list "$index"]
			}
			
			set vcihandle [vci_info -getvcihandle $handle]
			set instanceName [vci_exec plat GetInstanceNameByObject $_vciId $vcihandle]
			
			$_tableInstance AddLine2 $serial \
				[list $handle "" "tcl:<file>PLAT:///html/vci.tcl?handle=$handle&vci=$_vciId&version=$_vciVersion"] \
				[list $instanceName] \
				[list "close" "" "tcl:<file>PLAT:///html/vci.tcl?action=CloseInstance&closehandle=$handle&vci=$_vciId&version=$_vciVersion&handle=$_CurrentObjHandle"]
			
			incr index
		}
	}
}

#-------------------------------------------------------------
#	���������ʵ��
#-------------------------------------------------------------
body TLFAppPage::CreateVciObject {} {
	vci_open $_vciId "" $_vciVersion
}

#-------------------------------------------------------------
#	�ر����ʵ��
#-------------------------------------------------------------
body TLFAppPage::CloseInstance {} {
	vci_close $_CloseObjHandle
}

#-------------------------------------------------------------
#	ִ��һ���������
#-------------------------------------------------------------
body TLFAppPage::ExecFunction {} {
	# ��ȡ������Ϣ
	set lsFunc [vci_info -listfunc $_vciId];
	set lsDef [vci_info -listdef $_vciId];
	set funcindex 0
	# �ж��Ƿ��������ĺ���
	set funccount 0;	# ������������
	set index 0;		# ������
	set funcnindex 0;	# �˺��������������б��е�λ��
	foreach fname $lsFunc {
		if {$fname == $_CurrentFunction} {
			incr funccount;
			if {$index == $funcindex} {
				set funcnindex $funccount;
			}
		}
		incr index;
	}
	set funcnname $_CurrentFunction;
	if {$funccount > 1} {
		set funcnname "$_CurrentFunction\($funcnindex\)";
	}
	# �������б�
	set lsParam [vci_info -listparam $_vciId $_CurrentFunction];

	# ���û�в���,��ֱ��ִ��
	if {[llength $lsParam] == 0} {
		puts "###############################################";
		puts "Exec $_vciId $_CurrentFunction:";
		set ret [vci_exec $_CurrentObjHandle $funcnname];
		puts "Result is:$ret";
		puts "###############################################";
		return;
	}

	# �������Ȼ��ִ��
	set index 0;
	foreach param $lsParam {
		# ������ϸ��Ϣ
		set lsParamInfo [vci_info -listparam $_vciId $funcnname $param];
		# ȱʡֵ
		set defaultValue [lindex $lsParamInfo 2];
		set pvalue($param) $defaultValue;
		if {$_actionArgs(param$index) != ""} {
			set pvalue($param) $_actionArgs(param$index)
		}
		incr index;
	}
	puts "###############################################";
	puts "Exec $_vciId $funcnname:";
	
	set execcmd "vci_exec $_CurrentObjHandle $funcnname";
	foreach param $lsParam {
		# ������ϸ��Ϣ
		set lsParamInfo [vci_info -listparam $_vciId $funcnname $param];
		# ���÷�ʽ
		set ParamMode [lindex $lsParamInfo 1];
		if {$ParamMode==1 || $ParamMode==2} {
			# ָ�������������Ҫ����һ��������
			set execcmd "$execcmd pvalue($param)";
			puts "$param=$pvalue($param); type=$ParamMode";
		} else {
			# ֱ�ӵ���,ֻҪ���ݱ���ֵ�Ϳ�����
			set execcmd "$execcmd $pvalue($param)";
			puts "$param=$pvalue($param)";
		}			
	}

	puts "$execcmd";
	# ִ�к�������
	set ret [eval $execcmd];
	puts "Result is:$ret";

	# ���ָ������õķ���ֵ		
	foreach param $lsParam {
		# ������ϸ��Ϣ
		set lsParamInfo [vci_info -listparam $_vciId $funcnname $param];
		# ���÷�ʽ
		set ParamMode [lindex $lsParamInfo 1];
		if {$ParamMode==1 || $ParamMode==2} {
			#set pvar($param) $pvalue($param);
			puts "$param=$pvalue($param)";
		}
	}

	puts "###############################################";
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "�����Ϣ"
LangString STR_PAGE_TITLE english "Component Information"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]
delete object $page;
