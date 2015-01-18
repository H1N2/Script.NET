#################################################################
# projectpane.tcl
# Author     : blueant
# Version    : 4.0
# Date       : 2015-01-18
# Description: General project pane
#################################################################

package require Itcl

catch {namespace import itcl::*}

namespace eval prjdef {
# ҳ������
set PGITYPE_FILE		0
set PGITYPE_TEST		1
set PGITYPE_CUSTOM		2
set PGITYPE_TASKPANEL	3

set TREE_PROJECT_ID		10008					;# Projectҳ���ID

# ͼƬ����
set ILINK_PROJECT		0						;# ���ڵ�
}

#-------------------------------------------------------------
# ID��ϵ:
# ID=1	: ���ڵ�(����)
# ID=2	: �ű��ļ�
# ID=3	: �ļ��ڵ�
#-------------------------------------------------------------

#-------------------------------------------------------------
# ����������ҳ��
#-------------------------------------------------------------
proc ProjectCreateProjectTree { } {
	global platform_language
	
	if {[WorkTree treeexist $prjdef::TREE_PROJECT_ID] == 0} {
		# �����������,���˳�
		return
	}
	
	# ������
	LangStringEC STR_TREE_NAME "Project" "����"
	set tree [WorkTree gettreeid $STR_TREE_NAME]
	if {$tree == -1} {
		return
	}

	# �������ڵ�
	set hRootItem [WorkTree searchitem $tree "###PROJECT_ROOT###"]
	if {$hRootItem == 0} {
		set hRootItem [WorkTree additem $tree ROOT $STR_TREE_NAME -image(0) -id(1) -type(1) -bold(true) -info(###PROJECT_ROOT###)];

		# �ڵ�˫���Ľű�,������һ��TCL����,����3������,�ֱ�Ϊ��ID,�ڵ���,�ڵ�ID
		set dclickscript {
			proc dclick {tree item id} {
				set ItemName [WorkTree getitem name $tree $item -TIM_ITEM];
				set ItemInfo [WorkTree getitem info $tree $item -TIM_ITEM];
				#puts "item=$item,id=$id,ItemName=$ItemName,ItemInfo=$ItemInfo";
				
				if {$id != 3} {
					return
				}
				
				set filename [WorkTree getitem info $tree $item -TIM_ITEM]
				
				# ��ָ���ļ���ת��ָ������
				regsub -all {/} $filename {\\} filename
				# �����
				owm -sendcmd "org.owm.edit" 102 "<string>$filename" 0
				
			};dclick}
	
		WorkTree setdclickscript $tree $dclickscript;
	} else {
		#set vciprj [vci_exec plat GetObjectByInstanceName "###project###"]
		#set tclvciprj [vci_info -gettclvciobj "org.prj.general" $vciprj]
		#set project_name ""
		#vci_exec $tclvciprj GetActiveProject project_name
		#set project_name [pproject name]
		if {[pproject isopen] == 0} {
			# �����ѹر�
			WorkTree delitem $tree $hRootItem -TIM_ITEM -TIM_CHILDONLY
			return
		}
	}
	
	# ��ӽű��ļ��ڵ�
	set hItemScripts [WorkTree searchitem $tree "###SCRIPTFILE_ROOT###"]
	if {$hItemScripts != 0} {
		WorkTree delitem $tree $hRootItem -TIM_ITEM -TIM_CHILDONLY
	}
	set hItemScripts [WorkTree additem $tree $hRootItem "Script Files" -id(2) -image(4) -type(2) -info(###SCRIPTFILE_ROOT###)]
	
	if {$hItemScripts != 0} {
		set project_path [pproject path]
		set lsFile [glob -path "$project_path\\" "*.{tcl,py,pl,lua,rb}"]
		
		# �����нű��ļ����뵽�ļ��ڵ�����
		foreach filename $lsFile {
			set name [file tail $filename]
			WorkTree additem $tree $hItemScripts $name -id(3) -image(5) -type(3) -info($filename)
		}
	}
	
	WorkTree expanditem $tree $hRootItem -TIM_ITEM
	WorkTree expanditem $tree $hItemScripts -TIM_ITEM
}

#-------------------------------------------------------------
# Main
#-------------------------------------------------------------
ProjectCreateProjectTree
