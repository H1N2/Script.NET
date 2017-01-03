////////////////////////////////////////////////////////////////////////////
//	File:		terminal.h
//	Version:	1.0.0.0
//	Created:	2001-10-05
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of P terminal.
////////////////////////////////////////////////////////////////////////////
#ifndef __TERMINAL_H_
#define __TERMINAL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define OC_TERM_SWITCHSHELL			110	// �л��ն����
#define OC_TERM_DOCK_SWITCHSHELL	111	// �л�Docking�����ն����(ͨ������������)

// �����������
enum{
	SHELL_TCL,			// ����TCL����̨
	SHELL_PYTHON,		// ����Python����̨
	SHELL_LUA,			// ����Lua����̨
	SHELL_RUBY,			// ����Ruby����̨
	SHELL_PERL,			// ����Perl����̨
	SHELL_TELNET,		// TELNET����̨
	SHELL_COM,			// ���ڿ���̨
	SHELL_RCADMIN,		// Script.NETԶ�̿���̨
};

// ������ʾ��
#define SHELL_PROMPT_LOCAL	_T("%")			// ���ؿ���̨
#define SHELL_PROMPT_TCL	_T("TCL%")		// ����TCL����̨
#define SHELL_PROMPT_PYTHON	_T(">>>")		// ����Python����̨
#define SHELL_PROMPT_RUBY	_T("RUBY>")			// ����Ruby����̨
#define SHELL_PROMPT_LUA	_T("LUA>")			// ����Lua����̨
#define SHELL_PROMPT_PERL	_T("PERL>")			// ����Perl����̨
#define SHELL_PROMPT_TELNET	_T("TELNET>")	// TELNET�ͻ���
#define SHELL_PROMPT_COM	_T("COM>")		// ���ڿ���̨
#define SHELL_PROMPT_ADMIN	_T("Admin#")	// ATEStudioԶ�̿���̨
// ������չ��ʾ��
#define SHELL_EXTPROMPT_PYTHON	_T("...")	// Python��չ��ʾ��
#define SHELL_EXTPROMPTLINE_PYTHON	_T("\r\n...")// �����з���Python��չ��ʾ��

// ��������ض���
#define INTERP_NAME		_T("Interp_Script")	// ��������

// ������ɫ
#define SHELL_COLOR_PROMPT	RGB(0, 0, 0)	// ��ʾ����ɫ
#define SHELL_COLOR_INPUT	RGB(0, 0, 255)	// �����ַ���ɫ
#define SHELL_COLOR_RESOK	RGB(128, 0, 255)// ����������
#define SHELL_COLOR_RESFAIL	RGB(255, 0, 0)	// ʧ�ܽ�����

//	Registry keys & values
#define REG_PTERMINAL		_T("owm\\PTerminal")
#define REG_DEFAULTSHELL	_T("DefaultShell")
#define REG_INTERPNAME		_T("InterpName")
#define REG_RCADMIN_TCLINTERPNAME	_T("RemoteInterpName")
#define REG_RCADMIN_IP      _T("RemoteServerIP")
#define REG_RCADMIN_USER    _T("RemoteClientUser")
#define REG_ENABLE_LISTDLG	_T("EnableListDlg")	// �Ƿ������ԶԻ���ʽ��ʾ�����б�

// �������̨��¼����
//#define IDS_DOMAIN_TELNET	_T("Telnet������")

#endif // !defined(__TERMINAL_H_)