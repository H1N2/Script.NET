#########################################################################
# Script.NET TCL������֧�ֽű�											#
# ����:Blueant															#
# �汾:2.2																#
# ����:2009-06-30														#
#########################################################################
# ע:1.�˽ű�ΪScript.NET TCL������ʹ��,�����Ҫ�ı�������ı���		#
#    �����޸Ĵ˽ű�,�����뱣֤���к��������ֲ���						#
#    2.Ϊ�˱�������,�˽ű��������Զ������������_plat��Ϊǰ׺,		#
#    ������ǰ׺�ı����˴���ʾ��ʱ�����˵�							#
#########################################################################

#-------------------------------------------------------------
# ���ص�source����
#-------------------------------------------------------------
proc source_debug { args } {
	# �����������һ���͵���ȱʡ��source����
	if {[llength $args] != 1} {
		source_std $args
	}
	
	# ����ƽ̨��չ��source����(����֧�ֶ��ļ�����)
	plat_debug_source [lindex $args 0]
}

#-------------------------------------------------------------
# �鿴�����б�ű�
#-------------------------------------------------------------
set _plat_Tcl_WatchVariables {
	
	LangStringEC _plat_win_var "Variable" "����"
	
	set _plat_nOT [output -GetInfo "$_plat_win_var" window-id];

	# ���������
	output -format -$_plat_nOT "DelItem ALL";

	# ��ȡ��ǰ��������
	set _plat_interpName [uplevel #0 {set interpName}];

	# ���ñ���˫���Ľű�(��ͨ�ڵ�)
	set _plat_script_item {
		LangStringEC _plat_win_var "Variable" "����"
		LangStringEC _plat_str_watchvar "Watch Variable" "�鿴����"
		LangStringEC _plat_str_varname "Variable Name" "������"
		LangStringEC _plat_str_varvalue "Variable Value" "����ֵ"
		
		set _plat_hItem [output -GetInfo "$_plat_win_var" HotItem];
		set _plat_varname [output -GetInfo "$_plat_win_var" <#$_plat_hItem,0>];
		set _plat_hParentItem [output -GetInfo "$_plat_win_var" "ItemParent $_plat_hItem"];
		if { $_plat_hParentItem == "ROOT" } {
			set _plat_varvalue [set $_plat_varname];
		} else {
			set _plat_parentname [output -GetInfo "$_plat_win_var" <#$_plat_hParentItem,0>];
			set _plat_item1 "";
			set _plat_varvalue "";
			foreach {_plat_item1 _plat_varvalue} [array get $_plat_parentname $_plat_varname] {}
			unset _plat_item1 _plat_parentname;
		}
		MessageBox -MBOK -MB_ICONINFORMATION -title "$_plat_str_watchvar" \
					"$_plat_str_varname:$_plat_varname\n$_plat_str_varvalue:$_plat_varvalue";
		unset _plat_hItem _plat_varname _plat_varvalue;
		unset _plat_win_var _plat_str_watchvar _plat_str_varname _plat_str_varvalue;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";
	
	# ���ñ���˫���Ľű�(����ڵ�),˫��ʱ�Ŵ����ӽڵ�
	set _plat_script_item {
		LangStringEC _plat_win_var "Variable" "����"
		
		set _plat_hItem [output -GetInfo "$_plat_win_var" SelItem];
		set _plat_varname [output -GetInfo "$_plat_win_var" <#$_plat_hItem,0>];
		set _plat_item_child_count [output -GetInfo "$_plat_win_var" "ChildCount $_plat_hItem"];
		if {$_plat_item_child_count == 0} {
			# ��������Ԫ��
			set _plat_item1 "";
			set _plat_item_var "";
			foreach {_plat_item1 _plat_item_var} [array get $_plat_varname] {
				output "$_plat_win_var" "<$_plat_hItem,7>$_plat_item1,$_plat_item_var";
				set _plat_lasthItem [output -GetInfo "$_plat_win_var" LastItem];
				output "$_plat_win_var" "<I>Set Script<#$_plat_lasthItem> <1>";
				unset _plat_lasthItem;
			}
			output "$_plat_win_var" "<T>ExpandItem<#$_plat_hItem>";
			unset _plat_item1 _plat_item_var;
		}
		unset _plat_hItem _plat_varname _plat_item_child_count _plat_win_var;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";

	unset _plat_interpName _plat_script_item;

	# ��Ӿֲ�����
	set _plat_varcount 0;
	set _plat_var "";
	
	foreach _plat_var [info local] {
		if { [info exist $_plat_var] && [string first "_plat_" $_plat_var]==-1 } {
			if { [array exists $_plat_var] } {
				output -$_plat_nOT "<ROOT,4>$_plat_var,{...}" -direct;
				output -$_plat_nOT "<I>Set Item<$_plat_varcount,1> COLOR 5";
				output -$_plat_nOT "<I>Set Script<$_plat_varcount> <2>";
				incr _plat_varcount;
			} else {
				output -$_plat_nOT "<ROOT,5>$_plat_var,[set $_plat_var]" -direct;
				output -$_plat_nOT "<I>Set Script<$_plat_varcount> <1>";
				incr _plat_varcount;
			}
		}
	}

	# ���ȫ�ֱ���
	foreach _plat_var [info globals] {
		if { [info exist $_plat_var] && [string first "_plat_" $_plat_var]==-1 } {
			if { [array exists $_plat_var] } {
				output -$_plat_nOT "<ROOT,4>$_plat_var,{...}" -direct;
				output -$_plat_nOT "<I>Set Item<$_plat_varcount,1> COLOR 5";
				output -$_plat_nOT "<I>Set Script<$_plat_varcount> <2>";
				incr _plat_varcount;
			} else {
				output -$_plat_nOT "<ROOT,5>$_plat_var,[set $_plat_var]" -direct;
				output -$_plat_nOT "<I>Set Script<$_plat_varcount> <1>";
				incr _plat_varcount;
			}
		}
	}

	# ������ʱ����
	unset _plat_nOT _plat_var _plat_varcount _plat_win_var;
}

#-------------------------------------------------------------
# �鿴�����б�ű�
#-------------------------------------------------------------
set _plat_Tcl_WatchObjects {
	LangStringEC _plat_win_object "Object" "����"
	
	set _plat_nOT [output -GetInfo "$_plat_win_object" window-id];

	# ���������
	output -format -$_plat_nOT "DelItem ALL";

	# ��ȡ��ǰ��������
	set _plat_interpName [uplevel #0 {set interpName}];
		
	# ���ñ���˫���Ľű�(����ڵ�)
	set _plat_script_item {
		LangStringEC _plat_win_object "Object" "����"
		
		set _plat_hItem [output -GetInfo "$_plat_win_object" SelItem];
		set _plat_varname [output -GetInfo "$_plat_win_object" <#$_plat_hItem,0>];
		set _plat_item_child_count [output -GetInfo "$_plat_win_object" "ChildCount $_plat_hItem"];
		if { $_plat_item_child_count == 0 } {
			if [catch { [$_plat_varname] }] {
				global errorInfo;
				# ʹ��������ʽƥ���������з����������ӽڵ�
				# �������ȫ��������,��_plat_varnameǰ����::�������־,����ķ�����ʧ��
				set _plat_reg "\\n *$_plat_varname (\[^\\n\]*)\\n";
				set _plat_ls [regexp -all -inline $_plat_reg $errorInfo];
				set _plat_temp "";
				set _plat_method "";
				foreach {_plat_temp _plat_method} $_plat_ls {
					set _plat_spacepos [string first " " $_plat_method];
					set _plat_param "";
					if { $_plat_spacepos != -1 } {
						incr _plat_spacepos;
						set _plat_param [string range $_plat_method $_plat_spacepos end];
						incr _plat_spacepos -1;
						set _plat_method [string range $_plat_method 0 $_plat_spacepos];
					}
					output "$_plat_win_object" "<$_plat_hItem,7>$_plat_method,$_plat_param";
					set _plat_lasthItem [output -GetInfo "$_plat_win_object" LastItem];
					output "$_plat_win_object" "<I>Set Script<#$_plat_lasthItem> <3>";
					unset _plat_lasthItem;
				}
				output "$_plat_win_object" "<T>ExpandItem<#$_plat_hItem>";
				unset _plat_reg _plat_ls _plat_temp _plat_method;
			}
		}
		unset _plat_hItem _plat_varname _plat_item_child_count _plat_win_object;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";
	
	# ���ö���˫���Ľű�(����ڵ�),˫��ʱչ�������б�
	set _plat_script_item {
		LangStringEC _plat_win_object "Object" "����"
		
		set _plat_hItem [output -GetInfo "$_plat_win_object" SelItem];
		set _plat_classname [output -GetInfo "$_plat_win_object" <#$_plat_hItem,0>];
		set _plat_item_child_count [output -GetInfo "$_plat_win_object" "ChildCount $_plat_hItem"];
		if {$_plat_item_child_count == 0} {
			# ���iTcl����
			foreach _plat_object [itcl::find objects -class $_plat_classname] {
				output "$_plat_win_object" "<$_plat_hItem,28>$_plat_object,{...}";
				set _plat_lasthItem [output -GetInfo "$_plat_win_object" LastItem];
				output "$_plat_win_object" "<I>Set Item<#$_plat_lasthItem,1> COLOR 5";
				output "$_plat_win_object" "<I>Set Script<#$_plat_lasthItem> <1>";
				unset _plat_lasthItem;
			}
		}
		output "$_plat_win_object" "<T>ExpandItem<$_plat_hItem>";
		unset _plat_hItem _plat_classname _plat_object _plat_win_object _plat_item_child_count;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";
	
	# ���ñ���˫���Ľű�(���󷽷��ڵ�),˫��ʱִ�з���
	set _plat_script_item {
		LangStringEC _plat_win_object "Object" "����"
		
		set _plat_hItem [output -GetInfo "$_plat_win_object" SelItem];
		set _plat_method [output -GetInfo "$_plat_win_object" <#$_plat_hItem,0>];
		set _plat_param [output -GetInfo "$_plat_win_object" <#$_plat_hItem,1>];
		if { [string length $_plat_param] == 0 } {
			set _plat_hParentItem [output -GetInfo "$_plat_win_object" "ItemParent $_plat_hItem"];
			set _plat_parentname [output -GetInfo "$_plat_win_object" <#$_plat_hParentItem,0>];
			eval "$_plat_parentname $_plat_method";
			unset _plat_hParentItem _plat_parentname;
		}		
		unset _plat_hItem _plat_method _plat_param _plat_win_object;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";

	unset _plat_interpName _plat_script_item;

	# ���iTcl��
	set _plat_varcount 0;
	set _plat_class "";
	foreach _plat_class [itcl::find classes] {
		if { [llength [itcl::find objects -class $_plat_class]] > 0 } {
			output -$_plat_nOT "<ROOT,44>$_plat_class,{class}";
		} else {
			output -$_plat_nOT "<ROOT,30>$_plat_class,{class-no object}";
		}
		set _plat_lasthItem [output -GetInfo -$_plat_nOT LastItem];
		output -$_plat_nOT "<I>Set Item<$_plat_varcount,1> COLOR 5";
		output -$_plat_nOT "<I>Set Script<$_plat_varcount> <2>";
		incr _plat_varcount;
		set _plat_object "";
		# ���iTcl����
		# foreach _plat_object [itcl::find objects -class $_plat_class] {
			# output -$_plat_nOT "<$_plat_lasthItem,45>$_plat_object,{...}";
			# output -$_plat_nOT "<I>Set Item<$_plat_varcount,1> COLOR 5";
			# output -$_plat_nOT "<I>Set Script<$_plat_varcount> <1>";
			# incr _plat_varcount;
		# }
		
		unset _plat_lasthItem _plat_index _plat_object;		
	}

	# ������ʱ����
	unset _plat_nOT _plat_class _plat_varcount _plat_win_object;
}

#-------------------------------------------------------------
# �鿴�����б�ű�
#-------------------------------------------------------------
set _plat_Tcl_WatchProcedures {
	LangStringEC _plat_win_proc "Function" "����"
	
	set _plat_nOT [output -GetInfo "$_plat_win_proc" window-id];

	# ���������
	output -format -$_plat_nOT "DelItem ALL";

	# ��ȡ��ǰ��������
	set _plat_interpName [uplevel #0 {set interpName}];

	# ���ù���˫���Ľű�,˫��ʱ���������ӽڵ�
	set _plat_script_item {
		LangStringEC _plat_win_proc "Function" "����"
		
		set _plat_hItem [output -GetInfo "$_plat_win_proc" SelItem];
		set _plat_proc [output -GetInfo "$_plat_win_proc" <#$_plat_hItem,0>];
		set _plat_itemcount [output -GetInfo "$_plat_win_proc" "ItemCount $_plat_hItem"];
		if { $_plat_itemcount == 0 } {
			# ��Ӳ����ӽڵ�
			set _plat_param "";
			foreach {_plat_param} [info args $_plat_proc] {
				output "$_plat_win_proc" "<$_plat_hItem,7>$_plat_param";
			}
			output "$_plat_win_proc" "<T>ExpandItem<#$_plat_hItem>";
			unset _plat_param;
		}
		unset _plat_proc _plat_hItem _plat_itemcount;
	}
	output -$_plat_nOT "<I>Set Script<0> <<$_plat_interpName>>$_plat_script_item";

	unset _plat_interpName _plat_script_item;

	# ��ӹ���
	set _plat_proccount 0;
	set _plat_proc "";
	foreach _plat_proc [info procs] {
		output -$_plat_nOT "<ROOT,29>$_plat_proc";
		output -$_plat_nOT "<I>Set Script<$_plat_proccount> <1>";
		incr _plat_proccount;
	}
	
	# ������ʱ����
	unset _plat_nOT _plat_proc _plat_proccount _plat_win_proc;
}

#-------------------------------------------------------------
# �鿴��ջ�ű�
#-------------------------------------------------------------
set _plat_Tcl_WatchCallStack {
	LangStringEC _plat_win_stack "Stack" "��ջ"
	LangStringEC _plat_str_globalspace "Global Space" "ȫ�ֿռ�"
	
	set _plat_nOT [output -GetInfo "$_plat_win_stack" window-id];

	# ���������
	output -format -$_plat_nOT "DelItem ALL";

	# ������ջ��
	set _plat_level [info level];
	for {set _plat_i $_plat_level} {$_plat_i >= 1} {incr _plat_i -1} {
		set _plat_stack [info level $_plat_i];
		set _plat_spacepos [string first " " $_plat_stack];
		set _plat_param "";
		if { $_plat_spacepos != -1 } {
			incr _plat_spacepos;
			set _plat_param [string range $_plat_stack $_plat_spacepos end];
			incr _plat_spacepos -1;
			set _plat_stack [string range $_plat_stack 0 $_plat_spacepos];
		}
		output -$_plat_nOT "<ROOT,27>$_plat_stack,$_plat_param";
		
		unset _plat_stack _plat_spacepos _plat_param;
	}
	output -$_plat_nOT "<ROOT,27>$_plat_str_globalspace,{null}";
	
	# ������ʱ����
	unset _plat_nOT _plat_level _plat_i _plat_win_stack _plat_str_globalspace;
}
