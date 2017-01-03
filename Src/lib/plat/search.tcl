#################################################################
# File       : search.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2001-02-04
# Description: Script.NET editer search support script
#			 : if reimplement search function, please don't modify
#			 : follow function name.
#################################################################

#-------------------------------------------------------------
# ��ʼ�����Ҵ��ں���
# ��ڲ���: �ļ���
#-------------------------------------------------------------
proc _plat_Tcl_InitSearchWnd { sFilePath } {

	# ��������
	LangStringEC STR_WINNAME "Find" "����"
	set _nOT [puts create "$STR_WINNAME##GRID"];
	# �����
	puts active $_nOT;
	# �������
	puts format,$_nOT "DelItem ALL;DelColumn All";
	# ������ɫ
	puts format,$_nOT "SetRowColors RGB(0,0,255) RGB(255,255,205)";
	# �����
	LangStringEC STR_COL1 "No" "���"
	puts format,$_nOT "AddColumn $STR_COL1,60";
	LangStringEC STR_COL2 "Line" "�к�"
	puts format,$_nOT "AddColumn $STR_COL2,60";
	LangStringEC STR_COL3 "Content" "����"
	puts format,$_nOT "AddColumn $STR_COL3,300,AutoSize";

	# ����˫�������Ľű�
	set script "\
		set hItem \[puts getinfo,$_nOT HotItem\];\n\
		set nLineNo \[puts getinfo,$_nOT <#\$hItem,1>\];\n\
		owm -sendcmd \"org.owm.edit\" 111 \{<string>$sFilePath\} \$nLineNo 0;\
	";
	puts -$_nOT "<I>Set Script<0> $script";

	# ���ش���ID
	return $_nOT;
}

#-------------------------------------------------------------
# ��Ӳ��ҵ�����
#-------------------------------------------------------------
proc _plat_Tcl_InsertSearchLine { index nLineNo sLine } {
	LangStringEC STR_WINNAME "Find" "����"
	output "$STR_WINNAME" "<14>$index,$nLineNo,$sLine";
	incr index -1;
	output "$STR_WINNAME" "<I>Set Script<$index> <1>";
}

#-------------------------------------------------------------
# ������
#-------------------------------------------------------------
if {$argc > 0} {
	set opr [lindex $argv 0];
	if {$opr == "CreateSearchWnd"} {
		if {$argc > 1} {
			return [_plat_Tcl_InitSearchWnd [lindex $argv 1]];
		}
	} elseif {$opr == "InsertSearchLine"} {
		puts "$argv"
		if {$argc > 3} {
			_plat_Tcl_InsertSearchLine [lindex $argv 1] [lindex $argv 2] [lindex $argv 3];
		}
	}
}