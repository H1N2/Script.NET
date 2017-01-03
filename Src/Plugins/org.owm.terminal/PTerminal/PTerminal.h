// PTerminal.h : main header file for the PTERMINAL DLL
//

#ifndef __PTERMINAL_H__
#define __PTERMINAL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPTerminalApp
// See PTerminal.cpp for the implementation of this class
//

class CPTerminalApp : public CWinApp
{
public:
	CPTerminalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTerminalApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPTerminalApp)
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
	CWnd*		m_pTerminalView;	// �������̨���ڵ�ָ��
};

extern class CPTerminalApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__PTERMINAL_H__)
