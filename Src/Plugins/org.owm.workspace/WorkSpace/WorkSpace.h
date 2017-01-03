// WorkSpace.h : main header file for the WORKSPACE DLL
//

#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceApp
// See WorkSpace.cpp for the implementation of this class
//

class CWorkSpaceApp : public CWinApp
{
public:
	CWorkSpaceApp();

//	CString GetIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
//	int  GetIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
//	CString WriteIniString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
//	int  WriteIniInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CWorkSpaceApp)
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
	int			m_nLastTreePaneId;	// ���һ�δ�����Tree Pane��ID
	int			m_nLastTreeType;	// ���һ�δ�����Tree Pane����
	CString		m_strLastBmpFile;	// ���һ�δ�����TreeͼƬ�ļ���
};

extern class CWorkSpaceApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__WORKSPACE_H__)
