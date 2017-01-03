#################################################################
# File       : rhttp.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2002-02-04
# Description: Script.NET remote http protocal script
#################################################################

package require http;

#-------------------------------------------------------------
#	cancel callback
#-------------------------------------------------------------
proc http_Cancel {} {
	global nPaneID
	#ProgressDlg end;
	StatusBar -progress -Hide $nPaneID;
	StatusBar -delpane $nPaneID;
}

# Copy a URL to a file
proc http_download_file { url file {timeout 10000} {chunk 4096} } {
	global nPaneID
    set out [open $file w]
	set nPaneID [StatusBar -createpane -width 150];
	StatusBar -progress -Attach $nPaneID "����:" 100;
	#ProgressDlg create "����Script.NET" "��������������Ϣ�ļ�..." 100 http_Cancel;
    set token [http::geturl $url -channel $out -progress http_Progress -blocksize $chunk -timeout $timeout]
	#ProgressDlg end;
	StatusBar -progress -Hide $nPaneID;
	StatusBar -delpane $nPaneID;
    close $out
    # This ends the line started by http::Progress
    puts stderr ""
    upvar #0 $token state
    set max 0
    foreach {name value} $state(meta) {
    if {[string length $name] > $max} {
        set max [string length $name]
    }
    if {[regexp -nocase ^location$ $name]} {
        # Handle URL redirects
        puts stderr "Location:$value"
        return [copy [string trim $value] $file $chunk]
    }
    }
    incr max
    foreach {name value} $state(meta) {
    	puts [format "%-*s %s" $max $name: $value]
    }

    return $token
}

#-------------------------------------------------------------
#	���ؽ���callback
#-------------------------------------------------------------
proc http_Progress {token total current} {
	global nPaneID
	ProgressDlg setpos [expr $current*100/$total];
	StatusBar -progress -Pos $nPaneID [expr $current*100/$total];
}

#-------------------------------------------------------------
# Main Program
#-------------------------------------------------------------

# ��������ֵ
set address "";
set filename "";

# ���������в���
if {$argc > 0} {
	set argv1 [lindex $argv 0];
	regexp {([\w\.]*)[\\/]?(.*)} $argv1 all address filename;
	#puts "address=$address,filename=$filename";
}

# ȱʡ���ļ���Ϊindex.ral
if {$filename == ""} {
	set filename "index.ral";
}

# ���������ļ���
regsub -all {\\} $filename {/} filename;
# ����url·��
set url_base "http://$address"
set lfilename $filename;
# ȥ��·��,ֻ�����ļ���
set index [string last "/" $lfilename];
if {$index != -1} {
	set lfilename [string range $lfilename [expr $index+1] end];
	set path [string range $filename 0 [expr $index-1]];
	set url_base "http://$address/$path"
}
set localFile "$platform_path/option/temp/$lfilename";

set url "http://$address/$filename"
set nPaneID 0

# �����ļ�
ProgressDlg create "Script.NET-rhttp" "���������嵥�ļ�$lfilename..." 100;
if { [catch { http_download_file $url $localFile; } errInfo ] } {
	StatusBar -progress -Hide $nPaneID;
	StatusBar -delpane $nPaneID;
	ProgressDlg end;
	MessageBox -MB_OK -MB_ICONERROR -title "���ش���" "�ӷ����������ļ� $filename ʧ��,ԭ��Ϊ:\r\n$errInfo!";
	return "";
}
ProgressDlg end;

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
					ProgressDlg setpos $nLineIndex;
					incr nLineIndex;
					continue;
				}
				regsub {\[(\d)\]} $line {} line;
			}			

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
				set url "$url_base/$filesrc"				
				catch {
				http_download_file $url $filedesc;
				puts "Download-File:  $filesrc"
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
				set url "$url_base/$line"				
				catch {
				http_download_file $url $filedesc;
				puts "Download-File:  $line"
				}
			}
			
			StatusBar -progress -Hide $nPaneID;
			StatusBar -delpane $nPaneID;
			
			if {$bRetFile == 1} {
				set localFile $filedesc;
				if {[string first ".tcl" $localFile] > 0} {
					set localFile "tcl:$localFile";
				} elseif {[string first ".py" $localFile] > 0} {
					set localFile "python:$localFile";
				} elseif {[string first ".pl" $localFile] > 0} {
					set localFile "perl:$localFile";
				}
			}
		}

		ProgressDlg setpos $nLineIndex;
		incr nLineIndex;
	}

	ProgressDlg end;

	close $fral;
}

# �û�cancel��ֱ���˳�
if {$bRalCancel == 1} {
	return;
}

# �������صı����ļ���
if {[string tolower $ext] == ".tcl"} {
	result "tcl:$localFile";
} elseif {[string tolower $ext] == ".py"} {
	result "python:$localFile";
} elseif {[string tolower $ext] == ".pl"} {
	result "perl:$localFile";
} elseif {[string tolower $ext] == ".exe"} {
	if { [MessageBox -MB_YESNO -MB_ICONINFORMATION -title "��Ϣ" \
		"�ļ� $localFile �������,���ھ�Ҫִ����?"] == 6 } {								
		#Ҫִ��
		#�жϲ���ϵͳ
		if { [expr $tcl_platform(osVersion) < 5.0] } {
			if {[catch { exec $localFile &;
				}  errinfo]} {	
					puts $errinfo
			}								
		} else {									
			if {[catch { exec $localFile;
				}  errinfo]} {	
					puts $errinfo
		    }
		}								
	}
	result "";
} else {
	result "$localFile";
}
