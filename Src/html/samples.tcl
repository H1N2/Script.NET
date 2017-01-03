#################################################################
# Script.NET ҳ��ű�
# ����:blueant
# �汾:1.0
# ����:2002-03-05
# ע:�˽ű��ṩ��ʾ�����������
#################################################################

package require TclFace;

#-------------------------------------------------------------
# TLFAppPage class define
#-------------------------------------------------------------
if {[itcl::find classes TLFAppPage] != "TLFAppPage"} {
class  TLFAppPage {
	inherit TLFPage
	
	constructor {} {};
	
	### data member ###
	protected variable _objTextInfo;
	protected variable _tableFile;
	protected variable _curDir;			# ��ǰ·��
	protected variable _cdDir;			# �任��·��
	
	### public methods ###
	public method  ProcessFormValue {name value};	# ����Form���
	public method  OnCreate {};						# ����ҳ��
	public method  OnInitialUpdate {};				# ҳ���ʼ��
	public method  OnEndAction {};					# ����ִ�����Ĳ���
	public method  OnChangeDir {};
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TLFAppPage::constructor {} {
	chain;
	SetFormAction;
	set _curDir "";
	set _cdDir "";
	set _objTextInfo "";
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
body TLFAppPage::ProcessFormValue {name value} {
	if {[chain $name $value] == true} {
		return true
	}
	switch -glob $name {
		dir			{set _curDir $value}
		cddir		{set _cdDir $value;regsub -all "{|}" $_cdDir {} _cdDir;}
	}
}

#-------------------------------------------------------------
#	����ҳ��
#-------------------------------------------------------------
body TLFAppPage::OnCreate {} {
	LangStringEC STR_INFO_DIR "Current Directory:" "��ǰ·����"
	set _objTextInfo [AppendText "<b>$STR_INFO_DIR $_curDir</b>"]
	$_objTextInfo SetColor #800000
}

#-------------------------------------------------------------
#	ҳ���ʼ��
#-------------------------------------------------------------
body TLFAppPage::OnInitialUpdate {} {
}

#-------------------------------------------------------------
#	����ִ�����Ĳ���
#-------------------------------------------------------------
body TLFAppPage::OnEndAction {} {
	
	regsub -all {\\} $_curDir {/} _curDir;
	
	# �ر�����
	LangStringEC STR_INFO_DIR "Current Directory:" "��ǰ·����"
	$_objTextInfo SetText "<b>$STR_INFO_DIR $_curDir</b>"
	
	# �����ļ����
	set _tableInfo [CreateObject TLFTable]
	set _tableFile [CreateObject TLFTable]
	LangStringEC STR_INFO "File Name" "�ļ���"
	$_tableFile AddColumn $STR_INFO
	LangStringEC STR_INFO "Size" "��С"
	$_tableFile AddColumn $STR_INFO
	LangStringEC STR_INFO "Create Time" "����ʱ��"
	$_tableFile AddColumn $STR_INFO

	# ����dir��file�б�
	set lsItems [list]
	catch {set lsItems [glob -directory "$_curDir/" *]}

	# create back parent directory line
	if {[string index $_curDir end] != ":"} {
		$_tableFile AddLine2 \
							[list "<img border=0 src='image\\back.gif'>.." "" "tcl:<file>PLAT://\\html\\samples.tcl?action=OnChangeDir&dir=$_curDir&cddir=\{..\}"] \
							[list "&nbsp"] \
							[list "&nbsp"]
	}

	foreach item $lsItems {
		set splitpos [string last "/" $item];
		if {$splitpos != -1} {
			incr splitpos;
			set name [string range $item $splitpos end];
		} else {
			set name $item;
		}
		set size [file size $item];
		set mtime [clock format [file mtime $item] -format "%Y-%m-%d %H:%M:%S"];
		
		# ����Ŀ¼��
		if { $name == "." || $name == ".." } {
			continue;
		}
		
		if {[file isdirectory $item]} {
			# create dir line
			$_tableFile AddLine2 \
							[list "<img border=0 src='image\\folder.gif'>$name" "" "tcl:<file>PLAT://\\html\\samples.tcl?action=OnChangeDir&dir=$_curDir&cddir=\{$name\}"] \
							[list "&nbsp"] \
							[list "&nbsp"]
		} else {
			# create file line
			set img "file.gif";
			set dot [string last "." $name];
			if {$dot != -1} {
				incr dot;
				set ext [string range $name $dot end];
				set ext [string tolower $ext];				
				switch $ext {
					"gif"	{set img "filetype/gif.gif"}
					"jpg"	{set img "filetype/jpg.gif"}
					"bmp"	{set img "filetype/bmp.gif"}
					"ico"	-
					"pcx"	-
					"png"	-
					"tiff"	{set img "image.gif"}
					"zip"	-
					"rar"	-
					"arj"	-
					"lzh"	-
					"cab"	{set img "compressed.gif"}
					"htm"	-
					"html"	{set img "filetype/html.gif"}
					"xml"	{set img "filetype/xml.gif"}
					"c"		-
					"cpp"	{set img "filetype/source-c.gif"}
					"h"		-
					"hpp"	{set img "filetype/source-h.gif"}
					"cs"	{set img "filetype/cs.gif"}
					"doc"	{set img "filetype/doc.gif"}
					"ppt"	{set img "filetype/ppt.gif"}
					"xls"	{set img "filetype/xls.gif"}
					"js"	{set img "filetype/js.gif"}
					"tkv"	{set img "filetype/tkview.gif"}
					"tk"		-
					"tcl"	{set img "filetype/tcl.gif"}
					"pyw"		-
					"py"	{set img "filetype/python.gif"}
					"rb"	{set img "filetype/ruby.gif"}
					"pl"	{set img "filetype/perl.gif"}
					"php"	{set img "filetype/php.gif"}
					"lua"	{set img "filetype/lua.gif"}
					"java"	{set img "filetype/java.gif"}
					"uml"	{set img "filetype/uml.gif"}
					"nsh"		-
					"nsi"	{set img "filetype/nsis.gif"}
					"exe"	{set img "this.gif"}
				}
			}
			
			if {$img == "this.gif"} {
				$_tableFile AddLine2 \
								[list "<img border=0 src='image\\$img'>$name" "" "tcl:exec \"$item\" &"] \
								[list "$size"] \
								[list "$mtime"]
			} else {
				$_tableFile AddLine2 \
								[list "<img border=0 src='image\\$img'>$name" "" "pfe:open $item"] \
								[list "$size"] \
								[list "$mtime"]
			}
		}
	}
}

#-------------------------------------------------------------
#	cd dir
#-------------------------------------------------------------
body TLFAppPage::OnChangeDir {} {
	regsub -all {\\} $_curDir {/} _curDir;
	if {$_cdDir == ".."} {
		set splitpos [string last "/" $_curDir];
		if {$splitpos != -1} {
			incr splitpos -1;
			set _curDir [string range $_curDir 0 $splitpos];
		}
	} else {
		set _curDir "$_curDir/$_cdDir";
	}
	
	return true
}

#-------------------------------------------------------------
#	main
#-------------------------------------------------------------
LangString STR_PAGE_TITLE chinese "��ʾ����"
LangString STR_PAGE_TITLE english "Samples"
set page [TLFCreatePage $STR_PAGE_TITLE TLFAppPage]
#delete object $page;
