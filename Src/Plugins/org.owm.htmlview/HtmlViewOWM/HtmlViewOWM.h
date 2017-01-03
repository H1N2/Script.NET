// HtmlViewOWM.h : main header file for the HTMLVIEWOWM DLL
//

#ifndef __HTMLVIEWOWM_H__
#define __HTMLVIEWOWM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewOWMApp
// See HtmlViewOWM.cpp for the implementation of this class
//

class CHtmlViewOWMApp : public CWinApp
{
public:
	CHtmlViewOWMApp();

	CString	GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = "");
	int		GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault = 0);
	CString WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	int		WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	CString GetPlatRootPath();
	CString GetModulePath();
	void	ProcessPath(CString& strPath);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlViewOWMApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHtmlViewOWMApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	OWM_LINKAGE	m_ol;		// OWM�ӿڵĿ���
	BOOL		m_bDebug;	// ���Կ���
	CMultiDocTemplate*	m_pDocTemplate;	// �ĵ�ģ��
	int			m_nLanguage;// ��������
	IPlatUI*	m_pIPlatUI;

	CString		m_strDefaultPage;	// ȱʡ��ҳ��
	CPtrList	m_HtmlBarList;		// Html Bar�����б�
};

extern class CHtmlViewOWMApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__HTMLVIEWOWM_H__)
