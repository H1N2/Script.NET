#################################################################
# File       : project.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2001-02-04
# Description: Script.NET project operater script
#################################################################

#-------------------------------------------------------------
# ����״̬��Pane
#-------------------------------------------------------------
proc CreateStatusPane { hSnp } {
	# ����״̬��Pane
	set node [xml::GetNode $hSnp ptr1 "StatusBar" "Pane"];
	while {$node != "null"} {
		set Name [xml::GetNode $hSnp text $node];
		set ID [xml::GetNode $hSnp attr $node "ID"];
		set Width [xml::GetNode $hSnp attr $node "Width"];
		set ImageGroup [xml::GetNode $hSnp attr $node "ImageGroup"];
		set Image [xml::GetNode $hSnp attr $node "Image"];		
		set nID [StatusBar -createpane -width $Width -id $ID];
		StatusBar -settext $nID $Name;
		StatusBar -seticon $nID $ImageGroup $Image;
		set node [xml::GetNode $hSnp next $node];		
	}
}

#-------------------------------------------------------------
# ����������
#-------------------------------------------------------------
proc CreateProjectTree { hSnp prjfile } {

	# ��ȡ�����ļ���·��
	set prjpath [file dirname $prjfile];
		
	# ������
	WorkTree closetree "����";
	set tree [WorkTree gettreeid "����"];	
	if {$tree == -1} {
		set tree [WorkTree addtree PGI_CLASS "����"];
	}
	
	set PrjName [xml::GetNode $hSnp text "Project" ""];
	set hRootItem [WorkTree additem $tree ROOT "����\[$PrjName\]" -image(1) -id(1) -type(1) -bold(true)];

	set node [xml::GetNode $hSnp ptr1 "Files" "Folder"];
	while {$node != "null"} {
		set Name [xml::GetNode $hSnp attr $node "Name"];
		set Image [xml::GetNode $hSnp attr $node "Image"];
		set hFolderItem [WorkTree additem $tree $hRootItem $Name -image($Image) -id(2) -type(2)];

		set node1 [xml::GetNode $hSnp child $node ptr2];
		while {$node1 != "null"} {
			set Name [xml::GetNode $hSnp text $node1];
			set Type [xml::GetNode $hSnp attr $node1 "Type"];
			set Image [xml::GetNode $hSnp attr $node1 "Image"];
			set sfilename "$prjpath/$Name";
			if {$Type == "tcl" || $Type == "python"} {
				set _Name [xml::GetNode $hSnp attr $node1 "Name"];
				set hFileItem [WorkTree additem $tree $hFolderItem $_Name -image($Image) -id(4) -type(1) -info($sfilename)];
			} else {
				set hFileItem [WorkTree additem $tree $hFolderItem $Name -image($Image) -id(3) -type(3) -info($sfilename)];
			}
			
			set node1 [xml::GetNode $hSnp next $node1];		
		}
		
		set node [xml::GetNode $hSnp next $node];
	}

	WorkTree expanditem $tree $hRootItem -TIM_ITEM;
	
	# �ڵ�˫���Ľű�,������һ��TCL����,����3������,�ֱ�Ϊ��ID,�ڵ���,�ڵ�ID
	set dclickscript {
		proc dclick {tree item id} {
			set ItemName [WorkTree getitem name $tree $item -TIM_ITEM];
			set ItemInfo [WorkTree getitem info $tree $item -TIM_ITEM];
			#puts "item=$item,id=$id,ItemName=$ItemName,ItemInfo=$ItemInfo";

			# ���ڵ��򲻼�������
			if {$id == 1 || $id == 2} {
				# ���ڵ���ļ��нڵ�
				return;
			} elseif {$id == 3} {
				# �ļ��ڵ�
				set res [owm -sendcmd 100 102 "<string>$ItemInfo" 0];
				if {$res == 0} {
					# �������0˵��OwmEdit����δ��,��Ҫ�ȴ�һ������
					owm -open Frame 100 -active -onlyone;
					# ��OwmEdit���ڵĵ�һ��λ�ô��ļ�
					owm -sendcmd 100 103 "<string>$ItemInfo" 0;
				} else {
					owm -open Frame 100 -active -onlyone;
				}
				return;
			} elseif {$id == 4} {
				# ִ�нڵ�
				source $ItemInfo;
			}
		};dclick}

	WorkTree setdclickscript $tree $dclickscript;
}

#-------------------------------------------------------------
# �رչ�����
#-------------------------------------------------------------
proc CloseProjectTree {} {

	WorkTree closetree "����";
}

#-------------------------------------------------------------
# �򿪹���
#-------------------------------------------------------------
proc OpenProject { prjfile } {

	set hSnp [xml::open file $prjfile];
	if {$hSnp == 0} {
		return;
	}
	
	CreateStatusPane $hSnp;
	CreateProjectTree $hSnp $prjfile;
	
	xml::close $hSnp;

}

#-------------------------------------------------------------
# �رչ���
#-------------------------------------------------------------
proc CloseProject { prjfile } {

	CloseProjectTree;

}

#-------------------------------------------------------------
# ������
#-------------------------------------------------------------
if {$argc > 0} {
	set opr [lindex $argv 0];
	if {$opr == "open"} {
		if {$argc > 1} {
			OpenProject [lindex $argv 1];
		}
	} elseif {$opr == "close"} {
		if {$argc > 1} {
			CloseProject [lindex $argv 1];
		}
	}
}
