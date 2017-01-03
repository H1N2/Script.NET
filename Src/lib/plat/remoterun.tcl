#################################################################
# File       : remoterun.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2001-02-04
# Description: Script.NET remote run protocal script
#################################################################

package require ftp;

#-------------------------------------------------------------
# Main Program
#-------------------------------------------------------------

# ��������ֵ
set username "";
set password "";
set address "";
set filename "";
set bLogin "false";

# ���������в���
if {$argc > 0} {
	set argv1 [lindex $argv 0];
	if {[string first "@" $argv1] == -1} {
		regexp {([\w\.]*)[\\/]?(.*)} $argv1 all address filename;
	} else {
		regexp {(\w*)@*([\w\.]*)[\\/]?(.*)} $argv1 all username address filename;
	}
	#puts "username=$username,address=$address,filename=$filename";
}

# ȱʡ���ļ���Ϊindex.ral
if {$filename == ""} {
	set filename "index.ral";
}

# ����ȱʡ�û���Ϊ�����û�
if {$username == ""} {
	set username "Anonymous";
}

# ����������û�����Դ˵�ַ�Ƿ���Դ�
if {$username == "Anonymous" && $address != ""} {
	if { [ catch {
		set handle [ftp::Open $address $username $password];
	} errorInfo] } {
		puts $errorInfo;
	} else {
		set bLogin "true";
	}
}

# ���δ�ܵ�¼�������¼����
if {$bLogin == "false"} {
	set res [tclLogin -title "��¼RemoteRun������" \
			-address address $address \
			-username username $username \
			-password password $password \
			-domain filename $filename \
			-check rememberpwd "�������" false \
			-laddress "�� �� ����" \
			-ldomain  "�����ļ���"];
	if {$res != 1} {
		# �û�cancel�˴�����
		return;
	} else {
		# ��¼������
		set handle [ftp::Open $address $username $password];
	}
}

# ȱʡ���ļ���Ϊindex.ral
if {$filename == ""} {
	set filename "index.ral";
}

# ���������ļ���
regsub -all {\\} $filename {/} filename;
#set path "";
set lfilename $filename;
# ȥ��·��,ֻ�����ļ���
set index [string last "/" $lfilename];
if {$index != -1} {
	set lfilename [string range $lfilename [expr $index+1] end];
	#set path [string range $filename 0 [expr $index-1]];
}
set localFile "$platform_path/option/temp/$lfilename";

# �����ļ�
#ftp::Cd $handle $path;
if { [catch { ftp::Get $handle $filename $localFile; } errInfo ] } {
	MessageBox -MB_OK -MB_ICONERROR -title "���ش���" "�ӷ����������ļ� $filename ʧ��,ԭ��Ϊ:\r\n$errInfo!";
	return "";
}

#-------------------------------------------------------------
# ����ral�б�����е�cancel����
#-------------------------------------------------------------
set bRalCancel 0;
proc ralCancel {} {
	global bRalCancel;
	set bRalCancel 1;
}

# �����ļ���
regsub -all "/" $localFile "\\" localFile;
set ext [string range $localFile end-3 end];

if {[string tolower $ext] == ".ral"} {
	catch {global bRalCancel};
	
	# ral�ļ�ΪԶ�̵�ַ�嵥�ļ�
	set fral [open $localFile r];
	set nLineCount 0;
	while {[gets $fral line] >= 0} {
		incr nLineCount;
	}
	if {$nLineCount == 0} { set nLineCount 1; }
	seek $fral 0;

	#set nPaneID [StatusBar -createpane -width 150];
	#StatusBar -progress -Attach $nPaneID "����:" $nLineCount;
	ProgressDlg create "����-$lfilename" "�ļ�:" $nLineCount "ralCancel";
	set nLineIndex 0;

	while {$bRalCancel == 0 && [gets $fral line] >= 0} {
		if {[string length $line] > 0 && [string first "#" $line] != 0} {
			# ���˵�ע���кͿ���
			
			# ����ļ�����Ȩ��
			if {[regexp {\[(\d)\]} $line all rights]} {
				set userLogin [user -current];
				regexp {\((\d)\)} $userLogin all userRights;
				if {[expr $userRights > $rights]} {
					#StatusBar -progress -Pos $nPaneID $nLineIndex;
					ProgressDlg setpos $nLineIndex;
					incr nLineIndex;
					continue;
				}
				regsub {\[(\d)\]} $line {} line;
			}			

			#StatusBar -settext 0 $line;
			ProgressDlg setstatus $line;

			set bRetFile 0;
			set bFile 1;
			if {[string first "::" $line] == 0} {
				# ::��ͷ,��ʾ���������շ��ص��ļ���
				eval "set localFile [string range $line 2 end]";
				continue;
			} elseif {[string first ":" $line] == 0} {
				# :��ͷ,��ʾ�������ص��ļ�������ִ�е�
				set bRetFile 1;
				set line [string range $line 1 end];
			} elseif {[string first "%" $line] == 0} {
				# %��ͷ,��ʾ��Ҫ����һ��Ŀ¼
				set line [string range $line 1 end];
				eval "set sdir $line";
				catch {
				file mkdir $sdir;
				}
				set bFile 0;
			}
			if {($bFile == 1) && ([string first ">" $line] > 0)} {
				# ���Ϊ'filesrc > filedesc'����ʽ,��ʾ��filesrc����Ϊ����filedesc
				set index [string first ">" $line];
				set filesrc [string range $line 0 [expr $index-1]];
				set filedesc [string range $line [expr $index+1] end];
				set filesrc [string trim $filesrc];
				set filedesc [string trim $filedesc];
				# filedesc֧�ֱ���tcl�������滻
				eval "set filedesc $filedesc";
				catch {
				ftp::Get $handle $filesrc $filedesc;
				}
			} elseif {$bFile == 1} {
				# ֱ�ӵ��ļ���,��ʾ���ص����ص�tempĿ¼
				set lfilename $line;
				# ȥ��·��,ֻ�����ļ���
				set index [string last "/" $lfilename];
				if {$index != -1} {
					set lfilename [string range $lfilename [expr $index+1] end];
				}
				set filedesc "$platform_path/option/temp/$lfilename";
				catch {
				ftp::Get $handle $line $filedesc;
				}
			}
			if {$bRetFile == 1} {
				set localFile $filedesc;
				if {[string first ".tcl" $localFile] > 0} {
					set localFile "tcl:$localFile";
				} elseif {[string first ".py" $localFile] > 0} {
					set localFile "python:$localFile";
				}
			}
		}

		#StatusBar -progress -Pos $nPaneID $nLineIndex;
		ProgressDlg setpos $nLineIndex;
		incr nLineIndex;
	}

	#StatusBar -progress -Hide $nPaneID;
	#StatusBar -delpane $nPaneID;
	ProgressDlg end;

	close $fral;
}

ftp::Close $handle;

# �û�cancel��ֱ���˳�
if {$bRalCancel == 1} {
	return;
}

# �������صı����ļ���
if {[string tolower $ext] == ".tcl"} {
	result "tcl:$localFile";
} elseif {[string tolower $ext] == ".py"} {
	result "python:$localFile";
} else {
	result "$localFile";
}
