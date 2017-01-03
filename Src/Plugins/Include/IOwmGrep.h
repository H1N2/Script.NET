////////////////////////////////////////////////////////////////////////////
//	File:		IPlatUI.h
//	Version:	1.0.0.0
//	Created:	2006-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	owm grep command define
////////////////////////////////////////////////////////////////////////////
#ifndef __IOWMGREP_H_
#define __IOWMGREP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// OwmGrep�����
#define OC_GREP_SETTEXTFIND		100		// ���ò����ַ���
#define OC_GREP_GETTEXTFIND		101		// ��ȡ�����ַ���
#define OC_GREP_SETTEXTREPLACE	102		// �����滻�ַ���
#define OC_GREP_GETTEXTREPLACE	103		// ��ȡ�滻�ַ���
#define OC_GREP_SETLOCATION		104		// ����λ��
#define OC_GREP_GETLOCATION		105		// ��ȡλ��
#define OC_GREP_SETOPTION		106		// ���ò����滻����
#define OC_GREP_GETOPTION		107		// ��ȡ�����滻����
#define OC_GREP_CLEARRESULT		108		// ��ս����
#define OC_GREP_ADDRESULTLINE	109		// ��ӽ����

// ���ҽ������Ϣ
struct SearchResultInfo {
	SearchResultInfo(LPCTSTR lpszPath, int nLine, LPCTSTR lpszLine)
		: _strPath(lpszPath), _nLine(nLine), _strLine(lpszLine) {};
	CString _strPath;
	int		_nLine;
	CString	_strLine;
};

// ��������
struct SearchOption {
	BOOL    m_bIncludeSubFolders;
	BOOL    m_bRegularExpressions;
	CString m_strFind;
	CString m_strReplace;
	CString m_strPath;
	CString m_strFileTypes;
	BOOL    m_bMatchCase;
	BOOL    m_bMatchWholeWords;
};

// λ������
enum {GREP_LOC_CURRENTDOC, GREP_LOC_OPENDOCS, GREP_LOC_CURRENTPRJ, GREP_LOC_PATH};

#define LOC_CURRENTDOC	_T("Current Document")
#define LOC_OPENDOCS	_T("All Open Documents")
#define LOC_CURRENTPRJ	_T("Current Project")



#endif // !defined(__IOWMGREP_H_)