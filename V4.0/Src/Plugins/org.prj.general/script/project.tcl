#################################################################
# project.tcl
# Author     : blueant
# Version    : 4.0
# Date       : 2015-01-18
# Description: general project creater
#################################################################

package require Itcl

catch {namespace import itcl::*}

#-------------------------------------------------------------
# replace keywords of file
#-------------------------------------------------------------
proc ReplaceFileKey { fname lsKey } {
	
	if {[file exist $fname] == 0} {
		return
	}
	
	catch {
	set f [open $fname r]
	set filestream [read -nonewline $f]
	close $f
	}

	catch {
	foreach {key replace} $lsKey {
		regsub -all $key $filestream $replace filestream
	}
	}
	
	catch {
	set f [open $fname w+]
	puts -nonewline $f $filestream		
	close $f
	}
}

#-------------------------------------------------------------
# TProject class define
#-------------------------------------------------------------
if {[itcl::find classes TProject] != "TProject"} {
::itcl::class  TProject {
	constructor {} {};
	destructor {};
	
	### data member ###
	protected variable _project_name;	# ������
	
	### public methods ###
	public method  New {prj_name prj_path prj_type prj_desc author prj_template};	# �½�����
	public method  AfterNew {prj_name prj_path prj_type prj_desc author prj_template};	# �½�����֮��
}
}

#-------------------------------------------------------------
# ��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TProject::constructor {} {
}

#-------------------------------------------------------------
# ��Ԫ��������
#-------------------------------------------------------------
::itcl::body TProject::destructor {} {
}

#-------------------------------------------------------------
# �½�����
# Returns    : project file
#-------------------------------------------------------------
::itcl::body TProject::New {prj_name prj_path prj_type prj_desc author prj_template} {
	# ��������Ŀ¼
	#file mkdir $prj_path

	set lsFile [glob -path "$prj_template\\" "*.*"]
	
	# ��ģ��Ŀ¼�����ļ�
	foreach sFile $lsFile {
		file copy $sFile $prj_path
	}
	
	# ���ع����ļ���
	return "$prj_path\\$prj_name.snp"
}

#-------------------------------------------------------------
# �½�����֮��
# Returns    :
#-------------------------------------------------------------
::itcl::body TProject::AfterNew {prj_name prj_path prj_type prj_desc author prj_template} {
	
	# �滻�ļ��еĹؼ���
	set lsKey [list "%PRJ_NAME%"]
	lappend lsKey "$prj_name"
	lappend lsKey "%PRJ_TYPE%"
	lappend lsKey "$prj_type"
	lappend lsKey "%PRJ_DESC%"
	lappend lsKey "$prj_desc"
	lappend lsKey "%AUTHOR%"
	lappend lsKey "$author"
	lappend lsKey "%DATE%"
	lappend lsKey [clock format [clock seconds] -format "%Y-%m-%d"]

	ReplaceFileKey "$prj_path\\globalproperty.tcl" $lsKey
	ReplaceFileKey "$prj_path\\test.tcl" $lsKey
	ReplaceFileKey "$prj_path\\test.py" $lsKey
}
