#################################################################
# File       : xmlwdom.tcl
# Author     : Blueant Software
# Version    : 1.0
# Date       : 2004-09-20
# Description: Script.NET xml dom warp script
#################################################################

package require Itcl;
package require dom;

#-------------------------------------------------------------
# TXmlDom class define
#-------------------------------------------------------------
if {[itcl::find classes TXmlDom] != "TXmlDom"} {
::itcl::class  TXmlDom {
	constructor { rootname {fname "" } } {};
	destructor {};
	
	### data member ###
	private variable _filename;		#�ļ���
	private variable _xmlstream;	#xml��
	private variable _rootname;		#������
	private variable _doc;			#xml�ĵ�
	
	### public methods ###
	public method  Load { {fname ""} };	#���ļ�
	public method  Save { {fname ""} };	#�����ļ�����
	public method  GetNode { nodeName {nodeParaent ""} }; # ��ȡ�²��ĳ���ڵ�
	public method  NewNode { nodeParaent nodeStream };	#����һ���ӽڵ�,����ָ���Ĵ����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TXmlDom::constructor { rootname {fname "" } } {
	set _filename $fname
	set _xmlstream ""
	set _rootname $rootname
	Load
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
::itcl::body TXmlDom::destructor {} {
}

#-------------------------------------------------------------
#	�����ļ����ڲ���
#	����:
#	fname	: �ļ���,Ϊ�ձ�ʾ�ñ�����ļ���
#-------------------------------------------------------------
::itcl::body TXmlDom::Load { {fname ""} } {
	if {$fname != ""} {
		set _filename $fname
	}
	
	set _xmlstream ""
	catch {
	set fxml [open $_filename r]
	set _xmlstream [read -nonewline $fxml]
	close $fxml
	
	set _doc [::dom::tcl::DOMImplementation parse $_xmlstream]
	} inf
	puts $inf
}

#-------------------------------------------------------------
#	�����ļ�
#	����:
#	fname	: �ļ���,Ϊ�ձ�ʾ�ñ�����ļ���
#-------------------------------------------------------------
::itcl::body TXmlDom::Save { {fname ""} } {
	set _fname $fname
	if {$fname != ""} {
		set _fname $_filename
	}
	
	# XML�ĵ����л���������
	set _xmlstream [::dom::DOMImplementation serialize $_doc -newline $_rootname]
	
	catch {
	set fxml [open $_fname w+]
	puts -nonewline $fxml $_xmlstream
	close $fxml
	}
}

#-------------------------------------------------------------
#	��ȡ�²��ĳ���ڵ�
#	����:	nodeName	- �ڵ���
#			nodeParent	- �ϲ�ڵ���
#-------------------------------------------------------------
::itcl::body TXmlDom::GetNode { nodeName {nodeParaent ""} } {
	if {$nodeParaent == ""} {
		return [::dom::document getElementsByTagName $_doc $nodeName];
	} else {
		return [::dom::document getElementsByTagName $nodeParaent $nodeName];
	}
}

#-------------------------------------------------------------
#	����һ���ӽڵ�,����ָ���Ĵ����
#	����:	nodeParent	- �ϲ�ڵ���
#			nodeStream	- �ڵ����
#-------------------------------------------------------------
::itcl::body TXmlDom::NewNode { nodeParaent nodeStream } {
	if {$nodeParaent == ""} {
		set newNode [::dom::document createElement $_doc "NEWNODE"];
	} else {
		set newNode [::dom::document createElement $nodeParaent "NEWNODE"];
	}
	
	# XML�ĵ����л���������
	set _xmlstream [::dom::DOMImplementation serialize $_doc -newline $_rootname]
	
	regsub {<NEWNODE/>} $_xmlstream $nodeStream _xmlstream
	
	regsub "<!DOCTYPE $_rootname\[^>\]*>" $_xmlstream {} _xmlstream
	#regsub -all -line {^$} $_xmlstream {} _xmlstream
	set lsLine [regexp -all -line -inline {^.*?$} $_xmlstream]
	set xmlstream ""
	foreach line $lsLine {
		set line [string trimright $line]
		if {$line != ""} {
			if {$xmlstream != ""} {
				set xmlstream "$xmlstream\n$line"
			} else {
				set xmlstream "$line"
			}
		}
	}	
	
	# ������
	catch {
	set fxml [open $_filename w+]
	puts -nonewline $fxml $xmlstream
	#puts $xmlstream
	close $fxml
	}
}
