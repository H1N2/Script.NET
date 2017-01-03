////////////////////////////////////////////////////////////////////////////
//	File:		editcmd.h
//	Version:	1.0.0.0
//	Created:	29-Dec-1998
//
//	Author:		Stcherbatchenko Andrei
//	E-mail:		windfall@gmx.de
//
//	Command defintions and resource IDs for Crystal Edit - syntax coloring
//	text editor.
//	Include this in .RC file in your project.
//
//	You are free to use, distribute or modify this code
//	but LEAVE THIS HEADER INTACT.
////////////////////////////////////////////////////////////////////////////

#ifndef __EDITCMD_H_INCLUDED_
#define __EDITCMD_H_INCLUDED_

#define ID_MACRO 20000
#define ID_MACRO_LIMIT 20200

#define ID_USER_CMD 21000
#define ID_USER_CMD_LIMIT 21200

#define ID_PLUGINS_CMD 22000
#define ID_PLUGINS_CMD_LIMIT 22500

////////////////////////////////////////////////////////////////////
// OwmEdit��չ����
#define OC_OPENFILE				102	// ��һ���ĵ�(to owm)
#define OC_OPENFILEFIRST		103	// �ڵ�һ��λ�ô��ĵ�(to owm)
#define OC_SETREADONLY			104	// �����ĵ���ֻ������
#define OC_GETOPENFILE			105	// ��ȡ�򿪵��ļ���

#define OC_SETEXECUTION			110	// ���ô�����
#define OC_SETCURENTLINE		111	// ���õ�ǰ��
#define OC_SETCURENTEXECLINE	112	// ���õ�ǰִ����
#define OC_SEARCHLINE			113	// ����ָ�����ݵ��в�����Ϊ��ǰ��
#define OC_GETBREAKLIST			115	// ��ȡ�ϵ��б�
#define OC_SETSLAVEDEBUG		116	// ���ôӵ���ģʽ(����ģʽ,���������Գ���)

#define OC_GET_LINECOUNT		117	// ��ȡ������
#define OC_GET_LINE				118	// ��ȡ��
#define OC_SET_LINE				119	// ������
#define OC_GET_SELECTLINE		120	// ��ȡ��ǰѡ����з�Χ
#define OC_SET_SELECTLINE		121	// ����ѡ�����
#define OC_GET_CURSORPOS		122	// ��ȡ���λ��
#define OC_SET_CURSORPOS		123	// ���ù��λ��
#define OC_SET_BREAKLIST		124	// ���öϵ��б�
#define OC_CLEAR_BREAK			125	// ��նϵ�

#define OC_EDIT_FIND			130	// ����
#define OC_EDIT_REPLACE			131	// �滻

#define OC_WORD_HELP			140	// ���ʰ���


#endif
