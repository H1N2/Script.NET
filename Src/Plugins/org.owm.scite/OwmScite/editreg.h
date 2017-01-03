////////////////////////////////////////////////////////////////////////////
//	File:		editcmd.h
//	Version:	1.0.0.0
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CRYSEDITREG_H__INCLUDED_
#define __CRYSEDITREG_H__INCLUDED_

//	Registry keys & values
#define REG_OWMEDIT			_T("owm\\OwmScite")
#define REG_DEFAULTDOC		_T("DefaultDoc")
#define REG_INTERPNAME		_T("InterpName")
#define REG_AUTORESET_INTERP	_T("AutoResetInterp")
#define REG_EDITORSTYLE		_T("EditorStyle")

#define REG_FIND_SUBKEY		_T("owm\\OwmScite\\Find")
#define REG_REPLACE_SUBKEY	_T("owm\\OwmScite\\Replace")
#define REG_FIND_HISTORY	_T("owm\\OwmScite\\FindHistory")
#define REG_MATCH_CASE		_T("MatchCase")
#define REG_WHOLE_WORD		_T("WholeWord")
#define REG_FIND_WHAT		_T("FindWhat")
#define REG_REPLACE_WITH	_T("ReplaceWith")

#define REG_PAGE_SUBKEY		_T("owm\\OwmScite\\PageSetup")
#define REG_MARGIN_LEFT		_T("LeftMargin")
#define REG_MARGIN_RIGHT	_T("RightMargin")
#define REG_MARGIN_TOP		_T("TopMargin")
#define REG_MARGIN_BOTTOM	_T("BottomMargin")

// �༭����Python�Ӽ�
#define REG_PYTHON_SUBKEY		_T("owm\\OwmScite\\Python")
#define REG_PYTHON_TAB2SPACE	_T("Tab2Space")		// TABת��Ϊ�ո�

// �����ӽ�
#define REG_HELP_SUBKEY			_T("owm\\OwmScite\\Help")
#define REG_HELP_WORD_TOOLTIP	_T("EnableWordTooltip")		// ������ʾ������ʾ��Ϣ
#define REG_HELP_VAR_TOOLTIP	_T("EnableVarTooltip")		// �����ڵ���״̬����ʾ������ʾ��Ϣ
#define REG_HELP_WORDHELP_IN_DYNHELP _T("ShowWordHelpInDynamicHelp")	// �ڶ�̬������������ʾ���ʰ���
#define REG_HELP_SEARCHWORD_ACCURATE _T("SearchWordAccurate")	// ʹ�þ�ȷƥ����е��ʰ�������
#define REG_HELP_AUTO_COMPLETE	_T("AutoComplete")			// ����������Զ���ɹ���

// ���������ʽ�Ӽ�
#define REG_FORMAT_SUBKEY		_T("Format")
#define REG_FMFONT				_T("Font")			// ������
#define REG_FMSIZE				_T("Size")			// ��С
#define REG_FMCOLORFG			_T("ColorFg")		// ǰ��ɫ
#define REG_FMCOLORBK			_T("ColorBk")		// ����ɫ
#define REG_FMBOLD				_T("Bold")			// �Ƿ����
#define REG_FMITALIC			_T("Italic")		// �Ƿ�б��
#define REG_FMUNDERLINE			_T("UnderLine")		// �»���
#define REG_FMSTRIKEOUT			_T("StrikeOut")		// ɾ����


#endif
