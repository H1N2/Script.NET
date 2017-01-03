#################################################################
# Script.NET �鿴�ϵ��б��Tcl�ű�
# ����:Blueant
# �汾:1.0
# ����:2001-11-16
#################################################################

#-------------------------------------------------------------
# �����ϵ��б���
#-------------------------------------------------------------
proc _plat_Tcl_CreateWatchBreaks { sFilePath } {

	LangStringEC winName "Break List" "�ϵ�"
	LangStringEC colName1 "No" "���"
	LangStringEC colName2 "LineNo" "�к�"
	LangStringEC colName3 "Content" "����"
	
	# �����ϵ㴰��
	set _nOT [output -create "$winName##GRID"];
	output -active -$_nOT;

	# ���������
	output -format -$_nOT "DelItem ALL;DelColumn All";
	
	# ������ɫ
	output -format -$_nOT "SetRowColors RGB(0,0,128) RGB(255,255,205)";
	output -format -$_nOT "SetGridColor RGB(192,192,192)";
	output -format -$_nOT "ModifyStyle Horizontal Solid;ModifyStyle Vertical Solid";

	# ������
	output -format -$_nOT "AddColumn $colName1,50";
	output -format -$_nOT "AddColumn $colName2,65";
	output -format -$_nOT "AddColumn $colName3,300,AutoSize";

	# ����˫�������Ľű�
	set script "\
		set hItem \[output -GetInfo -$_nOT HotItem\];\
		set nLineNo \[output -GetInfo -$_nOT <#\$hItem,1>\];\
		owm -sendcmd \"org.owm.edit\" 111 \"<string>$sFilePath\" \$nLineNo 0;\
	";
	output -$_nOT "<I>Set Script<0> $script";
	
	return $_nOT;
}