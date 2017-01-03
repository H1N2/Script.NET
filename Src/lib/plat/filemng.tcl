#################################################################
# File       : filemng.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2001-02-04
# Description: Script.NET file manager script
#################################################################

#-------------------------------------------------------------
# ID��ϵ:
# ID=1	: ���ڵ� 
# ID=2	: �ļ��ڵ�
# ID=3	: proc/def�ڵ�
# ID=4	: itcl��/class�ڵ�
# ID=5	: itcl��Ա����/class def�ڵ�
# ID=6	: itcl��Ա�����ڵ�
# ID=10	: VC dsp����
# ID=11	: VC dsp Group
# ID=12	: VC dsp File
#-------------------------------------------------------------

#-------------------------------------------------------------
# �����ļ���ҳ��
#-------------------------------------------------------------
proc _plat_Tcl_CreateFileTree { } {
	global platform_language
	
#	if {[WorkTree treeexist 10006] == 0} {
		# �����������,���˳�
#		return
#	}
	
	# ������
	LangStringEC STR_TREE_NAME "File" "�ļ�"
	set tree [WorkTree gettreeid $STR_TREE_NAME]
	if {$tree == -1} {
		set tree [WorkTree addtree 6 $STR_TREE_NAME]
		return
	}
	#WorkTree closetree $STR_TREE_NAME
	if {$tree == -1} {
		return
	}
	
	# �������ڵ�
	set hRootItem [WorkTree additem $tree ROOT "WorkSpace files" -image(0) -id(1) -type(1) -bold(true)];
	WorkTree expanditem $tree $hRootItem -TIM_ITEM;

	# �ڵ�˫���Ľű�,������һ��TCL����,����3������,�ֱ�Ϊ��ID,�ڵ���,�ڵ�ID
	set dclickscript {
		proc dclick {tree item id} {
			set ItemName [WorkTree getitem name $tree $item -TIM_ITEM];
			set ItemInfo [WorkTree getitem info $tree $item -TIM_ITEM];
			#puts "item=$item,id=$id,ItemName=$ItemName,ItemInfo=$ItemInfo";

			# ���ڵ��򲻼�������
			if {$id == -1 || $id == 1 || $id == 2} {
				return;
			} elseif {$id == 12} {				
				#set res [owm -sendcmd "org.owm.edit" 102 "<string>$ItemInfo" 0];
				set res [owm -sendcmd "" 86 "<string>$ItemInfo" "<string>"];
				return;
			}

			set hItem $item;
			while {[WorkTree getitem type $tree $hItem -TIM_ITEM] != 2 && $hItem != 0} {
				set hItem [WorkTree getitem parent $tree $hItem -TIM_ITEM];
			}
			set filename [WorkTree getitem info $tree $hItem -TIM_ITEM];
			
			# ��ָ���ļ���ת��ָ������
			regsub -all {/} $filename {\\} filename;
			# �����
			owm -sendcmd "org.owm.edit" 102 "<string>$filename" 0;
			# ���ҵ�ָ�����в�����Ϊ��ǰ��
			owm -sendcmd "org.owm.edit" 113 "<string>$filename" "<string>$ItemInfo";
		};dclick}

	WorkTree setdclickscript $tree $dclickscript;
}

#-------------------------------------------------------------
# �ر��ļ���ҳ��
#-------------------------------------------------------------
proc _plat_Tcl_CloseFileTree { } {
	global platform_language
	
	if {$platform_language == "chinese"} {
		WorkTree closetree "�ļ�";
	} else {
		WorkTree closetree "File";
	}
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Tcl�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshTclFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(18) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ����������iTcl��ͳ�Ա�����ڵ�
	set ls [regexp -all -inline -lineanchor {^[ \t]*(?:::itcl::)*class[ \t]+(\w+)[ \t]+\{[ \t]*$} $buf];
	foreach {iline iclass} $ls {
		set hItem1 [WorkTree additem $tree $hItem "$iclass" -id(4) -image(4) -type(4) -info($iline)];
		# ����itcl��ĳ�Ա����
		set exp "^\[ \t\]*(?:::itcl::)*body\[ \t\]+$iclass\::\(\[\\w:\]+\)\[ \t\]+\{.*\}\[ \t\]+\{\[ \t\]*\$";
		set lsMethod [regexp -all -inline -line $exp $buf];
		foreach {mline mname} $lsMethod {
			WorkTree additem $tree $hItem1 "$mname" -id(5) -image(5) -type(5) -info($mline);
		}
	}
	
#	set ls [regexp -all -inline -lineanchor {^[ \t]*::itcl::class[ \t]+(\w+)[ \t]+\{[ \t]*$} $buf];
#	foreach {iline iclass} $ls {
#		set hItem1 [WorkTree additem $tree $hItem "$iclass" -id(4) -image(4) -type(4) -info($iline)];
#		# ����itcl��ĳ�Ա����
#		set exp "^\[ \t\]*::itcl::body\[ \t\]+$iclass\::\(\[\\w:\]+\)\[ \t\]+\{.*\}\[ \t\]+\{\[ \t\]*\$";
#		set lsMethod [regexp -all -inline -line $exp $buf];
#		foreach {mline mname} $lsMethod {
#			WorkTree additem $tree $hItem1 "$mname" -id(5) -image(5) -type(5) -info($mline);
#		}
#	}

	# ���������ɺ����ڵ�
	set ls [regexp -all -inline -line {^[ \t]*proc[ \t]+([\w:]+)[ \t]+\{.*\}[ \t]+\{[ \t]*$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(3) -image(7) -type(3) -info($pline);
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Python�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshPythonFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(19) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set level 0;
	set hPItem $hItem;	# ���ڵ�
	set hLItem $hItem;	# ��һ�β����Ľڵ�
	while {[gets $fi line] >= 0} {
		if {[regexp {(\s*)(def\s+(\w+)\(.*|class\s+(\w+)(\(.*\))?\s*:)} $line all space ldef defname classname classderive] > 0} {
			# �����������
			regsub -all {\t} $space {    } space;
			set newlevel [expr [string length $space] / 4];
			
			if {$newlevel > $level} {
				# �����ӽڵ�
				set hPItem $hLItem;
			} elseif {$newlevel < $level} {
				# ��������һ���ڵ�
				while {$newlevel != $level && $hPItem != $hItem} {
					set hPItem [WorkTree getitem parent $tree $hPItem -TIM_ITEM];
					incr level -1;
				}
			}
			
			# ���ɽڵ�
			if {$defname != ""} {
				if {$newlevel > 0} {
					set id 5;
				} else {
					set id 3;
				}
				set hItem1 [WorkTree additem $tree $hPItem "$defname" -id($id) -image(5) -type($id) -info($line)];
			} else {
				set hItem1 [WorkTree additem $tree $hPItem "$classname" -id(4) -image(4) -type(4) -info($line)];
			}
			
			set hLItem $hItem1;			
			set level $newlevel;
		}
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Perl�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshPerlFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(21) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ���������ɺ����ڵ�
	set ls [regexp -all -inline -line {^[ \t]*sub[ \t]+([\w:]+)[ \t]?.*$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(3) -image(7) -type(3) -info($pline);
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Ruby�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshRubyFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(22) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set level 0;
	set hPItem $hItem;	# ���ڵ�
	set hLItem $hItem;	# ��һ�β����Ľڵ�
	while {[gets $fi line] >= 0} {
		if {[regexp {^(\s*)(def\s+(\w+).*|class\s+(\w+).*)} $line all space ldef defname classname] > 0} {
			# �����������
			regsub -all {\t} $space {    } space;
			set newlevel [expr [string length $space] / 4];
			
			if {$newlevel > $level} {
				# �����ӽڵ�
				set hPItem $hLItem;
			} elseif {$newlevel < $level} {
				# ��������һ���ڵ�
				while {$newlevel != $level && $hPItem != $hItem} {
					set hPItem [WorkTree getitem parent $tree $hPItem -TIM_ITEM];
					incr level -1;
				}
			}
			
			# ���ɽڵ�
			if {$defname != ""} {
				if {$newlevel > 0} {
					set id 5;
				} else {
					set id 3;
				}
				set hItem1 [WorkTree additem $tree $hPItem "$defname" -id($id) -image(5) -type($id) -info($line)];
			} else {
				set hItem1 [WorkTree additem $tree $hPItem "$classname" -id(4) -image(4) -type(4) -info($line)];
			}
			
			set hLItem $hItem1;			
			set level $newlevel;
		}
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Lua�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshLuaFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(20) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ���������ɺ����ڵ�
	set ls [regexp -all -inline -line {^[ \t]*function[ \t]+([\w:\.]+)\s*\(.*?\).*$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(3) -image(7) -type(3) -info($pline);
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(Java�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshJavaFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(17) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ����������iTcl��ͳ�Ա�����ڵ�
	set ls [regexp -all -inline -lineanchor {^[ \t]*public[ \t]+class[ \t]+(\w+)[ \t]+.*$} $buf];
	foreach {iline iclass} $ls {
		set hItem1 [WorkTree additem $tree $hItem "$iclass" -id(4) -image(4) -type(4) -info($iline)];
		# ������ĳ�Ա����
		#set exp "^\[ \t\]*::itcl::body\[ \t\]+$iclass\::\(\[\\w:\]+\)\[ \t\]+\{.*\}\[ \t\]+\{\[ \t\]*\$";
		#set lsMethod [regexp -all -inline -line $exp $buf];
		#foreach {mline mname} $lsMethod {
		#	WorkTree additem $tree $hItem1 "$mname" -id(5) -image(5) -type(5) -info($mline);
		#}
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(JavaScript�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshJavaScriptFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(23) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ���������ɺ����ڵ�
	set ls [regexp -all -inline -line {^[ \t]*function[ \t]+([\w:]+)\s*\(.*?\).*$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(3) -image(7) -type(3) -info($pline);
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(NSIS�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshNsisFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(42) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ���������ɺ����ڵ�
	set ls [regexp -all -inline -line {^[ \t]*Function[ \t]+"*([\w:.]+)"*[ \t]*.*?$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(3) -image(29) -type(3) -info($pline);
	}
	
	# ����������macro�ڵ�
	set ls [regexp -all -inline -line {^[ \t]*!macro[ \t]+([\w:]+)[ \t]+.*?$} $buf];
	foreach {pline pname} $ls {
		WorkTree additem $tree $hItem "$pname" -id(4) -image(7) -type(4) -info($pline);
	}

	# ����������������ڵ�
	set ls [regexp -all -inline -line {^[ \t]*SectionGroup[ \t]+(?:/e[ \t]+)*("(.*?)"|[^ \t]+)[ \t]*.*?$} $buf];
	foreach {pline pname1 pname2} $ls {
		if {$pname2 != ""} {
			set pname $pname2
		} else {
			set pname $pname1
		}
		WorkTree additem $tree $hItem "$pname" -id(5) -image(44) -type(5) -info($pline);
	}

	# �������������νڵ�
	set ls [regexp -all -inline -line {^[ \t]*Section[ \t]+(?:/o[ \t]+)*("(.*?)"|[^ \t]+)[ \t]*.*?$} $buf];
	foreach {pline pname1 pname2} $ls {
		if {$pname2 != ""} {
			set pname $pname2
		} else {
			set pname $pname1
		}
		
		set pname [string trimleft $pname]
		if {[string first ";" $pname] == 0} {
			set pname "{Hide Section}"
		} elseif {$pname == {""}} {
			set pname "{Hide Section}"
		}
		WorkTree additem $tree $hItem "$pname" -id(5) -image(28) -type(5) -info($pline);
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�(.H�ļ�)
#-------------------------------------------------------------
proc _plat_Tcl_RefreshHFile {filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(2) -image(17) -type(2) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ���ļ����к����������
	set fi [open $filename r];
	set buf [read $fi];

	# ������������ͳ�Ա�����ڵ�
	set ls [regexp -all -inline -lineanchor {[ \t]*class[ \t]+(\w+)[ \t]*.*?;} $buf];
	foreach {iline iclass} $ls {
		set hItem1 [WorkTree additem $tree $hItem "$iclass" -id(4) -image(4) -type(4) -info($iline)];
		# ������ĳ�Ա����
		set exp ".*?[ \t]+(\w+)[ \t]*\(.+?\)[ \t]*;";
		set lsMethod [regexp -all -inline -line $exp $buf];
		foreach {mline mname} $lsMethod {
			WorkTree additem $tree $hItem1 "$mname" -id(5) -image(5) -type(5) -info($mline);
		}
	}

	close $fi;
}

#-------------------------------------------------------------
# ˢ���ļ��ڵ�
#-------------------------------------------------------------
proc _plat_Tcl_RefreshFile {opr filename} {
	global platform_language
	
	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}

	set hItem [WorkTree searchitem $tree $filename];

	# �ر��ļ�����
	if {$opr == "close"} {
		if {$hItem != 0 && $filename != ""} {
			WorkTree delitem $tree $hItem -TIM_ITEM;
		}
		return;
	}
	
	# ��ȡ�ļ���׺
	set ext [string tolower [file extension $filename]];
	
	# TCL�ļ��Ĵ���
	if {$ext == ".tcl" || $ext == ".tk" || $ext == ".itcl"} {
		_plat_Tcl_RefreshTclFile $filename;
	} elseif {$ext == ".py" || $ext == ".pyw"} {
		_plat_Tcl_RefreshPythonFile $filename;
	} elseif {$ext == ".pl" || $ext == ".pm"} {
		_plat_Tcl_RefreshPerlFile $filename;
	} elseif {$ext == ".rb" || $ext == ".rbw"} {
		_plat_Tcl_RefreshRubyFile $filename;
	} elseif {$ext == ".lua"} {
		_plat_Tcl_RefreshLuaFile $filename;
	} elseif {$ext == ".java"} {
		_plat_Tcl_RefreshJavaFile $filename;
	} elseif {$ext == ".js"} {
		_plat_Tcl_RefreshJavaScriptFile $filename;
	} elseif {$ext == ".nsh" || $ext == ".nsi"} {
		_plat_Tcl_RefreshNsisFile $filename;
	} elseif {$ext == ".h" || $ext == ".hpp"} {
		#_plat_Tcl_RefreshHFile $filename;
	}
}

#-------------------------------------------------------------
# ����VC����
#-------------------------------------------------------------
proc _plat_Tcl_ParseVCDsp {opr filename} {
	global platform_language
	
	# ����ļ���׺�Ƿ�Ϊdsp�����ļ�
	set ext [string tolower [file extension $filename]];
	if {$ext != ".dsp"} {
		return;
	}
	set filedir [file dirname $filename];

	# ��ȡ�ļ���ID
	if {$platform_language == "chinese"} {
		set tree [WorkTree gettreeid "�ļ�"];
	} else {
		set tree [WorkTree gettreeid "File"];
	}
	if {$tree == -1} {
		_plat_Tcl_CreateFileTree;
		if {$platform_language == "chinese"} {
			set tree [WorkTree gettreeid "�ļ�"];
		} else {
			set tree [WorkTree gettreeid "File"];
		}
	}

	set hItem [WorkTree searchitem $tree $filename];

	# �ر��ļ�����
	if {$opr == "close"} {
		if {$hItem != 0} {
			WorkTree delitem $tree $hItem -TIM_ITEM;
		}
		return;
	}

	# ���ļ�
	set fi [open $filename r];
	
	if {$hItem == 0} {
		set name [file tail $filename];
		set hItem [WorkTree additem $tree 1 "$name" -TIM_ID -id(10) -image(1) -type(10) -info($filename)];
		WorkTree expanditem $tree 1;
	} else {
		WorkTree delitem $tree $hItem -TIM_ITEM -TIM_CHILDONLY;
	}

	# ��ʾ������
	#set nPaneID [StatusBar -createpane -width 150];
	#StatusBar -progress -Attach $nPaneID "" 100;
	pwait 1;
	
	# �����������
	set buf [read $fi];
	close $fi;
	#StatusBar -progress -Pos $nPaneID 10;

	# ����Target��
	regexp {# Begin Target(.*)# End Target} $buf all buf;
	#StatusBar -progress -Pos $nPaneID 20;
	
	# ����Group
	set lsGroup [regexp -all -inline {# Begin Group \"(.*?)\"(.*?)# End Group} $buf];
	#StatusBar -progress -Pos $nPaneID 50;
	set nGroupCount [llength $lsGroup];
	set i 1;
	foreach {all groupname group} $lsGroup {
		set hItem1 [WorkTree additem $tree $hItem "$groupname" -id(11) -image(2) -type(11) -info($groupname)];
		set lsFile [regexp -all -inline -line {^SOURCE=\"?(.*?)\"?$} $group];
		foreach {allfile sfile} $lsFile {
			set filepath [file join $filedir $sfile];			
			set filetail [file tail $filepath];
			set ext [string tolower [file extension $filepath]];
			set fileimage 23
			switch $ext {
				".cpp" -
				".cxx" -
				".c" {set fileimage 16}
				".hpp" -
				".h" {set fileimage 17}
				".ico" -
				".bmp" {set fileimage 11}
			}
			WorkTree additem $tree $hItem1 $filetail -id(12) -image($fileimage) -type(12) -info($filepath);			
		}
		#StatusBar -progress -Pos $nPaneID [expr 50+$i*50/$nGroupCount];
		incr i;
	}
	
	# ���ؽ�����
	#StatusBar -progress -Hide $nPaneID;
	#StatusBar -delpane $nPaneID;
}

#-------------------------------------------------------------
# ������
#-------------------------------------------------------------
if {$argc > 0} {
	set opr [lindex $argv 0];
	if {$opr == "CreateFileTree"} {
		_plat_Tcl_CreateFileTree;
	} elseif {$opr == "CloseFileTree"} {
		_plat_Tcl_CloseFileTree;
	} elseif {$opr == "RefreshFile"} {
		if {$argc > 1} {
			_plat_Tcl_RefreshFile [lindex $argv 1] [lindex $argv 2];
		}
	}
}
