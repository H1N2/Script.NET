#################################################################
# File       : TclFace.tcl
# Author     : Blueant
# Version    : 1.0.20101022
# Date       : 2005-10-31
# Description: Tcl Face script library
#################################################################

package provide TclFace 1.0

package require Itcl

catch {namespace import itcl::*}

set g_tlf_obj_count 0		;# TLF������������,�����Զ�����������
set g_tlf_mode		tlf		;# TclFaceģʽ��(tlf or tsf)

# ���platform_path������,������,������ΪTSFģʽ
if {[info exist platform_path] == 0} {
	catch {
		set platform_path "[ns_info home]/modules"
		set g_tlf_mode tsf
	}
}

if {[info exist platform_path_style] == 0} {
	set platform_path_style "style"
}

if {[info exist platform_ui_style] == 0} {
	set platform_ui_style default
}

if {[info exist platform_language] == 0} {
	set platform_language english
}

#-------------------------------------------------------------
# set tclface mode to tlf or tsf
#-------------------------------------------------------------
proc set_tlf_mode {mode} {
	global g_tlf_mode
	set g_tlf_mode $mode
}

#-------------------------------------------------------------
# TLF Template class define
#-------------------------------------------------------------
if {[itcl::find classes TLFTemplate] != "TLFTemplate"} {
::itcl::class  TLFTemplate {
	constructor {{style "default"}} {};
	destructor {};
	
	### data member ###
	private variable _templateStyle;#ģ����
	private variable _templatePath;	#ģ��·��
	private variable _template;		#ģ������
	private variable _templateVars;	#ģ���������
	private variable _templateConsts;	#ģ�峣������(���ͬ�������ж��,��ÿ������Ԫ����һ���б�)
	
	### public methods ###
	public method  LoadTemplateFile {filename};	# ����ģ���ļ�
	public method  GetTemplateList {{type ""} {field ""}};	# ��ȡģ����б�
	public method  GetTemplateBlock {type field {name default}};	# ��ȡָ��ģ���
	public method  GetTemplateBlockByName {fullname};	# ��ȡָ��ģ���
	public method  GetTemplateVars {type field {name default}};	# ��ȡָ��ģ�������б�
	public method  GetTemplateVarsByName {fullname};	# ��ȡָ��ģ�������б�
	public method  GetTemplateConstValue {name};	# ��ȡָ��ģ�峣��ֵ
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFTemplate::constructor {{style "default"}} {
	global platform_path
	# ����ģ��·��
	set _templatePath "$platform_path/template/html/$style";
	# ���ģ��·��������,����Ĭ��·��
	if {[file exist $_templatePath] == 0} {
		set _templatePath "$platform_path/template/html/default";
	}
	catch {LoadTemplateFile "default.tft"}
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
::itcl::body TLFTemplate::destructor {} {
}

#-------------------------------------------------------------
#	����ģ���ļ�
#	����:
#	filename	: ģ���ļ�
#-------------------------------------------------------------
::itcl::body TLFTemplate::LoadTemplateFile {filename} {
	# ģ���ļ�ΪXML��ʽ
	if {[string first "/" $filename] == -1 && [string first "\\" $filename] == -1} {
		set xmlfilename "$_templatePath/$filename"
	} else {
		set xmlfilename $filename
	}
	set xmlTemplate [xml_open file $xmlfilename]
	
	set nodeBlocks [xml_get $xmlTemplate rootNode]	
	set lsNodeBlock [xml_get $xmlTemplate childNodes $nodeBlocks]
	foreach node $lsNodeBlock {
		set nodeType [xml_get $xmlTemplate nodeName $node]
		
		if {$nodeType == "consts"} {
			set lsNodeConst [xml_get $xmlTemplate childNodes $node]
			foreach nodeConst $lsNodeConst {
				set constName [xml_get $xmlTemplate nodeAttr -node $nodeConst name]
				set constValue [xml_get $xmlTemplate nodeAttr -node $nodeConst value]
				# const�����ÿ��Ԫ�ض���һ���б�,Ҳ����ÿ�������������ж��ֵ
				lappend _templateConsts($constName) [list $constValue]
			}
			continue
		}
		
		set nodeField [xml_get $xmlTemplate nodeAttr -node $node Field]
		set nodeName [xml_get $xmlTemplate nodeAttr -node $node Name]
		set nodeContent [xml_get $xmlTemplate childNode $node Content]
		set _template($nodeType.$nodeField.$nodeName) [xml_get $xmlTemplate nodeText -node $nodeContent -cdata]
		set _templateVars($nodeType.$nodeField.$nodeName) {}
		set nodeVars [xml_get $xmlTemplate childNode $node Vars]
		set lsNodeVar [xml_get $xmlTemplate childNodes $nodeVars]
		foreach nodeVar $lsNodeVar {
			set varName [xml_get $xmlTemplate nodeAttr -node $nodeVar name]
			set varValue [xml_get $xmlTemplate nodeAttr -node $nodeVar value]
			lappend _templateVars($nodeType.$nodeField.$nodeName) [list $varName $varValue]
		}
	}
	
	xml_close $xmlTemplate
}

#-------------------------------------------------------------
#	��ȡģ����б�
#	����:
#	type	: ������
#	field	: ���ֶ�
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateList {{type ""} {field ""}} {
	if {$field != ""} {
		set tempPre "$type.$field."
	} elseif {$type != ""} {
		set tempPre "$type."
	} else {
		return [array names _template]
	}
	
	set lsTemplate {}
	foreach templateName [array names _template] {
		if {[string first $tempPre $templateName] != -1} {
			lappend lsTemplate $templateName
		}
	}
	return $lsTemplate
}

#-------------------------------------------------------------
#	��ȡָ��ģ���
#	����:
#	type	: ������
#	field	: ���ֶ�
#	name	: ������
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateBlock {type field {name default}} {
	return $_template($type.$field.$name);
}

#-------------------------------------------------------------
#	��ȡָ��ģ���
#	����:
#	fullname: ������
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateBlockByName {fullname} {
	return $_template($fullname);
}

#-------------------------------------------------------------
#	��ȡָ��ģ�������б�
#	����:
#	type	: ������
#	field	: ���ֶ�
#	name	: ������
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateVars {type field {name default}} {
	return $_templateVars($type.$field.$name);
}

#-------------------------------------------------------------
#	��ȡָ��ģ�������б�
#	����:
#	fullname: ������
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateVarsByName {fullname} {
	return $_templateVars($fullname);
}

#-------------------------------------------------------------
#	��ȡָ��ģ�峣��ֵ
#	����:
#	name	: ��������
#-------------------------------------------------------------
::itcl::body TLFTemplate::GetTemplateConstValue {name} {
	return $_templateConsts($name);
}

# ����ȱʡ��ģ��������
if {[itcl::find objects g_TemplateManager -class TLFTemplate] != "g_TemplateManager"} {
	global platform_ui_style;
	TLFTemplate g_TemplateManager $platform_ui_style; #"default"
}

#-------------------------------------------------------------
# TLF Base Object class define
#-------------------------------------------------------------
if {[itcl::find classes TLFObject] != "TLFObject"} {
::itcl::class  TLFObject {
	constructor {{id ""}  {parent ""}} {};
	destructor {};
	
	### data member ###
	protected variable _obj_id;			#����ID
	protected variable _parent;			#������
	protected variable _contentHtml;	#��������
	protected variable _contentTag;		#�������ݶ�Ӧ�ı����,���Ϊ����Ϊȫ��
	protected variable _lsObj;			#���Ӷ����б�
	protected variable _enableDrawObjs;	#�Ƿ񻭸��Ӷ���
	protected variable _form_action;	#Form�Ķ���
	protected variable _form_post_type;	#Form���ύ����
	protected variable _action;			#����
	protected variable _actionObj;		#�����������ĸ�������(�ձ�ʾ������)
	protected variable _submitAction;	#�ύ����(JavaScript����)
	protected variable _submitActionObj;#�ύ�����������ĸ�������(�ձ�ʾ��)
	protected variable _varList;		#�洢�������л�����������
	protected variable _enableSerial;	#�Ƿ�����������л�
	protected variable _enableDraw;		#�Ƿ�������ʾ�˶���
	protected variable _disabled;		#�Ƿ����(���)
	protected variable _jsproperty;		#JavaScript�����ֶ�
	protected variable _styleType;		#������(��Ӧģ����еķ��)
	protected variable _addBr;			#�Ƿ���Ҫ�ڶ�������һ�����з�
	protected variable _addTrTd;		#�Ƿ���Ҫ�ڶ���ǰ���TRTD
	protected variable _addTrTdAlign;	#�ڶ���ǰ���TRTD�Ķ��뷽ʽ
	
	### public methods ###
	public method  Reset {};			# ��λ����
	public method  SetObjId {id};		# ����Id
	public method  GetObjId {};			# ��ȡId
	public method  GetClassName {};		# ��ȡ����
	public method  GetFullId {};		# ��ȡ����Id
	public method  SetParent {parent};	# ���ø�����
	public method  GetParent {};		# ��ȡ������
	public method  SetContentTag {tag};	# ���ö������ݶ�Ӧ�ı����
	public method  SetStyleType {style};# # ���ö�����(��Ӧģ����еķ��)
	public method  SetDrawObjs {enable};# �Ƿ񻭸��Ӷ���
	public method  SetEnableSerial {enable};	# �������л������־
	public method  GetEnableSerial {};	# ��ȡ���л������־
	public method  SetEnableDraw {enable};	# ���ö�����ʾ�����־
	public method  GetEnableDraw {};	# ��ȡ������ʾ�����־
	public method  SetDisable {disable};	# �����Ƿ���ñ�־
	public method  SetJavaScriptProperty {property};	# ����JavaScript�����ֶ�
	public method  AddBr {};			# �ڶ�������һ�����з�
	public method  GetBr {};			# ��ȡ�Ƿ�ӻ��з����
	public method  AddTrTd {{enable true} {align ""}};	# �ڶ���ǰ���TRTD
	public method  GetTrTd {};			# ��ȡ�Ƿ��TRTD���
	public method  GetTrTdAlign {};		# ��ȡ��TRTD��ǵĶ��뷽ʽ
	public method  SerializeVar {dir varName};	# ���л������һ������
	public method  SerializeVars {dir};	# ���л���������б���
	public method  Serialize {dir xmlfile xmlnode};	# �������л�
	public method  ShowInformation {};	# ��ʾ������Ϣ
	public method  SetFormAction {{action ""} {post_type "POST"}};	# ����Form����
	public method  ProcessFormValue {name value};	# ����Form���
	public method  SetAction {action {actionObj ""}};	# ���ö���
	public method  GetAction {};		# ��ȡ����
	public method  SetSubmitAction {action {actionObj ""}};# �����ύ����
	public method  OnCreate {};			# ����
	public method  OnAfterCreate {};	# ����֮��
	public method  OnInitialUpdate {};	# �����ʼ��
	public method  OnEndAction {};		# ����ִ�����Ĳ���
	public method  Draw {};				# �����������
	public method  AppendText {text};	# �����������
	public method  AppendTargetTag {tagTr replaceTr {cleartag "-cleartag"}};	# ��һ����Ƕδ洢��Ŀ�껺������
	public method  ReplaceStringTag {str tagTr replaceTr {cleartag "-cleartag"}};# �滻ָ���ַ����е��滻���
	public method  GetVarValue {lsVars varName};	# ��ȡ����ֵ
	public method  GetVarValueDefault {varValue lsVars varName};	# (���ָ��ֵΪ�����ȱʡֵ)
	public method  AppendObject {obj};	# ���TLF����
	public method  CreateObject {args};	# �������TLF����,���ض�����
	public method  DeleteObject {obj};	# ɾ��ĳ������
	public method  SetTagObject {tagTr {obj ""} {cleartag "-cleartag"}};	# ����TLF����,�滻ģ���е�һ�����
	public method  CreateTagObject {args};	# ����������TLF����,�滻ģ���е�һ�����,���ض�����
	public method  AppendJavaScript {jscript {type "file"}};	# ���JavaScript
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFObject::constructor {{id ""} {parent ""}} {
	set _obj_id $id;
	set _contentHtml "";
	set _contentTag "";
	set _lsObj {};
	set _enableDrawObjs true;
	set _parent $parent;
	set _form_action "";
	set _form_post_type "POST";
	set _action "";
	set _actionObj "";
	set _submitAction "";
	set _submitActionObj "";
	set _enableSerial true;
	set _enableDraw true;
	set _disabled "";
	set _jsproperty "";
	set _styleType "default";
	set _addBr false;
	set _addTrTd false;
	set _addTrTdAlign "";
	array set _varList {};
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
::itcl::body TLFObject::destructor {} {
	foreach obj $_lsObj {
		::itcl::delete object $obj;
	}
}

#-------------------------------------------------------------
# ��λ����
#-------------------------------------------------------------
::itcl::body TLFObject::Reset {} {
	set _contentHtml "";
	foreach obj $_lsObj {
		::itcl::delete object $obj;
	}
	set _lsObj {};
	array unset _varList;
	array set _varList {};
	set _action "";
	set _actionObj "";
}

#-------------------------------------------------------------
# ����ID
#-------------------------------------------------------------
::itcl::body TLFObject::SetObjId {id} {
	set _obj_id $id;
}

#-------------------------------------------------------------
# ��ȡID
#-------------------------------------------------------------
::itcl::body TLFObject::GetObjId {} {
	return $_obj_id;
}

#-------------------------------------------------------------
# ��ȡ����
#-------------------------------------------------------------
::itcl::body TLFObject::GetClassName {} {
	set classname [$this info class];
	if {[string first "::" $classname] == 0} {
		set classname [string range $classname 2 end];
	}
	return $classname;
}

#-------------------------------------------------------------
# ��ȡ������Id,�Ӷ������ʼ����
#-------------------------------------------------------------
::itcl::body TLFObject::GetFullId {} {
	if {$_parent != ""} {
		return "[$_parent GetFullId].$_obj_id";
	} else {
		return $_obj_id;
	}
}

#-------------------------------------------------------------
# ���ø�����
#-------------------------------------------------------------
::itcl::body TLFObject::SetParent {parent} {
	set _parent $parent;
}

#-------------------------------------------------------------
# ��ȡ������
#-------------------------------------------------------------
::itcl::body TLFObject::GetParent {} {
	return $_parent;
}

#-------------------------------------------------------------
# ���ö������ݶ�Ӧ�ı����
#-------------------------------------------------------------
::itcl::body TLFObject::SetContentTag {tag} {
	set _contentTag $tag;
}

#-------------------------------------------------------------
# ���ö�����(��Ӧģ����еķ��)
#-------------------------------------------------------------
::itcl::body TLFObject::SetStyleType {style} {
	set _styleType $style;
}

#-------------------------------------------------------------
# �����Ƿ񻭸��Ӷ���
#-------------------------------------------------------------
::itcl::body TLFObject::SetDrawObjs {enable} {
	set _enableDrawObjs $enable;
}

#-------------------------------------------------------------
# �������л������־
#-------------------------------------------------------------
::itcl::body TLFObject::SetEnableSerial {enable} {
	set _enableSerial $enable;
}

#-------------------------------------------------------------
# ��ȡ���л������־
#-------------------------------------------------------------
::itcl::body TLFObject::GetEnableSerial {} {
	return $_enableSerial;
}

#-------------------------------------------------------------
# ���ö�����ʾ�����־
#-------------------------------------------------------------
::itcl::body TLFObject::SetEnableDraw {enable} {
	set _enableDraw $enable;
}

#-------------------------------------------------------------
# ��ȡ������ʾ�����־
#-------------------------------------------------------------
::itcl::body TLFObject::GetEnableDraw {} {
	return $_enableDraw;
}

#-------------------------------------------------------------
# �����Ƿ���ñ�־
#-------------------------------------------------------------
::itcl::body TLFObject::SetDisable {disable} {
	if {($disable == "true") || ($disable == "disable")} {
		set _disabled "disabled";
	} else {
		set _disabled "";
	}
}

#-------------------------------------------------------------
# ����JavaScript�����ֶ�
#-------------------------------------------------------------
::itcl::body TLFObject::SetJavaScriptProperty {property} {
	set _jsproperty $property;
}

#-------------------------------------------------------------
# �ڶ�������һ�����з�
#-------------------------------------------------------------
::itcl::body TLFObject::AddBr {} {
	set _addBr true;
}

#-------------------------------------------------------------
# ��ȡ�Ƿ�ӻ��з����
#-------------------------------------------------------------
::itcl::body TLFObject::GetBr {} {
	return $_addBr;
}

#-------------------------------------------------------------
# �ڶ���ǰ���<TR><TD>
#-------------------------------------------------------------
::itcl::body TLFObject::AddTrTd {{enable true} {align ""}} {
	set _addTrTd $enable;
	set _addTrTdAlign $align;
}

#-------------------------------------------------------------
# ��ȡ�Ƿ��<TR><TD>���
#-------------------------------------------------------------
::itcl::body TLFObject::GetTrTd {} {
	return $_addTrTd;
}

#-------------------------------------------------------------
# ��ȡ��TRTD��ǵĶ��뷽ʽ
#-------------------------------------------------------------
::itcl::body TLFObject::GetTrTdAlign {} {
	return $_addTrTdAlign;
}

#-------------------------------------------------------------
# ���л������һ������
#-------------------------------------------------------------
::itcl::body TLFObject::SerializeVar {dir varName} {
	upvar $varName _local_varName
	if {$dir == "get"} {
		set lsTemp [array get _varList $varName]
		# ֻ���ڱ�����δ��ֵ������²Ŵ�XML�ļ������л�
		if {([llength $lsTemp] != 0) && ($_local_varName == "")} {
			set _local_varName [lindex $lsTemp 1]
		}
	} elseif {$dir == "set"} {
		set _varList($varName) $_local_varName;
	}
}

#-------------------------------------------------------------
# ���л���������б���
#-------------------------------------------------------------
::itcl::body TLFObject::SerializeVars {dir} {
}

#-------------------------------------------------------------
# �������л�
#-------------------------------------------------------------
::itcl::body TLFObject::Serialize {dir xmlfile xmlnode} {
	if {$dir == "get"} {
		#puts "--$_obj_id,$dir,$xmlfile,$xmlnode"
		# ��ȡ
		set className [xml_get $xmlfile nodeAttr -node $xmlnode Class]
		# �ж������Ƿ���ȷ
		if {$className == [$this GetClassName]} {
			set nodeVarList [xml_get $xmlfile childNode $xmlnode VarList]
			if {$nodeVarList != 0} {
				set lsNodeVar [xml_get $xmlfile childNodes $nodeVarList]
				foreach nodeVar $lsNodeVar {
					set nodeName [xml_get $xmlfile nodeAttr -node $nodeVar Name]
					set nodeValue [xml_get $xmlfile nodeText -node $nodeVar]
					#puts "==$nodeName,$nodeValue"
					if {$nodeName != ""} {
						set _varList($nodeName) $nodeValue
					}
				}
			}
		}
		# ���л�������Ҫ���л��ı����������б���
		SerializeVars $dir
	} elseif {$dir == "set"} {
		# ����
		# ���л�������Ҫ���л��ı����������б���
		SerializeVars $dir
		set className [xml_get $xmlfile nodeAttr -node $xmlnode Class]
		# �ж������Ƿ���ȷ
		if {$className != [$this GetClassName]} {
			# ���Ͳ���ȷ���޸�����
			xml_set $xmlfile nodeAttr -node $xmlnode Class [$this GetClassName];
			set className [xml_get $xmlfile nodeAttr -node $xmlnode Class]
		}
		if {$className == [$this GetClassName]} {
			set nodeVarList [xml_get $xmlfile childNode $xmlnode VarList]
			
			if {($nodeVarList == 0) && ([array size _varList] > 0)} {
				# ���VarList�ڵ㲻����,��_VarList���������Ϊ��,�򴴽��˽ڵ�
				set nodeVarList [xml_create $xmlfile node -node $xmlnode VarList]
			}
			
			if {$nodeVarList != 0} {
				set lsNodeVar [xml_get $xmlfile childNodes $nodeVarList]
				# ���ɵ�ǰXML�����б�,��¼���б�������XML�ڵ�
				set lsCurNode {}
				foreach nodeVar $lsNodeVar {
					set nodeName [xml_get $xmlfile nodeAttr -node $nodeVar Name]
					if {$nodeName != ""} {
						lappend lsCurNode [list $nodeName $nodeVar]
					}
				}
				# �����ж���������л���XML��,����ڵ㲻���ھʹ���
				foreach nodeName [array names _varList] {
					set pos [lsearch $lsCurNode [list $nodeName *]]
					if {$pos != -1} {
						# �����XML�����б����ҵ��˱���,��ֱ�����ô˽ڵ��ֵ
						set nodeVar [lindex [lindex $lsCurNode $pos] 1]
						xml_set $xmlfile nodeText -node $nodeVar $_varList($nodeName)
						continue
					} else {
						# ����ʹ����µ�XML�ڵ�
						set nodeVar [xml_create $xmlfile node -node $nodeVarList Var]
						if {$nodeVar != 0} {
							xml_set $xmlfile nodeAttr -node $nodeVar Name $nodeName
							xml_set $xmlfile nodeText -node $nodeVar $_varList($nodeName)
						}
					}
				}
				# ɾ��xml���л��ļ��ж���ı����ӽڵ�(�޷�������Щ�Ƕ���ı���,�˹����ݲ�ʵ��)
#				foreach nodeAtXml $lsCurNode {
#					puts $nodeAtXml
#					puts [array names _varList]
#					set nodeName [lindex $nodeAtXml 0]
#					if {[lsearch [array names _varList] [lindex $nodeAtXml 0]] == -1} {
#						puts "--[lindex $nodeAtXml 0]"
#					}
#				}
			}
		}
	}
	
	# �Ӷ������л�
	set lsObjId {}
	foreach obj $_lsObj {
		set childNode [xml_get $xmlfile childNode $xmlnode [$obj GetObjId]];
		if {($childNode == 0) && ([$obj GetEnableSerial] == "true")} {
			# ����Ӷ����������л�����xml�ļ��в�����,�򴴽�xml�ڵ�
			set childNode [xml_create $xmlfile node -node $xmlnode [$obj GetObjId]];
			if {$childNode != 0} {
				xml_set $xmlfile nodeAttr -node $childNode Class [$obj GetClassName];
			}
		}
		if {$childNode != 0} {
			$obj Serialize $dir $xmlfile $childNode;
		}
		lappend lsObjId [$obj GetObjId];
	}
	# ɾ��xml���л��ļ��ж�����ӽڵ�
	set lsNodeChildObj [xml_get $xmlfile childNodes $xmlnode]
	foreach nodeChildObj $lsNodeChildObj {
		set nodeObjId [xml_get $xmlfile nodeName $nodeChildObj]
		if {($nodeObjId != "VarList") && ([lsearch $lsObjId $nodeObjId] == -1)} {
			xml_remove $xmlfile node -node $xmlnode $nodeChildObj
		}
	}
}

#-------------------------------------------------------------
# ��ʾ������Ϣ
#-------------------------------------------------------------
::itcl::body TLFObject::ShowInformation {} {
	puts "###information $_obj_id: classname=[GetClassName], parent=[GetParent], fullid=[GetFullId]"
	# �Ӷ�����Ϣ
	foreach obj $_lsObj {
		$obj ShowInformation
	}
}

#-------------------------------------------------------------
# ����Form����
#-------------------------------------------------------------
::itcl::body TLFObject::SetFormAction {{action ""} {post_type "POST"}} {
	global interpName g_tlf_mode
	
	if {$action == ""} {
		if {$g_tlf_mode == "tsf"} {
			set _form_action "[ns_conn url]"
		} else {
			set _form_action "tcl:<<$interpName>><form><file>[info script]";
		}
	} else {
		set _form_action $action;
	}
	set _form_post_type $post_type;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFObject::ProcessFormValue {name value} {
	#puts "[GetObjId],$name,$value"
	if {[string first "[GetObjId]." $name] == 0} {
		# �Ǵ˶�����Ӷ���
		set name [string range $name [string length "[GetObjId]."] end]
		
		# ������������,ֻ����x�����,������ظ���������
		if {$name == "x"} {
			# �����.x��.y,���ʾ��ͼ��button��Ķ���,����Ҫ�������·���,������������ƥ��ķ�������
			if {$_action != ""} {
				if {$_actionObj != ""} {
					# ִ��ָ����������Ķ���
					$_actionObj $_action $value
				} else {
					# ��������Ϊ�ձ�ʾ�����ڸ�����,�ҵ�������,���ö����_action
					set objParent $this
					while {[$objParent GetParent] != ""} {
						set objParent [$objParent GetParent]
					}
					if {$objParent != ""} {
						$objParent SetAction $_action
					}
				}
			}
			return true
		}
		
		#puts "==$name,$_lsObj"
		foreach obj $_lsObj {
			if {[string first "." $name] == -1} {
				# ���name��û��.,��name��objid������ͬ
				if {[$obj GetObjId] == $name} {
					$obj ProcessFormValue $name $value
					return true
				}
			} else {
				# ��objid+.���Ƚ�,�������objid���ú�name��ǰ����ͬ��������,
				# �ͻ��жϴ���,����obj_tlf1��obj_tlf10���жϽ��
				if {[string first "[$obj GetObjId]." $name] == 0} {
					$obj ProcessFormValue $name $value
					return true
				}
			}
		}
	} elseif {[GetObjId] == $name} {
		# �������ƥ��,����_action����,����ǿ�,�������������action
		if {$_action != ""} {
			if {$_actionObj != ""} {
				# ִ��ָ����������Ķ���
				$_actionObj $_action $value
			} else {
				# ��������Ϊ�ձ�ʾ�����ڸ�����,�ҵ�������,���ö����_action
				set objParent $this
				while {[$objParent GetParent] != ""} {
					set objParent [$objParent GetParent]
				}
				if {$objParent != ""} {
					$objParent SetAction $_action
				}
			}
			return true
		}
	}
}

#-------------------------------------------------------------
# ���ö���
#-------------------------------------------------------------
::itcl::body TLFObject::SetAction {action {actionObj ""}} {
	set _action $action
	set _actionObj $actionObj
}

#-------------------------------------------------------------
# ��ȡ����
#-------------------------------------------------------------
::itcl::body TLFObject::GetAction {} {
	return $_action
}

#-------------------------------------------------------------
# �����ύ����
#-------------------------------------------------------------
::itcl::body TLFObject::SetSubmitAction {action {actionObj ""}} {
	set _submitAction $action
	set _submitActionObj $actionObj
}

#-------------------------------------------------------------
# ����
#-------------------------------------------------------------
::itcl::body TLFObject::OnCreate {} {
}

#-------------------------------------------------------------
# ����֮��
#-------------------------------------------------------------
::itcl::body TLFObject::OnAfterCreate {} {
}

#-------------------------------------------------------------
# �����ʼ��
#-------------------------------------------------------------
::itcl::body TLFObject::OnInitialUpdate {} {
}

#-------------------------------------------------------------
# ����ִ�����Ĳ���
#-------------------------------------------------------------
::itcl::body TLFObject::OnEndAction {} {
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFObject::Draw {} {
	# ���ر��������
	set _temp_contentHtml "";
	if {$_contentTag == ""} {
		set _temp_contentHtml $_contentHtml;
	}
	
	# ���������Ӷ��������
	if {$_form_action != ""} {
		set _temp_contentHtml "<FORM name='[GetFullId]' action='$_form_action' onsubmit='processFormSubmit(this)' method=$_form_post_type>\n$_temp_contentHtml";
	}
	
	if {$_enableDrawObjs == true} {
		foreach obj $_lsObj {
			if {[$obj GetEnableDraw] == true} {
				set objContent [$obj Draw];
				# �Ƿ��<TR><TD>
				if {[$obj GetTrTd] == "true"} {
					set align [$obj GetTrTdAlign]
					if {$align != ""} {
						set align "align=$align"
					}
					set _temp_contentHtml "$_temp_contentHtml\n<tr $align><td $align>$objContent</td></tr>";
				} else {
					set _temp_contentHtml "$_temp_contentHtml\n$objContent";
				}
				# �Ƿ��ڶ���������һ�����з�
				if {[$obj GetBr] == "true"} {
					set _temp_contentHtml "$_temp_contentHtml<br>";
				}
			}
		}
	}
	
	if {$_form_action != ""} {
		set _temp_contentHtml "$_temp_contentHtml\n</FORM>";
	}
	
	if {$_jsproperty != ""} {
		ReplaceStringTag _temp_contentHtml "JSPROP" $_jsproperty;
	} 
	
	if {$_contentTag != ""} {
		# ������������ݶ�Ӧ�ı��,���Ƚ������Ӷ��������滻�������
		AppendTargetTag $_contentTag $_temp_contentHtml;
	} else {
		# ����ֱ�ӽ��������Ӷ���������ӵ����ݵĺ���
		set _contentHtml $_temp_contentHtml;
	}
	
	return $_contentHtml;
}

#-------------------------------------------------------------
#	��һ����Ƕδ洢��Ŀ�껺������
#	����:
#	tagTr	: ��ģ����ļ���ģ��ҳ���е�Tr�����
#	cleartag: �Ƿ�����滻Ԫ�ر��(-cleartag or -append)
#-------------------------------------------------------------
::itcl::body TLFObject::AppendTargetTag {tagTr replaceTr {cleartag "-cleartag"}} {
	# �洢��Ŀ�껺����
	set index [string first "%$tagTr%" $_contentHtml];
	while { $index != -1 } {
		if { $cleartag == "-cleartag" } {
			set tagLen [string length $tagTr];
			set _contentHtml [string replace $_contentHtml $index [expr $index+$tagLen+1] $replaceTr];
			set index [string first "%$tagTr%" $_contentHtml];
		} else {
			set _contentHtml [string replace $_contentHtml $index $index "$replaceTr%"];
			set index -1;
		}
	}
}

#-------------------------------------------------------------
#	�滻ָ���ַ����еı��
#	����:
#	buffer	: Ŀ�껺����
#	tagTr	: ��ģ����ļ���ģ��ҳ���е�Tr�����
#	cleartag: �Ƿ�����滻Ԫ�ر��(-cleartag or -append)
#-------------------------------------------------------------
::itcl::body TLFObject::ReplaceStringTag {str tagTr replaceTr {cleartag "-cleartag"}} {
	upvar $str _str
	# �洢��Ŀ�껺����
	set index [string first "%$tagTr%" $_str];
	while { $index != -1 } {
		if { $cleartag == "-cleartag" } {
			set tagLen [string length $tagTr];
			set _str [string replace $_str $index [expr $index+$tagLen+1] $replaceTr];
			set index [string first "%$tagTr%" $_str];
		} else {
			set _str [string replace $_str $index $index "$replaceTr%"];
			set index -1;
		}
	}
}

#-------------------------------------------------------------
#	��ȡ����ֵ
#	����:
#	lsVars: �����б�
#	varName: ������
#-------------------------------------------------------------
::itcl::body TLFObject::GetVarValue {lsVars varName} {
	foreach var $lsVars {
		if {[lindex $var 0] == $varName} {
			return [lindex $var 1]
		}
	}
	return ""
}

#-------------------------------------------------------------
#	��ȡ����ֵ(���ָ��ֵΪ�����ȱʡֵ)
#	����:
#	lsVars: �����б�
#	varName: ������
#-------------------------------------------------------------
::itcl::body TLFObject::GetVarValueDefault {varValue lsVars varName} {
	if {$varValue != ""} {
		return $varValue
	}
	foreach var $lsVars {
		if {[lindex $var 0] == $varName} {
			return [lindex $var 1]
		}
	}
	return ""
}

#-------------------------------------------------------------
# �������
#-------------------------------------------------------------
::itcl::body TLFObject::AppendText {text} {
	return [CreateObject ::TLFText $text]
}

#-------------------------------------------------------------
# ���TLF����
#-------------------------------------------------------------
::itcl::body TLFObject::AppendObject {obj} {
	lappend _lsObj $obj;
	# ���ö���ĸ�����Ͷ���Id
	$obj SetParent $_obj_id;
	$obj SetObjId $obj
}

#-------------------------------------------------------------
# �������TLF����,���ض�����
#-------------------------------------------------------------
::itcl::body TLFObject::CreateObject {args} {
	global g_tlf_obj_count
	# �޸Ķ����������ķ���,ʹ���Զ����ȫ�ּ�����������������
	#set lsTmp [list [lindex $args 0] "obj_#auto"]
	set lsTmp [list [lindex $args 0] "obj_tlf_$g_tlf_obj_count"]
	incr g_tlf_obj_count
	
	if {[llength $args] > 1} {
		foreach arg [lrange $args 1 end] {
			lappend lsTmp $arg
		}
	}

	set objName [eval $lsTmp]
	#puts $objName
	
	AppendObject $objName

	return "TLFObject::$objName"
}

#-------------------------------------------------------------
# ɾ��ĳ������
#-------------------------------------------------------------
::itcl::body TLFObject::DeleteObject {obj} {
	set index 0
	foreach objItem $_lsObj {
		if {$obj == "TLFObject::$objItem"} {
			::itcl::delete object $objItem;
			set _lsObj [lreplace $_lsObj $index $index];
			return
		}
		incr index
	}
}

#-------------------------------------------------------------
# ����TLF����,�滻ģ���е�һ�����
# �����ֱ�Ϊ:tagTr cleartag args
#-------------------------------------------------------------
::itcl::body TLFObject::SetTagObject {tagTr {obj ""} {cleartag "-cleartag"}} {
	if {$obj != ""} {
		#AppendObject $obj
		# ���ö���ĸ�����Ͷ���Id
		$obj SetParent $_obj_id;
		$obj SetObjId $obj
		AppendTargetTag $tagTr [$obj Draw] $cleartag
	} else {
		AppendTargetTag $tagTr ""
	}
}

#-------------------------------------------------------------
# ���JavaScript
#-------------------------------------------------------------
::itcl::body TLFObject::AppendJavaScript {jscript {type "file"}} {
	if {$_parent != ""} {
		return [$_parent AppendJavaScript $jscript $type];
	}
}

#-------------------------------------------------------------
# ����������TLF����,�滻ģ���е�һ�����,���ض�����
# �����ֱ�Ϊ:tagTr cleartag args
#-------------------------------------------------------------
::itcl::body TLFObject::CreateTagObject {args} {
	if {[llength $args] < 3} {
		return
	}
	
	set objname [CreateObject [lrange $args 2 end]]
	AppendTargetTag $tagTr [$objname Draw] $cleartag
	return $objname
}

#-------------------------------------------------------------
# TLF Variable class define
#-------------------------------------------------------------
if {[itcl::find classes TLFVariable] != "TLFVariable"} {
::itcl::class  TLFVariable {
	inherit TLFObject
	
	constructor {{name ""} {value ""}} {};
	
	### data member ###
	protected variable _name;			# ������
	protected variable _value;			# ����ֵ
	
	### public methods ###
	public method  SetVar {name value};	# ���ñ���
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFVariable::constructor {{name ""} {value ""}} {
	chain;
	set _name $name;
	set _value $value;
}

#-------------------------------------------------------------
# ���ñ���
#-------------------------------------------------------------
::itcl::body TLFVariable::SetVar {name value} {
	set _name $name;
	set _value $value;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFVariable::Draw {} {
	
	set _contentHtml "<INPUT type=hidden name=$_name value=$_value>";
	return $_contentHtml;
}

#-------------------------------------------------------------
# TLF Row class define
#-------------------------------------------------------------
if {[itcl::find classes TLFRow] != "TLFRow"} {
::itcl::class  TLFRow {
	inherit TLFObject
	
	constructor {} {};
	
	### data member ###
	protected variable _color;			# ��ɫ
	
	### public methods ###
	public method  SetColor {color};	# ������ɫ
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFRow::constructor {} {
	chain;
	set _color "";
}

#-------------------------------------------------------------
# ������ɫ
#-------------------------------------------------------------
::itcl::body TLFRow::SetColor {color} {
	set _color $color;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFRow::Draw {} {
	set font1 "";
	set font2 "";
	if {$_color != ""} {
		set font1 "<font color=$_color>";
		set font2 "</font>";
	}
	set _contentHtml "<TR $_disabled><TD>$font1[chain]$font2</TD></TR>";
	return $_contentHtml;
}

#-------------------------------------------------------------
# TLF Text class define
#-------------------------------------------------------------
if {[itcl::find classes TLFText] != "TLFText"} {
::itcl::class  TLFText {
	inherit TLFObject
	
	constructor {text {borderColor ""} {bgColor ""}} {};
	
	### data member ###
	protected variable _text;			# �ı�
	protected variable _color;			# ��ɫ
	protected variable _lsText;			# �ı��б�
	protected variable _borderColor;	# �߿���ɫ
	protected variable _bgColor;		# ������ɫ
	
	### public methods ###
	public method  SetText {text};		# �����ı�
	public method  AddText {text};		# ����ı����ı��б�
	public method  SetColor {color};	# ������ɫ
	public method  SetBorder {borderColor {bgColor ""}};# ���ñ߿�
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFText::constructor {text {borderColor ""} {bgColor ""}} {
	chain;
	set _color "";
	set _text $text;
	set _lsText {};
	set _borderColor $borderColor;
	set _bgColor $bgColor;
}

#-------------------------------------------------------------
# �����ı�
#-------------------------------------------------------------
::itcl::body TLFText::SetText {text} {
	set _text $text;
}

#-------------------------------------------------------------
# ����ı�
#-------------------------------------------------------------
::itcl::body TLFText::AddText {text} {
	lappend _lsText $text;
}

#-------------------------------------------------------------
# ������ɫ
#-------------------------------------------------------------
::itcl::body TLFText::SetColor {color} {
	set _color $color;
}

#-------------------------------------------------------------
# ���ñ߿�
#-------------------------------------------------------------
::itcl::body TLFText::SetBorder {borderColor {bgColor ""}} {
	set _borderColor $borderColor;
	set _bgColor $bgColor;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFText::Draw {} {
	
	# ��ȡ����
	set varsText [g_TemplateManager GetTemplateVars Text Text];
	
	if {$_borderColor != ""} {
		# ���߿�����ģ��Ͳ���
		set blockBorderText [g_TemplateManager GetTemplateBlock Text Border];
		set varsBorderText [g_TemplateManager GetTemplateVars Text Border];
	}
	
	if {$_color == ""} {
		set _color [GetVarValue $varsText color];
	}
	
	set font1 "";
	set font2 "";
	if {$_color != ""} {
		set font1 "<font color=$_color>";
		set font2 "</font>";
	}
	
	if {$_borderColor == ""} {
		# ��ͨ����
		set objParent [GetParent]
		if {($objParent != "") && ([::itcl::is object TLFObject::$objParent]) && ([TLFObject::$objParent isa TLFTable])} {
			# ���������,�Ҹ�������TLFTable��������
			set _contentHtml "$font1$_text$font2[chain]";
		} else {
			set _contentHtml "<TR $_disabled><TD>$font1$_text$font2[chain]</TD></TR>";
		}
	} else {
		# ���߿�����
		if {$_borderColor == "default"} {
			set _borderColor [GetVarValue $varsBorderText borderColor];
		}
		if {$_bgColor == ""} {
			set _bgColor [GetVarValue $varsBorderText bgColor];
		}
		set tmpContent $blockBorderText;
		ReplaceStringTag tmpContent "DISABLE" $_disabled;
		ReplaceStringTag tmpContent "BORDERCOLOR" $_borderColor;
		ReplaceStringTag tmpContent "BGCOLOR" $_bgColor;
		if {[llength $_lsText] == 0} {
			ReplaceStringTag tmpContent "CONTENT" "<TR><TD>$font1$_text$font2[chain]</TD></TR>";
		} else {
			set tmpRow "<TD>$font1$_text$font2</TD>";
			foreach text $_lsText {
				set tmpRow "$tmpRow<TD>$font1$text$font2</TD>";
			}
			ReplaceStringTag tmpContent "CONTENT" "<TR>$tmpRow</TR><TR><TD>[chain]</TD></TR>";
		}
		set _contentHtml "<TR><TD>$tmpContent</TD></TR>";
	}
	
	return $_contentHtml;#[chain];
}

#-------------------------------------------------------------
# TLF SyntaxText class define
#-------------------------------------------------------------
if {[itcl::find classes TLFSyntaxText] != "TLFSyntaxText"} {
::itcl::class  TLFSyntaxText {
	inherit TLFText
	
	constructor {text {borderColor ""} {bgColor ""}} {chain $text $borderColor $bgColor} {};
	
	### data member ###
	protected variable _SyntaxText;		# ��������
	protected variable _SyntaxLanguage;	# ��������
	protected variable _option;			# ѡ��
	
	### public methods ###
	public method  SetSyntaxLanguage {type};	# ������������
	public method  SetOption {option};			# ����ѡ��
	public method  SetText {text};				# �����ı�
	public method  LoadFile {filename};			# ����һ���ļ�������
	public method  Draw {};						# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::constructor {text {borderColor ""} {bgColor ""}} {
	set _SyntaxText $text;
	set _SyntaxLanguage "";
	set _option "";
}

#-------------------------------------------------------------
# ������������
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::SetOption {option} {
	set _option "$_option:$option";
}

#-------------------------------------------------------------
# ����ѡ��
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::SetSyntaxLanguage {type} {
	set _SyntaxLanguage $type;
	AppendJavaScript "js/file/syntax/SyntaxHighlighter.css" css;
	AppendJavaScript "js/file/syntax/shCore.js";	
	AppendJavaScript "js/file/syntax/shBrush${_SyntaxLanguage}.js";
	AppendJavaScript "window.onload = function () {dp.SyntaxHighlighter.HighlightAll('_TLFSyntaxText_Code');}" body;
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::SetText {text} {
	set _SyntaxText $text;
}

#-------------------------------------------------------------
# ����һ���ļ�������
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::LoadFile {filename} {
	catch {
		set f [open $filename r];
		set _SyntaxText [read $f];
		close $f;
	}
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFSyntaxText::Draw {} {
	set _text "<pre name='_TLFSyntaxText_Code' class='${_SyntaxLanguage}${_option}'>\n$_SyntaxText\n</pre>";
	#set _text "<textarea name='_TLFSyntaxText_Code' class='${_SyntaxLanguage}${_option}'>\n$_SyntaxText\n</textarea>";
	return [chain];
}

#-------------------------------------------------------------
# TLF Link class define
#-------------------------------------------------------------
if {[itcl::find classes TLFLink] != "TLFLink"} {
::itcl::class  TLFLink {
	inherit TLFObject
	
	constructor {{link ""} {text ""} {title ""}} {};
	
	### data member ###
	protected variable _text;			# �ı�
	protected variable _link;			# ����
	protected variable _title;			# ������ʾ
	protected variable _color;			# ��ɫ
	
	### public methods ###
	public method  SetText {text};		# �����ı�
	public method  SetLink {link};		# ��������
	public method  SetTitle {title};	# ����������ʾ
	public method  SetColor {color};	# ������ɫ
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFLink::constructor {{link ""} {text ""} {title ""}} {
	chain;
	set _color "";
	set _text $text;
	set _link $link;
	set _title $title;
}

#-------------------------------------------------------------
# �����ı�
#-------------------------------------------------------------
::itcl::body TLFLink::SetText {text} {
	set _text $text;
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFLink::SetLink {link} {
	set _link $link;
}

#-------------------------------------------------------------
# ����������ʾ
#-------------------------------------------------------------
::itcl::body TLFLink::SetTitle {title} {
	set _title $title;
}

#-------------------------------------------------------------
# ������ɫ
#-------------------------------------------------------------
::itcl::body TLFLink::SetColor {color} {
	set _color $color;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFLink::Draw {} {
	global interpName g_tlf_mode;
	
	set font1 "";
	set font2 "";
	if {$_color != ""} {
		set font1 "<font color=$_color>";
		set font2 "</font>";
	}
	set linkTemp "";
	if {$_link != ""} {
		if {$g_tlf_mode == "tsf"} {
			set linkTemp "[ns_conn url]?$_link"
		} else {
			set linkTemp "tcl:<<$interpName>><file>[info script]?$_link";
		}
	}
	set _contentHtml "$font1<a href=\"$linkTemp\" title=\"$_title\" $_disabled>$_text</a>$font2[chain]";
	return $_contentHtml;#[chain];
}

#-------------------------------------------------------------
# TLF RadioButton class define
#-------------------------------------------------------------
if {[itcl::find classes TLFRadioButton] != "TLFRadioButton"} {
::itcl::class  TLFRadioButton {
	inherit TLFObject
	
	constructor {{name ""} {text ""} {check ""}} {};
	
	### data member ###
	protected variable _name;			# ������
	protected variable _text;			# ����
	protected variable _checked;		# �Ƿ�ѡ��
	
	### public methods ###
	public method  SerializeVars {dir};	# ���л���������б���
	public method  ProcessFormValue {name value};	# ����Form���
	public method  Draw {};				# �����������
	public method  SetCheck {check};	# ���ü���־
	public method  GetCheck {};			# ��ȡ����־
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFRadioButton::constructor {{name ""} {text ""} {check ""}} {
	chain;
	set _name $name;
	set _text $text;
	set _checked $check;
}

#-------------------------------------------------------------
# �������л�
#-------------------------------------------------------------
::itcl::body TLFRadioButton::SerializeVars {dir} {
	chain $dir;
	SerializeVar $dir _checked;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFRadioButton::ProcessFormValue {name value} {
	if {[chain $name $value] == "true"} {
		return true
	}
	set _checked $value
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFRadioButton::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	set blockRadioButton [g_TemplateManager GetTemplateBlock RadioButton RadioButton];
	
	set _contentHtml $blockRadioButton;
	
	if {($_name == "") && ($_parent != "")} {
		AppendTargetTag "NAME" $_parent;
	} else {
		AppendTargetTag "NAME" $_name;
	}
	AppendTargetTag "ID" [GetFullId];
	AppendTargetTag "TEXT" $_text;
	
	set check "";
	switch $_checked {
	true -
	1 -
	on {set check "checked"}
	}
	AppendTargetTag "CHECKED" $check;
	
	AppendTargetTag "DISABLED" $_disabled;

	return [chain];
}

#-------------------------------------------------------------
# ���ü���־
#-------------------------------------------------------------
::itcl::body TLFRadioButton::SetCheck {check} {
	set _checked $check;
}

#-------------------------------------------------------------
# ��ȡ����־
#-------------------------------------------------------------
::itcl::body TLFRadioButton::GetCheck {} {
	return $_checked;
}

#-------------------------------------------------------------
# TLF CheckBox class define
#-------------------------------------------------------------
if {[itcl::find classes TLFCheckBox] != "TLFCheckBox"} {
::itcl::class  TLFCheckBox {
	inherit TLFObject

	constructor {{text ""} {check ""}} {};
	
	### data member ###
	protected variable _text;			# ����
	protected variable _checked;		# �Ƿ�ѡ��
	
	### public methods ###
	public method  SerializeVars {dir};	# ���л���������б���
	public method  ProcessFormValue {name value};	# ����Form���
	public method  Draw {};				# �����������
	public method  SetCheck {check};	# ���ü���־
	public method  GetCheck {};			# ��ȡ����־
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFCheckBox::constructor {{text ""} {check ""}} {
	chain;
	set _text $text;
	set _checked $check;
}

#-------------------------------------------------------------
# �������л�
#-------------------------------------------------------------
::itcl::body TLFCheckBox::SerializeVars {dir} {
	chain $dir;
	SerializeVar $dir _checked;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFCheckBox::ProcessFormValue {name value} {
	if {[chain $name $value] == "true"} {
		return true
	}
	set _checked $value
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFCheckBox::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	set blockCheckBox [g_TemplateManager GetTemplateBlock CheckBox CheckBox];
	
	set _contentHtml $blockCheckBox;
	AppendTargetTag "NAME" [GetFullId];
	AppendTargetTag "TEXT" $_text;
	
	set check "";
	switch $_checked {
	true -
	1 -
	on {set check "checked"}
	}
	AppendTargetTag "CHECKED" $check;
	
	AppendTargetTag "DISABLED" $_disabled;

	return [chain];
}

#-------------------------------------------------------------
# ���ü���־
#-------------------------------------------------------------
::itcl::body TLFCheckBox::SetCheck {check} {
	set _checked $check;
}

#-------------------------------------------------------------
# ��ȡ����־
#-------------------------------------------------------------
::itcl::body TLFCheckBox::GetCheck {} {
	return $_checked;
}

#-------------------------------------------------------------
# TLF Select class define
#-------------------------------------------------------------
if {[itcl::find classes TLFSelect] != "TLFSelect"} {
::itcl::class  TLFSelect {
	inherit TLFObject
	
	constructor {{width ""}} {};
	
	### data member ###
	protected variable _width;			# ���
	protected variable _multiple;		# ����ģʽ
	protected variable _count;			# ѡ�������
	protected variable _lsOption;		# ѡ���б�
	protected variable _curSelect;		# ��ǰѡ���
	
	### public methods ###
	public method  SerializeVars {dir};	# ���л���������б���
	public method  ProcessFormValue {name value};	# ����Form���
	public method  SetWidth {width};	# ���ÿ��
	public method  SetMultiple {multiple};# ���ö���ģʽ
	public method  Draw {};				# �����������
	public method  AddOption {text {value ""}};		# �����
	public method  SetSelectValue {curSelect};	# ���õ�ǰѡ����
	public method  GetSelectValue {};	# ��ȡ��ǰѡ�����ֵ
	public method  GetSelectIndex {};	# ��ȡ��ǰѡ���������
	public method  GetSelectText {};	# ��ȡ��ǰѡ���������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFSelect::constructor {{width ""}} {
	chain;
	set _width $width;
	set _multiple "";
	set _count 0;
	set _lsOption {};
	set _curSelect "";
}

#-------------------------------------------------------------
# �������л�
#-------------------------------------------------------------
::itcl::body TLFSelect::SerializeVars {dir} {
	chain $dir;
	SerializeVar $dir _curSelect;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFSelect::ProcessFormValue {name value} {
	if {[chain $name $value] == "true"} {
		return true
	}
	set _curSelect $value
}

#-------------------------------------------------------------
# ���ÿ��
#-------------------------------------------------------------
::itcl::body TLFSelect::SetWidth {width} {
	set _width $width;
}

#-------------------------------------------------------------
# ���ö���ģʽ
#-------------------------------------------------------------
::itcl::body TLFSelect::SetMultiple {multiple} {
	if {($multiple == "true") || ($multiple == "multiple")} {
		set _multiple "multiple";
	} else {
		set _multiple "";
	}
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFSelect::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	set blockSelect [g_TemplateManager GetTemplateBlock Select Select $_styleType];
	set blockOption [g_TemplateManager GetTemplateBlock Select Option];
	
	set _contentHtml $blockSelect;
	
	AppendTargetTag "NAME" [GetFullId];
	AppendTargetTag "DISABLED" $_disabled;
	if {$_multiple != ""} {
		# ����ģʽ(������ͨ������Ч)
		AppendTargetTag "MULTIPLE" "$_multiple";
	} else {
		AppendTargetTag "MULTIPLE" "size=1";
	}
	
	set text_length 0;
	foreach option $_lsOption {
		set tmpOption $blockOption;
		set text [lindex $option 0];
		set value [lindex $option 1];
		if {[string length $text] > $text_length} {
			set text_length [string length $text]
		}
		ReplaceStringTag tmpOption "TEXT" $text
		ReplaceStringTag tmpOption "VALUE" $value
		set selected ""
		if {$_curSelect == $value} {
			set selected "selected"
		}
		ReplaceStringTag tmpOption "SELECTED" $selected
		AppendTargetTag "OPTIONS" $tmpOption -append
	}
	AppendTargetTag "OPTIONS" ""
	
	if {$_styleType == "ss"} {
		AppendJavaScript "js/file/select.js" file;
		if {$_width == ""} {
			set _width [expr $text_length*8+15];
		}
		AppendTargetTag "WIDTH" $_width;
		regsub -all {\n} $_contentHtml {} _contentHtml
	}
	
	return [chain];
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFSelect::AddOption {text {value ""}} {
	if {$value == ""} {
		set value $_count;
	}
	lappend _lsOption [list $text $value];
	incr _count;
}

#-------------------------------------------------------------
# ���õ�ǰѡ����
#-------------------------------------------------------------
::itcl::body TLFSelect::SetSelectValue {curSelect} {
	set _curSelect $curSelect;
}

#-------------------------------------------------------------
# ��ȡ��ǰѡ�����ֵ
#-------------------------------------------------------------
::itcl::body TLFSelect::GetSelectValue {} {
	return $_curSelect;
}

#-------------------------------------------------------------
# ��ȡ��ǰѡ���������
#-------------------------------------------------------------
::itcl::body TLFSelect::GetSelectIndex {} {
	set count 0;
	foreach option $_lsOption {
		if {[lindex $option 1] == $_curSelect} {
			return $count;
		}
		incr count;
	}
	return -1;
}

#-------------------------------------------------------------
# ��ȡ��ǰѡ���������
#-------------------------------------------------------------
::itcl::body TLFSelect::GetSelectText {} {
	set count 0;
	foreach option $_lsOption {
		if {[lindex $option 1] == $_curSelect} {
			return [lindex $option 0];
		}
		incr count;
	}
	return "";
}

#-------------------------------------------------------------
# TLF Input class define
#-------------------------------------------------------------
if {[itcl::find classes TLFInput] != "TLFInput"} {
::itcl::class  TLFInput {
	inherit TLFObject
	
	constructor {{title ""} {text ""} {size ""} {bold ""}} {};
	
	### data member ###
	protected variable _title;			# ����
	protected variable _text;			# ����
	protected variable _size;			# ����
	protected variable _bold;			# ����
	protected variable _rowcount;		# ����
	protected variable _type;			# ����
	
	### public methods ###
	public method  SerializeVars {dir};	# ���л���������б���
	public method  SetType {type};		# ��������
	public method  SetSize {size};		# ���ÿ��
	public method  SetRowCount {rowcount};	# ��������
	public method  SetText {text};		# �����ı�
	public method  GetInputText {};		# ��ȡ����
	public method  ProcessFormValue {name value};	# ����Form���
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFInput::constructor {{title ""} {text ""} {size ""} {bold ""}} {
	chain;
	set _title $title;
	set _text $text;
	set _size $size;
	set _bold $bold;
	set _rowcount 1;
	set _type "text";
}

#-------------------------------------------------------------
# �������л�
#-------------------------------------------------------------
::itcl::body TLFInput::SerializeVars {dir} {
	chain $dir;
	SerializeVar $dir _text;
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFInput::SetType {type} {
	set _type $type;
}

#-------------------------------------------------------------
# ���ÿ��
#-------------------------------------------------------------
::itcl::body TLFInput::SetSize {size} {
	set _size $size;
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFInput::SetRowCount {rowcount} {
	set _rowcount $rowcount;
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFInput::SetText {text} {
	set _text $text;
}

#-------------------------------------------------------------
# ��ȡ����
#-------------------------------------------------------------
::itcl::body TLFInput::GetInputText {} {
	return $_text;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFInput::ProcessFormValue {name value} {
	if {[chain $name $value] == "true"} {
		return true
	}
	set _text $value
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFInput::Draw {} {
	set bold1 "";
	set bold2 "";
	if {$_bold == "bold"} {
		set bold1 "<b>";
		set bold2 "</b>";
	}
	if {$_rowcount == 1} {
		set size "";
		if {$_size != ""} {
			set size "style=\"WIDTH: ${_size}px\""
		}
		set _contentHtml "$bold1$_title$bold2<INPUT class=normal type=$_type $size name=\"[GetFullId]\" value=\"$_text\" $_disabled>";
	} else {
		set _contentHtml "$bold1$_title$bold2<textarea class=normal cols=$_size rows=$_rowcount name=\"[GetFullId]\" $_disabled>$_text</textarea>";
	}
	return [chain];
}

#-------------------------------------------------------------
# TLF PathFileInput class define
#-------------------------------------------------------------
if {[itcl::find classes TLFPathFileInput] != "TLFPathFileInput"} {
::itcl::class  TLFPathFileInput {
	inherit TLFInput
	
	constructor {{title ""} {text ""} {size ""} {bold ""}} {chain $title $text $size $bold} {};
	
	### data member ###
	protected variable _set_by_button;	# ֵ�Ƿ��Ѿ�����ť���ù�
	protected variable _type;			# ����
	protected variable _filter;			# �ļ��Ի�������ַ���
	protected variable _saveto;			# �ļ��Ի�������(�򿪻��Ǳ���)
	protected variable _dlgtitle;		# �Ի������
	
	### public methods ###
	public method  SetType {type};		# ��������(path or file)
	public method  SetFileDlg {filter {saveto ""}};	# �����ļ��Ի������
	public method  SetDlgTitle {dlgtitle};	# ���öԻ������
	public method  ProcessFormValue {name value};	# ����Form���
	public method  OnButtonClick {value};	# ��ť����Ĵ�����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::constructor {{title ""} {text ""} {size ""} {bold ""}} {
	set _type "file";
	set _filter "All Files(*.*)|*.*||";
	set _saveto "";
	set _dlgtitle "";
	set _set_by_button false;
}

#-------------------------------------------------------------
# ��������(path or file)
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::SetType {type} {
	set _type $type;
	set objButton [CreateObject TLFButton];
	$objButton SetImage "image/folder.gif";
	$objButton SetParent $this;
	$objButton SetAction OnButtonClick $this
}

#-------------------------------------------------------------
# �����ļ��Ի������
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::SetFileDlg {filter {saveto ""}} {
	set _filter $filter;
	set _saveto $saveto;
}

#-------------------------------------------------------------
# ���öԻ������
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::SetDlgTitle {dlgtitle} {
	set _dlgtitle $dlgtitle;
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::ProcessFormValue {name value} {
	if {[TLFObject::ProcessFormValue $name $value] == "true"} {
		return true
	}
	if {$_set_by_button != "true"} {
		set _text $value
	}
}

#-------------------------------------------------------------
# ��ť����Ĵ�����
#-------------------------------------------------------------
::itcl::body TLFPathFileInput::OnButtonClick {value} {
	if {$_type == "file"} {
		# �ļ��Ի���
		if {$_saveto != ""} {
			set filepath [FileDlg -filter $_filter $_saveto]
		} else {
			set filepath [FileDlg -filter $_filter]
		}
		if {$filepath != ""} {
			set _text $filepath
			set _set_by_button true
		}
	} elseif {$_type == "path"} {
		# ·��ѡ��Ի���
		set filepath [PathSelect $_dlgtitle $_text]
		if {$filepath != ""} {
			set _text $filepath
			set _set_by_button true
		}
	}
}

#-------------------------------------------------------------
# TLF DateTimeInput class define
#-------------------------------------------------------------
if {[itcl::find classes TLFDateTimeInput] != "TLFDateTimeInput"} {
::itcl::class  TLFDateTimeInput {
	inherit TLFInput
	
	constructor {{title ""} {text ""} {size "100"} {bold ""}} {chain $title $text $size $bold} {};
	
	### data member ###
	protected variable _set_by_button;	# ֵ�Ƿ��Ѿ�����ť���ù�
	protected variable _type;			# ����
	protected variable _objButton;		# ��ť
	protected variable _lsSetup;		# ������Ϣ�б�
	protected variable _flat;			# �Ƿ�flat��������
	
	### public methods ###
	public method  SetProperty {name value};		# ����Setup����
	public method  ProcessFormValue {name value};	# ����Form���
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFDateTimeInput::constructor {{title ""} {text ""} {size "100"} {bold ""}} {
	set _type "date";
	set _set_by_button false;
	set _objButton "";
	set _lsSetup {{ifFormat "%Y-%m-%d"}};
	set _flat false;
}

#-------------------------------------------------------------
# ����Setup����
#-------------------------------------------------------------
::itcl::body TLFDateTimeInput::SetProperty {name value} {
	if {$name == "flat"} {
		# �Ƿ�ƽ������
		set _flat $value
		return
	}
	
	set index 0
	foreach setup $_lsSetup {
		if {[lindex $setup 0] == $name} {
			lset _lsSetup $index 1 $value
			return
		}
		incr index
	}
	lappend _lsSetup [list $name $value]
}

#-------------------------------------------------------------
# ����Form���
#-------------------------------------------------------------
::itcl::body TLFDateTimeInput::ProcessFormValue {name value} {
	if {[TLFObject::ProcessFormValue $name $value] == "true"} {
		return true
	}
	if {$_set_by_button != "true"} {
		set _text $value
	}
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFDateTimeInput::Draw {} {
	global g_TemplateManager;
	
	if {($_flat == "false") && ($_objButton == "")} {
		set _objButton [CreateObject TLFButton];
		$_objButton SetImage "image/calendar.gif";#"image/iconCalendar.gif";
		$_objButton SetParent $this;
	}
	
	if {$_flat == "true"} {
		# ƽ������
		set _contentHtml "<div style='float: right; margin-left: 1em; margin-bottom: 1em;' id='[GetFullId]'></div>";
	} else {
		chain;
	}

	# ���õ�JavaScript��CSS�ļ�
	AppendJavaScript "js/file/jscalendar/skins/aqua/theme.css" css;
	AppendJavaScript "js/file/jscalendar/calendar.js" file;
	AppendJavaScript "js/file/jscalendar/calendar-setup.js" file;
	LangStringEC STR_JSCALENDAR_LANGFILE "calendar-en.js" "calendar-zh.js";
	AppendJavaScript "js/file/jscalendar/lang/$STR_JSCALENDAR_LANGFILE" file;
	
	# ��ȡģ������
	set blockCalendar [g_TemplateManager GetTemplateBlock Calendar Calendar];
	set tmpJBody $blockCalendar;
	
	if {$_flat == "true"} {
		ReplaceStringTag tmpJBody "CALENDAR_SETUP" "flatCallback : \"\"," -append;
	}
	
	if {$_objButton != ""} {
		ReplaceStringTag tmpJBody "CALENDAR_SETUP" "button : \"[$_objButton GetFullId]\"," -append;
	}
	foreach setup $_lsSetup {
		ReplaceStringTag tmpJBody "CALENDAR_SETUP" "[lindex $setup 0] : \"[lindex $setup 1]\"," -append;
	}
	if {$_flat == "true"} {
		ReplaceStringTag tmpJBody "CALENDAR_SETUP" "flat : \"[GetFullId]\"" -append;
	} else {
		ReplaceStringTag tmpJBody "CALENDAR_SETUP" "inputField : \"[GetFullId]\"" -append;
	}
	ReplaceStringTag tmpJBody "CALENDAR_SETUP" "";
	
	AppendJavaScript $tmpJBody body;
	
	return $_contentHtml;#[chain];
}

#-------------------------------------------------------------
# TLF Button class define
#-------------------------------------------------------------
if {[itcl::find classes TLFButton] != "TLFButton"} {
::itcl::class  TLFButton {
	inherit TLFObject
	
	constructor {{text ""} {bold ""}} {};
	
	### data member ###
	protected variable _text;			# ����
	protected variable _bold;			# ����
	protected variable _image;			# ͼƬ
	protected variable _style;			# ���
	
	### public methods ###
	public method  SetText {text};		# ��������
	public method  SetBold {bold};		# ���ô���
	public method  SetImage {image};	# ����ͼƬ
	public method  SetStyle {style};	# ���÷��
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFButton::constructor {{text ""} {bold ""}} {
	chain;
	set _text $text;
	set _bold $bold;
	set _image "";
	set _style "";
}

#-------------------------------------------------------------
# ��������
#-------------------------------------------------------------
::itcl::body TLFButton::SetText {text} {
	set _text $text;
}

#-------------------------------------------------------------
# ���ð�ť����
#-------------------------------------------------------------
::itcl::body TLFButton::SetBold {bold} {
	set _bold $bold;
}

#-------------------------------------------------------------
# ����ͼƬ
#-------------------------------------------------------------
::itcl::body TLFButton::SetImage {image} {
	set _image $image;
}

#-------------------------------------------------------------
# ���÷��
#-------------------------------------------------------------
::itcl::body TLFButton::SetStyle {style} {
	set _style $style;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFButton::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	if {$_image != ""} {
		set blockButton [g_TemplateManager GetTemplateBlock Button Image];
		set _contentHtml $blockButton;
		AppendTargetTag "NAME" [GetFullId];
		AppendTargetTag "VALUE" $_text;
		AppendTargetTag "IMAGE" $_image;
	} else {
		set blockButton [g_TemplateManager GetTemplateBlock Button Button];
		set _contentHtml $blockButton;
		AppendTargetTag "NAME" [GetFullId];
		AppendTargetTag "VALUE" $_text;
		if {$_style == ""} {
			AppendTargetTag "STYLE" "";
		} else {
			AppendTargetTag "STYLE" "style='$_style'";
		}
	}
	
	AppendTargetTag "DISABLED" $_disabled;
	
	if {($_submitActionObj != "") && ($_submitAction != "")} {
		AppendTargetTag "ONSUBMIT" "[$_submitActionObj GetObjId].$_submitAction;return false;";
	}
	AppendTargetTag "ONSUBMIT" ""
	
	return [chain];
}

#-------------------------------------------------------------
# TLF table class define
#-------------------------------------------------------------
if {[itcl::find classes TLFTable] != "TLFTable"} {
::itcl::class  TLFTable {
	inherit TLFObject
	
	constructor {} {};
	
	### data member ###
	private variable _lsTitle;			# �����б�
	private variable _lsLine;			# ���б�
	private variable _haveTitle;		# �Ƿ��б�����
	private variable _gridColor;		# �������ɫ
	private variable _titleColor;		# ����������ɫ
	private variable _titleBgColor;		# ���ⱳ����ɫ
	private variable _lineColor;		# ����������ɫ
	private variable _lineBgColor;		# ���ı�����ɫ
	private variable _item_style;		# TD���е�style����
	
	### public methods ###
	public method  Draw {};				# �����������
	public method  SetHaveTitle {enable};# �����Ƿ��б�����
	public method  SetItemStyle {style};# ����TD���е�style����
	public method  SetGridColor {color};# ���ñ������ɫ
	public method  SetTitleColor {color};# ���ñ�����������ɫ
	public method  SetTitleBgColor {color};# ���ñ����б�����ɫ
	public method  SetLineColor {color};# ���ñ����������ɫ
	public method  SetLineBgColor {color};# ���ñ���б�����ɫ
	public method  SetOneLineStyle {lineno colorBg {colorText ""} {bold ""}};	# ���ñ��һ�з��
	public method  SetOneColumnStyle {columnno colorBg {colorText ""} {bold ""}};	# ���ñ��һ�з��
	public method  AddColumn {title {color ""} {bold ""} {bgColor ""} {width ""}};	# �����
	public method  AddLine {args};		# �����
	public method  AddLine2 {args};		# �����
	public method  DeleteLine {lineno};	# ɾ����
	public method  GetLineCount {};		# ��ȡ����
	public method  SetItemText {lineno colno text {link ""} {tip ""} {color ""} {bgColor ""} {bold ""}}; #����ĳһ��Ԫ����������(��ѡ���ӡ���ʾ����ɫ������ɫ������)
	public method  GetItemText {lineno colno};			# ��ȡĳһ��Ԫ������
	public method  SetItemColor {lineno colno color {bgColor ""}};	# ����ĳһ��Ԫ����ɫ
	public method  AppendItemText {lineno colno text};	# ��ĳһ��Ԫ���������������(��ӵ�����)
	public method  AppendItemObject {lineno colno args};# ��ĳһ��Ԫ������Ӷ���(��ӵ�����)
	public method  GetItemObject {lineno colno};	# ��ȡ������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFTable::constructor {} {
	chain;
	set _addTrTd true;
	set _lsTitle {};
	set _lsLine {};
	set _haveTitle true;
	set _gridColor "";
	set _titleColor "";
	set _titleBgColor "";
	set _lineColor "";
	set _lineBgColor "";
	set _item_style "";
	SetDrawObjs false;	# ��ֹ�����Ӷ���
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFTable::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	set blockFrame [g_TemplateManager GetTemplateBlock Table Frame];
	set blockTitle [g_TemplateManager GetTemplateBlock Table Title];
	set blockTitleItem [g_TemplateManager GetTemplateBlock Table TitleItem];
	set varsTitle [g_TemplateManager GetTemplateVars Table TitleItem];
	set varsItem [g_TemplateManager GetTemplateVars Table Item];
	set blockLine [g_TemplateManager GetTemplateBlock Table Line];
	set blockItem [g_TemplateManager GetTemplateBlock Table Item];
	set _contentHtml $blockFrame
	
	AppendTargetTag "DISABLED" $_disabled;
	
	if {$_gridColor == ""} {
		set varsFrame [g_TemplateManager GetTemplateVars Table Frame];
		set _gridColor [GetVarValue $varsFrame bgColor]
	}
	AppendTargetTag "GRIDCOLOR" $_gridColor
	
	if {$_haveTitle == "true"} {
		# ������
		set tmpTitle $blockTitle
		foreach title $_lsTitle {
			set tmp $blockTitleItem
			
			set title_text [lindex $title 0]
			set title_color [lindex $title 1]
			set title_bold [lindex $title 2]
			set title_bgColor [lindex $title 3]
			set title_width [lindex $title 4]
			
			if {$title_color == ""} {
				set title_color [GetVarValueDefault $_titleColor $varsTitle color]
			}
			
			set color1 ""
			set color2 ""
			if {$title_color != ""} {
				set color1 "<font color='$title_color'>"
				set color2 "</font>"
			}
			
			if {$title_bold == ""} {
				set title_bold [GetVarValue $varsTitle bold]
			}
	
			set bold1 ""
			set bold2 ""
			if {$title_bold == "bold"} {
				set bold1 "<b>"
				set bold2 "</b>"
			}
			
			if {$title_bgColor == ""} {
				set title_bgColor [GetVarValueDefault $_titleBgColor $varsTitle bgColor]
			}
			
			if {$title_width != ""} {
				set title_width "width='$title_width'"
			}

			ReplaceStringTag tmp "WIDTH" $title_width
			ReplaceStringTag tmp "BGCOLOR" $title_bgColor
			ReplaceStringTag tmp "TEXT" "$color1$bold1$title_text$bold2$color2"
			ReplaceStringTag tmpTitle "TITLE" $tmp -append
		}
		ReplaceStringTag tmpTitle "TITLE" ""
		AppendTargetTag "TITLE" $tmpTitle -append
	}
	AppendTargetTag "TITLE" ""
	
	set line_color $_lineColor
	set line_bgColor [GetVarValueDefault $_lineBgColor $varsItem bgColor]
	
	foreach line $_lsLine {
		set tmpLine $blockLine
		set columnIndex 0
		foreach item $line {
			set tmpItem $blockItem
			
			set item_color $line_color
			set item_bgcolor $line_bgColor
			if {[lindex $item 5] != ""} {
				set item_bgcolor [lindex $item 5]
			}
			
			set item_text [lindex $item 0]
			set item_type [lindex $item 1]
			
			set item_width [lindex [lindex $_lsTitle $columnIndex] 4]
			if {$item_width != ""} {
				set item_width "width='$item_width'"
			}
			
			if {$item_type == "object"} {
				ReplaceStringTag tmpItem "WIDTH" $item_width
				ReplaceStringTag tmpItem "BGCOLOR" $item_bgcolor
				ReplaceStringTag tmpItem "STYLE" $_item_style
				foreach obj $item_text {
					# �����TLF����,��ֱ�ӵ��ö����Draw������ȡ���
					ReplaceStringTag tmpItem "TEXT" [$obj Draw] -append
				}
				ReplaceStringTag tmpItem "TEXT" ""
				ReplaceStringTag tmpLine "ITEM" $tmpItem -append
				continue
			}

			set item_link [lindex $item 2]
			set item_tip [lindex $item 3]
			if {$item_link != ""} {
				set item_color ""
			}
			if {[lindex $item 4] != ""} {
				set item_color [lindex $item 4]
			}
			set item_bold [lindex $item 6]
			
			set link1 ""
			set link2 ""
			if {$item_link != ""} {
				set link1 "<a title='$item_tip' href='$item_link'>"
				set link2 "</a>"
			}
			
			set color1 ""
			set color2 ""
			if {$item_color != ""} {
				set color1 "<font color='$item_color'>"
				set color2 "</font>"
			}
			
			set bold1 ""
			set bold2 ""
			if {$item_bold == "bold"} {
				set bold1 "<b>"
				set bold2 "</b>"
			}
			
			ReplaceStringTag tmpItem "WIDTH" $item_width
			ReplaceStringTag tmpItem "BGCOLOR" $item_bgcolor
			ReplaceStringTag tmpItem "STYLE" $_item_style
			ReplaceStringTag tmpItem "TEXT" "$link1$color1$bold1$item_text$bold2$color2$link2"
			ReplaceStringTag tmpLine "ITEM" $tmpItem -append
			
			incr columnIndex
		}
		ReplaceStringTag tmpLine "ITEM" ""
		AppendTargetTag "LINE" $tmpLine -append
	}
	AppendTargetTag "LINE" ""
	
	return [chain];
}

#-------------------------------------------------------------
# �����Ƿ��б�����
#-------------------------------------------------------------
::itcl::body TLFTable::SetHaveTitle {enable} {
	set _haveTitle $enable;
}

#-------------------------------------------------------------
# ����TD���е�style����
#-------------------------------------------------------------
::itcl::body TLFTable::SetItemStyle {style} {
	set _item_style "style='$style'";
}

#-------------------------------------------------------------
# ���ñ������ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetGridColor {color} {
	set _gridColor $color;
}

#-------------------------------------------------------------
# ���ñ�����������ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetTitleColor {color} {
	set _titleColor $color;
}

#-------------------------------------------------------------
# ���ñ����б�����ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetTitleBgColor {color} {
	set _titleBgColor $color;
}

#-------------------------------------------------------------
# ���ñ����������ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetLineColor {color} {
	set _lineColor $color;
}

#-------------------------------------------------------------
# ���ñ���б�����ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetLineBgColor {color} {
	set _lineBgColor $color;
}

#-------------------------------------------------------------
# �����
#-------------------------------------------------------------
::itcl::body TLFTable::AddColumn {title {color ""} {bold ""} {bgColor ""} {width ""}} {
	lappend _lsTitle [list $title $color $bold $bgColor $width];
}

#-------------------------------------------------------------
# �����
#-------------------------------------------------------------
::itcl::body TLFTable::AddLine {args} {
	set line {}
	foreach item_text $args {
		# �ֱ�Ϊ���֡����͡����ӡ���ʾ����ɫ������ɫ������
		# ����Ϊ�ձ�ʾ��ͨ����,object��ʾTLF����
		set item [list $item_text "" "" "" "" "" ""]
		lappend line $item
	}
	lappend _lsLine $line;
	return [expr [llength $_lsLine] -1];
}

#-------------------------------------------------------------
# �����(֧�����ӡ���ʾ)
#-------------------------------------------------------------
::itcl::body TLFTable::AddLine2 {args} {
	lappend _lsLine $args;
	set count 0
	foreach item $args {
		if {[lindex $item 1] == "object"} {
			set lsTmp [list CreateObject]
			foreach arg [lindex $item 0] {
				lappend lsTmp $arg
			}
			set objTmp [eval $lsTmp];
			lset _lsLine end $count 0 $objTmp;
		} elseif {[lindex $item 1] == "objname"} {
			# ���ö���ĸ�����Ͷ���Id
			[lindex $item 0] SetParent $_obj_id;
			[lindex $item 0] SetObjId [lindex $item 0];
		}
		incr count;
	}
	return [expr [llength $_lsLine] -1];
}

#-------------------------------------------------------------
# ɾ����
#-------------------------------------------------------------
::itcl::body TLFTable::DeleteLine {lineno} {
	if {$lineno == "all"} {
		# ɾ������
		set _lsLine {}
	} else {
		# ɾ��ָ����
		set lsTmp {}
		set index 0
		foreach line $_lsLine {
			if {$index != $lineno} {
				lappend lsTmp $line
			}
			incr index
		}
		set _lsLine $lsTmp
	}
}

#-------------------------------------------------------------
# ��ȡ����
#-------------------------------------------------------------
::itcl::body TLFTable::GetLineCount {} {
	return [llength $_lsLine];
}

#-------------------------------------------------------------
# ����ĳһ��Ԫ����������(��ѡ���ӡ���ʾ����ɫ������ɫ������)
#-------------------------------------------------------------
::itcl::body TLFTable::SetItemText {lineno colno text {link ""} {tip ""} {color ""} {bgColor ""} {bold ""}} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	
	# ����в���,����
	set line [lindex $_lsLine $lineno]
	if {[llength $line] <= $colno} {
		while {[llength $line] <= $colno} {
			 lappend line ""
		}
		lset _lsLine $lineno $line
	}
	
	set item [lindex [lindex $_lsLine $lineno] $colno]
	if {([lindex $item 1] == "object") || ([lindex $item 1] == "objname")} {
		return
	}
	while {[llength $item] < 7} {
		lappend item ""
	}
	
	lset item 0 "$text"
	if {$link != ""} {
		lset item 2 "$link"
	}
	if {$tip != ""} {
		lset item 3 "$tip"
	}
	if {$color != ""} {
		lset item 4 "$color"
	}
	if {$bgColor != ""} {
		lset item 5 "$bgColor"
	}
	if {$bold != ""} {
		lset item 6 "$bold"
	}
	lset _lsLine $lineno $colno $item
}

#-------------------------------------------------------------
# ��ȡĳһ��Ԫ������
#-------------------------------------------------------------
::itcl::body TLFTable::GetItemText {lineno colno} {
	if {$lineno >= [llength $_lsLine]} {
		return ""
	}
	
	# ����в���,����
	if {[llength [lindex $_lsLine $lineno]] <= $colno} {
		return ""
	}
	
	set item [lindex [lindex $_lsLine $lineno] $colno]
	if {([lindex $item 1] == "object") || ([lindex $item 1] == "objname")} {
		# ���ض�����
		return [lindex $item 0]
	} else {
		# ������������
		return [lindex $item 0]
	}
}

#-------------------------------------------------------------
# ����ĳһ��Ԫ����ɫ
#-------------------------------------------------------------
::itcl::body TLFTable::SetItemColor {lineno colno color {bgColor ""}} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	
	# ����в���,���˳�
	if {[llength [lindex $_lsLine $lineno]] <= $colno} {
		return
	}

	set item [lindex [lindex $_lsLine $lineno] $colno]
	while {[llength $item] < 7} {
		lappend item ""
	}
	if {$color != ""} {
		lset item 4 $color
	}
	if {$bgColor != ""} {
		lset item 5 $bgColor
	}
	lset _lsLine $lineno $colno $item
}

#-------------------------------------------------------------
# ��ĳһ��Ԫ���������������(��ӵ�����)
#-------------------------------------------------------------
::itcl::body TLFTable::AppendItemText {lineno colno text} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	
	# ����в���,����
	set line [lindex $_lsLine $lineno]
	if {[llength $line] <= $colno} {
		while {[llength $line] <= $colno} {
			 lappend line ""
		}
		lset _lsLine $lineno $line
	}
	
	set item [lindex [lindex $_lsLine $lineno] $colno]
	if {([lindex $item 1] == "object") || ([lindex $item 1] == "objname")} {
		return
	}
	
	lset _lsLine $lineno $colno 0 "[lindex $item 0]$text"
}

#-------------------------------------------------------------
# ��ĳһ��Ԫ������Ӷ���(��ӵ�����)
#-------------------------------------------------------------
::itcl::body TLFTable::AppendItemObject {lineno colno args} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	
	# ����в���,����
	set line [lindex $_lsLine $lineno]
	if {[llength $line] <= $colno} {
		while {[llength $line] <= $colno} {
			 lappend line ""
		}
		lset _lsLine $lineno $line
	}
	
	set item [lindex [lindex $_lsLine $lineno] $colno]
	if {([lindex $item 1] != "object") && ([lindex $item 0] != "")} {
		return
	}
	
	# ��װ�����в���������
	set lsTmp [list CreateObject]
	foreach arg $args {
		lappend lsTmp $arg
	}
	
	set objTmp [eval $lsTmp]
	# ������ʱ�������б�
	set lsObjTmp [lindex $item 0]
	lappend lsObjTmp $objTmp
	if {[lindex $item 1] != "object"} {
		# ����˵�Ԫ����δ����κ�����,����������������
		lset _lsLine $lineno $colno [list $lsObjTmp "object"]
	} else {
		# ����ֻ�����Ӷ������б�
		lset _lsLine $lineno $colno 0 $lsObjTmp
	}
	
	return $objTmp
}

#-------------------------------------------------------------
# ����һ�з��
#-------------------------------------------------------------
::itcl::body TLFTable::SetOneLineStyle {lineno colorBg {colorText ""} {bold ""}} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	set count 0
	foreach item [lindex $_lsLine $lineno] {
		while {[llength $item] < 7} {
			lappend item ""
		}
		if {[llength $item] > 6} {
			if {$colorBg != ""} {
				lset item 5 $colorBg
			}
			if {$colorText != ""} {
				lset item 4 $colorText
			}
			if {$bold != ""} {
				lset item 6 $bold
			}
		}
		lset _lsLine $lineno $count $item
		incr count
	}	
}

#-------------------------------------------------------------
# ����һ�з��
#-------------------------------------------------------------
::itcl::body TLFTable::SetOneColumnStyle {columnno colorBg {colorText ""} {bold ""}} {
	set lineno 0
	foreach line $_lsLine {
		# ��λ����
		if {[llength $line] < $columnno} {
			while {[llength $line] < $columnno} {
				lappend line [list ""]
			}
			lset _lsLine $lineno $line
		}
		set item [lindex $line $columnno]
		# ���÷��
		while {[llength $item] < 7} {
			lappend item ""
		}
		if {[llength $item] > 6} {
			if {$colorBg != ""} {
				lset item 5 $colorBg
			}
			if {$colorText != ""} {
				lset item 4 $colorText
			}
			if {$bold != ""} {
				lset item 6 $bold
			}
		}
		lset _lsLine $lineno $columnno $item
		incr lineno
	}	
}

#-------------------------------------------------------------
# ��ȡ������
#-------------------------------------------------------------
::itcl::body TLFTable::GetItemObject {lineno colno} {
	if {$lineno >= [llength $_lsLine]} {
		return
	}
	set item [lindex [lindex $_lsLine $lineno] $colno]
	if {[lindex $item 1] != "object"} {
		return
	}
	
	return [lindex $item 0]
}

#-------------------------------------------------------------
# TLF toggle menu class define
#-------------------------------------------------------------
if {[itcl::find classes TLFToggleMenu] != "TLFToggleMenu"} {
::itcl::class  TLFToggleMenu {
	inherit TLFObject
	
	constructor {} {};
	
	### data member ###
	protected variable _lsMenu;			# �˵��б�
	
	### public methods ###
	public method  Draw {};				# �����������
	public method  ClearMenu {};		# ������в˵���
	public method  AddMenu {id text image display submenu};	# ��Ӳ˵���
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFToggleMenu::constructor {} {
	chain;
	set _lsMenu {};
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFToggleMenu::Draw {} {
	global g_TemplateManager;
	
	# ��ȡģ������
	set blockMainMenu [g_TemplateManager GetTemplateBlock ToggleMenu MainMenu];
	set blockSubMenu [g_TemplateManager GetTemplateBlock ToggleMenu SubMenu];
	set _contentHtml ""
	set _id_array_script "<SCRIPT type=text/javascript>var array_toggle_menu_id = new Array("
	
	set count 0
	foreach menu $_lsMenu {
		set tmp $blockMainMenu
		
		set menu_id [lindex $menu 0]
		set menu_text [lindex $menu 1]
		set menu_image [lindex $menu 2]
		set menu_display [lindex $menu 3]
		set lsSubMenu [lindex $menu 4]

		ReplaceStringTag tmp "ID_MAIN_MENU" $menu_id
		ReplaceStringTag tmp "STR_MAIN_MENU" $menu_text
		ReplaceStringTag tmp "IMG_MAIN_MENU" $menu_image
		ReplaceStringTag tmp "SUB_MENU_DISPLAY" $menu_display
		
		foreach submenu $lsSubMenu {
			set tmpSubMenu $blockSubMenu
			ReplaceStringTag tmpSubMenu "STR_SUB_MENU" [lindex $submenu 0]
			ReplaceStringTag tmpSubMenu "SUB_MENU_LINK" [lindex $submenu 1]
			
			ReplaceStringTag tmp "BLOCK_SUB_MENU" $tmpSubMenu -append
		}
		
		ReplaceStringTag tmp "BLOCK_SUB_MENU" ""
		
		set _contentHtml "$_contentHtml\n$tmp"
		
		incr count
		set _id_array_script "$_id_array_script \"$menu_id\""
		if {$count != [llength $_lsMenu]} {
			set _id_array_script "$_id_array_script,"
		}
	}
	
	set _id_array_script "$_id_array_script );</SCRIPT>"
	
	set _contentHtml "$_id_array_script\n$_contentHtml"
	
	return [chain];
}

#-------------------------------------------------------------
# ������в˵���
#-------------------------------------------------------------
::itcl::body TLFToggleMenu::ClearMenu {} {
	set _lsMenu {}
}

#-------------------------------------------------------------
# ��Ӳ˵���
#-------------------------------------------------------------
::itcl::body TLFToggleMenu::AddMenu {id text image display submenu} {
	lappend _lsMenu [list $id $text $image $display $submenu]
}

#-------------------------------------------------------------
# TLF FieldSet class define
#-------------------------------------------------------------
if {[itcl::find classes TLFFieldSet] != "TLFFieldSet"} {
::itcl::class  TLFFieldSet {
	inherit TLFObject
	
	constructor {{title ""}} {};
	
	### data member ###
	protected variable _title;			# ���ڱ���
	
	### public methods ###
	public method  Draw {};				# �����������
	public method  SetTitle {title};	# ���ñ���
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFFieldSet::constructor {{title ""}} {
	chain;
	set _title $title;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFFieldSet::Draw {} {
	global g_TemplateManager;
	
	chain;
	
	# ��ȡģ������
	set blockFieldSet [g_TemplateManager GetTemplateBlock FieldSet FieldSet];
	
	set tempContentHtml $blockFieldSet;
	
	ReplaceStringTag tempContentHtml "LEGEND" $_title;
	ReplaceStringTag tempContentHtml "DISABLED" $_disabled;
	ReplaceStringTag tempContentHtml "CONTENT" $_contentHtml;
	set _contentHtml $tempContentHtml;

	return $_contentHtml;
}

#-------------------------------------------------------------
# ���ñ���
#-------------------------------------------------------------
::itcl::body TLFFieldSet::SetTitle {title} {
	set _title $title;
}

#-------------------------------------------------------------
# TLF TabPage class define
#-------------------------------------------------------------
if {[itcl::find classes TLFTabPage] != "TLFTabPage"} {
::itcl::class  TLFTabPage {
	inherit TLFObject

	constructor {{title ""}} {};
	
	### data member ###
	protected variable _title;			# ҳ�����
	
	### public methods ###
	public method  Draw {};				# �����������
	public method  SetTitle {title};	# ����ҳ�����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFTabPage::constructor {{title ""}} {
	chain;
	set _title $title;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFTabPage::Draw {} {
	global g_TemplateManager;
	
	chain;
	
	# ��ȡģ������
	set blockTabPage [g_TemplateManager GetTemplateBlock TabPage TabPage];
	
	set tempContentHtml $blockTabPage;
	
	ReplaceStringTag tempContentHtml "ID" [GetFullId];
	ReplaceStringTag tempContentHtml "TPVAR" [GetParent];
	ReplaceStringTag tempContentHtml "TITLE" $_title;
	ReplaceStringTag tempContentHtml "CONTENT" $_contentHtml;
	set _contentHtml $tempContentHtml;

	return $_contentHtml;
}

#-------------------------------------------------------------
# ����ҳ�����
#-------------------------------------------------------------
::itcl::body TLFTabPage::SetTitle {title} {
	set _title $title;
}

#-------------------------------------------------------------
# TLF TabPane class define
#-------------------------------------------------------------
if {[itcl::find classes TLFTabPane] != "TLFTabPane"} {
::itcl::class  TLFTabPane {
	inherit TLFObject

	constructor {{style ""}} {};
	
	### data member ###
	protected variable _style;			# ���
	
	### public methods ###
	public method  Draw {};				# �����������
	public method  SetStyle {style};	# ���÷��
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFTabPane::constructor {{style ""}} {
	chain;
	set _style $style;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFTabPane::Draw {} {
	global g_TemplateManager;
	
	chain;
	
	AppendJavaScript "js/file/tabpane/tabpane.js" file
	AppendJavaScript "js/file/tabpane/css/tab.webfx.css" css
	
	# ��ȡģ������
	set blockTabPane [g_TemplateManager GetTemplateBlock TabPane TabPane];
	
	set tempContentHtml $blockTabPane;
	
	ReplaceStringTag tempContentHtml "ID" [GetFullId];
	ReplaceStringTag tempContentHtml "TPVAR" [GetObjId];
	
	# ���ҳ�����
	ReplaceStringTag tempContentHtml "PAGES" $_contentHtml;
	set _contentHtml $tempContentHtml;

	return $_contentHtml;
}

#-------------------------------------------------------------
# ���÷��
#-------------------------------------------------------------
::itcl::body TLFTabPane::SetStyle {style} {
	set _style $style;
}

#-------------------------------------------------------------
# TLF frame class define
#-------------------------------------------------------------
if {[itcl::find classes TLFFrame] != "TLFFrame"} {
::itcl::class  TLFFrame {
	inherit TLFObject
}
}

#-------------------------------------------------------------
# TLF Window class define
#-------------------------------------------------------------
if {[itcl::find classes TLFWindow] != "TLFWindow"} {
::itcl::class  TLFWindow {
	inherit TLFObject
	
	constructor {{title ""} {type ""}} {};
	
	### data member ###
	protected variable _type;			# ��������
	protected variable _title;			# ���ڱ���
	protected variable _width;			# ���ڿ��
	protected variable _height;			# ���ڸ߶�
	protected variable _left;			# ��������x
	protected variable _top;			# ��������y
	protected variable _icon;			# ����ͼ��
	
	### public methods ###
	public method  SetSize {width height};# ���ô��ڴ�С
	public method  SetWindowPos {left top};# ���ô�������
	public method  SetWindowIcon {ico};	# ���ô���ͼ��
	public method  Draw {};				# �����������
	public method  Show {};				# ��ʾ����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFWindow::constructor {{title ""} {type ""}} {
	chain;
	set _title $title;
	set _type $type;
	set _width "";
	set _height "";
	set _left "";
	set _top "";
	set _icon "";
}

#-------------------------------------------------------------
# ���ô��ڴ�С
#-------------------------------------------------------------
::itcl::body TLFWindow::SetSize {width height} {
	set _width $width;
	set _height $height;
}

#-------------------------------------------------------------
# ���ô�������
#-------------------------------------------------------------
::itcl::body TLFWindow::SetWindowPos {left top} {
	set _left $left;
	set _top $top;
}

#-------------------------------------------------------------
# ���ô���ͼ��
#-------------------------------------------------------------
::itcl::body TLFWindow::SetWindowIcon {ico} {
	set _icon $ico;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFWindow::Draw {} {
	global g_TemplateManager;
	
	chain;

	if {$_type == "popwin"} {
		AppendJavaScript "PopWindow" class
	} else {
		AppendJavaScript "Window" class
	}
	
	# ��ȡģ������
	set blockWindow [g_TemplateManager GetTemplateBlock Window Window];
	set tmpJBody $blockWindow;
	
	if {$_type == "popwin"} {
		ReplaceStringTag tmpJBody "WINCLASS" "PopWindow";
	} else {
		ReplaceStringTag tmpJBody "WINCLASS" "Window";
	}
	ReplaceStringTag tmpJBody "WINNAME" $_obj_id;
	ReplaceStringTag tmpJBody "WININIT" "$_obj_id.SetDivId(\"$_obj_id\");" -append;
	
	if {$_title != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Title = \"$_title\";" -append;
	}
	if {$_width != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Width = $_width;" -append;
	}
	if {$_height != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Height = $_height;" -append;
	}
	if {$_left != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Left = $_left;" -append;
	}
	if {$_top != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Top = $_top;" -append;
	}
	if {$_icon != ""} {
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Ico = $_icon;" -append;
	}
	if {$_contentHtml != ""} {
		regsub -all {\n} $_contentHtml {} _contentHtml;
		regsub -all {\\} $_contentHtml {\\\\} _contentHtml;
		regsub -all {"} $_contentHtml {\"} _contentHtml;
		ReplaceStringTag tmpJBody "WININIT" "$_obj_id.Content = \"<table width='100%'>$_contentHtml</table>\";" -append;
	}
	ReplaceStringTag tmpJBody "WININIT" "";
	
	AppendJavaScript $tmpJBody body;
	
	return "";#[chain];
}

#-------------------------------------------------------------
# ��ʾ����
#-------------------------------------------------------------
::itcl::body TLFWindow::Show {} {
	if {$_type == "popwin"} {
		return "$_obj_id.Show(\"$_obj_id\");"
	} else {
		return "$_obj_id.Show();"
	}
}

#-------------------------------------------------------------
# TLF Dialog class define
#-------------------------------------------------------------
if {[itcl::find classes TLFDialog] != "TLFDialog"} {
::itcl::class  TLFDialog {
	inherit TLFObject

	constructor {{title ""} {width ""} {height ""}} {};
	
	### data member ###
	protected variable _title;			# ����
	protected variable _width;			# ���
	protected variable _height;			# �߶�
	protected variable _icon;			# ͼ��
	
	### public methods ###
	public method  SetSize {width height};# ���ô��ڴ�С
	public method  SetWindowIcon {ico};	# ���ô���ͼ��
	public method  Draw {};				# �����������
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFDialog::constructor {{title ""} {width ""} {height ""}} {
	chain;
	set _title $title;
	set _width $width;
	set _height $height;
	set _icon "";
}

#-------------------------------------------------------------
# ���ô��ڴ�С
#-------------------------------------------------------------
::itcl::body TLFDialog::SetSize {width height} {
	set _width $width;
	set _height $height;
}

#-------------------------------------------------------------
# ���ô���ͼ��
#-------------------------------------------------------------
::itcl::body TLFDialog::SetWindowIcon {ico} {
	set _icon $ico;
}

#-------------------------------------------------------------
# �����������
#-------------------------------------------------------------
::itcl::body TLFDialog::Draw {} {
	global g_TemplateManager;
	
	set temp_contentHtml [chain];
	
	# ��ȡģ������
	set blockDialog [g_TemplateManager GetTemplateBlock Dialog Dialog];
	set _contentHtml $blockDialog;
	
	AppendTargetTag "DISABLED" $_disabled;
	if {$_width != ""} {
		AppendTargetTag "WIDTH" "width='$_width'";
	} else {
		AppendTargetTag "WIDTH" $_width;
	}
	if {$_height != ""} {
		AppendTargetTag "HEIGHT" "height='$_height'";
	} else {
		AppendTargetTag "HEIGHT" $_height;
	}
	AppendTargetTag "TITLE" $_title;
	
	AppendTargetTag "CONTENT" $temp_contentHtml;
	
	return $_contentHtml;
}

#-------------------------------------------------------------
# TLF Portlet class define
#-------------------------------------------------------------
if {[itcl::find classes TLFPortlet] != "TLFPortlet"} {
::itcl::class  TLFPortlet {
	inherit TLFObject
}
}

#-------------------------------------------------------------
# TLFPage class define
#-------------------------------------------------------------
if {[itcl::find classes TLFPage] != "TLFPage"} {
::itcl::class  TLFPage {
	inherit TLFObject
	
	constructor {} {};
	
	### data member ###
	protected variable _templateStyle;#ģ����
	protected variable _targetHtml;	#Ŀ��ҳ��
	protected variable _xmlSerializeFile;	#���л��õ�XML�ļ�
	protected variable _targetVarList;#�洢Ŀ��ҳ�������б���
	protected variable _stylePath;	#��ǰ���·��
	protected variable _htmlTitle;	#ҳ�����
	protected variable _actionArgs;	#�����������
	protected variable _javaScriptFile;		#JavaScript����(�ļ�)
	protected variable _javaScriptCss;		#JavaScript����(CSS����)
	protected variable _javaScriptClass;	#JavaScript����(��)
	protected variable _javaScriptPrototype;#JavaScript����(������չ)
	protected variable _javaScriptBody;		#JavaScript����(����)
	
	### public methods ###
	public method  Init {targetHtml templatePath templateHtml {style "default"}};
	public method  Reset {};						#��λ����
	public method  GetStylePath {};					#��ȡ��ǰ���·��
	public method  SetTitle {title};				#����ҳ�����
	public method  SetMenuDisable {disable};		#����ҳ���Ƿ������Ҽ��˵�
	public method  MakeTargetHtml {};				#����Ŀ��ҳ��
	public method  LoadTargetHtmlVar {};			#��Ŀ��ҳ���ж�ȡ���б�����Ϣ
	public method  LoadFromXmlFile {};				#��XML�ļ��ж�ȡ�������л���Ϣ
	public method  SaveToXmlFile {};				#д�����л���Ϣ��XML�ļ�
	public method  SaveTargetVar {varName varValue};#��ָ��������Ϣд��Ŀ�껺����
	public method  GetTargetVar {varName};			#��ȡĿ��ҳ����ָ��������ֵ
	public method  ParseActionArgs {strAction};		#�����������,��ִ�ж�������
	public method  ProcessFormValues {};			# ����Form���
	public method  AppendJavaScript {jscript {type "file"}};#���JavaScript
	public method  CreateJavaScriptDefine {};		# ����JavaScript����
	public method  OnPaint {};						# Paint������
	public method  OnAfterPaint {};					# ҳ���Ѿ���ʾ֮����õĴ�����(����Խ�����פ��ģʽ)
	public method  Redirect {{url ""}};				# ҳ���ض���
	public method  InitialMessageFilter {};			# ��ʼ��ҳ����Ϣ�����б�
	public method  ProcessMessage {msg};			# ҳ����Ϣ����
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFPage::constructor {} {
	chain;
	SetEnableSerial false;
	set _xmlSerializeFile "";
	array set _targetVarList {};
	array set _actionArgs {};
	set _javaScriptFile {};		# ��ʼ��JavaScript�ļ��б�
	set _javaScriptCss {};		# ��ʼ��JavaScript CSS�ļ��б�
	set _javaScriptClass {};	# ��ʼ��JavaScript���б�
	set _javaScriptPrototype {};# ��ʼ��JavaScript������չ�����б�
	set _javaScriptBody {};		# ��ʼ��JavaScript�����б�
	set _contentTag "TABLE_ALL";
}

#-------------------------------------------------------------
#	��ʼ��
#-------------------------------------------------------------
::itcl::body TLFPage::Init {targetHtml templatePath templateHtml {style "default"}} {
	global g_tlf_mode platform_path_style platform_ui_style
	set _targetHtml $targetHtml;
	# ���ģ��·��������,����Ĭ��·��
	if {[file exist "$templatePath/$style"] == 0} {
		set style "default";
	}
	# ��ȡģ����ģ��ҳ��
	set fi [open "$templatePath/$style/$templateHtml" r];
	set _contentHtml [read $fi];
	close $fi;
	if {$g_tlf_mode == "tsf"} {
		set _stylePath "$platform_path_style/$platform_ui_style";
	} else {
		set _stylePath "$templatePath/$style";
	}
}

#-------------------------------------------------------------
# ��λ����
#-------------------------------------------------------------
::itcl::body TLFPage::Reset {} {
	chain;
	array unset _targetVarList;
	array unset _actionArgs;
	array set _targetVarList {};
	array set _actionArgs {};
	set _javaScriptFile {};		# ��ʼ��JavaScript�ļ��б�
	set _javaScriptCss {};		# ��ʼ��JavaScript CSS�ļ��б�
	set _javaScriptClass {};	# ��ʼ��JavaScript���б�
	set _javaScriptPrototype {};# ��ʼ��JavaScript������չ�����б�
	set _javaScriptBody {};		# ��ʼ��JavaScript�����б�
}

#-------------------------------------------------------------
#	��ȡ��ǰ���·��
#-------------------------------------------------------------
::itcl::body TLFPage::GetStylePath {} {
	return $_stylePath
}

#-------------------------------------------------------------
#	����ҳ�����
#-------------------------------------------------------------
::itcl::body TLFPage::SetTitle {title} {
	set _htmlTitle $title
	AppendTargetTag "HTML_TITLE" $title
}

#-------------------------------------------------------------
#	����ҳ���Ƿ������Ҽ��˵�
#-------------------------------------------------------------
::itcl::body TLFPage::SetMenuDisable {disable} {
	if {$disable == "disablemenu"} {
		set menu_disable_script "<SCRIPT LANGUAGE=javascript>document.oncontextmenu = function(){return false;};</SCRIPT>"
		AppendTargetTag "MENU_DISABLE_SCRIPT" $menu_disable_script
	}	
}

#-------------------------------------------------------------
#	����Ŀ��ҳ��
#-------------------------------------------------------------
::itcl::body TLFPage::MakeTargetHtml {} {
	global g_tlf_mode
	
	# ����ҳ������
	LangString charset english "UTF-8"
	LangString charset chinese "GB2312"
	AppendTargetTag "PAGE_CHARSET" $charset
	# �˵���ֹ�ű�
	AppendTargetTag "MENU_DISABLE_SCRIPT" ""
	# �滻��ǰ���·��
	AppendTargetTag "STYLE_PATH" $_stylePath
	
	Draw;
	
	# ����JavaScript����������
	CreateJavaScriptDefine
	
	if {$g_tlf_mode == "tsf"} {
		return $_contentHtml;
	} else {	
		set fi [open $_targetHtml w+];
		puts $fi $_contentHtml;
		close $fi;
	}
}

#-------------------------------------------------------------
#	��Ŀ��ҳ���ж�ȡ���б�����Ϣ
#	����������ʽ<!-- @VarName=Value@ -->
#-------------------------------------------------------------
::itcl::body TLFPage::LoadTargetHtmlVar {} {
	global g_tlf_mode
	if {$g_tlf_mode == "tsf"} {
		return
	}
	
	catch {
		set fi [open $_targetHtml r];
		set targetHtml [read $fi];
		close $fi;
		# �������б������洢�������һ��������
		set ls [regexp -all -inline -line "^<!-- @(\\\w+)=(.*?)@ -->$" $targetHtml];
		set index 0;
		set name "";
		foreach item $ls {
			if {$index%3 == 1} {
				set name $item;
			} elseif {$index%3 == 2} {
				set _targetVarList($name) $item;
			}		
			incr index;
		}
	}
}

#-------------------------------------------------------------
#	��XML�ļ��ж�ȡ�������л���Ϣ
#-------------------------------------------------------------
::itcl::body TLFPage::LoadFromXmlFile {} {
	if {[GetEnableSerial] == "false"} {
		return
	}
	
	if { [ catch {
		if {$_xmlSerializeFile == ""} {
			set scriptfile [info script]
			regsub -nocase {\.tcl$} $scriptfile ".xml" _xmlSerializeFile
		}

		if {($_xmlSerializeFile != "") && [file exists $_xmlSerializeFile]} {
			set xml [xml_open file $_xmlSerializeFile]
			set nodePage [xml_get $xml node $_obj_id]
			Serialize get $xml $nodePage
			xml_close $xml
		}
	} errInfo ]} {
		puts stderr $errInfo
	}
}

#-------------------------------------------------------------
#	д�����л���Ϣ��XML�ļ�
#-------------------------------------------------------------
::itcl::body TLFPage::SaveToXmlFile {} {
	global platform_path
	
	if {[GetEnableSerial] == "false"} {
		return
	}
	
	if { [ catch {
		if {($_xmlSerializeFile != "") && ([file exists $_xmlSerializeFile] == 0)} {
			# ���XML�ļ������ھʹ�ģ��·������һ��ģ�����
			file copy "$platform_path/Template/Html/serial.xml" $_xmlSerializeFile
		}
		
		if {($_xmlSerializeFile != "") && [file exists $_xmlSerializeFile]} {
			set xml [xml_open file $_xmlSerializeFile]
			set nodePage [xml_get $xml node $_obj_id]
			Serialize set $xml $nodePage
			xml_write $xml file $_xmlSerializeFile
			xml_close $xml			
		}
	} errInfo ]} {
		puts stderr $errInfo
	}
}

#-------------------------------------------------------------
#	��ָ��������Ϣд��Ŀ�껺����
#-------------------------------------------------------------
::itcl::body TLFPage::SaveTargetVar {varName varValue} {
	# �洢��Ŀ�껺����
	set index [string first "<!-- @$varName=" $_contentHtml];
	if { $index != -1 } {
		set len [string length "<!-- @$varName="];
		incr index $len;
		set index2 [string first "@ -->" $_contentHtml [expr $index]];
		if { $index2 != -1 } {
			set _contentHtml [string replace $_contentHtml $index $index2 "$varValue@"];
		}
	}
}

#-------------------------------------------------------------
#	��ȡĿ��ҳ����ָ��������ֵ
#-------------------------------------------------------------
::itcl::body TLFPage::GetTargetVar {varName} {
	set value "";
	catch {
		set a $_targetVarList($varName);
		set value $_targetVarList($varName);
	}
	return $value;
}

#-------------------------------------------------------------
#	�����������,��ִ�ж�������
#-------------------------------------------------------------
::itcl::body TLFPage::ParseActionArgs {strAction} {
	set strAction "$strAction&"
	regsub -all {&nbsp;} $strAction " " strAction
	regsub -all {\r\n} $strAction "\n" strAction
	#puts $strAction
	set lsAction [regexp -all -inline {([\w\(\)\.\-# ]+)=([^&]*)&} $strAction]
	foreach {all name value} $lsAction {
		if {$name == "action"} {
			set _action $value
		} else {
			set _actionArgs($name) $value
			#puts [array names _actionArgs]
		}
	}
	# ���ض���������
	return $_action
}

#-------------------------------------------------------------
#	����Form���
#-------------------------------------------------------------
::itcl::body TLFPage::ProcessFormValues {} {
	#puts "++[array names _actionArgs]"
	# ����Form����
	foreach name [array names _actionArgs] {
		ProcessFormValue $name $_actionArgs($name)
		#puts "$name=$_actionArgs($name)"
	}
}

#-------------------------------------------------------------
# ���JavaScript
#-------------------------------------------------------------
::itcl::body TLFPage::AppendJavaScript {jscript {type "file"}} {
	# ���û����ӹ�,����ӵ�JavaScript�б���
	switch $type {
		file {
			if {[lsearch $_javaScriptFile $jscript] == -1} {
				lappend _javaScriptFile $jscript
			}
		}
		css {
			if {[lsearch $_javaScriptCss $jscript] == -1} {
				lappend _javaScriptCss $jscript
			}
		}
		class {
			if {[lsearch $_javaScriptClass $jscript] == -1} {
				lappend _javaScriptClass $jscript
			}
		}
		prototype {
			if {[lsearch $_javaScriptPrototype $jscript] == -1} {
				lappend _javaScriptPrototype $jscript
			}
		}
		body {
			if {[lsearch $_javaScriptBody $jscript] == -1} {
				lappend _javaScriptBody $jscript
			}
		}
	}
}

#-------------------------------------------------------------
# ����JavaScript����
#-------------------------------------------------------------
::itcl::body TLFPage::CreateJavaScriptDefine {} {
	# ����ļ�
	foreach jfile $_javaScriptCss {
		AppendTargetTag "EXT_JSCRIPT_FILE" "<link rel=\"stylesheet\" type=\"text/css\" href=\"$jfile\" />" -append
	}
	foreach jfile $_javaScriptFile {
		AppendTargetTag "EXT_JSCRIPT_FILE" "<SCRIPT src=\"$jfile\" type=text/javascript></SCRIPT>" -append
	}
	AppendTargetTag "EXT_JSCRIPT_FILE" ""
	
	# ���class
	AppendTargetTag "LOAD_JSCRIPT_CLASS" [join $_javaScriptClass ";"]
	
	# ���prototype
	AppendTargetTag "LOAD_JSCRIPT_PROTOTYPE" [join $_javaScriptPrototype ";"]
	
	# ���ִ����
	foreach jbody $_javaScriptBody {
		AppendTargetTag "JSCRIPT_BODY" $jbody -append
	}
	AppendTargetTag "JSCRIPT_BODY" ""
}

#-------------------------------------------------------------
#	Paint
#-------------------------------------------------------------
::itcl::body TLFPage::OnPaint {} {
}

#-------------------------------------------------------------
#	After Paint(only exec at interp trs mode)
#-------------------------------------------------------------
::itcl::body TLFPage::OnAfterPaint {} {
}

#-------------------------------------------------------------
#	Page Redirect
#-------------------------------------------------------------
::itcl::body TLFPage::Redirect {{url ""}} {
	if {$url == ""} {
		set url $_form_action
	}
	# ʹ��javascript�ű�����ҳ���location.href������ʵ��ҳ���ض���
	set jsRelocation "window.location.href='$url'"
	regsub -all {\\} $jsRelocation "/" jsRelocation
	AppendJavaScript $jsRelocation body
}

#-------------------------------------------------------------
# ��ʼ��ҳ����Ϣ�����б�
#-------------------------------------------------------------
::itcl::body TLFPage::InitialMessageFilter {} {
}

#-------------------------------------------------------------
# ҳ����Ϣ����
#-------------------------------------------------------------
::itcl::body TLFPage::ProcessMessage {msg} {
}


#-------------------------------------------------------------
# TLFPageCreater class define
#-------------------------------------------------------------
if {[itcl::find classes TLFPageCreater] != "TLFPageCreater"} {
::itcl::class  TLFPageCreater {
	
	constructor { {title ""} } {};
	
	### data member ###
	protected variable _objname;	#ҳ�����
	protected variable _title;		#ҳ�����
	protected variable _pageClass;	#ҳ������
	protected variable _template;	#ҳ��ģ���ļ�
	protected variable _disableMenu;#�����Ҽ��˵�
	protected variable _outFile;	#�����ļ���
	protected variable _footInfo;	#ҳ��ײ���Ϣ
	protected variable _startpage;	#��ʼҳ����
	
	### public methods ###
	public method  GetPageObj {};				#��ȡҳ�����
	public method  SetPageClass { pageclass };	#����ҳ����
	public method  SetTemplate { template };	#����ҳ��ģ��
	public method  SetMenuDisable { disable };	#�����Ҽ��˵��Ƿ�����
	public method  SetOutFile { outfile };		#��������ļ�
	public method  SetFootInfo { footinfo };	#���õײ���Ϣ
	public method  SetStartPage { startpage };	#������ʼҳ����
	public method  CreatePage {{objname "obj_#auto"}};	#����ҳ��
	public method  MakeHtml {};		#����Htmlҳ��
}
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
::itcl::body TLFPageCreater::constructor { {title ""} } {
	global g_tlf_obj_count
	set g_tlf_obj_count 0
	set _objname ""
	set _title $title
	set _pageClass "TLFPage"
	set _template "normal.tph"
	set _disableMenu ""
	set _outFile "index.htm"
	set _startpage ""
	# ���ð�Ȩ
	#set copyright [vci_exec plat GetPlatCopyRight]
	set copyright [platform_info copyright]
	set _footInfo $copyright
}

#-------------------------------------------------------------
# ��ȡҳ�����
#-------------------------------------------------------------
::itcl::body TLFPageCreater::GetPageObj {} {
	return $_objname
}

#-------------------------------------------------------------
# ����ҳ����
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetPageClass { pageclass } {
	set _pageClass $pageclass
}

#-------------------------------------------------------------
# ����ҳ��ģ��
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetTemplate { template } {
	set _template $template
}

#-------------------------------------------------------------
# �����Ҽ��˵��Ƿ�����
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetMenuDisable { disable } {
	if {$disable == "true"} {
		set _disableMenu "disablemenu"
	} else {
		set _disableMenu ""
	}
}

#-------------------------------------------------------------
# ��������ļ�
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetOutFile { outfile } {
	set _outFile $outfile
}

#-------------------------------------------------------------
# ���õײ���Ϣ
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetFootInfo { footinfo } {
	set _footInfo $footinfo
}

#-------------------------------------------------------------
# ������ʼҳ����
#-------------------------------------------------------------
::itcl::body TLFPageCreater::SetStartPage { startpage } {
	set _startpage $startpage
}

#-------------------------------------------------------------
# ����ҳ��
#-------------------------------------------------------------
::itcl::body TLFPageCreater::CreatePage {{objname "obj_#auto"}} {
	global platform_path platform_ui_style _htmlNewURL g_TemplateManager
	global argc argv platform_argv
	global g_tlf_obj_count
	
	# ����ҳ�����
	set _htmlCurrentFile "$platform_path/html/$_outFile"
	if {$_objname == ""} {
		#set _objname [$_pageClass $objname]
		set _objname [$_pageClass "obj_tLFAppPage$g_tlf_obj_count"]
		incr g_tlf_obj_count
	} else {
		# ��λҳ�����
		$_objname Reset
	}
	
	$_objname Init "$_htmlCurrentFile" "$platform_path/template/html" \
						$_template $platform_ui_style;# "class"
	$_objname SetObjId $_objname
	
	# ��ʼ��ҳ����Ϣ�����б�
	$objname InitialMessageFilter
	
	# ����css��jscript
	set stylePath [$_objname GetStylePath]
	set lsCssFile [g_TemplateManager GetTemplateConstValue CSS_FILE]
	if {[llength $lsCssFile] == 1} {
		$_objname AppendTargetTag "CSS_FILE" "$stylePath/[lindex $lsCssFile 0]"
	} else {
		foreach cssFile $lsCssFile {
			set css "<LINK href=\"$stylePath/$cssFile\" type=text/css rel=stylesheet>"
			$_objname AppendTargetTag "CSS_FILES" $css -append
		}
		$_objname AppendTargetTag "CSS_FILES" ""
	}
	set lsJScriptFile [g_TemplateManager GetTemplateConstValue INCLUDE_JSCRIPT]
	if {[llength $lsJScriptFile] == 1} {
		$_objname AppendTargetTag "INCLUDE_JSCRIPT" "$stylePath/[lindex $lsJScriptFile 0]"
	} else {
		foreach jscriptFile $lsJScriptFile {
			set jscript "<SCRIPT src=\"$stylePath/$jscriptFile\" type=text/javascript></SCRIPT>"
			$_objname AppendTargetTag "INCLUDE_JSCRIPTS" $jscript -append
		}
		$_objname AppendTargetTag "INCLUDE_JSCRIPTS" ""
	}
		
	$_objname AppendTargetTag "COPYRIGHT" $_footInfo
	
	# ���ñ���
	if {$_title != ""} {
		$_objname SetTitle $_title
	}
	
	# ����StartPage
	if {$_startpage == ""} {
		set startpage [platform_info startpage]
	} else {
		set startpage $_startpage
	}
	$_objname AppendTargetTag "STARTPAGE" $startpage
	
	# ���ý�ֹ�Ҽ��˵����
	$_objname SetMenuDisable $_disableMenu

	# ����ҳ��λ��
	set _htmlNewURL "$platform_path/html/$_outFile";
	# ����
	$_objname OnCreate;
	# ����XML�����ļ�
	$_objname LoadFromXmlFile;
	# ����֮��
	$_objname OnAfterCreate;
	if {$argc >= 1} {
		set actionArgs $platform_argv
	} else {
		set actionArgs "action=OnPaint"
	}
	#puts $actionArgs
	set actionResult true
	set action [$_objname ParseActionArgs $actionArgs]
	if {$action == ""} {
		set action "OnPaint"
	}
	$_objname ProcessFormValues
	if {[$_objname GetAction] != ""} {
		set action [$_objname GetAction]
	}
	#puts "---$action"
	# ҳ���ʼ��
	$_objname OnInitialUpdate;

	# ִ����Ӧ�����ĺ���
	if {[catch {
			set actionResult [$_objname $action]
		} errInfo]} {
		puts stderr $errInfo
	}
	
	# ����ִ�����Ĳ���
	$_objname OnEndAction;

	# ���ִ�гɹ�,�ͱ���XML�����ļ�
	if {$actionResult != "false"} {
		$_objname SaveToXmlFile
	}

	return $actionResult
}

#-------------------------------------------------------------
# ����Htmlҳ��
#-------------------------------------------------------------
::itcl::body TLFPageCreater::MakeHtml {} {
	set actionResult [CreatePage]
	# ����Html�ļ�
	if {$actionResult != "false"} {
		$_objname MakeTargetHtml
	}
}


#-------------------------------------------------------------
#	����һ��TLFҳ��
#-------------------------------------------------------------
proc TLFCreatePage {{title ""} {page_class "TLFPage"} {template_file "normal.tph"} {disable_menu ""} {startpage ""} {makehtml "true"} {outfile "index.htm"} {footinfo ""}} {
	global platform_path platform_ui_style _htmlNewURL g_TemplateManager
	global argc argv platform_argv
	global g_tlf_obj_count g_tlf_mode
	
	# ��λTLF������������
	set g_tlf_obj_count 0
	
	if {$page_class == ""} {
		set page_class "TLFPage"
	}
	
	if {$template_file == ""} {
		set template_file "normal.tph"
	}
	
	if {$outfile == ""} {
		set outfile "index.htm"
	}
	
	# ����ҳ�����
	set _htmlCurrentFile "$platform_path/html/$outfile"
	
	# ����Ѿ�������ҳ�����,��ʹ���Ѿ������Ķ���
	set lsObjPage [itcl::find objects -class $page_class]
	if {[llength $lsObjPage] > 0} {
		set objname [lindex $lsObjPage 0]
		# ��λҳ�����
		$objname Reset
	} else {
		#set objname [$page_class "obj_#auto"]
		#set objname [$page_class "obj_tlf_$g_tlf_obj_count"]
		set objname [$page_class "obj_tLFAppPage$g_tlf_obj_count"]
		incr g_tlf_obj_count
	}

	$objname Init "$_htmlCurrentFile" "$platform_path/template/html" \
						$template_file $platform_ui_style;# "class"
	$objname SetObjId $objname
	
	# ��ʼ��ҳ����Ϣ�����б�
	$objname InitialMessageFilter
	
	# ����css��jscript
	set stylePath [$objname GetStylePath]
	set lsCssFile [g_TemplateManager GetTemplateConstValue CSS_FILE]
	if {[llength $lsCssFile] == 1} {
		$objname AppendTargetTag "CSS_FILE" "$stylePath/[lindex $lsCssFile 0]"
	} else {
		foreach cssFile $lsCssFile {
			set css "<LINK href=\"$stylePath/$cssFile\" type=text/css rel=stylesheet>"
			$objname AppendTargetTag "CSS_FILES" $css -append
		}
		$objname AppendTargetTag "CSS_FILES" ""
	}
	set lsJScriptFile [g_TemplateManager GetTemplateConstValue INCLUDE_JSCRIPT]
	if {[llength $lsJScriptFile] == 1} {
		$objname AppendTargetTag "INCLUDE_JSCRIPT" "$stylePath/[lindex $lsJScriptFile 0]"
	} else {
		foreach jscriptFile $lsJScriptFile {
			set jscript "<SCRIPT src=\"$stylePath/$jscriptFile\" type=text/javascript></SCRIPT>"
			$objname AppendTargetTag "INCLUDE_JSCRIPTS" $jscript -append
		}
		$objname AppendTargetTag "INCLUDE_JSCRIPTS" ""
	}
	
	# ���ð�Ȩ
	#set copyright [vci_exec plat GetPlatCopyRight]
	set copyright [platform_info copyright]

	if {$footinfo != ""} {
		set copyright $footinfo
	}
		
	$objname AppendTargetTag "COPYRIGHT" $copyright
	
	# ���ñ���
	if {$title != ""} {
		$objname SetTitle $title
	}
	
	# ����StartPage
	if {$startpage == ""} {
		set startpage [platform_info startpage]
	}
	$objname AppendTargetTag "STARTPAGE" $startpage
	
	# ���ý�ֹ�Ҽ��˵����
	$objname SetMenuDisable $disable_menu

	# ����ҳ��λ��
	set _htmlNewURL "$platform_path/html/$outfile";
	# ����
	$objname OnCreate;
	# ����XML�����ļ�
	$objname LoadFromXmlFile;
	# ����֮��
	$objname OnAfterCreate;
	if {$argc >= 1} {
		#set actionArgs [lindex $argv 0]
		set actionArgs $platform_argv
	} else {
		set actionArgs "action=OnPaint"
	}
	#puts $actionArgs
	
	set actionResult true
	set action [$objname ParseActionArgs $actionArgs]
	if {$action == ""} {
		set action "OnPaint"
	}
	$objname ProcessFormValues
	if {[$objname GetAction] != ""} {
		set action [$objname GetAction]
	}
	#puts "---$action"
	
	# ҳ���ʼ��
	$objname OnInitialUpdate;

	# ִ����Ӧ�����ĺ���
	if {[catch {
			set actionResult [$objname $action]
		} errInfo]} {
		puts stderr $errInfo
	}
	
	# ����ִ�����Ĳ���
	$objname OnEndAction;

	# ���ִ�гɹ�,�ͱ���XML�����ļ�
	if {$actionResult != "false"} {
		$objname SaveToXmlFile
	}

	# ����Html�ļ�
	if {($makehtml == "true") && ($actionResult != "false") && ($g_tlf_mode == "tlf")} {
		$objname MakeTargetHtml
	}

	return $objname;
}

#-------------------------------------------------------------
#	��ʾHtmlҳ��
#-------------------------------------------------------------
proc ShowHtmlPage { {url ""} } {
	global platform_path
	
	if {$url == ""} {
		set url "$platform_path/html/index.htm"
	}
	
	owm -sendcmd "org.owm.htmlview" 84 "<string>$url" 0
}