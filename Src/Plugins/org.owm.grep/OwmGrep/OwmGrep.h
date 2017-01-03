// OwmGrep.h : main header file for the OWMGREP DLL
//

#ifndef __OWMGREP_H__
#define __OWMGREP_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COwmGrepApp
// See OwmGrep.cpp for the implementation of this class
//

class COwmGrepView;
class COwmGrepApp : public CWinApp
{
public:
	COwmGrepApp();

	CString GetModulePath();
	CString	GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = "");
	int		GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0);
	CString WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	int		WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	CSearchOptionsView* GetSearchOptionsView();
	COwmGrepView* GetSearchResultView(int nIndex = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COwmGrepApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(COwmGrepApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	OWM_LINKAGE	m_ol;		// OWM�ӿڵĿ���
	BOOL		m_bDebug;	// ���Կ���
	CMultiDocTemplate*	m_pDocTemplate;	// �ĵ�ģ��
	int			m_nLanguage;// ��������
	IPlatUI*	m_pIPlatUI;	// ƽ̨�ӿ�ָ��
	CWnd*		m_pSearchOptionsView;	// ����SearchOptionView��ָ��
	CWnd*		m_pSearchResultView;	// ������ҽ�����ڵ�ָ��
};

class COwmGrepView;
COwmGrepView* GetFirstOwmGrepView();

extern class COwmGrepApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__OWMGREP_H__)
