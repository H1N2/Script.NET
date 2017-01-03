#################################################################
# Script.NET ѡ����ļ���ģ��ű�
# ����:Blueant
# �汾:1.0
# ����:2007-01-01
#################################################################

# ����һ���б�,��Ӹ��ڵ�
LangStringEC STR_GROUP_TEXT "Select Module" "ѡ����ĵ���ģ��"
set opt [list "-OPT_GROUP -ICO_GROUP T\"$STR_GROUP_TEXT\""]

set lsVci [vci_info -listinstalled]
set lsOwm {}
foreach vciId $lsVci {
	set vcitype [vci_exec plat GetVciType $vciId]
	if {$vcitype == "owm"} {
		lappend lsOwm $vciId
	}
}

# ��Ӹ����ӽڵ�
foreach owmId $lsOwm {
	set owmName [vci_exec plat GetVciName $owmId]
	lappend opt "-OPT_RADIOBUTTON(radio($owmId)) T\"$owmName\[$owmId\]\""
}

# ���ñ������Ϣ��
LangStringEC STR_MSG_WIN_TITLE "Select Open File Module" "��ѡ��ģ��"
LangStringEC STR_MSG_TITLE1 "Select module to open file:" "ѡ����ĵ�"
LangStringEC STR_MSG_TITLE2 "" "��ģ��"
lappend opt "-OPT_MSG T\"$STR_MSG_WIN_TITLE\" E\"$STR_MSG_TITLE1 $argv $STR_MSG_TITLE2\""
# ִ���������Ի���
set ret [OptionTree $opt];
if { $ret != -1 } {
	set ls [array get radio]
	foreach {owmId select} $ls {
		if {$select ==1} {
			owm -sendcmd "" 86 "<string>$argv" "<string>$owmId"
		}
	}
}