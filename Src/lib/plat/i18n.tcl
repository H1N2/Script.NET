#################################################################
# Script.NET ���ʻ�֧�ֽű�
# ����:Blueant
# �汾:1.0
# ����:2006-06-10
#################################################################

#----------------------------------------------------------------
# ���������ַ���
# �÷�: �ֱ��ò�ͬ���Ե��ô˺���,���ս���ѵ�ǰ���Զ�Ӧ���ַ���
#		���õ�����strName��
#----------------------------------------------------------------
proc LangString { strName lang strValue } {
	upvar $strName _strName
	global platform_language
	if {$lang == $platform_language} {
		set _strName $strValue
	}
}

#----------------------------------------------------------------
# ���������ַ���(ͬʱ����Ӣ�ĺ�����)
#----------------------------------------------------------------
proc LangStringEC { strName strValueEnglish strValueChinese } {
	upvar $strName _strName
	global platform_language
	switch $platform_language {
		english {set _strName $strValueEnglish}
		chinese {set _strName $strValueChinese}
	}
}

#----------------------------------------------------------------
# ���ݵ�ǰ����,������Ӧ���ļ���
# ����: dir			- �ļ�·��
#		fileEnglish	- Ӣ���ļ���
#		fileChinese	- �����ļ���
#----------------------------------------------------------------
proc GetLangFile { dir fileEnglish fileChinese } {
	global platform_language
	switch $platform_language {
		english {set fileName "$dir/$fileEnglish"}
		chinese {set fileName "$dir/$fileChinese"}
	}
	if {[file exist $fileName]} {
		return $fileName
	} else {
		if {[file exist "$dir/$fileEnglish"]} {
			return "$dir/$fileEnglish"
		} elseif {[file exist "$dir/$fileChinese"]} {
			return "$dir/$fileChinese"
		}
	}
	return ""
}